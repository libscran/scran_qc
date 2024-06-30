#ifndef SCRAN_CHOOSE_FILTER_THRESHOLDS_HPP
#define SCRAN_CHOOSE_FILTER_THRESHOLDS_HPP

#include <vector>
#include <limits>
#include <cmath>

#include "find_median_mad.hpp"

/**
 * @file choose_filter_thresholds.hpp
 * @brief Define QC filter thresholds using a MAD-based approach.
 */

namespace scran {

/**
 * @namespace scran::choose_filter_thresholds
 * @brief Define QC filter thresholds using a MAD-based approach.
 *
 * We define filter thresholds on the QC metrics by assuming that most cells in the experiment are of high (or at least acceptable) quality.
 * Any outlier values are indicative of low-quality cells that should be filtered out.
 * Given an array of values, outliers are defined as those that are more than some number of median absolute deviations (MADs) from the median value.
 * Outliers can be defined in both directions or just a single direction, depending on the interpretation of the QC metric.
 * We can also apply a log-transformation to the metrics to identify outliers with respect to their fold-change from the median.
 *
 * For datasets with multiple blocks, we can also compute block-specific thresholds for each metric.
 * This assumes that differences in the metric distributions between blocks are driven by uninteresting causes (e.g., differences in sequencing depth);
 * variable thresholds can adapt to each block's distribution for effective removal of outliers.
 * However, if the differences in the distributions between blocks are interesting,
 * it may be preferable to ignore the blocking factor so that the MADs are correctly increased to reflect that variation.
 */
namespace choose_filter_thresholds {

/**
 * @brief Options for `compute()`.
 */
struct Options {
    /**
     * Should low values be considered as potential outliers?
     * If `false`, no lower threshold is applied when defining outliers.
     */
    bool lower = true;

    /**
     * Should high values be considered as potential outliers?
     * If `false`, no upper threshold is applied when defining outliers.
     */
    bool upper = true;

    /**
     * Number of MADs to use to define outliers.
     * Larger values result in more relaxed thresholds.
     * By default, we require 3 MADs, which is motivated by the low probability (less than 1%) of obtaining such a value under the normal distribution.
     */
    double num_mads = 3;

    /**
     * Minimum difference from the median to define outliers.
     * This enforces a more relaxed threshold in cases where the MAD may be too small.
     * If `Options::log = true`, this difference is interpreted as a unit on the log-scale.
     */
    double min_diff = 0;

    /**
     * Whether the supplied median and MAD should be computed on the log-scale (i.e., `find_median_mad::Options::log = true`).
     * This focuses on the fold-change from the median when defining outliers.
     * In practice, this is useful for metrics that are always positive and have right-skewed distributions,
     * as the log-transformation symmetrizes the distribution and makes it more normal-like such that the `Options::num_mads` interpretation can be applied.
     * It also ensures that the defined threshold is always positive.
     *
     * If this is set to true, the thresholds are converted back to the original scale of the metrics prior to filtering.
     */
    bool log = false;
};

/**
 * @brief Results of `compute()`.
 * @tparam Float_ Floating-point type for the thresholds.
 */
template<typename Float_>
struct Results {
    /**
     * Lower threshold.
     * Cells where the relevant QC metric is below this threshold are considered to be low quality.j
     * This is set to negative infinity if `Options::lower = false`.
     */
    Float_ lower = 0;

    /**
     * Upper threshold.
     * Cells where the relevant QC metric is above this threshold are considered to be low quality.
     * This is set to positive infinity if `Options::upper = false`.
     */
    Float_ upper = 0;
};

/**
 * @cond
 */
namespace internal {

template<typename Float_>
Float_ sanitize(Float_ val, bool unlog) {
    if (unlog) {
        if (std::isinf(val)) {
            if (val < 0) {
                return 0;
            }
        } else {
            return std::exp(val);
        }
    }
    return val;
}

template<bool lower_, typename Float_>
std::vector<Float_> strip(const std::vector<Results<Float_> >& res) {
    std::vector<Float_> output;
    output.reserve(res.size());
    for (const auto& r : res) {
        if constexpr(lower_) {
            output.push_back(r.lower);
        } else {
            output.push_back(r.upper);
        }
    }
    return output;
}

}
/**
 * @endcond
 */

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @param mm Median and MAD from `find_median_mad::compute()`.
 * If `Options::log = true`, it is expected that the median and MAD are computed on the log-transformed metrics
 * (i.e., `find_median_mad::Options::log` was also set to true).
 * @param options Further options.
 * @return The upper and lower thresholds derived from `mm`.
 */
template<typename Float_>
Results<Float_> compute(const find_median_mad::Results<Float_>& mm, const Options& options) {
    static_assert(std::is_floating_point<Float_>::value);
    Results<Float_> output;
    Float_& lthresh = output.lower;
    Float_& uthresh = output.upper;
    lthresh = -std::numeric_limits<Float_>::infinity();
    uthresh = std::numeric_limits<double>::infinity();

    auto median = mm.median;
    auto mad = mm.mad;
    if (!std::isnan(median) && !std::isnan(mad)) {
        auto delta = std::max(static_cast<Float_>(options.min_diff), options.num_mads * mad);
        if (options.lower) {
            lthresh = internal::sanitize(median - delta, options.log);
        }
        if (options.upper) {
            uthresh = internal::sanitize(median + delta, options.log);
        }
    }

    return output;
}

/**
 * This overload computes the median and MAD via `find_median_mad::compute()` before deriving thresholds with `compute()`.
 *
 * @tparam Index_ Integer type for the array indices. 
 * @tparam Float_ Floating-point type for the metrics and thresholds.
 *
 * @param num Number of cells.
 * @param[in] metrics Pointer to an array of length `num`, containing a QC metric for each cell.
 * This is modified arbitrarily on output.
 * @param options Further options.
 *
 * @return The upper and lower thresholds derived from `metrics`.
 */
template<typename Index_, typename Float_>
Results<Float_> compute(Index_ num, Float_* metrics, const Options& options) {
    find_median_mad::Options fopt;
    fopt.log = options.log;
    auto mm = find_median_mad::compute(num, metrics, fopt);
    return compute(mm, options);
}

/**
 * This overload computes the median and MAD via `find_median_mad::compute()` with automatic buffer allocation,
 * before deriving thresholds with `compute()`.
 *
 * @tparam Index_ Integer type for the array indices. 
 * @tparam Value_ Type for the input data.
 * @tparam Float_ Floating-point type for the metrics and thresholds.
 *
 * @param num Number of cells.
 * @param[in] metrics Pointer to an array of length `num`, containing a QC metric for each cell.
 * @param buffer Pointer to an array of length `num` in which to store intermediate results.
 * Alternatively NULL, in which case a buffer is automatically allocated.
 * @param options Further options.
 *
 * @return The upper and lower thresholds derived from `metrics`.
 */
template<typename Index_, typename Value_, typename Float_>
Results<Float_> compute(Index_ num, const Value_* metrics, Float_* buffer, const Options& options) {
    find_median_mad::Options fopt;
    fopt.log = options.log;
    auto mm = find_median_mad::compute(num, metrics, buffer, fopt);
    return compute(mm, options);
}

/**
 * @tparam Float_ Floating-point type for the thresholds.
 * @param mms Vector of medians and MADs for each block.
 * @param options Further options.
 *
 * @return A vector containing the upper and lower thresholds for each block.
 */
template<typename Float_>
std::vector<Results<Float_> > compute_blocked(const std::vector<find_median_mad::Results<Float_> > mms, const Options& options) {
    std::vector<Results<Float_> > output;
    output.reserve(mms.size());
    for (auto& mm : mms) {
        output.emplace_back(compute(mm, options));
    }
    return output;
}

/**
 * This overload computes the median and MAD for each block via `find_median_mad::compute_blocked()` before deriving thresholds in each block with `compute_blocked()`.
 *
 * @tparam Index_ Integer type for the array indices. 
 * @tparam Value_ Type for the input data.
 * @tparam Float_ Floating-point type for the metrics and thresholds.
 *
 * @param num Number of cells.
 * @param[in] metrics Pointer to an array of length `num`, containing a QC metric for each cell.
 * @param[in] block Optional pointer to an array of block identifiers, see `find_median_mad::compute_blocked()` for details.
 * @param workspace Pointer to a workspace object, see `find_median_mad::compute_blocked()` for details.
 * @param options Further options.
 *
 * @return A vector containing the upper and lower thresholds for each block.
 */
template<typename Index_, typename Value_, typename Block_, typename Float_>
std::vector<Results<Float_> > compute_blocked(Index_ num, const Value_* metrics, const Block_* block, find_median_mad::Workspace<Float_, Index_>* workspace, const Options& options) {
    find_median_mad::Options fopt;
    fopt.log = options.log;
    auto mms = find_median_mad::compute_blocked(num, metrics, block, workspace, fopt);
    return compute_blocked(mms, options);
}

}

}

#endif
