#include "header.h"
#include <iostream>
#include <iomanip>
#include <ctime>

int fcLayer_test(int verbose, std::string caseChoice){
	px_data_t IfMap[FMAP_MEMSIZE];
	wt_data_t WtMapFC[512*256];
	px_data_t OfMap[FMAP_MEMSIZE];
	px_data_t finalOut[17];
	initIfMap(13, 0);
	for(int i=0;i<512*256;i++){
		WtMapFC[i] = rand()%10;
	}
	if(caseChoice=="ConvX"){
		fcChoice(IfMap, WtMapFC, OfMap);
		// ConvX(IfMap, WtMap, WtMapFc, OfMap); // has widened ports
		fcLayersOF(IfMap, WtMapFC, finalOut);
		for(int i=0;i<17;i++){
			if(finalOut[i]!=OfMap[i]){
				std::cout << "Error found!" << std::endl;
				return 1;
			}
			std::cout << std::setw(4) <<finalOut[i];
		}
		std::cout << "\nFc layers test passed!" << std::endl;
	}
	else{
		fcLayer_toyEx();
	}
	return 0;
}

void fcLayer_toyEx(){
	const int sizeIn = 8, sizeOut = 6;
	px_data_t inPx[sizeIn];
	const int SIZE = 8*6;
	wt_data_t WtMap[SIZE];
	px_data_t outPx[sizeOut];
	b_data_t bias[sizeOut];

	// srand(static_cast<unsigned>(time(0)));
	// Initialize IfMap
	for(int i=0;i<sizeIn;i++){
		inPx[i] = rand()%3;
	}
	// Initialize WtMap
	for(int i=0;i<SIZE;i++){
		WtMap[i] = rand()%3;
	}

	fcLayer(inPx, WtMap, sizeIn, sizeOut, 0, outPx);

	std::cout << "Printing toy example for FC Layer" << std::endl;
	// Print IfMap
	std::cout << "Input Pixels of FC Layer" << std::endl;
	for(int i=0;i<sizeIn;i++){
		std::cout << std::setw(5) << inPx[i];
	}
	std::cout << std::endl;
	// Print WtMap
	std::cout << "Weights of FC Layer" << std::endl;
	for(int i=0;i<sizeOut;i++){
		for(int j=0;j<sizeIn;j++){
			std::cout << std::setw(5) << WtMap[i*sizeIn + j];
		}
		std::cout << std::endl;
	}
	// Print Bias
	std::cout << "Bias of FC Layer" << std::endl;
	for(int i=0;i<sizeOut;i++){
		std::cout << std::setw(5) << biasFC[0][i];
	}
	std::cout << std::endl;
	// Print Output of FC Layer
	std::cout << "Output Pixels of FC Layer" << std::endl;
	for(int i=0;i<sizeOut;i++){
		std::cout << std::setw(5) << outPx[i];
	}
	std::cout << "\n\n";

}
