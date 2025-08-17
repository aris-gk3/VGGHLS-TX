#include "header.h"
#include <iostream>
#include <iomanip>
#include <windows.h>

int main(){
	int check = 0;

	std::cout << "************************************************************************"
			"*********  Starting Verification. *********************"
			"***************************************************************" << std::endl;
	// ***** Design/TB Helper Functions  *****
	// Auxiliary_Calcultions(/*PerformanceEstimation=*/1, /*MemoryBufferSizing=*/0, /*ParameterCalculation=*/0,
	// 		/*PrintLoopLimits=*/0, /*PrintAddrOffsets=*/0, /*PrintBinaryLengths=*/0,
	// 		/*PrintBias=*/0, /*PrintBiasFC=*/0);
	// check += Print_Check_Parameters(/*Verbose=*/1);
	// check += loadFromBin_wrapper_test();
	// std::cout << path() << std::endl; // To check base path
	// To be added:
	// performanceEstimation(/*Layer=*/0, /*CNN=*/1, /*CNNAll=*/1, /*Prefetch=*/2, /*Overlap=*/0, /*SemiOverlap=*/0, /*Seq=*/0);
	// gap_test();

	// *****  Module Test Functions  *****
	// check += loadInBuf2Pe_test(/*Verbose=*/0, /*printFlagInfo=*/0, /*printErrorOnly=*/0, /*printLayer=*/0, /*printToyStep=*/0,
	// 		/*printToxStep=*/0, /*printTofStep=*/0, /*printNif=*/0, /*printNky=*/0, /*printNkx=*/0);
	// check += loadWtBuf2Pe_test(/*Verbose=*/0, /*printErrorOnly=*/0, /*printLayer=*/0, /*printToyStep=*/0, /*printToxStep=*/0,
	// 		/*printTofStep=*/0, /*printNif=*/0, /*printNky=*/0, /*printNkx=*/0);
	// check += storePe2Buf_test(/*Verbose=*/0, /*printErrorOnly=*/0, /*printLayer=*/0, /*printNofStep=*/0,
	// 		/*printNoyStep=*/0, /*printToyStep=*/0, /*printToxStep=*/0, /*printTofStep=*/0, /*biasReLuTrue=*/0);
	// check += wndClc_test(/*Verbose=*/0, /*printLayer=*/0);
	// check += tileClc_test(/*Verbose=*/0, /*printErrorOnly=*/1, /*printLayer=*/0, /*biasReLuTrue=*/0);
	// check += loadIfMap_test(/*Verbose=*/0, /*printLayer=*/0, /*printNofStep=*/0, /*printNoyStep=*/0);
	// check += loadWtMap_test(/*Verbose=*/0, /*printLayer=*/0, /*printNofStep=*/0, /*printNoyStep=*/0, /*printProgress=*/1);
	// check += storeMaps_test(/*Verbose=*/0, /*printLayer=*/0, /*printNofStep=*/0, /*printNoyStep=*/0);
	// check += convLayer_test(/*Verbose=*/0, /*debug=*/1, /*minPrint=*/1, /*printErrorOnly=*/0, /*printLayer=*/0, /*biasReLuTrue=*/1, /*binInput=*/0);
	// check += fcLayer_test(/*Verbose=*/1);
	// check += maxPool_test(/*Verbose=*/1); /* prints max pool for a toy example */
	
	// Has random input, therefore we don't check for range
	// check += vgg16_test(/*Verbose=*/1, /*minPrint=*/1, /*biasReLuTrue=*/1);
	// check += oxfordFlowers_test(/*Verbose=*/0, /*debug=*/1, /*minPrint=*/1, /*biasReLuTrue=*/1);
	minimalRunSynth(/*layerNo=*/0);

	std::cout << "************************************************************************"
				"*********  Verification Complete. *********************"
				"***************************************************************" << std::endl;	

	// MemInitBin_test();

	return check;
}
