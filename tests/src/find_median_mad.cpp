#include <gtest/gtest.h>

#include "find_median_mad.hpp"

static const std::vector<double> even_values = { 
    0.761164335, 0.347582428, 0.430822695, 0.888530395, 0.627701241, 0.678912751,
    0.097218816, 0.692552865, 0.143479605, 0.049907948, 0.648174966, 0.848563147,
    0.472604294, 0.022525487, 0.738983761, 0.915699533, 0.577269375, 0.799325422,
    0.554883985, 0.009624974, 0.215816610
};

TEST(FindMedianMad, BasicTests) {
    scran::find_median_mad::Options opt;
    auto stats = scran::find_median_mad::compute<double>(even_values.size(), even_values.data(), NULL, opt);
    EXPECT_FLOAT_EQ(stats.median, 0.5772693750);
    EXPECT_FLOAT_EQ(stats.mad, 0.3292202953);

    // Checking for odd.
    auto stats2 = scran::find_median_mad::compute<double>(even_values.size() - 1, even_values.data(), NULL, opt);
    EXPECT_FLOAT_EQ(stats2.median, 0.6024853080);
    EXPECT_FLOAT_EQ(stats2.mad, 0.2731710715);
}

TEST(FindMedianMad, EdgeTests) {
    scran::find_median_mad::Options opt;

    auto stats = scran::find_median_mad::compute<double, int, double>(0, NULL, NULL, opt);
    EXPECT_TRUE(std::isnan(stats.median));
    EXPECT_TRUE(std::isnan(stats.mad));

    std::vector<double> extremes{ std::numeric_limits<double>::infinity() };
    stats = scran::find_median_mad::compute<double>(extremes.size(), extremes.data(), NULL, opt);
    EXPECT_TRUE(std::isinf(stats.median));
    EXPECT_TRUE(stats.median > 0);
    EXPECT_EQ(stats.mad, 0);

    extremes.push_back(-std::numeric_limits<double>::infinity());
    stats = scran::find_median_mad::compute<double>(extremes.size(), extremes.data(), NULL, opt);
    EXPECT_TRUE(std::isnan(stats.median));
    EXPECT_TRUE(std::isnan(stats.mad));

    extremes.push_back(-std::numeric_limits<double>::infinity()); // add twice to keep it even.
    extremes.push_back(-std::numeric_limits<double>::infinity());
    stats = scran::find_median_mad::compute<double>(extremes.size(), extremes.data(), NULL, opt);
    EXPECT_TRUE(std::isinf(stats.median));
    EXPECT_TRUE(stats.median < 0);
    EXPECT_EQ(stats.mad, 0);
}

TEST(FindMedianMad, MissingFilter) {
    scran::find_median_mad::Options opt;

    // One at the end.
    {
        auto ref = scran::find_median_mad::compute<double>(even_values.size(), even_values.data(), NULL, opt);

        auto copy = even_values;
        copy.push_back(std::numeric_limits<double>::quiet_NaN());
        auto withnan = scran::find_median_mad::compute<double>(copy.size(), copy.data(), NULL, opt);
        EXPECT_EQ(ref.median, withnan.median);
        EXPECT_EQ(ref.mad, withnan.mad);
    }

    // Two at the start.
    {
        auto copy = even_values;
        copy[0] = std::numeric_limits<double>::quiet_NaN();
        copy[1] = std::numeric_limits<double>::quiet_NaN();
        auto withnan = scran::find_median_mad::compute<double>(copy.size(), copy.data(), NULL, opt);

        auto ref = scran::find_median_mad::compute<double>(copy.size() - 2, copy.data() + 2, NULL, opt);
        EXPECT_EQ(ref.median, withnan.median);
        EXPECT_EQ(ref.mad, withnan.mad);
    }
}

TEST(FindMedianMad, MedianOnly) {
    scran::find_median_mad::Options opt;
    opt.median_only = true;

    auto copy = even_values;
    auto stats = scran::find_median_mad::compute<double>(copy.size(), copy.data(), NULL, opt);

    EXPECT_FLOAT_EQ(stats.median, 0.5772693750);
    EXPECT_TRUE(std::isnan(stats.mad));
}

TEST(FindMedianMad, LogTests) {
    scran::find_median_mad::Options opt;
    opt.log = true;

    auto copy = even_values;
    auto isres = scran::find_median_mad::compute<double>(copy.size(), copy.data(), NULL, opt);
    EXPECT_FLOAT_EQ(isres.median, -0.5494462670);
    EXPECT_FLOAT_EQ(isres.mad, 0.4825257172);

    // Handles occasional zero values.
    {
        auto copy1 = even_values;
        copy1.push_back(0.0000001);
        auto copy2 = even_values;
        copy2.push_back(0);

        auto isres1 = scran::find_median_mad::compute<double>(copy1.size(), copy1.data(), NULL, opt);
        auto isres2 = scran::find_median_mad::compute<double>(copy2.size(), copy2.data(), NULL, opt);
        EXPECT_EQ(isres1.median, isres2.median);
        EXPECT_EQ(isres1.mad, isres2.mad);
    }

    // Does something sensible with loads of zeroes.
    {
        std::vector<double> empty(even_values.size());
        auto isres = scran::find_median_mad::compute<double>(empty.size(), empty.data(), NULL, opt);
        EXPECT_TRUE(std::isinf(isres.median));
        EXPECT_TRUE(isres.median < 0);
        EXPECT_EQ(isres.mad, 0);
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
    auto isres = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), NULL, opt);
    EXPECT_EQ(isres.size(), 4);

    std::vector<double> buffer(even_values.size());
    std::vector<double> copy;
    for (int i = 0; i < 4; ++i) {
        copy.clear();
        for (size_t j = 0; j < block.size(); ++j) {
            if (block[j] == i) {
                copy.push_back(even_values[j]);
            }
        }

        auto is2res = scran::find_median_mad::compute(copy.size(), copy.data(), buffer.data(), opt);
        EXPECT_EQ(isres[i].median, is2res.median);
        EXPECT_EQ(isres[i].mad, is2res.mad);
    }

    // NULL blocked falls back to single-batch processing.
    {
        auto ref = scran::find_median_mad::compute<double>(even_values.size(), even_values.data(), NULL, opt);
        auto isres_none = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), static_cast<int*>(NULL), even_values.data(), NULL, opt);
        EXPECT_EQ(isres_none.size(), 1);
        EXPECT_EQ(isres_none[0].median, ref.median);
        EXPECT_EQ(isres_none[0].mad, ref.mad);
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
    auto isres = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), &work, opt);
    auto isres2 = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), &work, opt);

    ASSERT_EQ(isres.size(), isres2.size());
    for (size_t i = 0; i < isres.size(); ++i) {
        EXPECT_EQ(isres[i].median, isres2[i].median);
        EXPECT_EQ(isres[i].mad, isres2[i].mad);
    }

    // Now trying to reuse the same workspace but with different blocking.
    std::reverse(block.begin(), block.end());
    auto ref = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), NULL, opt);
    work.set(block.size(), block.data());
    isres = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), &work, opt);
    isres2 = scran::find_median_mad::compute_blocked<double, int>(even_values.size(), block.data(), even_values.data(), &work, opt);

    ASSERT_EQ(isres.size(), isres2.size());
    for (size_t i = 0; i < isres.size(); ++i) {
        EXPECT_EQ(isres[i].median, isres2[i].median);
        EXPECT_EQ(isres[i].mad, isres2[i].mad);
    }
}

TEST(FindMedianMad, DifferentType) {
    std::vector<int> foobar { 1, 2, 4, 3, 6, 9, 8, 7 };
    std::vector<int> block { 0, 0, 0, 0, 1, 1, 1, 1 };
    auto out = scran::find_median_mad::compute_blocked<float, int>(foobar.size(), block.data(), foobar.data(), NULL, scran::find_median_mad::Options());
    EXPECT_FLOAT_EQ(out[0].median, 2.5);
    EXPECT_FLOAT_EQ(out[0].mad, 1.4826);
    EXPECT_FLOAT_EQ(out[1].median, 7.5);
    EXPECT_FLOAT_EQ(out[1].mad, 1.4826);
}
