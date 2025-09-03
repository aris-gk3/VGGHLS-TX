// Not synthesizable
#include "header.h"
#include <iostream>
#include <cmath> // for power of number

void Auxiliary_Calcultions(int MemoryBufferSizing, int ParameterCalculation,
		int PrintLoopLimits, int PrintAddrOffsets, int PrintBinaryLengths,
		int PrintBias, int PrintBiasFC){

	if(MemoryBufferSizing){
		memoryBufferSizing();
	}

	if(ParameterCalculation){
		calculateParameters();
	}

	if(PrintLoopLimits){
		calculateLoopLimits();
	}

	if(PrintAddrOffsets){
		calculateAddressOffsets();
	}

	if(PrintBinaryLengths){
		findBinaryLengths();
	}

	if(PrintBias){
		std::cout << "Print random values for bias" << "\n\n";
		int GenerateBias[BIASMEM_LENGTH];
		for(int i=0;i<BIASMEM_LENGTH;i++){
			GenerateBias[i] = 0;
			std::cout << std::setw(3) << GenerateBias[i] << ", ";
			if((i+1)%35==0){
				std::cout << "\n";
			}
		}
		std::cout << "\n\n";
	}

	if(PrintBiasFC){
		std::cout << "Print random values for bias for FC layers" << "\n\n";
		int GenerateBias[3][4096];
		for(int i=0;i<3;i++){
			std::cout << "{";
			for(int j=0;j<4096;j++){
				GenerateBias[i][j] = (rand()%198) - 99;
				std::cout << std::setw(3) << GenerateBias[i][j] << ", ";
				if((j+1)%35==0){
					std::cout << "\n";
				}
			}
			std::cout << "}\n\n";
		}
		std::cout << "\n\n\n";
	}

}

void performanceEstimation(int Layer, int CNN, int CNNAll, int Prefetch, int Overlap, int SemiOverlap, int Seq){
// List of all cases are:
// For Region 3: (mem2buf, region2, buf2mem)
// 	1) Prefetch of weights or no, no extra overlapping
// 	2) Sequential, Semi-Overlap, Overlap
// For Region 2: (region1, pe2buf)
// 	1) Loop Order
// 	2) Sequential, Semi-Overlap, Overlap
// For Region 1: (buf2pe, pe array)
// Inner impl. estimation doesn't change.

// Prefetch = 0 -> Prints estimation without prefetching of weights
// Prefetch = 1 -> Prints estimation without prefetching of weights
// Prefetch = 2 -> Prints estimation for both cases
	int numInPx[LAYERS], numOutPx[LAYERS], tileInPx[LAYERS], tileOutPx[LAYERS];
	int numWt[LAYERS],tileWt[LAYERS], numOp[LAYERS], tileOp[LAYERS];
	// Cycle Counts declarations
	int seq, seqPref, semiOverlap, semiOverlapPref, overlap, overlapPref;
	// Other secondary declarations
	int mem2buf[LAYERS], comp;



	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		numInPx[layerNo] = Nif_rom[layerNo]*Noy_rom[layerNo]*Tox_rom[layerNo];
		numOutPx[layerNo] = Nof_step_rom[layerNo]*Tof_rom[layerNo]*Noy_rom[layerNo]*Tox_rom[layerNo];
		tileInPx[layerNo] = Nif_rom[layerNo]*Toy_rom[layerNo]*Tox_rom[layerNo];
		tileOutPx[layerNo] = Tof_rom[layerNo]*Toy_rom[layerNo]*Tox_rom[layerNo];
		numWt[layerNo] = Nof_step_rom[layerNo]*Tof_rom[layerNo]*Nif_rom[layerNo]*NKY*NKX;
		tileWt[layerNo] = Tof_rom[layerNo]*Nif_rom[layerNo]*NKY*NKX;
		numOp[layerNo] = Nof_step_rom[layerNo]*Tof_rom[layerNo]*Nif_rom[layerNo]*Noy_rom[layerNo]*Tox_rom[layerNo]*NKY*NKX;
		tileOp[layerNo] = Tof_rom[layerNo]*Nif_rom[layerNo]*Toy_rom[layerNo]*Tox_rom[layerNo]*NKY*NKX;
	}

	// Calculate total cycle count for execution of all convolution layers
	// mem2buf -> tilecalc -> buf2mem

	// Pre-fetch weights or no
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		mem2buf[layerNo] = (tileWt[layerNo] > tileInPx[layerNo] ? tileWt[layerNo] : tileInPx[layerNo]);
	}

	// Sequential Case
	seq = mem2buf[0] + tileOp[0]/(POF*POY*POX) + tileOutPx[0];
	seqPref = seq;
	for(int layerNo=1;layerNo<LAYERS;layerNo++){
		seq += mem2buf[layerNo] + tileOp[layerNo]/(POF*POY*POX) + tileOutPx[layerNo];
		seqPref += tileInPx[layerNo] + tileOp[layerNo]/(POF*POY*POX) + tileOutPx[layerNo];
	}
	// SemiOverlap Case
	if(LAYERS == 1){
		semiOverlap = seq;
		semiOverlapPref = seqPref;
	}
	else if(LAYERS == 2){
		semiOverlap = mem2buf[0] + tileOp[0]/(POF*POY*POX);
		semiOverlap += (mem2buf[1] > tileOutPx[0]? mem2buf[1]: tileOutPx[0]) + tileOp[1]/(POF*POY*POX);
		semiOverlap += tileOutPx[1];

		semiOverlapPref = mem2buf[0] + tileOp[0]/(POF*POY*POX);
		semiOverlapPref += (mem2buf[1] > tileOutPx[0]? mem2buf[1]: tileOutPx[0]) + tileOp[1]/(POF*POY*POX);
		semiOverlapPref += tileOutPx[1];
	}
	else{
		semiOverlap = mem2buf[0] + tileOp[0]/(POF*POY*POX);
		for(int i=0;i<LAYERS-1;i++){
			semiOverlap += (mem2buf[i+1] > tileOutPx[i]? mem2buf[i+1]: tileOutPx[i]) + tileOp[i]/(POF*POY*POX);
		}
		semiOverlap += tileOutPx[LAYERS-1];

		semiOverlapPref = mem2buf[0] + tileOp[0]/(POF*POY*POX);
		for(int i=0;i<LAYERS-1;i++){
			semiOverlapPref += (tileInPx[i+1] > tileOutPx[i]? tileInPx[i+1]: tileOutPx[i]) + tileOp[i]/(POF*POY*POX);
		}
		semiOverlapPref += tileOutPx[LAYERS-1];
	}

	// Overlap Case
	if(LAYERS == 1){
		overlap = seq;
		overlapPref = seq;
	}
	else if(LAYERS == 2){
		overlap = mem2buf[0] + (tileOp[0]/(POF*POY*POX) > mem2buf[1]? tileOp[0]/(POF*POY*POX): mem2buf[1]);
		overlap += (tileOutPx[0] > tileOp[1]/(POF*POY*POX)? tileOutPx[0]: tileOp[1]/(POF*POY*POX)) + tileOutPx[1];

		overlapPref = mem2buf[0] + (tileOp[0]/(POF*POY*POX) > tileInPx[1]? tileOp[0]/(POF*POY*POX): tileInPx[1]);
		overlapPref += (tileOutPx[0] > tileOp[1]/(POF*POY*POX)? tileOutPx[0]: tileOp[1]/(POF*POY*POX)) + tileOutPx[1];
	}
	else{
		overlap = mem2buf[0] + (tileOp[0]/(POF*POY*POX) > mem2buf[1]? tileOp[0]/(POF*POY*POX): mem2buf[1]);
		for(int i=0;i<LAYERS-2;i++){
			comp = (mem2buf[i+2] > tileOp[i+1]/(POF*POY*POX) ? mem2buf[i+2]: tileOp[i+1]/(POF*POY*POX));
			overlap += (comp > tileOutPx[i] ? comp: tileOutPx[i]);
		}
		overlap += (tileOutPx[LAYERS-2]>tileOp[LAYERS-1]/(POF*POY*POX) ? tileOutPx[LAYERS-2]: tileOp[LAYERS-1]/(POF*POY*POX)) + tileOutPx[LAYERS-1];

		overlapPref = mem2buf[0] + (tileOp[0]/(POF*POY*POX) > tileInPx[1]? tileOp[0]/(POF*POY*POX): tileInPx[1]);
		for(int i=0;i<LAYERS-2;i++){
			comp = (tileInPx[i+2] > tileOp[i+1]/(POF*POY*POX) ? tileInPx[i+2]: tileOp[i+1]/(POF*POY*POX));
			overlapPref += (comp > tileOutPx[i] ? comp: tileOutPx[i]);
		}
		overlapPref += (tileOutPx[LAYERS-2]>tileOp[LAYERS-1]/(POF*POY*POX) ? tileOutPx[LAYERS-2]: tileOp[LAYERS-1]/(POF*POY*POX)) + tileOutPx[LAYERS-1];
	}

	// Calculate total cycle count for execution of maxpool part

	// Calculate total cycle count for execution of fc part



	std::cout << "*****  Performance Estimation  *****\n\n";
	std::cout << "Model " << MODEL << " is chosen\n\n";

	if(Layer){ // Print Layer Estimation
		for(int layerNo=0;layerNo<LAYERS;layerNo++){
			std::cout << "So for layer " << layerNo << " we have...\n";
			std::cout << "In.Px.: " << numInPx[layerNo] << " -> ";
			std::cout << "#Wt.: " << numWt[layerNo] << " -> ";
			std::cout << "#Ops.: " << numOp[layerNo] << " -> ";
			std::cout << "Out.Px.: " << numOutPx[layerNo] << "\n";
			std::cout << "And for the tiles of layer " << layerNo << " we have...\n";
			std::cout << "In.Px.: " << tileInPx[layerNo] << " -> ";
			std::cout << "#Wt.: " << tileWt[layerNo] << " -> ";
			std::cout << "#Ops.: " << tileOp[layerNo] << " -> ";
			std::cout << "Out.Px.: " << tileOutPx[layerNo] << "\n";
		}
	}

	if(CNN){ // Print Estimations for all of the model
		if(CNNAll || Seq){
			if(Prefetch == 1 || Prefetch == 2){
				std::cout << "Sequential Implementation with pre-fetch of weights:";
				std::cout << " Total cycle count estimation is " << seqPref << "\n";
			}
			if(Prefetch == 0 || Prefetch == 2){
				std::cout << "Sequential Implementation :";
				std::cout << " Total cycle count estimation is " << seq << "\n";
			}
		}
		if(CNNAll || SemiOverlap){
			if(Prefetch == 1 || Prefetch == 2){
				std::cout << "Semi-Overlapping Implementation with pre-fetch of weights:";
				std::cout << " Total cycle count estimation is " << semiOverlapPref << "\n";
			}
			if(Prefetch == 0 || Prefetch == 2){
				std::cout << "Semi-Overlapping Implementation :";
				std::cout << " Total cycle count estimation is " << semiOverlap << "\n";
			}
		}
		if(CNNAll || Overlap){
			if(Prefetch == 1 || Prefetch == 2){
				std::cout << "Overlapping Implementation with pre-fetch of weights:";
				std::cout << " Total cycle count estimation is " << overlapPref << "\n";
			}
			if(Prefetch == 0 || Prefetch == 2){
				std::cout << "Overlapping Implementation :";
				std::cout << " Total cycle count estimation is " << overlap << "\n";
			}
		}
	}

}

void performanceCalculation(){

}

void parameterCalculation(
		const int Noy[LAYERS], const int Tof[LAYERS], const int Toy[LAYERS],
		const int Tox[LAYERS], int layerNo,
		int *Noy_step, int *Niy, int *Tof_step, int *Toy_step, int *Tox_step,
		int *Tiy, int *Tix, int *row_1map,int *wrd_1row){
#pragma HLS INLINE
	*Noy_step = Noy[layerNo]/Toy[layerNo]; *Niy = Noy[layerNo];
	*Tof_step = my_ceil(Tof[layerNo],POF); *Toy_step = my_ceil(Toy[layerNo],POY); *Tox_step = my_ceil(Tox[layerNo],POX);
	*Tiy = Toy[layerNo] + NKY -1; *Tix = Tox[layerNo] + NKX -1;
	*row_1map = my_ceil(*Tiy,POY); *wrd_1row = my_ceil(*Tix,POX);
}

void memoryBufferSizing(){
	int tmp, max = 0;
	std::cout << " * Auxiliary Calculations for Sizing\n";
	std::cout << "Input Buffer row size needed for each layer:\n";
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = wrd_1row_rom[layerNo] * row_1map_rom[layerNo] * Nif_rom[layerNo];
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n";

	std::cout << "Weight Buffer row size needed for each layer:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = NKX * NKY * Nif_rom[layerNo] * my_ceil(Tof_rom[layerNo],POF);
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n";

	std::cout << "Output Buffer row size needed for each layer:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = my_ceil(Tof_rom[layerNo], OUTBUF_NUM) * Toy_rom[layerNo] * my_ceil(Tox_rom[layerNo], POX);
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n";

	std::cout << "Bias Buffer length needed for each tile:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = Tof_rom[layerNo] + POF%OUTBUF_NUM;
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Size needed for Bias Buffer is" << " -> " << max << "\n";

	std::cout << "Bias length needed for each layer:\n";
	int sum = 0; int tmpsum;
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = Nof_step_rom[layerNo] * Tof_rom[layerNo];
		tmpsum = sum + Nof_step_rom[layerNo] * Tof_rom[layerNo] + POF%OUTBUF_NUM;

		sum +=  tmp;
		max = (max>tmpsum) ? max : tmpsum;
		max = (max>sum) ? max : sum;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Total size is" << " -> " << max << "\n";

	std::cout << "Memory needed for IfMap for each layer:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = Nif_rom[layerNo]*niy_rom[layerNo]*(tix_rom[layerNo]-2); // = Nif*Niy*Nix
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n";

	std::cout << "Memory needed for WtMap for each layer:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = Nif_rom[layerNo]*Nof_step_rom[layerNo]*Tof_rom[layerNo]*NKY*NKX; // = Nif*Nof*Nky*Nkx
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n";

	std::cout << "Memory needed for OfMap for each layer:\n";
	max = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		tmp = Nof_step_rom[layerNo]*Tof_rom[layerNo]*Noy_rom[layerNo]*Tox_rom[layerNo]; // = Nof*Noy*Nox
		max = (max>tmp) ? max : tmp;
		std::cout << "Layer " << layerNo+1 << " -> " << tmp << "\n";
	}
	std::cout << "Max size is" << " -> " << max << "\n\n";

	std::cout << "Weight Memory offsets for address:\n";
	int WtMapOffset[LAYERS];
	int wtmap_full_size = 0;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		WtMapOffset[layerNo] = Nif_rom[layerNo]*Nof_step_rom[layerNo]*Tof_rom[layerNo]*NKY*NKX; // = Nif*Nof*Nky*Nkx
		wtmap_full_size += WtMapOffset[layerNo];
	}
	wtmap_full_size += (25088+4096+1000)*4096;
	std::cout << " {";
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		std::cout << std::setw(8) << WtMapOffset[layerNo];
		std::cout << ", ";
	}
	std::cout << "}\n";

	std::cout << "Calculating WTMAP_FULL_SIZE is " << wtmap_full_size << "\n";
}

void calculateParameters(){
	std::cout << "\n*Calculate Parameters from Necessary Parameters" << "\n\n";
	int noy_step_calc[LAYERS], nofy_step_calc[LAYERS], niy_calc[LAYERS], tof_step_calc[LAYERS], toy_step_calc[LAYERS];
	int tox_step_calc[LAYERS], tiy_calc[LAYERS], tix_calc[LAYERS], row_1map_rom_calc[LAYERS], wrd_1row_rom_calc[LAYERS];
	for(int i=0;i<LAYERS;i++){
		noy_step_calc[i] = Noy_rom[i]/Toy_rom[i];
		nofy_step_calc[i] = (Nof_step_rom[i]>noy_step_rom[i]) ? Nof_step_rom[i] : noy_step_rom[i];
		niy_calc[i] = Noy_rom[i];
		tof_step_calc[i] = my_ceil(Tof_rom[i],POF);
		toy_step_calc[i] = my_ceil(Toy_rom[i],POY);
		tox_step_calc[i] = my_ceil(Tox_rom[i],POX);
		tiy_calc[i] = Toy_rom[i] + NKY -1;
		tix_calc[i] = Tox_rom[i] + NKX -1;
		row_1map_rom_calc[i] = my_ceil(tiy_calc[i],POY);
		wrd_1row_rom_calc[i] = my_ceil(tix_calc[i],POX);
	}
	std::cout << "Noy_step: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << noy_step_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Nofy_step: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << nofy_step_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Niy: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << niy_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Tof_step: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << tof_step_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Toy_step: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << toy_step_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Tox_step: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << tox_step_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Tiy: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << tiy_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "Tix: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << tix_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "row_1map: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << row_1map_rom_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "wrd_1row: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << wrd_1row_rom_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";
}

void calculateLoopLimits(){
	std::cout << "\n* Calculating Loop Limits:\n\n";
	int pe_loop_limit_calc[LAYERS], wtbuf2pe_loop_limitcalc[LAYERS];
	int wndclc_loop_limitcalc[LAYERS], tileclc_loop_limitcalc[LAYERS];

	for(int i=0;i<LAYERS;i++){
		pe_loop_limit_calc[i] = Nif_rom[i]*NKX*NKY-1;
		wtbuf2pe_loop_limitcalc[i] = my_ceil(Tof_rom[i],POF)*Nif_rom[i]*NKY*NKX-1;
		wndclc_loop_limitcalc[i] = Nif_rom[i]*NKX*NKY;
		tileclc_loop_limitcalc[i] = my_ceil(Tof_rom[i],POF)*my_ceil(Toy_rom[i],POY)*my_ceil(Tox_rom[i],POX);
	}

	std::cout << "pe_loop_limit: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << pe_loop_limit_calc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "wtbuf2pe_loop_limit: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << wtbuf2pe_loop_limitcalc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "wndclc_loop_limit_rom: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << wndclc_loop_limitcalc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "tileclc_loop_limit_rom: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << tileclc_loop_limitcalc[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";
}

void calculateAddressOffsets(){
	std::cout << "\n* Calculating Address Offsets:\n\n";
	int pe2buf_addr_offset1[LAYERS], pe2buf_addr_offset2[LAYERS];
	int pe2buf_addr_offset3[LAYERS];

	for(int i=0;i<LAYERS;i++){
		pe2buf_addr_offset1[i] = POY*tox_step_rom[i] - (tox_step_rom[i]-1)
									- (tof_step_rom[i]-1)*(POF/OUTBUF_NUM)*Toy_rom[i]*tox_step_rom[i];
		pe2buf_addr_offset2[i] = (tof_step_rom[i]-1)*(POF/OUTBUF_NUM)*Toy_rom[i]*tox_step_rom[i];
		pe2buf_addr_offset3[i] = (POF/OUTBUF_NUM)*Toy_rom[i]*tox_step_rom[i];
	}
	std::cout << "pe2buf_addr_offset1: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << pe2buf_addr_offset1[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "pe2buf_addr_offset2: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << pe2buf_addr_offset2[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";

	std::cout << "pe2buf_addr_offset3: {";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(3) << pe2buf_addr_offset3[i] << ",";
		if((i+1)%35==0){
			std::cout << "\n";
		}
	}
	std::cout << "}";
	std::cout << "\n\n";
}

void findBinaryLengths(){
	std::cout << "\n* Calculating Binary Lengths:\n\n";

	// Calculate binary length for binary variables (0)
	int layer_i_b, nk_i_b;
	findBinaryLength(&layer_i_b, LAYERS-1);
	findBinaryLength(&nk_i_b, NKY-1);
	std::cout << "LAYERS variable is -> " << LAYERS
			  << "\nNeed " << layer_i_b << " digits for counter\n";
	std::cout << "Kernel size is -> " << NKY
			  << "\nNeed " << nk_i_b << " digits for counter\n\n";

	// Calculate binary length for binary variables (1)
	int Nif_max = 0, Nif_b, Nif_i_b;
	int Niy_max = 0, Niy_b, Niy_i_b;
	int Noy_max = 0, Noy_b, Noy_i_b;
	int Nof_step_max = 0, Nof_step_b, Nof_step_i_b;
	int Noy_step_max = 0, Noy_step_b, Noy_step_i_b;
	int Nofy_step_max = 0, Nofy_step_b, Nofy_step_i_b;

	for(int i=0;i<LAYERS;i++){
		Nif_max = (Nif_max>Nif_rom[i]) ? Nif_max : Nif_rom[i];
		Niy_max = (Niy_max>(niy_rom[i]-1)) ? Niy_max : (niy_rom[i]-1);
		Noy_max = (Noy_max>Noy_rom[i]) ? Noy_max : Noy_rom[i];
		Nof_step_max = (Nof_step_max>Nof_step_rom[i]) ? Nof_step_max : Nof_step_rom[i];
		Noy_step_max = (Noy_step_max>noy_step_rom[i]) ? Noy_step_max : noy_step_rom[i];
	}
	Nofy_step_max = (Nof_step_max>Noy_step_max) ? Nof_step_max : Noy_step_max ;

	findBinaryLength(&Nif_b, Nif_max);
	findBinaryLength(&Nif_i_b, Nif_max-1);

	findBinaryLength(&Niy_b, Niy_max);
	findBinaryLength(&Niy_i_b, Niy_max-1);

	findBinaryLength(&Noy_b, Noy_max);
	findBinaryLength(&Noy_i_b, Noy_max-1);

	findBinaryLength(&Nof_step_b, Nof_step_max);
	findBinaryLength(&Nof_step_i_b, Nof_step_max-1);

	findBinaryLength(&Noy_step_b, Noy_step_max);
	findBinaryLength(&Noy_step_i_b, Noy_step_max-1);

	findBinaryLength(&Nofy_step_b, Nofy_step_max);
	findBinaryLength(&Nofy_step_i_b, Nofy_step_max-1);

	std::cout << "Maximum of Nif is -> " << Nif_max
			  << "\nNeed " << Nif_b << " binary digits \n"
			  << "And " << Nif_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Niy is -> " << Niy_max
			  << "\nNeed " << Niy_b << " binary digits \n"
			  << "And " << Niy_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Noy is -> " << Noy_max
			  << "\nNeed " << Noy_b << " binary digits \n"
			  << "And " << Noy_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Nof_step is -> " << Nof_step_max
			  << "\nNeed " << Nof_step_b << " binary digits \n"
			  << "And " << Nof_step_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Noy_step is -> " << Noy_step_max
			  << "\nNeed " << Noy_step_b << " binary digits \n"
			  << "And " << Noy_step_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Nofy_step is -> " << Nofy_step_max
			  << "\nNeed " << Nofy_step_b << " binary digits \n"
			  << "And " << Nofy_step_i_b << " binary digits for counter\n\n";

	// Calculate binary length for binary variables (2)
	int Tiy_max = 0, Tiy_b, Tiy_i_b;
	int Tix_max = 0, Tix_b, Tix_i_b;
	int Tof_max = 0, Tof_b, Tof_i_b;
	int Toy_max = 0, Toy_b, Toy_i_b;
	int Tox_max = 0, Tox_b, Tox_i_b;
	int Tof_step_max = 0, Tof_step_b, Tof_step_i_b;
	int Toy_step_max = 0, Toy_step_b, Toy_step_i_b;
	int Tox_step_max = 0, Tox_step_b, Tox_step_i_b;

	for(int i=0;i<LAYERS;i++){
		Tiy_max = (Tiy_max>tiy_rom[i]) ? Tiy_max : tiy_rom[i];
		Tix_max = (Tix_max>tix_rom[i]) ? Tix_max : tix_rom[i];
		Tof_max = (Tof_max>Tof_rom[i]) ? Tof_max : Tof_rom[i];
		Toy_max = (Toy_max>Toy_rom[i]) ? Toy_max : Toy_rom[i];
		Tox_max = (Tox_max>Tox_rom[i]) ? Tox_max : Tox_rom[i];
		Tof_step_max = (Tof_step_max>tof_step_rom[i]) ? Tof_step_max : tof_step_rom[i];
		Toy_step_max = (Toy_step_max>toy_step_rom[i]) ? Toy_step_max : toy_step_rom[i];
		Tox_step_max = (Tox_step_max>tox_step_rom[i]) ? Tox_step_max : tox_step_rom[i];
	}

	findBinaryLength(&Tiy_b, Tiy_max);
	findBinaryLength(&Tiy_i_b, Tiy_max-1);

	findBinaryLength(&Tix_b, Tix_max);
	findBinaryLength(&Tix_i_b, Tix_max-1);

	findBinaryLength(&Tof_b, Tof_max);
	findBinaryLength(&Tof_i_b, Tof_max-1);

	findBinaryLength(&Toy_b, Toy_max);
	findBinaryLength(&Toy_i_b, Toy_max-1);

	findBinaryLength(&Tox_b, Tox_max);
	findBinaryLength(&Tox_i_b, Tox_max-1);

	findBinaryLength(&Tof_step_b, Tof_step_max);
	findBinaryLength(&Tof_step_i_b, Tof_step_max-1);

	findBinaryLength(&Toy_step_b, Toy_step_max);
	findBinaryLength(&Toy_step_i_b, Toy_step_max-1);

	findBinaryLength(&Tox_step_b, Tox_step_max);
	findBinaryLength(&Tox_step_i_b, Tox_step_max-1);

	std::cout << "Maximum of Tiy is -> " << Tiy_max
			  << "\nNeed " << Tiy_b << " binary digits \n"
			  << "And " << Tiy_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Tix is -> " << Tix_max
			  << "\nNeed " << Tix_b << " binary digits \n"
			  << "And " << Tix_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Tof is -> " << Tof_max
			  << "\nNeed " << Tof_b << " binary digits \n"
			  << "And " << Tof_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Toy is -> " << Toy_max
			  << "\nNeed " << Toy_b << " binary digits \n"
			  << "And " << Toy_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Tox is -> " << Tox_max
			  << "\nNeed " << Tox_b << " binary digits \n"
			  << "And " << Tox_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Tof_step is -> " << Tof_step_max
			  << "\nNeed " << Tof_step_b << " binary digits \n"
			  << "And " << Tof_step_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Toy_step is -> " << Toy_step_max
			  << "\nNeed " << Toy_step_b << " binary digits \n"
			  << "And " << Toy_step_i_b << " binary digits for counter\n\n";
	std::cout << "Maximum of Tox_step is -> " << Tox_step_max
			  << "\nNeed " << Tox_step_b << " binary digits \n"
			  << "And " << Tox_step_i_b << " binary digits for counter\n\n";
	// Calculate binary length for binary variables (3)
	int pof_b, pof_i_b, poy_b, poy_i_b;
	int pox_b, pox_i_b, pofbank_step_i_b;

	findBinaryLength(&pof_b, POF);
	findBinaryLength(&pof_i_b, POF-1);
	findBinaryLength(&poy_b, POY);
	findBinaryLength(&poy_i_b, POY-1);
	findBinaryLength(&pox_b, POX);
	findBinaryLength(&pox_i_b, POX-1);
	findBinaryLength(&pofbank_step_i_b, POFBANK_STEP-1);

	std::cout << "Pof is -> " << POF
			  << "\nNeed " << pof_b << " binary digits \n"
			  << "And " << pof_i_b << " binary digits for counter\n\n";
	std::cout << "Poy is -> " << POY
			  << "\nNeed " << poy_b << " binary digits \n"
			  << "And " << poy_i_b << " binary digits for counter\n\n";
	std::cout << "Pox is -> " << POX
			  << "\nNeed " << pox_b << " binary digits \n"
			  << "And " << pox_i_b << " binary digits for counter\n\n";
	std::cout << "POFBANK_STEP is -> " << POFBANK_STEP
			  << "\nNeed " << pofbank_step_i_b << " binary digits for counter\n\n";

	// Calculate binary length for binary variables (4)
	int wrd_1row_max = 0, wrd_1row_b;
	int row_1map_max = 0, row_1map_b;
	int conv_loop_max = 0, conv_loop_b;
	int tile_loop_max = 0, tile_loop_b;
	int wnd_loop_max = 0, wnd_loop_b;
	int wtbuf2pe_loop_max = 0, wtbuf2pe_loop_b;

	int tmp;
	for(int i=0;i<LAYERS;i++){
		wrd_1row_max = (wrd_1row_max>my_ceil(tix_rom[i],POX)) ? wrd_1row_max : my_ceil(tix_rom[i],POX);
		row_1map_max = (row_1map_max>my_ceil(tiy_rom[i],POY)) ? row_1map_max : my_ceil(tiy_rom[i],POY);
		tmp = (Nof_step_rom[i]>noy_step_rom[i]) ? Nof_step_rom[i] : noy_step_rom[i];
		conv_loop_max = (conv_loop_max>tmp) ? conv_loop_max : tmp;
		tmp = tof_step_rom[i]*toy_step_rom[i]*tox_step_rom[i];
		tile_loop_max = (tile_loop_max>tmp) ? tile_loop_max : tmp;
		tmp = Nif_rom[i]*NKY*NKX;
		wnd_loop_max = (wnd_loop_max>tmp) ? wnd_loop_max : tmp;
		tmp = tof_step_rom[i]*Nif_rom[i]*NKY*NKX-1;
		wtbuf2pe_loop_max = (wtbuf2pe_loop_max>tmp) ? wtbuf2pe_loop_max : tmp;
	}

	findBinaryLength(&wrd_1row_b, wrd_1row_max);

	findBinaryLength(&row_1map_b, row_1map_max);

	findBinaryLength(&conv_loop_b, conv_loop_max);

	findBinaryLength(&tile_loop_b, tile_loop_max);

	findBinaryLength(&wnd_loop_b, wnd_loop_max);

	findBinaryLength(&wtbuf2pe_loop_b, wtbuf2pe_loop_max);

	std::cout << "Maximum of wrd_1row is -> " << wrd_1row_max
			  << "\nNeed " << wrd_1row_b << " binary digits \n\n";
	std::cout << "Maximum of row_1map is -> " << row_1map_max
			  << "\nNeed " << row_1map_b << " binary digits \n\n";
	std::cout << "Maximum of convLayer loop limit is -> " << conv_loop_max
			  << "\nNeed " << conv_loop_b << " binary digits \n\n";
	std::cout << "Maximum of tileClc loop limit is -> " << tile_loop_max
			  << "\nNeed " << tile_loop_b << " binary digits \n\n";
	std::cout << "Maximum of wndClc loop limit is -> " << wnd_loop_max
			  << "\nNeed " << wnd_loop_b << " binary digits \n\n";
	std::cout << "Maximum of WtBuf2Pe loop limit is -> " << wtbuf2pe_loop_max
			  << "\nNeed " << wtbuf2pe_loop_b << " binary digits \n\n";

	// Calculate binary length for binary variables (5)
	int row_out_b, row_out_i_b;
	int wrd_wtbuf_b, wrd_wtbuf_i_b;
	int wrd_outbuf_b, wrd_outbuf_i_b;
	int wrd_biasbuf_b, wrd_biasbuf_i_b;
	int outbufnum_b, outbufnum_i_b;

	findBinaryLength(&row_out_b, WRD_INBUF);
	findBinaryLength(&row_out_i_b, WRD_INBUF-1);

	findBinaryLength(&wrd_wtbuf_b, WRD_WTBUF);
	findBinaryLength(&wrd_wtbuf_i_b, WRD_WTBUF-1);

	findBinaryLength(&wrd_outbuf_b, WRD_OUTBUF);
	findBinaryLength(&wrd_outbuf_i_b, WRD_OUTBUF-1);

	findBinaryLength(&wrd_biasbuf_b, BIASBUF_LENGTH);
	findBinaryLength(&wrd_biasbuf_i_b, BIASBUF_LENGTH-1);

	findBinaryLength(&outbufnum_b, OUTBUF_NUM);
	findBinaryLength(&outbufnum_i_b, OUTBUF_NUM-1);

	std::cout << "WRD_INBUF is -> " << WRD_INBUF
			  << "\nSo for row_out I need " << row_out_b << " binary digits \n"
			  << "And " << row_out_i_b << " binary digits for counter\n\n";
	std::cout << "WRD_WTBUF is -> " << WRD_WTBUF
			  << "\nSo for wrd_wtbuf I need " << wrd_wtbuf_b << " binary digits \n"
			  << "And " << wrd_wtbuf_i_b << " binary digits for counter\n\n";
	std::cout << "WRD_OUTBUF is -> " << WRD_OUTBUF
			  << "\nSo for wrd_outbuf I need " << wrd_outbuf_b << " binary digits \n"
			  << "And " << wrd_outbuf_i_b << " binary digits for counter\n\n";
	std::cout << "BIASBUF_LENGTH is -> " << BIASBUF_LENGTH
			  << "\nSo for wrd_biasbuf I need " << wrd_biasbuf_b << " binary digits \n"
			  << "And " << wrd_biasbuf_i_b << " binary digits for counter\n\n";
	std::cout << "OUTBUF_NUM is -> " << OUTBUF_NUM
			  << "\nSo for outbufnum I need " << outbufnum_b << " binary digits \n"
			  << "And " << outbufnum_i_b << " binary digits for counter\n\n";
}

void findBinaryLength(int *length, int bound){
	int i = 0;
	while(pow(2, i) <= bound){
		i++;
	}
	*length = i;
}

void printParameters(){
	std::cout << " Layers: " << LAYERS << std::endl;
	std::cout << " NKX: " << NKX << std::endl;
	std::cout << " NKY: " << NKY << std::endl;
	std::cout << " S: " << S << std::endl;
	std::cout << " Nif_rom: ";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Nif_rom[i];
	}
	std::cout << "\n Noy_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Noy_rom[i];
	}
	std::cout << "\n Tox_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Tox_rom[i];
	}
	std::cout << " WRD_INBUF: " << WRD_INBUF << std::endl;
	std::cout << " WRD_WTBUF: " << WRD_WTBUF << std::endl;
	std::cout << " WRD_OUTBUF: " << WRD_OUTBUF << std::endl;
	std::cout << " BIASBUF_LENGTH: " << BIASBUF_LENGTH << std::endl;
	std::cout << " OUTBUF_NUM: " << OUTBUF_NUM << std::endl;
	std::cout << " BIASMEM_LENGTH: " << BIASMEM_LENGTH << std::endl;
	std::cout << " FMAP_MEMSIZE: " << FMAP_MEMSIZE << std::endl;
	std::cout << " FMAP_MEMSIZE_WIDENED: " << FMAP_MEMSIZE_WIDENED << std::endl;
	std::cout << " WTMAP_MEMSIZE: " << WTMAP_MEMSIZE << std::endl;
	std::cout << " WTMAP_MEMSIZE_WIDENED: " << WTMAP_MEMSIZE_WIDENED << std::endl;
	std::cout << " niy_rom: ";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << niy_rom[i];
	}
	std::cout << "\n tox_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << tox_step_rom[i];
	}
	std::cout << "\n tix_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << tix_rom[i];
	}
	std::cout << "\n wrd_1row_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << wrd_1row_rom[i];
	}
	std::cout << "\n pe_loop_limit_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << pe_loop_limit_rom[i];
	}
	std::cout << "\n wndclc_loop_limit_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << wndclc_loop_limit_rom[i];
	}
	std::cout << "\n nofFirst:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << nofFirst[i];
	}
	std::cout << "\n fulBufWt:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << fulBufWt[i];
	}
	std::cout << "\n fulBufPx:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << fulBufPx[i];
	}
	std::cout << "\n bit_shift_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << bit_shift_rom[i];
	}
	std::cout << " PIF: " << PIF << std::endl;
	std::cout << " POF: " << POF << std::endl;
	std::cout << " POY: " << POY << std::endl;
	std::cout << " POX: " << POX << std::endl;
	std::cout << " PIY: " << PIY << std::endl;
	std::cout << " PIX: " << PIX << std::endl;
	std::cout << "\n nofy_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << nofy_step_rom[i];
	}
	std::cout << "\n Nof_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Nof_step_rom[i];
	}
	std::cout << "\n Tof_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Tof_rom[i];
	}
	std::cout << "\n noy_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << noy_step_rom[i];
	}
	std::cout << "\n Toy_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << Toy_rom[i];
	}
	std::cout << "\n tiy_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << tiy_rom[i];
	}
	std::cout << " POFBANK_STEP: " << POFBANK_STEP << std::endl;
	std::cout << "\n tof_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << tof_step_rom[i];
	}
	std::cout << "\n toy_step_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << toy_step_rom[i];
	}
	std::cout << "\n row_1map_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << row_1map_rom[i];
	}
	std::cout << "\n wtbuf2pe_loop_limit_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << wtbuf2pe_loop_limit_rom[i];
	}
	std::cout << "\n tileclc_loop_limit_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << tileclc_loop_limit_rom[i];
	}
	std::cout << "\n pe2buf_addr_offset1_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << pe2buf_addr_offset1_rom[i];
	}

	std::cout << "\n pe2buf_addr_offset2_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << pe2buf_addr_offset2_rom[i];
	}
	std::cout << "\n pe2buf_addr_offset3_rom:";
	for(int i=0;i<LAYERS;i++){
		std::cout << std::setw(7) << pe2buf_addr_offset3_rom[i];
	}
}