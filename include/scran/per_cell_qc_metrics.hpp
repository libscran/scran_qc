#ifndef SCRAN_PER_CELL_QC_METRICS_HPP
#define SCRAN_PER_CELL_QC_METRICS_HPP

#include <vector>
#include <algorithm>
#include <limits>
#include <cstdint>

#include "tatami/tatami.hpp"
#include "tatami_stats/tatami_stats.hpp"

/**
 * @file per_cell_qc_metrics.hpp
 * @brief Compute per-cell quality control metrics from a count matrix.
 */

namespace scran {

/**
 * @namespace scran::per_cell_qc_metrics.hpp
 * @brief Compute per-cell quality control metrics from a count matrix.
 *
 * Given a feature-by-cell count matrix, we compute several QC metrics:
 * 
 * - The total sum for each cell, which represents the efficiency of library preparation and sequencing.
 *   Low totals indicate that the library was not successfully captured.
 * - The number of detected features (i.e., with non-zero counts).
 *   This also quantifies the library preparation efficiency, but with a greater focus on capturing the transcriptional complexity.
 * - The maximum count across all features.
 *   This is useful in situations where only one feature is expected to be present, e.g., CRISPR guides, hash tags.
 * - The row index of the feature with the maximum count.
 *   If multiple features are tied for the maximum count, the earliest feature is reported.
 * - The total count in pre-defined feature subsets.
 *   The exact interpretation depends on the nature of the subset -
 *   most commonly, one subset will contain all genes on the mitochondrial chromosome,
 *   where higher proportions of counts in the mitochondrial subset indicate cell damage due to loss of cytoplasmic transcripts.
 *   Spike-in proportions can be interpreted in a similar manner.
 * - The number of detected features in pre-defined feature subsets.
 *   Analogous to the number of detected features for the entire feature space.
 */
namespace per_cell_qc_metrics {

/**
 * @brief Options for `compute()`.
 */
struct Options {
    /**
     * Whether to compute the total count for each cell.
     * This option only affects the `compute()` overload that returns a `Results` object.
     *
     */
    static constexpr bool compute_total = true;

    /**
     * Whether to compute the number of detected features for each cell.
     * This option only affects the `compute()` overload that returns a `Results` object.
     */
    static constexpr bool compute_detected = true;

    /**
     * Whether to compute the maximmum count for each cell.
     * This option only affects the `compute()` overload that returns a `Results` object.
     */
    static constexpr bool compute_max_count = true;

    /**
     * Whether to store the index of the feature with the maximum count for each cell.
     * This option only affects the `compute()` overload that returns a `Results` object.
     */
    static constexpr bool compute_max_index = true;

    /**
     * Whether to compute the total count in each feature subset.
     * This option only affects the `compute()` overload that returns a `Results` object.
     */
    static constexpr bool compute_subset_total = true;

    /**
     * Whether to compute the number of detected features in each feature subset.
     * This option only affects the `compute()` overload that returns a `Results` object.
     */
    static constexpr bool compute_subset_detected = true;

    /**
     * Number of threads to use.
     */
    static constexpr int num_threads = 1;
};

/**
 * @brief Buffers for direct storage of the calculated statistics.
 *
 * @tparam Sum_ Floating point type to store the totals.
 * @tparam Detected_ Integer type to store the number of detected cells.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Integer type of the matrix index.
 */
template<typename Sum_, typename Detected_, typename Value_, typename Index_>
struct Buffers {
    /**
     * @cond
     */
    Buffers() = default;

    Buffers(size_t nsubsets) : subset_total(nsubsets, NULL), subset_detected(nsubsets, NULL) {}
    /**
     * @endcond
     */

    /**
     * Pointer to an array of length equal to the number of cells, equivalent to `Results::total`.
     * Set to `NULL` to skip this calculation.
     */
    Sum_* total = NULL;

    /**
     * Pointer to an array of length equal to the number of cells, equivalent to `Results::detected`.
     * Set to `NULL` to skip this calculation.
     */
    Detected_* detected = NULL;

    /**
     * Pointer to an array of length equal to the number of cells, equivalent to `Results::max_index`.
     * Set to `NULL` to skip this calculation.
     */
    Index_* max_index = NULL;

    /**
     * Pointer to an array of length equal to the number of cells, equivalent to `Results::max_count`.
     * Set to `NULL` to skip this calculation.
     */
    Value_* max_count = NULL;

    /**
     * Vector of pointers of length equal to the number of feature subsets,
     * where each point is to an array of length equal to the number of cells; equivalent to `Results::subset_total`.
     * Set any value to `NULL` to skip the calculation for the corresponding feature subset,
     * or leave empty to skip calculations for all feature subsets.
     */
    std::vector<Sum_*> subset_total;

    /**
     * Vector of pointers of length equal to the number of feature subsets,
     * where each point is to an array of length equal to the number of cells; equivalent to `Results::subset_detected`.
     * Set any value to `NULL` to skip the calculation for the corresponding feature subset,
     * or leave empty to skip calculations for all feature subsets.
     */
    std::vector<Detected_*> subset_detected;
};

/**
 * @cond
 */
namespace internal {

template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute_direct_dense(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, Buffers<Sum_, Detected_, Index_>& output) {
    std::vector<std::vector<Index_> > subset_indices;
    if (!output.subset_total.empty() || !output.subset_detected.empty()) {
        if constexpr(std::is_pointer<Subset_>::value) {
            size_t nsubsets = subsets.size();
            subset_indices.resize(nsubsets);
            auto NR = mat->nrow();

            for (size_t s = 0; s < nsubsets; ++s) {
                auto& current = subset_indices[s];
                const auto& source = subsets[s];
                for (int i = 0; i < NR; ++i) {
                    if (source[i]) {
                        current.push_back(i);
                    }
                }
            }
        }
    }

    tatami::parallelize([&](size_t, Index_ start, Index_ length) {
        auto NR = mat->nrow();
        auto ext = tatami::consecutive_extractor<false>(mat, false, start, length);
        std::vector<Value_> vbuffer(NR);
        bool do_max = output.max_index || output.max_count;

        for (Index_ c = start, end = start + length; c < end; ++c) {
            auto ptr = ext->fetch(c, vbuffer.data());

            if (output.total) {
                output.total[c] = std::accumulate(ptr, ptr + NR, static_cast<Sum_>(0));
            }

            if (output.detected) {
                Detected_ count = 0;
                for (Index_ r = 0; r < NR; ++r) {
                    count += (ptr[r] != 0);
                }
                output.detected[c] = count;
            }

            if (do_max) {
                auto max_count = std::numeric_limits<Value_>::lowest();
                Detected_ max_index = 0;
                for (Index_ r = 0; r < NR; ++r) {
                    if (max_count < ptr[r]) {
                        max_count = ptr[r];
                        max_index = r;
                    }
                }

                if (output.max_index) {
                    output.max_index[c] = max_index;
                }
                if (output.max_count) {
                    output.max_count[c] = max_count;
                }
            }

            size_t nsubsets = subset_indices.size();
            for (size_t s = 0; s < nsubsets; ++s) {
                const auto& sub = [&]() {
                    if constexpr(std::is_pointer<Subset_>::value) {
                        return subset_indices[s];
                    } else {
                        return subsets[s];
                    }
                }();

                if (!output.subset_total.empty() && output.subset_total[s]) {
                    Sum_ current = 0;
                    for (auto r : sub) {
                        current += ptr[r];
                    }
                    output.subset_total[s][c] = current;
                }

                if (!output.subset_detected.empty() && output.subset_detected[s]) {
                    Detected_ current = 0;
                    for (auto r : sub) {
                        current += ptr[r] != 0;
                    }
                    output.subset_detected[s][c] = current;
                }
            }
        }
    }, mat->ncol(), num_threads);
}

template<typename Index_, typename Subset_, typename Sum_, typename Detected_>
std::vector<std::vector<uint8_t> > boolify_subsets(Index_ NR, const std::vector<Subset_>& subsets, Buffers<Sum_, Detected_, Index_>& output) {
    std::vector<std::vector<uint8_t> > is_in_subset;

    if (!output.subset_total.empty() || !output.subset_detected.empty()) {
        if constexpr(!std::is_pointer<Subset_>::value) {
            size_t nsubsets = subsets.size();
            for (size_t s = 0; s < nsubsets; ++s) {
                is_in_subset.emplace_back(NR);
                const auto& source = subsets[s];
                auto& last = is_in_subset.back();
                for (Index_ i = 0; i < NR; ++i) {
                    last[source[i]] = 1;
                }
            }
        }
    }

    return is_in_subset;
}

template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute_direct_sparse(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, Buffers<Sum_, Detected_, Index_>& output) {
    tatami::Options opt;
    opt.sparse_ordered_index = false;
    auto is_in_subset = boolify_subsets(mat->nrow(), subsets, output);

    tatami::parallelize([&](size_t, Index_ start, Index_ length) {
        auto NR = mat->nrow();
        auto ext = tatami::consecutive_extractor<true>(mat, false, start, length, opt);
        std::vector<Value_> vbuffer(NR);
        std::vector<Index_> ibuffer(NR);

        bool do_max = output.max_index || output.max_count;
        std::vector<unsigned char> internal_is_nonzero(output.max_index ? NR : 0);

        for (Index_ c = start, end = start + length; c < end; ++c) {
            auto range = ext->fetch(vbuffer.data(), ibuffer.data());

            if (output.total) {
                output.total[c] = std::accumulate(range.value, range.value + range.number, static_cast<Sum_>(0));
            }

            if (output.detected) {
                Detected_ current = 0;
                for (Index_ i = 0; i < range.number; ++i) {
                    current += (range.value[i] != 0);
                }
                output.detected[c] = current;
            }

            if (do_max) {
                auto max_count = std::numeric_limits<Value_>::lowest();
                Detected_ max_index = 0;
                for (Index_ i = 0; i < range.number; ++i) {
                    if (max_count < range.value[i]) {
                        max_count = range.value[i];
                        max_index = range.index[i];
                    }
                }

                if (max_count <= 0 && range.number < NR) {
                    // Zero is the max.
                    max_count = 0;

                    // Finding the index of the first zero by tracking all
                    // indices with non-zero values. This isn't the fastest
                    // approach but it's simple and avoids assuming that
                    // indices are sorted. Hopefully we don't have to hit
                    // this section often.
                    if (output.max_index) {
                        for (Index_ i = 0; i < range.number; ++i) {
                            if (range.value[i]) {
                                internal_is_nonzero[range.index[i]] = 1;
                            }
                        }
                        for (Index_ r = 0; r < NR; ++r) {
                            if (internal_is_nonzero[r] == 0) {
                                max_index = r;
                                break;
                            }
                        }
                        for (Index_ i = 0; i < range.number; ++i) { // setting back to zero.
                            internal_is_nonzero[range.index[i]] = 0;
                        }
                    }
                }

                if (output.max_index) {
                    output.max_index[c] = max_index;
                }
                if (output.max_count) {
                    output.max_count[c] = max_count;
                }
            }

            size_t nsubsets = subsets.size();
            for (size_t s = 0; s < nsubsets; ++s) {
                const auto& sub = [&]() {
                    if constexpr(std::is_pointer<Subset_>::value) {
                        return subsets[s];
                    } else {
                        return is_in_subset[s];
                    }
                }();

                if (!output.subset_total.empty() && output.subset_total[s]) {
                    Sum_ current = 0;
                    for (Index_ i = 0; i < range.number; ++i) {
                        current += (sub[range.index[i]] != 0) * range.value[i];
                    }
                    output.subset_total[s][c] = current;
                }

                if (!output.subset_detected.empty() && output.subset_detected[s]) {
                    Detected_ current = 0;
                    for (Index_ i = 0; i < range.number; ++i) {
                        current += (sub[range.index[i]] != 0) * (range.value[i] != 0);
                    }
                    output.subset_detected[s][c] = current;
                }
            }
        }
    }, mat->ncol(), num_threads);
}

template<typename Sum_, typename Detected_, typename Value_, typename Index_>
struct RunningBuffers {
    RunningBuffers(Buffers<Sum_, Detected_, Value_, Index_>& output, Sum_* max_count_ptr, size_t thread, Index_ start, Index_ len) {
        if (output.total) {
            total = tatami_stats::LocalOutputBuffer<Sum_>(thread, start, len, output.total);
        }

        if (output.detected) {
            detected = tatami_stats::LocalOutputBuffer<Detected_>(thread, start, len, output.detected);
        }

        if (output.max_count) {
            max_count = tatami_stats::LocalOutputBuffer<Value_>(thread, start, len, max_count_ptr);
        }
        if (output.max_index) {
            max_index = tatami_stats::LocalOutputBuffer<Index_>(thread, start, len, output.max_index);
        }

        subset_total.resize(output.subset_total.size());
        for (size_t s = 0, send = output.subset_total.size(); s < send; ++s) {
            if (output.subset_total[s]) {
                subset_total[s] = tatami_stats::LocalOutputBuffer<Sum_>(thread, start, len, output.subset_total[s]);
            }
        }

        subset_detected.resize(output.subset_detected.size());
        for (size_t s = 0, send = output.subset_detected.size(); s < send; ++s) {
            if (output.subset_detected[s]) {
                subset_detected[s] = tatami_stats::LocalOutputBuffer<Sum_>(thread, start, len, output.subset_detected[s]);
            }
        }
    }

public:
    tatami_stats::LocalOutputBuffer<Sum_> total;
    tatami_stats::LocalOutputBuffer<Detected_> detected;
    tatami_stats::LocalOutputBuffer<Value_> max_count;
    tatami_stats::LocalOutputBuffer<Index_> max_index;
    std::vector<tatami_stats::LocalOutputBuffer<Sum_> > subset_total;
    std::vector<tatami_stats::LocalOutputBuffer<Detected_> > subset_detected;
};

template<typename Sum_, typename Detected_, typename Value_, typename Index_>
void transfer_running_buffers(Buffers<Sum_, Detected_, Value_, Index_>& output, RunningBuffer<Sum_, Detected_, Value_, Index_>& locals) { 
    if (output.total) {
        locals.total.transfer();
    }
    if (output.detected) {
        locals.detected.transfer();
    }

    if (output.max_count) {
        locals.max_count.transfer();
    }
    if (output.max_index) {
        locals.max_index.transfer();
    }

    for (size_t s = 0, send = output.subset_total.size(); s < send; ++s) {
        if (output.subset_total[s]) {
            locals.subset_total[s].transfer();
        }
    }
    for (size_t s = 0, send = output.subset_detected.size(); s < send; ++s) {
        if (output.subset_detected[s]) {
            locals.subset_detected[s].transfer();
        }
    }
}

template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute_running_dense(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, Buffers<Sum_, Detected_, Index_>& output) {
    auto is_in_subset = boolify_subsets(mat->nrow(), subsets, output);

    bool do_max = output.max_index || output.max_count;
    Sum_ max_count_ptr = output.max_count;
    std::vector<Sum_> tmp_max_count;
    if (do_max && !output.max_count) {
        tmp_max_count.resize(mat->nrow());
        max_count_ptr = tmp_max_count.data();
    }

    tatami::parallelize([&](size_t thread, Index_ start, Index_ len) {
        auto NR = mat->nrow();
        auto ext = tatami::consecutive_extractor<false>(mat, true, 0, NR, start, len);
        std::vector<Value_> vbuffer(len);

        RunningBuffers<Sum_, Detected_, Value_, Index_> locals(output, max_count_ptr, thread, start, len);

        for (Index_ r = 0; r < NR; ++r) {
            auto ptr = ext->fetch(r, vbuffer.data());

            if (output.total) {
                auto outt = locals.total.data();
                for (Index_ i = 0; i < len; ++i) {
                    outt[i] += ptr[i];
                }
            }

            if (output.detected) {
                auto outd = locals.detected.data();
                for (Index_ i = 0; i < len; ++i) {
                    outd[i] += (ptr[i] != 0);
                }
            }

            if (do_max) {
                auto outmc = locals.max_count.data();
                if (r == 0) {
                    std::copy_n(ptr, len, outmc);
                    if (output.max_index) {
                        auto outmi = locals.max_index.data();
                        std::fill_n(outmi, len, 0);
                    }
                } else {
                    auto outmi = locals.max_index.data();
                    for (Index_ i = 0; i < len; ++i) {
                        auto& curmax = outmc[i];
                        if (curmax < ptr[i]) {
                            curmax = ptr[i];
                            if (output.max_index) {
                                outmi[i] = r;
                            }
                        }
                    }
                }
            }

            for (size_t s = 0; s < nsubsets; ++s) {
                const auto& sub = [&]() {
                    if constexpr(std::is_pointer<Subset_>::value) {
                        return subsets[s];
                    } else {
                        return is_in_subset[s];
                    }
                }();
                if (sub[r] == 0) {
                    continue;
                }

                if (!output.subset_total.empty() && output.subset_total[s]) {
                    auto current = locals.subset_total[s].data();
                    for (Index_ i = 0; i < len; ++i) {
                        current[i] += ptr[i];
                    }
                }

                if (!output.subset_detected.empty() && output.subset_detected[s]) {
                    auto current = locals.subset_detected[s].data();
                    for (Index_ i = 0; i < len; ++i) {
                        current[i] += (ptr[i] != 0);
                    }
                }
            }
        }

        transfer_running_buffer(output, locals);
    }, mat->ncol(), num_threads);
}

template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute_running_sparse(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, Buffers<Sum_, Detected_, Index_>& output) const {
    tatami::Options opt;
    opt.sparse_ordered_index = false;
    auto is_in_subset = boolify_subsets(mat->nrow(), subsets, output);

    bool do_max = output.max_index || output.max_count;
    Sum_ max_count_ptr = output.max_count;
    std::vector<Sum_> tmp_max_count;
    if (do_max && !output.max_count) {
        tmp_max_count.resize(mat->nrow());
        max_count_ptr = tmp_max_count.data();
    }

    tatami::parallelize([&](size_t t, Index_ start, Index_ len) {
        auto NR = mat->nrow();
        auto ext = tatami::consecutive_extractor<true>(mat, true, 0, NR, start, len, opt);
        std::vector<Value_> vbuffer(len);
        std::vector<Index_> ibuffer(len);

        RunningBuffers<Sum_, Detected_, Value_, Index_> locals(output, max_count_ptr, thread, start, len);
        std::vector<Index_> last_consecutive_nonzero(len);

        for (Index_ r = 0; r < NR; ++r) {
            auto range = ext->fetch(r, vbuffer.data(), ibuffer.data());

            if (output.total) {
                auto outt = locals.total.data();
                for (Index_ i = 0; i < range.number; ++i) {
                    outt[range.index[i]] += range.value[i];
                }
            }

            if (output.detected) {
                auto outd = locals.detected.data();
                for (Index_ i = 0; i < range.number; ++i) {
                    outd[range.index[i]] += (range.value[i] != 0);
                }
            }

            if (do_max) {
                auto outmc = locals.max_count.data();
                auto outmi = locals.max_index.data();

                for (Index_ i = 0; i < range.number; ++i) {
                    auto& curmax = outmc[range.index[i]];
                    if (curmax < range.value[i]) {
                        curmax = range.value[i];
                        if (output.max_index) {
                            outmi[range.index[i]] = r;
                        }
                    }

                    // Getting the index of the last consecutive non-zero entry, so that
                    // we can check if zero is the max and gets its first occurrence, if necessary.
                    auto& last = last_consecutive_nonzero[range.index[i] - start];
                    if (static_cast<Index_>(last) == r) {
                        if (range.value[i] != 0) {
                            ++last;
                        }
                    }
                }
            }

            size_t nsubsets = subsets.size();
            for (size_t s = 0; s < nsubsets; ++s) {
                const auto& sub = [&]() {
                    if constexpr(std::is_pointer<Subset_>::value) {
                        return subsets[s];
                    } else {
                        return is_in_subset[s];
                    }
                }();
                if (sub[r] == 0) {
                    continue;
                }

                if (!output.subset_total.empty() && output.subset_total[s]) {
                    auto current = locals.subset_total[s].data();
                    for (size_t i = 0; i < range.number; ++i) {
                        current[range.index[i]] += range.value[i];
                    }
                }

                if (!output.subset_detected.empty() && output.subset_detected[s]) {
                    auto current = locals.subset_detected[s].data();
                    for (size_t i = 0; i < range.number; ++i) {
                        current[range.index[i]] += (range.value[i] != 0);
                    }
                }
            }
        }

        transfer_running_buffer(output, locals);

        if (do_max) {
            auto NR = mat->nrow();

            // Checking anything with non-positive maximum, and replacing it with zero
            // if there are any zeros (i.e., consecutive non-zeros is not equal to the number of rows).
            for (Index_ c = start, end = start + len; c < end; ++c) {
                auto& current = max_count_ptr[c];
                if (current > 0) {
                    continue;
                }

                auto last_nz = last_consecutive_nonzero[c - start];
                if (last_nz == NR) {
                    continue;
                }

                current = 0;
                if (output.max_index) {
                    output.max_index[c] = last_nz;
                }
            }
        }
    }, NC, num_threads);
}

}
/**
 * @endcond
 */

/**
 * @brief Result store for QC metric calculations.
 * 
 * @tparam Sum_ Floating point type to store the totals.
 * @tparam Detected_ Integer type to store the number of detected cells.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Integer type to store the gene index.
 *
 * Meaningful instances of this object should generally be constructed by calling the `compute()` functions.
 * Empty instances can be default-constructed as placeholders.
 */
template<typename Sum_, typename Detected_, typename Value_, typename Index_>
struct Results {
    /**
     * @cond
     */
    Results() = default;

    Results(size_t nsubsets) : subset_total(nsubsets), subset_detected(nsubsets) {}
    /**
     * @endcond
     */

    /**
     * Total count for each cell.
     * Empty if `Options::compute_total` is false.
     */
    std::vector<Sum_> total;

    /**
     * Number of detected features in each cell.
     * Empty if `Options::compute_detected` is false.
     */
    std::vector<Detected_> detected;

    /**
     * Row index of the most-expressed feature in each cell.
     * On ties, the first feature is arbitrarily chosen.
     * Empty if `Options::compute_max_index` is false.
     */
    std::vector<Index_> max_index;

    /**
     * Maximum count value in each cell.
     * Empty if `Options::compute_max_count` is false.
     */
    std::vector<Value_> max_count;

    /**
     * Total count of each feature subset in each cell.
     * Each inner vector corresponds to a feature subset and is of length equal to the number of cells.
     * Empty if there are no feature subsets or if `Options::compute_subset_total` is false.
     */
    std::vector<std::vector<Sum_> > subset_total;

    /**
     * Number of detected features in each feature subset in each cell.
     * Each inner vector corresponds to a feature subset and is of length equal to the number of cells.
     * Empty if there are no feature subsets or if `Options::compute_subset_detected` is false.
     */
    std::vector<std::vector<Detected_> > subset_detected;
};

/**
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 * @tparam Sum_ Floating point type to store the totals.
 * @tparam Detected_ Integer type to store the number of detected cells.
 *
 * @param mat Pointer to a feature-by-cells `tatami::Matrix` of counts.
 * @param[in] subsets Vector of feature subsets, where each entry represents a feature subset and may be either:
 * - A pointer to an array of length equal to `mat->nrow()` where each entry is interpretable as a boolean.
 *   This indicates whether each row in `mat` belongs to the subset.
 * - A `std::vector` containing sorted and unique row indices.
 *   This specifies the rows in `mat` that belong to the subset.
 * @param[out] output A `Buffers` object in which the computed statistics are to be stored.
 */
template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset>& subsets, Buffers<Sum_, Detected_, Value_, Index_>& output) {
    if (mat->sparse()) {
        if (mat->prefer_rows()) {
            internal::compute_running_sparse(mat, subsets, output);
        } else {
            internal::compute_direct_sparse(mat, subsets, output);
        }
    } else {
        if (mat->prefer_rows()) {
            internal::compute_running_dense(mat, subsets, output);
        } else {
            internal::compute_direct_dense(mat, subsets, output);
        }
    }
}

/**
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 * @tparam Sum_ Floating point type to store the totals.
 * @tparam Detected_ Integer type to store the number of detected cells.
 *
 * @param mat Pointer to a feature-by-cells `tatami::Matrix` of counts.
 * @param[in] subsets Vector of feature subsets, where each entry represents a feature subset and may be either:
 * - A pointer to an array of length equal to `mat->nrow()` where each entry is interpretable as a boolean.
 *   This indicates whether each row in `mat` belongs to the subset.
 * - A `std::vector` containing sorted and unique row indices.
 *   This specifies the rows in `mat` that belong to the subset.
 *
 * @return A `Results` object containing the QC metrics.
 */
template<typename Sum_ = double, typename Detected_ = int, typename Value_, typename Index_, typename Subset_>
Results<Sum_, Detected_, Value_, Index_> compute(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets) {
    Results<Sum_, Detected_, Value_, Index_> output;
    Buffers<Sum_, Detected_, Value_, Index_> buffers;
    auto ncells = mat->ncol();

    if (compute_total) {
        output.total.resize(ncells);
        buffers.total = output.total.data();
    }
    if (compute_detected) {
        output.detected.resize(ncells);
        buffers.detected = output.detected.data();
    }
    if (compute_max_index) {
        output.max_index.resize(ncells);
        buffers.max_index = output.max_index.data();
    }
    if (compute_max_count) {
        output.max_count.resize(ncells, pick_fill_value<double>());
        buffers.max_count = output.max_count.data();
    }

    size_t nsubsets = subsets.size();

    if (compute_subset_total) {
        output.subset_total.resize(nsubsets);
        buffers.subset_total.resize(nsubsets);
        for (size_t s = 0; s < nsubsets; ++s) {
            output.subset_total[s].resize(ncells);
            buffers.subset_total[s] = output.subset_total[s].data();
        }
    }

    if (compute_subset_detected) {
        output.subset_detected.resize(nsubsets);
        buffers.subset_detected.resize(nsubsets);
        for (size_t s = 0; s < nsubsets; ++s) {
            output.subset_detected[s].resize(ncells);
            buffers.subset_detected[s] = output.subset_detected[s].data();
        }
    }

    buffers.already_zeroed = true;
    compute(mat, subsets, buffers);
    return output;
}

}

}

#endif
