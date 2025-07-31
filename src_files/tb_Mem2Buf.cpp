#include "header.h"
#include <iostream>
#include <iomanip>

// ****** Testbench of Modules for Reading/Writing External Memory  *****

int loadIfMap_test(int verbose, int printLayer, int printNofStep, int printNoyStep){
// verbose=1 -> print every loadIfMap results for the impl.
// printLayer, printNofStep, printNoyStep -> print call for Layer=printLayer,...
// NofFirst -> loop order in each conv. layer. NofFirst = 1, load weights first and fully buffer them (Nof=Tof)
// Change yBase calculation if we change order to NofFirst = 1

	// Declare all variable parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	static px_data_t IfMap[IFMAP_MEMSIZE];
	static px_data_t InBuf[POY][WRD_INBUF][POX], InBuf_golden[POY][WRD_INBUF][POX];
	static px_data_t Compared_InBuf[POY][WRD_INBUF][POX] = {0};
	Niy_dt yBase;
	data_bool northTile, southTile;

	int check = 0;

	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

		// Initialize IfMap
		for(int Nif_i=0; Nif_i<Nif_rom[layerNo];Nif_i++){
			for(int Niy_i=0; Niy_i<Niy;Niy_i++){
				for(int Nix_i=0;Nix_i<(Tix-2);Nix_i++){
					*(IfMap
					+ Nif_i*Niy*(Tix-2)
					+ Niy_i*(Tix-2)
					+ Nix_i) = (Nif_i+1)*10000
							+ (Niy_i+1)*100 + (Nix_i+1);
				}
			}
		}

		// Print IfMap
		if(verbose || (printLayer*printNofStep*printNoyStep && printLayer == layerNo+1)){
			printIfMap(layerNo, IfMap, 0);
		}

		// Call loadIfMap to load parameters
		loadIfMap(/*layerCnfg=*/1, northTile, southTile,
					yBase, IfMap, InBuf);
		yBase = 0;
		if(nofFirst[layerNo]){
			for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
				// Nof_step = 1
				for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
					initInBuf_undef(InBuf_golden);
					loadIfMap_software(layerNo, Noy_step_i,
							IfMap, InBuf_golden);

					// Code below needs to be copy-paste from ConvLayer module
					if(Noy_step_i == 0){
						northTile = 1;
					}
					else{
						northTile = 0;
					}
					if(Noy_step_i == Noy_step - 1){
						southTile = 1;
					}
					else{
						southTile = 0;
					}

					// Could be replaced with yBase = 0
					if(Noy_step_i == 0){  // copy-paste from ConvLayer module
						yBase = 0;
					}
					else{
						yBase = Noy_step_i*(Tiy - 2) -1;
					}

					initInBuf_undef(InBuf);
					loadIfMap(/*layerCnfg=*/0, northTile, southTile,
										yBase, IfMap, InBuf);

					// Compare values of InBuf
					for(int index=0; index<WRD_INBUF;index++){
						for(int Poy_i=0; Poy_i<POY;Poy_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								if(InBuf[Poy_i][index][Pox_i] != InBuf_golden[Poy_i][index][Pox_i]){
									check = 1;
								}
								Compared_InBuf[Poy_i][index][Pox_i] = InBuf[Poy_i][index][Pox_i] - InBuf_golden[Poy_i][index][Pox_i];
							}
						}
					}

					// Print information for debugging
					if(verbose || (layerNo == printLayer-1 && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Input Buffers under test" << std::endl;
						printInBuf(InBuf);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Input Buffers" << std::endl;
						printInBuf(InBuf_golden);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Input Buffers" << std::endl;
						printInBuf(Compared_InBuf);
					}
				}
			}
		}
		else{
			for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
				// Noy_step = 1
				for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
					initInBuf_undef(InBuf_golden);
					loadIfMap_software(layerNo, Noy_step_i,
												IfMap, InBuf_golden);

					// Code below needs to be copy-paste from ConvLayer module
					if(Noy_step_i == 0){
						northTile = 1;
					}
					else{
						northTile = 0;
					}
					if(Noy_step_i == Noy_step - 1){
						southTile = 1;
					}
					else{
						southTile = 0;
					}


					if(Noy_step_i == 0){  // copy-paste from ConvLayer module
						yBase = 0;
					}
					else{
						yBase = Noy_step_i*(Tiy - 2) -1;
					}

					initInBuf_undef(InBuf);
					loadIfMap(/*layerCnfg=*/0, northTile, southTile,
										yBase, IfMap, InBuf);

					// Compare values of InBuf
					for(int index=0; index<WRD_INBUF;index++){
						for(int Poy_i=0; Poy_i<POY;Poy_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								if(InBuf[Poy_i][index][Pox_i] != InBuf_golden[Poy_i][index][Pox_i]){
									check = 1;
								}
								Compared_InBuf[Poy_i][index][Pox_i] = InBuf[Poy_i][index][Pox_i] - InBuf_golden[Poy_i][index][Pox_i];
							}
						}
					}

					// Print information for debugging
					if(verbose || (printLayer == layerNo && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Input Buffers under test" << std::endl;
						printInBuf(InBuf);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Input Buffers" << std::endl;
						printInBuf(InBuf_golden);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Input Buffers" << std::endl;
						printInBuf(Compared_InBuf);
					}
				}
			}
		}
	}

	// Verification Print
	if(check){
		std::cout << "*****  Loading IfMap to InBuf test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "*****  loadIfMap Test Passed!  ******\n" << std::endl;
	}
	return check;
}

int loadWtMap_test(int verbose, int printLayer, int printNofStep, int printNoyStep, int printProgress){
// verbose=1 -> print every loadIfMap results for the impl.
// printLayer, printNofStep, printNoyStep -> print call for Layer=printLayer,...
// NofFirst -> loop order in each conv. layer
// May need to change yBase calculation if we change order to NofFirst = 1
// -1 are elements that are not written to in the specific iteration

	// Declare all variable parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	// Declare Variables
	static wt_data_t WtMap[WTMAP_MEMSIZE]; // [NOF][NIF][NKY][NKX]
	static wt_data_t WtBuf[WRD_WTBUF][POF], WtBuf_golden[WRD_WTBUF][POF];
	static wt_data_t Compared_WtBuf[WRD_WTBUF][POF] = {0};
	int ofBase; // = Nof_step_i*Tof[layerNo];

	int check = 0;

	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
								&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);
		if(printProgress){
			std::cout << "Finished layer " << layerNo << std::endl;
		}
		// Initialize WtMap
		for(int Nif_i=0; Nif_i<Nif_rom[layerNo];Nif_i++){
			for(int Nof_i=0; Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]);Nof_i++){
				for(int Nky_i=0;Nky_i<NKY;Nky_i++){
					for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
						*(WtMap
						+ Nof_i*Nif_rom[layerNo]*NKX*NKY
						+ Nif_i*NKX*NKY
						+ Nky_i*NKY
						+ Nkx_i) =
								(Nif_i+1)*1000
								+ (Nof_i+1)*100 + (Nky_i+1)*7 + (Nkx_i+1);
					}
				}
			}
		}

		// Print Weight Maps
		if(verbose || printLayer*printNofStep*printNoyStep){
			printWtMap(layerNo, WtMap, 0);
		}

		// Call loadWtMap to load parameters
		loadWtMap(/*layerCnfg=*/1, ofBase, WtMap, WtBuf);
		ofBase = 0;

		if(nofFirst[layerNo]){
			for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
				for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
					initWtBuf_undef(WtBuf_golden);
					loadWtMap_software(layerNo, Nof_step_i, WtMap, WtBuf_golden);

					// Code below needs to be copy-paste from ConvLayer module
					ofBase = Nof_step_i; // needs to change if we change order, and maybe change place

					initWtBuf_undef(WtBuf);
					loadWtMap(/*layerCnfg=*/0, ofBase, WtMap, WtBuf);

					// Compare values of WtBuf
					for(int index=0; index<WRD_WTBUF;index++){
						for(int Pof_i=0; Pof_i<POF;Pof_i++){
							if(WtBuf[index][Pof_i] != WtBuf_golden[index][Pof_i]){
								check = 1;
							}
							Compared_WtBuf[index][Pof_i] = WtBuf[index][Pof_i] - WtBuf_golden[index][Pof_i];
						}
					}

					// Print information for debugging
					if(verbose || (layerNo == printLayer-1 && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Weight Buffers under test" << std::endl;
						printWtBuf(WtBuf);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Weight Buffers" << std::endl;
						printWtBuf(WtBuf_golden);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Weight Buffers" << std::endl;
						printWtBuf(Compared_WtBuf);
					}
				}
			}
		}
		else{
			for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
				for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
					initWtBuf_undef(WtBuf_golden);
					loadWtMap_software(layerNo, Nof_step_i, WtMap, WtBuf_golden);

					ofBase = Nof_step_i;  // copy-paste from ConvLayer module

					initWtBuf_undef(WtBuf);
					loadWtMap(/*layerCnfg=*/0, ofBase, WtMap, WtBuf);

					// Compare values of WtBuf
					for(int index=0; index<WRD_WTBUF;index++){
						for(int Pof_i=0; Pof_i<POF;Pof_i++){
							if(WtBuf[index][Pof_i] != WtBuf_golden[index][Pof_i]){
								check = 1;
							}
							Compared_WtBuf[index][Pof_i] = WtBuf[index][Pof_i] - WtBuf_golden[index][Pof_i];
						}
					}

					// Print information for debugging
					if(verbose || (layerNo == printLayer-1 && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Weight Buffers under test" << std::endl;
						printWtBuf(WtBuf);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Weight Buffers" << std::endl;
						printWtBuf(WtBuf_golden);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Weight Buffers" << std::endl;
						printWtBuf(Compared_WtBuf);
					}
				}
			}
		}
	}

	// Verification Print
	if(check){
		std::cout << "*****  Loading WtMap to InBuf test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "*****  loadWtMap Test Passed!  ******\n" << std::endl;
	}
	return check;
}

int storeMaps_test(int verbose, int printLayer, int printNofStep, int printNoyStep){
// verbose=1 -> print every loadIfMap results for the impl.
// printLayer, printNofStep, printNoyStep -> print call for Layer=printLayer,...
// NofFirst -> loop order in each conv. layer
// May need to change yBase calculation if we change order to NofFirst = 1
// -1 are elements that are not written to in the specific iteration

	// Declare all variable parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	// Declare Variables
	static px_data_t OfMap[OFMAP_MEMSIZE], OfMap_golden[OFMAP_MEMSIZE];
	static px_data_t Compared_OfMap[OFMAP_MEMSIZE];
	static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
	int ofBase, yBase;

	int check = 0;

	if(verbose || printLayer*printNofStep*printNoyStep){
		std::cout << "\n***** Debugging storeMap  ***** " << "\n\n";
	}

	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		initOfMap_undef(layerNo, OfMap, -1); initOfMap_undef(layerNo, OfMap_golden, -1);

		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

		// Call storeMap to load parameters
		storeMap(/*layerCnfg=*/1, OutBuf, OfMap);

		if(nofFirst[layerNo]){
			for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){
				for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){

					// Initialize OutBuf [OUTBUF_NUM][WRD_OUTBUF][POX]
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] =
										Nof_step_i*100000 + Noy_step_i*10000
										+ OutBufNum_i*1000 + WrdOutBuf_i*100
										+ Pox_i*10;
							}
						}
					}

					if(verbose || printLayer*printNofStep*printNoyStep){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						printOutBuf(layerNo, OutBuf);
					}

					// Code below needs to be copy-paste from ConvLayer module
					ofBase = Nof_step_i; yBase = Noy_step_i;

					storeMap(/*layerCnfg=*/0, OutBuf, OfMap);

					storeMaps_software(layerNo, Nof_step_i, Noy_step_i,
							OfMap_golden, OutBuf);

					// Compare values of OfMap
					for(int Nof_i=0;Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]);Nof_i++){
						for(int Noy_i=0;Noy_i<Noy_rom[layerNo];Noy_i++){
							for(int Nox_i=0;Nox_i<Tox_rom[layerNo];Nox_i++){
								if(*( OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) !=
										*( OfMap_golden + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i )){
									check = 1;
								}
								*( Compared_OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) =
										*( OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) -
										*( OfMap_golden + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i );
							}
						}
					}

					// Print information for debugging
					if(verbose || (layerNo == printLayer-1 && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Output Feature Maps under test" << std::endl;
						printOfMap(layerNo, OfMap, 0);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Output Feature Maps" << std::endl;
						printOfMap(layerNo, OfMap_golden, 0);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Output Feature Maps" << std::endl;
						printOfMap(layerNo, Compared_OfMap, 0);
					}
				}
			}
		}
		else{
			for(int Noy_step_i=0;Noy_step_i<Noy_step;Noy_step_i++){
				for(int Nof_step_i=0;Nof_step_i<Nof_step_rom[layerNo];Nof_step_i++){

					// Initialize OutBuf [OUTBUF_NUM][WRD_OUTBUF][POX]
					for(int OutBufNum_i=0;OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
						for(int WrdOutBuf_i=0;WrdOutBuf_i<WRD_OUTBUF;WrdOutBuf_i++){
							for(int Pox_i=0;Pox_i<POX;Pox_i++){
								OutBuf[OutBufNum_i][WrdOutBuf_i][Pox_i] =
										Nof_step_i*100000 + Noy_step_i*10000
										+ OutBufNum_i*1000 + WrdOutBuf_i*100
										+ Pox_i*10;
							}
						}
					}

					if(verbose || printLayer*printNofStep*printNoyStep){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						printOutBuf(layerNo, OutBuf);
					}

					// Code below needs to be copy-paste from ConvLayer module
					ofBase = Nof_step_i; yBase = Noy_step_i;

					storeMap(/*layerCnfg=*/0, OutBuf, OfMap);

					storeMaps_software(layerNo, Nof_step_i, Noy_step_i,
							OfMap_golden, OutBuf);

					// Compare values of OfMap
					for(int Nof_i=0;Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]);Nof_i++){
						for(int Noy_i=0;Noy_i<Noy_rom[layerNo];Noy_i++){
							for(int Nox_i=0;Nox_i<Tox_rom[layerNo];Nox_i++){
								if(*( OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) !=
										*( OfMap_golden + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i )){
									check = 1;
								}
								*( Compared_OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) =
										*( OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i ) -
										*( OfMap_golden + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i );
							}
						}
					}

					// Print information for debugging
					if(verbose || (layerNo == printLayer-1 && Nof_step_i == printNofStep-1
							&& Noy_step_i == printNoyStep-1)){
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Output Feature Maps under test" << std::endl;
						printOfMap(layerNo, OfMap, 0);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Golden Output Feature Maps" << std::endl;
						printOfMap(layerNo, OfMap_golden, 0);
						std::cout << "\nFor Layer -> " << layerNo << " ,Nof_step -> " << Nof_step_i << " ,Noy_step -> " << Noy_step_i << "\n\n";
						std::cout << "Printing Comparison of Output Feature Maps" << std::endl;
						printOfMap(layerNo, Compared_OfMap, 0);
					}
				}
			}
		}
	}

	// Verification Print
	if(check){
		std::cout << "*****  Loading OutBuf to OfMap test failed!  ******\n" << std::endl;
	}
	else{
		std::cout << "*****  storeMap Test Passed!  ******\n" << std::endl;
	}
	return check;
}

// ***** Printing Functions  *****

void printIfMap(int layerNo, px_data_t *IfMap, int minPrint){
// Input Feature Maps are printed on top or below each other
	std::cout << "***  Printing Input Feature Map for Layer " << layerNo
			<< "  ***" << std::endl;

	// Calculating Variable Parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;
	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
							&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

	if(minPrint==0){
		for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
			std::cout << Nif_i+1 << " Map" << std::endl;
			for(int Niy_i=0;Niy_i<Niy;Niy_i++){
				for(int Nix_i=0;Nix_i<Tix-2;Nix_i++){
					std::cout << std::setw(4) <<
					*(IfMap
					+ Nif_i*Niy*(Tix-2)
					+ Niy_i*(Tix-2)
					+ Nix_i);
				}

				std::cout << " " << std::endl;
			}
		}
	}
	else{
		for(int Nif_i=0;(Nif_i<16) && (Nif_i<Nif_rom[layerNo]);Nif_i++){
			std::cout << Nif_i+1 << " Map" << std::endl;
			for(int Niy_i=0;(Niy_i<16) && (Niy_i<Niy);Niy_i++){
				for(int Nix_i=0;(Nix_i<16) && (Nix_i<Tix-2);Nix_i++){
					std::cout << std::setw(4) <<
					*(IfMap
					+ Nif_i*Niy*(Tix-2)
					+ Niy_i*(Tix-2)
					+ Nix_i);
				}

				std::cout << " " << std::endl;
			}
		}	
	}

}

void printWtMap(int layerNo, wt_data_t *WtMap /* [NOF][NIF][NKY][NKX] */, int minPrint){
// Input Feature Kernel are printed side by side
	int Nof = Nof_step_rom[layerNo]*Tof_rom[layerNo];
	std::cout << "***  Printing Weight Kernel Maps for Layer " << layerNo
			<< "  ***\n" << std::endl;
	if(minPrint==0){
		for(int Nof_i=0;Nof_i<Nof;Nof_i++){
			std::cout << "* Printing Weight Kernels for " << Nof_i << " OfMap *" << std::endl;
			for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
					for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
						std::cout << std::setw(4) << *(WtMap + Nof_i*Nif_rom[layerNo]*NKY*NKX + Nif_i*NKY*NKX + Nky_i*NKX +Nkx_i) << "  ";
					}
					std::cout << "|  ";
				}
				std::cout << " " << std::endl;
			}
		}
	}
	else{
		for(int Nof_i=0;(Nof_i<16) && (Nof_i<Nof);Nof_i++){
			std::cout << "* Printing Weight Kernels for " << Nof_i << " OfMap *" << std::endl;
			for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				for(int Nif_i=0;(Nif_i<16) && (Nif_i<Nif_rom[layerNo]);Nif_i++){
					for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
						std::cout << std::setw(4) << *(WtMap + Nof_i*Nif_rom[layerNo]*NKY*NKX + Nif_i*NKY*NKX + Nky_i*NKX +Nkx_i) << "  ";
					}
					std::cout << "|  ";
				}
				std::cout << " " << std::endl;
			}
		}		
	}

}

void printOfMap(int layerNo, px_data_t *OfMap /* [NOF][NOY][NOX] */, int minPrint){
// Output Feature Maps are printed on top or below each other
	std::cout << "***  Printing Output Feature Map  ***" << std::endl;
	if(minPrint==0){
		for(int Nof_i=0;Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]);Nof_i++){
			std::cout << Nof_i+1 <<" Map" << std::endl;
			for(int Noy_i=0;Noy_i<Noy_rom[layerNo];Noy_i++){
				for(int Nox_i=0;Nox_i<Tox_rom[layerNo];Nox_i++){
					std::cout << std::setw(4) << *(OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i) << "  " ;
				}
				std::cout << "|  " << std::endl;
			}

		}		
	}
	else{
		for(int Nof_i=0;(Nof_i<16) && (Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]));Nof_i++){
			std::cout << Nof_i+1 <<" Map" << std::endl;
			for(int Noy_i=0;(Noy_i<16) && (Noy_i<Noy_rom[layerNo]);Noy_i++){
				for(int Nox_i=0;(Nox_i<16) && (Nox_i<Tox_rom[layerNo]);Nox_i++){
					std::cout << std::setw(4) << *(OfMap + Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo] + Noy_i*Tox_rom[layerNo] + Nox_i) << "  " ;
				}
				std::cout << "|  " << std::endl;
			}

		}
	}
}

void printInBuf(px_data_t InBuf[POY][WRD_INBUF][POX]){
// InBuf are printed side by side
	std::cout << "***  Printing Input Buffers  ***" << std::endl;
	for(int Wrd_InBuf_i=0;Wrd_InBuf_i<WRD_INBUF;Wrd_InBuf_i++){
		// std::cout << std::setw(4) << Wrd_InBuf_i << " :   "; // Print line number for debugging
		for(int Poy_i=0;Poy_i<POY;Poy_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				std::cout << std::setw(4) << InBuf[Poy_i][Wrd_InBuf_i][Pox_i] << "  ";
			}
			std::cout << "|  ";
		}
		std::cout << " " << std::endl;
	}
}

void printWtBuf(wt_data_t WtBuf[WRD_WTBUF][POF]){
	std::cout << "***  Printing Weight Buffers  ***" << std::endl;
	for(int Wrd_WtBuf_i=0;Wrd_WtBuf_i<WRD_WTBUF;Wrd_WtBuf_i++){
		// std::cout << std::setw(4) << Wrd_WtBuf_i << " :   "; // Print line number for debugging
		for(int Pof_i=0;Pof_i<POF;Pof_i++){
			std::cout << std::setw(4) << WtBuf[Wrd_WtBuf_i][Pof_i] << "  ";
		}
		std::cout << "|  ";
		std::cout << " " << std::endl;
	}
}

void printOutBuf(int layerNo, px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]){
// OutBuf Banks are printed side by side
	std::cout << "***  Printing Output Buffers  ***" << std::endl;
	for(int Wrd_OutBuf_i=0;Wrd_OutBuf_i<WRD_OUTBUF;Wrd_OutBuf_i++){
		std::cout << std::setw(4) << Wrd_OutBuf_i << " :   "; // Print line number for debugging
		for(int OutBuf_Num_i=0;OutBuf_Num_i<OUTBUF_NUM;OutBuf_Num_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				std::cout << std::setw(4) << OutBuf[OutBuf_Num_i][Wrd_OutBuf_i][Pox_i] << "  ";
			}
			std::cout << "|  ";
		}
		std::cout << " " << std::endl;
		if( ( (Wrd_OutBuf_i+1) % (Toy_rom[layerNo]*my_ceil(Tox_rom[layerNo],POX)) ) == 0 ){
			std::cout << " " << std::endl;
		}
	}
}

// *****  Software Functions  *****

void loadIfMap_software(int layerNo, int Noy_step_i,
		px_data_t *IfMap , px_data_t InBuf[POY][WRD_INBUF][POX]){

	// Declare all other parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

	Init_Loop_1: for(int index=0; index<WRD_INBUF;index++){
		Init_Loop_2: for(int Poy_i=0; Poy_i<POY;Poy_i++){
			Init_Loop_3: for(int Pox_i=0;Pox_i<POX;Pox_i++){
				InBuf[Poy_i][index][Pox_i] = -1;
			}
		}
	}

	data_bool northTile, southTile;
	if(Noy_step_i == 0){
		northTile = 1;
	}
	else{
		northTile = 0;
	}
	if(Noy_step_i == Noy_step - 1){
		southTile = 1;
	}
	else{
		southTile = 0;
	}

	// Calculate Address Offset, of appropriate loadIfMap iteration
	int yBase;
	if(Noy_step_i == 0){
		yBase = 0;
	}
	else{
		yBase = Noy_step_i*(Tiy - 2);
	}

	// Write Input Buffers
	for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
		for(int Tiy_i=0;Tiy_i<Tiy;Tiy_i++){
			for(int Tix_i=0;Tix_i<Tix;Tix_i++){
				if(Tix_i == 0 || Tix_i == Tix-1){
					InBuf[(Tiy_i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix_i/POX]
											 [Tix_i%POX] = 0;
				}
				else if( (northTile && (Tiy_i == 0)) || (southTile && (Tiy_i == Tiy-1))){
					InBuf[(Tiy_i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix_i/POX]
																 [Tix_i%POX] = 0;
				}
				else{
					InBuf[(Tiy_i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix_i/POX][Tix_i%POX] =
						*(IfMap + Nif_i*Niy*(Tix-2) + (Tiy_i+yBase-1)*(Tix-2) + Tix_i-1);
				}
			}
		}
	}

	// 1. When I add the North or South zero padding, I don't check for end of line of IfMap
	// 2. I also add west, south padding to all Poy banks even if not used.
	// They Happen in parallel so it doesn't affect results. Maybe I add an extra zero in same POX
	// I add here those zeros for the comparison.
	for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
		for(int Tiy_i=0;Tiy_i<Tiy;Tiy_i++){
			for(int Tix_i=0;Tix_i<Tix;Tix_i++){
				if( (northTile && (Tiy_i == 0)) || (southTile && (Tiy_i == Tiy-1))){
					if((Tix%POX)!=0){
						for(int i=(Tix%POX);i<POX;i++){
							InBuf[(Tiy_i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix/POX]
																								 [i] = 0;
						}
					}
				}
			}
			if(Tiy_i == Tiy-1){
				if(Tiy%POY!=0){
					for(int i=Tiy%POY;i<POY;i++){
						int Tix_i = 0;
						InBuf[(i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix_i/POX]
																							 [Tix_i%POX] = 0;
						Tix_i = Tix-1;
						InBuf[(i)%POY][Nif_i*row_1map*wrd_1row + ((Tiy_i)/POY)*wrd_1row + Tix_i/POX]
																							 [Tix_i%POX] = 0;
					}
				}
			}
		}
	}
}

void loadWtMap_software(int layerNo, int Nof_step_i,
		wt_data_t *WtMap /* [NOF][NIF][NKY][NKX] */, wt_data_t WtBuf[WRD_WTBUF][POF]){

	// Declare all other parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);

	Init_Loop_1: for(int index=0; index<WRD_WTBUF;index++){
		Init_Loop_2: for(int Pof_i=0; Pof_i<POF;Pof_i++){
			WtBuf[index][Pof_i] = -1;
		}
	}

	// Calculate Address Offset, of appropriate loadWtMap iteration
	int ofBase = Nof_step_i*Tof_rom[layerNo];

	// Write Weight Buffers
	for(int Nif_i=0;Nif_i<Nif_rom[layerNo];Nif_i++){
		for(int Tof_i=0;Tof_i<Tof_rom[layerNo];Tof_i++){
			for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
					WtBuf[(Tof_i/POF)*Nif_rom[layerNo]*NKY*NKX
						  + Nif_i*NKY*NKX
						  + Nky_i*NKX
						  + Nkx_i]
						  [Tof_i%POF] =
							*(WtMap + (Tof_i+ofBase)*Nif_rom[layerNo]*NKY*NKX
									+ Nif_i*NKY*NKX
									+ Nky_i*NKX
									+ Nkx_i);
				}
			}
		}
	}
}

void storeMaps_software(int layerNo, int Nof_step_i, int Noy_step_i,
		px_data_t *OfMap /* [NOF][NOY][NOX] */, px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]){

	// Declare all other parameters
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;

	parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
		&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);


	// Write Buffers to Memory
	for(int Tof_i=0;Tof_i<Tof_rom[layerNo];Tof_i++){
		for(int Toy_i=0;Toy_i<Toy_rom[layerNo];Toy_i++){
			for(int Tox_i=0;Tox_i<Tox_rom[layerNo];Tox_i++){
				*(OfMap
				+ (Tof_i + Nof_step_i*Tof_rom[layerNo])*Noy_rom[layerNo]*Tox_rom[layerNo]
				+ (Toy_i + Noy_step_i*Toy_rom[layerNo])*Tox_rom[layerNo]
				+ Tox_i) =
						OutBuf[Tof_i%OUTBUF_NUM]
							   [  (Tof_i/OUTBUF_NUM)*Tox_step*Toy_rom[layerNo]
								+ Toy_i*Tox_step
								+ Tox_i/POX]
								[Tox_i%POX];
			}
		}
	}
}

// *****  Auxiliary Functions  *****

void initInBuf_undef(px_data_t InBuf[POY][WRD_INBUF][POX]){
	for(int Poy_i=0;Poy_i<POY;Poy_i++){
		for(int wrdInBuf_i=0; wrdInBuf_i<WRD_INBUF;wrdInBuf_i++){
			for(int Pox_i=0;Pox_i<POX;Pox_i++){
				InBuf[Poy_i][wrdInBuf_i][Pox_i] = -1;
			}
		}
	}
}

void initWtBuf_undef(wt_data_t WtBuf[WRD_WTBUF][POF]){
	for(int wrdWtBuf_i=0; wrdWtBuf_i<WRD_WTBUF;wrdWtBuf_i++){
		for(int Pof_i=0;Pof_i<POF;Pof_i++){
			WtBuf[wrdWtBuf_i][Pof_i] = -1;
		}
	}
}

void initOfMap_undef(int layerNo, px_data_t *OfMap /* [NOF][NOY][NOX] */, int value){
	for(int Nof_i=0; Nof_i<(Nof_step_rom[layerNo]*Tof_rom[layerNo]);Nof_i++){
		for(int Noy_i=0; Noy_i<Noy_rom[layerNo];Noy_i++){
			for(int Nox_i=0;Nox_i<Tox_rom[layerNo];Nox_i++){
				*(OfMap
				+ Nof_i*Noy_rom[layerNo]*Tox_rom[layerNo]
				+ Noy_i*Tox_rom[layerNo]
				+ Nox_i) = value;
			}
		}
	}
}
