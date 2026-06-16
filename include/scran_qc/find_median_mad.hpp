#ifndef SCRAN_QC_FIND_MEDIAN_MAD_H
#define SCRAN_QC_FIND_MEDIAN_MAD_H

#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <cassert>

#include "quickstats/quickstats.hpp"
#include "sanisizer/sanisizer.hpp"

#include "utils.hpp"

/**
 * @file find_median_mad.hpp
 * @brief Compute the median and MAD from an array of values.
 */

namespace scran_qc {

/**
 * @brief Options for `find_median_mad()`.
 */
struct FindMedianMadOptions {
    /**
     * Whether to compute the median and MAD after log-transformation of the values.
     * This is useful for defining thresholds based on fold-changes from the center.
     * If `true`, all values are assumed to be non-negative.
     */
    bool log = false;

    /**
     * Whether to only compute the median.
     * If true, `FindMedianMadResults::mad` will be set to NaN.
     */
    bool median_only = false;
};

/**
 * @brief Results of `find_median_mad()`.
 * @tparam Float_ Floating-point type. 
 */
template<typename Float_>
struct FindMedianMadResults {
    /**
     * @cond
     */
    FindMedianMadResults(Float_ m1, Float_ m2) : median(m1), mad(m2) {}
    FindMedianMadResults() = default;
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
 * Pretty much as it says on the tin; computes the median and MAD of an array.
 *
 * @tparam Float_ Floating-point type of the input and output.
 * This should be capable of representing NaNs.
 *
 * @param num_obs Number of observations.
 * @param[in] metrics Pointer to an array of observations of length `num_obs`.
 * Array contents are arbitrarily modified on function return and should not be used afterwards.
 * @param options Further options.
 *
 * @return Median and MAD for `metrics`, possibly after log-transformation.
 * If `num_obs = 0`, both values are NaNs.
 * If `FindMedianMadOptions::median_only = true`, the MAD is NaN.
 */
template<typename Float_> 
FindMedianMadResults<Float_> find_median_mad(std::size_t num_obs, Float_* metrics, const FindMedianMadOptions& options) {
    static_assert(std::is_floating_point<Float_>::value);

    // Rotate all the NaNs to the front of the buffer and ignore them.
    I<decltype(num_obs)> lost = 0;
    for (I<decltype(num_obs)> i = 0; i < num_obs; ++i) {
        if (std::isnan(metrics[i])) {
            std::swap(metrics[i], metrics[lost]);
            ++lost;
        }
    }
    metrics += lost;
    num_obs -= lost;

    if (options.log) {
        for (I<decltype(num_obs)> i = 0; i < num_obs; ++i) {
            auto& val = metrics[i];
            if (val > 0) {
                val = std::log(val);
            } else if (val == 0) {
                val = -std::numeric_limits<double>::infinity();
            } else {
                throw std::runtime_error("cannot log-transform negative values");
            }
        }
    }

    const auto median = quickstats::median<Float_>(num_obs, metrics);
    if (options.median_only || std::isnan(median)) {
        // Giving up.
        return FindMedianMadResults<Float_>(median, std::numeric_limits<Float_>::quiet_NaN());
    } else if (std::isinf(median)) {
        // MADs should be no-ops when added/subtracted from infinity. Any
        // finite value will do here, so might as well keep it simple.
        return FindMedianMadResults<Float_>(median, static_cast<Float_>(0));
    }

    // As an aside, there's no way to avoid passing in 'metrics' as a Float_,
    // even if the original values were integers, because we need to do this
    // subtraction here that could cast integers to floats. So at some point we
    // will need a floating-point buffer, and so we might as well just pass the
    // metrics in as floats in the first place. Technically the first sort
    // could be done with an integer buffer but then we'd need an extra argument.

    for (I<decltype(num_obs)> i = 0; i < num_obs; ++i) {
        metrics[i] = std::abs(metrics[i] - median);
    }
    auto mad = quickstats::median<Float_>(num_obs, metrics);
    mad *= 1.4826; // for equivalence with the standard deviation under normality.

    return FindMedianMadResults<Float_>(median, mad);
}

/**
 * @brief Workspace for `find_median_mad_blocked()`.
 *
 * This can be re-used across multiple `find_median_mad_blocked()` calls to avoid reallocation.
 *
 * @tparam Float_ Floating-point type of the buffer.
 */
template<typename Float_>
struct FindMedianMadBlockedWorkspace {
    /**
     * @tparam Block_ Integer type of the block identifiers.
     * @param num_obs Number of observations.
     * @param[in] block Pointer to an array of block identifiers. 
     * The array should be of length equal to `num`.
     * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
     * @param num_blocks Total ncumber of blocks, i.e., \f$N\f$.
     */
    template<typename Block_>
    FindMedianMadBlockedWorkspace(const std::size_t num_obs, const Block_* const block, const std::size_t num_blocks) {
        reset_find_median_mad_blocked_workspace(*this, num_obs, block, num_blocks);
    }

    /**
     * Default constructor.
     */
    FindMedianMadBlockedWorkspace() = default;

/**
 * @cond
 */
public:
    std::vector<Float_> buffer;
    std::vector<std::size_t> block_starts;
    std::vector<std::size_t> block_offsets;
/**
 * @endcond
 */
};

/**
 * Reset a `FindMedianMadBlockedWorkspace` object so that it can be used with a new blocking factor.
 *
 * @tparam Float_ Floating-point type of the buffer.
 * @tparam Block_ Integer type of the block identifiers.
 *
 * @param work Workspace object.
 * On return, `work` is equivalent to an object that was constructed with `num_obs`, `block`, and `num_blocks`.
 * @param num_obs New number of observations.
 * @param[in] block Pointer to an array of block identifiers.
 * The array should be of length equal to `num_obs`.
 * Values should be integer IDs in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param num_blocks Total ncumber of blocks, i.e., \f$N\f$.
 */
template<typename Float_, typename Block_>
void reset_find_median_mad_blocked_workspace(
    FindMedianMadBlockedWorkspace<Float_>& work,
    const std::size_t num_obs,
    const Block_* const block,
    const std::size_t num_blocks
) {
    work.block_starts.clear();

    sanisizer::resize(work.block_starts, num_blocks);
    for (I<decltype(num_obs)> i = 0; i < num_obs; ++i) {
        ++work.block_starts[block[i]];
    }

    std::size_t sofar = 0;
    for (auto& s : work.block_starts) {
        const auto last = sofar;
        sofar += s;
        s = last;
    }

    sanisizer::resize(work.buffer, num_obs
#ifdef SCRAN_QC_TEST_INIT
        , SCRAN_QC_TEST_INIT
#endif
    );

    sanisizer::resize(work.block_offsets, num_blocks
#ifdef SCRAN_QC_TEST_INIT
        , SCRAN_QC_TEST_INIT
#endif
    );
}

/**
 * For blocked datasets, this function computes the median and MAD for each block.
 * It is equivalent to calling `find_median_mad()` separately on all observations from each block.
 *
 * @tparam Output_ Floating-point type of the output.
 * @tparam Block_ Integer type, containing the block IDs.
 * @tparam Value_ Numeric type of the input.
 *
 * @param num_obs Number of observations.
 * @param[in] metrics Pointer to an array of observations of length `num_obs`.
 * NaNs are ignored.
 * @param[in] block Pointer to an array of length `num_obs`, containing block assignments.
 * Eacn entry should be an integer ID in \f$[0, N)\f$ where \f$N\f$ is the number of blocks.
 * @param workspace Pointer to a workspace object, either (i) constructed with `num_obs` and `block`
 * or (ii) configured using `reset_find_median_mad_blocked_workspace()` on `num_obs` and `block`. 
 * The same object can be re-used across multiple calls to `find_median_mad_blocked()` with the same `num` and `block`.
 * @param options Further options.
 *
 * @return Vector of length \f$N\f$, where each entry contains the median and MAD for each block in `block`.
 */
template<typename Output_ = double, typename Value_, typename Block_>
std::vector<FindMedianMadResults<Output_> > find_median_mad_blocked(
    const std::size_t num_obs,
    const Value_* const metrics, 
    const Block_* const block,
    FindMedianMadBlockedWorkspace<Output_>& workspace,
    const FindMedianMadOptions& options
) {
    assert(num_obs == workspace.buffer.size());

    auto& buffer = workspace.buffer;
    const auto& starts = workspace.block_starts;
    auto& ends = workspace.block_offsets;
    std::copy(starts.begin(), starts.end(), ends.begin());
    for (I<decltype(num_obs)> i = 0; i < num_obs; ++i) {
        auto& pos = ends[block[i]];
        buffer[pos] = metrics[i];
        ++pos;
    }

    const auto num_blocks = workspace.block_starts.size();
    std::vector<FindMedianMadResults<Output_> > output;
    output.reserve(num_blocks);
    for (I<decltype(num_blocks)> g = 0; g < num_blocks; ++g) {
        output.push_back(find_median_mad(ends[g] - starts[g], buffer.data() + starts[g], options));
    }

    return output;
}

}

#endif
