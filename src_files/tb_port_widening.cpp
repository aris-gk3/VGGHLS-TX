#include "header.h"
#include <type_traits>

// function definitions or overloading
void pack(px_data_t *Map, px_data_t *Map_widened,
            int factor, int mem_size_widened){
    std::cerr << "Function pack() called incorrectly!" << std::endl;
}


void unpack(px_data_t *Map, px_data_t *Map_widened,
            int factor, int mem_size_widened){
    std::cerr << "Function pack() called incorrectly!" << std::endl;
}

void wt_reorder(const wt_data_t *Map, wt_data_t *Map_reordered, int layerNo){
// Reorder all the elements of the current layer
    for(int ofBase=0;ofBase<Nof_step_rom[layerNo];ofBase++){
        for(int Tof_step_i=0;Tof_step_i<(Tof_rom[layerNo]/POF);Tof_step_i++){
            for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
                for(int Nky_i=0;Nky_i<NKY;Nky_i++){
                    for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
                        for(int Pof_i=0;Pof_i<POF;Pof_i++){
                            Map_reordered[
                                (Tof_step_i+ofBase*tof_step_rom[layerNo])*Nif_rom[layerNo]*NKY*NKX*POF
                                + Nif_i*NKY*NKX*POF
                                + Nky_i*NKX*POF
                                + Nkx_i*POF
                                + Pof_i
                            ] =
                                Map[(Tof_step_i*POF+Pof_i+ofBase*Tof_rom[layerNo])*Nif_rom[layerNo]*NKY*NKX
                                    + Nif_i*NKY*NKX
                                    + Nky_i*NKX
                                    + Nkx_i];
                        }
                    }
                }
            }
        }
    }
}