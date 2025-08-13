#include "header.h"
#include <type_traits>

// void swap(px_data_t *&Map, px_data_t *&Map_widened){
//     px_data_t *Map_temp = Map;
//     Map_widened = Map;
//     Map_temp = Map_temp;
// }

// Primary template: disabled for different types
template <typename T1, typename T2>
void swap_if_same_type(T1*& a, T2*& b) {
    // static_assert(std::is_same<T1, T2>::value, "swap_if_same_type requires both pointers to have the same type");
    // This line will never run if types differ because static_assert triggers compile-time error.
}

// Partial specialization for same types
template <typename T>
void swap_if_same_type(T*& a, T*& b) {
    T* temp = a;
    a = b;
    b = temp;
}


// // Maybe in header files
// template void pack<ap_int<224>>(ap_int<32>*, ap_int<224>*, int, int);
// template void pack<ap_int<448>>(ap_int<32>*, ap_int<448>*, int, int);

// void pack(px_data_t *Map, px_data_t *Map_widened,
//             int factor, int mem_size_widened){

// }

// template<parameters>
// void pack<px_data_t>(px_data_t *Map, px_data_t *Map_widened,
//             int factor, int mem_size_widened){
//     swap_if_same_type(Map, Map_widened);
// }

// // // Explicit template instantiations for the specific types used
// template void pack<ap_int<224>>(ap_int<32>*, ap_int<224>*, int, int);
// // template void unpack<ap_int<224>>(ap_int<224>*, ap_int<32>*, int, int);
// template void pack<px_data_t_widened>
// (px_data_t *Map, px_data_t_widened *Map_widened,
//             int factor, int mem_size_widened);
// // template void pack<px_data_t>
// // (px_data_t *Map, px_data_t *Map_widened,
// //             int factor, int mem_size_widened);
// // template void pack<wt_data_t_widened>
// // (px_data_t *Map, wt_data_t_widened *Map_widened,
// //             int factor, int mem_size_widened);
// // (px_data_t *Map, wt_data_t *Map_widened,
// //             int factor, int mem_size_widened);

// void unpack(px_data_t_widened *Map_widened, px_data_t *Map, 
// 			int factor, int mem_size_widened);
// void unpack(px_data_t *Map_widened, px_data_t *Map, 
// 			int factor, int mem_size_widened);
// void unpack(wt_data_t_widened *Map_widened, px_data_t *Map, 
// 			int factor, int mem_size_widened);
// void unpack(wt_data_t *Map_widened, px_data_t *Map, 
// 			int factor, int mem_size_widened);