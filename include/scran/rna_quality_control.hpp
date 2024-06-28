#ifndef SCRAN_PER_CELL_RNA_QC_METRICS_HPP
#define SCRAN_PER_CELL_RNA_QC_METRICS_HPP

#include "../utils/macros.hpp"

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
void populate(Host_& host, Index_ n, MetricsBuffers<Sum_, Detected_, Proportion_>& res, BlockSource_ block) {
    constexpr bool unblocked = std::is_same<BlockSource_, bool>::value;
    auto buffer = [&]() {
        if constexpr(unblocked) {
            return std::vector<Float_>(n);
        } else {
            return scran::find_median_mad::Workspace(n, block);
        }
    }();

    if (res.sum) {
        scran::find_median_mad::Options mopts;
        mopts.log = true;
        auto mm = [&]() {
            if constexpr(unblocked) {
                std::copy_n(res.sum, n, buffer.data());
                return scran::find_median_mad::compute(n, buffer.data(), mopt);
            } else {
                return scran::find_median_mad::compute_blocked(n, res.sum, block, buffer, mopt);
            }
        }();

        scran::choose_filter_thresholds::Options copts;
        copts.num_mads = sum_num_mads;
        copts.unlog = true;
        copts.upper = false;
        output.sum = scran::choose_filter_thresholds::compute(std::move(mm), copts);
    }

    if (results.detected) {
        scran::find_median_mad::Options mopts;
        mopts.log = true;
        auto mm = [&]() {
            if constexpr(unblocked) {
                std::copy_n(res.detected, n, buffer.data());
                return scran::find_median_mad::compute(buffer.data(), mopt);
            } else {
                return scran::find_median_mad::compute(res.detected, block, work, mopt);
            }
        }();

        scran::choose_filter_thresholds::Options copts;
        copts.num_mads = detected_num_mads;
        copts.unlog = true;
        copts.upper = false;
        output.detected = scran::choose_filter_thresholds::compute(std::move(mm), copts);
    }

    size_t nsubsets = buffer.subset_proportion.size();
    output.subset_proportion.resize(nsubset);
    for (size_t s = 0; s < nsubsets; ++s) {
        auto sub = buffer.subset_proportion[s];
        if (sub) {
            scran::find_median_mad::Options mopts;
            auto mm = [&]() {
                if constexpr(unblocked) {
                    std::copy_n(sub, n, buffer.data());
                    return scran::find_median_mad::compute(n, buffer.data(), mopt);
                } else {
                    return scran::find_median_mad::compute(n, sub, block, work, mopt);
                }
            }();

            scran::choose_filter_thresholds::Options copts;
            copts.num_mads = subset_proportion_num_mads;
            copts.lower = false;
            output.subset_proportion = scran::choose_filter_thresholds::compute(std::move(mm), copts);
        }
    }
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
        compute(*this, n, results, options);
    }

    template<typename Index_, typename Sum_, typename Detected_, typename Proportion_>
    FilterThresholds(const MetricsResults<Sum_, Detected_, Proportion_>& results, const FiltersOptions& options) :
        FilterThresholds(internal::infer_size(results), internal::to_buffer(results), options) {}

public:
    /**
     * @return Thresholds to apply to the sums.
     */
    const choose_filter_thresholds::Thresholds<Float_>& sum() const {
        return my_sum;
    }

    /**
     * @return Thresholds to apply to the number of detected genes.
     */
    const choose_filter_thresholds::Thresholds<Float_>& get_detected() const {
        return my_detected;
    }

    /**
     * @return Thresholds to apply to the subset proportions.
     */
    const std::vector<choose_filter_thresholds::Thresholds<Float_> > >& get_subset_proportion() const {
        return my_subset_proportion;
    }

private:
    choose_filter_thresholds::Thresholds<Float_> my_sum;
    choose_filter_thresholds::Thresholds<Float_> my_detected;
    std::vector<choose_filter_thresholds::Thresholds<Float_> > > my_subset_proportion;

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

public:
    /**
     * @tparam overwrite Whether to overwrite existing truthy entries in `output`.
     * @tparam Float Floating point type for the metrics.
     * @tparam Integer Integer for the metrics.
     * @tparam Output Boolean type for the low-quality calls.
     *
     * @param n Number of cells.
     * @param[in] buffers Pointers to arrays of length `n`, containing the per-cell RNA-derived metrics.
     * These should be comparable to the values used to create this `Thresholds` object.
     * @param[out] output Pointer to an array of length `n`, to store the low-quality calls.
     * Values are set to `true` for low-quality cells.
     * If `overwrite = true`, values are set to `false` for high-quality cells, otherwise the existing entry is preserved.
     *
     * Use `filter_blocked()` instead for multi-block datasets. 
     */
    template<bool overwrite = true, typename Float, typename Integer, typename Output>
    void filter(size_t n, const PerCellRnaQcMetrics::Buffers<Float, Integer>& buffers, Output* output) const {
        if (detected.size() != 1) {
            throw std::runtime_error("should use filter_blocked() for multiple batches");
        }
        filter_<overwrite>(n, buffers, output, [](size_t i) -> size_t { return 0; });
    }

    /**
     * @overload
     *
     * @tparam Output Boolean type for the low-quality calls.
     *
     * @param metrics Collection of arrays of per-cell RNA metrics.
     * These should be comparable to the values used to create this `Thresholds` object.
     *
     * @return Vector of low-quality calls, of length equal to the number of cells in `metrics`.
     */
    template<typename Output = uint8_t>
    std::vector<Output> filter(const PerCellRnaQcMetrics::Results& metrics) const {
        std::vector<Output> output(metrics.detected.size());
        filter(output.size(), metrics.buffers(), output.data());
        return output;
    }

    public:
        /**
         * @tparam overwrite Whether to overwrite existing truthy entries in `output`.
         * @tparam Block Integer type for the block assignments.
         * @tparam Float Floating point type for the metrics.
         * @tparam Integer Integer for the metrics.
         * @tparam Output Boolean type for the low-quality calls.
         *
         * @param n Number of cells.
         * @param[in] block Pointer to an array of length `n`, containing the block assignment for each cell.
         * This may be `NULL`, in which case all cells are assumed to belong to the same block.
         * @param[in] buffers Pointers to arrays of length `n`, containing the per-cell RNA-derived metrics.
         * These should be comparable to the values used to create this `Thresholds` object.
         * @param[out] output Pointer to an array of length `n`, to store the low-quality calls.
         * Values are set to `true` for low-quality cells.
         * If `overwrite = true`, values are set to `false` for high-quality cells, otherwise the existing entry is preserved.
         */
        template<bool overwrite = true, typename Block, typename Float, typename Integer, typename Output>
        void filter_blocked(size_t n, const Block* block, const PerCellRnaQcMetrics::Buffers<Float, Integer>& buffers, Output* output) const {
            if (block) {
                filter_<overwrite>(n, buffers, output, [&](size_t i) -> Block { return block[i]; });
            } else {
                filter<overwrite>(n, buffers, output);
            }
        }

        /**
         * @overload
         *
         * @tparam Block Integer type for the block assignments.
         * @tparam Output Boolean type for the low-quality calls.
         *
         * @param metrics Collection of arrays of per-cell RNA metrics.
         * These should be comparable to the values used to create this `Thresholds` object.
         * @param[in] block Pointer to an array of length `n`, containing the block assignment for each cell.
         * This may be `NULL`, in which case all cells are assumed to belong to the same block.
         *
         * @return Vector of low-quality calls, of length equal to the number of cells in `metrics`.
         */
        template<typename Output = uint8_t, typename Block>
        std::vector<Output> filter_blocked(const PerCellRnaQcMetrics::Results& metrics, const Block* block) const {
            std::vector<Output> output(metrics.detected.size());
            filter_blocked(output.size(), block, metrics.buffers(), output.data());
            return output;
        }

    private:
        template<bool overwrite, typename Float, typename Integer, typename Output, typename Function>
        void filter_(size_t n, const PerCellRnaQcMetrics::Buffers<Float, Integer>& buffers, Output* output, Function indexer) const {
            size_t nsubsets = subset_proportions.size();

            for (size_t i = 0; i < n; ++i) {
                auto b = indexer(i);
                if (quality_control::is_less_than(buffers.sums[i], sums[b])) {
                    output[i] = true;
                    continue;
                }

                if (quality_control::is_less_than<double>(buffers.detected[i], detected[b])) {
                    output[i] = true;
                    continue;
                }

                bool fail = false;
                for (size_t s = 0; s < nsubsets; ++s) {
                    if (quality_control::is_greater_than(buffers.subset_proportions[s][i], subset_proportions[s][b])) {
                        fail = true;
                        break;
                    }
                }
                if (fail) {
                    output[i] = true;
                    continue;
                }

                if constexpr(overwrite) {
                    output[i] = false;
                }
            }

            return;
        }
    };

public:
    /**
     * @tparam Float Floating point type for the metrics.
     * @tparam Integer Integer for the metrics.
     *
     * @param n Number of cells.
     * @param[in] buffers Pointers to arrays of length `n`, containing the per-cell RNA-derived metrics.
     *
     * @return Filtering thresholds for each metric.
     */
    template<typename Float, typename Integer>
    Thresholds run(size_t n, const PerCellRnaQcMetrics::Buffers<Float, Integer>& buffers) const {
        return run_blocked(n, static_cast<int*>(NULL), buffers);
    }

    /**
     * @overload
     * @param metrics Collection of arrays of length equal to the number of cells, containing the per-cell RNA-derived metrics.
     *
     * @return Filtering thresholds for each metric.
     */
    Thresholds run(const PerCellRnaQcMetrics::Results& metrics) const {
        return run(metrics.detected.size(), metrics.buffers());
    }

public:
    /**
     * @tparam Block Integer type for the block assignments.
     * @tparam Float Floating point type for the metrics.
     * @tparam Integer Integer for the metrics.
     *
     * @param n Number of cells.
     * @param[in] block Pointer to an array of length `n`, containing the block assignments for each cell.
     * This may be `NULL`, in which case all cells are assumed to belong to the same block.
     * @param[in] buffers Pointers to arrays of length `n`, containing the per-cell RNA-derived metrics.
     *
     * @return Filtering thresholds for each metric in each block.
     */
    template<typename Block, typename Float, typename Integer>
    Thresholds run_blocked(size_t n, const Block* block, const PerCellRnaQcMetrics::Buffers<Float, Integer>& buffers) const {
        Thresholds output;
        std::vector<double> workspace(n);
        std::vector<int> starts;
        if (block) {
            starts = ComputeMedianMad::compute_block_starts(n, block);
        }

        // Filtering on the total counts.
        {
            ComputeMedianMad meddler;
            meddler.set_log(true);
            auto sums_res = meddler.run_blocked(n, block, starts, buffers.sums, workspace.data());

            ChooseOutlierFilters filter;
            filter.set_num_mads(sums_num_mads);
            filter.set_upper(false);
            auto sums_filt = filter.run(std::move(sums_res));

            output.sums = std::move(sums_filt.lower);
        }

        // Filtering on the detected features.
        {
            ComputeMedianMad meddler;
            meddler.set_log(true);
            auto detected_res = meddler.run_blocked(n, block, starts, buffers.detected, workspace.data());

            ChooseOutlierFilters filter;
            filter.set_num_mads(sums_num_mads);
            filter.set_upper(false);
            auto detected_filt = filter.run(std::move(detected_res));

            output.detected = std::move(detected_filt.lower);
        }

        // Filtering on the subset proportions (no log). 
        {
            ComputeMedianMad meddler;

            ChooseOutlierFilters filter;
            filter.set_num_mads(subset_num_mads);
            filter.set_lower(false);

            size_t nsubsets = buffers.subset_proportions.size();
            for (size_t s = 0; s < nsubsets; ++s) {
                auto subset_res = meddler.run_blocked(n, block, starts, buffers.subset_proportions[s], workspace.data());
                auto subset_filt = filter.run(std::move(subset_res));
                output.subset_proportions.push_back(std::move(subset_filt.upper));
            }
        }

        return output;
    }

    /**
     * @overload
     * @tparam Block Integer type for the block assignments.
     *
     * @param metrics Collection of arrays of length equal to the number of cells, containing the per-cell RNA-derived metrics.
     * @param[in] block Pointer to an array of length equal to the number of cells, containing the block assignments for each cell.
     * This may be `NULL`, in which case all cells are assumed to belong to the same block.
     *
     * @return Filtering thresholds for each metric in each block.
     */
    template<typename Block>
    Thresholds run_blocked(const PerCellRnaQcMetrics::Results& metrics, const Block* block) const {
        return run_blocked(metrics.detected.size(), block, metrics.buffers());
    }
};



}

}

#endif
