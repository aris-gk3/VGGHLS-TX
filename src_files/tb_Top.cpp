#include "header.h"
#include <iostream>
#include <iomanip>
#include <ctime>

// *****  Testing Functions  *****

#if MODEL == 7
int vgg16_test(int verbose, int minPrint, int biasReLuTrue){
    int check = 0;
    px_data_t finalOut[1000], finalOut_golden[1000], Compared_finalOut[1000]; // Final Output

    if (MODEL != 7) {
        std::cout << "Forgot to change to the correct model!\n";
        return 1;
    }

    wt_data_t* WtMapCNN = nullptr;
    wt_data_t** WtMapConv = nullptr;
    wt_data_t** WtMapFC = nullptr;
    px_data_t* IfMap = nullptr;
    px_data_t* Map2 = nullptr;

    MemInit(WtMapCNN, WtMapConv, WtMapFC, IfMap, Map2);

    vgg16_software(IfMap, WtMapConv, WtMapFC, finalOut_golden, biasReLuTrue);
	std::cout << "Computed SW C sim of VGG_16" << std::endl;
    vgg16Top(IfMap, WtMapCNN, Map2, finalOut);
	std::cout << "Computed HW C sim of VGG_16" << std::endl;

    // Compare outputs
    for (int i = 0; i < 1000; i++) {
        if (finalOut[i] != finalOut_golden[i]) {
            check = 1;
        }
        Compared_finalOut[i] = finalOut[i] - finalOut_golden[i];
    }

    // Debug print
    if (verbose) {
        std::cout << "Printing Output Results Under Test\n\n";
        for (int i = 0; i < 1000; i++) {
            std::cout << std::setw(7) << finalOut[i];
            if ((i+1) % 10 == 0) std::cout << "\n";
        }
        std::cout << "\n\n";
        std::cout << "Printing Golden Output Results\n\n";
        for (int i = 0; i < 1000; i++) {
            std::cout << std::setw(7) << finalOut_golden[i];
            if ((i+1) % 10 == 0) std::cout << "\n";
        }
        std::cout << "\n\n";
    }

    // Verification
    if (check) {
        std::cout << "*****  VGG-16 ";
        std::cout << (biasReLuTrue ? "with " : "without ");
        std::cout << "bias + ReLu test failed!  ******\n" << std::endl;
    } else {
        std::cout << "*****  CnnTop ";
        std::cout << (biasReLuTrue ? "with " : "without ");
        std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
    }

    // Free memory
    delete[] WtMapCNN;
    for (int i = 0; i < 13; i++) delete[] WtMapConv[i];
    delete[] WtMapConv;
    for (int i = 0; i < 3; i++) delete[] WtMapFC[i];
    delete[] WtMapFC;
    delete[] IfMap;
    delete[] Map2;

    return check;
}
#endif

// int oxfordFlowers_test(int verbose, int debug, int minPrint, int biasReLuTrue){
//     int check = 0;
//     px_data_t finalOut[CLASSES], finalOut_golden[CLASSES], Compared_finalOut[CLASSES]; // Final Output

//     if (MODEL != 9) {
//         std::cout << "Forgot to change to the correct model!\n";
//         return 1;
//     }

// 	// wt_data_t* WtMapCNN = new wt_data_t[WTMAP_FULL_SIZE];
//     // wt_data_t** WtMapConv = new wt_data_t*[LAYERS];
// 	// wt_data_t** WtMapFC = new wt_data_t*[2];
// 	wt_data_t* WtMapCNN = nullptr;
//     wt_data_t** WtMapConv = nullptr;
// 	wt_data_t** WtMapFC = nullptr;

//     wtMemInitBin(WtMapCNN, WtMapConv, WtMapFC);

// 	// std::cout << "1D array print\n";
// 	// for(int i=0;i<13;i++){
// 	// 	WtMapCNN += WtMapOffsetConv[i];
// 	// }
// 	// WtMapCNN += WtMapOffsetFC[0];
// 	// for(int i=0;i<17;i++){
// 	// 	for(int j=0;j<256;j++){
// 	// 		std::cout << std::setw(4) << *(WtMapCNN + i*256 + j) << "  ";
// 	// 	}
// 	// 	std::cout << "\n";
// 	// }

// 	// std::cout << "2D array print\n";
// 	// for(int i=0;i<17;i++){
// 	// 	for(int j=0;j<256;j++){
// 	// 		std::cout << std::setw(4) << *(WtMapFC[1] + i*256 + j) << "  ";
// 	// 	}
// 	// 	std::cout << "\n";
// 	// }

// 	px_data_t* IfMap = initIfMap(0, 1);
// 	px_data_t* Map2 = new px_data_t[MAP_SIZE];

//     oxfordFlowers_software(IfMap, WtMapConv, WtMapFC, finalOut_golden, biasReLuTrue);
// 	std::cout << "Computed SW C sim of Oxford Flowers Model" << std::endl;
//     tlModelTop(IfMap, WtMapCNN, Map2, finalOut);
// 	std::cout << "Computed HW C sim of Oxford Flowers Model" << std::endl;

//     // // Compare outputs
//     // for (int i = 0; i < CLASSES; i++) {
//     //     if (finalOut[i] != finalOut_golden[i]) {
//     //         check = 1;
//     //     }
//     //     Compared_finalOut[i] = finalOut[i] - finalOut_golden[i];
//     // }

//     // Debug print
//     if (debug) {
//         std::cout << "Printing Output Results Under Test\n\n";
//         for (int i = 0; i < CLASSES; i++) {
//             std::cout << std::setw(7) << finalOut[i];
//             if ((i+1) % 10 == 0) std::cout << "\n";
//         }
//         std::cout << "\n\n";
//         std::cout << "Printing Golden Output Results\n\n";
//         for (int i = 0; i < CLASSES; i++) {
//             std::cout << std::setw(7) << finalOut_golden[i];
//             if ((i+1) % 10 == 0) std::cout << "\n";
//         }
//         std::cout << "\n\n";
//     }

// 	// Find position of max
// 	int maxIndex = 0; // start by assuming first element is max
//     for (int i = 1; i < CLASSES; ++i) {
//         if (finalOut_golden[i] > finalOut_golden[maxIndex]) {
//             maxIndex = i;
//         }
//     }
//     std::cout << "Class found in testbench is number " << maxIndex << std::endl;
// 	maxIndex = 0;
//     for (int i = 1; i < CLASSES; ++i) {
//         if (finalOut[i] > finalOut[maxIndex]) {
//             maxIndex = i;
//         }
//     }
//     std::cout << "Class found in DUT is number " << maxIndex << std::endl;

//     // Verification
//     if (check) {
//         std::cout << "*****  Tranfer Learning Model for Oxford Flowers ";
//         std::cout << (biasReLuTrue ? "with " : "without ");
//         std::cout << "bias + ReLu test failed!  ******\n" << std::endl;
//     } else {
//         std::cout << "*****  CnnTop ";
//         std::cout << (biasReLuTrue ? "with " : "without ");
//         std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
//     }

// 	delete[] WtMapConv;
//     delete[] WtMapFC;
//     delete[] WtMapCNN;
// 	WtMapCNN = nullptr;  // Good practice to avoid dangling pointer
// 	WtMapConv = nullptr;
// 	WtMapFC = nullptr;

// 	delete[] IfMap;
// 	IfMap = nullptr;
// 	delete[] Map2;
// 	Map2 = nullptr;
//     return check;
// }


void minimalRunSynth(int layerNo){
	static px_data_t_widened IfMap[MAP_SIZE] = {0};
	static px_data_t WtMap[WTMAP_MEMSIZE] = {0};
	static px_data_t OfMap[MAP_SIZE] = {0};

	ConvLayer(IfMap, WtMap, OfMap);
}


// *****  Software Functions  *****

#if MODEL == 7
void vgg16_software(px_data_t* IfMap,
                    wt_data_t** WtMapConv,
                    wt_data_t** WtMapFC,
                    px_data_t* finalOut,
                    int biasReLuTrue){

	static px_data_t Map1[MAP_SIZE], Map2[MAP_SIZE];

	for(int i=0;i<MAP_SIZE;i++){
		Map1[i] = 0; Map2[i] = 0;
	}

	convLayer_software(0, IfMap, WtMapConv[0], Map1, biasReLuTrue);

	int max = 0, tmp;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "1) Max here is :" << max << "\n";

	convLayer_software(1, Map1, WtMapConv[1], Map2, biasReLuTrue);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "2) Max here is :" << max << "\n";

	maxPool(Map2, 64, 112, 112, Map1);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "3) Max here is :" << max << "\n";

	convLayer_software(2, Map1, WtMapConv[2], Map2, biasReLuTrue);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "4) Max here is :" << max << "\n";

	convLayer_software(3, Map2, WtMapConv[3], Map1, biasReLuTrue);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "5) Max here is :" << max << "\n";

	maxPool(Map1, 128, 56, 56, Map2);

	convLayer_software(4, Map2, WtMapConv[4], Map1, biasReLuTrue);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "6) Max here is :" << max << "\n";
	convLayer_software(5, Map1, WtMapConv[5], Map2, biasReLuTrue);
	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "7) Max here is :" << max << "\n";
	convLayer_software(6, Map2, WtMapConv[6], Map1, biasReLuTrue);
	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "8) Max here is :" << max << "\n";

	maxPool(Map1, 256, 28, 28, Map2);

	convLayer_software(7, Map2, WtMapConv[7], Map1, biasReLuTrue);
	convLayer_software(8, Map1, WtMapConv[8], Map2, biasReLuTrue);
	convLayer_software(9, Map2, WtMapConv[9], Map1, biasReLuTrue);
	maxPool(Map1, 512, 14, 14, Map2);

	convLayer_software(10, Map2, WtMapConv[10], Map1, biasReLuTrue);
	convLayer_software(11, Map1, WtMapConv[11], Map2, biasReLuTrue);
	convLayer_software(12, Map2, WtMapConv[12], Map1, biasReLuTrue);
	maxPool(Map1, 512, 7, 7, Map2);

	fcLayer(Map2, WtMapFC[0], 25088, 4096, 0, Map1);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "9) Max here is :" << max << "\n";

	fcLayer(Map1, WtMapFC[1], 4096, 4096, 1, Map2);

	max = 0;
	for(int i=0;i<MAP_SIZE;i++){
		tmp = (Map1[i]>Map2[i]) ? Map1[i] : Map2[i];
		max = (max>tmp) ? max : tmp;
		if( (Map1[i] > 2047483647) || (Map2[i] > 2047483647)
				|| (Map1[i] < -2047483647) || (Map2[i] < -2047483647) ){
			//std::cout << "Found something here!\n";
		}
	}
	std::cout << "10) Max here is :" << max << "\n";

	fcLayer(Map2, WtMapFC[2], 4096, 1000, 2, finalOut);
}
#endif

void oxfordFlowers_software(px_data_t* IfMap,
                    wt_data_t** WtMapConv,
                    wt_data_t** WtMapFC,
                    px_data_t* finalOut,
                    int biasReLuTrue){
	// static px_data_t Map1[MAP_SIZE], Map2[MAP_SIZE];
    px_data_t* Map1 = new px_data_t[MAP_SIZE];
    px_data_t* Map2 = new px_data_t[MAP_SIZE];
	int min, max, minWt, maxWt;

	for(int i=0;i<MAP_SIZE;i++){
		Map1[i] = 0; Map2[i] = 0;
	}
	findMinMax(IfMap, 3*224*224, min, max);
	std::cout << "Input Range : min=" << min << " ,max=" << max << "\n";

	// First Conv. Block
	convLayer_software(0, IfMap, WtMapConv[0], Map1, biasReLuTrue);
	findMinMax(WtMapConv[0], 3*64*3*3, minWt, maxWt);
	findMinMax(Map1, 64*224*224, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "01) After block1_conv1: min=" << min << ", max=" << max << "\n";

	convLayer_software(1, Map1, WtMapConv[1], Map2, biasReLuTrue);
	findMinMax(WtMapConv[1], 64*64*3*3, minWt, maxWt);
	findMinMax(Map2, 64*224*224, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "02) After block1_conv2: min=" << min << ", max=" << max << "\n";

	maxPool(Map2, 64, 112, 112, Map1);
	findMinMax(Map1, 64*112*112, min, max);
	std::cout << "03) After maxpool1: min=" << min << ", max=" << max << "\n";

	// Second Conv. Block
	convLayer_software(2, Map1, WtMapConv[2], Map2, biasReLuTrue);
	findMinMax(WtMapConv[2], 64*128*3*3, minWt, maxWt);
	findMinMax(Map2, 128*112*112, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "04) After block2_conv1: min=" << min << ", max=" << max << "\n";

	convLayer_software(3, Map2, WtMapConv[3], Map1, biasReLuTrue);
	findMinMax(WtMapConv[3], 128*128*3*3, minWt, maxWt);
	findMinMax(Map1, 128*112*112, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "05) After block2_conv2: min=" << min << ", max=" << max << "\n";

	maxPool(Map1, 128, 56, 56, Map2);
	findMinMax(Map2, 128*56*56, min, max);
	std::cout << "06) After maxpool2: min=" << min << ", max=" << max << "\n";

	// Third Conv. Block	
	convLayer_software(4, Map2, WtMapConv[4], Map1, biasReLuTrue);
	findMinMax(WtMapConv[4], 128*256*3*3, minWt, maxWt);
	findMinMax(Map1, 256*56*56, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "07) After block3_conv1: min=" << min << ", max=" << max << "\n";

	convLayer_software(5, Map1, WtMapConv[5], Map2, biasReLuTrue);
	findMinMax(WtMapConv[5], 256*256*3*3, minWt, maxWt);
	findMinMax(Map2, 256*56*56, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "08) After block3_conv2: min=" << min << ", max=" << max << "\n";

	convLayer_software(6, Map2, WtMapConv[6], Map1, biasReLuTrue);
	findMinMax(WtMapConv[6], 256*256*3*3, minWt, maxWt);
	findMinMax(Map1, 256*56*56, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "09) After block3_conv3: min=" << min << ", max=" << max << "\n";

	maxPool(Map1, 256, 28, 28, Map2);
	findMinMax(Map2, 256*28*28, min, max);
	std::cout << "10) After maxpool3: min=" << min << ", max=" << max << "\n";

	// Fourth Conv. Block
	convLayer_software(7, Map2, WtMapConv[7], Map1, biasReLuTrue);
	findMinMax(WtMapConv[7], 256*512*3*3, minWt, maxWt);
	findMinMax(Map1, 512*28*28, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "11) After block4_conv1: min=" << min << ", max=" << max << "\n";

	convLayer_software(8, Map1, WtMapConv[8], Map2, biasReLuTrue);
	findMinMax(WtMapConv[8], 512*512*3*3, minWt, maxWt);
	findMinMax(Map2, 512*28*28, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "12) After block4_conv2: min=" << min << ", max=" << max << "\n";

	convLayer_software(9, Map2, WtMapConv[9], Map1, biasReLuTrue);
	findMinMax(WtMapConv[9], 512*512*3*3, minWt, maxWt);
	findMinMax(Map1, 512*28*28, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "13) After block4_conv3: min=" << min << ", max=" << max << "\n";

	maxPool(Map1, 512, 14, 14, Map2);
	findMinMax(Map2, 512*14*14, min, max);
	std::cout << "14) After maxpool4: min=" << min << ", max=" << max << "\n";

	// Fifth Conv. Block
	convLayer_software(10, Map2, WtMapConv[10], Map1, biasReLuTrue);
	findMinMax(WtMapConv[10], 512*512*3*3, minWt, maxWt);
	findMinMax(Map1, 512*14*14, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "15) After block5_conv1: min=" << min << ", max=" << max << "\n";

	convLayer_software(11, Map1, WtMapConv[11], Map2, biasReLuTrue);
	findMinMax(WtMapConv[11], 512*512*3*3, minWt, maxWt);
	findMinMax(Map2, 512*14*14, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "16) After block5_conv2: min=" << min << ", max=" << max << "\n";

	convLayer_software(12, Map2, WtMapConv[12], Map1, biasReLuTrue);
	findMinMax(WtMapConv[12], 512*512*3*3, minWt, maxWt);
	findMinMax(Map1, 512*14*14, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "17) After block5_conv3: min=" << min << ", max=" << max << "\n";

	maxPool(Map1, 512, 7, 7, Map2);
	findMinMax(Map2, 512*7*7, min, max);
	std::cout << "18) After maxpool5: min=" << min << ", max=" << max << "\n";

    gap(Map2, Map1);
	findMinMax(Map1, 512, min, max);
	std::cout << "19) After gap: min=" << min << ", max=" << max << "\n";

	fcLayer(Map1, WtMapFC[0], 512, 256, 0, Map2);
	findMinMax(WtMapFC[0], 512*256, minWt, maxWt);
	findMinMax(Map2, 256, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "20) After fc1: min=" << min << ", max=" << max << "\n";

    fcLayer(Map2, WtMapFC[1], 256, 17, 1, finalOut);
	findMinMax(WtMapFC[1], 256*17, minWt, maxWt);
	findMinMax(finalOut, CLASSES, min, max);
	std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
	std::cout << "21) After fc2: min=" << min << ", max=" << max << "\n";

	delete[] Map1; delete[] Map2;
}


// *****  Auxiliary Functions  *****

void MemInit(
    wt_data_t*& WtMapCNN,
    wt_data_t**& WtMapConv,
    wt_data_t**& WtMapFC,
    px_data_t*& IfMap,
    px_data_t*& Map2){
	const int32_t WtMapOffsetConv[13] = {1728, 36864, 73728, 147456, 294912, 589824, 589824, 
                                    1179648, 2359296, 2359296, 2359296, 2359296, 2359296};
    const int32_t WtMapOffsetFC[3] = {25088*4096, 4096*4096, 4096*1000};
    // Allocate arrays
    WtMapCNN = new wt_data_t[WTMAP_FULL_SIZE];
    WtMapConv = new wt_data_t*[13];
    for (int i = 0; i < 13; i++) {
        WtMapConv[i] = new wt_data_t[WtMapOffsetConv[i]];
    }

    WtMapFC = new wt_data_t*[3];
    for (int i = 0; i < 3; i++) {
        WtMapFC[i] = new wt_data_t[WtMapOffsetFC[i]];
    }

    IfMap = new px_data_t[MAP_SIZE];
    Map2 = new px_data_t[MAP_SIZE];

    // Initialize weights
    long int wtMapAddress = 0;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < WtMapOffsetConv[i]; j++) {
            WtMapConv[i][j] = rand() % 5 - 2;
            WtMapCNN[wtMapAddress + j] = WtMapConv[i][j];
            if (wtMapAddress + j >= WTMAP_FULL_SIZE) {
                std::cout << "found here(1)\n";
            }
        }
        wtMapAddress += WtMapOffsetConv[i];
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < WtMapOffsetFC[i]; j++) {
            WtMapFC[i][j] = rand() % 5 - 2;
            WtMapCNN[wtMapAddress + j] = WtMapFC[i][j];
            if (wtMapAddress + j >= WTMAP_FULL_SIZE) {
                std::cout << "found here\n";
            }
        }
        wtMapAddress += WtMapOffsetFC[i];
    }

    // Initialize IfMap
    for (int i = 0; i < MAP_SIZE; i++) {
        IfMap[i] = rand() % 5 - 2;
    }
}


int returnMax(px_data_t *Map, const int NUM_ELEMENTS){
	int max = 0, tmp;
	for(int i=0;i<NUM_ELEMENTS;i++){
		tmp = Map[i];
		max = (max>tmp) ? max : tmp;
	}
	return max;
}


void findMinMax(const px_data_t *Map, const int NUM_ELEMENTS, int &minVal, int &maxVal){
    if (NUM_ELEMENTS <= 0) {
        std::cerr << "Array size must be greater than 0." << std::endl;
        return;
    }

    minVal = Map[0];
    maxVal = Map[0];

    for (int i = 1; i < NUM_ELEMENTS; ++i) {
        if (Map[i] < minVal)
            minVal = Map[i];
        if (Map[i] > maxVal)
            maxVal = Map[i];
    }
}