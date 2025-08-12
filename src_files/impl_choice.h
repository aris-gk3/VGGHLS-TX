#ifndef IMPL_CHOICE_H
#define IMPL_CHOICE_H

// **Module Integrations**
// Max pool: MAXPOOL
// Head: HEAD
#define MAXPOOL
#define HEAD
// **Interface**
// IfMap: IFMAP_FACTOR1, IFMAP_FACTOR7, IFMAP_FACTOR14
// WtMap: WTMAP_FACTOR1,WTMAP_FACTOR9, WTMAP_FACTOR18
// #define IFMAP_FACTOR7
// #define IFMAP_WIDTHFACTOR 7
#define IFMAP_FACTOR1
#define IFMAP_WIDTHFACTOR 1
// #define WTMAP_FACTOR9
// **Scheduling**
// Region1: 

// Region2: REGION2_SEQ, REGION2_DFL, REGION2_MNLSCHEDULE_2BUF, REGION2_PPL
#define REGION2_DFL
// Region3: 

// **Buffer Sizing**

#endif // IMPL_CHOICE_H