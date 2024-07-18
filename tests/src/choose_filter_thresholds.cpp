#include <gtest/gtest.h>

#include "scran_qc/choose_filter_thresholds.hpp"

TEST(ChooseFilterThresholds, Simple) {
    scran_qc::FindMedianMadResults<double> mm(1, 0.2);

    // Manual check.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.lower, 0.4);
        EXPECT_DOUBLE_EQ(thresholds.upper, 1.6);
    }

    // Turns off on request.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.lower = false;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.lower));
        EXPECT_TRUE(thresholds.lower < 0);
        EXPECT_DOUBLE_EQ(thresholds.upper, 1.6);
    }

    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.upper = false;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.lower, 0.4);
        EXPECT_TRUE(std::isinf(thresholds.upper));
        EXPECT_TRUE(thresholds.upper > 0);
    }

    // Respects the minimum difference.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.min_diff = 100;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.lower, -99);
        EXPECT_DOUBLE_EQ(thresholds.upper, 101);
    }
}

TEST(ChooseFilterThresholds, Logged) {
    scran_qc::FindMedianMadResults<double> mm(1, 0.2);

    // Manual check.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.log = true;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.lower, std::exp(0.4));
        EXPECT_DOUBLE_EQ(thresholds.upper, std::exp(1.6));
    }

    // With minimum difference.
    {
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.log = true;
        opt.min_diff = 1;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.lower, std::exp(0));
        EXPECT_DOUBLE_EQ(thresholds.upper, std::exp(2.0));
    }

    // Handles -Inf correctly.
    {
        scran_qc::FindMedianMadResults<double> mm2(-std::numeric_limits<double>::infinity(), 0);
        scran_qc::ChooseFilterThresholdsOptions opt;
        opt.log = true;
        auto thresholds = scran_qc::choose_filter_thresholds(mm2, opt);
        EXPECT_EQ(thresholds.lower, 0);
        EXPECT_EQ(thresholds.upper, 0);
    }
}

TEST(ChooseFilterThresholds, EdgeCases) {
    // If the median and MAD are NaN, effectively no filtering is done.
    {
        constexpr double nan = std::numeric_limits<double>::quiet_NaN();
        scran_qc::FindMedianMadResults<double> mm(nan, nan);
        scran_qc::ChooseFilterThresholdsOptions opt;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.lower));
        EXPECT_TRUE(thresholds.lower < 0);
        EXPECT_TRUE(std::isinf(thresholds.upper));
        EXPECT_TRUE(thresholds.upper > 0);
    }

    // Checking that it handles Inf inputs.
    {
        scran_qc::FindMedianMadResults<double> mm(std::numeric_limits<double>::infinity(), 0);
        scran_qc::ChooseFilterThresholdsOptions opt;
        auto thresholds = scran_qc::choose_filter_thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.lower));
        EXPECT_TRUE(thresholds.lower > 0);
        EXPECT_TRUE(std::isinf(thresholds.upper));
        EXPECT_TRUE(thresholds.upper > 0);
    }
}

TEST(ChooseFilterThresholds, Blocked) {
    std::vector<scran_qc::FindMedianMadResults<double> > mms;
    mms.emplace_back(1, 0.2);
    mms.emplace_back(2, 0.1);

    scran_qc::ChooseFilterThresholdsOptions opt;
    auto bthresholds = scran_qc::choose_filter_thresholds_blocked(mms, opt);

    EXPECT_DOUBLE_EQ(bthresholds[0].lower, 0.4);
    EXPECT_DOUBLE_EQ(bthresholds[0].upper, 1.6);
    EXPECT_DOUBLE_EQ(bthresholds[1].lower, 1.7);
    EXPECT_DOUBLE_EQ(bthresholds[1].upper, 2.3);
}

TEST(ChooseFilterThresholds, FromMetrics) {
    scran_qc::ChooseFilterThresholdsOptions opt;

    std::vector<double> metrics { 0, 1, 2, 3, 4, 5 };
    auto thresholds = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), static_cast<double*>(NULL), opt);
    EXPECT_DOUBLE_EQ(thresholds.lower, 2.5 - 2.2239 * 3);
    EXPECT_DOUBLE_EQ(thresholds.upper, 2.5 + 2.2239 * 3);

    auto thresholds2 = scran_qc::choose_filter_thresholds(metrics.size(), metrics.data(), opt);
    EXPECT_EQ(thresholds.lower, thresholds2.lower);
    EXPECT_EQ(thresholds.upper, thresholds2.upper);

    std::vector<int> imetrics { 0, 1, 2, 3, 4, 5 };
    std::vector<int> block { 0, 0, 0, 1, 1, 1 };
    auto bthresholds = scran_qc::choose_filter_thresholds_blocked<int, int, int, double>(imetrics.size(), imetrics.data(), block.data(), NULL, opt);
    ASSERT_EQ(bthresholds.size(), 2);
    EXPECT_DOUBLE_EQ(bthresholds[0].lower, 1 - 1.4826 * 3);
    EXPECT_DOUBLE_EQ(bthresholds[0].upper, 1 + 1.4826 * 3);
    EXPECT_DOUBLE_EQ(bthresholds[1].lower, 4 - 1.4826 * 3);
    EXPECT_DOUBLE_EQ(bthresholds[1].upper, 4 + 1.4826 * 3);
}
