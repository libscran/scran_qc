#include <gtest/gtest.h>

#include "find_median_mad.hpp"

static std::vector<double> even_values = { 
    0.761164335, 0.347582428, 0.430822695, 0.888530395, 0.627701241, 0.678912751,
    0.097218816, 0.692552865, 0.143479605, 0.049907948, 0.648174966, 0.848563147,
    0.472604294, 0.022525487, 0.738983761, 0.915699533, 0.577269375, 0.799325422,
    0.554883985, 0.009624974, 0.215816610
};

TEST(FindMedianMad, BasicTests) {
    auto copy = even_values;

    scran::find_median_mad::Options opt;
    auto stats = scran::find_median_mad::compute(copy.size(), copy.data(), opt);
    EXPECT_FLOAT_EQ(stats.first, 0.5772693750);
    EXPECT_FLOAT_EQ(stats.second, 0.3292202953);

    // Checking for odd.
    copy = even_values;
    auto stats2 = scran::find_median_mad::compute(copy.size() - 1, copy.data(), opt);
    EXPECT_FLOAT_EQ(stats2.first, 0.6024853080);
    EXPECT_FLOAT_EQ(stats2.second, 0.2731710715);
}

TEST(FindMedianMad, EdgeTests) {
    scran::find_median_mad::Options opt;

    std::vector<double> empty;
    auto stats = scran::find_median_mad::compute(empty.size(), empty.data(), opt);
    EXPECT_TRUE(std::isnan(stats.first));
    EXPECT_TRUE(std::isnan(stats.second));

    empty.push_back(std::numeric_limits<double>::infinity());
    stats = scran::find_median_mad::compute(empty.size(), empty.data(), opt);
    EXPECT_TRUE(std::isinf(stats.first));
    EXPECT_TRUE(stats.first > 0);
    EXPECT_EQ(stats.second, 0);
    
    empty.push_back(-std::numeric_limits<double>::infinity());
    stats = scran::find_median_mad::compute(empty.size(), empty.data(), opt);
    EXPECT_TRUE(std::isnan(stats.first));
    EXPECT_TRUE(std::isnan(stats.second));

    empty.push_back(-std::numeric_limits<double>::infinity()); // add twice to keep it even.
    empty.push_back(-std::numeric_limits<double>::infinity());
    stats = scran::find_median_mad::compute(empty.size(), empty.data(), opt);
    EXPECT_TRUE(std::isinf(stats.first));
    EXPECT_TRUE(stats.first < 0);
    EXPECT_EQ(stats.second, 0);
}

TEST(FindMedianMad, MissingFilter) {
    scran::find_median_mad::Options opt;

    // One at the end.
    {
        auto copy = even_values;
        auto ref = scran::find_median_mad::compute(copy.size(), copy.data(), opt);

        copy = even_values;
        copy.push_back(std::numeric_limits<double>::quiet_NaN());
        auto withnan = scran::find_median_mad::compute(copy.size(), copy.data(), opt);
        EXPECT_EQ(ref, withnan);
    }

    // Two at the start.
    {
        auto copy = even_values;
        copy[0] = std::numeric_limits<double>::quiet_NaN();
        copy[1] = std::numeric_limits<double>::quiet_NaN();
        auto withnan = scran::find_median_mad::compute(copy.size(), copy.data(), opt);

        copy = even_values;
        auto ref = scran::find_median_mad::compute(copy.size() - 2, copy.data() + 2, opt);
        EXPECT_EQ(ref, withnan);
    }
}

TEST(FindMedianMad, MedianOnly) {
    scran::find_median_mad::Options opt;
    opt.median_only = true;

    auto copy = even_values;
    auto stats = scran::find_median_mad::compute(copy.size(), copy.data(), opt);

    EXPECT_FLOAT_EQ(stats.first, 0.5772693750);
    EXPECT_TRUE(std::isnan(stats.second));
}

TEST(FindMedianMad, LogTests) {
    scran::find_median_mad::Options opt;
    opt.log = true;

    auto copy = even_values;
    auto isres = scran::find_median_mad::compute(copy.size(), copy.data(), opt);
    EXPECT_FLOAT_EQ(isres.first, -0.5494462670);
    EXPECT_FLOAT_EQ(isres.second, 0.4825257172);

    // Handles occasional zero values.
    {
        auto copy1 = even_values;
        copy1.push_back(0.0000001);
        auto copy2 = even_values;
        copy2.push_back(0);

        auto isres1 = scran::find_median_mad::compute(copy1.size(), copy1.data(), opt);
        auto isres2 = scran::find_median_mad::compute(copy2.size(), copy2.data(), opt);
        EXPECT_EQ(isres1, isres2);
    }

    // Does something sensible with loads of zeroes.
    {
        std::vector<double> empty(even_values.size());
        auto isres = scran::find_median_mad::compute(empty.size(), empty.data(), opt);
        EXPECT_TRUE(std::isinf(isres.first));
        EXPECT_TRUE(isres.first < 0);
        EXPECT_EQ(isres.second, 0);
    }
}

TEST(FindMedianMad, BlockTests) {
    std::vector<int> block {
        0, 1, 2, 3, 
        1, 0, 2, 3,
        1, 2, 0, 3,
        1, 2, 3, 0,
        0, 0, 0, 0, 
        1
    };

    scran::find_median_mad::Options opt;
    auto isres = scran::find_median_mad::compute_blocked(even_values.size(), block.data(), even_values.data(), opt);
    EXPECT_EQ(isres.size(), 4);

    for (int i = 0; i < 4; ++i) {
        std::vector<double> copy;
        for (size_t j = 0; j < block.size(); ++j) {
            if (block[j] == i) {
                copy.push_back(even_values[j]);
            }
        }

        auto is2res = scran::find_median_mad::compute(copy.size(), copy.data(), opt);
        EXPECT_EQ(isres[i], is2res);
    }

    // NULL blocked falls back to single-batch processing.
    {
        auto copy = even_values;
        auto ref = scran::find_median_mad::compute(copy.size(), copy.data(), opt);
        auto isres_none = scran::find_median_mad::compute_blocked(even_values.size(), static_cast<int*>(NULL), even_values.data(), opt);
        EXPECT_EQ(isres_none.size(), 1);
        EXPECT_EQ(ref, isres_none[0]);
    }
}

TEST(FindMedianMad, WorkspaceReuse) {
    std::vector<int> block { 
        0, 1, 1, 2,
        2, 1, 1, 1,
        2, 2, 1, 1,
        2, 2, 0, 0,
        0, 0, 2, 0,
        2
    };

    scran::find_median_mad::Options opt;
    scran::find_median_mad::Workspace<double, int> work(block.size(), block.data());
    auto isres = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), work, opt);
    auto isres2 = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), work, opt);
    EXPECT_EQ(isres, isres2);

    // Now trying to reuse the same workspace but with different blocking.
    std::reverse(block.begin(), block.end());
    auto ref = scran::find_median_mad::compute_blocked(even_values.size(), block.data(), even_values.data(), opt);
    work.set(block.size(), block.data());
    isres = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), work, opt);
    isres2 = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), work, opt);
    EXPECT_EQ(isres, isres2);
}

TEST(FindMedianMad, DifferentType) {
    std::vector<int> foobar { 1, 2, 4, 3, 6, 9, 8, 7 };
    std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };
    auto out = scran::find_median_mad::compute_blocked<float>(foobar.size(), block.data(), foobar.data(), scran::find_median_mad::Options());
    EXPECT_FLOAT_EQ(out[0].first, 2.5);
    EXPECT_FLOAT_EQ(out[0].second, 1.4826);
    EXPECT_FLOAT_EQ(out[1].first, 7.5);
    EXPECT_FLOAT_EQ(out[1].second, 1.4826);
}
