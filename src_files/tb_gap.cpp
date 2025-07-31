#include "header.h"
#include <iostream>
#include <iomanip>


void gap_test(){
	const int C = 512, C_SAMPLE = 2;
	const int H = 7, W = 7;
	px_data_t input[C * H * W];
	px_data_t output[C];

	std::cout << "Sample Data before GAP:\n\n\n";
	for (int i = 0; i < C_SAMPLE; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int k = 0; k < W; ++k) {
                input[i*49 + j*7 + k] = std::rand()%50;
				printf("%4d ", input[i*H*W + j*W + k]);
            }
			printf("\n");
        }
		printf("%d window\n", i);
    }

	gap(input, output);

	std::cout << "Sample sata after GAP:\n\n\n";
	for (int i = 0; i < C_SAMPLE; ++i) {
		printf("%4d ", output[i]);
		if(i%W==0 && i!=0){
			printf("\n");
		}
    }
	printf("\n");
}