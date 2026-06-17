#include "scran_tests/scran_tests.hpp"

#include "scran_qc/choose_filter_thresholds.hpp"

static const std::vector<double> metrics = { 
    0.761164335, 0.347582428, 0.430822695, 0.888530395, 0.627701241, 0.678912751,
    0.097218816, 0.692552865, 0.143479605, 0.049907948, 0.648174966, 0.848563147,
    0.472604294, 0.022525487, 0.738983761, 0.915699533, 0.577269375, 0.799325422,
    0.554883985, 0.009624974, 0.215816610
};

TEST(ChooseFilterThresholds, Simple) {
    std::vector<double> buffer(metrics.size());
    const double med = 0.5772694;
    const double mad = 0.3292203;

    // Manual check.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_FLOAT_EQ(thresholds.lower, med - 3 * mad);
        EXPECT_FLOAT_EQ(thresholds.upper, med + 3 * mad);
    }

    // Turns off on request.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.lower = false;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_TRUE(std::isinf(thresholds.lower));
        EXPECT_TRUE(thresholds.lower < 0);
        EXPECT_FLOAT_EQ(thresholds.upper, med + 3 * mad);
    }

    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.upper = false;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_FLOAT_EQ(thresholds.lower, med - 3 * mad);
        EXPECT_TRUE(std::isinf(thresholds.upper));
        EXPECT_TRUE(thresholds.upper > 0);
    }

    // Respects the minimum difference.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.min_diff = 100;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_FLOAT_EQ(thresholds.lower, med - 100);
        EXPECT_FLOAT_EQ(thresholds.upper, med + 100);
    }
}

TEST(ChooseFilterThresholds, Logged) {
    std::vector<double> buffer(metrics.size());
    const double lmed = -0.5494463;
    const double lmad = 0.4825257;

    // Manual check.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.log = true;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_FLOAT_EQ(thresholds.lower, std::exp(lmed - 3 * lmad));
        EXPECT_FLOAT_EQ(thresholds.upper, std::exp(lmed + 3 * lmad));
    }

    // With minimum difference.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.log = true;
        opt.min_diff = 2;
        auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), buffer.data(), opt);
        EXPECT_FLOAT_EQ(thresholds.lower, std::exp(lmed - 2.0));
        EXPECT_FLOAT_EQ(thresholds.upper, std::exp(lmed + 2.0));
    }
}

TEST(ChooseFilterThresholds, LoggedZeros) {
    scran_qc::ChooseFilterThresholdsOptions opt;
    opt.log = true;

    // Handles log-transformation of all-zeroes correctly.
    {
        std::vector<double> values(21);
        std::vector<double> buffer(values.size());
        auto thresholds = scran_qc::choose_filter_thresholds(values.size(), values.data(), buffer.data(), opt);
        EXPECT_EQ(thresholds.lower, 0);
        EXPECT_EQ(thresholds.upper, 0);
    }

    // A bit trickier when median is zero and MAD is infinite.
    {
        auto copy = metrics;
        copy.insert(copy.end(), metrics.size(), 0); // equal numbers of zeros and non-zeros.
        std::vector<double> buffer(copy.size());
        auto thresholds = scran_qc::choose_filter_thresholds(copy.size(), copy.data(), buffer.data(), opt);
        EXPECT_EQ(thresholds.lower, 0);
        EXPECT_EQ(thresholds.upper, std::numeric_limits<double>::infinity());
    }

    // Otherwise it treats log(0) as just a large negative number.
    {
        auto copy1 = metrics;
        copy1.push_back(0.0000001);
        auto copy2 = metrics;
        copy2.push_back(0);

        auto thresholds1 = scran_qc::choose_filter_thresholds(copy1.size(), copy1.data(), copy1.data(), opt);
        auto thresholds2 = scran_qc::choose_filter_thresholds(copy2.size(), copy2.data(), copy2.data(), opt);

        EXPECT_EQ(thresholds1.lower, thresholds2.lower);
        EXPECT_EQ(thresholds1.upper, thresholds2.upper);
    }
}

TEST(ChooseFilterThresholds, RemoveNans) {
    scran_qc::ChooseFilterThresholdsOptions opt;

    // One at the end.
    {
        auto copy = metrics;
        auto ref = scran_qc::choose_filter_thresholds(copy.size(), copy.data(), copy.data(), opt);

        copy = metrics;
        copy.push_back(std::numeric_limits<double>::quiet_NaN());
        auto withnan = scran_qc::choose_filter_thresholds(copy.size(), copy.data(), copy.data(), opt);
        EXPECT_EQ(ref.lower, withnan.lower);
        EXPECT_EQ(ref.upper, withnan.upper);
    }

    // Two at the start.
    {
        auto copy = metrics;
        copy[0] = std::numeric_limits<double>::quiet_NaN();
        copy[1] = std::numeric_limits<double>::quiet_NaN();
        auto withnan = scran_qc::choose_filter_thresholds(copy.size(), copy.data(), copy.data(), opt);

        copy = metrics;
        auto ref = scran_qc::choose_filter_thresholds(copy.size() - 2, copy.data() + 2, copy.data() + 2, opt);
        EXPECT_EQ(ref.lower, withnan.lower);
        EXPECT_EQ(ref.upper, withnan.upper);
    }
}

TEST(ChooseFilterThresholds, EdgeCases) {
    const auto inf = std::numeric_limits<double>::infinity();
    scran_qc::ChooseFilterThresholdsOptions opt;

    {
        auto empty = scran_qc::choose_filter_thresholds<double, double>(0, NULL, NULL, opt);
        EXPECT_EQ(empty.lower, -inf);
        EXPECT_EQ(empty.upper, inf);
    }

    // The median is NaN.
    {
        std::vector<double> metrics { -inf, inf };
        auto empty = scran_qc::choose_filter_thresholds<double, double>(metrics.size(), metrics.data(), metrics.data(), opt);
        EXPECT_EQ(empty.lower, -inf);
        EXPECT_EQ(empty.upper, inf);
    }

    // Median is -inf and MAD is +inf.
    {
        std::vector<double> metrics { 0, 0, -inf, -inf };
        auto empty = scran_qc::choose_filter_thresholds<double, double>(metrics.size(), metrics.data(), metrics.data(), opt);
        EXPECT_EQ(empty.lower, -inf);
        EXPECT_EQ(empty.upper, inf);
    }

    // Median is +inf and MAD is +inf.
    {
        std::vector<double> metrics { 0, 0, inf, inf };
        auto empty = scran_qc::choose_filter_thresholds<double, double>(metrics.size(), metrics.data(), metrics.data(), opt);
        EXPECT_EQ(empty.lower, -inf);
        EXPECT_EQ(empty.upper, inf);
    }
}

TEST(ChooseFilterThresholds, Blocked) {
    std::vector<int> block {
        0, 1, 2, 3, 
        1, 0, 2, 3,
        1, 2, 0, 3,
        1, 2, 3, 0,
        0, 0, 0, 0, 
        1
    };
    ASSERT_EQ(block.size(), metrics.size());
    const int num_blocks = 4;

    scran_qc::ChooseFilterThresholdsOptions opt;
    scran_qc::ChooseFilterThresholdsBlockedWorkspace<double> work(metrics.size(), block.data(), num_blocks);
    auto isres = scran_qc::choose_filter_thresholds_blocked(metrics.size(), metrics.data(), block.data(), num_blocks, work, opt);
    EXPECT_EQ(isres.size(), num_blocks);

    std::vector<double> buffer(metrics.size());
    std::vector<double> copy;
    for (int i = 0; i < num_blocks; ++i) {
        copy.clear();
        for (size_t j = 0; j < block.size(); ++j) {
            if (block[j] == i) {
                copy.push_back(metrics[j]);
            }
        }

        auto is2res = scran_qc::choose_filter_thresholds(copy.size(), copy.data(), copy.data(), opt);
        EXPECT_EQ(isres[i].lower, is2res.lower);
        EXPECT_EQ(isres[i].upper, is2res.upper);
    }

    // Respects empty blocks.
    {
        auto blockcopy = block;
        for (auto& b : blockcopy) {
            ++b;
        }
        const int new_num_blocks = num_blocks + 2;

        scran_qc::ChooseFilterThresholdsBlockedWorkspace<double> work(metrics.size(), blockcopy.data(), new_num_blocks);
        auto empty = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), blockcopy.data(), new_num_blocks, work, opt);

        EXPECT_TRUE(std::isinf(empty[0].lower));
        EXPECT_TRUE(std::isinf(empty[0].upper));
        for (int i = 0; i < num_blocks; ++i) {
            EXPECT_EQ(empty[i + 1].lower, isres[i].lower);
            EXPECT_EQ(empty[i + 1].upper, isres[i].upper);
        }
        EXPECT_TRUE(std::isinf(empty[new_num_blocks - 1].lower));
        EXPECT_TRUE(std::isinf(empty[new_num_blocks - 1].upper));
    }
}

TEST(ChooseFilterThresholds, WorkspaceReuse) {
    std::vector<int> block { 
        0, 1, 1, 2,
        2, 1, 1, 1,
        2, 2, 1, 1,
        2, 2, 0, 0,
        0, 0, 2, 0,
        2
    };
    ASSERT_EQ(block.size(), metrics.size());
    const int num_blocks = 3;

    scran_qc::ChooseFilterThresholdsOptions opt;
    scran_qc::ChooseFilterThresholdsBlockedWorkspace<double> work(block.size(), block.data(), num_blocks);
    auto isres = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), block.data(), num_blocks, work, opt);
    auto isres2 = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), block.data(), num_blocks, work, opt);

    ASSERT_EQ(isres.size(), isres2.size());
    for (size_t i = 0; i < isres.size(); ++i) {
        EXPECT_EQ(isres[i].lower, isres2[i].lower);
        EXPECT_EQ(isres[i].upper, isres2[i].upper);
    }

    // Now trying to reuse the same workspace but with different blocking.
    std::reverse(block.begin(), block.end());
    scran_qc::ChooseFilterThresholdsBlockedWorkspace<double> fresh(block.size(), block.data(), num_blocks);
    auto ref = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), block.data(), num_blocks, fresh, opt);
    scran_qc::reset_choose_filter_thresholds_blocked_workspace(work, block.size(), block.data(), num_blocks);
    isres = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), block.data(), num_blocks, work, opt);
    isres2 = scran_qc::choose_filter_thresholds_blocked<double>(metrics.size(), metrics.data(), block.data(), num_blocks, work, opt);

    ASSERT_EQ(ref.size(), isres.size());
    ASSERT_EQ(ref.size(), isres2.size());
    for (size_t i = 0; i < isres.size(); ++i) {
        EXPECT_EQ(ref[i].lower, isres[i].lower);
        EXPECT_EQ(ref[i].upper, isres[i].upper);
        EXPECT_EQ(ref[i].lower, isres2[i].lower);
        EXPECT_EQ(ref[i].upper, isres2[i].upper);
    }
}

TEST(ChooseFilterThresholds, DifferentType) {
    std::vector<int> foobar { 1, 2, 4, 3, 6, 9, 8, 7 };
    std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };

    scran_qc::ChooseFilterThresholdsBlockedWorkspace<float> work(foobar.size(), block.data(), 2);
    auto out = scran_qc::choose_filter_thresholds_blocked(foobar.size(), foobar.data(), block.data(), 2, work, scran_qc::ChooseFilterThresholdsOptions());

    const float med0 = 2.5, mad0 = 1.4826;
    const float med1 = 7.5, mad1 = 1.4826;
    EXPECT_FLOAT_EQ(out[0].lower, med0 - 3 * mad0);
    EXPECT_FLOAT_EQ(out[0].upper, med0 + 3 * mad0);
    EXPECT_FLOAT_EQ(out[1].lower, med1 - 3 * mad0);
    EXPECT_FLOAT_EQ(out[1].upper, med1 + 3 * mad1);
}
