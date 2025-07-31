#include "header.h"
#include <cstdlib> // for rand()
#include <iostream>
#include <iomanip>

// ****** Testbench of Modules for data movement from/to Buffers to/from Pe  *****

// int loadInBuf2Pe_test(int verbose, int printFlagInfo, int printErrorOnly, int printLayer,
// 		int printToyStep, int printToxStep, int printTofStep,
// 		int printNif, int printNky, int printNkx){
// 	// I check for the whole tile, if the Poy*Pox pixels that are forwarded to PE are correct
// 	// verbose        -> print all results (complete data writes) for the model
// 	// printFlagInfo  -> print flags along with states(counters)
// 	// printErrorOnly -> print the results only for when an error is found
// 	// printLayer, printToyStep, printToxStep, printTofStep, printNif, printNky,
// 	// printNkx       -> print results for this particular iteration

// 	// Declare all variable parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

// 	static px_data_t InBuf[POY][WRD_INBUF][POX];
// 	px_data_t to_PE[POY][POX], to_PE_golden[POY][POX];
// 	px_data_t Comparedto_PE[POY][POX];

// 	ap_uint<ROW_OUT_I_B> row_out; ap_uint<POX_I_B> col; ap_uint<POF_I_B> bank; 	// InBuf2Pe_ctrl output variables (1)
// 	data_bool dct_ld; data_bool fl_ld; data_bool wr_fifo; data_bool out;   		// InBuf2Pe_ctrl output variables (2)

// 	int check = 0;

// 	// Initialize InBuf
// 	for(int Wrd_InBuf_i=0;Wrd_InBuf_i<WRD_INBUF;Wrd_InBuf_i++){
// 		for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 			for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 				InBuf[Poy_i][Wrd_InBuf_i][Pox_i] =
// 						Wrd_InBuf_i*1000 + Poy_i*100 + Pox_i;
// 			}
// 		}
// 	}

// 	for(int layerNo=0;layerNo<LAYERS;layerNo++){
// 		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 		if(printFlagInfo){
// 			std::cout << "\n*************************** For Layer " << layerNo+1 << " ***************************\n\n";
// 		}


// 		// Print InBuf
// 		if(verbose || (printLayer*printTofStep*printToyStep*printToxStep*printNif*printNky*printNkx && printLayer == layerNo+1)){
// 			printInBuf(InBuf);
// 		}


// 		// Check to see if loop order is correct (in InBuf2Pe_ctrl)
// 		for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
// 			for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
// 				for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
// 					row_out = Toy_step_i*wrd_1row + Tox_step_i;
// 					for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
// 						for(int Nky_i=0;Nky_i<NKY;Nky_i++){
// 							for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
// 								InBuf2Pe_ctrl(/*Inputs*/ Nif_rom[layerNo], wrd_1row_rom[layerNo], row_1map_rom[layerNo],
// 											  /*Outputs*/ &row_out, &col, &bank, &dct_ld, &fl_ld, &wr_fifo);
// 								InBuf2Pe_wrapper(/*Address Inputs*/ row_out, col, bank,
// 												 /*Control Flow Inputs*/ dct_ld, fl_ld, wr_fifo,
// 												 InBuf, to_PE);
// 								if(printFlagInfo){
// 									std::cout << "\nToy_step = " << Toy_step_i << " Tox_step = " << Tox_step_i << " Tof_step = " << Tof_step_i << "\n";
// 									std::cout << "Nif = " << Nif_i << " Nky = " << Nky_i << " Nkx = " << Nkx_i << "\n";
// 									std::cout << "dct_ld = " << dct_ld << " fl_ld = " << fl_ld << " wr_fifo = " << wr_fifo << " out = " << out << "\n\n\n";
// 									std::cout << "row_out = " << row_out << " col = " << col << " bank = " << bank << "\n\n\n";
// 								}

// 								InBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i, Tof_step_i, Nif_i, Nky_i, Nkx_i,
// 										InBuf, to_PE_golden);

// 								// Compare pixel values
// 								for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 									for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 										if(to_PE[Poy_i][Pox_i] != to_PE_golden[Poy_i][Pox_i]){
// 											check = 1;
// 										}
// 										Comparedto_PE[Poy_i][Pox_i] = to_PE[Poy_i][Pox_i] -
// 												to_PE_golden[Poy_i][Pox_i];
// 									}
// 								}

// 								// For debugging only
// 								int printError = 0;
// 								for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 									for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 										if(Comparedto_PE[Poy_i][Pox_i]!=0){
// 											printError = 1;
// 										}
// 									}
// 								}

// 								// Print information for debugging
// 								if(verbose || printError*printErrorOnly || (layerNo == printLayer-1 && Tof_step_i == printTofStep-1 && Toy_step_i == printToyStep-1
// 										&& Tox_step_i == printToxStep-1 && Nif_i == printNif-1 && Nky_i == printNky-1
// 										&& Nkx_i == printNkx-1)){
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 											<<  " ,Tof_step -> " << Tof_step_i << " ,Nif -> " << Nif_i << " ,Nky -> " << Nky_i
// 											<< " ,Nkx -> " << Nkx_i << "\n\n";
// 									std::cout << "Printing Pixel Stream under test" << std::endl;
// 									printPxStream(to_PE);
// 									std::cout << "Printing Golden Pixel Stream" << std::endl;
// 									printPxStream(to_PE_golden);
// 									std::cout << "Printing Comparison of Pixel Stream" << std::endl;
// 									printPxStream(Comparedto_PE);
// 									if(printError){
// 										std::cout << "Error Here!!!" << std::endl; //
// 									}
// 								}

// 							}
// 						}
// 					}

// 				}
// 			}
// 		}

// 	}

// 	// Verification Print
// 	if(check){
// 		std::cout << "*****  Loading InBuf to Pe test failed!  ******\n" << std::endl;
// 	}
// 	else{
// 		std::cout << "*****  InBuf2Pe Test Passed!  ******\n" << std::endl;
// 	}
// 	return check;
// }


// int loadWtBuf2Pe_test(int verbose, int printErrorOnly, int printLayer,
// 		int printToyStep, int printToxStep, int printTofStep,
// 		int printNif, int printNky, int printNkx){
// 	// I check for the whole tile, if the Pof weights that are forwarded to PE are correct
// 	// verbose        -> print all results (complete data writes) for the model
// 	// printErrorOnly -> print the results only for when an error is found
// 	// printLayer, printToyStep, printToxStep, printTofStep, printNif, printNky,
// 	// printNkx       -> print results for this particular iteration

// 	// Declare all variable parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;
// 	int wtbuf2pe_loop_limit;

// 	static wt_data_t WtBuf[WRD_WTBUF][POF];
// 	wt_data_t wt_stream[POF], wt_stream_golden[POF];
// 	wt_data_t Compared_wt_stream[POF];

// 	ap_uint<WRD_WTBUF_I_B> row_wt; // WtBuf2Pe_ctrl output variables

// 	int check = 0;

// 	// Initialize WtBuf
// 	for(int Wrd_WtBuf_i=0;Wrd_WtBuf_i<WRD_WTBUF;Wrd_WtBuf_i++){
// 		for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 			WtBuf[Wrd_WtBuf_i][Pof_i] =
// 					Wrd_WtBuf_i*100 + Pof_i;
// 		}
// 	}

// 	for(int layerNo=0;layerNo<LAYERS;layerNo++){
// 		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 		// Print WtBuf
// 		if(verbose || (printLayer*printTofStep*printToyStep*printToxStep*printNif*printNky*printNkx && printLayer == layerNo+1)){
// 			printWtBuf(WtBuf);
// 		}

// 		// Check to see if loop order is correct (in InBuf2Pe_ctrl)
// 		for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
// 			for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
// 				for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){

// 					for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
// 						for(int Nky_i=0;Nky_i<NKY;Nky_i++){
// 							for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){

// 								wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_rom[layerNo];
// 								WtBuf2Pe_ctrl(wtbuf2pe_loop_limit, &row_wt);
// 								WtBuf2Pe(WtBuf, row_wt, wt_stream);
// 								WtBuf2Pe_software(layerNo, Toy_step_i, Tox_step_i, Tof_step_i,
// 										Nif_i, Nky_i, Nkx_i, WtBuf, wt_stream_golden);

// 								// Compare weight values
// 								for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 									if(wt_stream[Pof_i] != wt_stream_golden[Pof_i]){
// 										check = 1;
// 									}
// 									Compared_wt_stream[Pof_i] = wt_stream[Pof_i] -
// 											wt_stream_golden[Pof_i];
// 								}

// 								// For debugging only
// 								int printError = 0;
// 								for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 									if(Compared_wt_stream[Pof_i]!=0){
// 										printError = 1;
// 									}
// 								}

// 								// Print information for debugging
// 								if(verbose || printError*printErrorOnly || (layerNo == printLayer-1 && Tof_step_i == printTofStep-1 && Toy_step_i == printToyStep-1
// 										&& Tox_step_i == printToxStep-1 && Nif_i == printNif-1 && Nky_i == printNky-1
// 										&& Nkx_i == printNkx-1)){
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 											<<  " ,Tof_step -> " << Tof_step_i << " ,Nif -> " << Nif_i << " ,Nky -> " << Nky_i
// 											<< " ,Nkx -> " << Nkx_i << "\n\n";
// 									std::cout << "Printing Weight Stream under test" << std::endl;
// 									printWtStream(wt_stream);
// 									// std::cout << "\nFor Layer -> " << layerNo << " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 																				// <<  " ,Tof_step -> " << Tof_step_i << " ,Nif -> " << Nif_i << " ,Nky -> " << Nky_i
// 																				// << " ,Nkx -> " << Nkx_i << "\n\n";
// 									std::cout << "Printing Golden Weight Stream" << std::endl;
// 									printWtStream(wt_stream_golden);
// 									// std::cout << "\nFor Layer -> " << layerNo << " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 																				// <<  " ,Tof_step -> " << Tof_step_i << " ,Nif -> " << Nif_i << " ,Nky -> " << Nky_i
// 																				// << " ,Nkx -> " << Nkx_i << "\n\n";
// 									std::cout << "Printing Comparison of Weight Stream" << std::endl;
// 									printWtStream(Compared_wt_stream);
// 									if(printError){
// 										std::cout << "Error Here!!!" << std::endl; //
// 									}
// 								}

// 							}
// 						}
// 					}

// 				}
// 			}
// 		}

// 	}

// 	// Verification Print
// 	if(check){
// 		std::cout << "*****  Loading WtBuf to Pe test failed!  ******\n" << std::endl;
// 	}
// 	else{
// 		std::cout << "*****  WtBuf2Pe Test Passed!  ******\n" << std::endl;
// 	}
// 	return check;
// }


// int storePe2Buf_test(int verbose, int printErrorOnly, int printLayer, int printNofStep, int printNoyStep,
// 		int printToyStep, int printToxStep, int printTofStep, int biasReLuTrue){
// 	// I check for the whole tile, if the Pof*Poy*Pox PE results,
// 	// are forwarded to the correct addresses and banks of the Output Buffers
// 	// verbose        -> print all results (complete data writes) for the model
// 	// printErrorOnly -> print the results only for when an error is found
// 	// printLayer, printNofStep, printNoyStep, printToyStep, printToxStep,
// 	// printTofStep   -> print results for this particular iteration
// 	// biasReLuTrue = 1 -> I use bias + ReLu
// 	// biasReLuTrue = 0 -> I turn off bias + ReLu to verify all other functionality
// 	// biases are loaded to buffer with loadBiasTile function, it is used here in correct loop place

// 	// Declare all variable parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;


// 	px_data_t px_stream[POF][POY][POX]; b_data_t BiasBuf[BIASBUF_LENGTH];
// 	static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX], OutBuf_golden[OUTBUF_NUM][WRD_OUTBUF][POX];
// 	static px_data_t Compared_OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];

// 	int check = 0; int printcheck;

// 	for(int layerNo=0;layerNo<LAYERS;layerNo++){
// 		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 		if(nofFirst[layerNo]){
// 			for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){

// 				loadBiasTile(/*layerCnfg=*/0, BiasBuf);

// 				for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){

// 					initOutBuf_undef(OutBuf);
// 					initOutBuf_undef(OutBuf_golden);

// 					// Write one tile

// 					// This loop order should be the same everywhere
// 					for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
// 						for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
// 							for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
// 								// Set values in PE
// 								for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 									for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 										for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 											px_stream[Pof_i][Poy_i][Pox_i] = rand() % 10000;
// 										}
// 									}
// 								}

// 								Pe2Buf_software(layerNo, Nof_step_i, Toy_step_i, Tox_step_i, Tof_step_i,
// 										OutBuf_golden, px_stream, biasReLuTrue);

// 								Pe2Buf(Toy_rom[layerNo], toy_step_rom[layerNo], tox_step_rom[layerNo], tof_step_rom[layerNo],
// 										pe2buf_addr_offset1_rom[layerNo], pe2buf_addr_offset2_rom[layerNo], pe2buf_addr_offset3_rom[layerNo],
// 										bit_shift_rom[layerNo],
// 										px_stream, BiasBuf, OutBuf);

// 								// Set Compared_OutBuf to zero (so that elements of no importance don't show up as errors)
// 								for(int Wrd_OutBuf_i=0;Wrd_OutBuf_i<WRD_OUTBUF;Wrd_OutBuf_i++){
// 									for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
// 										for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 											Compared_OutBuf[OutBuf_Num_i][Wrd_OutBuf_i][Pox_i] = 0;
// 										}
// 									}
// 								}

// 								printcheck = 0;
// 								// Compare Output Buffers
// 								for(int Tof_i=0;Tof_i<Tof_rom[layerNo];Tof_i++){
// 									for(int Toy_i=0;Toy_i<Toy_rom[layerNo];Toy_i++){
// 										for(int Tox_i=0;Tox_i<Tox_rom[layerNo];Tox_i++){

// 											if(OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] !=
// 													OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX]){
// 												check = 1; printcheck = 1;
// 											}
// 											Compared_OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] =
// 													OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] -
// 													OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX];
// 										}
// 									}
// 								}

// 								// Print PE results and Output Buffers
// 								if(verbose || printcheck*printErrorOnly || (printLayer*printTofStep*printToyStep*printToxStep*printNofStep*printNoyStep
// 									&& printLayer == layerNo+1)){
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i
// 											<< " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 											<<  " ,Tof_step -> " << Tof_step_i
// 											<< "\n\n";
// 									if(printcheck){
// 										std::cout << "Error here!!!" << std::endl;
// 									}
// 									std::cout << "Printing PE results" << std::endl;
// 									printPeResults(px_stream);
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i
// 											<< " ,Noy_step -> " << Noy_step_i << "\n\n";
// 									std::cout << "Printing Output Buffers under test" << std::endl;
// 									printOutBuf(layerNo, OutBuf);
// 									std::cout << "Printing Golden Output Buffers" << std::endl;
// 									printOutBuf(layerNo, OutBuf_golden);
// 									std::cout << "Printing Comparison of Output Buffers" << std::endl;
// 									printOutBuf(layerNo, Compared_OutBuf);
// 								}

// 							}
// 						}
// 					}

// 					// // Set Compared_OutBuf to zero (so that elements of no importance don't show up as errors)
// 					// for(int Wrd_OutBuf_i=0;Wrd_OutBuf_i<WRD_OUTBUF;Wrd_OutBuf_i++){
// 					// 	for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
// 					// 		for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 					// 			Compared_OutBuf[OutBuf_Num_i][Wrd_OutBuf_i][Pox_i] = 0;
// 					// 		}
// 					// 	}
// 					// }

// 					// // Compare Output Buffers
// 					// for(int Tof_i=0;Tof_i<Tof[layerNo];Tof_i++){
// 					// 	for(int Toy_i=0;Toy_i<Toy[layerNo];Toy_i++){
// 					// 		for(int Tox_i=0;Tox_i<Tox[layerNo];Tox_i++){

// 					// 			if(OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] !=
// 					// 					OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX]){
// 					// 				check = 1;
// 					// 			}
// 					// 			Compared_OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] =
// 					// 					OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] -
// 					// 					OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX];
// 					// 		}
// 					// 	}
// 					// }

// 					// // Print Output Buffers
// 					// if(verbose || printcheck*printErrorOnly || (printLayer*printTofStep*printToyStep*printToxStep*printNofStep*printNoyStep
// 					// 		&& printLayer == layerNo+1)){
// 					// 	std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i
// 					// 			<< " ,Noy_step -> " << Noy_step_i << "\n\n";
// 					// 	std::cout << "Printing Output Buffers under test" << std::endl;
// 					// 	printOutBuf(layerNo, OutBuf);
// 					// 	std::cout << "Printing Golden Output Buffers" << std::endl;
// 					// 	printOutBuf(layerNo, OutBuf_golden);
// 					// 	std::cout << "Printing Comparison of Output Buffers" << std::endl;
// 					// 	printOutBuf(layerNo, Compared_OutBuf);
// 					// }

// 				}
// 			}
// 		}
// 		else{
// 			for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
// 				for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){

// 					// Write the correct biases for the tile to the buffer
// 					loadBiasTile(/*layerCnfg=*/0, BiasBuf);

// 					initOutBuf_undef(OutBuf);
// 					initOutBuf_undef(OutBuf_golden);

// 					// Write one tile

// 					// This loop order should be the same everywhere
// 					for(int Toy_step_i=0;Toy_step_i<Toy_step;Toy_step_i++){
// 						for(int Tox_step_i=0;Tox_step_i<Tox_step;Tox_step_i++){
// 							for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
// 								// Set values in PE
// 								for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 									for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 										for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 											px_stream[Pof_i][Poy_i][Pox_i] = rand() % 10000;
// 										}
// 									}
// 								}

// 								Pe2Buf_software(layerNo, Nof_step_i, Toy_step_i, Tox_step_i, Tof_step_i,
// 										OutBuf_golden, px_stream, biasReLuTrue);

// 								Pe2Buf(Toy_rom[layerNo], toy_step_rom[layerNo], tox_step_rom[layerNo], tof_step_rom[layerNo],
// 										pe2buf_addr_offset1_rom[layerNo], pe2buf_addr_offset2_rom[layerNo], pe2buf_addr_offset3_rom[layerNo],
// 										bit_shift_rom[layerNo],
// 										px_stream, BiasBuf, OutBuf);

// 								// Set Compared_OutBuf to zero (so that elements of no importance don't show up as errors)
// 								for(int Wrd_OutBuf_i=0;Wrd_OutBuf_i<WRD_OUTBUF;Wrd_OutBuf_i++){
// 									for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
// 										for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 											Compared_OutBuf[OutBuf_Num_i][Wrd_OutBuf_i][Pox_i] = 0;
// 										}
// 									}
// 								}

// 								printcheck = 0;
// 								// Compare Output Buffers
// 								for(int Tof_i=0;Tof_i<Tof_rom[layerNo];Tof_i++){
// 									for(int Toy_i=0;Toy_i<Toy_rom[layerNo];Toy_i++){
// 										for(int Tox_i=0;Tox_i<Tox_rom[layerNo];Tox_i++){

// 											if(OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] !=
// 													OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX]){
// 												check = 1; printcheck = 1;
// 											}
// 											Compared_OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] =
// 													OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] -
// 													OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX];
// 										}
// 									}
// 								}

// 								// Print PE results and Output Buffers
// 								if(verbose || printcheck*printErrorOnly || (printLayer*printTofStep*printToyStep*printToxStep*printNofStep*printNoyStep
// 									&& printLayer == layerNo+1)){
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i
// 											<< " ,Toy_step -> " << Toy_step_i << " ,Tox_step -> " << Tox_step_i
// 											<<  " ,Tof_step -> " << Tof_step_i
// 											<< "\n\n";
// 									if(printcheck){
// 										std::cout << "Error here!!!" << std::endl;
// 									}
// 									std::cout << "Printing PE results" << std::endl;
// 									printPeResults(px_stream);
// 									std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i
// 											<< " ,Noy_step -> " << Noy_step_i << "\n\n";
// 									std::cout << "Printing Output Buffers under test" << std::endl;
// 									printOutBuf(layerNo, OutBuf);
// 									std::cout << "Printing Golden Output Buffers" << std::endl;
// 									printOutBuf(layerNo, OutBuf_golden);
// 									std::cout << "Printing Comparison of Output Buffers" << std::endl;
// 									printOutBuf(layerNo, Compared_OutBuf);
// 								}
// 							}
// 						}
// 					}

// 					// // Set Compared_OutBuf to zero (so that elements of no importance don't show up as errors)
// 					// for(int Wrd_OutBuf_i=0;Wrd_OutBuf_i<WRD_OUTBUF;Wrd_OutBuf_i++){
// 					// 	for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
// 					// 		for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 					// 			Compared_OutBuf[OutBuf_Num_i][Wrd_OutBuf_i][Pox_i] = 0;
// 					// 		}
// 					// 	}
// 					// }

// 					// // Compare Output Buffers
// 					// for(int Tof_i=0;Tof_i<Tof[layerNo];Tof_i++){
// 					// 	for(int Toy_i=0;Toy_i<Toy[layerNo];Toy_i++){
// 					// 		for(int Tox_i=0;Tox_i<Tox[layerNo];Tox_i++){

// 					// 			if(OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] !=
// 					// 					OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX]){
// 					// 				check = 1;
// 					// 			}
// 					// 			Compared_OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] =
// 					// 					OutBuf[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX] -
// 					// 					OutBuf_golden[Tof_i%OUTBUF_NUM][(Tof_i/OUTBUF_NUM)*Toy[layerNo]*Tox_step+Toy_i*Tox_step+(Tox_i/POX)][Tox_i%POX];
// 					// 		}
// 					// 	}
// 					// }

// 					// // Print Output Buffers
// 					// if(verbose || printcheck*printErrorOnly || (printLayer*printTofStep*printToyStep*printToxStep*printNofStep*printNoyStep
// 					// 		&& printLayer == layerNo+1)){
// 					// 	std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i
// 					// 			<< " ,Noy_step -> " << Noy_step_i << "\n\n";
// 					// 	std::cout << "Printing Output Buffers under test" << std::endl;
// 					// 	printOutBuf(layerNo, OutBuf);
// 					// 	std::cout << "Printing Golden Output Buffers" << std::endl;
// 					// 	printOutBuf(layerNo, OutBuf_golden);
// 					// 	std::cout << "Printing Comparison of Output Buffers" << std::endl;
// 					// 	printOutBuf(layerNo, Compared_OutBuf);
// 					// }

// 				}
// 			}
// 		}

// 	}

// 	// Verification Print
// 	if(check){
// 		std::cout << "*****  Loading PE Results to OutBuf ";
// 		if(biasReLuTrue){
// 			std::cout << "with ";
// 		}
// 		else{
// 			std::cout << "without ";
// 		}
// 		std::cout << " bias + ReLu test failed!  ******\n" << std::endl;
// 	}
// 	else{
// 		std::cout << "*****  Pe2Buf Test ";
// 		if(biasReLuTrue){
// 			std::cout << "with ";
// 		}
// 		else{
// 			std::cout << "without ";
// 		}
// 		std::cout << "bias + ReLu Passed!  ******\n" << std::endl;
// 	}
// 	return check;
// }


// ***** Printing Functions  *****

void printPxStream(px_data_t PxStream[POY][POX]){
	for(int Poy_i=0;Poy_i<POY;Poy_i++){
		for(int Pox_i=0;Pox_i<POX;Pox_i++){
			std::cout << std::setw(20) << PxStream[Poy_i][Pox_i] << "     ";
		}
		std::cout << " " << std::endl;
	}
}


void printWtStream(wt_data_t WtStream[POF]){
	for(int Pof_i=0;Pof_i<POF;Pof_i++){
		std::cout << std::setw(20) << WtStream[Pof_i] << "     ";
	}
	std::cout << " " << std::endl;
}


void printPeResults(px_data_t px_stream[POF][POY][POX]){
	for(int Pof_i=0;Pof_i<POF;Pof_i++){
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				std::cout << std::setw(10) << px_stream[Pof_i][Poy_i][Pox_i] << "    ";
			}
			std::cout << "|      ";
		}
		std::cout << " " << std::endl;
	}
}


// *****  Software Functions  *****

// void InBuf2Pe_software(int layerNo, int Toy_step_i, int Tox_step_i, int Tof_step_i,
// 		int Nif_i, int Nky_i, int Nkx_i,
// 		px_data_t InBuf[POY][WRD_INBUF][POX], px_data_t to_PE[POY][POX]){

// 	// Declare all other parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

// 	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 	for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 		for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 			to_PE[Poy_i][Pox_i] = InBuf[(Nky_i+Poy_i)%POY]
// 										[ Nif_i*row_1map*wrd_1row
// 										 + (((Nky_i+Poy_i)/POY) + Toy_step_i) * wrd_1row
// 										 + (Nkx_i+Pox_i)/POX + Tox_step_i]
// 										 [(Nkx_i+Pox_i)%POX];
// 		}
// 	}
// 	// Zero if POX is not fully used
// 	if( (Tox_step_i == Tox_step-1) && ((Tox_rom[layerNo]%POX) != 0) ){
// 		for(int Pox_i = (Tox_rom[layerNo]%POX); Pox_i<POX;Pox_i++){
// 			for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 				to_PE[Poy_i][Pox_i] = 0;
// 			}
// 		}
// 	}
// 	// Zero if POY is not fully used
// 	if( (Toy_step_i == Toy_step-1) && ((Toy_rom[layerNo]%POY) != 0) ){
// 		for(int Poy_i = (Toy_rom[layerNo]%POY); Poy_i<POY;Poy_i++){
// 			for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 				to_PE[Poy_i][Pox_i] = 0;
// 			}
// 		}
// 	}

// }


// void WtBuf2Pe_software(int layerNo, int Toy_step_i, int Tox_step_i, int Tof_step_i,
// 		int Nif_i, int Nky_i, int Nkx_i,
// 		wt_data_t WtBuf[WRD_WTBUF][POF], wt_data_t wt_stream[POF]){

// 	// Declare all other parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

// 	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 	for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 		wt_stream[Pof_i] = WtBuf[ Tof_step_i*Nif_rom[layerNo]*NKY*NKX
// 								 + Nif_i*NKY*NKX
// 								 + Nky_i*NKX
// 								 + Nkx_i][Pof_i];
// 	}

// }


// void Pe2Buf_software(int layerNo, int Nof_step_i, int Toy_step_i, int Tox_step_i, int Tof_step_i,
// 		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX], px_data_t px_stream[POF][POY][POX],
// 		int biasReLuTrue){

// 	// Declare all other parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

// 	px_data_t  applyBiasReLu;

// 	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 	for(int Pof_i=0;Pof_i<POF;Pof_i++){
// 		for(int Poy_i=0;Poy_i<POY;Poy_i++){
// 			for(int Pox_i=0;Pox_i<POX;Pox_i++){
// 				if(biasReLuTrue){
// 					BiasReLu_software(layerNo, Nof_step_i, Tof_step_i, Pof_i,
// 							px_stream[Pof_i][Poy_i][Pox_i], &applyBiasReLu);
// 					if(applyBiasReLu<0){
// 						std::cout << "Found Error Here!\n";
// 					}
// 					if( Toy_step_i*POY+Poy_i < Toy_rom[layerNo]){ // This if is to make sure we don't write garbage values if Toy%Poy!=0
// 						OutBuf[(Tof_step_i*POF+Pof_i)%OUTBUF_NUM]
// 							   [((Tof_step_i*POF+Pof_i)/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step
// 								+ (Toy_step_i*POY+Poy_i)*Tox_step
// 								+ Tox_step_i]
// 								[Pox_i] =
// 										applyBiasReLu;
// 					}
// 				}
// 				else{
// 					if( Toy_step_i*POY+Poy_i < Toy_rom[layerNo]){ // This if is to make sure we don't write garbage values if Toy%Poy!=0
// 						OutBuf[(Tof_step_i*POF+Pof_i)%OUTBUF_NUM]
// 							   [((Tof_step_i*POF+Pof_i)/OUTBUF_NUM)*Toy_rom[layerNo]*Tox_step
// 								+ (Toy_step_i*POY+Poy_i)*Tox_step
// 								+ Tox_step_i]
// 								[Pox_i] =
// 										px_stream[Pof_i][Poy_i][Pox_i];
// 					}
// 				}
// 			}
// 		}
// 	}

// }


// void BiasReLu_software(int layerNo, int Nof_step_i, int Tof_step_i, int Pof_i,
// 		px_data_t in, px_data_t *out){
// 	int addressBase; // where the bias for each layer starts
// 	px_data_t biased_in;

// 	// Declare all other parameters
// 	int Noy_step /*Noy/Toy*/, Niy;
// 	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
// 	int Tiy, Tix;
// 	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

// 	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
// 		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

// 	// I want to load POF pixels, even if T%P!=0 and not load from next layers biases
// 	// So I allocate this length for each layer, broken into tiles.
// 	addressBase = 0;
// 	for(int i=0;i<layerNo;i++){
// 		addressBase += Nof_step_rom[i] * Tof_rom[i];
// 	}

// 	if(addressBase + Tof_step_i*POF + Pof_i >= BIASMEM_LENGTH){
// 		std::cout << "Error Here!!!" << std::endl;
// 	}

// 	biased_in = in + bias[addressBase + Nof_step_i*Tof_rom[layerNo] + Tof_step_i*POF + Pof_i];

// 	if(biased_in > 0){
// 		*out = biased_in;
// 	}
// 	else{
// 		*out = 0;
// 	}
// 	// *out = (biased_in > ap_int<8>(0)) ? biased_in : 0;
// }


// *****  Auxiliary Functions  *****

void initOutBuf_undef(px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]){
	for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
		for(int WrdOutBuf_i=0; WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				OutBuf[OutBuf_Num_i][WrdOutBuf_i][Pox_i] = -1;
			}
		}
	}
}
