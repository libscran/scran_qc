#include <gtest/gtest.h>

#include "choose_filter_thresholds.hpp"

TEST(ChooseFilterThresholds, Simple) {
    scran::find_median_mad::Results<double> mm(1, 0.2);
    std::vector<double> metrics { 0, 1.0, 1.7, std::numeric_limits<double>::quiet_NaN() };

    // Manual check.
    {
        scran::choose_filter_thresholds::Options opt;
        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.get_lower(), 0.4);
        EXPECT_DOUBLE_EQ(thresholds.get_upper(), 1.6);

        std::vector<uint8_t> expected { 0, 1, 0, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data()), expected);
    }

    // Turns off on request.
    {
        scran::choose_filter_thresholds::Options opt;
        opt.lower = false;

        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.get_lower()));
        EXPECT_TRUE(thresholds.get_lower() < 0);
        EXPECT_DOUBLE_EQ(thresholds.get_upper(), 1.6);

        std::vector<uint8_t> expected { 1, 1, 0, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data()), expected);
    }

    {
        scran::choose_filter_thresholds::Options opt;
        opt.upper = false;

        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.get_lower(), 0.4);
        EXPECT_TRUE(std::isinf(thresholds.get_upper()));
        EXPECT_TRUE(thresholds.get_upper() > 0);

        std::vector<uint8_t> expected { 0, 1, 1, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data()), expected);
    }

    // Respects the minimum difference.
    {
        scran::choose_filter_thresholds::Options opt;
        opt.min_diff = 100;
        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.get_lower(), -99);
        EXPECT_DOUBLE_EQ(thresholds.get_upper(), 101);
    }
}

TEST(ChooseFilterThresholds, Logged) {
    scran::find_median_mad::Results<double> mm(1, 0.2);

    // Manual check.
    {
        scran::choose_filter_thresholds::Options opt;
        opt.unlog = true;

        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.get_lower(), std::exp(0.4));
        EXPECT_DOUBLE_EQ(thresholds.get_upper(), std::exp(1.6));
    }

    // With minimum difference.
    {
        scran::choose_filter_thresholds::Options opt;
        opt.unlog = true;
        opt.min_diff = 1;

        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_DOUBLE_EQ(thresholds.get_lower(), std::exp(0));
        EXPECT_DOUBLE_EQ(thresholds.get_upper(), std::exp(2.0));
    }

    // Handles -Inf correctly.
    {
        scran::find_median_mad::Results<double> mm2(-std::numeric_limits<double>::infinity(), 0);
        scran::choose_filter_thresholds::Options opt;
        opt.unlog = true;

        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm2, opt);
        EXPECT_EQ(thresholds.get_lower(), 0);
        EXPECT_EQ(thresholds.get_upper(), 0);
    }
}

TEST(ChooseFilterThresholds, EdgeCases) {
    // If the median and MAD are NaN, effectively no filtering is done.
    {
        constexpr double nan = std::numeric_limits<double>::quiet_NaN();
        scran::find_median_mad::Results<double> mm(nan, nan);
        scran::choose_filter_thresholds::Options opt;
        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.get_lower()));
        EXPECT_TRUE(thresholds.get_lower() < 0);
        EXPECT_TRUE(std::isinf(thresholds.get_upper()));
        EXPECT_TRUE(thresholds.get_upper() > 0);
    }

    // Checking that it handles Inf inputs.
    {
        scran::find_median_mad::Results<double> mm(std::numeric_limits<double>::infinity(), 0);
        scran::choose_filter_thresholds::Options opt;
        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        EXPECT_TRUE(std::isinf(thresholds.get_lower()));
        EXPECT_TRUE(thresholds.get_lower() > 0);
        EXPECT_TRUE(std::isinf(thresholds.get_upper()));
        EXPECT_TRUE(thresholds.get_upper() > 0);
    }
}

TEST(ChooseFilterThresholds, Blocked) {
    std::vector<scran::find_median_mad::Results<double> > mms;
    mms.emplace_back(1, 0.2);
    mms.emplace_back(2, 0.1);

    scran::choose_filter_thresholds::Options opt;
    scran::choose_filter_thresholds::BlockThresholds<double> thresholds(mms, opt);

    EXPECT_DOUBLE_EQ(thresholds.get_lower()[0], 0.4);
    EXPECT_DOUBLE_EQ(thresholds.get_upper()[0], 1.6);
    EXPECT_DOUBLE_EQ(thresholds.get_lower()[1], 1.7);
    EXPECT_DOUBLE_EQ(thresholds.get_upper()[1], 2.3);

    std::vector<double> metrics { 0, 0.5, 1.0, 1.5, 1.8, 2.1, 3.0, std::numeric_limits<double>::quiet_NaN() };

    {
        std::vector<int> block(metrics.size(), 0);
        std::vector<uint8_t> expected { 0, 1, 1, 1, 0, 0, 0, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data(), block.data()), expected);
    }

    {
        std::vector<int> block(metrics.size(), 1);
        std::vector<uint8_t> expected { 0, 0, 0, 0, 1, 1, 0, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data(), block.data()), expected);
    }

    {
        std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };
        std::vector<uint8_t> expected { 0, 1, 1, 1, 1, 1, 0, 0 };
        EXPECT_EQ(thresholds.filter(metrics.size(), metrics.data(), block.data()), expected);
    }
}

TEST(ChooseFilterThresholds, Overwrite) {
    // Unblocked.
    {
        scran::find_median_mad::Results<double> mm(1, 0.2);
        std::vector<double> metrics { 0, 1.0, 1.7, std::numeric_limits<double>::quiet_NaN() };

        scran::choose_filter_thresholds::Options opt;
        scran::choose_filter_thresholds::Thresholds<double> thresholds(mm, opt);
        auto expected = thresholds.filter(metrics.size(), metrics.data());

        std::vector<uint8_t> buffer(metrics.size(), 1);
        thresholds.filter(metrics.size(), metrics.data(), buffer.data(), false);
        EXPECT_EQ(buffer, expected);

        std::fill_n(buffer.data(), buffer.size(), 0);
        std::fill_n(expected.data(), expected.size(), 0);
        thresholds.filter(metrics.size(), metrics.data(), buffer.data(), false);
        EXPECT_EQ(buffer, expected);
    }

    // Blocked.
    {
        std::vector<scran::find_median_mad::Results<double> > mm;
        mm.emplace_back(1, 0.2);
        std::vector<double> metrics { 0, 1.0, 1.7, std::numeric_limits<double>::quiet_NaN() };

        scran::choose_filter_thresholds::Options opt;
        scran::choose_filter_thresholds::BlockThresholds<double> thresholds(mm, opt);
        std::vector<int> block { 0, 0, 0, 0 };
        auto expected = thresholds.filter(metrics.size(), metrics.data(), block.data());

        std::vector<uint8_t> buffer(metrics.size(), 1);
        thresholds.filter(metrics.size(), metrics.data(), block.data(), buffer.data(), false);
        EXPECT_EQ(buffer, expected);

        std::fill_n(buffer.data(), buffer.size(), 0);
        std::fill_n(expected.data(), expected.size(), 0);
        thresholds.filter(metrics.size(), metrics.data(), block.data(), buffer.data(), false);
        EXPECT_EQ(buffer, expected);
    }
}
