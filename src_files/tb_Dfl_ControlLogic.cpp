#include "header.h"
#include <cstdlib> // for rand()
#include <iostream>
#include <iomanip>

// ****** Testbench of Modules that Manage Dataflow or Control Logic  *****


/*
int wndClc_test(int verbose, int printLayer){
	// I call wndClc only once per layer, because there is no state registered values
	// that differ from tile to tile

	// Declare all variable parameters
	int Noy_step , Niy; // Noy_step = Noy/Toy 
	int Tof_step, Toy_step, Tox_step; // Tof_step=ceil(Tof/Pof), Toy_step=ceil(Toy/Poy), Tox_step=ceil(Tox/Pox)
	int Tiy, Tix;
	int row_1map, wrd_1row; // row_1map=ceil(Tiy/Poy), wrd_1row=ceil(Tix/Pox)

	static px_data_t InBuf[POY][WRD_INBUF][POX];
	static wt_data_t WtBuf[WRD_WTBUF][POF];
	px_data_t rslt_stream[POF][POY][POX], rslt_stream_golden[POF][POY][POX];
	px_data_t Compared_rslt_stream[POF][POY][POX] = {0};

	int rowStepAddress;

	int check = 0, printcheck;

	// Initialize InBuf
	for(int Wrd_InBuf_i=0;Wrd_InBuf_i<WRD_INBUF;Wrd_InBuf_i++){
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				InBuf[Poy_i][Wrd_InBuf_i][Pox_i] =
						//(Wrd_InBuf_i+1)*31 + (Poy_i+1)*7 + (Pox_i+1);
						( (Wrd_InBuf_i+1)*31 + (Poy_i+1)*7 + (Pox_i+1) )% 30;
			}
		}
	}

	// Initialize WtBuf
	for(int Wrd_WtBuf_i=0;Wrd_WtBuf_i<WRD_WTBUF;Wrd_WtBuf_i++){
		for(int Pof_i=0;Pof_i<POF;Pof_i++){
			WtBuf[Wrd_WtBuf_i][Pof_i] =
					rand()%3-1;
		}
	}

	// Print InBuf & WtBuf
	if(verbose || printLayer){
		printInBuf(InBuf);
		printWtBuf(WtBuf);
	}

	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

		for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
			for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
				for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
					rowStepAddress = Toy_step_i*wrd_1row + Tox_step_i;
					wndClc_Dfl(wndclc_loop_limit_rom[layerNo], wtbuf2pe_loop_limit_rom[layerNo],
							rowStepAddress, Nif_rom[layerNo], wrd_1row, row_1map, InBuf, WtBuf, rslt_stream);
					// wndClc(wndclc_loop_limit_rom[layerNo], wtbuf2pe_loop_limit_rom[layerNo],
					// 		rowStepAddress, Nif_rom[layerNo], wrd_1row, row_1map, InBuf, WtBuf, rslt_stream);
					wndClc_software(layerNo, Tof_step_i, Toy_step_i, Tox_step_i, InBuf, WtBuf, rslt_stream_golden);

					printcheck = 0;
					// Compare results
					for(int Pof_i=0;Pof_i<POF;Pof_i++){
						for(int Poy_i=0;Poy_i<POY;Poy_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								if(rslt_stream[Pof_i][Poy_i][Pox_i] != rslt_stream_golden[Pof_i][Poy_i][Pox_i]){
									check = 1; printcheck = 1;
								}
								Compared_rslt_stream[Pof_i][Poy_i][Pox_i] =
										rslt_stream[Pof_i][Poy_i][Pox_i] - rslt_stream_golden[Pof_i][Poy_i][Pox_i];
							}
						}
					}

					// Debugging Info
					if(verbose ||  (printLayer && printLayer == layerNo+1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
								<<  " ,Tof_step -> " << Tof_step_i << "\n\n";
						std::cout << "Printing MAC results under test" << std::endl;
						printPeResults(rslt_stream);
						std::cout << "Printing Golden MAC results" << std::endl;
						printPeResults(rslt_stream_golden);
						std::cout << "Printing Compared MAC results" << std::endl;
						printPeResults(Compared_rslt_stream);
						if(printcheck){
							std::cout << "There is an Error Here!" << std::endl;
						}
					}
				}
			}
		}

	}
	// Verification Print
	if(check){
		std::cout << "*****  Window Calculation test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "*****  wndClc Test Passed!  ******\n" << std::endl;
	}
	return check;
// }
*/


/*
int tileClc_test(int verbose, int printErrorOnly, int printLayer, int biasReLuTrue){
	// It is almost the same testbench with wndClc except the tile steps
	// of P* are iterated inside the module
	// That happens because I tried to keep perfect loops with excecution in the inner loops
	// I also call loadBiasTile to feed the correct biases, because Pe2Buf_software reads the bias tile
	// from the bias memory.

	// Declare all other parameters
	int Noy_step, Niy; // Noy_step = Noy/Toy
	int Tof_step, Toy_step, Tox_step; // Tof_step=ceil(Tof/Pof), Toy_step=ceil(Toy/Poy), Tox_step=ceil(Tox/Pox)
	int Tiy, Tix;
	int row_1map, wrd_1row; // row_1map=ceil(Tiy/Poy), wrd_1row=ceil(Tix/Pox)

	static px_data_t InBuf[POY][WRD_INBUF][POX];
	static wt_data_t WtBuf[WRD_WTBUF][POF];
	b_data_t BiasBuf[BIASBUF_LENGTH];
	static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX], OutBuf_golden[OUTBUF_NUM][WRD_OUTBUF][POX];
	static px_data_t Compared_OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];

	int check = 0, printcheck;

	// Initialize InBuf
	for(int Wrd_InBuf_i=0;Wrd_InBuf_i<WRD_INBUF;Wrd_InBuf_i++){
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				InBuf[Poy_i][Wrd_InBuf_i][Pox_i] =
						// (Wrd_InBuf_i+1)*31 + (Poy_i+1)*7 + (Pox_i+1);
						rand()%30;
			}
		}
	}

	// Initialize WtBuf
	for(int Wrd_WtBuf_i=0;Wrd_WtBuf_i<WRD_WTBUF;Wrd_WtBuf_i++){
		for(int Pof_i=0;Pof_i<POF;Pof_i++){
			WtBuf[Wrd_WtBuf_i][Pof_i] =
					// (Wrd_WtBuf_i+1)*11 + (Pof_i+1);
					rand()%7;
		}
	}

	// // Print InBuf & WtBuf
	// if(verbose || printLayer){
	// 	printInBuf(InBuf);
	// 	printWtBuf(WtBuf);
	// }

	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

		if(nofFirst[layerNo]){
			for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
				for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){

					// Initialize OutBuf & OutBuf_golden for undefined values
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] = -1;
								OutBuf_golden[OutBufNum_i][WrdOutBuf_i][Pox_i] = -1;
							}
						}
					}

					tileClc_Dfl(
							// Variable Parameters
							tileclc_loop_limit_rom[layerNo], wndclc_loop_limit_rom[layerNo],
							wtbuf2pe_loop_limit_rom[layerNo],
							Nif_rom[layerNo], Toy_rom[layerNo], toy_step_rom[layerNo], tox_step_rom[layerNo],
							tof_step_rom[layerNo], wrd_1row_rom[layerNo], row_1map_rom[layerNo],
							pe2buf_addr_offset1_rom[layerNo], pe2buf_addr_offset2_rom[layerNo],
							pe2buf_addr_offset3_rom[layerNo],
							bit_shift_rom[layerNo],
							// Inputs
							InBuf, WtBuf, BiasBuf,
							// Output
							OutBuf);
					tileClc_software(layerNo, Nof_step_i, InBuf, WtBuf, OutBuf_golden, biasReLuTrue);

					printcheck = 0;
					// Compare OutBuf
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								if(OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] != OutBuf_golden[OutBufNum_i][WrdOutBuf_i][Pox_i]){
									check = 1; printcheck = 1;
								}
								Compared_OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] =
										OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] - OutBuf_golden[OutBufNum_i][WrdOutBuf_i][Pox_i];
							}
						}
					}

					// Debugging Info
					if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
						std::cout << "\nFor Layer -> " << layerNo << " Nof_step -> " << Nof_step_i
								<< " Noy_step -> " << Noy_step_i << "\n\n";
						if(printcheck){
							std::cout << "There is an Error Here!" << std::endl;
						}
						std::cout << "Printing OutBuf under test" << std::endl;
						printOutBuf(layerNo, OutBuf);
						std::cout << "Printing Golden OutBuf" << std::endl;
						printOutBuf(layerNo, OutBuf_golden);
						std::cout << "Printing Compared OutBuf" << std::endl;
						printOutBuf(layerNo, Compared_OutBuf);
					}
				}
			}
		}
		else{
			for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
				for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){

					// Initialize OutBuf & OutBuf_golden for undefined values
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] = -1;
							}
						}
					}

					tileClc_Dfl(
							// Variable Parameters
							tileclc_loop_limit_rom[layerNo], wndclc_loop_limit_rom[layerNo],
							wtbuf2pe_loop_limit_rom[layerNo],
							Nif_rom[layerNo], Toy_rom[layerNo], toy_step_rom[layerNo], tox_step_rom[layerNo],
							tof_step_rom[layerNo], wrd_1row_rom[layerNo], row_1map_rom[layerNo],
							pe2buf_addr_offset1_rom[layerNo], pe2buf_addr_offset2_rom[layerNo],
							pe2buf_addr_offset3_rom[layerNo],
							bit_shift_rom[layerNo],
							// Inputs
							InBuf, WtBuf, BiasBuf,
							// Output
							OutBuf);
					tileClc_software(layerNo, Nof_step_i, InBuf, WtBuf, OutBuf_golden, biasReLuTrue);

					printcheck = 0;
					// Compare OutBuf
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								if(OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] != OutBuf_golden[OutBufNum_i][WrdOutBuf_i][Pox_i]){
									check = 1; printcheck = 1;
								}
								Compared_OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] =
										OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] - OutBuf_golden[OutBufNum_i][WrdOutBuf_i][Pox_i];
							}
						}
					}

					// Debugging Info
					if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
						std::cout << "\nFor Layer -> " << layerNo << " Nof_step -> " << Nof_step_i
								<< " Noy_step -> " << Noy_step_i << "\n\n";
						if(printcheck){
							std::cout << "There is an Error Here!" << std::endl;
						}
						std::cout << "Printing OutBuf under test" << std::endl;
						printOutBuf(layerNo, OutBuf);
						std::cout << "Printing Golden OutBuf" << std::endl;
						printOutBuf(layerNo, OutBuf_golden);
						std::cout << "Printing Compared OutBuf" << std::endl;
						printOutBuf(layerNo, Compared_OutBuf);
					}
				}
			}
		}

	}

	// Verification Print
	if(check){
		std::cout << "*****  Tile Calculation test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "*****  tileClc Test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
	}
	return check;
}
*/


int convLayer_test(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput){
	// Memory Data
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#if defined(WTMAP_WIDEN)
		wt_data_t* WtMap_reordered = nullptr;
		WtMap_reordered = new wt_data_t[WTMAP_MEMSIZE];
		wt_data_t_port* WtMap_port = nullptr;
		WtMap_port = new wt_data_t_port[WTMAP_MEMSIZE_WIDENED];
	#endif
	static px_data_t OfMap[FMAP_MEMSIZE] = {0}, OfMap_golden[FMAP_MEMSIZE] = {0};
	static px_data_t Compared_OfMap[FMAP_MEMSIZE];
	int check = 0, printcheck;
	std::cout << "Reached HERE!1" << std::endl;
	for(int layerNo=0;layerNo<2;layerNo++){
		std::cout << "*****  Layer " << layerNo+1 << "  *****" << std::endl;
		// Initialize Memories
		px_data_t* IfMap = initIfMap(layerNo, binInput);
		wt_data_t* WtMap = initWtMap(layerNo, binInput);
		std::cout << "Reached HERE2!" << std::endl;
		convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
		std::cout << "Reached HERE3!" << std::endl;
		#if defined(WTMAP_WIDEN)
			wt_reorder(WtMap, WtMap_reordered, layerNo);
			pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
		#else
			wt_data_t_port* WtMap_port = WtMap;
		#endif
		std::cout << "Reached HERE4!" << std::endl;
		std::cout << "Reached HERE4aqwe!" << std::endl;
		#if defined(FMAP_WIDEN)
			pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
			std::cout << "Reached HERE5!" << std::endl;
			ConvLayer(IfMap_port, WtMap_port, OfMap_port);
			std::cout << "Got HERE!" << std::endl;
			std::cout << "Did Got HERE!" << std::endl;
			unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		#else
			ConvLayer(IfMap, WtMap_port, OfMap);
		#endif
		std::cout << "Reached HERE!" << std::endl;
		printcheck = 0;
		// Compare Output Feature Maps
		int error_count = 0, error_positions[20], error_values[20], error_values_synth[20], error_values_tb[20];
		for(int i=0;i<FMAP_MEMSIZE;i++){
			Compared_OfMap[i] = OfMap[i] - OfMap_golden[i];
			if(OfMap[i] != OfMap_golden[i]){
				check = 1; printcheck = 1;
				if(error_count<20){
					error_positions[error_count] = i;
					error_values[error_count] = Compared_OfMap[i];
					error_values_synth[error_count] = OfMap[i];
					error_values_tb[error_count] = OfMap_golden[i];
				}
				error_count++;
			}
		}

		int outofrange_synth = 0, outofrange_synth_positions[20], outofrange_synth_values[20];
		int outofrange_tb = 0, outofrange_tb_positions[20], outofrange_tb_values[20];
		for(int i=0;i<FMAP_MEMSIZE;i++){
			if(OfMap[i] < -HW_EMUL_SYMM_RANGE || OfMap[i] > HW_EMUL_SYMM_RANGE){
				if(outofrange_synth<20){
					outofrange_synth_positions[outofrange_synth] = i;
					outofrange_synth_values[outofrange_synth] = OfMap[i];
				}
				outofrange_synth++;
			}
			if(OfMap_golden[i] < -HW_EMUL_SYMM_RANGE || OfMap_golden[i] > HW_EMUL_SYMM_RANGE){
				if(outofrange_tb<20){
					outofrange_tb_positions[outofrange_tb] = i;
					outofrange_tb_values[outofrange_tb] = OfMap_golden[i];
				}
				outofrange_tb++;
			}
		}

		// Print Debugging Info
		if(debug){
			if(printcheck){
				std::cout << "\nNumber of errors is  " << error_count << std::endl;
				std::cout << "\nPositions of errors (first 20) are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_positions[i] << "   ";
				}
				std::cout << "\nValues of errors (first 20) for synth module are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_values_synth[i] << "   ";
				}
				std::cout << "\nValues of errors (first 20) for tb module are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_values_tb[i] << "   ";
				}
			}
			if(outofrange_synth){
				std::cout << "\nNumber of out of range synth values are:" << outofrange_synth << std::endl;
				std::cout << "\nPositions of out of range values (first 20) for synth module are:\n";
				for(int i=0;i<outofrange_synth && i<20;i++){
					std::cout << outofrange_synth_positions[i] << "   ";
				}
				std::cout << "\nValues out of range (first 20) for synth module are:\n";
				for(int i=0;i<outofrange_synth && i<20;i++){
					std::cout << outofrange_synth_values[i] << "   ";
				}
			}
			if(outofrange_tb){
				std::cout << "Number of out of range tb values are:" << outofrange_tb << std::endl;
				std::cout << "\nPositions of out of range values (first 20) for TB are:\n";
				for(int i=0;i<outofrange_tb && i<20;i++){
					std::cout << outofrange_tb_positions[i] << "   ";
				}

				std::cout << "\nValues out of range (first 20) for TB are:\n";
				for(int i=0;i<outofrange_tb && i<20;i++){
					std::cout << outofrange_tb_values[i] << "   ";
				}
			}
			std::cout << "\n";
		}
		if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
			std::cout << "Printing IfMap of Layer" << layerNo+1 << std::endl;
			printIfMap(layerNo, IfMap, minPrint);
			std::cout << "Printing WtMap of Layer" << layerNo+1 << std::endl;
			printWtMap(layerNo, WtMap, minPrint);
			#if defined(WTMAP_WIDEN)
			std::cout << "Printing WtMap reordered of Layer" << layerNo+1 << std::endl;
			printWtMap(layerNo, WtMap_reordered, minPrint);
			#endif
			std::cout << "Printing OfMap under test of Layer" << layerNo+1 << std::endl;
			printOfMap(layerNo, OfMap, minPrint);
			std::cout << "Printing Golden OfMap of Layer" << layerNo+1 << std::endl;
			printOfMap(layerNo, OfMap_golden, minPrint);
			std::cout << "Printing Compared OfMap of Layer " << layerNo+1 << std::endl;
			printOfMap(layerNo, Compared_OfMap, minPrint);
		}
	}

	// Verification Print
	if(check){
		std::cout << "*****  Convolutional Layer ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "Printing first 20 elements of last output layer" << std::endl;
		for(int i=0;i<20;i++){
			std::cout << std::setw(8) << OfMap[i];
		}
		std::cout << "\n\n*****  ConvLayer Test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
	}
	return check;
}


int convLayer_test_experimental_new(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput){
	// Memory Data
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#if defined(WTMAP_WIDEN)
		wt_data_t* WtMap_reordered = nullptr;
		WtMap_reordered = new wt_data_t[WTMAP_MEMSIZE];
		wt_data_t_port* WtMap_port = nullptr;
		WtMap_port = new wt_data_t_port[WTMAP_MEMSIZE_WIDENED];
	#endif
	static px_data_t OfMap_golden_tmp[FMAP_MEMSIZE] = {0}, OfMap_golden_tmp2[FMAP_MEMSIZE] = {0}, OfMap_golden[FMAP_MEMSIZE] = {0};
	static px_data_t OfMap[FMAP_MEMSIZE] = {0},Compared_OfMap[FMAP_MEMSIZE];
	int check = 0, printcheck;
	std::cout << "Reached HERE!1" << std::endl;
	for(int layerNo=0;layerNo<LAYERS-1;layerNo++){
		px_data_t* IfMap = initIfMap(layerNo, binInput);
		wt_data_t* WtMap = initWtMap(layerNo, binInput);
		#if defined(WTMAP_WIDEN)
			wt_reorder(WtMap, WtMap_reordered, layerNo);
			pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
		#else
			wt_data_t_port* WtMap_port = WtMap;
		#endif
		std::cout << "Reached HERE4!" << std::endl;
		#if defined(FMAP_WIDEN)
			pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
			std::cout << "Reached HERE5!" << std::endl;
			ConvLayer(IfMap_port, WtMap_port, OfMap_port);
			unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		#else
			ConvLayer(IfMap, WtMap_port, OfMap);
		#endif
	}
	int layerNo = 12;
	
	std::cout << "*****  Layer " << layerNo+1 << "  *****" << std::endl;
	// Initialize Memories
	px_data_t* IfMap = initIfMap(layerNo, binInput);
	wt_data_t* WtMap = initWtMap(layerNo, binInput);
	std::cout << "Reached HERE2!" << std::endl;
	// convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
	#if defined(MAXPOOL_INTEGRATION)
		if(layerNo==1 || layerNo==3 || layerNo==6 || layerNo==9){
			convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
			maxPool(OfMap_golden_tmp, Nif_rom[layerNo+1], Noy_rom[layerNo+1], Tox_rom[layerNo+1], OfMap_golden);
		}
		else if(layerNo==12){
			#if defined(HEAD_INTEGRATION)
				convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
				maxPool(OfMap_golden_tmp, 512, 7, 7, OfMap_golden_tmp2);
				gap(OfMap_golden_tmp2,OfMap_golden);
			#elif not defined(HEAD_INTEGRATION)
				convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
				maxPool(OfMap_golden_tmp, 512, 7, 7, OfMap_golden);
			#endif
		}
		else{
			convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
		}
	#else
		convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
	#endif
	std::cout << "Reached HERE3!" << std::endl;
	#if defined(WTMAP_WIDEN)
		wt_reorder(WtMap, WtMap_reordered, layerNo);
		pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
	#else
		wt_data_t_port* WtMap_port = WtMap;
	#endif
	std::cout << "Reached HERE4!" << std::endl;
	#if defined(FMAP_WIDEN)
		pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		std::cout << "Reached HERE5!" << std::endl;
		ConvLayer(IfMap_port, WtMap_port, OfMap_port);
		unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
	#else
		ConvLayer(IfMap, WtMap_port, OfMap);
	#endif
	std::cout << "Reached HERE!" << std::endl;
	printcheck = 0;
	// Compare Output Feature Maps
	int error_count = 0, error_positions[20], error_values[20], error_values_synth[20], error_values_tb[20];
	for(int i=0;i<FMAP_MEMSIZE;i++){
		Compared_OfMap[i] = OfMap[i] - OfMap_golden[i];
		if(OfMap[i] != OfMap_golden[i]){
			check = 1; printcheck = 1;
			if(error_count<20){
				error_positions[error_count] = i;
				error_values[error_count] = Compared_OfMap[i];
				error_values_synth[error_count] = OfMap[i];
				error_values_tb[error_count] = OfMap_golden[i];
			}
			error_count++;
		}
	}

	int outofrange_synth = 0, outofrange_synth_positions[20], outofrange_synth_values[20];
	int outofrange_tb = 0, outofrange_tb_positions[20], outofrange_tb_values[20];
	for(int i=0;i<FMAP_MEMSIZE;i++){
		if(OfMap[i] < -HW_EMUL_SYMM_RANGE || OfMap[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_synth<20){
				outofrange_synth_positions[outofrange_synth] = i;
				outofrange_synth_values[outofrange_synth] = OfMap[i];
			}
			outofrange_synth++;
		}
		if(OfMap_golden[i] < -HW_EMUL_SYMM_RANGE || OfMap_golden[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_tb<20){
				outofrange_tb_positions[outofrange_tb] = i;
				outofrange_tb_values[outofrange_tb] = OfMap_golden[i];
			}
			outofrange_tb++;
		}
	}

	// Print Debugging Info
	if(debug){
		if(printcheck){
			std::cout << "\nNumber of errors is  " << error_count << std::endl;
			std::cout << "\nPositions of errors (first 20) are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_positions[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for synth module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_synth[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for tb module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_tb[i] << "   ";
			}
		}
		if(outofrange_synth){
			std::cout << "\nNumber of out of range synth values are:" << outofrange_synth << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_positions[i] << "   ";
			}
			std::cout << "\nValues out of range (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_values[i] << "   ";
			}
		}
		if(outofrange_tb){
			std::cout << "Number of out of range tb values are:" << outofrange_tb << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_positions[i] << "   ";
			}

			std::cout << "\nValues out of range (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_values[i] << "   ";
			}
		}
		std::cout << "\n";
	}
	if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
		std::cout << "Printing IfMap of Layer" << layerNo+1 << std::endl;
		printIfMap(layerNo, IfMap, minPrint);
		std::cout << "Printing WtMap of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap, minPrint);
		#if defined(WTMAP_WIDEN)
		std::cout << "Printing WtMap reordered of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap_reordered, minPrint);
		#endif
		std::cout << "Printing OfMap under test of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap, minPrint);
		std::cout << "Printing Golden OfMap before max pooling of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden_tmp, minPrint);
		std::cout << "Printing Golden OfMap before gap of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden_tmp2, minPrint);
		std::cout << "Printing Golden OfMap of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden, minPrint);
		std::cout << "Printing Compared OfMap of Layer " << layerNo+1 << std::endl;
		printOfMap(layerNo, Compared_OfMap, minPrint);
	}

	// Verification Print
	if(check){
		std::cout << "*****  Convolutional Layer ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "Printing first 20 elements of last output layer" << std::endl;
		for(int i=0;i<20;i++){
			std::cout << std::setw(8) << OfMap[i];
		}
		std::cout << "\n\n*****  ConvLayer Test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
	}
	return check;
}


// This is the tested version
int convLayer_test_experimental(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput){
	// Memory Data
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#if defined(WTMAP_WIDEN)
		wt_data_t* WtMap_reordered = nullptr;
		WtMap_reordered = new wt_data_t[WTMAP_MEMSIZE];
		wt_data_t_port* WtMap_port = nullptr;
		WtMap_port = new wt_data_t_port[WTMAP_MEMSIZE_WIDENED];
	#endif
	static px_data_t OfMap_golden_tmp[FMAP_MEMSIZE] = {0}, OfMap_golden_tmp2[FMAP_MEMSIZE] = {0}, OfMap_golden[FMAP_MEMSIZE] = {0};
	static px_data_t OfMap[FMAP_MEMSIZE] = {0},Compared_OfMap[FMAP_MEMSIZE];
	int check = 0, printcheck;
	std::cout << "Reached HERE!1" << std::endl;
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		std::cout << "*****  Layer " << layerNo+1 << "  *****" << std::endl;
		// Initialize Memories
		px_data_t* IfMap = initIfMap(layerNo, binInput);
		wt_data_t* WtMap = initWtMap(layerNo, binInput);
		std::cout << "Reached HERE2!" << std::endl;
		#if defined(MAXPOOL_INTEGRATION)
			if(layerNo==1 || layerNo==3 || layerNo==6 || layerNo==9){
				convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
				maxPool(OfMap_golden_tmp, Nif_rom[layerNo+1], Noy_rom[layerNo+1], Tox_rom[layerNo+1], OfMap_golden);
			}
			else if(layerNo==12){
				#if defined(HEAD_INTEGRATION)
					convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
					maxPool(OfMap_golden_tmp, 512, 7, 7, OfMap_golden_tmp2);
					gap(OfMap_golden_tmp2,OfMap_golden);
				#elif not defined(HEAD_INTEGRATION)
					convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
					maxPool(OfMap_golden_tmp, 512, 7, 7, OfMap_golden);
				#endif
			}
			else{
				convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
			}
		#else
			convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
		#endif
		std::cout << "Reached HERE3!" << std::endl;
		std::cout << "Reached HERE3333!" << std::endl;
		#if defined(WTMAP_WIDEN)
			wt_reorder(WtMap, WtMap_reordered, layerNo);
			pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
		#else
			wt_data_t_port* WtMap_port = WtMap;
		#endif
		std::cout << "Reached HERE4!" << std::endl;
		std::cout << "Reached HERE44!" << std::endl;
		#if defined(FMAP_WIDEN)
			pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
			std::cout << "Reached HERE5!" << std::endl;
			ConvLayer(IfMap_port, WtMap_port, OfMap_port);
			unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		#else
			ConvLayer(IfMap, WtMap_port, OfMap);
		#endif
		std::cout << "Reached HERE!" << std::endl;
		printcheck = 0;
		// Compare Output Feature Maps
		int error_count = 0, error_positions[20], error_values[20], error_values_synth[20], error_values_tb[20];
		for(int i=0;i<FMAP_MEMSIZE;i++){
			Compared_OfMap[i] = OfMap[i] - OfMap_golden[i];
			if(OfMap[i] != OfMap_golden[i]){
				check = 1; printcheck = 1;
				if(error_count<20){
					error_positions[error_count] = i;
					error_values[error_count] = Compared_OfMap[i];
					error_values_synth[error_count] = OfMap[i];
					error_values_tb[error_count] = OfMap_golden[i];
				}
				error_count++;
			}
		}

		int outofrange_synth = 0, outofrange_synth_positions[20], outofrange_synth_values[20];
		int outofrange_tb = 0, outofrange_tb_positions[20], outofrange_tb_values[20];
		for(int i=0;i<FMAP_MEMSIZE;i++){
			if(OfMap[i] < -HW_EMUL_SYMM_RANGE || OfMap[i] > HW_EMUL_SYMM_RANGE){
				if(outofrange_synth<20){
					outofrange_synth_positions[outofrange_synth] = i;
					outofrange_synth_values[outofrange_synth] = OfMap[i];
				}
				outofrange_synth++;
			}
			if(OfMap_golden[i] < -HW_EMUL_SYMM_RANGE || OfMap_golden[i] > HW_EMUL_SYMM_RANGE){
				if(outofrange_tb<20){
					outofrange_tb_positions[outofrange_tb] = i;
					outofrange_tb_values[outofrange_tb] = OfMap_golden[i];
				}
				outofrange_tb++;
			}
		}

		// Print Debugging Info
		if(debug){
			if(printcheck){
				std::cout << "\nNumber of errors is  " << error_count << std::endl;
				std::cout << "\nPositions of errors (first 20) are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_positions[i] << "   ";
				}
				std::cout << "\nValues of errors (first 20) for synth module are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_values_synth[i] << "   ";
				}
				std::cout << "\nValues of errors (first 20) for tb module are:\n";
				for(int i=0;i<error_count && i<20;i++){
					std::cout << error_values_tb[i] << "   ";
				}
			}
			if(outofrange_synth){
				std::cout << "\nNumber of out of range synth values are:" << outofrange_synth << std::endl;
				std::cout << "\nPositions of out of range values (first 20) for synth module are:\n";
				for(int i=0;i<outofrange_synth && i<20;i++){
					std::cout << outofrange_synth_positions[i] << "   ";
				}
				std::cout << "\nValues out of range (first 20) for synth module are:\n";
				for(int i=0;i<outofrange_synth && i<20;i++){
					std::cout << outofrange_synth_values[i] << "   ";
				}
			}
			if(outofrange_tb){
				std::cout << "Number of out of range tb values are:" << outofrange_tb << std::endl;
				std::cout << "\nPositions of out of range values (first 20) for TB are:\n";
				for(int i=0;i<outofrange_tb && i<20;i++){
					std::cout << outofrange_tb_positions[i] << "   ";
				}

				std::cout << "\nValues out of range (first 20) for TB are:\n";
				for(int i=0;i<outofrange_tb && i<20;i++){
					std::cout << outofrange_tb_values[i] << "   ";
				}
			}
			std::cout << "\n";
		}
		if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
			std::cout << "Printing IfMap of Layer" << layerNo+1 << std::endl;
			printIfMap(layerNo, IfMap, minPrint);
			std::cout << "Printing WtMap of Layer" << layerNo+1 << std::endl;
			printWtMap(layerNo, WtMap, minPrint);
			#if defined(WTMAP_WIDEN)
			std::cout << "Printing WtMap reordered of Layer" << layerNo+1 << std::endl;
			printWtMap(layerNo, WtMap_reordered, minPrint);
			#endif
			std::cout << "Printing OfMap under test of Layer" << layerNo+1 << std::endl;
			printOfMap(layerNo, OfMap, minPrint);
			std::cout << "Printing Golden OfMap of Layer" << layerNo+1 << std::endl;
			printOfMap(layerNo, OfMap_golden, minPrint);
			std::cout << "Printing Compared OfMap of Layer " << layerNo+1 << std::endl;
			printOfMap(layerNo, Compared_OfMap, minPrint);
		}
	}

	// Verification Print
	if(check){
		std::cout << "*****  Convolutional Layer ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "Printing first 20 elements of last output layer" << std::endl;
		for(int i=0;i<20;i++){
			std::cout << std::setw(8) << OfMap[i];
		}
		std::cout << "\n\n*****  ConvLayer Test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
	}
	return check;
}


int convLayer_experimentalold(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput){
	// Memory Data
	// static px_data_t IfMap[FMAP_MEMSIZE*2] = {0};
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#if defined(WTMAP_WIDEN)
		wt_data_t* WtMap_reordered = nullptr;
		WtMap_reordered = new wt_data_t[WTMAP_MEMSIZE];
		wt_data_t_port* WtMap_port = nullptr;
		WtMap_port = new wt_data_t_port[WTMAP_MEMSIZE_WIDENED];
	#endif
	// static wt_data_t WtMap[WTMAP_MEMSIZE] = {0};
	static px_data_t OfMap[FMAP_MEMSIZE] = {0}, OfMap_golden_tmp[FMAP_MEMSIZE] = {0}, OfMap_golden[FMAP_MEMSIZE] = {0};
	static px_data_t Compared_OfMap[FMAP_MEMSIZE];
	int check = 0, printcheck;
	std::cout << "Reached HERE!1" << std::endl;

	int layerNo = 0;

	std::cout << "*****  Layer " << layerNo+1 << "  *****" << std::endl;
	// Initialize Memories
	px_data_t* IfMap = initIfMap(layerNo, binInput);
	wt_data_t* WtMap = initWtMap(layerNo, binInput);
	std::cout << "Reached HERE2!" << std::endl;
	convLayer_software(layerNo, IfMap, WtMap, OfMap_golden, biasReLuTrue);
	std::cout << "Reached HERE3!" << std::endl;
	#if defined(WTMAP_WIDEN)
		wt_reorder(WtMap, WtMap_reordered, layerNo);
		pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
	#else
		wt_data_t_port* WtMap_port = WtMap;
	#endif
	std::cout << "Reached HERE4!" << std::endl;
	#if defined(FMAP_WIDEN)
		pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		std::cout << "Reached HERE5!" << std::endl;
		ConvLayer(IfMap_port, WtMap_port, OfMap_port);
		unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
	#else
		ConvLayer(IfMap, WtMap_port, OfMap);
	#endif
	std::cout << "Reached HERE!" << std::endl;
	printcheck = 0;
	// Compare Output Feature Maps
	int error_count = 0, error_positions[20], error_values[20], error_values_synth[20], error_values_tb[20];
	for(int i=0;i<FMAP_MEMSIZE;i++){
		Compared_OfMap[i] = OfMap[i] - OfMap_golden[i];
		if(OfMap[i] != OfMap_golden[i]){
			check = 1; printcheck = 1;
			if(error_count<20){
				error_positions[error_count] = i;
				error_values[error_count] = Compared_OfMap[i];
				error_values_synth[error_count] = OfMap[i];
				error_values_tb[error_count] = OfMap_golden[i];
			}
			error_count++;
		}
	}

	int outofrange_synth = 0, outofrange_synth_positions[20], outofrange_synth_values[20];
	int outofrange_tb = 0, outofrange_tb_positions[20], outofrange_tb_values[20];
	for(int i=0;i<FMAP_MEMSIZE;i++){
		if(OfMap[i] < -HW_EMUL_SYMM_RANGE || OfMap[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_synth<20){
				outofrange_synth_positions[outofrange_synth] = i;
				outofrange_synth_values[outofrange_synth] = OfMap[i];
			}
			outofrange_synth++;
		}
		if(OfMap_golden[i] < -HW_EMUL_SYMM_RANGE || OfMap_golden[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_tb<20){
				outofrange_tb_positions[outofrange_tb] = i;
				outofrange_tb_values[outofrange_tb] = OfMap_golden[i];
			}
			outofrange_tb++;
		}
	}

	// Print Debugging Info
	if(debug){
		if(printcheck){
			std::cout << "\nNumber of errors is  " << error_count << std::endl;
			std::cout << "\nPositions of errors (first 20) are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_positions[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for synth module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_synth[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for tb module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_tb[i] << "   ";
			}
		}
		if(outofrange_synth){
			std::cout << "\nNumber of out of range synth values are:" << outofrange_synth << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_positions[i] << "   ";
			}
			std::cout << "\nValues out of range (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_values[i] << "   ";
			}
		}
		if(outofrange_tb){
			std::cout << "Number of out of range tb values are:" << outofrange_tb << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_positions[i] << "   ";
			}

			std::cout << "\nValues out of range (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_values[i] << "   ";
			}
		}
		std::cout << "\n";
	}
	if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
		std::cout << "Printing IfMap of Layer" << layerNo+1 << std::endl;
		printIfMap(layerNo, IfMap, minPrint);
		std::cout << "Printing WtMap of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap, minPrint);
		#if defined(WTMAP_WIDEN)
		std::cout << "Printing WtMap reordered of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap_reordered, minPrint);
		#endif
		std::cout << "Printing OfMap under test before max pool of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden_tmp, minPrint);
		std::cout << "Printing OfMap under test of Layer" << layerNo+1 << std::endl;
		printIfMap(layerNo+1, OfMap, minPrint);
		std::cout << "Printing Golden OfMap of Layer" << layerNo+1 << std::endl;
		printIfMap(layerNo+1, OfMap_golden, minPrint);
		std::cout << "Printing Compared OfMap of Layer " << layerNo+1 << std::endl;
		printOfMap(layerNo+1, Compared_OfMap, minPrint);
	}

	layerNo = 1;

	std::cout << "*****  Layer " << layerNo+1 << "  *****" << std::endl;
	// Initialize Memories
	IfMap = initIfMap(layerNo, binInput);
	WtMap = initWtMap(layerNo, binInput);
	std::cout << "Reached HERE2!" << std::endl;
	convLayer_software(layerNo, IfMap, WtMap, OfMap_golden_tmp, biasReLuTrue);
	maxPool(OfMap_golden_tmp, 64, 112, 112, OfMap_golden);
	std::cout << "Reached HERE3!" << std::endl;
	#if defined(WTMAP_WIDEN)
		wt_reorder(WtMap, WtMap_reordered, layerNo);
		pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
	#else
		WtMap_port = WtMap;
	#endif
	std::cout << "Reached HERE4!" << std::endl;
	#if defined(FMAP_WIDEN)
		pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		std::cout << "Reached HERE5!" << std::endl;
		ConvLayer(IfMap_port, WtMap_port, OfMap_port);
		unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
	#else
		ConvLayer(IfMap, WtMap_port, OfMap);
	#endif
	std::cout << "Reached HERE!" << std::endl;
	printcheck = 0;
	// Compare Output Feature Maps
	error_count = 0;
	for(int i=0;i<FMAP_MEMSIZE;i++){
		Compared_OfMap[i] = OfMap[i] - OfMap_golden[i];
		if(OfMap[i] != OfMap_golden[i]){
			check = 1; printcheck = 1;
			if(error_count<20){
				error_positions[error_count] = i;
				error_values[error_count] = Compared_OfMap[i];
				error_values_synth[error_count] = OfMap[i];
				error_values_tb[error_count] = OfMap_golden[i];
			}
			error_count++;
		}
	}

	outofrange_synth = 0;
	outofrange_tb = 0;
	for(int i=0;i<FMAP_MEMSIZE;i++){
		if(OfMap[i] < -HW_EMUL_SYMM_RANGE || OfMap[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_synth<20){
				outofrange_synth_positions[outofrange_synth] = i;
				outofrange_synth_values[outofrange_synth] = OfMap[i];
			}
			outofrange_synth++;
		}
		if(OfMap_golden[i] < -HW_EMUL_SYMM_RANGE || OfMap_golden[i] > HW_EMUL_SYMM_RANGE){
			if(outofrange_tb<20){
				outofrange_tb_positions[outofrange_tb] = i;
				outofrange_tb_values[outofrange_tb] = OfMap_golden[i];
			}
			outofrange_tb++;
		}
	}

	// Print Debugging Info
	if(debug){
		if(printcheck){
			std::cout << "\nNumber of errors is  " << error_count << std::endl;
			std::cout << "\nPositions of errors (first 20) are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_positions[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for synth module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_synth[i] << "   ";
			}
			std::cout << "\nValues of errors (first 20) for tb module are:\n";
			for(int i=0;i<error_count && i<20;i++){
				std::cout << error_values_tb[i] << "   ";
			}
		}
		if(outofrange_synth){
			std::cout << "\nNumber of out of range synth values are:" << outofrange_synth << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_positions[i] << "   ";
			}
			std::cout << "\nValues out of range (first 20) for synth module are:\n";
			for(int i=0;i<outofrange_synth && i<20;i++){
				std::cout << outofrange_synth_values[i] << "   ";
			}
		}
		if(outofrange_tb){
			std::cout << "Number of out of range tb values are:" << outofrange_tb << std::endl;
			std::cout << "\nPositions of out of range values (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_positions[i] << "   ";
			}

			std::cout << "\nValues out of range (first 20) for TB are:\n";
			for(int i=0;i<outofrange_tb && i<20;i++){
				std::cout << outofrange_tb_values[i] << "   ";
			}
		}
		std::cout << "\n";
	}
	if(verbose || printErrorOnly*printcheck || (printLayer && printLayer == layerNo+1)){
		std::cout << "Printing IfMap of Layer" << layerNo+1 << std::endl;
		printIfMap(layerNo, IfMap, minPrint);
		std::cout << "Printing WtMap of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap, minPrint);
		#if defined(WTMAP_WIDEN)
		std::cout << "Printing WtMap reordered of Layer" << layerNo+1 << std::endl;
		printWtMap(layerNo, WtMap_reordered, minPrint);
		#endif
		std::cout << "Printing OfMap under test before max pool of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden_tmp, minPrint);
		std::cout << "Printing OfMap under test of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap, minPrint);
		std::cout << "Printing Golden OfMap of Layer" << layerNo+1 << std::endl;
		printOfMap(layerNo, OfMap_golden, minPrint);
		std::cout << "Printing Compared OfMap of Layer " << layerNo+1 << std::endl;
		printOfMap(layerNo, Compared_OfMap, minPrint);
	}


	// Verification Print
	if(check){
		std::cout << "*****  Convolutional Layer ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "Printing first 20 elements of last output layer" << std::endl;
		for(int i=0;i<20;i++){
			std::cout << std::setw(8) << OfMap[i];
		}
		std::cout << "\n\n*****  ConvLayer Test ";
		if(biasReLuTrue){
			std::cout << "with ";
		}
		else{
			std::cout << "without ";
		}
		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
	}
	return check;
}


// *****  Software Functions  *****

/*
void wndClc_software(int layerNo, int Tof_step_i, int Toy_step_i, int Tox_step_i,
		px_data_t InBuf[POY][WRD_INBUF][POX], wt_data_t WtBuf[WRD_WTBUF][POF],
		px_data_t rslt_stream[POF][POY][POX]){

	// Declare all other parameters
	int Noy_step, Niy; // Noy_step = Noy/Toy
	int Tof_step, Toy_step, Tox_step; // Tof_step=ceil(Tof/Pof), Toy_step=ceil(Toy/Poy), Tox_step=ceil(Tox/Pox)
	int Tiy, Tix;
	int row_1map, wrd_1row; // row_1map=ceil(Tiy/Poy), wrd_1row=ceil(Tix/Pox)

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

	px_data_t to_PE[POY][POX];
	wt_data_t wt_stream[POF];
	px_data_t inter_rslt_stream[POF][POY][POX];

	// Initialize results to zero
	for(int Pof_i=0;Pof_i<POF;Pof_i++){
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				inter_rslt_stream[Pof_i][Poy_i][Pox_i] = 0;
			}
		}
	}

	for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
		for(int Nky_i=0;Nky_i<NKY;Nky_i++){
			for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){

				// Read Pixel
				InBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i,  Tof_step_i, Nif_i, Nky_i, Nkx_i, InBuf, to_PE);
				// Read Weights
				WtBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i, Tof_step_i, Nif_i, Nky_i, Nkx_i, WtBuf, wt_stream);
				// Multiply and Accumulate
				for(int Pof_i=0;Pof_i<POF;Pof_i++){
					for(int Poy_i=0;Poy_i<POY;Poy_i++){
						for(int Pox_i=0;Pox_i<POX;Pox_i++){
							inter_rslt_stream[Pof_i][Poy_i][Pox_i] += to_PE[Poy_i][Pox_i]*wt_stream[Pof_i];
						}
					}
				}

			}
		}
	}

	// Output MAC results
	for(int Pof_i=0;Pof_i<POF;Pof_i++){
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				rslt_stream[Pof_i][Poy_i][Pox_i] = inter_rslt_stream[Pof_i][Poy_i][Pox_i];
			}
		}
	}

}
*/


/*
void tileClc_software(int layerNo, int Nof_step_i,
		px_data_t InBuf[POY][WRD_INBUF][POX], wt_data_t WtBuf[WRD_WTBUF][POF],
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		int biasReLuTrue){
	// Declare all other parameters
	int Noy_step, Niy; // Noy_step = Noy/Toy
	int Tof_step, Toy_step, Tox_step; // Tof_step=ceil(Tof/Pof), Toy_step=ceil(Toy/Poy), Tox_step=ceil(Tox/Pox)
	int Tiy, Tix;
	int row_1map, wrd_1row; // row_1map=ceil(Tiy/Poy),wrd_1row= ceil(Tix/Pox)

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

	px_data_t to_PE[POY][POX];
	wt_data_t wt_stream[POF];
	px_data_t rslt_stream[POF][POY][POX];

	for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
		for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
			for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
				// Initialize results to zero
				for(int Pof_i=0;Pof_i<POF;Pof_i++){
					for(int Poy_i=0;Poy_i<POY;Poy_i++){
						for(int Pox_i=0;Pox_i<POX;Pox_i++){
							rslt_stream[Pof_i][Poy_i][Pox_i] = 0;
						}
					}
				}

				for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
					for(int Nky_i=0;Nky_i<NKY;Nky_i++){
						for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){

							// Read Pixel
							InBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i,  Tof_step_i, Nif_i, Nky_i, Nkx_i, InBuf, to_PE);
							// Read Weights
							WtBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i, Tof_step_i, Nif_i, Nky_i, Nkx_i, WtBuf, wt_stream);
							// Multiply and Accumulate
							for(int Pof_i=0;Pof_i<POF;Pof_i++){
								for(int Poy_i=0;Poy_i<POY;Poy_i++){
									for(int Pox_i=0;Pox_i<POX;Pox_i++){
										rslt_stream[Pof_i][Poy_i][Pox_i] += to_PE[Poy_i][Pox_i]*wt_stream[Pof_i];
									}
								}
							}

						}
					}
				}

				Pe2Buf_software(layerNo, Nof_step_i, Toy_step_i, Tox_step_i, Tof_step_i,
						OutBuf, rslt_stream, biasReLuTrue);
			}
		}
	}

}
*/


void convLayer_software(int layerNo,
		px_data_t *IfMap, wt_data_t *WtMap,
		px_data_t *OfMap,
		int biasReLuTrue
	){
	// Declare all other parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);
    static px_data_t IfMap_wpad[FMAP_MEMSIZE*2] = {0};
    // Add zero padding to Input Feature Maps
    for(int ni=0;ni<Nif_rom[layerNo];ni++){ // Loop-4
        for(int y=0;y<Noy_rom[layerNo]+2;y++){ // Loop-3.1
            for(int x=0;x<Tox_rom[layerNo]+2;x++){ // Loop-3.2
            	if(x<ZERO_PAD || x>=Tox_rom[layerNo]+1 || y<ZERO_PAD || y>=Noy_rom[layerNo]+1){
            		*(IfMap_wpad
            		+ ni*(Noy_rom[layerNo]+2)*(Tox_rom[layerNo]+2)
					+ y*(Tox_rom[layerNo]+2)
					+ x)
						= 0;
            	}
            	else{
            		*(IfMap_wpad
            		+ ni*(Noy_rom[layerNo]+2)*(Tox_rom[layerNo]+2)
					+ y*(Tox_rom[layerNo]+2)
					+ x) = *(IfMap
		            		+ ni*Noy_rom[layerNo]*Tox_rom[layerNo]
							+ (y-ZERO_PAD)*Tox_rom[layerNo]
							+ x-ZERO_PAD);
            	}
            }
        }
    }

	int tmpIfMap, tmp_WtMap, tmp_rslt;
	int addressBase; addressBase = 0;
	for(int i=0;i<layerNo;i++){
		addressBase += Nof_step_rom[i]*Tof_rom[i];
	}
	for(int no=0;no<Nof_step_rom[layerNo]*Tof_rom[layerNo];no++){ // Loop-4
		#ifdef DEBUG_PRINTS
			if(bias[addressBase + no]!=0){
				std::cout << "Found non-zero values in bias of TB! -> " << bias[addressBase + no] << std::endl;
			}
			std:: cout << "\nBelow is the TB bias array" << std::endl;
			for(int counter=0;counter<256;counter++){
				if(counter%16==0){
					std::cout << std::endl;
				}
				std:: cout << std::setw(5) << bias[addressBase + counter];
			}
		#endif
		for(int y=0;y<Noy_rom[layerNo];y++){ // Loop-3.1
			for(int x=0;x<Tox_rom[layerNo];x++){ // Loop-3.2
				tmp_rslt = 0;
				for(int ni=0;ni<Nif_rom[layerNo];ni++){ // Loop-2
					for(int ky=0;ky<NKY;ky++){ // Loop-1.1
						for(int kx=0;kx<NKX;kx++){ // Loop-1.2
							// WtMap -> [NOF][NIF][NKY][NKX]
							tmpIfMap = *(IfMap_wpad
										+ ni*(Noy_rom[layerNo]+2)*(Tox_rom[layerNo]+2)
										+ (S*y+ky)*(Tox_rom[layerNo]+2)
										+ S*x+kx);
							tmp_WtMap = *(WtMap
										+ no*Nif_rom[layerNo]*NKY*NKX
										+ ni*NKY*NKX
										+ ky*NKX
										+ kx);
							tmp_rslt += tmpIfMap*tmp_WtMap;
						}
					}
				}
				// tmp_rslt = tmp_rslt/(1 << bit_shift_rom[layerNo]); (yielded wrong results, value difference of 1)
				tmp_rslt = tmp_rslt >> bit_shift_rom[layerNo];
				tmp_rslt += bias[addressBase + no];
			    if(biasReLuTrue){
					*(OfMap + no*Noy_rom[layerNo]*Tox_rom[layerNo] + y*Tox_rom[layerNo] + x) = (tmp_rslt>0? tmp_rslt : 0);
			    }
			    else{
			    	*(OfMap + no*Noy_rom[layerNo]*Tox_rom[layerNo] + y*Tox_rom[layerNo] + x) = tmp_rslt;
			    }

			}
		}
	}

}

// *****  Auxiliary Functions  *****

px_data_t* initIfMap(int layerNo, int binInput){
	px_data_t* imgMem = nullptr;
	if(binInput==1){
		if(layerNo==0){
			imgMem = imgLoadFromBin_wrapper();
		}
		else{
			imgMem = new px_data_t[FMAP_MEMSIZE];
			for(int i=0;i<FMAP_MEMSIZE;i++){
				imgMem[i] = rand()%50;
			}
		}
	}
	else{
		imgMem = new px_data_t[FMAP_MEMSIZE];
		for(int i=0;i<FMAP_MEMSIZE;i++){
			imgMem[i] = rand()%128;
		}
	}
	return imgMem;
}


wt_data_t* initWtMap(int layerNo, int binInput){
	wt_data_t* WtMap = nullptr;
	if(binInput==1){
		WtMap = wtLoadFromBin_wrapper(layerNo);
	}
	else{
		WtMap = new wt_data_t[WTMAP_MEMSIZE];
		for(int i=0;i<WTMAP_MEMSIZE;i++){
			WtMap[i] = (rand()%(HW_EMUL_SYMM_RANGE*2+1)-HW_EMUL_SYMM_RANGE);
		}
	}
	return WtMap;
}