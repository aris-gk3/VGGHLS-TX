#include "header.h"
#include <algorithm>
#include <cstdint>  // for int32_t
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <sys/stat.h>  // for stat()
#include <vector>


bool pathExists(const std::string& path){
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}


std::string getPath(){
    std::string pathLocal = "C:/Programming_Files/Test/Vgg_HLS/src_files/";
    std::string pathRM = "C:/Users/arisi/Documents/VitisWorkspace/VggHLS/src_files/";

    if (pathExists(pathLocal)) {
        std::cout << "Found hardcoded local path.\n";
        std::cout << pathLocal << std::endl;
        return pathLocal;
    }
    else if(pathExists(pathRM)){
        std::cout << "Found hardcoded RM path.\n";
        std::cout << pathRM << std::endl;
        return pathRM;
    } 
    else {
        std::cout << "Path does not exist.\n";
        throw std::runtime_error("Error detected!");
    }
}


const std::string& path() {
    static const std::string greeting = [] {
        return getPath();
    }();
    return greeting;
}


int8_t* imgLoadFromBin(const std::string& filename){
    const int channels = 3;
    const int height = 224;
    const int width = 224;
    const size_t num_elements = channels * height * width;

    // Allocate raw array
    int8_t* data = new int8_t[num_elements];

    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file!" << std::endl;
    }

    // Read data
    file.read(reinterpret_cast<char*>(data), num_elements * sizeof(int8_t));
    if (!file) {
        std::cerr << "Error reading file!" << std::endl;
    }

    // Example: print first few values
    // for (int i = 0; i < 10; i++) {
    //     std::cout << "data[" << i << "] = " << static_cast<int>(data[i]) << std::endl;
    // }
    // delete[] data;
    return data;
}


px_data_t* imgLoadFromBin_wrapper(){
    // std::string rel_path = "OxfordFlowers/Bluebell/"; // replaced with #define
    // std::string bin_filename = "OxfordFlowers_Bluebell_image_0256_int8.bin"; // replaced with #define
    // std::string rel_path = "OxfordFlowers/Crocus/";
    // std::string bin_filename = "OxfordFlowers_Crocus_image_0373_int8.bin";


    int8_t* imgMem = imgLoadFromBin(path() + REL_PATH + BIN_FILENAME);
    px_data_t* imgMem_bw = new px_data_t[MAP_SIZE];
    // px_data_t* imgMem_bw = new px_data_t[224*224*3];
    for(int i=0;i<224*224*3;i++){
        imgMem_bw[i] = imgMem[i];
    }
    delete[] imgMem;
    return imgMem_bw;
}


// Function to read int32 data from a binary file into a raw array
int32_t* wtLoadFromBin(const std::string& filename, size_t num_ints){
    std::ifstream infile(filename, std::ios::binary | std::ios::ate);
    if (!infile) {
        throw std::runtime_error("Failed to open file " + filename);
    }

    std::streamsize filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);

    if (filesize < static_cast<std::streamoff>(num_ints * sizeof(int32_t))) {
        throw std::runtime_error("File does not contain enough data.");
    }

    int32_t* data = new int32_t[num_ints];

    if (!infile.read(reinterpret_cast<char*>(data), num_ints * sizeof(int32_t))) {
        delete[] data;
        throw std::runtime_error("Failed to read data from file " + filename);
    }

    return data;
}


wt_data_t* wtLoadFromBin_wrapper(int layerNo){
    std::string int_quant_folder = "OF2_P1_FT_val0.1014_8b_HWA_int/"; // Will be set with #define
    std::vector<std::string> layer_array_file = {"block1_conv1", "block1_conv2", 
                                        "block2_conv1", "block2_conv2",
                                        "block3_conv1", "block3_conv2", "block3_conv3",
                                        "block4_conv1", "block4_conv2", "block4_conv3",
                                        "block5_conv1", "block5_conv2", "block5_conv3",
                                        "dense", "dense_1"};
    const std::string& wt_folder = int_quant_folder+"Wt/";
    const int wtDim[15] = {3*3*3*64, 3*3*64*64, 3*3*64*128,
                            3*3*128*128, 3*3*128*256,
                            3*3*256*256, 3*3*256*256,
                            3*3*256*512, 3*3*512*512,
                            3*3*512*512, 3*3*512*512,
                            3*3*512*512, 3*3*512*512,
                            512*256, 256*17};

    int32_t* wtMem = wtLoadFromBin(path()+int_quant_folder+"Wt/"+layer_array_file[layerNo]+"_weight"+".bin", wtDim[layerNo]);
    wt_data_t* wtMem_bw = new wt_data_t[wtDim[layerNo]];
    for(int i=0;i<wtDim[layerNo];i++){
        wtMem_bw[i] = wtMem[i];
    }
    return wtMem_bw;
}


px_data_t* biasLoadFromBin_wrapper(int layerNo){
    std::string int_quant_folder = "OF2_P1_FT_val0.1014_8b_HWA_int/"; // Will be set with #define
    std::vector<std::string> layer_array_file = {"block1_conv1", "block1_conv2", 
                                        "block2_conv1", "block2_conv2",
                                        "block3_conv1", "block3_conv2", "block3_conv3",
                                        "block4_conv1", "block4_conv2", "block4_conv3",
                                        "block5_conv1", "block5_conv2", "block5_conv3",
                                        "dense", "dense_1"};
    const std::string& bias_folder = int_quant_folder+"Bias/";
    const int biasDim[15] = {64, 64, 128, 128, 256, 256, 256,
                            512, 512, 512, 512, 512, 512,
                            256, 17};

    int32_t* biasMem = wtLoadFromBin(path()+int_quant_folder+"Bias/"+layer_array_file[layerNo]+"_bias"+".bin", biasDim[layerNo]);
    px_data_t* biasMem_bw = new px_data_t[biasDim[layerNo]];
    for(int i=0;i<biasDim[layerNo];i++){
        biasMem_bw[i] = biasMem[i];
        // std::cout << biasMem_bw[i] << std::endl;
    }
    
    return biasMem_bw;
}


// Currently works only for OF2_P1_FT_val0.1014_8b_HWA_int & OxfordFlowers_Bluebell_image_0256_int8.bin
int loadFromBin_wrapper_test(){
    int layerNo = 14;
    int imgMem_golden[10] = {-47, -45, -44, -45, -46,
                             -42, -40, -43, -44, -42};
    int wtMem_golden[17] = {-49, -16,  15,   7,
                            -19,  57,  -6,  23,
                              7,  48,  64, -75,
                             46, -51, -16, -31,
                             -50};
    int biasMem_golden[17] = {0,  2,  4,  8,  0,
                         -1, -4,  0, -5, -2,
                          1,  3, -2, -3, -4,
                          1,  1};
    px_data_t* imgMem = imgLoadFromBin_wrapper();
    wt_data_t* wtMem = wtLoadFromBin_wrapper(layerNo);
    px_data_t* biasMem = biasLoadFromBin_wrapper(layerNo);
    for(int i=0;i<17;i++){
        if(wtMem_golden[i]!=wtMem[i]){
            std::cout << "Comparison of last layer Wt & Bias with golden data for OF2_P1_FT_val0.1014_8b_HWA_int failed"
                     << std::endl;
            return 1;
        }
        if(biasMem_golden[i]!=biasMem[i]){
            std::cout << "Comparison of last layer Wt & Bias with golden data for OF2_P1_FT_val0.1014_8b_HWA_int failed"
                     << std::endl;
            return 1;
        }
        std::cout << "wt[" << i << "]: " << wtMem[i]
                  << ", bias[" << i << "]: " << biasMem[i] << std::endl;
    }
    for(int i=0;i<10;i++){
        if(imgMem_golden[i]!=imgMem[i]){
            std::cout << "Comparison of 10 first pixels for OxfordFlowers_Bluebell_image_0256_int8.bin failed"
                     << std::endl;
            return 1;
        }
        std::cout << "img[" << i << "]: " << imgMem[i] << std::endl;
    }
    return 0;
}


// Currently works only for OF2_P1_FT_val0.1014_8b_HWA_int & OxfordFlowers_Bluebell_image_0256_int8.bin
int MemInitBin_test(){
    std::cout << "Comparing first 20 weights & bias of layers conv1, conv5, conv13 & fc2!" << std::endl;
    std::cout << "Weights are read from binary file in path (for first layer):\n" << path()+INT_QUANT_FOLDER+"Wt/" + LAYER_FILENAME[0] + "_weight"+".bin" << std::endl;
    std::cout << "Biases are read from header file." << std::endl;
    std::cout << "Input image pixels are read from binary file in path:\n" << path() + REL_PATH + BIN_FILENAME << std::endl;

    // ** Weight Verification
    wt_data_t* WtMapCNN = nullptr;
    wt_data_t** WtMapConv = nullptr;
	wt_data_t** WtMapFC = nullptr;

    wtMemInitBin(WtMapCNN, WtMapConv, WtMapFC);
    for(int i=0;i<20;i++){
        if(WtMapCNN[i]!=WtMapConv[0][i]){
            std::cout << "Comparison of 20 first weights for OxfordFlowers failed"
                     << std::endl;
            std::cout << "WtMapCNN["<< i << "]: " << WtMapCNN[i] << "WtMapConv[0][" << i << "]: " 
                    << WtMapConv[0][i] << std::endl;
            return 1;
        }
    }

	delete[] WtMapConv;
    delete[] WtMapFC;
    delete[] WtMapCNN;
	WtMapCNN = nullptr;  // Good practice to avoid dangling pointer
	WtMapConv = nullptr;
	WtMapFC = nullptr;
    // ** Input Image Pixel Verification
    // Compare read binary with saved values in bin_parameters.h
    std::cout << "Input image pixels are read from binary file in path:\n" << path() + REL_PATH + BIN_FILENAME << std::endl;
    px_data_t* IfMap = initIfMap(0, 1);
    for(int i=0;i<10;i++){
        if(IMGMEM_GOLDEN[i]!=IfMap[i]){
            std::cout << "Comparison of 10 first pixels for OxfordFlowers_Bluebell_image_0256_int8.bin failed"
                     << std::endl;
            std::cout << "Wrong value: img[" << i << "]: " << IfMap[i] << std::endl;
            return 1;
        }
    }
    std::cout << "Comparison of 10 first pixels for OxfordFlowers_Bluebell_image_0256_int8.bin passed!\n";

    // ** Bias Verification
    // Compare binary with header
    return 0;
}


void wtMemInitBin(
        wt_data_t*& WtMapCNN,
        wt_data_t**& WtMapConv,
        wt_data_t**& WtMapFC){
    // This for loading in arrays all the weights for the TB
    // of vgg16, oxford flowers
    // Relative path strings
    // std::string int_quant_folder = "OF2_P1_FT_val0.1014_8b_HWA_int/"; // replaced with define macro

    // Allocation of array memory
    WtMapCNN = new wt_data_t[WTMAP_FULL_SIZE];
    WtMapConv = new wt_data_t*[LAYERS];
    WtMapFC = new wt_data_t*[2];

    // Loading of data from binary files
    for (int i = 0; i < 15; ++i) {
        std::string bin_path = path()+INT_QUANT_FOLDER+"Wt/"+LAYER_FILENAME[i]+"_weight"+".bin";
        std::ifstream file(bin_path, std::ios::binary);

        if (!file) {
            std::cerr << "Failed to open: " << bin_path << std::endl;
              WtMapCNN[i] = -6666;
            // WtMapCNN[i] = nullptr;
            continue;
        }

        file.seekg(0, std::ios::end);
        size_t filesize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (filesize % sizeof(int32_t) != 0) {
            std::cerr << "File not aligned: " << bin_path << std::endl;
            continue;
        }

        size_t num_elements = filesize / sizeof(int32_t);

        // Temporary buffer for reading
        int32_t* buffer = new int32_t[num_elements];
        file.read(reinterpret_cast<char*>(buffer), filesize);

        if (!file) {
            std::cerr << "Read failed: " << bin_path << std::endl;
            delete[] buffer;
            WtMapCNN[i] = -6666;
            // WtMapCNN[i] = nullptr;
            continue;
        }

        int offset = 0;
        if (i < 13){
            for (int k = 0; k < i; k++)
                offset += WtMapOffsetConv[k];
        }
        else{
            for (int k = 0; k < 13; k++)
                offset += WtMapOffsetConv[k];
            for (int k = 13; k < i; k++)
                offset += WtMapOffsetFC[k - 13];
        }
        for (size_t j = 0; j < num_elements; ++j) {
            WtMapCNN[offset+j] = wt_data_t(buffer[j]);
        }

        delete[] buffer;
        std::cout << "Loaded " << LAYER_FILENAME[i]
                  << " as wt_data_t array of size " << num_elements << std::endl;
        if(num_elements != WT_LENGTH[i]){
            std::cout << "Error in the length of wright binary file of layer." << i << std::endl;
        }
        std::cout << "Length of binary files per layer verified!" << i << std::endl;
    }

    int offset = 0;

    // For convolutional layers (13 layers):
    for (int i = 0; i < 13; ++i) {
        // Set pointer for conv layer i to the current offset in WtMapCNN
        WtMapConv[i] = &WtMapCNN[offset];
        
        // Move offset forward by the size of this conv layer’s weights
        offset += WtMapOffsetConv[i];
    }

    // For fully connected layers (2 layers):
    for (int i = 0; i < 2; ++i) {
        // Set pointer for FC layer i to the current offset in WtMapCNN
        WtMapFC[i] = &WtMapCNN[offset];
        
        // Move offset forward by the size of this FC layer’s weights
        offset += WtMapOffsetFC[i];
    }
}