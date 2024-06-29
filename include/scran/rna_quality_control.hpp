#ifndef SCRAN_RNA_QUALITY_CONTROL_HPP
#define SCRAN_RNA_QUALITY_CONTROL_HPP

#include <vector>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "tatami/tatami.hpp"

#include "find_median_mad.hpp"
#include "per_cell_qc_metrics.hpp"
#include "choose_filter_thresholds.hpp"

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
     * This should not be NULL when calling `compute_metrics()`.
     */
    Sum_* sum = NULL;

    /**
     * Pointer to an array of length equal to the number of cells, see `MetricsResults::detected`.
     * This should not be NULL when calling `compute_metrics()`.
     */
    Detected_* detected = NULL;

    /**
     * Vector of pointers of length equal to the number of feature subsets.
     * Each entry should point to an array of length equal to the number of cells, see `MetricsResults::subset_proportion`.
     * This should have length equal to the `subsets` used in `compute_metrics()` and all pointers should be non-NULL.
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

    per_cell_qc_metrics::Buffers<Sum_, Detected_, Value_, Index_> tmp;
    tmp.sum = output.sum;
    tmp.detected = output.detected;

    constexpr bool same_type = std::is_same<Sum_, Proportion_>::value;
    typename std::conditional<same_type, bool, std::vector<std::vector<Sum_> > >::type placeholder_subset;

    if (output.subset_proportion.size()) {
        // Providing space for the subset sums if they're not the same type.
        if constexpr(same_type) {
            tmp.subset_sum = output.subset_proportion;
        } else {
            placeholder_subset.resize(nsubsets);
            tmp.subset_sum.resize(nsubsets);
            for (size_t s = 0; s < nsubsets; ++s) {
                auto& b = placeholder_subset[s];
                b.resize(NC);
                tmp.subset_sum[s] = b.data();
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
                dest[c] = static_cast<Proportion_>(src[c]) / static_cast<Proportion_>(tmp.sum[c]);
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
    MetricsResults() = default;
    /**
     * @endcond
     */

    /**
     * Vector of length equal to the number of cells in the dataset, containing the sum of counts for each cell.
     */
    std::vector<Sum_> sum;

    /**
     * Vector of length equal to the number of cells in the dataset, containing the number of detected features in each cell.
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
template<typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double, typename Value_ = double, typename Index_ = int, typename Subset_ = const uint8_t*>
MetricsResults<Sum_, Detected_, Proportion_> compute_metrics(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, const MetricsOptions& options) {
    auto NC = mat->ncol();
    MetricsBuffers<Sum_, Detected_, Proportion_> x;
    MetricsResults<Sum_, Detected_, Proportion_> output;

    output.sum.resize(NC);
    x.sum = output.sum.data();

    output.detected.resize(NC);
    x.detected = output.detected.data();

    size_t nsubsets = subsets.size();
    x.subset_proportion.resize(nsubsets);
    output.subset_proportion.resize(nsubsets);
    for (size_t s = 0; s < nsubsets; ++s) {
        output.subset_proportion[s].resize(NC);
        x.subset_proportion[s] = output.subset_proportion[s].data();
    }

    compute_metrics(mat, subsets, x, options);
    return output;
}

/**
 * @brief Options for `Filters()`.
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

template<typename Float_, class Host_, typename Sum_, typename Detected_, typename Proportion_, typename BlockSource_>
void populate(Host_& host, size_t n, const MetricsBuffers<Sum_, Detected_, Proportion_>& res, BlockSource_ block, const FiltersOptions& options) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    auto buffer = [&]() {
        if constexpr(unblocked) {
            return std::vector<Float_>(n);
        } else {
            return find_median_mad::Workspace<Float_, size_t>(n, block);
        }
    }();

    {
        choose_filter_thresholds::Options opts;
        opts.num_mads = options.sum_num_mads;
        opts.log = true;
        opts.upper = false;
        host.get_sum() = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, res.sum, buffer.data(), opts).lower;
            } else {
                return choose_filter_thresholds::internal::strip<true>(choose_filter_thresholds::compute_blocked(n, res.sum, block, &buffer, opts));
            }
        }();
    }

    {
        choose_filter_thresholds::Options opts;
        opts.num_mads = options.detected_num_mads;
        opts.log = true;
        opts.upper = false;
        host.get_detected() = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, res.detected, buffer.data(), opts).lower;
            } else {
                return choose_filter_thresholds::internal::strip<true>(choose_filter_thresholds::compute_blocked(n, res.detected, block, &buffer, opts));
            }
        }();
    }

    size_t nsubsets = res.subset_proportion.size();
    host.get_subset_proportion().resize(nsubsets);
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = res.subset_proportion[s];
        choose_filter_thresholds::Options opts;
        opts.num_mads = options.subset_proportion_num_mads;
        opts.lower = false;
        host.get_subset_proportion()[s] = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, sub, buffer.data(), opts).upper;
            } else {
                return choose_filter_thresholds::internal::strip<false>(choose_filter_thresholds::compute_blocked(n, sub, block, &buffer, opts));
            }
        }();
    }
}

template<class Host_, typename Sum_, typename Detected_, typename Proportion_, typename BlockSource_, typename Output_>
void filter(const Host_& host, size_t n, const MetricsBuffers<Sum_, Detected_, Proportion_>& metrics, BlockSource_ block, Output_* output) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    std::fill_n(output, n, 1);

    for (size_t i = 0; i < n; ++i) {
        auto thresh = [&]() {
            if constexpr(unblocked) {
                return host.get_sum();
            } else {
                return host.get_sum()[block[i]];
            }
        }();
        output[i] = output[i] && (metrics.sum[i] >= thresh);
    }

    for (size_t i = 0; i < n; ++i) {
        auto thresh = [&]() {
            if constexpr(unblocked) {
                return host.get_detected();
            } else {
                return host.get_detected()[block[i]];
            }
        }();
        output[i] = output[i] && (metrics.detected[i] >= thresh);
    }

    size_t nsubsets = metrics.subset_proportion.size();
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = metrics.subset_proportion[s];
        const auto& sthresh = host.get_subset_proportion()[s];
        for (size_t i = 0; i < n; ++i) {
            auto thresh = [&]() {
                if constexpr(unblocked) {
                    return sthresh;
                } else {
                    return sthresh[block[i]];
                }
            }();
            output[i] = output[i] && (sub[i] <= thresh);
        }
    }
}

template<typename Sum_, typename Detected_, typename Proportion_>
MetricsBuffers<const Sum_, const Detected_, const Proportion_> to_buffer(const MetricsResults<Sum_, Detected_, Proportion_>& metrics) {
    MetricsBuffers<const Sum_, const Detected_, const Proportion_> buffer;
    buffer.sum = metrics.sum.data();
    buffer.detected = metrics.detected.data();
    buffer.subset_proportion.reserve(metrics.subset_proportion.size());
    for (const auto& s : metrics.subset_proportion) {
        buffer.subset_proportion.push_back(s.data());
    }
    return buffer;
}

}
/**
 * @endcond
 */

/**
 * @brief Filter for high-quality cells using RNA-based metrics. 
 * @tparam Float_ Floating-point type for filter thresholds.
 */
template<typename Float_ = double>
class Filters {
public:
    /**
     * Default constructor.
     */
    Filters() = default;

public:
    /**
     * @return Lower threshold to apply to the sums.
     */
    Float_ get_sum() const {
        return my_sum;
    }

    /**
     * @return Lower threshold to apply to the number of detected genes.
     */
    Float_ get_detected() const {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to each subset proportion.
     */
    const std::vector<Float_>& get_subset_proportion() const {
        return my_subset_proportion;
    }

    /**
     * @return Lower threshold to apply to the sums.
     */
    Float_& get_sum() {
        return my_sum;
    }

    /**
     * @return Lower threshold to apply to the number of detected genes.
     */
    Float_& get_detected() {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to each subset proportion.
     */
    std::vector<Float_>& get_subset_proportion() {
        return my_subset_proportion;
    }

private:
    Float_ my_sum = 0;
    Float_ my_detected = 0;
    std::vector<Float_> my_subset_proportion;

public:
    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @tparam Output_ Boolean type to store the high quality flags.
     * @param num Number of cells.
     * @param metrics A collection of arrays containing RNA-based QC metrics, filled by `compute_metrics()`.
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Proportion_, typename Output_>
    void filter(size_t num, const MetricsBuffers<Sum_, Detected_, Proportion_>& metrics, Output_* output) const {
        internal::filter(*this, num, metrics, false, output);
    }

    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @tparam Output_ Boolean type to store the high quality flags.
     * @param metrics RNA-based QC metrics returned by `compute_metrics()`.
     * @param[out] output Pointer to an array of length `num`. 
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Proportion_, typename Output_>
    void filter(const MetricsResults<Sum_, Detected_, Proportion_>& metrics, Output_* output) const {
        return filter(metrics.sum.size(), internal::to_buffer(metrics), output);
    }

    /**
     * @tparam Output_ Boolean type to store the high quality flags.
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @param metrics RNA-based QC metrics returned by `compute_metrics()`.
     * @return Vector of length `num`, containing the high-quality calls.
     */
    template<typename Output_ = uint8_t, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double>
    std::vector<Output_> filter(const MetricsResults<Sum_, Detected_, Proportion_>& metrics) const {
        std::vector<Output_> output(metrics.sum.size());
        filter(metrics, output.data());
        return output;
    }
};

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 * @param num Number of cells.
 * @param metrics A collection of arrays containing RNA-based QC metrics, filled by `compute_metrics()`.
 * @param options Further options for filtering.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double>
Filters<Float_> compute_filters(size_t num, const MetricsBuffers<Sum_, Detected_, Proportion_>& metrics, const FiltersOptions& options) {
    Filters<Float_> output;
    internal::populate<Float_>(output, num, metrics, false, options);
    return output;
}

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 * @param metrics RNA-based QC metrics from `compute_metrics()`.
 * @param options Further options for filtering.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double>
Filters<Float_> compute_filters(const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const FiltersOptions& options) {
    return compute_filters(metrics.sum.size(), internal::to_buffer(metrics), options);
}

/**
 * @brief Filter for high-quality cells using RNA-based metrics with blocking.
 * @tparam Float_ Floating-point type for filter thresholds.
 */
template<typename Float_ = double>
class BlockedFilters {
public:
    /**
     * Default constructor.
     */
    BlockedFilters() = default;

public:
    /**
     * @return Vector of length equal to the number of blocks,
     * containing the lower threshold on the sums in each block.
     */
    const std::vector<Float_>& get_sum() const {
        return my_sum;
    }

    /**
     * @return Vector of length equal to the number of blocks,
     * containing the lower threshold on the number of detected genes in each block.
     */
    const std::vector<Float_>& get_detected() const {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of blocks.
     * Each entry is a vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to the each subset proportion.
     */
    const std::vector<std::vector<Float_> >& get_subset_proportion() const {
        return my_subset_proportion;
    }

    /**
     * @return Vector of length equal to the number of blocks,
     * containing the lower threshold on the sums in each block.
     */
    std::vector<Float_>& get_sum() {
        return my_sum;
    }

    /**
     * @return Vector of length equal to the number of blocks,
     * containing the lower threshold on the number of detected genes in each block.
     */
    std::vector<Float_>& get_detected() {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets.
     * Each entry is a vector of length equal to the number of blocks,
     * containing the upper threshold to apply to the subset proportion for that block.
     */
    std::vector<std::vector<Float_> >& get_subset_proportion() {
        return my_subset_proportion;
    }

private:
    std::vector<Float_> my_sum;
    std::vector<Float_> my_detected;
    std::vector<std::vector<Float_> > my_subset_proportion;

public:
    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type to store the high quality flags.
     * @param num Number of cells.
     * @param metrics A collection of arrays containing RNA-based QC metrics, filled by `compute_metrics()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_, typename Block_, typename Output_>
    void filter(Index_ num, const MetricsBuffers<Sum_, Detected_, Proportion_>& metrics, const Block_* block, Output_* output) const {
        internal::filter(*this, num, metrics, block, output);
    }

    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type to store the high quality flags.
     * @param metrics RNA-based QC metrics computed by `compute_metrics()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Proportion_, typename Block_, typename Output_>
    void filter(const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const Block_* block, Output_* output) const {
        return filter(metrics.sum.size(), internal::to_buffer(metrics), block, output);
    }

    /**
     * @tparam Output_ Boolean type to store the high quality flags.
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Proportion_ Floating-point type to store the proportions.
     * @tparam Block_ Integer type for the block assignment.
     * @param metrics RNA-based QC metrics computed by `compute_metrics()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     * @return Vector of length `num`, containing the high-quality calls.
     */
    template<typename Output_ = uint8_t, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double, typename Block_ = int>
    std::vector<Output_> filter(const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const Block_* block) const {
        std::vector<Output_> output(metrics.sum.size());
        filter(metrics, block, output.data());
        return output;
    }
};

/**
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 * @tparam Block_ Integer type for the block assignments.
 * @param num Number of cells.
 * @param metrics A collection of arrays containing RNA-based QC metrics, filled by `compute_metrics()`.
 * @param[in] block Pointer to an array of length `num` containing block identifiers.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param options Further options for filtering.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double, typename Block_ = int>
BlockedFilters<Float_> compute_filters_blocked(size_t num, const MetricsBuffers<Sum_, Detected_, Proportion_>& metrics, const Block_* block, const FiltersOptions& options) {
    BlockedFilters<Float_> output;
    internal::populate<Float_>(output, num, metrics, block, options);
    return output;
}

/**
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Proportion_ Floating-point type to store the proportions.
 * @tparam Block_ Integer type for the block assignments.
 * @param metrics RNA-based QC metrics computed by `compute_metrics()`.
 * @param[in] block Pointer to an array of length `num` containing block identifiers.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param options Further options for filtering.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Proportion_ = double, typename Block_ = int>
BlockedFilters<Float_> compute_filters_blocked(const MetricsResults<Sum_, Detected_, Proportion_>& metrics, const Block_* block, const FiltersOptions& options) {
    return compute_filters_blocked(metrics.sum.size(), internal::to_buffer(metrics), block, options);
}

}

}

#endif
