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

template <typename data_t_widened>
void pack(px_data_t *Map, data_t_widened *Map_widened,
            int factor, int mem_size_widened){
    if(factor!=1){
        for(int i=0;i<mem_size_widened;i++){
            for(int factor_i=0;factor_i<factor;factor_i++){
                Map_widened[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i)
                    = Map[i*factor+factor_i];
            }
        }
    }
    else{ // swap pointers
        if(std::is_same<px_data_t, data_t_widened>::value){
            swap_if_same_type(Map, Map_widened);
        }
        else{
            std::cerr << "Unaccounted state!" << std::endl;
        } 
    }
}

template <typename data_t_widened>
void unpack(data_t_widened *Map_widened, px_data_t *Map,
            int factor, int mem_size_widened){
    if(factor!=1){
        for(int i=0;i<mem_size_widened;i++){
            for(int factor_i=0;factor_i<factor;factor_i++){
                Map [i*factor+factor_i] = 
                    Map_widened[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i);
            }
        }
    }
    else{ // swap pointers
        try {
            px_data_t *Map_temp = Map;
            Map_widened = Map;
            Map_temp = Map_temp;
        }catch(...){
            std::cerr << "Mismatch of array data types!" << std::endl;
        }   
    }
}

// // Explicit template instantiations for the specific types used
// template void pack<ap_int<224>>(ap_int<32>*, ap_int<224>*, int, int);
// template void unpack<ap_int<224>>(ap_int<224>*, ap_int<32>*, int, int);
template void pack<px_data_t_widened>
(px_data_t *Map, px_data_t_widened *Map_widened,
            int factor, int mem_size_widened);
// template void pack<px_data_t>
// (px_data_t *Map, px_data_t *Map_widened,
//             int factor, int mem_size_widened);
// template void pack<wt_data_t_widened>
// (px_data_t *Map, wt_data_t_widened *Map_widened,
//             int factor, int mem_size_widened);
// (px_data_t *Map, wt_data_t *Map_widened,
//             int factor, int mem_size_widened);

void unpack(px_data_t_widened *Map_widened, px_data_t *Map, 
			int factor, int mem_size_widened);
void unpack(px_data_t *Map_widened, px_data_t *Map, 
			int factor, int mem_size_widened);
void unpack(wt_data_t_widened *Map_widened, px_data_t *Map, 
			int factor, int mem_size_widened);
void unpack(wt_data_t *Map_widened, px_data_t *Map, 
			int factor, int mem_size_widened);