#include "header.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void printDesignChoice(){
	// Conv. Block Integrations
	#if not (defined(MAXPOOL_INTEGRATION) && defined(HEAD_INTEGRATION))
		std::cout << "No integration of max pooling or network head in the convolutional layer."
			<< std::endl;
	#else
		#if(defined(MAXPOOL_INTEGRATION))
			std::cout << "Max pooling was integrated in the convolutional layer."
				<< std::endl;
		#endif
		#if(defined(HEAD_INTEGRATION))
			std::cout << "Neural network head was integrated in the convolutional layer."
				<< std::endl;
		#endif
	#endif
	// Port Widening
	#if not (defined(FMAP_WIDEN) && defined(WTMAP_WIDEN))
		std::cout << "No port widening applied."
			<< std::endl;
	#else
		#if(defined(FMAP_WIDEN))
			std::cout << "Port widening of factor " << FMAP_WIDTHFACTOR << " was implemented for feature map data."
				<< std::endl;
		#endif
		#if(defined(WTMAP_WIDEN))
			std::cout << "Port widening of factor " << WTMAP_WIDTHFACTOR << " was implemented for weight data."
				<< std::endl;
		#endif
	#endif
	// Schediling (Outer Region)
		// #if (defined(REGION3_SEQ))
		// 	std::cout << "Sequential scheduling for outer region (3) was applied."
		// 		<< std::endl;
		// #else
		// 	std::cout << "Sequential scheduling for outer region (3) was applied."
		// 		<< std::endl;
		// #endif
	// Schediling (Middle Region)
	#if (defined(REGION2_SEQ))
		std::cout << "Sequential scheduling for middle region (2) was applied."
			<< std::endl;
	#elif (defined(REGION2_DFL))
		std::cout << "Dataflow scheduling for middle region (2) was applied."
			<< std::endl;
	#elif (defined(REGION2_MNLSCHEDULE_2BUF))
		std::cout << "Manual scheduling with double buffering for middle region (2) was applied."
			<< std::endl;
	#elif (defined(REGION2_PPL))
		std::cout << "Pipeline scheduling for middle region (2) was applied."
			<< std::endl;
	#endif
	// Miscellaneous
	#if not (defined(SET_CONFIG_LAYER))
		std::cout << "layerCnfg is an internal state."
				<< std::endl;
	#else
		std::cout << "layerCnfg signal is passed as input."
				<< std::endl;		
	#endif
}


int Print_Check_Parameters(int verbose){
	// Checking Convolutional Layer parameters.
	// Print parameters that are same for all layers.
	// Print necessary parameters
	// Print all other parameters
	// Print buffer parameters

	// Rules are
	// 1. P*<T*
	// 2. N*%T*=0 (in Nix, Niy add zero padding)
	// 3. Pof > #OutBuf
	// 4. Buffer Size is enough for all layers.
	// 5. Memory Size is enough for feature maps.
	// 5. T*<N* (is true by default)
	// 6. Nif=Tif, Nix=Tix (is true by default)
	// 7. Dimensions and Tile equation (is true by default)
	//    (NIY[count]-NKY)%S=0, (NIX[count]-NKX)%S=0
	//    Niy=(Noy-1)*S+Nky, Nix=(Nox-1)*S+Nkx -> Niy=Noy+2, Nix=Nox+2
	// Detect where
	// 1. T*%P*=0 or Nof=Tof or Noy=Toy

	if(verbose){
		std::cout << " * Parameters for Convolutional Layers" << std::endl;
		std::cout << " ** Constant Parameters" << std::endl;
		std::cout << "Number of Layers -> " << LAYERS << std::endl;
		std::cout << "Stride -> " << S << "  Zero Padding -> " << ZERO_PAD << std::endl;
		std::cout << "Nkx -> " << NKX << "  Nky -> " << NKY << std::endl;
		std::cout << " *** P* Parameters:" << std::endl;
		std::cout << "Pif -> " << PIF << "  Pof -> " << POF << "  ";
		std::cout << "Piy -> " << PIY << "  Pix -> " << PIX << "  ";
		std::cout << "Poy -> " << POY << "  Pox -> " << POX << std::endl;
		std::cout << " *** Buffer Sizing:" << std::endl;
		std::cout << "Row size of Input Pixel Buffer -> " << WRD_INBUF << "  Row size of Weight Buffer -> " << WRD_WTBUF << std::endl;
		std::cout << "Row size of Output Pixel Buffer -> " << WRD_OUTBUF << "  Bank Number of Output Pixel Buffer -> " << OUTBUF_NUM << std::endl;
		std::cout << " *** Memory Sizing:" << std::endl;
		std::cout << "Memory Size for Input Feature Maps -> " << FMAP_MEMSIZE << std::endl;
		std::cout << "Memory Size for Weight Maps -> " << WTMAP_MEMSIZE << std::endl;
		std::cout << "Memory Size for Output Feature Maps -> " << FMAP_MEMSIZE << std::endl;
		std::cout << " *** Secondary Parameters:" << std::endl;
		std::cout << "  Pof step in OutBuf banks -> " << POFBANK_STEP << std::endl;
		std::cout << " ** Variable Parameters" << std::endl;
		std::cout << " *** Comment:\nTiy, Tix include padding, Niy,Nix doesn't include padding" << std::endl;
		std::cout << " *** Always true equations:" << std::endl;
		std::cout << "Nif = Tif, Pif=1" << std::endl;
		std::cout << "Nof%Tof = 0, Niy = Noy, Noy%Toy = 0" << std::endl;
		std::cout << "Nix = Nox, Nix = Tix-2" << std::endl;
		std::cout << "Tiy = Toy+2, Tix = Tox+2" << std::endl;
		std::cout << "Piy = Poy, Pix = Pox\n" << std::endl;
		for(int layerNo=0;layerNo<LAYERS;layerNo++){
			std::cout << " *** For Layer " << layerNo << std::endl;
			std::cout << "Nif -> " << Nif_rom[layerNo] << "  Nof -> " << Nof_step_rom[layerNo]*Tof_rom[layerNo] << std::endl;
			std::cout << "Niy -> " << niy_rom[layerNo] << "  Nix -> " << tix_rom[layerNo]-2 << std::endl;
			std::cout << "Noy -> " << Noy_rom[layerNo] << "  Nox -> " << Tox_rom[layerNo]-2 << std::endl;

			std::cout << "Tif -> " << Nif_rom[layerNo] << "  Tof -> " << Nof_step_rom[layerNo]*Tof_rom[layerNo] << std::endl;
			std::cout << "Tiy -> " << niy_rom[layerNo] << "  Tix -> " << tix_rom[layerNo]-2 << std::endl;
			std::cout << "Toy -> " << Noy_rom[layerNo] << "  Tox -> " << Tox_rom[layerNo]-2 << std::endl;

			std::cout << "ceil(Nof/Tof) = Nof_step -> " << Nof_step_rom[layerNo] << "  ceil(Noy/Toy) = Noy_step -> " << noy_step_rom[layerNo] << std::endl;
			if(Nof_step_rom[layerNo] == 1){
				std::cout << "We have fully buffered weights." << std::endl;
			}
			if(noy_step_rom[layerNo] == 1){
				std::cout << "We have fully buffered pixels." << std::endl;
			}
			std::cout << "ceil(Tof/POF) = Tof_step -> " << tof_step_rom[layerNo] << "  ceil(Toy/POY) = Toy_step -> " << toy_step_rom[layerNo]
					<< "  ceil(Tox/POX) = Tox_step -> " << tox_step_rom[layerNo] << std::endl;
			std::cout << "ceil(Tiy/POY) = row_1map -> " << row_1map_rom[layerNo] << " ceil(Tix/POX) = wrd_1row -> " << wrd_1row_rom[layerNo] << std::endl;

			std::cout << "We have -> Tof%Pof = " << Tof_rom[layerNo]%POF << " ,  Toy%Poy = " << Toy_rom[layerNo]%POY
					<< " ,  Tox%Pox = " << Tox_rom[layerNo]%POX << "\n\n";

			std::cout << "We have -> Loop limit for Pe module = " << pe_loop_limit_rom[layerNo] << "\n";
			std::cout << "Loop limit for WtBuf2Pe module = " << wtbuf2pe_loop_limit_rom[layerNo] << "\n";
			std::cout << "Loop limit for wndClc module = " << wndclc_loop_limit_rom[layerNo] << "\n";
			std::cout << "Loop limit for tileClc module = " << tileclc_loop_limit_rom[layerNo] << "\n\n";
		}
	}
	// Check Parameter constraints and declartions
	int check = 0;

	// 1. Pof >= #OutBuf
	if(OUTBUF_NUM>POF){
		std::cout << "#OutBuf not chosen correctly." << std::endl;
		check = 1;
	}
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		// 2. P*<T*
		if(POF>Tof_rom[layerNo] || POY>Toy_rom[layerNo] || POX>Tox_rom[layerNo]){
			std::cout << "P* parameter not smaller than T*." << std::endl;
			std::cout << "Error in layer " << layerNo << std::endl;
			check = 1;
		}
		// 3. N*%T*=0 (in Nix, Niy add zero padding)
		if((Nof_step_rom[layerNo]*Tof_rom[layerNo])%Tof_rom[layerNo]!=0 ||
				Noy_rom[layerNo]%Toy_rom[layerNo]!=0){
			std::cout << "T* doesn't perfectly divide N*." << std::endl;
			std::cout << "Error in layer " << layerNo << std::endl;
			check = 1;
		}

		// 4. Buffer Size is enough for all layers.
		if(WRD_INBUF<wrd_1row_rom[layerNo] * row_1map_rom[layerNo] * Nif_rom[layerNo] ||
				WRD_WTBUF < NKX * NKY * Nif_rom[layerNo] * my_ceil(Tof_rom[layerNo],POF) ||
				WRD_OUTBUF < my_ceil(Tof_rom[layerNo], OUTBUF_NUM) * Toy_rom[layerNo] * my_ceil(Tox_rom[layerNo], POX)){
			std::cout << "Buffer size is not enough." << std::endl;
			std::cout << "Error in layer " << layerNo << std::endl;
			check = 1;
		}
		// 5. Memory Size is enough for feature maps.
		if(FMAP_MEMSIZE < Nif_rom[layerNo]*niy_rom[layerNo]*(tix_rom[layerNo]-2) ||
				WTMAP_MEMSIZE < Nif_rom[layerNo]*Nof_step_rom[layerNo]*Tof_rom[layerNo]*NKY*NKX ||
				FMAP_MEMSIZE < Nof_step_rom[layerNo]*Tof_rom[layerNo]*Noy_rom[layerNo]*Tox_rom[layerNo]){
			std::cout << "Memory size is not enough." << std::endl;
			std::cout << "Error in layer " << layerNo << std::endl;
			check = 1;
		}
	}

	check = Check_Binary_Lengths();

	// All other conv layer parameters needed (to be calculated)
	int Noy_step /*Noy/Toy*/, Niy;
	int Tof_step /*ceil(Tof/Pof)*/, Toy_step /*ceil(Toy/Poy)*/, Tox_step /*ceil(Tox/Pox)*/;
	int Tiy, Tix;
	int row_1map /*ceil(Tiy/Poy)*/, wrd_1row /*ceil(Tix/Pox)*/;
	// Verify variable parameters in rom with parameterCalculation()
	for(int layerNo=0;layerNo<LAYERS;layerNo++){
		parameterCalculation(Noy_rom, Tof_rom, Toy_rom, Tox_rom, layerNo,
			&Noy_step, &Niy, &Tof_step, &Toy_step, &Tox_step, &Tiy, &Tix, &row_1map, &wrd_1row);
		if( 	noy_step_rom[layerNo] != Noy_step || niy_rom[layerNo] != Niy           ||
				tof_step_rom[layerNo] != Tof_step || toy_step_rom[layerNo] != Toy_step ||
				tox_step_rom[layerNo] != Tox_step || tiy_rom[layerNo] != Tiy           ||
				tix_rom[layerNo] != Tix           || row_1map_rom[layerNo] != row_1map ||
				wrd_1row_rom[layerNo] != wrd_1row){
			std::cout << "Variable Parameters are not stored correctly!!!" << std::endl;
			check = 1;
		}
		if( nofy_step_rom[layerNo]!= ( (Nof_step_rom[layerNo]>noy_step_rom[layerNo])?Nof_step_rom[layerNo]:noy_step_rom[layerNo] ) ){
			std::cout << "Variable Parameters are not stored correctly(2)!!!" << std::endl;
			check = 1;
		}
		if(		pe_loop_limit_rom[layerNo] != Nif_rom[layerNo]*NKX*NKY-1                ||
				wtbuf2pe_loop_limit_rom[layerNo] != Tof_step*Nif_rom[layerNo]*NKY*NKX-1 ||
				wndclc_loop_limit_rom[layerNo] != Nif_rom[layerNo]*NKY*NKX                           ||
				tileclc_loop_limit_rom[layerNo] != Tof_step*Toy_step*Tox_step){
			std::cout << "Loop limits are not stored correctly!!!" << std::endl;
			check = 1;
		}
		if(		pe2buf_addr_offset1_rom[layerNo] != POY*tox_step_rom[layerNo] - (tox_step_rom[layerNo]-1)
				- (tof_step_rom[layerNo]-1)*(POF/OUTBUF_NUM)*Toy_rom[layerNo]*tox_step_rom[layerNo] ||
				pe2buf_addr_offset2_rom[layerNo] != (tof_step_rom[layerNo]-1)*(POF/OUTBUF_NUM)*Toy_rom[layerNo]*tox_step_rom[layerNo] ||
				pe2buf_addr_offset3_rom[layerNo] != (POF/OUTBUF_NUM)*Toy_rom[layerNo]*tox_step_rom[layerNo]){
			std::cout << "Pe2Buf address offsets are not stored correctly!!!" << std::endl;
			check = 1;
		}
	}

	std::cout << "*****  Rules of Convolutional Parameters Verified!  *****\n" << std::endl;
	return check;
}


int Check_Binary_Lengths(){
	int Nif_max = 0, Niy_max = 0, Noy_max = 0, Nof_step_max = 0 , Noy_step_max = 0, Nofy_step_max = 0;
	int Tiy_max = 0, Tix_max = 0, Tof_max = 0, Toy_max = 0, Tox_max = 0;
	int Tof_step_max = 0, Toy_step_max = 0, Tox_step_max = 0;
	int wrd_1row_max = 0, row_1map_max = 0;
	int conv_loop_max = 0, tile_loop_max = 0;
	int wnd_loop_max = 0, wtbuf2pe_loop_max = 0;
	int tmp;

	for(int i=0;i<LAYERS;i++){
		Nif_max = (Nif_max>Nif_rom[i]) ? Nif_max : Nif_rom[i];
		Niy_max = (Niy_max>niy_rom[i]) ? Niy_max : niy_rom[i];
		Noy_max = (Noy_max>Noy_rom[i]) ? Noy_max : Noy_rom[i];
		Nof_step_max = (Nof_step_max>Nof_step_rom[i]) ? Nof_step_max : Nof_step_rom[i];
		Noy_step_max = (Noy_step_max>noy_step_rom[i]) ? Noy_step_max : noy_step_rom[i];
		Nofy_step_max = (Nof_step_max>Noy_step_max) ? Nof_step_max : Noy_step_max;

		Tiy_max = (Tiy_max>tiy_rom[i]) ? Tiy_max : tiy_rom[i];
		Tix_max = (Tix_max>tix_rom[i]) ? Tix_max : tix_rom[i];
		Tof_max = (Tof_max>Tof_rom[i]) ? Tof_max : Tof_rom[i];
		Toy_max = (Toy_max>Toy_rom[i]) ? Toy_max : Toy_rom[i];
		Tox_max = (Tox_max>Tox_rom[i]) ? Tox_max : Tox_rom[i];

		Tof_step_max = (Tof_step_max>tof_step_rom[i]) ? Tof_step_max : tof_step_rom[i];
		Toy_step_max = (Toy_step_max>toy_step_rom[i]) ? Toy_step_max : toy_step_rom[i];
		Tox_step_max = (Tox_step_max>tox_step_rom[i]) ? Tox_step_max : tox_step_rom[i];

		wrd_1row_max = (wrd_1row_max>wrd_1row_rom[i]) ? wrd_1row_max : wrd_1row_rom[i];
		row_1map_max = (row_1map_max>row_1map_rom[i]) ? row_1map_max : row_1map_rom[i];

		tmp = (Nof_step_rom[i]>noy_step_rom[i]) ? Nof_step_rom[i] : noy_step_rom[i];
		conv_loop_max = (conv_loop_max>tmp) ? conv_loop_max : tmp;
		tmp = tof_step_rom[i]*toy_step_rom[i]*tox_step_rom[i];
		tile_loop_max = (tile_loop_max>tmp) ? tile_loop_max : tmp;
		tmp = Nif_rom[i]*NKY*NKX;
		wnd_loop_max = (wnd_loop_max>tmp) ? wnd_loop_max : tmp;
		tmp = tof_step_rom[i]*Nif_rom[i]*NKY*NKX-1;
		wtbuf2pe_loop_max = (wtbuf2pe_loop_max>tmp) ? wtbuf2pe_loop_max : tmp;
	}

	if( (LAYERS-1)>(pow(2, LAYER_B)-1) || (NKY-1)>(pow(2, NKY_B)-1) || (NKX-1)>(pow(2, NKX_B)-1)){
		std::cout << "*****  Binary Lengths are not enough!(0)  *****\n" << std::endl;
		return 1;
	}

	if(		Nif_max>(pow(2, NIF_B)-1) || (Nif_max-1)>(pow(2, NIF_I_B)-1) ||
			Niy_max>(pow(2, NIY_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(1)  *****\n" << std::endl;
		return 1;
	}
	if(		Noy_max>(pow(2, NOY_B)-1)                 || Nof_step_max>(pow(2, NOF_STEP_B)-1) ||
			(Nof_step_max-1)>(pow(2, NOF_STEP_I_B)-1) || Noy_step_max>(pow(2, NOY_STEP_B)-1) ||
			(Noy_step_max-1)>(pow(2, NOY_STEP_I_B)-1)){
		std::cout << "*****  Binary Lengths are not enough!(2)  *****\n" << std::endl;
		return 1;
	}
	if(		Nofy_step_max>(pow(2, NOFY_STEP_B)-1) 	  || Nofy_step_max-1>(pow(2, NOFY_STEP_I_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(3)  *****\n" << std::endl;
		return 1;
	}
	if(		Tiy_max>(pow(2, TIY_B)-1) || Tix_max>(pow(2, TIX_B)-1) || Tof_max>(pow(2, TOF_B)-1) ||
			Toy_max>(pow(2, TOY_B)-1) || Tox_max>(pow(2, TOX_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(4)  *****\n" << std::endl;
		return 1;
	}
	if(		Tof_step_max>(pow(2, TOF_STEP_B)-1) || (Tof_step_max-1)>(pow(2, TOF_STEP_I_B)-1)  ||
			Toy_step_max>(pow(2, TOY_STEP_B)-1) || (Toy_step_max-1)>(pow(2, TOY_STEP_I_B)-1)  ||
			Tox_step_max>(pow(2, TOX_STEP_B)-1) || (Tox_step_max-1)>(pow(2, TOX_STEP_I_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(5)  *****\n" << std::endl;
		return 1;
	}
	if(		POF>(pow(2, POF_B)-1)       || (POF-1)>(pow(2, POF_I_B)-1) || POY>(pow(2, POY_B)-1)       ||
			(POY-1)>(pow(2, POY_I_B)-1) || POX>(pow(2, POX_B)-1)       || (POX-1)>(pow(2, POX_I_B)-1) ||
			(POFBANK_STEP-1)>(pow(2, POFBANK_STEP_I_B)-1)){
		std::cout << "*****  Binary Lengths are not enough!(6)  *****\n" << std::endl;
		return 1;
	}
	if(wrd_1row_max>(pow(2, WRD_1ROW_B)-1) || row_1map_max>(pow(2, ROW_1MAP_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(7)  *****\n" << std::endl;
		return 1;
	}
	if(		conv_loop_max>(pow(2, CONV_LOOP_B)-1) || tile_loop_max>(pow(2, TILE_LOOP_B)-1)        ||
			wnd_loop_max>(pow(2, WND_LOOP_B)-1)   || wtbuf2pe_loop_max>(pow(2, WTBUF2PE_LOOP_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(8)  *****\n" << std::endl;
		return 1;
	}
	if(		(WRD_INBUF-1)>(pow(2, ROW_INBUF_I_B)-1)     || (WRD_WTBUF-1)>(pow(2, WRD_WTBUF_I_B)-1)        ||
			(WRD_OUTBUF-1)>(pow(2, WRD_OUTBUF_I_B)-1) || (BIASBUF_LENGTH-1)>(pow(2, WRD_BIASBUF_I_B)-1) ||
			(OUTBUF_NUM-1)>(pow(2, OUTBUFNUM_I_B)-1) ){
		std::cout << "*****  Binary Lengths are not enough!(9)  *****\n" << std::endl;
		return 1;
	}

	// Checking to see if lengths are the minimum that can be achieved
	if(		( (LAYERS-1)<=(pow(2, LAYER_B-1)-1) && (pow(2, LAYER_B-1)-1)!=0 ) || (NKY-1)<=(pow(2, NKY_B-1)-1) || (NKX-1)<=(pow(2, NKX_B-1)-1) ){
		std::cout << "Binary Length not minimum here (0)!\n" << std::endl;
	}
	if(		Nif_max<=(pow(2, NIF_B-1)-1) || (Nif_max-1)<=(pow(2, NIF_I_B-1)-1) ||
			Niy_max<=(pow(2, NIY_B-1)-1) ){
		std::cout << "Binary Length not minimum here (1)!\n" << std::endl;
	}
	if(		Noy_max<=(pow(2, NOY_B-1)-1)                 || Nof_step_max<=(pow(2, NOF_STEP_B-1)-1) ||
			( (Nof_step_max-1)<=(pow(2, NOF_STEP_I_B-1)-1) &&  (pow(2, NOF_STEP_I_B-1)-1) !=0 )    ||
			Noy_step_max<=(pow(2, NOY_STEP_B-1)-1)       ||
			( (Noy_step_max-1)<=(pow(2, NOY_STEP_I_B-1)-1) &&  (pow(2, NOY_STEP_I_B-1)-1) !=0 )){
		std::cout << "Binary Length not minimum here (2)!\n" << std::endl;
	}
	if(		Nofy_step_max<=(pow(2, NOFY_STEP_B-1)-1) 	  || Nofy_step_max-1<=(pow(2, NOFY_STEP_I_B-1)-1) ){
		std::cout << "Binary Length not minimum here (3)!\n" << std::endl;
	}
	if(		Tiy_max<=(pow(2, TIY_B-1)-1) || Tix_max<=(pow(2, TIX_B-1)-1) ||
			Tof_max<=(pow(2, TOF_B-1)-1) || Toy_max<=(pow(2, TOY_B-1)-1)        ||
			Tox_max<=(pow(2, TOX_B-1)-1) ){
		std::cout << "Binary Length not minimum here (4)!\n" << std::endl;
	}
	if(		Tof_step_max<=(pow(2, TOF_STEP_B-1)-1)       || (Tof_step_max-1)<=(pow(2, TOF_STEP_I_B-1)-1) && (pow(2, TOF_STEP_I_B-1)-1!=0) || Toy_step_max<=(pow(2, TOY_STEP_B-1)-1)      ||
			(Toy_step_max-1)<=(pow(2, TOY_STEP_I_B-1)-1) || Tox_step_max<=(pow(2, TOX_STEP_B-1)-1)       || (Tox_step_max-1)<=(pow(2, TOX_STEP_I_B-1)-1) ){
		std::cout << "Binary Length not minimum here (5)!\n" << std::endl;
	}
	if(     POF<=(pow(2, POF_B-1)-1)       || (POF-1)<=(pow(2, POF_I_B-1)-1) || POY<=(pow(2, POY_B-1)-1)       ||
			(POY-1)<=(pow(2, POY_I_B-1)-1) || POX<=(pow(2, POX_B-1)-1)       || (POX-1)<=(pow(2, POX_I_B-1)-1) ||
			(POFBANK_STEP-1)<=(pow(2, POFBANK_STEP_I_B-1)-1)){
		std::cout << "Binary Length not minimum here (6)!\n" << std::endl;
	}
	if(wrd_1row_max<=(pow(2, WRD_1ROW_B-1)-1) || row_1map_max<=(pow(2, ROW_1MAP_B-1)-1) ){
		std::cout << "Binary Length not minimum here (7)!\n" << std::endl;
	}
	if(     conv_loop_max<=(pow(2, CONV_LOOP_B-1)-1) || tile_loop_max<=(pow(2, TILE_LOOP_B-1)-1)       ||
			wnd_loop_max<=(pow(2, WND_LOOP_B-1)-1)   || wtbuf2pe_loop_max<=(pow(2, WTBUF2PE_LOOP_B-1)-1) ){
		std::cout << "Binary Length not minimum here (8)!\n" << std::endl;
	}
	if(     (WRD_INBUF-1)<=(pow(2, ROW_INBUF_I_B-1)-1)     || (WRD_WTBUF-1)<=(pow(2, WRD_WTBUF_I_B-1)-1)        ||
			(WRD_OUTBUF-1)<=(pow(2, WRD_OUTBUF_I_B-1)-1) || (BIASBUF_LENGTH-1)<=(pow(2, WRD_BIASBUF_I_B-1)-1) ||
			(OUTBUF_NUM-1)<=(pow(2, OUTBUFNUM_I_B-1)-1)){
		std::cout << "Binary Length not minimum here (9)!\n" << std::endl;
	}
	return 0;
}
