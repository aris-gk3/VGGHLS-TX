#ifndef BIN_PARAMETERS_H
#define BIN_PARAMETERS_H

#include <iostream>
#include <iomanip>
#include <vector>

#define INT_QUANT_FOLDER "OF2_P1_FT_val0.1014_8b_HWA_int/"
#define REL_PATH "OxfordFlowers/Bluebell/"
#define BIN_FILENAME "OxfordFlowers_Bluebell_image_0256_int8.bin"

// Length of each binary file with weights for 1 layer
constexpr int WT_LENGTH[15] = {
    1728  , 36864  , 73728  , 147456 , 294912 , 589824 ,
    589824, 1179648, 2359296, 2359296, 2359296, 2359296,
    2359296, 131072, 4352   };

constexpr const char* LAYER_FILENAME[15] = {
        "block1_conv1", "block1_conv2",
        "block2_conv1", "block2_conv2",
        "block3_conv1", "block3_conv2", "block3_conv3",
        "block4_conv1", "block4_conv2", "block4_conv3",
        "block5_conv1", "block5_conv2", "block5_conv3",
        "dense", "dense_1"};

// First 10 values of binary file for image
const int IMGMEM_GOLDEN[10] = {-47, -45, -44, -45, -46,
                         -42, -40, -43, -44, -42};

#endif // BIN_PARAMETERS_H