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
// Region3: 

// **Buffer Sizing**


// **Misc**
#define RECIPROCAL_BITS 30
#define CASE_AB

#endif // IMPL_CHOICE_H