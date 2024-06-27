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
 * We define filter thresholds on the QC metrics by assuming that cells with extreme values for the metrics are of low quality.
 * Given an array of values, outliers are defined as those that are more than some number of median absolute deviations (MADs) from the median value.
 * Outliers can be defined in both directions, or just a single direction, depending on the interpretation of the QC metric.
 */
namespace choose_filter_thresholds {

/**
 * @brief Options for `prepare()`.
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
     * If `Options::unlog = true`, this difference is interpreted as a unit on the log-scale.
     */
    double min_diff = 0;

    /**
     * Whether the supplied median and MAD were computed on the log-scale (i.e., `find_median_mad::Options::log = true`).
     * If true, the thresholds are converted back to the original scale of the metrics prior to filtering.
     */
    bool unlog = false;
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

template<typename Float_>
std::pair<Float_, Float_> choose(Float_ median, Float_ mad, const Options& options) {
    static_assert(std::is_floating_point<Float_>::value);
    Float_ lthresh = -std::numeric_limits<Float_>::infinity();
    Float_ uthresh = std::numeric_limits<double>::infinity();

    if (!std::isnan(median) && !std::isnan(mad)) {
        auto delta = std::max(static_cast<Float_>(options.min_diff), options.num_mads * mad);
        if (options.lower) {
            lthresh = sanitize(median - delta, options.unlog);
        }
        if (options.upper) {
            uthresh = sanitize(median + delta, options.unlog);
        }
    }

    return std::make_pair(lthresh, uthresh);
}

}
/**
 * @endcond
 */

/**
 * @brief Outlier thresholds for QC filtering.
 * 
 * @tparam Float_ Floating-point type for the thresholds.
 */
template<typename Float_>
class Thresholds {
public:
    /**
     * @cond
     */
    Thresholds() = default;
    static_assert(std::is_floating_point<Float_>::value);
    /**
     * @endcond
     */

    /**
     * @param mm Median and MAD, typically from `find_median_mad::compute()`.
     * @param options Further options.
     */
    Thresholds(const find_median_mad::Results<Float_>& mm, const Options& options) {
        auto choice = internal::choose(mm.median, mm.mad, options);
        my_lower = choice.first;
        my_upper = choice.second;
    }

private:
    Float_ my_lower, my_upper;

public:
    /**
     * @return Lower threshold.
     * Cells where the relevant QC metric is below this threshold are considered to be low quality.j
     * This is set to negative infinity if `Options::lower = false`.
     */
    Float_ get_lower() const {
        return my_lower;
    }

    /**
     * @return Upper threshold.
     * Cells where the relevant QC metric is above this threshold are considered to be low quality.
     * This is set to positive infinity if `Options::upper = false`.
     */
    Float_ get_upper() const {
        return my_upper;
    }

public:
    /**
     * @param x Value of the metric for a single cell.
     * @return Whether the cell is of high quality, i.e., not NaN, not an outlier.
     * This is true if `x` is not below `get_lower()` and not above `get_upper()`.
     */
    bool filter(Float_ x) const {
        return x >= my_lower && x <= my_upper; // NaNs don't compare true in any comparison so they get auto-discarded here.
    }

    /**
     * @tparam Index_ Integer type for the array indices.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     * @tparam Output_ Boolean type for the outlier calls.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     * @param[out] output Pointer to an array of length `n`, to store the high-quality calls (see the other `filter()` overload).
     * If `overwrite = false`, entries are only set to false for low-quality cells;
     * this effectively performs AND'ing of high-quality calls across multiple calls to `filter()`.
     * @param overwrite Whether to overwrite existing false-y entries in `output`.
     */
    template<typename Index_, typename Value_, typename Output_>
    void filter(Index_ n, const Value_* input, Output_* output, bool overwrite) const {
        if (overwrite) {
            for (Index_ i = 0; i < n; ++i) {
                output[i] = filter(input[i]);
            }
        } else {
            for (Index_ i = 0; i < n; ++i) {
                if (!filter(input[i])) {
                    output[i] = false;
                }
            }
        }
    }

    /**
     * @tparam Output_ Boolean type for the outlier calls.
     * @tparam Index_ Integer type for the array indices.
     * @tparam Value_ Type for the metrics, to be compared to the thresholds.
     *
     * @param n Number of observations.
     * @param[in] input Pointer to an array of length `n`, containing the values to be filtered.
     *
     * @return Vector of length `n`, specifying whether a cell is of high quality.
     */
    template<typename Output_ = uint8_t, typename Index_, typename Value_>
    std::vector<Output_> filter(Index_ n, const Value_* input) {
        std::vector<Output_> output(n);
        filter(n, input, output.data(), true);
        return output;
    }
};

/**
 * @brief Outlier thresholds for QC filtering with blocks.
 *
 * @tparam Float_ Floating-point type for the thresholds.
 */
template<typename Float_>
class BlockThresholds {
public:
    /**
     * @cond
     */
    BlockThresholds() = default;
    static_assert(std::is_floating_point<Float_>::value);
    /**
     * @endcond
     */

    /**
     * @param mm Median and MAD for each block, typically from `find_median_mad::compute_blocked()`.
     * @param options Further options.
     */
    BlockThresholds(const std::vector<find_median_mad::Results<Float_> >& mm, const Options& options) {
        size_t nblocks = mm.size();
        my_lower.reserve(nblocks);
        my_upper.reserve(nblocks);

        for (size_t b = 0; b < nblocks; ++b) {
            auto out = internal::choose(mm[b].median, mm[b].mad, options);
            my_lower.push_back(out.first);
            my_upper.push_back(out.second);
        }
    }

private:
    std::vector<Float_> my_lower, my_upper;

public:
    /**
     * @return Vector of lower thresholds, one per batch.
     * Cells where the relevant QC metric is below this threshold are considered to be low quality.
     */
    const std::vector<Float_>& get_lower() const {
        return my_lower;
    }

    /**
     * @return Vector of upper thresholds, one per batch.
     * Cells where the relevant QC metric is above this threshold are considered to be low quality.
     */
    const std::vector<Float_>& get_upper() const {
        return my_upper;
    }

public:
    /**
     * @tparam Block_ Integer type for the block assignment.
     * @param x Value of the metric for a single cell.
     * @param b Block of origin for the cell.
     * This should be less than the `mm.size()` in the constructor.
     * @return Whether the cell is of high quality, i.e., not NaN, not an outlier.
     * This is true if `x` is not below `get_lower()` and not above `get_upper()` for its block.
     */
    template<typename Block_>
    bool filter(Float_ x, Block_ b) const {
        return x >= my_lower[b] && x <= my_upper[b]; // NaNs don't compare true in any comparison so they get auto-discarded here.
    }

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
     * If `overwrite = false`, entries are only set to false for low-quality cells;
     * this effectively performs AND'ing of high-quality calls across multiple calls to `filter()`.
     * @param overwrite Whether to overwrite existing false-y entries in `output`.
     */
    template<typename Index_, typename Value_, typename Block_, typename Output_>
    void filter(Index_ n, const Value_* input, const Block_* block, Output_* output, bool overwrite) const {
        if (overwrite) {
            for (Index_ i = 0; i < n; ++i) {
                output[i] = filter(input[i], block[i]);
            }
        } else {
            for (Index_ i = 0; i < n; ++i) {
                if (!filter(input[i], block[i])) {
                    output[i] = false;
                }
            }
        }
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
    template<typename Output_ = uint8_t, typename Index_, typename Value_, typename Block_>
    std::vector<Output_> filter(Index_ n, const Value_* input, const Block_* block) {
        std::vector<Output_> output(n);
        filter(n, input, block, output.data(), true);
        return output;
    }
};

}

}

#endif
