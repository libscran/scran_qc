#include <gtest/gtest.h>

#include "scran_tests/scran_tests.hpp"
#include "tatami/tatami.hpp"

#include "scran_qc/crispr_quality_control.hpp"

class CrisprQualityControlMetricsTest : public ::testing::Test {
protected:
    inline static std::shared_ptr<tatami::NumericMatrix> mat;

    static void SetUpTestSuite() {
        size_t nr = 11, nc = 199;
        auto vec = scran_tests::simulate_vector(nr * nc, []{ 
            scran_tests::SimulationParameters sparams;
            sparams.density = 0.05;
            sparams.lower = 1;
            sparams.upper = 10;
            sparams.seed = 333;
            return sparams;
        }());
        mat.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(vec)));
    }
};

TEST_F(CrisprQualityControlMetricsTest, Basic) {
    scran_qc::ComputeCrisprQcMetricsOptions opts;
    auto res = scran_qc::compute_crispr_qc_metrics(*mat, opts);
    EXPECT_EQ(res.sum, tatami_stats::sums::by_column(mat.get()));

    auto nonzeros = tatami_stats::counts::zero::by_column(mat.get());
    for (auto& nz : nonzeros) {
        nz = mat->nrow() - nz;
    }
    EXPECT_EQ(res.detected, nonzeros);

    auto maxed = tatami_stats::ranges::by_column(mat.get());
    EXPECT_EQ(res.max_value, maxed.second);

    size_t NR = mat->nrow(), NC = mat->ncol();
    std::vector<int> best(NC);
    auto ext = mat->dense_column();
    std::vector<double> buffer(NR);
    for (size_t c = 0; c < NC; ++c) {
        auto ptr = ext->fetch(c, buffer.data());
        auto last = ptr[0];
        for (size_t r = 1; r < NR; ++r) {
            if (ptr[r] > last) {
                best[c] = r;
                last = ptr[r];
            }
        }
    }
    EXPECT_EQ(best, res.max_index);
}

TEST(CrisprQualityControlFilters, Basic) {
    scran_qc::ComputeCrisprQcMetricsResults<int, int, int, int> results;

    // First 10 have all-zero proportions, next 10 have all-1 proportions.
    // Only the second half should survive the proportion filter that is 
    // used to estimate the max count threshold. 
    results.sum.resize(20, 100);
    std::iota(results.sum.begin() + 10, results.sum.end(), 100);
    results.max_value.resize(20);
    std::iota(results.max_value.begin() + 10, results.max_value.end(), 100);

    scran_qc::ComputeCrisprQcFiltersOptions opt;
    auto thresholds = scran_qc::compute_crispr_qc_filters(results, opt);
    EXPECT_GT(thresholds.get_max_value(), 0);
    EXPECT_LT(thresholds.get_max_value(), 100);

    auto keep = thresholds.filter(results);
    std::vector<uint8_t> expected(20);
    std::fill(expected.begin() + 10, expected.end(), 1);
    EXPECT_EQ(expected, keep);

    // Double-checking that only the second half is used.
    scran_qc::FindMedianMadOptions fopt;
    fopt.log = true;
    auto ref = scran_qc::find_median_mad<double>(10, results.max_value.data() + 10, NULL, fopt);
    scran_tests::compare_almost_equal(thresholds.get_max_value(), std::exp(ref.median - 3 * ref.mad));
}

TEST(CrisprQualityControlFilters, Blocked) {
    {
        scran_qc::ComputeCrisprQcMetricsResults<> results;
        results.sum = std::vector<double> { 100, 101, 102, 103, 104, 105 };
        results.max_value = std::vector<double> { 0, 90, 91, 92, 93, 94 };

        scran_qc::ComputeCrisprQcFiltersOptions opt;
        auto thresholds = scran_qc::compute_crispr_qc_filters(results, opt);

        // Checking what happens when no filtering is performed.
        std::vector<int> block(6);
        auto bthresholds = scran_qc::compute_crispr_qc_filters_blocked(results, block.data(), opt);
        EXPECT_EQ(thresholds.get_max_value(), bthresholds.get_max_value()[0]);

        auto keep = bthresholds.filter(results, block.data());
        std::vector<uint8_t> expected { 0, 1, 1, 1, 1, 1 };
        EXPECT_EQ(expected, keep);

        // Same result with a NULL pointer.
        auto nthresholds = scran_qc::compute_crispr_qc_filters_blocked(results, static_cast<int*>(NULL), opt);
        EXPECT_EQ(thresholds.get_max_value(), nthresholds.get_max_value()[0]);

        auto nkeep = nthresholds.filter(results, static_cast<int*>(NULL));
        EXPECT_EQ(expected, nkeep);
    }

    {
        scran_qc::ComputeCrisprQcMetricsResults<double, int, double> results;
        results.sum = std::vector<double> { 1000, 1010, 1020, 1030, 1040, 1050, 1060, 1070 };
        results.max_value = std::vector<double> { 0, 90, 91, 92, 0, 900, 910, 920 };

        scran_qc::ComputeCrisprQcFiltersOptions opt;
        std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };
        auto bthresholds = scran_qc::compute_crispr_qc_filters_blocked(results, block.data(), opt);

        EXPECT_LT(bthresholds.get_max_value()[0], 90);
        EXPECT_GT(bthresholds.get_max_value()[0], 50);
        EXPECT_LT(bthresholds.get_max_value()[1], 900);
        EXPECT_GT(bthresholds.get_max_value()[1], 500);

        auto keep = bthresholds.filter(results, block.data());
        std::vector<uint8_t> expected { 0, 1, 1, 1, 0, 1, 1, 1 };
        EXPECT_EQ(expected, keep);
    }
}
