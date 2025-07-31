#include "header.h"
#include <iostream>
#include <iomanip>
#include <ctime>

int fcLayer_test(int verbose){
	if(verbose){
		fcLayer_toyEx();
	}
	else{
		std::cout << "***  No verification for FC Layers. ***" << std::endl;
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

	srand(static_cast<unsigned>(time(0)));
	// Initialize IfMap
	for(int i=0;i<sizeIn;i++){
		*(inPx + i) = rand()%11;
	}
	// Initialize WtMap
	for(int i=0;i<SIZE;i++){
		*(WtMap + i) = rand()%17;
	}

	fcLayer(inPx, WtMap, sizeIn, sizeOut, 0, outPx);

	std::cout << "Printing toy example for FC Layer" << std::endl;
	// Print IfMap
	std::cout << "Input Pixels of FC Layer" << std::endl;
	for(int i=0;i<sizeIn;i++){
		std::cout << std::setw(5) << *(inPx + i);
	}
	std::cout << std::endl;
	// Print WtMap
	std::cout << "Weights of FC Layer" << std::endl;
	for(int i=0;i<sizeOut;i++){
		for(int j=0;j<sizeIn;j++){
			std::cout << std::setw(5) << *(WtMap + i*sizeIn + j);
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
		std::cout << std::setw(5) << *(outPx + i);
	}
	std::cout << "\n\n";

}
