#include "header.h"
#include <iostream>
#include <iomanip>

int maxPool_test(int verbose){
	if(verbose){
		maxPool_toyEx();
	}
	else{
		std::cout << "***  No verification for Max pooling. ***" << std::endl;
	}
	return 0;
}

void maxPool_toyEx(){
	int channels = 3, ydimOut = 6, xdimOut =6;
	int ydimIn = ydimOut*2; int xdimIn = xdimOut*2;
	px_data_t input[3*12*2*12*2];
	px_data_t output[3*12*12];


	// Initialize input
	for(int c=0; c<channels; c++){
		for(int i=0; i<ydimOut*2; i++){
			for(int j=0; j<xdimOut*2; j++){
				*(input + c*ydimIn*xdimIn + i*xdimIn + j) = c*19 + i*7 + j*3;
			}
		}
	}
	// maxPool(input, channels, ydimOut, xdimOut, output);
	maxpool2x2(input, channels, ydimIn, xdimIn, output);

	std::cout << "Toy Example for max pooling." << std::endl;
	std::cout << "Printing Input before max pooling:" << std::endl;
	for(int c=0; c<channels; c++){
		std::cout << c << " Channel" << std::endl;
		for(int i=0; i<ydimOut*2; i++){
			for(int j=0; j<xdimOut*2; j++){
				std::cout << std::setw(10) << *(input + c*ydimIn*xdimIn + i*xdimIn + j);
			}
		std::cout << " " << std::endl;
		}
	}
	std::cout << "Printing Output after max pooling:" << std::endl;
	for(int c=0; c<channels; c++){
		std::cout << c << " Channel" << std::endl;
		for(int i=0; i<ydimOut; i++){
			for(int j=0; j<xdimOut; j++){
				std::cout << std::setw(10) << *(output + c*ydimOut*xdimOut + i*xdimOut + j);
			}
		std::cout << " " << std::endl;
		}
	}
}
