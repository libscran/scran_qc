#ifndef SCRAN_ADT_QUALITY_CONTROL_HPP
#define SCRAN_ADT_QUALITY_CONTROL_HPP

#include <vector>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "tatami/tatami.hpp"

#include "find_median_mad.hpp"
#include "per_cell_qc_metrics.hpp"
#include "choose_filter_thresholds.hpp"

/**
 * @file adt_quality_control.hpp
 *
 * @brief Simple per-cell QC metrics from an ADT count matrix.
 */

namespace scran {

/**
 * @namespace scran::adt_quality_control
 * @brief Simple per-cell QC metrics from an ADT count matrix.
 *
 * Given a feature-by-cell ADT count matrix, this class computes several QC metrics:
 * 
 * - The sum of counts for each cell, which (in theory) represents the efficiency of library preparation and sequencing.
 *   This is less useful as a QC metric for ADT data given that the sum is strongly influenced by biological variation in the abundance of the targeted features.
 *   Nonetheless, we compute it for diagnostic purposes.
 * - The number of detected tags per cell.
 *   Even though ADTs are commonly applied in situations where few features are highly abundant, 
 *   we still expect detectable coverage of most features due to ambient contamination, non-specific binding or some background expression.
 *   The absence of detectable coverage indicates that library preparation or sequencing depth was suboptimal.
 * - The sum of counts in pre-defined feature subsets.
 *   While the exact interpretation depends on the nature of the subset, the most common use case involves isotype control (IgG) features.
 *   IgG antibodies should not bind to anything, so high coverage suggests that non-specific binding is a problem, e.g., due to antibody conjugates.
 *
 * We consider low-quality cells to be those with a low number of detected tags and high subset sums.
 * We define thresholds for each metric using an MAD-based outlier approach, see `choose_filter_thresholds` for details.
 * For the number of detected features and the subset sums, the outliers are defined after log-transformation of the metrics.
 *
 * For the number of detected features, we supplement the MAD-based threshold with a minimum drop in the proportion from the median.
 * That is, cells are only considered to be low quality if the difference in the number of detected features from the median is greater than a certain percentage.
 * By default, the number must drop by at least 10% from the median.
 * This avoids overly aggressive filtering when the MAD is zero due to the discrete nature of this statistic in datasets with few tags.
 */
namespace adt_quality_control {

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
 */
template<typename Sum_ = double, typename Detected_ = int>
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
     * Each entry should point to an array of length equal to the number of cells, see `MetricsResults::subset_sum`.
     * This should have length equal to the `subsets` used in `compute_metrics()` and all pointers should be non-NULL.
     */
    std::vector<Sum_*> subset_sum;
};

/**
 * Compute the QC metrics from an input matrix.
 * This is a wrapper around `per_cell_qc_metrics::compute()` with some pre-configuration for ADT-relevant metrics.
 *
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 *
 * @param mat Pointer to a feature-by-cells matrix containing counts.
 * @param[in] subsets Vector of feature subsets, typically IgG controls (see comments in `adt_quality_control`).
 * See `per_cell_qc_metrics::compute()` for more details on the expected format.
 * @param[out] output `MetricsBuffers` object in which to store the output.
 * @param options Further options.
 */
template<typename Value_, typename Index_, typename Subset_, typename Sum_, typename Detected_>
void compute_metrics(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, MetricsBuffers<Sum_, Detected_>& output, const MetricsOptions& options) {
    per_cell_qc_metrics::Buffers<Sum_, Detected_, Value_, Index_> tmp;
    tmp.sum = output.sum;
    tmp.detected = output.detected;
    tmp.subset_sum = output.subset_sum;

    per_cell_qc_metrics::Options opt;
    opt.num_threads = options.num_threads;
    per_cell_qc_metrics::compute(mat, subsets, tmp, opt);
}

/**
 * @brief Results of the QC metric calculations.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 *
 * Meaningful instances of this object should generally be constructed by calling the `compute_metrics()` function.
 */
template<typename Sum_ = double, typename Detected_ = int>
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
     * Sum of counts in each feature subset in each cell.
     * Each inner vector corresponds to a feature subset and is of length equal to the number of cells.
     */
    std::vector<std::vector<Sum_> > subset_sum;
};

/**
 * Overload of `compute_metrics()` that allocates memory for the results.
 *
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Value_ Type of matrix value.
 * @tparam Index_ Type of the matrix indices.
 * @tparam Subset_ Either a pointer to an array of booleans or a `vector` of indices.
 *
 * @param mat Pointer to a feature-by-cells **tatami** matrix containing counts.
 * @param[in] subsets Vector of feature subsets, typically IgG controls (see comments in `adt_quality_control`).
 * See `per_cell_qc_metrics::compute()` for more details on the expected format.
 * @param options Further options.
 *
 * @return An object containing the QC metrics.
 */
template<typename Sum_ = double, typename Detected_ = int, typename Value_ = double, typename Index_ = int, typename Subset_ = const uint8_t*>
MetricsResults<Sum_, Detected_> compute_metrics(const tatami::Matrix<Value_, Index_>* mat, const std::vector<Subset_>& subsets, const MetricsOptions& options) {
    auto NC = mat->ncol();
    MetricsBuffers<Sum_, Detected_> x;
    MetricsResults<Sum_, Detected_> output;

    output.sum.resize(NC);
    x.sum = output.sum.data();

    output.detected.resize(NC);
    x.detected = output.detected.data();

    size_t nsubsets = subsets.size();
    x.subset_sum.resize(nsubsets);
    output.subset_sum.resize(nsubsets);
    for (size_t s = 0; s < nsubsets; ++s) {
        output.subset_sum[s].resize(NC);
        x.subset_sum[s] = output.subset_sum[s].data();
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
     * Minimum drop in the number of detected features from the median, in order to consider a cell to be of low quality.
     * This should lie in \f$[0, 1)\f$.
     */
    double detected_min_drop = 0.1;

    /**
     * Number of MADs above the median, to define the threshold for outliers in the subset sums.
     * This should be non-negative.
     */
    double subset_sum_num_mads = 3;
};

/**
 * @cond
 */
namespace internal {

template<typename Float_, class Host_, typename Sum_, typename Detected_, typename BlockSource_>
void populate(Host_& host, size_t n, const MetricsBuffers<Sum_, Detected_>& res, BlockSource_ block, const FiltersOptions& options) {
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
        opts.num_mads = options.detected_num_mads;
        opts.log = true;
        opts.upper = false;
        opts.min_diff = -std::log(1 - options.detected_min_drop);
        host.get_detected() = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, res.detected, buffer.data(), opts).lower;
            } else {
                return choose_filter_thresholds::internal::strip<true>(choose_filter_thresholds::compute_blocked(n, res.detected, block, &buffer, opts));
            }
        }();
    }

    size_t nsubsets = res.subset_sum.size();
    host.get_subset_sum().resize(nsubsets);
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = res.subset_sum[s];
        choose_filter_thresholds::Options opts;
        opts.num_mads = options.subset_sum_num_mads;
        opts.log = true;
        opts.lower = false;
        host.get_subset_sum()[s] = [&]() {
            if constexpr(unblocked) {
                return choose_filter_thresholds::compute(n, sub, buffer.data(), opts).upper;
            } else {
                return choose_filter_thresholds::internal::strip<false>(choose_filter_thresholds::compute_blocked(n, sub, block, &buffer, opts));
            }
        }();
    }
}

template<class Host_, typename Sum_, typename Detected_, typename BlockSource_, typename Output_>
void filter(const Host_& host, size_t n, const MetricsBuffers<Sum_, Detected_>& metrics, BlockSource_ block, Output_* output) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    std::fill_n(output, n, 1);

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

    size_t nsubsets = metrics.subset_sum.size();
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = metrics.subset_sum[s];
        const auto& sthresh = host.get_subset_sum()[s];
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

template<typename Sum_, typename Detected_>
MetricsBuffers<const Sum_, const Detected_> to_buffer(const MetricsResults<Sum_, Detected_>& metrics) {
    MetricsBuffers<const Sum_, const Detected_> buffer;
    buffer.sum = metrics.sum.data();
    buffer.detected = metrics.detected.data();
    buffer.subset_sum.reserve(metrics.subset_sum.size());
    for (const auto& s : metrics.subset_sum) {
        buffer.subset_sum.push_back(s.data());
    }
    return buffer;
}

}
/**
 * @endcond
 */

/**
 * @brief Filter for high-quality cells using ADT-based metrics. 
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
     * @return Lower threshold to apply to the number of detected tags.
     */
    Float_ get_detected() const {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to each subset proportion.
     */
    const std::vector<Float_>& get_subset_sum() const {
        return my_subset_sum;
    }

    /**
     * @return Lower threshold to apply to the number of detected tags.
     */
    Float_& get_detected() {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to each subset proportion.
     */
    std::vector<Float_>& get_subset_sum() {
        return my_subset_sum;
    }

private:
    Float_ my_detected = 0;
    std::vector<Float_> my_subset_sum;

public:
    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Output_ Boolean type to store the high quality flags.
     *
     * @param num Number of cells.
     * @param metrics A collection of arrays containing ADT-based QC metrics, filled by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     *
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Output_>
    void filter(size_t num, const MetricsBuffers<Sum_, Detected_>& metrics, Output_* output) const {
        internal::filter(*this, num, metrics, false, output);
    }

    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Output_ Boolean type to store the high quality flags.
     *
     * @param metrics ADT-based QC metrics returned by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     * @param[out] output Pointer to an array of length `num`. 
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Output_>
    void filter(const MetricsResults<Sum_, Detected_>& metrics, Output_* output) const {
        return filter(metrics.detected.size(), internal::to_buffer(metrics), output);
    }

    /**
     * @tparam Output_ Boolean type to store the high quality flags.
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     *
     * @param metrics ADT-based QC metrics returned by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     *
     * @return Vector of length `num`, containing the high-quality calls.
     */
    template<typename Output_ = uint8_t, typename Sum_ = double, typename Detected_ = int>
    std::vector<Output_> filter(const MetricsResults<Sum_, Detected_>& metrics) const {
        std::vector<Output_> output(metrics.detected.size());
        filter(metrics, output.data());
        return output;
    }
};

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 *
 * @param num Number of cells.
 * @param metrics A collection of arrays containing ADT-based QC metrics, filled by `compute_metrics()`.
 * @param options Further options for filtering.
 *
 * @return An object containing the filter thresholds.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int>
Filters<Float_> compute_filters(size_t num, const MetricsBuffers<Sum_, Detected_>& metrics, const FiltersOptions& options) {
    Filters<Float_> output;
    internal::populate<Float_>(output, num, metrics, false, options);
    return output;
}

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 *
 * @param metrics ADT-based QC metrics from `compute_metrics()`.
 * @param options Further options for filtering.
 *
 * @return An object containing the filter thresholds.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int>
Filters<Float_> compute_filters(const MetricsResults<Sum_, Detected_>& metrics, const FiltersOptions& options) {
    return compute_filters(metrics.detected.size(), internal::to_buffer(metrics), options);
}

/**
 * @brief Filter for high-quality cells using ADT-based metrics with blocking.
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
     * containing the lower threshold on the number of detected tags in each block.
     */
    const std::vector<Float_>& get_detected() const {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of blocks.
     * Each entry is a vector of length equal to the number of feature subsets,
     * containing the upper threshold to apply to the each subset proportion.
     */
    const std::vector<std::vector<Float_> >& get_subset_sum() const {
        return my_subset_sum;
    }

    /**
     * @return Vector of length equal to the number of blocks,
     * containing the lower threshold on the number of detected tags in each block.
     */
    std::vector<Float_>& get_detected() {
        return my_detected;
    }

    /**
     * @return Vector of length equal to the number of feature subsets.
     * Each entry is a vector of length equal to the number of blocks,
     * containing the upper threshold to apply to the subset proportion for that block.
     */
    std::vector<std::vector<Float_> >& get_subset_sum() {
        return my_subset_sum;
    }

private:
    std::vector<Float_> my_sum;
    std::vector<Float_> my_detected;
    std::vector<std::vector<Float_> > my_subset_sum;

public:
    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type to store the high quality flags.
     *
     * @param num Number of cells.
     * @param metrics A collection of arrays containing ADT-based QC metrics, filled by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Index_, typename Sum_, typename Detected_, typename Block_, typename Output_>
    void filter(Index_ num, const MetricsBuffers<Sum_, Detected_>& metrics, const Block_* block, Output_* output) const {
        internal::filter(*this, num, metrics, block, output);
    }

    /**
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Block_ Integer type for the block assignment.
     * @tparam Output_ Boolean type to store the high quality flags.
     *
     * @param metrics ADT-based QC metrics computed by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     * @param[out] output Pointer to an array of length `num`.
     * On output, this is truthy for cells considered to be of high quality, and false otherwise.
     */
    template<typename Sum_, typename Detected_, typename Block_, typename Output_>
    void filter(const MetricsResults<Sum_, Detected_>& metrics, const Block_* block, Output_* output) const {
        return filter(metrics.detected.size(), internal::to_buffer(metrics), block, output);
    }

    /**
     * @tparam Output_ Boolean type to store the high quality flags.
     * @tparam Sum_ Numeric type to store the summed expression.
     * @tparam Detected_ Integer type to store the number of cells.
     * @tparam Block_ Integer type for the block assignment.
     *
     * @param metrics ADT-based QC metrics computed by `compute_metrics()`.
     * The feature subsets should be the same as those used in the `metrics` supplied to `compute_filters()`.
     * @param[in] block Pointer to an array of length `num` containing block identifiers.
     * Each identifier should correspond to the same blocks used in the constructor.
     *
     * @return Vector of length `num`, containing the high-quality calls.
     */
    template<typename Output_ = uint8_t, typename Sum_ = double, typename Detected_ = int, typename Block_ = int>
    std::vector<Output_> filter(const MetricsResults<Sum_, Detected_>& metrics, const Block_* block) const {
        std::vector<Output_> output(metrics.detected.size());
        filter(metrics, block, output.data());
        return output;
    }
};

/**
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Block_ Integer type for the block assignments.
 *
 * @param num Number of cells.
 * @param metrics A collection of arrays containing ADT-based QC metrics, filled by `compute_metrics()`.
 * @param[in] block Pointer to an array of length `num` containing block identifiers.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param options Further options for filtering.
 *
 * @return Object containing filter thresholds for each block.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Block_ = int>
BlockedFilters<Float_> compute_filters_blocked(size_t num, const MetricsBuffers<Sum_, Detected_>& metrics, const Block_* block, const FiltersOptions& options) {
    BlockedFilters<Float_> output;
    internal::populate<Float_>(output, num, metrics, block, options);
    return output;
}

/**
 * @tparam Sum_ Numeric type to store the summed expression.
 * @tparam Detected_ Integer type to store the number of cells.
 * @tparam Block_ Integer type for the block assignments.
 *
 * @param metrics ADT-based QC metrics computed by `compute_metrics()`.
 * @param[in] block Pointer to an array of length `num` containing block identifiers.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param options Further options for filtering.
 *
 * @return Object containing filter thresholds for each block.
 */
template<typename Float_ = double, typename Sum_ = double, typename Detected_ = int, typename Block_ = int>
BlockedFilters<Float_> compute_filters_blocked(const MetricsResults<Sum_, Detected_>& metrics, const Block_* block, const FiltersOptions& options) {
    return compute_filters_blocked(metrics.detected.size(), internal::to_buffer(metrics), block, options);
}

}

}

#endif
