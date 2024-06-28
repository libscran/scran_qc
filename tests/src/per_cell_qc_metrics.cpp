#include <gtest/gtest.h>

#include "simulate_vector.h"
#include "compare_almost_equal.h"

#include "tatami/tatami.hpp"

#include "per_cell_qc_metrics.hpp"

#include <cmath>

class PerCellQcMetricsTestStandard : public ::testing::TestWithParam<int> {
protected:
    inline static std::shared_ptr<tatami::NumericMatrix> dense_row, dense_column, sparse_row, sparse_column;

    static void SetUpTestSuite() {
        size_t nr = 100, nc = 50;
        auto vec = simulate_sparse_vector(nr * nc, 0.1, /* lower = */ 1, /* upper = */ 100);
        dense_row.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(vec)));
        dense_column = tatami::convert_to_dense(dense_row.get(), false);
        sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);
    }

public:
    template<bool exact_ = false, class Result_>
    static void compare(const Result_& ref, const Result_& other) {
        if constexpr(exact_) {
            EXPECT_EQ(ref.sum, other.sum);
        } else {
            compare_almost_equal(ref.sum, other.sum);
        }
        EXPECT_EQ(ref.detected, other.detected);
        EXPECT_EQ(ref.max_value, other.max_value);
        EXPECT_EQ(ref.max_index, other.max_index);

        ASSERT_EQ(ref.subset_sum.size(), other.subset_sum.size());
        for (size_t i = 0; i < ref.subset_sum.size(); ++i) {
            if constexpr(exact_) {
                EXPECT_EQ(ref.subset_sum[i], other.subset_sum[i]);
            } else {
                compare_almost_equal(ref.subset_sum[i], other.subset_sum[i]);
            }
        }

        ASSERT_EQ(ref.subset_detected.size(), other.subset_detected.size());
        for (size_t i = 0; i < ref.subset_detected.size(); ++i) {
            EXPECT_EQ(ref.subset_detected[i], other.subset_detected[i]);
        }
    }

    template<typename Value_, typename Index_>
    static std::vector<int> compute_num_detected(const tatami::Matrix<Value_, Index_>* mat) {
        auto detected = tatami_stats::counts::zero::by_column(mat);
        for (auto& z : detected) {
            z = mat->nrow() - z;
        }
        return detected;
    }
};

TEST_P(PerCellQcMetricsTestStandard, NoSubset) {
    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);

    int threads = GetParam();
    opt.num_threads = threads;

    if (threads > 1) {
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        compare<true>(ref, res1);
    } else {
        EXPECT_EQ(ref.sum, tatami_stats::sums::by_column(dense_row.get()));
        EXPECT_EQ(ref.detected, compute_num_detected(dense_row.get()));
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
    compare(ref, res2);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    compare(ref, res3);

    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    compare(ref, res3);
}

TEST_P(PerCellQcMetricsTestStandard, OneSubset) {
    std::vector<std::vector<int> > subs;
    subs.push_back({ 0, 5, 7, 8, 9, 10, 16, 17 });

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), subs, opt);

    int threads = GetParam();
    opt.num_threads = threads;

    if (threads == 1) {
        auto sub = tatami::make_DelayedSubset(dense_row, subs[0], true);
        EXPECT_EQ(ref.subset_sum[0], tatami_stats::sums::by_column(sub.get()));
        EXPECT_EQ(ref.subset_detected[0], compute_num_detected(sub.get()));
    } else {
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), subs, opt);
        compare<true>(ref, res1);
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), subs, opt);
    compare(ref, res2);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), subs, opt);
    compare(ref, res3);

    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), subs, opt);
    compare(ref, res4);

    // Checking with boolified subsets.
    std::vector<std::vector<uint8_t> > bool_sub_raw;
    bool_sub_raw.emplace_back(dense_row->nrow());
    for (auto s : subs.front()) {
        bool_sub_raw[0][s] = 1;
    }
    std::vector<uint8_t*> bool_sub{ bool_sub_raw.front().data() };

    auto bres1 = scran::per_cell_qc_metrics::compute(dense_row.get(), bool_sub, opt);
    compare<true>(ref, bres1);

    auto bres2 = scran::per_cell_qc_metrics::compute(dense_column.get(), bool_sub, opt);
    compare(ref, bres2);

    auto bres3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), bool_sub, opt);
    compare(ref, bres3);

    auto bres4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), bool_sub, opt);
    compare(ref, bres4);
}

TEST_P(PerCellQcMetricsTestStandard, TwoSubsets) {
    std::vector<std::vector<int> > subs;
    subs.push_back({ 0, 5, 7, 8, 9, 10, 16, 17 });
    subs.push_back({ 1, 2, 5, 6, 8, 11, 18, 19});

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), subs, opt);

    int threads = GetParam();
    opt.num_threads = threads;

    if (threads == 1) {
        auto ref1 = tatami::make_DelayedSubset<0>(dense_row, subs[0]);
        auto refprop1 = tatami_stats::sums::by_column(ref1.get());
        EXPECT_EQ(refprop1, ref.subset_sum[0]);
        EXPECT_EQ(ref.subset_detected[0], compute_num_detected(ref1.get()));

        auto ref2 = tatami::make_DelayedSubset<0>(dense_row, subs[1]);
        auto refprop2 = tatami_stats::sums::by_column(ref2.get());
        EXPECT_EQ(refprop2, ref.subset_sum[1]); 
        EXPECT_EQ(ref.subset_detected[1], compute_num_detected(ref2.get()));
    } else {
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), subs, opt);
        compare<true>(ref, res1);
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), subs, opt);
    compare(ref, res2);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), subs, opt);
    compare(ref, res3);

    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), subs, opt);
    compare(ref, res4);

    // Checking with boolified subsets.
    std::vector<std::vector<uint8_t> > bool_sub_raw;
    bool_sub_raw.emplace_back(dense_row->nrow());
    for (auto s : subs.front()) {
        bool_sub_raw[0][s] = 1;
    }
    bool_sub_raw.emplace_back(dense_row->nrow());
    for (auto s : subs.back()) {
        bool_sub_raw[1][s] = 1;
    }
    std::vector<uint8_t*> bool_sub{ bool_sub_raw.front().data(), bool_sub_raw.back().data() };

    auto bres1 = scran::per_cell_qc_metrics::compute(dense_row.get(), bool_sub, opt);
    compare<true>(ref, bres1);

    auto bres2 = scran::per_cell_qc_metrics::compute(dense_column.get(), bool_sub, opt);
    compare(ref, bres2);

    auto bres3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), bool_sub, opt);
    compare(ref, bres3);

    auto bres4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), bool_sub, opt);
    compare(ref, bres4);
}

INSTANTIATE_TEST_SUITE_P(
    PerCellQcMetrics,
    PerCellQcMetricsTestStandard,
    ::testing::Values(1, 3) // number of threads
);

class PerCellQcMetricsTestMaxed : public ::testing::TestWithParam<int> {
protected:
    std::shared_ptr<tatami::NumericMatrix> dense_row, dense_column, sparse_row, sparse_column;

    void propagate() {
        dense_column = tatami::convert_to_dense(dense_row.get(), false);
        sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);
    }
};

TEST_P(PerCellQcMetricsTestMaxed, SparseNegatives) {
    int threads = GetParam();

    {
        size_t nr = 99, nc = 47;
        auto vec = simulate_sparse_vector(nr * nc, /* density = */ 0.1, /* lower = */ -2, /* upper = */ -1, /* seed = */ 123 * threads);
        dense_row.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(vec)));
        propagate();
    }

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);

    if (threads == 1) { 
        // max should be all-zeros.
        std::vector<double> expected(dense_row->ncol());
        EXPECT_EQ(ref.max_value, expected);
    } else {
        opt.num_threads = threads;
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        EXPECT_EQ(res1.max_value, ref.max_value);
        EXPECT_EQ(res1.max_index, ref.max_index);
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res2.max_value, ref.max_value);
    EXPECT_EQ(res2.max_index, ref.max_index);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res3.max_value, ref.max_value);
    EXPECT_EQ(res3.max_index, ref.max_index);
    
    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res4.max_value, ref.max_value);
    EXPECT_EQ(res4.max_index, ref.max_index);
}

TEST_P(PerCellQcMetricsTestMaxed, DenseNegatives) {
    int threads = GetParam();

    {
        size_t nr = 20, nc = 100;
        auto vec = simulate_vector(nr * nc, /* lower = */ -5, /* upper = */ -1, /* seed = */ 123 * threads);
        dense_row.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(vec)));
        propagate();
    }

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);

    opt.num_threads = threads;

    if (threads == 1) {
        bool max_neg = true;
        for (auto x : ref.max_value) {
            if (x >= 0) {
                max_neg = false;
                break;
            }
        }
        EXPECT_TRUE(max_neg);
    } else {
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        EXPECT_EQ(res1.max_value, ref.max_value);
        EXPECT_EQ(res1.max_index, ref.max_index);
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res2.max_value, ref.max_value);
    EXPECT_EQ(res2.max_index, ref.max_index);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res3.max_value, ref.max_value);
    EXPECT_EQ(res3.max_index, ref.max_index);
    
    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res4.max_value, ref.max_value);
    EXPECT_EQ(res4.max_index, ref.max_index);
}

TEST_P(PerCellQcMetricsTestMaxed, AllZeros) {
    int threads = GetParam();

    {
        size_t nr = 12, nc = 45;
        std::vector<double> empty(nr * nc);
        dense_row.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(empty)));
        propagate();
    }

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);

    opt.num_threads = threads;

    if (threads == 1) { 
        // max should be all-zeros.
        std::vector<double> expected(dense_row->ncol());
        EXPECT_EQ(ref.max_value, expected);
        std::vector<int> indices(dense_row->ncol());
        EXPECT_EQ(ref.max_index, indices);
    } else {
        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        EXPECT_EQ(res1.max_value, ref.max_value);
        EXPECT_EQ(res1.max_index, ref.max_index);
    }

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res2.max_value, ref.max_value);
    EXPECT_EQ(res2.max_index, ref.max_index);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res3.max_value, ref.max_value);
    EXPECT_EQ(res3.max_index, ref.max_index);
    
    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res4.max_value, ref.max_value);
    EXPECT_EQ(res4.max_index, ref.max_index);
}

TEST_P(PerCellQcMetricsTestMaxed, StructuralZeros) {
    int threads = GetParam();

    {
        // Fewer rows, so we're more likely to get a row where the maximum
        // is determined by one of the structural zeros.
        size_t nr = 9, nc = 1001;

        std::vector<int> i;
        std::vector<double> x;
        std::vector<size_t> p(1);
        std::mt19937_64 rng(98712 * threads);
        std::uniform_real_distribution<> structural(0.0, 1.0);

        for (size_t c = 0; c < nc; ++c) {
            for (size_t r = 0; r < nr; ++r) {
                if (structural(rng) < 0.2) {
                    i.push_back(r);
                    auto choice = structural(rng);
                    if (choice < 0.3) {
                        x.push_back(-1);
                    } else if (choice < 0.7) {
                        x.push_back(0); // spiking in structural values that are actually non-zero.
                    } else {
                        x.push_back(1);
                    }
                }
            }
            p.push_back(i.size());
        }

        // Tests are only relevant to sparse matrices here. Conversion to row-major
        // doesn't discard structural zeros when convert_to_compressed_sparse is used.
        sparse_column.reset(new tatami::CompressedSparseColumnMatrix<double, int>(nr, nc, std::move(x), std::move(i), std::move(p)));
        sparse_row = tatami::convert_to_compressed_sparse(sparse_column.get(), true);

        dense_row = tatami::convert_to_dense(sparse_column.get(), true);
    }

    scran::per_cell_qc_metrics::Options opt;
    auto ref = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);

    opt.num_threads = threads;
    auto res1 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res1.max_value, ref.max_value);
    EXPECT_EQ(res1.max_index, ref.max_index);

    auto res2 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res2.max_value, ref.max_value);
    EXPECT_EQ(res2.max_index, ref.max_index);
}

TEST_P(PerCellQcMetricsTestMaxed, OkayOnMissing) {
    auto threads = GetParam();

    {
        size_t nr = 20, nc = 100;
        auto vec = simulate_sparse_vector(nr * nc, 0.1, /* lower = */ -5, /* upper = */ -1, /* seed = */ 42 * threads);
        dense_row.reset(new tatami::DenseRowMatrix<double, int>(nr, nc, std::move(vec)));
        propagate();
    }

    {
        scran::per_cell_qc_metrics::Options opt;
        opt.compute_max_value = false;
        opt.num_threads = threads;

        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res1.max_value.empty());

        auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res2.max_value.empty());
        EXPECT_EQ(res2.max_index, res1.max_index);

        auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res3.max_value.empty());
        EXPECT_EQ(res3.max_index, res1.max_index);
        
        auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res4.max_value.empty());
        EXPECT_EQ(res4.max_index, res1.max_index);
    }

    {
        scran::per_cell_qc_metrics::Options opt;
        opt.compute_max_index = false;
        opt.num_threads = threads;

        auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res1.max_index.empty());

        auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res2.max_index.empty());
        EXPECT_EQ(res2.max_value, res1.max_value);

        auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res3.max_index.empty());
        EXPECT_EQ(res3.max_value, res1.max_value);
        
        auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
        EXPECT_TRUE(res4.max_index.empty());
        EXPECT_EQ(res4.max_value, res1.max_value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    PerCellQcMetrics,
    PerCellQcMetricsTestMaxed,
    ::testing::Values(1, 3) // number of threads
);

TEST(PerCellQcMetrics, Empty) {
    size_t nr = 0, nc = 50;
    scran::per_cell_qc_metrics::Options opt;

    auto dense_row = std::make_shared<tatami::DenseRowMatrix<double, int> >(nr, nc, std::vector<double>{});
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<double> expected(nc);
    auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res1.max_value, expected);

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res2.max_value, expected);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res3.max_value, expected);

    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), std::vector<char*>{}, opt);
    EXPECT_EQ(res4.max_value, expected);
}

TEST(PerCellQcMetrics, NewType) {
    std::vector<std::vector<int> > subs;
    subs.push_back({ 0, 5, 7, 8, 9, 10, 16, 17 });
    subs.push_back({ 1, 2, 5, 6, 8, 11, 18, 19});

    size_t nr = 200, nc = 60;
    auto vec = simulate_sparse_vector(nr * nc, 0.1, /* lower = */ 1, /* upper = */ 100);
    for (auto& v : vec) {
        v = std::round(v);
    }
    auto ref = std::make_shared<tatami::DenseRowMatrix<double, int> >(nr, nc, vec);

    std::vector<uint8_t> uvec(vec.begin(), vec.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<uint8_t, uint8_t> >(nr, nc, std::move(uvec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    scran::per_cell_qc_metrics::Options opt;
    auto refres = scran::per_cell_qc_metrics::compute(ref.get(), subs, opt);
    auto compare = [&](const auto& other) {
        EXPECT_EQ(refres.sum, other.sum);
        EXPECT_EQ(refres.detected, other.detected);
        EXPECT_EQ(refres.max_value, std::vector<double>(other.max_value.begin(), other.max_value.end()));
        EXPECT_EQ(refres.max_index, std::vector<int>(other.max_index.begin(), other.max_index.end()));

        ASSERT_EQ(refres.subset_sum.size(), other.subset_sum.size());
        for (size_t i = 0; i < refres.subset_sum.size(); ++i) {
            EXPECT_EQ(refres.subset_sum[i], other.subset_sum[i]);
        }

        ASSERT_EQ(refres.subset_detected.size(), other.subset_detected.size());
        for (size_t i = 0; i < refres.subset_detected.size(); ++i) {
            EXPECT_EQ(refres.subset_detected[i], other.subset_detected[i]);
        }
    };

    auto res1 = scran::per_cell_qc_metrics::compute(dense_row.get(), subs, opt);
    compare(res1);

    auto res2 = scran::per_cell_qc_metrics::compute(dense_column.get(), subs, opt);
    compare(res2);

    auto res3 = scran::per_cell_qc_metrics::compute(sparse_row.get(), subs, opt);
    compare(res3);

    auto res4 = scran::per_cell_qc_metrics::compute(sparse_column.get(), subs, opt);
    compare(res4);
}

TEST(PerCellQcMetrics, Disabled) {
    size_t nr = 100, nc = 50;
    auto vec = simulate_sparse_vector(nr * nc, 0.1, /* lower = */ 1, /* upper = */ 100, /* seed */ 42);
    tatami::DenseRowMatrix<double, int> mat(nr, nc, std::move(vec));

    // Fully disabled.
    {
        scran::per_cell_qc_metrics::Options opt;
        opt.compute_sum = false;
        opt.compute_detected = false;
        opt.compute_max_value = false;
        opt.compute_max_index = false;
        opt.compute_subset_sum = false;
        opt.compute_subset_detected = false;

        std::vector<std::vector<int> > subs;
        subs.push_back({ 0, 1, 2, 3 });

        auto res = scran::per_cell_qc_metrics::compute(&mat, subs, opt);
        EXPECT_TRUE(res.sum.empty());
        EXPECT_TRUE(res.detected.empty());
        EXPECT_TRUE(res.max_value.empty());
        EXPECT_TRUE(res.max_index.empty());

        EXPECT_TRUE(res.subset_sum.empty());
        EXPECT_TRUE(res.subset_detected.empty());
    }

    // Subsets present but otherwise disabled.
    {
        std::vector<std::vector<int> > subs;
        subs.push_back({ 0, 1, 2, 3 });

        scran::per_cell_qc_metrics::Buffers<double, int, double, int> buffers;
        buffers.subset_sum.resize(1, NULL);
        buffers.subset_detected.resize(1, NULL);

        scran::per_cell_qc_metrics::Options opt;
        scran::per_cell_qc_metrics::compute(&mat, subs, buffers, opt);

        EXPECT_TRUE(buffers.subset_sum[0] == NULL);
        EXPECT_TRUE(buffers.subset_detected[0] == NULL);
    }
}

TEST(PerCellQcMetrics, DirtyBufferFilling) {
    size_t nr = 100, nc = 50;
    auto vec = simulate_sparse_vector(nr * nc, 0.1, /* lower = */ 1, /* upper = */ 100, /* seed */ 69);
    tatami::DenseRowMatrix<double, int> mat(nr, nc, std::move(vec));

    scran::per_cell_qc_metrics::Results<double, int, double, int> output;
    scran::per_cell_qc_metrics::Buffers<double, int, double, int> buffers;

    // Prefilling each vector with a little bit of nonsense.
    {
        output.sum.resize(nc, 99);
        buffers.sum = output.sum.data();

        output.detected.resize(nc, 111);
        buffers.detected = output.detected.data();

        output.max_index.resize(nc, 91);
        buffers.max_index = output.max_index.data();

        output.max_value.resize(nc, 23214);
        buffers.max_value = output.max_value.data();

        size_t nsubsets = 1;
        output.subset_sum.resize(nsubsets);
        buffers.subset_sum.resize(nsubsets);
        output.subset_detected.resize(nsubsets);
        buffers.subset_detected.resize(nsubsets);

        for (size_t s = 0; s < nsubsets; ++s) {
            output.subset_sum[s].resize(nc, 99);
            buffers.subset_sum[s] = output.subset_sum[s].data();
            output.subset_detected[s].resize(nc, -100);
            buffers.subset_detected[s] = output.subset_detected[s].data();
        }
    }

    std::vector<std::vector<int> > subs;
    subs.push_back({ 1, 5, 7, 9, 11 });

    scran::per_cell_qc_metrics::Options opt;
    scran::per_cell_qc_metrics::compute(&mat, subs, buffers, opt);
    auto ref = scran::per_cell_qc_metrics::compute(&mat, subs, opt);
    PerCellQcMetricsTestStandard::compare<true>(ref, output);
}
