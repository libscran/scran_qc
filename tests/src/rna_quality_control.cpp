#include <gtest/gtest.h>

#include "simulate_vector.h"
#include "compare_almost_equal.h"
#include "tatami/tatami.hpp"
#include "rna_quality_control.hpp"

class RnaQualityControlMetricsTest : public ::testing::Test {
protected:
    inline static std::shared_ptr<tatami::NumericMatrix> mat;
    inline static std::vector<std::vector<size_t> > subs;

    static void SetUpTestSuite() {
        size_t nr = 211, nc = 99;
        mat.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, simulate_sparse_vector<double>(nr * nc, 0.05, /* lower = */ 1, /* upper = */ 10, /* seed = */ 1111)));
        subs.push_back({ 0, 5, 7, 8, 9, 10, 16, 17, 101, 159, 202 });
    }
};

TEST_F(RnaQualityControlMetricsTest, NoSubset) {
    scran::rna_quality_control::MetricsOptions opts;
    auto res = scran::rna_quality_control::compute_metrics(mat.get(), {}, opts);
    EXPECT_EQ(res.sum, tatami_stats::sums::by_column(mat.get()));

    auto nonzeros = tatami_stats::counts::zero::by_column(mat.get());
    for (auto& nz : nonzeros) {
        nz = mat->nrow() - nz;
    }
    EXPECT_EQ(res.detected, nonzeros);

    EXPECT_TRUE(res.subset_proportion.empty());
}

TEST_F(RnaQualityControlMetricsTest, OneSubset) {
    scran::rna_quality_control::MetricsOptions opts;
    auto res = scran::rna_quality_control::compute_metrics(mat.get(), subs, opts);

    auto submat = tatami::make_DelayedSubset(mat, subs[0], true);
    auto subsums = tatami_stats::sums::by_column(submat.get());
    auto it = res.sum.begin();
    for (auto& s : subsums) {
        s /= *it;
        ++it;
    }

    EXPECT_EQ(res.subset_proportion[0], subsums);
}

TEST_F(RnaQualityControlMetricsTest, DifferentType) {
    // Checking that the placeholder subset arrays are correctly allocated.
    scran::rna_quality_control::MetricsOptions opts;
    auto ref = scran::rna_quality_control::compute_metrics(mat.get(), subs, opts);
    auto res = scran::rna_quality_control::compute_metrics<double, int, float>(mat.get(), subs, opts);

    std::vector<float> copy(ref.subset_proportion[0].begin(), ref.subset_proportion[0].end());
    compare_almost_equal(copy, res.subset_proportion[0], /* tol = */ 1e-5);
}

TEST_F(RnaQualityControlMetricsTest, NaSubset) {
    size_t nc = 50;
    std::vector<double> nothing(mat->nrow() * nc);
    tatami::DenseColumnMatrix<double, int> empty(mat->nrow(), nc, std::move(nothing));

    scran::rna_quality_control::MetricsOptions opts;
    opts.num_threads = 2; // just for some coverage.
    auto res = scran::rna_quality_control::compute_metrics(&empty, subs, opts);

    EXPECT_EQ(res.sum, std::vector<double>(nc));
    EXPECT_EQ(res.detected, std::vector<int>(nc));
    EXPECT_TRUE(std::isnan(res.subset_proportion[0][0]));
    EXPECT_TRUE(std::isnan(res.subset_proportion[0][nc-1]));
}

TEST(RnaQualityControlFilters, Basic) {
    scran::rna_quality_control::MetricsResults<double, int, double> results;
    results.sum = std::vector<double> { 0, 100, 200, 300, 400, 500 };
    results.detected = std::vector<int> { 50, 0, 200, 300, 400, 500 };
    results.subset_proportion.push_back({ 0.1, 0.2, 1, 0.05, 0.1, 0.15 });
    results.subset_proportion.push_back({ 0.1, 0.2, 0.05, 1, 0.1, 0.15 });

    scran::rna_quality_control::FiltersOptions opt;
    scran::rna_quality_control::Filters<double> thresholds(results, opt);
    EXPECT_TRUE(thresholds.get_sum() > 0);
    EXPECT_TRUE(thresholds.get_detected() > 0);
    EXPECT_TRUE(thresholds.get_subset_proportion()[0] < 1);
    EXPECT_TRUE(thresholds.get_subset_proportion()[1] < 1);

    auto keep = thresholds.filter(results);
    std::vector<uint8_t> expected { 0, 0, 0, 0, 1, 1 };
    EXPECT_EQ(expected, keep);
}

TEST(RnaQualityControlFilters, Blocked) {
    {
        scran::rna_quality_control::MetricsResults<double, int, double> results;
        results.sum = std::vector<double> { 0, 100, 200, 300, 400, 500 };
        results.detected = std::vector<int> { 50, 0, 200, 300, 400, 500 };
        results.subset_proportion.push_back({ 0.1, 0.2, 1, 0.05, 0.1, 0.15 });
        results.subset_proportion.push_back({ 0.1, 0.2, 0.05, 1, 0.1, 0.15 });

        scran::rna_quality_control::FiltersOptions opt;
        scran::rna_quality_control::Filters<double> thresholds(results, opt);

        std::vector<int> block(6);
        scran::rna_quality_control::BlockedFilters<double> bthresholds(results, block.data(), opt);
        EXPECT_EQ(thresholds.get_sum(), bthresholds.get_sum()[0]);
        EXPECT_EQ(thresholds.get_detected(), bthresholds.get_detected()[0]);
        EXPECT_EQ(thresholds.get_subset_proportion()[0], bthresholds.get_subset_proportion()[0][0]);
        EXPECT_EQ(thresholds.get_subset_proportion()[1], bthresholds.get_subset_proportion()[1][0]);

        auto keep = bthresholds.filter(results, block.data());
        std::vector<uint8_t> expected { 0, 0, 0, 0, 1, 1 };
        EXPECT_EQ(expected, keep);
    }

    {
        scran::rna_quality_control::MetricsResults<double, int, double> results;
        results.sum = std::vector<double> { 0, 1900, 2000, 2100, 4, 5, 6, 7000 };
        results.detected = std::vector<int> { 1500, 0, 2000, 2500, 4000, 5, 6, 7 };
        results.subset_proportion.push_back({ 0.1, 0.2, 1, 0.05, 0.9, 0.95, 0.8, 0.1 });
        results.subset_proportion.push_back({ 0.1, 0.2, 0.05, 1, 0.99, 0.85, 0.94, 1.0 });

        scran::rna_quality_control::FiltersOptions opt;
        std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };
        scran::rna_quality_control::BlockedFilters<double> bthresholds(results, block.data(), opt);

        EXPECT_GT(bthresholds.get_sum()[0], 100);
        EXPECT_GT(bthresholds.get_detected()[0], 100);
        EXPECT_LT(bthresholds.get_subset_proportion()[0][0], 1);
        EXPECT_LT(bthresholds.get_subset_proportion()[1][0], 1);

        EXPECT_LT(bthresholds.get_sum()[1], 100);
        EXPECT_LT(bthresholds.get_detected()[1], 100);
        EXPECT_GT(bthresholds.get_subset_proportion()[0][1], 1);
        EXPECT_GT(bthresholds.get_subset_proportion()[1][1], 1);

        std::cout << bthresholds.get_sum()[1] << std::endl;
        std::cout << bthresholds.get_detected()[1] << std::endl;
        std::cout << bthresholds.get_subset_proportion()[0][1] << std::endl;
        std::cout << bthresholds.get_subset_proportion()[1][1] << std::endl;

        auto keep = bthresholds.filter(results, block.data());
        std::vector<uint8_t> expected { 0, 0, 0, 0, 1, 1, 1, 1 };
        EXPECT_EQ(expected, keep);
    }
}
