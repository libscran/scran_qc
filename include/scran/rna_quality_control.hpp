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
 * Compute the QC metrics from an input matrix and return the results.
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

}

}

#endif
