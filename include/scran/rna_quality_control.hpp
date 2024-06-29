#ifndef SCRAN_PER_CELL_RNA_QC_METRICS_HPP
#define SCRAN_PER_CELL_RNA_QC_METRICS_HPP

#include "find_median_mad.hpp"

#include <vector>
#include <limits>

#include "tatami/base/Matrix.hpp"
#include "PerCellQcMetrics.hpp"
#include "utils.hpp"

/**
 * @file rna_quality_control.hpp
 *
 * @brief Simple per-cell QC metrics from an RNA count matrix.
 */

namespace scran {

/**
 * @namespace scran::rna_quality_control
 * @brief Simple per-cell QC metrics from an RNA count matrix.
 *
 * Given a feature-by-cell RNA count matrix, we compute several metrics for filtering high-quality cells:
 * 
 * - The total sum of counts for each cell, which represents the efficiency of library preparation and sequencing.
 *   Low totals indicate that the library was not successfully captured.
 * - The number of detected features.
 *   This also quantifies the library preparation efficiency, but with a greater focus on capturing the transcriptional complexity.
 * - The proportion of counts in pre-defined feature subsets.
 *   The exact interpretation depends on the nature of the subset -
 *   most commonly, one subset will contain all genes on the mitochondrial chromosome,
 *   where higher proportions indicate cell damage due to loss of cytoplasmic transcripts.
 *   Spike-in proportions can be interpreted in a similar manner.
 *
 * Outliers are defined on each metric by counting the number of MADs from the median value across all cells, see `choose_filter_thresholds::compute()` for details.
 * For the total counts and number of detected features, the outliers are defined after log-transformation of the metrics.
 * This improves resolution at low values and ensures that the defined threshold is not negative.
 * Note that all thresholds are still reported on the original scale, so no further exponentiation is required.
 */
namespace rna_quality_control {

/**
 * @brief Options for `compute_metrics()`.
 */
struct MetricsOptions {
    /**
     * Whether to compute the sum of expression values for each cell.
     * This option only affects the `compute_metrics()` overload that returns a `MetricsResults` object.
     */
    bool compute_sum = true;

    /**
     * Whether to compute the number of detected features for each cell.
     * This option only affects the `compute_metrics()` overload that returns a `MetricsResults` object.
     */
    bool compute_detected = true;

    /**
     * Whether to compute the proportion of expression in each feature subset.
     * This option only affects the `compute_metrics()` overload that returns a `MetricsResults` object.
     */
    bool compute_subset_proportion = true;

    /**
     * Number of threads to use.
     */
    int num_threads = 1;
};

/**
 * @brief Buffers for direct storage of the calculated statistics.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 */
template<typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double>
struct MetricsBuffers {
    /**
     * Pointer to an array of length equal to the number of cells, see `MetricsResults::sum`.
     * This can be set to NULL to omit the calculation of the sums.
     */
    Sum_* sum = NULL;

    /**
     * Pointer to an array of length equal to the number of cells, see `MetricsResults::detected`.
     * This can be set to NULL to omit the calculation of the number of detected cells.
     */
    Detected_* detected = NULL;

    /**
     * Vector of pointers of length equal to the number of feature subsets.
     * Each entry should point to an array of length equal to the number of cells, see `MetricsResults::subset_proportion`.
     * This can be left empty to omit calculation of all subset proportions,
     * or individual pointers may be set to NULL to omit calculation of the corresponding subset.
     */
    std::vector<Proportion_*> subset_proportion;
};

/**
 * Compute the QC metrics from an input matrix.
 * This is a wrapper around `per_cell_qc_metrics::compute()` with some pre-configuration for RNA-relevant metrics.
 *
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 *
 * @param mat Pointer to a feature-by-cells matrix containing counts.
 * @param[in] subsets Vector of feature subsets, see `per_cell_qc_metrics::compute()` for details.
 * @param[out] output `MetricsBuffers` object in which to store the output.
 * @param options Further options.
 */
template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_, typename Proportion_>
void compute_metrics(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, MetricsBuffers<Sum_, Detected_, Proportion_>& output, const MetricsOptions& options) {
    auto NC = mat->ncol();
    size_t nsubsets = subsets.size();

    per_cell_qc_metrics::Buffers<Sum_, Detected_> tmp;
    tmp.sum = output.sum;
    tmp.detected = output.detected;

    std::vector<Sum_> placeholder_sum;
    constexpr bool same_type = std::is_same<Sum_, Proportion_>::value;
    typename std::conditional<same_type, bool, std::vector<std::vector<Sum_> > >::type placeholder_subset;

    if (output.subset_proportion.size()) {
        // Make sure that sums are computed for the proportion calculations.
        if (!tmp.total) {
            placeholder_sum.resize(NC);
            tmp.total = placeholder.data();            
        }

        // Providing space for the subset sums if they're not the same type.
        if constexpr(same_type) {
            tmp.subset_sum = output.subset_proportion;
        } else {
            placeholder_subset.resize(nsubsets);
            tmp.subset_sum.resize(nsubsets);
            for (size_t s = 0; s < nsubsets; ++s) {
                if (output.subset_proportion[s]) {
                    auto& b = placeholder_subset[s];
                    b.resize(NC);
                    tmp.subset_sum[s] = b.data();
                }
            }
        }
    }

    per_cell_qc_metrics::Options opt;
    opt.num_threads = options.num_threads;
    per_cell_qc_metrics::compute(mat, subsets, tmp, opt);

    for (size_t s = 0 ; s < nsubsets; ++s) {
        auto dest = output.subset_proportion[s];
        if (dest) {
            auto src = tmp.subset_sum[s];
            for (Index_ c = 0; c < NC; ++c) {
                dest[c] = static_cast<Proportion_>(src[c]) / static_cast<Proportion_>(tmp.total[c]);
            }
        }
    }

    return;
}

/**
 * @brief Results of the QC metric calculations.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 *
 * Meaningful instances of this object should generally be constructed by calling the `compute_metrics()` function.
 */
template<typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double>
struct MetricsResults {
    /**
     * @cond
     */
    Results() = default;
    /**
     * @endcond
     */

    /**
     * Sum of counts for each cell.
     */
    std::vector<Sum_> sum;

    /**
     * Number of detected features in each cell.
     */
    std::vector<Detected_> detected;

    /**
     * Proportion of counts in each feature subset in each cell.
     * Each inner vector corresponds to a feature subset and is of length equal to the number of cells.
     */
    std::vector<std::vector<Proportion_> > subset_proportion;
};

/**
 * Overload of `compute_metrics()` that allocates memory for the results.
 *
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 *
 * @param mat Pointer to a feature-by-cells **tatami** matrix containing counts.
 * @param[in] subsets Vector of feature subsets, see `per_cell_qc_metrics::compute()` for details.
 * @param options Further options.
 *
 * @return A `PerCellRnaQcMetrics::Results` object containing the QC metrics.
 * Subset proportions are returned depending on the `subsets`.
 */
template<typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double, typename Value_ = double, typename Index_ = int, typename Subset = const uint8_t*>
MetricsResults<Sum_, Detected_, Proportion_> compute_metrics(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, const MetricsOptions& options) {
    auto NC = mat->ncol();
    MetricsBuffers<Sum_, Detected_, Proportion_> x;
    MetricsResults<Sum_, Detected_, Proportion_> output;

    if (options.compute_sum) {
        output.sum.resize(NC);
        x.sum = output.sum.data();
    }

    if (options.compute_detected) {
        output.detected.resize(NC);
        x.detected = output.detected.data();
    }

    if (options.compute_subset_proportion) {
        size_t nsubsets = subsets.size();
        x.subset_proportion.resize(nsubsets);
        for (size_t s = 0; s < nsubsets; ++s) {
            output.subset_proportion[s].resize(NC);
            x.subset_proportion[s] = output.subset_proportion[s].data();
        }
    }

    compute_metrics(mat, subsets, x, options);
    return output;
}

/**
 * @brief Options for `compute_filters()`.
 */
struct FiltersOptions {
    /**
     * Number of MADs below the median, to define the threshold for outliers in the number of detected features.
     * This should be non-negative.
     */
    double detected_num_mads = 3;

    /**
     * Number of MADs below the median, to define the threshold for outliers in the total count per cell.
     * This should be non-negative.
     */
    double sum_num_mads = 3;

    /**
     * Number of MADs above the median, to define the threshold for outliers in the subset proportions.
     * This should be non-negative.
     */
    double subset_proportion_num_mads = 3;
};

/**
 * @cond
 */
namespace internal {

template<class Host_, typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename BlockSource_>
void populate(Host_& host, Index_ n, MetricsBuffers<Sum_, Detected_, Proportion_>& res, BlockSource_ block, const Options& options) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    auto buffer = [&]() {
        if constexpr(unblocked) {
            return std::vector<Float_>(n);
        } else {
            return find_median_mad::Workspace<Float_, Index_>(n, block);
        }
    }();

    if (res.sum) {
        choose_filter_thresholds::Options opts;
        opts.num_mads = sum_num_mads;
        opts.log = true;
        opts.upper = false;
        output.my_sum = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, res.sum, buffer.data(), opts).lower;
            } else {
                return choose_filter_thresholds::internal::strip<true>(choose_filter_thresholds::compute_blocked(n, res.sum, block, &buffer, opts))
            }
        }();
    }

    if (results.detected) {
        choose_filter_thresholds::Options opts;
        opts.num_mads = detected_num_mads;
        opts.log = true;
        opts.upper = false;
        output.my_detected = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, res.detected, buffer.data(), opts).lower;
            } else {
                return choose_filter_thresholds::internal::strip<true>(choose_filter_thresholds::compute_blocked(n, res.detected, block, &buffer, opts))
            }
        }();
    }

    size_t nsubsets = buffer.subset_proportion.size();
    output.my_subset_proportion.resize(nsubset);
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = buffer.subset_proportion[s];
        if (sub) {
            choose_filter_thresholds::Options opts;
            opts.num_mads = subset_proportion_num_mads;
            opts.lower = false;
            output.my_subset_proportion[s] = [&]() {
                if constexpr(unblocked) {
                    return choose_filter_thresholds::compute(n, sub, buffer.data(), mopt).upper;
                } else {
                    return choose_filter_thresholds::internal::strip<false>(choose_filter_thresholds::compute_blocked(n, sub, block, &buffer, opts));
                }
            }();
        }
    }
}

template<class Host_, typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename BlockSource_, typename Output_>
void filter(Host_& host, Index_ n, MetricsBuffers<Sum_, Detected_, Proportion_>& res, BlockSource_ block, Output_* output) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    std::fill_n(output, num, 1);

    if (metrics.sum) {
        for (Index_ i = 0; i < n; ++i) {
            auto thresh = [&]() {
                if constexpr(unblocked) {
                    return my_sum;
                } else {
                    return my_sum[block[i]];
                }
            }();
            output[i] = output[i] && (metrics.sum[i] >= thresh);
        }
    }

    if (metrics.detected) {
        for (Index_ i = 0; i < n; ++i) {
            auto thresh = [&]() {
                if constexpr(unblocked) {
                    return my_detected;
                } else {
                    return my_detected[block[i]];
                }
            }();
            output[i] = output[i] && (metrics.detected[i] >= thresh);
        }
    }

    size_t nsubsets = metrics.subset_proportions.size();
    if (nsubsets) {
        for (size_t s = 0; s < nsubsets; ++s) {
            auto sub = metrics.subset_proportions[s];
            if (sub) {
                const auto& sthresh = my_subset_proportions[s];
                for (Index_ i = 0; i < n; ++i) {
                    auto thresh = [&]() {
                        if constexpr(unblocked) {
                            return sthresh;
                        } else {
                            return sthresh[block[i]];
                        }
                    }();
                    output[i] = output[i] && (metrics.subset_proportions[i] <= thresh);
                }
            }
        }
    }
}

template<typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename BlockSource_, typename Output_>
MetricsBuffer<const Sum_, const Detected_, const Proportion_> to_buffer(const MetricsResults<Sum_, Detected_, Proportion_>& metrics) {
    MetricsBuffer<const Sum_, const Detected_, const Proportion_> buffer;
    buffer.sum = (metrics.sum.empty() ? NULL : metrics.sum.data());
    buffer.detected = (metrics.detected.empty() ? NULL : metrics.detected.data());
    buffer.subset_proportion.reserve(metrics.subset_proportion.size());
    for (const auto& s : metrics.subset_proportion) {
        buffer.subset_proportion.push_back(s.empty() ? NULL : s.data());
    }
    return buffer;
}

}
/**
 * @endcond
 */

/**
 * @brief Thresholds to define outliers on each metric.
 */
template<typename Float_ = double>
class FilterThresholds {
public:
    /**
     * Default constructor.
     */
    FilterThresholds() = default;

    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_>
    FilterThresholds(Index_ n, const MetricsBuffer<Sum_, Detected_, Proportion_>& results, const FiltersOptions& options) {
        internal::populate(*this, n, results, false, options);
    }

    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_>
    FilterThresholds(Index_ n, const MetricsResults<Sum_, Detected_, Proportion_>& results, const FiltersOptions& options) :
        FilterThresholds(n, internal::to_buffer(results), options) {}

public:
    /**
     * @return Lower threshold to apply to the sums.
     */
    Float_ get_sum() const {
        return my_sum;
    }

    /**
     * @return Thresholds to apply to the number of detected genes.
     */
    Float_ get_detected() const {
        return my_detected;
    }

    /**
     * @return Thresholds to apply to the subset proportions.
     */
    const std::vector<Float_>& get_subset_proportion() const {
        return my_subset_proportion;
    }

private:
    Float_ my_sum = 0;
    Float_ my_detected = 0;
    std::vector<Float_> my_subset_proportion;

public:
    /**
     * @tparam Index_ Integer type for the array indices.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type for the outlier calls.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     * @param[in] block Pointer to an array of length `n`, containing the block of origin for each cell.
     * Each entry should be less than the `mm.size()` in the constructor.
     * @param[out] output Pointer to an array of length `n`, to store the high-quality calls (see the other `filter()` overload).
     * @param overwrite Whether to overwrite existing false-y entries in `output`.
     */
    template<typename Index_, typename Value_, typename Block_, typename Output_>
    void filter(Index_ num, const MetricsBuffer<Sum_, Detected_, Proportion_>& metrics, Output_* output) const {
        internal::filter(num, metrics, false, output);
    }

    /**
     * @tparam Output_ Boolean type for the outlier calls.
     * @tparam Index_ Integer type for the array indices.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     * @param[in] block Pointer to an array of length `n`, containing the block of origin for each cell.
     *
     * @return Vector of length `n`, specifying whether a cell is of high quality.
     */
    template<typename Index_, typename Value_, typename Sum_, typename Detected_, typename Proportion_, typename Output_>
    void filter(Index_ num, const MetricsResults<Sum_, Detected_, Proportion_>& metrics, Output_* output) const {
        return filter(n, internal::to_buffer(metrics), output);
    }

    template<typename Output_, typename Index_, typename Value_, typename Sum_, typename Detected_, typename Proportion_>
    std::vector<Output_> filter(Index_ num, const MetricsResults<Sum_, Detected_, Proportion_>& metrics) const {
        std::vector<Output_> output(num);
        filter(num, metrics, output.data());
        return output;
    }
};

/**
 * @brief Thresholds to define outliers on each metric.
 */
template<typename Float_ = double>
class BlockedFilterThresholds {
public:
    /**
     * Default constructor.
     */
    BlockedFilterThresholds() = default;

    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename Block_>
    BlockedFilterThresholds(Index_ num, const MetricsBuffer<Sum_, Detected_, Proportion_>& results, const Block_* block, const FiltersOptions& options) {
        internal::populate(*this, num, results, block, options);
    }

    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename Block_>
    BlockedFilterThresholds(Index_ num, const MetricsResults<Sum_, Detected_, Proportion_>& results, const Block_* block, const FiltersOptions& options) :
        FilterThresholds(num, internal::to_buffer(results), block, options) {}

public:
    /**
     * @return Lower threshold to apply to the sums.
     */
    const std::vector<Float_> get_sum() const {
        return my_sum;
    }

    /**
     * @return Thresholds to apply to the number of detected genes.
     */
    const std::vector<Float_> get_detected() const {
        return my_detected;
    }

    /**
     * @return Thresholds to apply to the subset proportions.
     */
    const std::vector<std::vector<Float_> >& get_subset_proportion() const {
        return my_subset_proportion;
    }

private:
    std::vector<Float_> my_sum = 0;
    std::vector<Float_> my_detected = 0;
    std::vector<std::vector<Float_> > my_subset_proportion;

public:
    /**
     * @tparam Index_ Integer type for the array indices.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type for the outlier calls.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     * @param[in] block Pointer to an array of length `n`, containing the block of origin for each cell.
     * Each entry should be less than the `mm.size()` in the constructor.
     * @param[out] output Pointer to an array of length `n`, to store the high-quality calls (see the other `filter()` overload).
     * @param overwrite Whether to overwrite existing false-y entries in `output`.
     */
    template<typename Index_, typename Value_, typename Sum_, typename Detected_, typename Proportion_, typename Block_, typename Output_>
    void filter(Index_ num, const MetricsBuffer<Sum_, Detected_, Proportion_>& metrics, const Block_* block, Output_* output) const {
        internal::filter(num, metrics, block, output);
    }

    /**
     * @tparam Output_ Boolean type for the outlier calls.
     * @tparam Index_ Integer type for the array indices.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     * @param[in] block Pointer to an array of length `n`, containing the block of origin for each cell.
     *
     * @return Vector of length `n`, specifying whether a cell is of high quality.
     */
    template<typename Index_, typename Value_, typename Sum_, typename Detected_, typename Proportion_, typename Block_, typename Output_>
    void filter(Index_ num, const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const Block_* block, Output_* output) const {
        return filter(num, internal::to_buffer(metrics), block, output);
    }

    template<typename Index_, typename Value_, typename Sum_, typename Detected_, typename Proportion_, typename Block_, typename Output_>
    std::vector<Output_> filter(Index_ num, const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const Block_* block) const {
        std::vector<Output_> output(num);
        filter(num, metrics, block, output.data());
        return output;
    }
};

}

}

#endif
