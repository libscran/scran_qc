#include "scran_tests/scran_tests.hpp"

#include "scran_qc/format_filters.hpp"

TEST(FormatFilters, ComputeWhich) {
    std::vector<unsigned char> keep{ 0, 1, 1, 0, 0, 1, 0 };
    auto output = scran_qc::filter_index<int>(keep.size(), keep.data());
    std::vector<int> expected { 1, 2, 5 };
    EXPECT_EQ(output, expected);
}

TEST(FormatFilters, ComputeCombined) {
    std::vector<unsigned char> keep1{ 0, 1, 1, 1, 0, 1, 0 };
    std::vector<unsigned char> keep2{ 0, 0, 1, 0, 0, 1, 0 };

    auto output = scran_qc::combine_filters(keep1.size(), std::vector<const unsigned char*>{ keep1.data(), keep2.data() });
    std::vector<unsigned char> expected { 0, 0, 1, 0, 0, 1, 0 };
    EXPECT_EQ(output, expected);

    auto woutput = scran_qc::combine_filters_index<int>(keep1.size(), std::vector<const unsigned char*>{ keep1.data(), keep2.data() });
    std::vector<int> wexpected { 2, 5 };
    EXPECT_EQ(woutput, wexpected);
}
