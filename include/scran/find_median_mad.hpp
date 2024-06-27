#ifndef SCRAN_FIND_MEDIAN_MAD_H
#define SCRAN_FIND_MEDIAN_MAD_H

#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "tatami_stats/tatami_stats.hpp"

/**
 * @file find_median_mad.hpp
 * @brief Compute the median and MAD from an array of values.
 */

namespace scran {

/**
 * @namespace scran::find_median_mad
 * @brief Calculate the median and MAD from an array of values.
 *
 * Pretty much as it says on the can; calculates the median of an array of values first,
 * and uses the median to then compute the median absolute deviation (MAD) from that array.
 * We support calculation of medians and MADs for separate "blocks" of observations in the same array.
 */
namespace find_median_mad {

/**
 * @brief Options for `find_median_mad::compute()`.
 */
struct Options {
    /**
     * Whether to compute the median and MAD after log-transformation of the values.
     * This is useful for defining thresholds based on fold changes from the center.
     * If `true`, all values are assumed to be non-negative.
     */
    bool log = false;

    /**
     * Whether to only compute the median.
     * If true, `Results::mads` will be filled with NaNs.
     */
    bool median_only = false;
};

/**
 * @brief Container for the median and MAD.
 * @tparam Float_ Floating-point type. 
 */
template<typename Float_>
struct Results {
    /**
     * @cond
     */
    Results(Float_ m1, Float_ m2) : median(m1), mad(m2) {}
    Results() = default;
    /**
     * @endcond
     */

    /**
     * Median.
     */
    Float_ median = 0;

    /**
     * Median absolute deviation.
     */
    Float_ mad = 0;
};

/**
 * @tparam Index_ Integer type for array indices.
 * @tparam Float_ Floating-point type for input and output.
 *
 * @param num Number of observations.
 * @param[in] metrics Pointer to an array of observations of length `n`.
 * NaNs are ignored.
 * Array contents are arbitrarily modified on function return and should not be used afterwards.
 * @param options Further options.
 *
 * @return Median and MAD for `metrics`, possibly after log-transformation.
 */
template<typename Index_, typename Float_> 
Results<Float_> compute(Index_ num, Float_* metrics, const Options& options) {
    static_assert(std::is_floating_point<Float_>::value);

    // Rotate all the NaNs to the front of the buffer and ignore them.
    Index_ lost = 0;
    for (Index_ i = 0; i < num; ++i) {
        if (std::isnan(metrics[i])) {
            std::swap(metrics[i], metrics[lost]);
            ++lost;
        }
    }
    metrics += lost;
    num -= lost;

    if (options.log) {
        auto copy = metrics;
        for (Index_ i = 0; i < num; ++i, ++copy) {
            if (*copy > 0) {
                *copy = std::log(*copy);
            } else if (*copy == 0) {
                *copy = -std::numeric_limits<double>::infinity();
            } else {
                throw std::runtime_error("cannot log-transform negative values");
            }
        }
    }

    // No need to skip the NaNs again.
    auto median = tatami_stats::medians::direct<Float_>(metrics, num, /* skip_nan = */ false);

    if (options.median_only || std::isnan(median)) {
        // Giving up.
        return Results<Float_>(median, std::numeric_limits<Float_>::quiet_NaN());
    } else if (std::isinf(median)) {
        // MADs should be no-ops when added/subtracted from infinity. Any
        // finite value will do here, so might as well keep it simple.
        return Results<Float_>(median, static_cast<Float_>(0));
    }

    auto copy = metrics;
    for (Index_ i = 0; i < num; ++i, ++copy) {
        *copy = std::abs(*copy - median);
    }
    auto mad = tatami_stats::medians::direct<Float_>(metrics, num, /* skip_nan = */ false);
    mad *= 1.4826; // for equivalence with the standard deviation under normality.

    return Results<Float_>(median, mad);
}

/**
 * @brief Temporary data structures for `compute_blocked()`.
 *
 * This can be re-used across multiple `compute_blocked()` calls to avoid reallocation.
 *
 * @tparam Float_ Floating-point type for buffering.
 * @tparam Index_ Integer type for array indices.
 */
template<typename Float_, typename Index_>
class Workspace {
public:
    /**
     * @tparam Block_ Integer type for the block identifiers.
     * @param num Number of observations.
     * @param[in] block Pointer to an array of block identifiers, see `set()`.
     */
    template<typename Block_>
    Workspace(Index_ num, const Block_* block) : my_buffer(num) {
        set(num, block);
    }

    /**
     * Default constructor.
     */
    Workspace() = default;

    /**
     * @tparam Block_ Integer type for the block identifiers.
     * @param num Number of observations.
     * @param[in] block Pointer to an array of block identifiers.
     * The array should be of length equal to `num`.
     * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
     */
    template<typename Block_>
    void set(Index_ num, const Block_* block) {
        my_block_starts.clear();

        if (block) { 
            for (Index_ i = 0; i < num; ++i) {
                size_t candidate = block[i];
                if (candidate >= my_block_starts.size()) {
                    my_block_starts.resize(candidate + 1);
                }
                ++my_block_starts[candidate];
            }

            Index_ sofar = 0;
            for (auto& s : my_block_starts) {
                Index_ last = sofar;
                sofar += s;
                s = last;
            }
        }

        my_buffer.resize(num);
        my_block_ends.resize(my_block_starts.size());
    }

/**
 * @cond
 */
public:
    // Can't figure out how to make compute_blocked() a friend,
    // so these puppies are public for simplicity.
    std::vector<Float_> my_buffer;
    std::vector<Index_> my_block_starts;
    std::vector<Index_> my_block_ends;
/**
 * @endcond
 */
};

/**
 * @tparam Output_ Floating-point type for the output.
 * @tparam Index_ Integer type for array indices.
 * @tparam Block_ Integer type, containing the block IDs.
 * @tparam Value_ Numeric type for the input.
 *
 * @param num Number of observations.
 * @param[in] block Optional pointer to an array of block identifiers.
 * If provided, the array should be of length equal to `num`.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * If a null pointer is supplied, all observations are assumed to belong to the same block.
 * @param[in] metrics Pointer to an array of observations of length `num`.
 * @param workspace A workspace object, either (i) constructed on `num` and `block` or (ii) configured using `Workspace::set()` on `num` and `block`.
 * The same object can be re-used across multiple calls to `compute_blocked()` with the same `num` and `block`.
 * @param options Further options.
 *
 * @return Vector of length \f$N\f$, where each entry contains the median and MAD for each block in `block`.
 */
template<typename Output_ = double, typename Index_ = int, typename Block_ = int, typename Value_ = double>
std::vector<Results<Output_> > compute_blocked(Index_ num, const Block_* block, const Value_* metrics, Workspace<Output_, Index_>& workspace, const Options& options) {
    std::vector<Results<Output_> > output;

    auto& buffer = workspace.my_buffer;
    if (!block) {
        std::copy_n(metrics, num, buffer.begin());
        output.push_back(compute(num, buffer.data(), options));
        return output;
    }

    const auto& starts = workspace.my_block_starts;
    auto& ends = workspace.my_block_ends;
    std::copy(starts.begin(), starts.end(), ends.begin());
    for (Index_ i = 0; i < num; ++i) {
        auto& pos = ends[block[i]];
        buffer[pos] = metrics[i];
        ++pos;
    }

    // Using the ranges on the buffer.
    size_t nblocks = starts.size();
    output.reserve(nblocks);
    for (size_t g = 0; g < nblocks; ++g) {
        output.push_back(compute(ends[g] - starts[g], buffer.data() + starts[g], options));
    }

    return output;
}

/**
 * Overload that handles the creation of the `Workspace`.
 *
 * @tparam Output_ Floating-point type for the output.
 * @tparam Index_ Integer type for array indices
 * @tparam Block_ Integer type, containing the block IDs.
 * @tparam Value_ Numeric type for the input.
 *
 * @param num Number of observations.
 * @param[in] block Pointer to an array of block identifiers, see `compute_blocked()`.
 * @param[in] metrics Pointer to an array of observations of length `num`.
 * @param options Further options.
 *
 * @return Vector of length \f$N\f$, where each entry is a pair containing the median and MAD for each block in `block`.
 */
template<typename Output_ = double, typename Index_ = int, typename Block_ = int, typename Value_ = double>
std::vector<Results<Output_> > compute_blocked(Index_ num, const Block_* block, const Value_* metrics, const Options& options) {
    Workspace<Output_, Index_> wrk(num, block);
    return compute_blocked<Output_>(num, block, metrics, wrk, options);
}

}

}

#endif
