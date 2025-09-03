#ifndef IMPL_CHOICE_H
#define IMPL_CHOICE_H

// **Module Integrations**
// Max pool: MAXPOOL_INTEGRATION, Head: HEAD_INTEGRATION
// Head integration is only defined if max pool integration is defined
// #define MAXPOOL_INTEGRATION
// #define HEAD_INTEGRATION

// **Interface**
// IfMap, OfMap: are widened if FMAP_WIDEN is declared (7)
// WtMap: is widened if WTMAP_WIDEN is declared (8, 16, 32)
// If not widened, width factor is declared 1
// #define FMAP_WIDEN
#define FMAP_WIDTHFACTOR 1
// #define WTMAP_WIDEN
#define WTMAP_WIDTHFACTOR 1

// **Scheduling**
// Region1: 

// Region2: REGION2_SEQ, REGION2_DFL, REGION2_MNLSCHEDULE_2BUF, REGION2_PPL
#define REGION2_SEQ
// Region3: REG3_SEQ, REG3_OVLP, REG3_FOVLP, REG3_DFL
#define REG3_SEQ
// **Tiling Factors**
// Tof: PREVIOUS_TOF_CHOICE, MAX_TOF_CHOICE, MIDDLE_TOF_CHOICE, MIN_TOF_CHOICE (conv. layers:8-13)
// Toy: MAX_TOY_CHOICE, MIN_TOY_CHOICE (conv. layers:1-7)
#define PREVIOUS_TOF_CHOICE
#define MIN_TOY_CHOICE

// **Misc**
#define RECIPROCAL_BITS 30
#define INTERNAL_CONFIG_LAYER
    // used for debbuging/running without fc layers in the end
#define CONVX
#define NEWFACTORCHOICE

#endif // IMPL_CHOICE_H