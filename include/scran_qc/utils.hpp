#ifndef SCRAN_QC_UTILS_HPP
#define SCRAN_QC_UTILS_HPP

#include <type_traits>

namespace scran_qc {

namespace internal {

template<bool vector_, typename Float_> 
auto maybe_vector(Float_ val) {
    if constexpr(vector_) {
        return std::vector<Float_>(1, val);
    } else {
        return val;
    }
}

template<typename Input_, typename BlockSource_>
const auto& define_threshold_outer(const Input_& input, const BlockSource_&) {
    if constexpr(std::is_same<BlockSource_, std::false_type>::value) {
        return input; // input is a float, directly returned.
    } else if constexpr(std::is_same<BlockSource_, std::true_type>::value) {
        return input[0]; // input is a vector of length 1, we get the threshold for the first block.
    } else {
        return input; // defer to define_threshold_inner().
    }
}

template<typename Input_, typename BlockSource_>
auto define_threshold_inner(const Input_& input, const BlockSource_& block, size_t i) {
    if constexpr(std::is_same<BlockSource_, std::false_type>::value || std::is_same<BlockSource_, std::true_type>::value) {
        return input; // input is a float, directly returned, see define_threshold_outer().
    } else {
        return input[block[i]]; // get the threshold for the relevant block.
    }
}

}

}

#endif
