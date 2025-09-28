#include "header.h"
#include "hls_stream.h"
#include <iostream>
#include <chrono> // for wait timer
#include <thread> // for wait timer

void ReLu(
		px_data_t in, px_data_t *out){
	#pragma HLS INLINE
	if (in > 0) {
		*out = in;
	} else {
		*out = 0;
	}
}


void bias_ReLu(
		// "State Input
		Tof_step_i_dt Tof_step_i,
		// Shift number
		bit_shift_dt bit_shift,
		// Inputs
		b_data_t BiasBuf[BIASBUF_LENGTH], acc_data_t pxSerial[OUTBUF_NUM][POX],
		// Output
		px_data_t Out[OUTBUF_NUM][POX]
	){
	static Poy_i_dt Poy_i = 0;                   // Locally keep track of iterations (1)
	static PofBank_step_i_dt POFBANK_STEP_i = 0; // Locally keep track of iterations (2)
	px_data_t ReLuOut[OUTBUF_NUM][POX];
	px_data_t pxSerialShift[OUTBUF_NUM][POX];
	// Read OutBuf_Num*Pox px from PE (input to this module)
	// Read OutBuf_Num bias
	// scale, add and apply ReLu in parallel
	// as input is an array with biases for tile

	for(int i=0;i<OUTBUF_NUM;i++){
	#pragma HLS UNROLL
		for(int Pox_i=0;Pox_i<POX;Pox_i++){
		#pragma HLS UNROLL
			pxSerialShift[i][Pox_i] = pxSerial[i][Pox_i] >> bit_shift;
			pxSerialShift[i][Pox_i] += BiasBuf[Tof_step_i*POF + POFBANK_STEP_i*OUTBUF_NUM + i];
			#ifdef DEBUG_MODE
				if(Tof_step_i*POF + POFBANK_STEP_i*OUTBUF_NUM + i >= 256){
					std::cout << "Out of bounds bias buffer access!";
				}
			#endif
			ReLu(pxSerialShift[i][Pox_i], &(ReLuOut[i][Pox_i]));
			Out[i][Pox_i] = ReLuOut[i][Pox_i];
			
		}
	}

	if( Poy_i == POY-1 ){
		Poy_i = 0;
		if(POFBANK_STEP_i == POFBANK_STEP-1){
			POFBANK_STEP_i = 0;
		}
		else{
			POFBANK_STEP_i++;
		}
	}
	else{
		Poy_i++;
	}
	/*	// Bypass bias + ReLu
		loop_bias_ReLu_OutBufNum_Bypass: for(int i=0;i<OUTBUF_NUM;i++){
		#pragma HLS UNROLL
			loop_bias_ReLu_Pox_Bypass: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				ReLuOut[i][Pox_i] = pxSerial[i][Pox_i];
			}
		}	
	*/
}


void Pe(
		// Inputs
		px_data_t px_stream[POY][POX], wt_data_t wt_stream[POF],
		// Output
		acc_data_t rslt_stream[POF][POY][POX]
	){
	#pragma HLS INLINE

	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_stream
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_stream

	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=wt_stream

	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=rslt_stream
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=rslt_stream
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=rslt_stream


	loop_Pe_Pof_MAC: for(int Pof_i=0;Pof_i<POF;Pof_i++){
	#pragma HLS UNROLL
		loop_Pe_Poy_MAC: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS UNROLL
			loop_Pe_Pox_MAC: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				rslt_stream[Pof_i][Poy_i][Pox_i] += px_stream[Poy_i][Pox_i] * wt_stream[Pof_i];
			}
		}
	}
}


void InBuf2Pe_ctrl(
		// Parameters
		Nif_dt Nif,
		wrd_1row_dt wrd_1row, row_1map_dt row_1map,
		// Address & Control Signal Outputs
		row_inbuf_i_dt *row_px, Pox_i_dt *col, Poy_i_dt *bank,
		data_bool *dct_ld, data_bool *fl_ld, data_bool *wr_fifo
	){
	#pragma HLS INLINE
	static Nif_i_dt Nif_i = 0;			// Initialization: To track inner rolled loop (1)
	static Nky_dt Nky_i = 0; 			// Initialization: To track inner rolled loop (2)
	static Nkx_dt Nkx_i = 0;			// Initialization: To track inner rolled loop (3)
	// Calculate control signals, based on iteration
	if( Nkx_i == 0){
		*fl_ld = 1;
	}
	else{
		*fl_ld = 0;
	}
	if( Nky_i == 0){
		*dct_ld = 1;
	}
	else{
		*dct_ld = 0;
	}
	if( Nky_i == NKY-1 ){
		*wr_fifo = 0;
	}
	else{
		*wr_fifo = 1;
	}

	// Calculate address for loading InBuf
	// Row
	if( Nif_i != 0 && Nky_i == 0 && Nkx_i == 0 ){
		(*row_px) += wrd_1row*row_1map - wrd_1row - 1;
	}
	else if( Nky_i == 1 && Nkx_i == 0 ){
		(*row_px) += wrd_1row - 1;
	}
	else if( Nkx_i == 0 && ( Nif_i != 0 || Nky_i != 0 ) ){
		(*row_px) += -1;
	}
	else if( Nkx_i == 1 ){
		(*row_px)++ ;
	}
	// Column
	if( Nkx_i == 0){
		*col = 0;
	}
	else{
		*col = Nkx_i-1;
	}
	// Bank
	if( Nky_i == 0){
		*bank = POY-1;
	}
	else{
		*bank = Nky_i-1;
	}

	if(0){ // Debugging Prints
		std::cout << "*** In the module -> Nif = " << Nif_i << " Nky = " << Nky_i << " Nkx = " << Nkx_i << "\n";
		std::cout << "*** In the module -> dct_ld = " << *dct_ld << " fl_ld = " << *fl_ld << " wr_fifo = " << *wr_fifo << "\n\n";
		std::cout << "*** In the module -> row_px = " << *row_px << "\n\n";
	}

	// Loop iteration calculation
	if( Nkx_i == NKX-1){
		Nkx_i = 0;
		if( Nky_i == NKY-1){
			Nky_i = 0;
			if( Nif_i == Nif-1){
				Nif_i = 0;
			}
			else{
				Nif_i++;
			}
		}
		else{
			Nky_i++;
		}
	}
	else{
		Nkx_i++;
	}
}


void InBuf2Pe_wrapper(
		// Inputs
		row_inbuf_i_dt row, Pox_i_dt col,
		Poy_i_dt bank, data_bool dct_ld, data_bool fl_ld,
		data_bool wr_fifo, const px_data_t InBuf[POY][WRD_INBUF][POX],
		// Output
		px_data_t px_stream[POY][POX]
	){
	#pragma HLS INLINE

	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=InBuf
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=InBuf

	static hls::stream<px_data_t,4> FIFO_arr[POY][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=FIFO_arr
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=FIFO_arr

	InBuf2Pe(row, col, bank, dct_ld, fl_ld, wr_fifo, InBuf, px_stream, FIFO_arr);
}


void InBuf2Pe(
		// Inputs
		row_inbuf_i_dt row, Pox_i_dt col,
		Poy_i_dt bank, data_bool dct_ld, data_bool fl_ld,
		data_bool wr_fifo, const px_data_t InBuf[POY][WRD_INBUF][POX],
		// Output
		px_data_t px_stream[POY][POX],
		// Input-Output
		hls::stream<px_data_t,4> FIFO_arr[POY][POX]
	){
	#pragma HLS INLINE
	// * Sequence
	// (1.a) : Read InBuf to Internal Registers
	// (1.b) : Partly read InBuf to Internal Registers and shift
	// (1.c) : Read FIFO to Internal Registers
	// (2.a) : Write Internal Registers to PE
	// (2.b) : Write Internal Registers to PE and FIFO
	// Step 1 & 2 are pipelined

	static px_data_t Internal_Reg[POY][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=Internal_Reg
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=Internal_Reg

	 // *** Read Pixels from InBuf ***
	px_data_t Buff_read[POY][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=Buff_read
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=Buff_read

	if(fl_ld){
		loop_InBuf2Pe_rdPx_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS UNROLL
			loop_InBuf2Pe_rdPx_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				Buff_read[Poy_i][Pox_i] = InBuf[Poy_i][row][Pox_i];
			}
		}
	}
	else{
		loop_InBuf2Pe_rdPx_Poy_2: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS UNROLL
			Buff_read[Poy_i][col] = InBuf[Poy_i][row][col];
		}
	}

	// *** Write Internal Registers ***
	// Internal Registers (minus outer bank) read POX pixels or 1 pixel and shift OR read FIFO
	if(dct_ld){ // read from Buffer
		if(fl_ld){ // read Pox pixels
			loop_InBuf2Pe_wrReg_Poy: for(int Poy_i=0;Poy_i<POY-1;Poy_i++){
			#pragma HLS UNROLL
				loop_InBuf2Pe_wrReg_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
				#pragma HLS UNROLL
					Internal_Reg[Poy_i][Pox_i] = Buff_read[Poy_i][Pox_i];
				}
			}
		}
		else{ // read 1 pixel and shift
			loop_InBuf2Pe_wrReg_Poy_2: for(int Poy_i=0;Poy_i<POY-1;Poy_i++){
			#pragma HLS UNROLL
				loop_InBuf2Pe_wrReg_Pox_2: for(int Pox_i=0;Pox_i<POX-1;Pox_i++){
				#pragma HLS UNROLL
					Internal_Reg[Poy_i][Pox_i] = Internal_Reg[Poy_i][Pox_i+1];
				}
				Internal_Reg[Poy_i][POX-1] = Buff_read[Poy_i][col];
			}
		}
	}
	else{ // read from FIFO
		loop_InBuf2Pe_wrReg_Poy_3: for(int Poy_i=0;Poy_i<POY-1;Poy_i++){
		#pragma HLS UNROLL
			loop_InBuf2Pe_wrReg_Pox_3: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				FIFO_arr[Poy_i][Pox_i].read(Internal_Reg[Poy_i][Pox_i]);
			}
		}
	}

	// Outer bank of Internal Registers reads POX pixels or 1 pixel and shift
	if(fl_ld){
		loop_InBuf2Pe_wrReg_Pox_4: for(int Pox_i=0;Pox_i<POX;Pox_i++){
		#pragma HLS UNROLL
			Internal_Reg[POY-1][Pox_i] = Buff_read[bank][Pox_i];
		}
	}
	else{
		loop_InBuf2Pe_wrReg_Pox_5: for(int Pox_i=0;Pox_i<POX-1;Pox_i++){
		#pragma HLS UNROLL
			Internal_Reg[POY-1][Pox_i] = Internal_Reg[POY-1][Pox_i+1];
		}
		Internal_Reg[POY-1][POX-1] = Buff_read[bank][col];
	}

	// *** Write to PE array ***
	loop_InBuf2Pe_out_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
	#pragma HLS UNROLL
		loop_InBuf2Pe_out_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
	#pragma HLS UNROLL
			px_stream[Poy_i][Pox_i] = Internal_Reg[Poy_i][Pox_i];
		}
	}
	// *** Write to FIFO ***
	/* If outside loops
	if(wr_fifo){
		loop_InBuf2Pe_wrFIFO_Poy: for(int Poy_i=0;Poy_i<POY-1;Poy_i++){
		#pragma HLS UNROLL
			loop_InBuf2Pe_wrFIFO_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				//FIFO_arr[Poy_i][Pox_i] = Internal_Reg[Poy_i+1][Pox_i];
				FIFO_arr[Poy_i][Pox_i].write(Internal_Reg[Poy_i+1][Pox_i]);
			}
		}
	}
	*/
	loop_InBuf2Pe_wrFIFO_Poy: for(int Poy_i=0;Poy_i<POY-1;Poy_i++){
	#pragma HLS UNROLL
		loop_InBuf2Pe_wrFIFO_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
		#pragma HLS UNROLL
			if(wr_fifo){
				FIFO_arr[Poy_i][Pox_i].write(Internal_Reg[Poy_i+1][Pox_i]);
			}
		}
	}
}


void WtBuf2Pe_ctrl(
		/* Parameter */ wtbuf2pe_loop_dt wtbuf2pe_loop_limit,
		/* Address Output */ row_wtbuf_i_dt *row_wt_o){
	// I assume loop order Toy_step*Tox_step*Tof_step*Nif*NKY*NKX (right to left)
	#pragma HLS INLINE

	static row_wtbuf_i_dt row_wtlocal = 0;     // row of WtBuf that is to be written

	*row_wt_o = row_wtlocal;
	if(row_wtlocal == wtbuf2pe_loop_limit){
		row_wtlocal = 0;
	}
	else{
		row_wtlocal++;
	}

}


void WtBuf2Pe(
		/* Inputs */ const wt_data_t WtBuf[WRD_WTBUF][POF],
		row_wtbuf_i_dt row_wt,
		/* Output */ wt_data_t wt_stream[POF]){
	#pragma HLS INLINE
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=WtBuf
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=wt_stream

	loop_WtBuf2Pe: for(int Pof_i=0;Pof_i<POF;Pof_i++){
	#pragma HLS UNROLL
		wt_stream[Pof_i] = WtBuf[row_wt][Pof_i];
	}
}


void wndClc_ctrl(
		// Parameters
		Toy_step_dt Toy_step, Tox_step_dt Tox_step,
		Tof_step_dt Tof_step, wrd_1row_dt wrd_1row,
		// Address Output
		row_inbuf_i_dt *rowStepAddress
	){
	// Calculates the base address that changes for each each execution of a calculation of a window of pixels.
	// Note that for WtBuf this isn't necessary, since the loop order chosen
	// simplifies the algorithm of the row to just incrementing by 1 each execution.
	#pragma HLS INLINE off

	static Tof_step_i_dt Tof_step_i = 0;
	static Toy_step_i_dt Toy_step_i = 0;
	static Tox_step_i_dt Tox_step_i = 0;

	(*rowStepAddress) = Toy_step_i*wrd_1row + Tox_step_i;

	// Loop iteration calculation
	if(Tof_step_i == Tof_step-1){
		Tof_step_i = 0;
		if(Tox_step_i == Tox_step-1){
			Tox_step_i = 0;
			if(Toy_step_i == Toy_step-1){
				Toy_step_i = 0;
			}
			else{
				Toy_step_i++;
			}
		}
		else{
			Tox_step_i++;
		}
	}
	else{
		Tof_step_i++;
	}
}


void wndClc_Dfl(
		// Parameters
		wnd_loop_dt wndclc_loop_limit_in, wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
		row_inbuf_i_dt rowStepAddress, Nif_dt Nif_in,
		wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
		// Inputs
		const px_data_t InBuf[POY][WRD_INBUF][POX],
		const wt_data_t WtBuf[WRD_WTBUF][POF],
		// Output
		acc_data_t rslt_stream_out[POF][POY][POX]
	){
	#pragma HLS INLINE off

	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=InBuf
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=InBuf
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=WtBuf
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=rslt_stream_out
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=rslt_stream_out
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=rslt_stream_out
	acc_data_t rslt_stream[POF][POY][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=rslt_stream
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=rslt_stream
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=rslt_stream

	loop_init_Pof_MAC: for(int Pof_i=0;Pof_i<POF;Pof_i++){
	#pragma HLS UNROLL
		loop_init_Poy_MAC: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS UNROLL
			loop_init_Pox_MAC: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				rslt_stream[Pof_i][Poy_i][Pox_i] = 0;
			}
		}
	}

	row_inbuf_i_dt row_px;
	wnd_loop_dt wndclc_loop_limit;
	wtbuf2pe_loop_dt wtbuf2pe_loop_limit;
	Nif_dt Nif;
	wrd_1row_dt wrd_1row;
	row_1map_dt row_1map;

	row_px = rowStepAddress;
	wndclc_loop_limit = wndclc_loop_limit_in;
	wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_in;
	Nif = Nif_in;
	wrd_1row = wrd_1row_in;
	row_1map = row_1map_in;

	Region1: for(int counter=0;counter<wndclc_loop_limit;counter++){
	#pragma HLS PIPELINE
	#pragma HLS loop_tripcount min=WNDCLC_TRIPCOUNT max=WNDCLC_TRIPCOUNT
		Pox_i_dt col;                  		// InBuf2Pe_ctrl output variables (2)
		Poy_i_dt bank;                 		// InBuf2Pe_ctrl output variables (3)
		data_bool dct_ld, fl_ld, wr_fifo; 	// InBuf2Pe_ctrl output variables (4)
		row_wtbuf_i_dt row_wt;  			// WtBuf2Pe_ctrl output variable

		px_data_t px_stream[POY][POX]; 	// Channel from InBuf2Pe to PE
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_stream
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_stream
		wt_data_t wt_stream[POF]; 		// Channel from WtBuf2Pe to PE
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=wt_stream

		InBuf2Pe_ctrl(/*Inputs*/ Nif, wrd_1row, row_1map,
					  /*Outputs*/ &row_px, &col, &bank, &dct_ld, &fl_ld, &wr_fifo);
		#ifdef DEBUG_MODE
			if( row_px<0 || row_px >= WRD_INBUF || col<0 || col>=POX || bank<0 || bank>=POY){
				std::cout << "Error Here! 31542\n";
			}
		#endif
		WtBuf2Pe_ctrl(wtbuf2pe_loop_limit, &row_wt);
		InBuf2Pe_wrapper(/*Address Inputs*/ row_px, col, bank,
						 /*Control Flow Inputs*/ dct_ld, fl_ld, wr_fifo,
						 InBuf, px_stream);
		#ifdef DEBUG_MODE
			if( row_wt<0 || row_wt >= WRD_WTBUF){
				std::cout << "Error Here! 198\n";
			}
		#endif
		WtBuf2Pe(WtBuf, row_wt, wt_stream);
		Pe(px_stream, wt_stream, rslt_stream);
	}
	loop_read_Pof_MAC: for(int Pof_i=0;Pof_i<POF;Pof_i++){
	#pragma HLS UNROLL
		loop_read_Poy_MAC: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS UNROLL
			loop_read_Pox_MAC: for(int Pox_i=0;Pox_i<POX;Pox_i++){
			#pragma HLS UNROLL
				rslt_stream_out[Pof_i][Poy_i][Pox_i] = rslt_stream[Pof_i][Poy_i][Pox_i];
			}
		}
	}
}


/* Unused
	void wndClc(
			// Parameters 
			wnd_loop_dt wndclc_loop_limit_in, wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
			row_inbuf_i_dt rowStepAddress, Nif_dt Nif_in,
			wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
			// Inputs
			px_data_t InBuf[POY][WRD_INBUF][POX],
			wt_data_t WtBuf[WRD_WTBUF][POF],
			// Output
			acc_data_t rslt_stream_out[POF][POY][POX]){
		//#pragma HLS BIND_STORAGE variable=WtBuf type=ram_1p impl=bram
		//#pragma HLS BIND_STORAGE variable=InBuf type=ram_1p impl=bram
		// For tb usage for now
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=InBuf
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=InBuf
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=WtBuf
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=rslt_stream_out
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=rslt_stream_out
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=rslt_stream_out

		static row_inbuf_i_dt row_px;
		static wnd_loop_dt wndclc_loop_limit;
		static wtbuf2pe_loop_dt wtbuf2pe_loop_limit;
		static Nif_dt Nif;
		static wrd_1row_dt wrd_1row;
		static row_1map_dt row_1map;

		row_px = rowStepAddress;
		wndclc_loop_limit = wndclc_loop_limit_in;
		wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_in;
		Nif = Nif_in;
		wrd_1row = wrd_1row_in;
		row_1map = row_1map_in;

		wndClc_Dfl(
				// Parameters
				wndclc_loop_limit, wtbuf2pe_loop_limit,
				row_px, Nif, wrd_1row, row_1map,
				// Inputs
				InBuf, WtBuf,
				// Output
				rslt_stream_out);
	}
*/


void Pe2Buf(
		// Parameters
		Toy_dt Toy, Toy_step_dt Toy_step,
		Tox_step_dt Tox_step, Tof_step_dt Tof_step,
		row_outbuf_i_dt in1, row_outbuf_i_dt in2,
		row_outbuf_i_dt in3,
		bit_shift_dt bit_shift,
		// Inputs
		acc_data_t px_stream[POF][POY][POX], b_data_t BiasBuf[BIASBUF_LENGTH],
		// Output
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
	){
	#pragma HLS INLINE off
	// Partition I/O
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_stream
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_stream
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_stream
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=OutBuf
	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=OutBuf
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=BiasBuf
	static Toy_step_i_dt Toy_step_i = 0; // To track iteration (1)
	static Tox_step_i_dt Tox_step_i = 0; // To track iteration (2)
	static Tof_step_i_dt Tof_step_i = 0; // To track iteration (3)
	static row_outbuf_i_dt rowStep = 0;
	row_outbuf_i_dt row;
	acc_data_t pxSerial[OUTBUF_NUM][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pxSerial
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=pxSerial
	px_data_t ReLuOut[OUTBUF_NUM][POX];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=ReLuOut
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=ReLuOut

	// Comments below show reordering/transormation operations (previous versions)
	//(*rowStep) = Tox_step_i + Toy_step_i*POY*Tox_step + ((Tof_step_i*POF)/OUTBUF_NUM)*Toy*Tox_step;
	/* Version 1
		// Tox_step_i
		if( Toy_step_i != 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
			(*rowStep) -= (Tox_step-1);
		}
		else if(Tox_step_i != 0 && Tof_step_i == 0){
			(*rowStep)++;
		}
		// Toy_step_i*POY*Tox_step
		if( Toy_step_i != 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
			(*rowStep) += POY*Tox_step;
		}
		else if( Toy_step_i == 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
			(*rowStep) = 0;
		}
		// ((Tof_step_i*POF)/OUTBUF_NUM)*Toy*Tox_step
		if( not(Toy_step_i == 0  && Tox_step_i == 0 && Tof_step_i == 0) && Tof_step_i ==0){
			(*rowStep) -= (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step;
		}
		else if(Tof_step_i != 0){
			(*rowStep) += (POF/OUTBUF_NUM)*Toy*Tox_step;
		}	
	*/

	/* Version 2
		if( Toy_step_i != 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
			(*rowStep) += POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step;
		}
		else if(Tox_step_i != 0 && Tof_step_i == 0){
			(*rowStep) -= (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step - 1;
		}
		else if( Toy_step_i == 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
			(*rowStep) = 0;
		}
		else if( Tof_step_i == 0){
			(*rowStep) -= (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step;
		}
		else if(Tof_step_i != 0){
			(*rowStep) += (POF/OUTBUF_NUM)*Toy*Tox_step;
		}	
	*/


	if( Toy_step_i != 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
		rowStep += in1;
	}
	else if(Tox_step_i != 0 && Tof_step_i == 0){
		rowStep -= in2 - 1;
	}
	else if( Toy_step_i == 0  && Tox_step_i == 0 && Tof_step_i == 0 ){
		rowStep = 0;
	}
	else if( Tof_step_i == 0){
		rowStep -= in2;
	}
	else if(Tof_step_i != 0){
		rowStep += in3;
	}

	row = rowStep;
	loop_Pe2Buf_PofBankStep: for(int POFBANK_STEP_i=0;POFBANK_STEP_i<POFBANK_STEP;POFBANK_STEP_i++){
		loop_Pe2Buf_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
		#pragma HLS PIPELINE
			// Load #OutBuf*Pox pixels in parallel
			loop_Pe2Buf_OutBufNum_Load: for(int OUTBUF_NUM_i=0;OUTBUF_NUM_i<OUTBUF_NUM;OUTBUF_NUM_i++){
			#pragma HLS UNROLL
				loop_Pe2Buf_Pox_Load: for(int Pox_i=0;Pox_i<POX;Pox_i++){
				#pragma HLS UNROLL
					pxSerial[OUTBUF_NUM_i][Pox_i] =
							px_stream[OUTBUF_NUM_i + POFBANK_STEP_i*OUTBUF_NUM][Poy_i][Pox_i];
				}
			}

			// Apply ReLu in parallel
			bias_ReLu(Tof_step_i, bit_shift, BiasBuf, pxSerial, ReLuOut);

			// Load them in parallel to OutBuf
			loop_Pe2Buf_OutBufNum_Store: for(int OUTBUF_NUM_i=0;OUTBUF_NUM_i<OUTBUF_NUM;OUTBUF_NUM_i++){
			#pragma HLS UNROLL
				loop_Pe2Buf_Pox_Store: for(int Pox_i=0;Pox_i<POX;Pox_i++){
				#pragma HLS UNROLL
					OutBuf[OUTBUF_NUM_i][row /*+ POFBANK_STEP_i*Toy*Tox_step*/ + Poy_i*Tox_step][Pox_i] = ReLuOut[OUTBUF_NUM_i][Pox_i];
				}
			}
		}
		row += Toy*Tox_step; // Add rows to move to next set of Maps
		// instead of adding POFBANK_STEP_i*Toy*Tox_step in rows of OutBuf1
	}
	// Step Calculation (Loop order (inside to outside) is 1. Tof -> 2. Tox -> 3. Toy)
	if(Tof_step_i == Tof_step-1){
		Tof_step_i = 0;
		if(Tox_step_i == Tox_step-1){
			Tox_step_i = 0;
			if(Toy_step_i == Toy_step-1){
				Toy_step_i = 0;
			}
			else{
				Toy_step_i++;
			}
		}
		else{
			Tox_step_i++;
		}
	}
	else{
		Tof_step_i++;
	}
}


// Dev for macros (all versions of region 2)
void tileClc_Dfl(
		// Parameters
		tile_loop_dt tileclc_loop_limit_in, wnd_loop_dt wndclc_loop_limit_in,
		wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
		Nif_dt Nif_in, Toy_dt Toy_in,
		Toy_step_dt Toy_step_in, Tox_step_dt Tox_step_in, Tof_step_dt Tof_step_in,
		wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
		row_outbuf_i_dt pe2buf_addr_offset1_in, row_outbuf_i_dt pe2buf_addr_offset2_in,
		row_outbuf_i_dt pe2buf_addr_offset3_in,
		bit_shift_dt bit_shift_in,
		// Inputs
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		// Output
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
	){
	#ifndef __SYNTHESIS__
		std::cout << "For this tile, bit shift is " << bit_shift_in << std::endl;
	#endif

	#if defined(REGION2_SEQ) || defined(REGION2_DFL) || defined(REGION2_PPL)
		#pragma HLS INLINE
		Region2: for(int counter=0;counter<tileclc_loop_limit_in;counter++){
		#ifdef REGION2_SEQ
			#pragma HLS PIPELINE off
		#endif
		#ifdef REGION2_DFL
			#pragma HLS DATAFLOW
		#endif
		#ifdef REGION2_PPL
			#pragma HLS PIPELINE
		#endif
		#pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			acc_data_t px_toBuf[POF][POY][POX]; 		// Channel for calculated output pixels between wndClc and Pe2Buf
			#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
			#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
			#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
			row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf, BiasBuf, OutBuf);
		}   
	#endif // defined(REGION2_SEQ) || defined(REGION2_PPL)

	#ifdef REGION2_MNLSCHEDULE_2BUF
		#pragma HLS INLINE off
		acc_data_t px_toBuf[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
		acc_data_t px_toBuf2[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf	
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf2
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf2
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf2
		row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
		wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
		wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
				Nif_in, wrd_1row_in, row_1map_in,
				InBuf, WtBuf, px_toBuf);
		Region2: for(int counter=0;counter<tileclc_loop_limit_in;counter++){
		#pragma HLS PIPELINE off
		#pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf, BiasBuf, OutBuf);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf2);

			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf2, BiasBuf, OutBuf);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf);
		}
		Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
				pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
				pe2buf_addr_offset3_in,
				bit_shift_in,
				px_toBuf, BiasBuf, OutBuf);
	#endif // REGION2_MNLSCHEDULE_2BUF
}


/* Non dataflow verions
	void tileClc_Dfl(
			// Parameters
			tile_loop_dt tileclc_loop_limit_in, wnd_loop_dt wndclc_loop_limit_in,
			wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
			Nif_dt Nif_in, Toy_dt Toy_in,
			Toy_step_dt Toy_step_in, Tox_step_dt Tox_step_in, Tof_step_dt Tof_step_in,
			wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
			row_outbuf_i_dt pe2buf_addr_offset1_in, row_outbuf_i_dt pe2buf_addr_offset2_in,
			row_outbuf_i_dt pe2buf_addr_offset3_in,
			bit_shift_dt bit_shift_in,
			// Inputs
			px_data_t InBuf[POY][WRD_INBUF][POX],
			wt_data_t WtBuf[WRD_WTBUF][POF],
			b_data_t BiasBuf[BIASBUF_LENGTH],
			// Output
			px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
		){
		#pragma HLS INLINE off
		// tile_loop_dt tileclc_loop_limit = tileclc_loop_limit_in;
		// wnd_loop_dt wndclc_loop_limit = wndclc_loop_limit_in;
		// wtbuf2pe_loop_dt wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_in;
		// Nif_dt Nif = Nif_in;
		// Toy_dt Toy = Toy_in;
		// Toy_step_dt Toy_step = Toy_step_in;
		// Tox_step_dt Tox_step = Tox_step_in;
		// Tof_step_dt Tof_step = Tof_step_in;
		// wrd_1row_dt wrd_1row = wrd_1row_in;
		// row_1map_dt row_1map = row_1map_in;
		// row_outbuf_i_dt pe2buf_addr_offset1 = pe2buf_addr_offset1_in;
		// row_outbuf_i_dt pe2buf_addr_offset2 = pe2buf_addr_offset2_in;
		// row_outbuf_i_dt pe2buf_addr_offset3 = pe2buf_addr_offset3_in;
		// bit_shift_dt bit_shift = bit_shift_in;
		#ifndef __SYNTHESIS__
			std::cout << "For this tile, bit shift is " << bit_shift << std::endl;
		#endif

		// Region2: for(int counter=0;counter<tileclc_loop_limit;counter++){
			// #pragma HLS PIPELINE off
			// #pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			// 	acc_data_t px_toBuf[POF][POY][POX]; 		// Channel for calculated output pixels between wndClc and Pe2Buf
			// 	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
			// 	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
			// 	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
			// 	row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
			// 	wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// 	wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 			Nif, wrd_1row, row_1map,
			// 			InBuf, WtBuf, px_toBuf);
			// 	Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 			pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 			pe2buf_addr_offset3,
			// 			bit_shift,
			// 			px_toBuf, BiasBuf, OutBuf);
			// }
		
		// // Decouple Tile: Delayed Pipeline + Double Buffering
			// acc_data_t px_toBuf[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf
			// #pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
			// #pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
			// #pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
			// acc_data_t px_toBuf2[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf	
			// #pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf2
			// #pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf2
			// #pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf2
			// row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
			// wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 		Nif, wrd_1row, row_1map,
			// 		InBuf, WtBuf, px_toBuf);
			// Region2: for(int counter=0;counter<tileclc_loop_limit;counter++){
			// #pragma HLS PIPELINE off
			// #pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			// 	wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// 	Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 			pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 			pe2buf_addr_offset3,
			// 			bit_shift,
			// 			px_toBuf, BiasBuf, OutBuf);
			// 	wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 			Nif, wrd_1row, row_1map,
			// 			InBuf, WtBuf, px_toBuf2);

			// 	wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// 	Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 			pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 			pe2buf_addr_offset3,
			// 			bit_shift,
			// 			px_toBuf2, BiasBuf, OutBuf);
			// 	wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 			Nif, wrd_1row, row_1map,
			// 			InBuf, WtBuf, px_toBuf);
			// }
			// Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 		pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 		pe2buf_addr_offset3,
			// 		bit_shift,
			// 		px_toBuf, BiasBuf, OutBuf);
					
		// Decouple Tile (2): Double Buffering
			// Region2: for(int counter=0;counter<tileclc_loop_limit;counter++){
			// #pragma HLS PIPELINE off
			// #pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			// 	acc_data_t px_toBuf[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf
			// 	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
			// 	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
			// 	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
			// 	acc_data_t px_toBuf2[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf	
			// 	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf2
			// 	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf2
			// 	#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf2
			// 	row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
			// 	wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// 	wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 			Nif, wrd_1row, row_1map,
			// 			InBuf, WtBuf, px_toBuf);
			// 	Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 			pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 			pe2buf_addr_offset3,
			// 			bit_shift,
			// 			px_toBuf, BiasBuf, OutBuf);
			// 	wndClc_ctrl(Toy_step, Tox_step, Tof_step, wrd_1row, &rowStepAddress);
			// 	wndClc_Dfl(wndclc_loop_limit, wtbuf2pe_loop_limit, rowStepAddress,
			// 			Nif, wrd_1row, row_1map,
			// 			InBuf, WtBuf, px_toBuf2);
			// 	Pe2Buf(Toy, Toy_step, Tox_step, Tof_step,
			// 			pe2buf_addr_offset1, pe2buf_addr_offset2,
			// 			pe2buf_addr_offset3,
			// 			bit_shift,
			// 			px_toBuf2, BiasBuf, OutBuf);
			// }

		// Decouple Tile (3): Read Inputs Directly + Delayed Pipeline + Double Buffering
		acc_data_t px_toBuf[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
		acc_data_t px_toBuf2[POF][POY][POX]; // Channel for calculated output pixels between wndClc and Pe2Buf	
		#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf2
		#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf2
		#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf2
		row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc
		wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
		wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
				Nif_in, wrd_1row_in, row_1map_in,
				InBuf, WtBuf, px_toBuf);
		Region2: for(int counter=0;counter<tileclc_loop_limit_in;counter++){
		#pragma HLS PIPELINE off
		#pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf, BiasBuf, OutBuf);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf2);

			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf2, BiasBuf, OutBuf);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf);
		}
		Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
				pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
				pe2buf_addr_offset3_in,
				bit_shift_in,
				px_toBuf, BiasBuf, OutBuf);
	}
*/


/* Dataflow version:
	void tileClc_Dfl(
			// Parameters
			tile_loop_dt tileclc_loop_limit_in, wnd_loop_dt wndclc_loop_limit_in,
			wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
			Nif_dt Nif_in, Toy_dt Toy_in,
			Toy_step_dt Toy_step_in, Tox_step_dt Tox_step_in, Tof_step_dt Tof_step_in,
			wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
			row_outbuf_i_dt pe2buf_addr_offset1_in, row_outbuf_i_dt pe2buf_addr_offset2_in,
			row_outbuf_i_dt pe2buf_addr_offset3_in,
			bit_shift_dt bit_shift_in,
			// Inputs
			px_data_t InBuf[POY][WRD_INBUF][POX],
			wt_data_t WtBuf[WRD_WTBUF][POF],
			b_data_t BiasBuf[BIASBUF_LENGTH],
			// Output
			px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
		){
		#pragma HLS INLINE off
		// tile_loop_dt tileclc_loop_limit = tileclc_loop_limit_in;
		// wnd_loop_dt wndclc_loop_limit = wndclc_loop_limit_in;
		// wtbuf2pe_loop_dt wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_in;
		// Nif_dt Nif = Nif_in;
		// Toy_dt Toy = Toy_in;
		// Toy_step_dt Toy_step = Toy_step_in;
		// Tox_step_dt Tox_step = Tox_step_in;
		// Tof_step_dt Tof_step = Tof_step_in;
		// // Toy_step_dt Toy_step = Toy_step_in;
		// // Tox_step_dt Tox_step = Tox_step_in;
		// // Tof_step_dt Tof_step = Tof_step_in;
		// // Toy_step_dt Toy_step2 = Toy_step_in;
		// // Tox_step_dt Tox_step2 = Tox_step_in;
		// // Tof_step_dt Tof_step2 = Tof_step_in;

		// wrd_1row_dt wrd_1row = wrd_1row_in;
		// row_1map_dt row_1map = row_1map_in;
		// row_outbuf_i_dt pe2buf_addr_offset1 = pe2buf_addr_offset1_in;
		// row_outbuf_i_dt pe2buf_addr_offset2 = pe2buf_addr_offset2_in;
		// row_outbuf_i_dt pe2buf_addr_offset3 = pe2buf_addr_offset3_in;
		// bit_shift_dt bit_shift = bit_shift_in;
		#ifndef __SYNTHESIS__
			std::cout << "For this tile, bit shift is " << bit_shift << std::endl;
		#endif

		Region2: for(int counter=0;counter<tileclc_loop_limit_in;counter++){
		#pragma HLS DATAFLOW
		#pragma HLS loop_tripcount min=TILECLC_TRIPCOUNT max=TILECLC_TRIPCOUNT
			acc_data_t px_toBuf[POF][POY][POX]; 		// Channel for calculated output pixels between wndClc and Pe2Buf
			#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=px_toBuf
			#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=px_toBuf
			#pragma HLS ARRAY_PARTITION dim=3 type=complete variable=px_toBuf
			row_inbuf_i_dt rowStepAddress; 				// Base address for row of InBuf for each execution of wndClc

			wndClc_ctrl(Toy_step_in, Tox_step_in, Tof_step_in, wrd_1row_in, &rowStepAddress);
			wndClc_Dfl(wndclc_loop_limit_in, wtbuf2pe_loop_limit_in, rowStepAddress,
					Nif_in, wrd_1row_in, row_1map_in,
					InBuf, WtBuf, px_toBuf);
			Pe2Buf(Toy_in, Toy_step_in, Tox_step_in, Tof_step_in,
					pe2buf_addr_offset1_in, pe2buf_addr_offset2_in,
					pe2buf_addr_offset3_in,
					bit_shift_in,
					px_toBuf, BiasBuf, OutBuf);
		}

	}
*/

// ******  Sequential Case Region 3  *****
#if not defined(INTERNAL_CONFIG_LAYER)
void loadIfMap(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		data_bool northTile_in, data_bool southTile_in,
		Niy_dt yBase_in, const px_data_t_port *IfMap, //[NIF][NIX-2*ZERO_PAD][NIY-2*ZERO_PAD]
		// Output
		px_data_t InBuf[POY][WRD_INBUF][POX]
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0; 	// "State" for layer
	static Nif_dt Nif;           	// Parameters stored locally (1)
	static Niy_dt Niy;           	// Parameters stored locally (2)
	static Tiy_dt Tiy;           	// Parameters stored locally (3)
	static Tix_dt Tix;           	// Parameters stored locally (4)
	static row_1map_dt row_1map; 	// Parameters stored locally (5)
	static wrd_1row_dt wrd_1row; 	// Parameters stored locally (6)
	static Poy_i_dt TiyRem;      	// Parameters stored locally (7)
	static Pox_i_dt TixRem;      	// Parameters stored locally (8)

	data_bool northTile, southTile;
	Niy_dt yBase;
	Tiy_dt yTile;				 	// Number of pixels for y dimension to be loaded
	Tix_dt xTile; 				 	// Number of pixels for x dimension to be loaded
	row_inbuf_i_dt wrd_i; 			// Variables for addressing of InBuf (1)
	row_inbuf_i_dt wrdMap; 			// Variables for addressing of InBuf (2)
	row_inbuf_i_dt wrdY, wrdX; 		// Variables for addressing of InBuf (3)
	Poy_i_dt Poy_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		Nif = Nif_rom[layerNo];
		Niy = niy_rom[layerNo];
		Tiy = tiy_rom[layerNo]; // Contains zero padding
		Tix = tix_rom[layerNo]; // Contains zero padding
		row_1map = row_1map_rom[layerNo];
		wrd_1row = wrd_1row_rom[layerNo];
		TiyRem = (Tiy-1)%POY;
		TixRem = (Tix-1)%POX; 	// find position of last element of x row of IfMap -> mapped to InBuf
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		northTile = northTile_in; southTile = southTile_in;
		yBase = yBase_in;
		yTile = Tiy - northTile - southTile; xTile = Tix - 2;
		wrdMap = 0;
		If_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
		#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
			Poy_i = northTile; wrdY = 0;
			If_Tiy: for(int yTile_i=0;yTile_i<yTile;yTile_i++){
			#pragma HLS LOOP_TRIPCOUNT min=TIY_TRIPCOUNT max=TIY_TRIPCOUNT
				#if defined(FMAP_WIDEN)
					wrdX = 0;
					// std::cout << "calling if_nix" << Poy_i << std::endl;
					If_Nix: for(int xTile_i=0;xTile_i<xTile/7;xTile_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						wrd_i = wrdMap + wrdY + wrdX;
						InBuf[Poy_i][wrd_i  ][1] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS-1,0);
						InBuf[Poy_i][wrd_i  ][2] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*2-1,SYNTH_BITS);
						InBuf[Poy_i][wrd_i  ][3] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*3-1,SYNTH_BITS*2);
						InBuf[Poy_i][wrd_i  ][4] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*4-1,SYNTH_BITS*3);
						InBuf[Poy_i][wrd_i  ][5] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*5-1,SYNTH_BITS*4);
						InBuf[Poy_i][wrd_i  ][6] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*6-1,SYNTH_BITS*5);
						InBuf[Poy_i][wrd_i+1][0] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*7-1,SYNTH_BITS*6);
						// std::cout << "Packed hex value:";
						// std::cout << IfMap[Nif_i*(Niy)*(Tix-2) + (yBase+yTile_i)*(Tix-2) + xTile_i].to_string(16) << std::endl; // prints in hexadecimal << std::endl;
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][1];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][2];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][3];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][4];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][5];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i][6];
						// std::cout << std::setw(8) << InBuf[Poy_i][wrd_i+1][0] << std::endl;
						
						// if(Pox_i == POX-1){
						// 	wrdX++;
						// 	Pox_i = 0;
						// }
						// else{
						// 	Pox_i++;
						// }
						// std::cout << "Calling wrd_i: " << wrd_i;
						wrdX++;
					}
					// std::cout << "\n";
					if(Poy_i == POY-1){
						Poy_i = 0;
						wrdY += wrd_1row; // ceil(Tix,Pox)
					}
					else{
						Poy_i++;
					}
					// std::this_thread::sleep_for(std::chrono::seconds(5));
				#else
					Pox_i = 1; wrdX = 0;
					// std::cout << "calling if_nix" << Poy_i << std::endl;
					If_Nix: for(int xTile_i=0;xTile_i<xTile;xTile_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						wrd_i = wrdMap + wrdY + wrdX;
						InBuf[Poy_i][wrd_i][Pox_i] = *(IfMap + Nif_i*(Niy)*(Tix-2) + (yBase+yTile_i)*(Tix-2) + xTile_i);
						// std::cout << "Calling wrd_i: " << wrd_i;
						if(Pox_i == POX-1){
							wrdX++;
							Pox_i = 0;
						}
						else{
							Pox_i++;
						}
					}
					// std::cout << "\n";
					if(Poy_i == POY-1){
						Poy_i = 0;
						wrdY += wrd_1row; // ceil(Tix,Pox)
					}
					else{
						Poy_i++;
					}
				#endif
				// std::this_thread::sleep_for(std::chrono::seconds(5));
			}
			wrdMap += row_1map * wrd_1row; // ceil(Tiy,Poy)*ceil(Tix,POY)
		}

		// North Zero Padding
		if(northTile){
			NorthPad_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				NorthPad_wrd: for(int wrd_1row_i=0;wrd_1row_i<wrd_1row;wrd_1row_i++){
				#pragma HLS LOOP_TRIPCOUNT min=WRD1ROW_TRIPCOUNT max=WRD1ROW_TRIPCOUNT
					NorthPad_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
					#pragma HLS UNROLL
						InBuf[0][Nif_i*row_1map*wrd_1row + wrd_1row_i][Pox_i] = 0;
					}
				}
			}
		}
		// South Zero Padding
		if(southTile){
			SouthPad_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				SouthPad_wrd: for(int wrd_1row_i=0;wrd_1row_i<wrd_1row;wrd_1row_i++){
				#pragma HLS LOOP_TRIPCOUNT min=WRD1ROW_TRIPCOUNT max=WRD1ROW_TRIPCOUNT
					SouthPad_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
					#pragma HLS UNROLL
						InBuf[TiyRem][Nif_i*row_1map*wrd_1row + (row_1map-1)*wrd_1row+ wrd_1row_i][Pox_i] = 0;
					}
				}
			}
		}
		// West Zero Padding
		WestPad_Line: for(int nxtLine_i=0;nxtLine_i<Nif*row_1map;nxtLine_i++){
		#pragma HLS LOOP_TRIPCOUNT min=ROW1MAPNIF_TRIPCOUNT max=ROW1MAPNIF_TRIPCOUNT
			WestPad_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
			#pragma HLS UNROLL
				InBuf[Poy_i][nxtLine_i*wrd_1row][0] = 0;
			}
		}
		// East Zero Padding
		EastPad_Line: for(int nxtLine_i=0;nxtLine_i<Nif*row_1map;nxtLine_i++){
		#pragma HLS LOOP_TRIPCOUNT min=ROW1MAPNIF_TRIPCOUNT max=ROW1MAPNIF_TRIPCOUNT
			EastPad_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
			#pragma HLS UNROLL
				InBuf[Poy_i][nxtLine_i*wrd_1row + wrd_1row-1][TixRem] = 0;
			}
		}
	}
}


#if not defined(WTMAP_WIDEN)
void loadWtMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ Nofy_step_dt ofBase, const wt_data_t_port *WtMap,
		/* Output */ wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static Nif_dt Nif; 				// Parameters stored locally (1)
	static Tof_dt Tof; 				// Parameters stored locally (2)
	static layerNo_dt layerNo = 0;  // "State" for layer
	Pof_i_dt Pof_i;
	row_wtbuf_i_dt wrdMap;

	if(layerCnfg){
		Nif = Nif_rom[layerNo];
		Tof = Tof_rom[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		Pof_i = 0; wrdMap = 0;
		WtLoop_Tof: for(int Tof_i=0;Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			WtLoop_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
			
				WtLoop_Nky: for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				#pragma HLS LOOP_TRIPCOUNT min=NKY max=NKY
					WtLoop_Nkx: for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
					#pragma HLS LOOP_TRIPCOUNT min=NKX max=NKX
						WtBuf[wrdMap + Nif_i*NKY*NKX + Nky_i*NKX + Nkx_i][Pof_i] = *(WtMap + (Tof_i+ofBase*Tof)*Nif*NKY*NKX + Nif_i*NKY*NKX + Nky_i*NKX + Nkx_i);
					}
				}
			}
			if(Pof_i == POF-1){
				Pof_i = 0;
				wrdMap += Nif*NKY*NKX;
			}
			else{
				Pof_i++;
			}
		}
	}

}
#endif

#if defined(WTMAP_WIDEN)
void loadWtMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ Nofy_step_dt ofBase, const wt_data_t_port *WtMap,
		/* Output */ wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static Nif_dt Nif; 				// Parameters stored locally (1)
	static Tof_step_dt Tof_step; 				// Parameters stored locally (2)
	static layerNo_dt layerNo = 0;  // "State" for layer
	// Pof_i_dt Pof_i;
	// row_wtbuf_i_dt wrdMap;

	if(layerCnfg){
		Nif = Nif_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		WtLoop_Tof: for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_STEP_TRIPCOUNT max=TOF_STEP_TRIPCOUNT
			WtLoop_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				WtLoop_Nky: for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				#pragma HLS LOOP_TRIPCOUNT min=NKY max=NKY
					WtLoop_Nkx: for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
					#pragma HLS LOOP_TRIPCOUNT min=NKX max=NKX
						WtLoop_Pof: for (int i = 0; i < (POF/WTMAP_WIDTHFACTOR); i++) {
						#pragma HLS UNROLL
							for (int chunk = 0; chunk < WTMAP_WIDTHFACTOR; chunk++) {
								int idx = i*WTMAP_WIDTHFACTOR + chunk;
								int bit_start = chunk * SYNTH_BITS;
								int bit_end   = bit_start + SYNTH_BITS - 1;

								WtBuf[Tof_step_i*Nif*NKY*NKX 
									+ Nif_i*NKY*NKX 
									+ Nky_i*NKX 
									+ Nkx_i][idx] =
									WtMap[(Tof_step_i + ofBase*Tof_step) * Nif*NKY*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nif_i*NKY*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nky_i*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nkx_i*(POF/WTMAP_WIDTHFACTOR)
										+ i].range(bit_end, bit_start);
							}
						}
					}
				}
			}
		}
	}
}
#endif


void mem2Buf(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Outputs
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static data_bool NofFirst;
	static Noy_step_dt Noy_step; 		// convLayer module loop tripcount
	static Tiy_dt Tiy;					// Tile dimension variable
	static layerNo_dt layerNo = 0; 		// "State" for layer
	static Nofy_step_dt tileCount;
	data_bool northTile, southTile; 	// Flags for tile position (for zero padding)
	Niy_dt yBase;  // (Niy offset) 		// y offset address for tile of IfMap to be loaded
	Nofy_step_dt ofBase; 				// map offset address for tile of OfMap to be stored

	if(layerCnfg){
		NofFirst = nofFirst[layerNo];
		Noy_step = noy_step_rom[layerNo];
		Tiy = tiy_rom[layerNo];
		tileCount = 0;
		loadIfMap(/*layerCnfg=*/1, /*normal operation*/ 0, 0, 0, IfMap, InBuf);
		loadWtMap(/*layerCnfg=*/1, /*normal operation*/ 0, WtMap, WtBuf);
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(NofFirst == 1){
			ofBase = 0;
			yBase = tileCount*(Tiy - 2) -1;
			if(tileCount == 0){
				northTile = 1;
			}
			else{
				northTile = 0;
			}
			if(tileCount == Noy_step - 1){
				southTile = 1;
			}
			else{
				southTile = 0;
			}
		}
		else{
			ofBase = tileCount; // loadWtMap, storeMap Input
			yBase = 0;
			northTile = 1;
			southTile = 1;
		}

		if( tileCount == 0 ){
			loadIfMap(/*layerCnfg=*/0, /*normal operation*/ northTile, southTile, 0, IfMap, InBuf);
			loadWtMap(/*layerCnfg=*/0, /*normal operation*/ ofBase, WtMap, WtBuf);
		}
		else if( NofFirst == 0 ){
			loadWtMap(/*layerCnfg=*/0, /*normal operation*/ ofBase, WtMap, WtBuf);
		}
		else{
			loadIfMap(/*layerCnfg=*/0, /*normal operation*/ northTile, southTile, yBase, IfMap, InBuf);
		}
		tileCount++;
	}
}


void loadBiasTile(
		data_bool layerCnfg,
		b_data_t BiasBuf[BIASBUF_LENGTH]
	){
#pragma HLS PIPELINE off
	#pragma HLS INLINE off
	// Based on Nof_step, Noy_step and layerNo, we load
	// the correct Tof biases to the corresponding buffer
	// bias[] is stored locally in this module, has the biases for all layers of the CNN.

	static row_biasbuf_i_dt  addressBase = 0; 	// Address base for each layer
	static Nof_step_dt   Nof_step;        		// Locally store variables (1)
	static Tof_dt        Tof;       			// Locally store variables (2)
	static Tof_step_dt   Tof_step;      		// Locally store variables (3)
	static Nof_step_i_dt Nof_step_i = 0;  		// Locally stored counter
	static layerNo_dt layerNo = 0;          	// Layer "state"
	static Nofy_step_dt Nofy_step;				// convLayer module loop tripcount

	if(layerCnfg){
		if(layerNo != 0){
			addressBase += Nof_step*Tof_step*POF; // this needs to be fixed!!!
		}
		else{
			addressBase = 0;
		}
		Nofy_step = nofy_step_rom[layerNo];
		Nof_step = Nof_step_rom[layerNo];
		// Nofy_step = nofy_step_rom[layerNo];
		Tof = Tof_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		Nof_step_i = 0;
		// tileCount = 0;
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		BiasLoop_Tof: for(int Tof_i=0;Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			BiasBuf[Tof_i] = bias[addressBase + Nof_step_i*Tof + Tof_i];
		}
		if(Nof_step_i == Nof_step-1){
			Nof_step_i = 0;
		}
		else{
			Nof_step_i++;
		}
	}

}


void tileClc(
		// Parameters
		data_bool layerCnfg,
		// Inputs
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		// Output
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
	){
	static layerNo_dt layerNo = 0; 					// Layer "state"
	static tile_loop_dt tileclc_loop_limit; 		// tileClc Pe iteration bound
	static wnd_loop_dt wndclc_loop_limit; 			// Pe iteration bound
	static wtbuf2pe_loop_dt wtbuf2pe_loop_limit;	// WtBuf2Pe Iteration bound
	static wnd_loop_dt pe_loop_limit; 				// wndClc Iteration bound
	static Nif_dt Nif;           		  			// Variable Parameter Stored Locally (1)
	static Toy_dt Toy;								// Variable Parameter Stored Locally (2)
	static Toy_step_dt Toy_step; 					// Variable Parameter Stored Locally (3)
	static Tox_step_dt Tox_step; 					// Variable Parameter Stored Locally (4)
	static Tof_step_dt Tof_step; 					// Variable Parameter Stored Locally (5)
	static wrd_1row_dt wrd_1row; 					// Variable Parameter Stored Locally (6)
	static row_1map_dt row_1map; 					// Variable Parameter Stored Locally (7)
	static row_outbuf_i_dt pe2buf_addr_offset1;		// Variable Parameter Stored Locally (8)
	static row_outbuf_i_dt pe2buf_addr_offset2;		// Variable Parameter Stored Locally (9)
	static row_outbuf_i_dt pe2buf_addr_offset3;		// Variable Parameter Stored Locally (10)
	static bit_shift_dt bit_shift;					// Variable Parameter Stored Locally (11)

	if(layerCnfg){
		tileclc_loop_limit = tileclc_loop_limit_rom[layerNo];
		wndclc_loop_limit = wndclc_loop_limit_rom[layerNo];
		wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_rom[layerNo]; // Tof_step*Nif*NKY*NKX-1
		pe_loop_limit = pe_loop_limit_rom[layerNo];
		Nif = Nif_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Toy_step = toy_step_rom[layerNo];
		Tox_step = tox_step_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		wrd_1row = wrd_1row_rom[layerNo];
		row_1map = row_1map_rom[layerNo];
		pe2buf_addr_offset1 = pe2buf_addr_offset1_rom[layerNo];
		pe2buf_addr_offset2 = pe2buf_addr_offset2_rom[layerNo];
		pe2buf_addr_offset3 = pe2buf_addr_offset3_rom[layerNo];
		bit_shift = bit_shift_rom[layerNo];

		if(layerNo==LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		tileClc_Dfl(
				/* Parameters*/ tileclc_loop_limit, wndclc_loop_limit,
				wtbuf2pe_loop_limit,
				Nif, Toy,
				Toy_step, Tox_step, Tof_step,
				wrd_1row, row_1map, pe2buf_addr_offset1,
				pe2buf_addr_offset2, pe2buf_addr_offset3,
				bit_shift,
				/* Inputs */ InBuf, WtBuf, BiasBuf,
				/* Output */ OutBuf);
	}


}


#if not defined(MAXPOOL_INTEGRATION)
void storeMap(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		// Output
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	static Noy_dt Noy;
	static Tof_dt Tof;
	static Toy_dt Toy;
	static Tox_dt Tox;
	static Tox_step_dt wrd_1rowOut;
	static int tileCount;				// Count iterations of tiles for current layer
	static data_bool NofFirst;			// Boolean value for loop order
	static layerNo_dt layerNo = 0; 		// "State" for layer

	int ofBase, yBase;

	row_outbuf_i_dt wrdMap, wrdY, wrdX;
	outbufnum_i_dt OutBufNum_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		Noy = Noy_rom[layerNo];
		Tof = Tof_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Tox = Tox_rom[layerNo];
		wrd_1rowOut = tox_step_rom[layerNo];
		tileCount = 0;
		NofFirst = nofFirst[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(NofFirst){
			ofBase = 0;
			yBase = tileCount;
		}
		else{
			ofBase = tileCount;
			yBase = 0;
		}
		wrdMap = 0; OutBufNum_i = 0;
		Loop_Tof: for(int Tof_i=0; Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			wrdY = 0;
			Loop_Toy: for(int Toy_i=0; Toy_i<Toy;Toy_i++){
			#pragma HLS LOOP_TRIPCOUNT min=TOY_TRIPCOUNT max=TOY_TRIPCOUNT
				Pox_i = 0; wrdX = 0;
				#if not defined(FMAP_WIDEN)
					Loop_Tox: for(int Tox_i=0; Tox_i<Tox;Tox_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						*(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + wrdX][Pox_i];
						#ifdef DEBUG_MODE
							if( *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) >= (1 << HW_EMUL_NUM_BITS)){
									std::cout << "Value out of range found in " << (Tof_i+ofBase*Tof) << " Map, "
										<< (Toy_i+yBase*Toy) << " Row, " << Tox_i << " Column\n";
									std::cout << "Value is " << *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) << "\n";
							}
						#endif
						if(Pox_i == POX-1){
							Pox_i = 0;
							wrdX++;
						}
						else{
							Pox_i++;
						}
					}
				#elif defined(FMAP_WIDEN)
					Loop_Tox: for(int Tox_i=0; Tox_i<Tox/7;Tox_i++){
					#pragma HLS LOOP_TRIPCOUNT min=(TOX_TRIPCOUNT/7) max=(TOX_TRIPCOUNT/7)
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS-1,0) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][0];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*2-1,SYNTH_BITS) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][1];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*3-1,SYNTH_BITS*2) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][2];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*4-1,SYNTH_BITS*3) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][3];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*5-1,SYNTH_BITS*4) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][4];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*6-1,SYNTH_BITS*5) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][5];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*7-1,SYNTH_BITS*6) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][6];
					}	
				#endif
				wrdY += wrd_1rowOut;
			}

			if(OutBufNum_i == OUTBUF_NUM-1){
				OutBufNum_i = 0;
				wrdMap += Toy*wrd_1rowOut;
			}
			else{
				OutBufNum_i++;
			}
		}
		tileCount++;
	}
}
#endif


#if defined(MAXPOOL_INTEGRATION)
void maxPoolTree(px_data_t tmp1[POX], px_data_t tmp2[POX], px_data_t tmp3[POX], 
			px_data_t tmp4[POX], px_data_t max[POX]){
	px_data_t maxTmp[POX*2];
	#pragma HLS ARRAY_PARTITION variable=maxTmp complete dim=1
	for(int i=0;i<POX;i++){
	#pragma HLS UNROLL
		maxTmp[i    ] = (tmp1[i]>tmp3[i]) ? tmp1[i] : tmp3[i];
		maxTmp[i+POX] = (tmp2[i]>tmp4[i]) ? tmp2[i] : tmp4[i];
	}
	for(int i=0;i<POX;i++){
		max[i] = (maxTmp[2*i]>maxTmp[2*i+1]) ? maxTmp[2*i] : maxTmp[2*i+1];
	}
}
#endif


#if defined(MAXPOOL_INTEGRATION)
void storeMap(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		// Output
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	static Noy_dt Noy;
	static Noy_dt Noy_map;
	static Tof_dt Tof;
	static Toy_dt Toy;
	static Toy_dt Toy_map;
	static Tox_dt Tox;
	static Tox_dt Tox_map;				// Tox variable for OfMap (was not used as it should)
	static Tox_step_dt wrd_1rowOut;
	static int tileCount;				// Count iterations of tiles for current layer
	static data_bool NofFirst;			// Boolean value for loop order
	static layerNo_dt layerNo = 0; 		// "State" for layer

	int ofBase, yBase;
	int base_addr, addr;
	#if defined(HEAD_INTEGRATION)
		acc_data_t acc;
		int addr_gap;
		const acc_data_t reciprocal = (1 << RECIPROCAL_BITS) / (7 * 7); // fixed-point reciprocal
	#endif
	#if defined(FMAP_WIDEN)
		static ap_uint<3> state;
	#endif

	px_data_t tmp1[POX], tmp2[POX], tmp3[POX], tmp4[POX], max[POX];

	row_outbuf_i_dt wrdMap, wrdY, wrdX;
	outbufnum_i_dt OutBufNum_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		Noy = Noy_rom[layerNo];
		if(layerNo==0 || layerNo==2 || layerNo==4 || layerNo==5 ||
			layerNo==7 || layerNo==8 || layerNo==10 || layerNo==11){
			Noy_map = Noy_rom[layerNo];
			Toy_map = Toy_rom[layerNo];
			Tox_map = Tox_rom[layerNo];
		}
		else{
			Noy_map = Noy_rom[layerNo]/2;
			Toy_map = Toy_rom[layerNo]/2;
			Tox_map = Tox_rom[layerNo]/2;
		}
		Tof = Tof_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Tox = Tox_rom[layerNo];
		wrd_1rowOut = tox_step_rom[layerNo];
		tileCount = 0;
		#if defined(FMAP_WIDEN)
			state = 0;
		#endif
		NofFirst = nofFirst[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(NofFirst){
			ofBase = 0;
			yBase = tileCount;
		}
		else{
			ofBase = tileCount;
			yBase = 0;
		}

		base_addr = ofBase*Tof*Noy_map*Tox_map/FMAP_WIDTHFACTOR + yBase*Toy_map*Tox_map/FMAP_WIDTHFACTOR;
		
		#if defined(HEAD_INTEGRATION)
			addr_gap = tileCount*Tof/FMAP_WIDTHFACTOR;
		#endif

		Loop_Tof_ii: for(int Tof_ii=0; Tof_ii<Tof/OUTBUF_NUM;Tof_ii++){
		#pragma HLS LOOP_TRIPCOUNT min=(TOF_TRIPCOUNT/OUTBUF_NUM) max=(TOF_TRIPCOUNT/OUTBUF_NUM)
			Loop_OutBufNum: for(int OutBufNum_i=0; OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
			#pragma HLS LOOP_TRIPCOUNT min=OUTBUF_NUM max=OUTBUF_NUM
				addr = base_addr;		
				#if defined(HEAD_INTEGRATION)
					acc = 0;
				#endif
				Loop_Toy_step: for(int Toy_i=0; Toy_i<Toy_map;Toy_i++){
				#pragma HLS LOOP_TRIPCOUNT min=TOYMAP_TRIPCOUNT max=TOYMAP_TRIPCOUNT

					Loop_Tox_step: for(int Tox_step_i=0; Tox_step_i<Tox_map/POX;Tox_step_i++){
					#pragma HLS LOOP_TRIPCOUNT min=(TOXMAP_TRIPCOUNT/POX) max=(TOXMAP_TRIPCOUNT/POX)
						#if not defined(FMAP_WIDEN)
							if(layerNo==1 || layerNo==3 || layerNo==5 || layerNo==6 ||
								layerNo==8 || layerNo==9 || layerNo==11 || layerNo==12){
								Loop_POX0: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
									OfMap[addr] =
										OutBuf[OutBufNum_i][addr2][Pox_i];
									addr++;
								}
							}
							else{
								Loop_POX: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS UNROLL
									tmp1[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp2[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
									tmp3[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp4[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
								}
								maxPoolTree(tmp1, tmp2, tmp3, tmp4, max);
								#if not defined(HEAD_INTEGRATION)
									Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
									#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
										OfMap[addr] =
											max[Pox_i];
										addr++;
									}
								#else
									if(layerNo==0){
										for(int Pox_i=0;Pox_i<POX;Pox_i++){
											acc += max[Pox_i];
										}
									}
									else{
										Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
										#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
											OfMap[addr] =
												max[Pox_i];
											addr++;
										}
									}
								#endif
							}
						#elif defined(FMAP_WIDEN)
							if(layerNo==1 || layerNo==3 || layerNo==5 || layerNo==6 ||
								layerNo==8 || layerNo==9 || layerNo==11 || layerNo==12){
								Loop_POX0: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
									OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
										OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + Toy_i*wrd_1rowOut + Tox_step_i][Pox_i];
								}
								addr++;
							}
							else{
								Loop_POX: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS UNROLL
									tmp1[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp2[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
									tmp3[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp4[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
								}
								maxPoolTree(tmp1, tmp2, tmp3, tmp4, max);
								#if not defined(HEAD_INTEGRATION)
									Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
									#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
										OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
											max[Pox_i];
									}
									addr++;
								#else
									if(layerNo==0){
										for(int Pox_i=0;Pox_i<POX;Pox_i++){
											acc += max[Pox_i];
										}
									}
									else{
										Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
										#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
											OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
												max[Pox_i];
										}
										addr++;
									}
								#endif
							}
						#endif
					}

				}
				
				base_addr += Noy_map*Tox_map/FMAP_WIDTHFACTOR;
				#if defined(HEAD_INTEGRATION)
					if(layerNo==0){
						acc *= reciprocal;
						#if defined(FMAP_WIDEN)
							OfMap[addr_gap].range(SYNTH_BITS*(state+1)-1, SYNTH_BITS*state) = (acc >> RECIPROCAL_BITS);
							if(state==6){
								addr_gap++;
								state = 0;
							}
							else{
								state++;
							}
							acc = 0;
						#else
							OfMap[addr_gap] = (acc >> RECIPROCAL_BITS);
							addr_gap++;
							acc = 0;
						#endif
					}
				#endif
			}
		}
		tileCount++;
	}
}
#endif
#endif
// ******  Sequential Case Region 3 END  *****
// ********  Overlap Case Region 3  *******
#if defined(INTERNAL_CONFIG_LAYER)
void loadIfMap(
		const px_data_t_port *IfMap, //[NIF][NIX-2*ZERO_PAD][NIY-2*ZERO_PAD]
		px_data_t InBuf[POY][WRD_INBUF][POX]
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0; 	// "State" for layer
	static data_bool layerCnfg = 1;	// State for loading layer confiduration data
	static Nif_dt Nif;           	// Parameters stored locally (1)
	static Niy_dt Niy;           	// Parameters stored locally (2)
	static Noy_step_dt Noy_step; 	// Parameters stored locally (3)
	static Tiy_dt Tiy;           	// Parameters stored locally (4)
	static Tix_dt Tix;           	// Parameters stored locally (5)
	static row_1map_dt row_1map; 	// Parameters stored locally (6)
	static wrd_1row_dt wrd_1row; 	// Parameters stored locally (7)
	static Poy_i_dt TiyRem;      	// Parameters stored locally (8)
	static Pox_i_dt TixRem;      	// Parameters stored locally (9)
	static data_bool NofFirst; 		// Parameters stored locally (10)
	static int yTileCount; 			// Parameters stored locally (11)

	data_bool northTile, southTile;
	Niy_dt yBase;
	Tiy_dt yTile;				 	// Number of pixels for y dimension to be loaded
	Tix_dt xTile; 				 	// Number of pixels for x dimension to be loaded
	row_inbuf_i_dt wrd_i; 			// Variables for addressing of InBuf (1)
	row_inbuf_i_dt wrdMap; 			// Variables for addressing of InBuf (2)
	row_inbuf_i_dt wrdY, wrdX; 		// Variables for addressing of InBuf (3)
	Poy_i_dt Poy_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		NofFirst = nofFirst[layerNo];
		Nif = Nif_rom[layerNo];
		Niy = niy_rom[layerNo];
		Noy_step = noy_step_rom[layerNo];
		Tiy = tiy_rom[layerNo]; // Contains zero padding
		Tix = tix_rom[layerNo]; // Contains zero padding
		row_1map = row_1map_rom[layerNo];
		wrd_1row = wrd_1row_rom[layerNo];
		TiyRem = (Tiy-1)%POY;
		TixRem = (Tix-1)%POX; 	// find position of last element of x row of IfMap -> mapped to InBuf
		layerCnfg = 0;
		yTileCount = 0;
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		// Calculate Input Parameters
		if(yTileCount == 0){
			northTile = 1;
		}
		else{
			northTile = 0;
		}
		if(yTileCount == Noy_step - 1){
			southTile = 1;
		}
		else{
			southTile = 0;
		}
		if(yTileCount == 0){
			yBase = 0;
		}
		else{
			yBase = yTileCount*(Tiy - 2) -1;
		}
		yTile = Tiy - northTile - southTile; xTile = Tix - 2;
		wrdMap = 0;
		If_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
		#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
			Poy_i = northTile; wrdY = 0;
			If_Tiy: for(int yTile_i=0;yTile_i<yTile;yTile_i++){
			#pragma HLS LOOP_TRIPCOUNT min=TIY_TRIPCOUNT max=TIY_TRIPCOUNT
				#if defined(FMAP_WIDEN)
					wrdX = 0;
					If_Nix: for(int xTile_i=0;xTile_i<xTile/7;xTile_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						wrd_i = wrdMap + wrdY + wrdX;
						InBuf[Poy_i][wrd_i  ][1] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS-1,0);
						InBuf[Poy_i][wrd_i  ][2] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*2-1,SYNTH_BITS);
						InBuf[Poy_i][wrd_i  ][3] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*3-1,SYNTH_BITS*2);
						InBuf[Poy_i][wrd_i  ][4] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*4-1,SYNTH_BITS*3);
						InBuf[Poy_i][wrd_i  ][5] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*5-1,SYNTH_BITS*4);
						InBuf[Poy_i][wrd_i  ][6] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*6-1,SYNTH_BITS*5);
						InBuf[Poy_i][wrd_i+1][0] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(SYNTH_BITS*7-1,SYNTH_BITS*6);
						wrdX++;
					}
					if(Poy_i == POY-1){
						Poy_i = 0;
						wrdY += wrd_1row; // ceil(Tix,Pox)
					}
					else{
						Poy_i++;
					}
				#else
					Pox_i = 1; wrdX = 0;
					If_Nix: for(int xTile_i=0;xTile_i<xTile;xTile_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						wrd_i = wrdMap + wrdY + wrdX;
						InBuf[Poy_i][wrd_i][Pox_i] =
						*(IfMap + Nif_i*(Niy)*(Tix-2) + (yBase+yTile_i)*(Tix-2) + xTile_i);
						if(Pox_i == POX-1){
							wrdX++;
							Pox_i = 0;
						}
						else{
							Pox_i++;
						}
					}
					if(Poy_i == POY-1){
						Poy_i = 0;
						wrdY += wrd_1row; // ceil(Tix,Pox)
					}
					else{
						Poy_i++;
					}
				#endif
			}
			wrdMap += row_1map * wrd_1row; // ceil(Tiy,Poy)*ceil(Tix,POY)
		}

		// North Zero Padding
		if(northTile){
			NorthPad_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				NorthPad_wrd: for(int wrd_1row_i=0;wrd_1row_i<wrd_1row;wrd_1row_i++){
				#pragma HLS LOOP_TRIPCOUNT min=WRD1ROW_TRIPCOUNT max=WRD1ROW_TRIPCOUNT
					NorthPad_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
					#pragma HLS UNROLL
						InBuf[0][Nif_i*row_1map*wrd_1row + wrd_1row_i][Pox_i] = 0;
					}
				}
			}
		}
		// South Zero Padding
		if(southTile){
			SouthPad_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				SouthPad_wrd: for(int wrd_1row_i=0;wrd_1row_i<wrd_1row;wrd_1row_i++){
				#pragma HLS LOOP_TRIPCOUNT min=WRD1ROW_TRIPCOUNT max=WRD1ROW_TRIPCOUNT
					SouthPad_Pox: for(int Pox_i=0;Pox_i<POX;Pox_i++){
					#pragma HLS UNROLL
						InBuf[TiyRem][Nif_i*row_1map*wrd_1row + (row_1map-1)*wrd_1row+ wrd_1row_i][Pox_i] = 0;
					}
				}
			}
		}
		// West Zero Padding
		WestPad_Line: for(int nxtLine_i=0;nxtLine_i<Nif*row_1map;nxtLine_i++){
		#pragma HLS LOOP_TRIPCOUNT min=ROW1MAPNIF_TRIPCOUNT max=ROW1MAPNIF_TRIPCOUNT
			WestPad_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
			#pragma HLS UNROLL
				InBuf[Poy_i][nxtLine_i*wrd_1row][0] = 0;
			}
		}
		// East Zero Padding
		EastPad_Line: for(int nxtLine_i=0;nxtLine_i<Nif*row_1map;nxtLine_i++){
		#pragma HLS LOOP_TRIPCOUNT min=ROW1MAPNIF_TRIPCOUNT max=ROW1MAPNIF_TRIPCOUNT
			EastPad_Poy: for(int Poy_i=0;Poy_i<POY;Poy_i++){
			#pragma HLS UNROLL
				InBuf[Poy_i][nxtLine_i*wrd_1row + wrd_1row-1][TixRem] = 0;
			}
		}
		if(yTileCount == Noy_step - 1){
			layerCnfg = 1;
		}
		else{
			yTileCount++;
		}
	}
}


#if not defined(WTMAP_WIDEN)
void loadWtMap(
		const wt_data_t_port *WtMap,
		wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0;  	// "State" for layer
	static data_bool layerCnfg = 1;		// State for loading layer confiduration data
	static Nif_dt Nif; 					// Parameters stored locally (1)
	static Nof_step_dt Nof_step;		// Parameters stored locally (2)
	static Tof_dt Tof; 					// Parameters stored locally (3)
	static data_bool NofFirst; 			// Parameters stored locally (4)
	static int fTileCount; 				// Parameters stored locally (5)
	Pof_i_dt Pof_i;
	row_wtbuf_i_dt wrdMap;
	Nofy_step_dt ofBase;

	if(layerCnfg){
		Nif = Nif_rom[layerNo];
		Nof_step = Nof_step_rom[layerNo];
		Tof = Tof_rom[layerNo];
		NofFirst = nofFirst[layerNo];
		layerCnfg = 0;
		fTileCount = 0;
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		Pof_i = 0; wrdMap = 0;
		ofBase = fTileCount;
		WtLoop_Tof: for(int Tof_i=0;Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			WtLoop_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
			
				WtLoop_Nky: for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				#pragma HLS LOOP_TRIPCOUNT min=NKY max=NKY
					WtLoop_Nkx: for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
					#pragma HLS LOOP_TRIPCOUNT min=NKX max=NKX
						WtBuf[wrdMap + Nif_i*NKY*NKX + Nky_i*NKX + Nkx_i][Pof_i] = 
							*(WtMap + (Tof_i+ofBase*Tof)*Nif*NKY*NKX + Nif_i*NKY*NKX + Nky_i*NKX + Nkx_i);
					}
				}
			}
			if(Pof_i == POF-1){
				Pof_i = 0;
				wrdMap += Nif*NKY*NKX;
			}
			else{
				Pof_i++;
			}
		}
		if(fTileCount == Nof_step - 1){
			layerCnfg = 1;
			fTileCount = 0;
		}
		else{
			fTileCount++;
		}
	}

}
#endif

#if defined(WTMAP_WIDEN)
void loadWtMap(
		const wt_data_t_port *WtMap,
		wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0;  	// "State" for layer
	static data_bool layerCnfg = 1;		// State for loading layer confiduration data
	static Nif_dt Nif; 					// Parameters stored locally (1)
	static Nof_step_dt Nof_step;		// Parameters stored locally (2)
	static Tof_step_dt Tof_step; 		// Parameters stored locally (3)
	static data_bool NofFirst; 			// Parameters stored locally (4)
	static int fTileCount; 				// Parameters stored locally (5)
	Nofy_step_dt ofBase;

	if(layerCnfg){
		Nif = Nif_rom[layerNo];
		Nof_step = Nof_step_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		NofFirst = nofFirst[layerNo];
		layerCnfg = 0;
		fTileCount = 0;
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		ofBase = fTileCount;
		WtLoop_Tof: for(int Tof_step_i=0;Tof_step_i<Tof_step;Tof_step_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_STEP_TRIPCOUNT max=TOF_STEP_TRIPCOUNT
			WtLoop_Nif: for(int Nif_i=0;Nif_i<Nif;Nif_i++){
			#pragma HLS LOOP_TRIPCOUNT min=NIF_TRIPCOUNT max=NIF_TRIPCOUNT
				WtLoop_Nky: for(int Nky_i=0;Nky_i<NKY;Nky_i++){
				#pragma HLS LOOP_TRIPCOUNT min=NKY max=NKY
					WtLoop_Nkx: for(int Nkx_i=0;Nkx_i<NKX;Nkx_i++){
					#pragma HLS LOOP_TRIPCOUNT min=NKX max=NKX
						WtLoop_Pof: for (int i = 0; i < (POF/WTMAP_WIDTHFACTOR); i++) {
							for (int chunk = 0; chunk < WTMAP_WIDTHFACTOR; chunk++) {
							#pragma HLS UNROLL
								int idx = i*WTMAP_WIDTHFACTOR + chunk;
								int bit_start = chunk * SYNTH_BITS;
								int bit_end   = bit_start + SYNTH_BITS - 1;

								WtBuf[Tof_step_i*Nif*NKY*NKX 
									+ Nif_i*NKY*NKX 
									+ Nky_i*NKX 
									+ Nkx_i][idx] =
									WtMap[(Tof_step_i + ofBase*Tof_step) * Nif*NKY*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nif_i*NKY*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nky_i*NKX*(POF/WTMAP_WIDTHFACTOR)
										+ Nkx_i*(POF/WTMAP_WIDTHFACTOR)
										+ i].range(bit_end, bit_start);
							}
						}
					}
				}
			}
		}
		if(fTileCount == Nof_step - 1){
			layerCnfg = 1;
			fTileCount = 0;
		}
		else{
			fTileCount++;
		}
	}
}
#endif


void mem2Buf(
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	static data_bool NofFirst;
	static Nofy_step_dt nofy_step; 		// convLayer module loop tripcount
	static layerNo_dt layerNo = 0; 		// "State" for layer
	static data_bool layerCnfg = 1;		// State for loading layer confiduration data
	static Nofy_step_dt tileCount;

	if(layerCnfg){
		NofFirst = nofFirst[layerNo];
		nofy_step = nofy_step_rom[layerNo];
		layerCnfg = 0;
		tileCount = 0;
		loadIfMap(IfMap, InBuf);
		loadWtMap(WtMap, WtBuf);
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(tileCount == 0){
			loadIfMap(IfMap, InBuf);
			loadWtMap(WtMap, WtBuf);
		}
		else{
			if(NofFirst == 1){
				loadIfMap(IfMap, InBuf);
			}
			else{
				loadWtMap(WtMap, WtBuf);
			}
		}
		if(tileCount == nofy_step-1){
			layerCnfg = 1;
			tileCount = 0;
		}
		else{
			tileCount++;
		}
	}
}


void loadBiasTile(
		b_data_t BiasBuf[BIASBUF_LENGTH]
	){
#pragma HLS PIPELINE off
	#pragma HLS INLINE off
	// Based on Nof_step, Noy_step and layerNo, we load
	// the correct Tof biases to the corresponding buffer
	// bias[] is stored locally in this module, has the biases for all layers of the CNN.

	static row_biasbuf_i_dt  addressBase = 0; 	// Address base for each layer
	static Nof_step_dt   Nof_step;        		// Locally store variables (1)
	static Tof_dt        Tof;       			// Locally store variables (2)
	static Tof_step_dt   Tof_step;      		// Locally store variables (3)
	static Nof_step_i_dt Nof_step_i = 0;  		// Locally stored counter
	static data_bool layerCnfg = 1;				// State for loading layer confiduration data
	static layerNo_dt layerNo = 0;          	// Layer "state"
	static int tileCount; 						// Count of executed tiles
	static Nofy_step_dt Nofy_step;				// convLayer module loop tripcount

	if(layerCnfg){
		if(layerNo != 0){
			addressBase += Nof_step*Tof_step*POF; // this needs to be fixed!!!
		}
		else{
			addressBase = 0;
		}
		Nof_step = Nof_step_rom[layerNo];
		Nofy_step = nofy_step_rom[layerNo];
		Tof = Tof_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		Nof_step_i = 0;
		layerCnfg = 0;
		tileCount = 0;
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		BiasLoop_Tof: for(int Tof_i=0;Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			BiasBuf[Tof_i] = bias[addressBase + Nof_step_i*Tof + Tof_i];
		}
		if(Nof_step_i == Nof_step-1){ // In the case of Nof_step=1, Nof_step_i stays zero
			Nof_step_i = 0;
		}
		else{
			Nof_step_i++;
		}
		if(tileCount == Nofy_step - 1){
			layerCnfg = 1;
			tileCount = 0;
		}
		else{
			tileCount++;
		}
	}

}


void tileClc(
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0; 					// Layer "state"
	static data_bool layerCnfg = 1;					// State for loading layer confiduration data
	static int tileCount;							// Count iterations of tiles for current layer
	static tile_loop_dt tileclc_loop_limit; 		// tileClc Pe iteration bound
	static wnd_loop_dt wndclc_loop_limit; 			// Pe iteration bound
	static wtbuf2pe_loop_dt wtbuf2pe_loop_limit;	// WtBuf2Pe Iteration bound
	static wnd_loop_dt pe_loop_limit; 				// wndClc Iteration bound
	static Nofy_step_dt Nofy_step;					// Variable Parameter stored locally (1)
	static Nif_dt Nif;           		  			// Variable Parameter Stored Locally (2)
	static Toy_dt Toy;								// Variable Parameter Stored Locally (3)
	static Toy_step_dt Toy_step; 					// Variable Parameter Stored Locally (4)
	static Tox_step_dt Tox_step; 					// Variable Parameter Stored Locally (5)
	static Tof_step_dt Tof_step; 					// Variable Parameter Stored Locally (6)
	static wrd_1row_dt wrd_1row; 					// Variable Parameter Stored Locally (7)
	static row_1map_dt row_1map; 					// Variable Parameter Stored Locally (8)
	static row_outbuf_i_dt pe2buf_addr_offset1;		// Variable Parameter Stored Locally (9)
	static row_outbuf_i_dt pe2buf_addr_offset2;		// Variable Parameter Stored Locally (10)
	static row_outbuf_i_dt pe2buf_addr_offset3;		// Variable Parameter Stored Locally (11)
	static bit_shift_dt bit_shift;					// Variable Parameter Stored Locally (12)

	if(layerCnfg){
		tileclc_loop_limit = tileclc_loop_limit_rom[layerNo];
		wndclc_loop_limit = wndclc_loop_limit_rom[layerNo];
		wtbuf2pe_loop_limit = wtbuf2pe_loop_limit_rom[layerNo]; // Tof_step*Nif*NKY*NKX-1
		pe_loop_limit = pe_loop_limit_rom[layerNo];
		Nofy_step = nofy_step_rom[layerNo];
		Nif = Nif_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Toy_step = toy_step_rom[layerNo];
		Tox_step = tox_step_rom[layerNo];
		Tof_step = tof_step_rom[layerNo];
		wrd_1row = wrd_1row_rom[layerNo];
		row_1map = row_1map_rom[layerNo];
		pe2buf_addr_offset1 = pe2buf_addr_offset1_rom[layerNo];
		pe2buf_addr_offset2 = pe2buf_addr_offset2_rom[layerNo];
		pe2buf_addr_offset3 = pe2buf_addr_offset3_rom[layerNo];
		bit_shift = bit_shift_rom[layerNo];
		layerCnfg = 0;
		tileCount = 0;
		if(layerNo==LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		tileClc_Dfl(
				/* Parameters*/ tileclc_loop_limit, wndclc_loop_limit,
				wtbuf2pe_loop_limit,
				Nif, Toy,
				Toy_step, Tox_step, Tof_step,
				wrd_1row, row_1map, pe2buf_addr_offset1,
				pe2buf_addr_offset2, pe2buf_addr_offset3,
				bit_shift,
				/* Inputs */ InBuf, WtBuf, BiasBuf,
				/* Output */ OutBuf);
		if(tileCount == Nofy_step - 1){
			layerCnfg = 1;
			tileCount = 0;
		}
		else{
			tileCount++;
		}
	}
}


#if not defined(MAXPOOL_INTEGRATION)
void storeMap(
		const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	static Nofy_step_dt Nofy_step;
	static Noy_dt Noy;
	static Tof_dt Tof;
	static Toy_dt Toy;
	static Tox_dt Tox;
	static Tox_step_dt wrd_1rowOut;
	static data_bool layerCnfg = 1;		// State for loading layer confiduration data
	static int tileCount;				// Count iterations of tiles for current layer
	static data_bool NofFirst;			// Boolean value for loop order
	static layerNo_dt layerNo = 0; 		// "State" for layer

	int ofBase, yBase;
	px_data_t_port tmpOut;
	#pragma HLS ARRAY_PARTITION variable=tmpOut complete dim=1

	row_outbuf_i_dt wrdMap, wrdY, wrdX;
	outbufnum_i_dt OutBufNum_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		Nofy_step = nofy_step_rom[layerNo];
		Noy = Noy_rom[layerNo];
		Tof = Tof_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Tox = Tox_rom[layerNo];
		wrd_1rowOut = tox_step_rom[layerNo];
		layerCnfg = 0;
		tileCount = 0;
		NofFirst = nofFirst[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(NofFirst){
			ofBase = 0;
			yBase = tileCount;
		}
		else{
			ofBase = tileCount;
			yBase = 0;
		}
		wrdMap = 0; OutBufNum_i = 0;
		Loop_Tof: for(int Tof_i=0; Tof_i<Tof;Tof_i++){
		#pragma HLS LOOP_TRIPCOUNT min=TOF_TRIPCOUNT max=TOF_TRIPCOUNT
			wrdY = 0;
			Loop_Toy: for(int Toy_i=0; Toy_i<Toy;Toy_i++){
			#pragma HLS LOOP_TRIPCOUNT min=TOY_TRIPCOUNT max=TOY_TRIPCOUNT
				Pox_i = 0; wrdX = 0;
				#if not defined(FMAP_WIDEN)
					Loop_Tox: for(int Tox_i=0; Tox_i<Tox;Tox_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						*(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + wrdX][Pox_i];
						#ifdef DEBUG_MODE
							if( *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) >= (1 << HW_EMUL_NUM_BITS)){
									std::cout << "Value out of range found in " << (Tof_i+ofBase*Tof) << " Map, "
										<< (Toy_i+yBase*Toy) << " Row, " << Tox_i << " Column\n";
									std::cout << "Value is " << *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) << "\n";
							}
						#endif
						if(Pox_i == POX-1){
							Pox_i = 0;
							wrdX++;
						}
						else{
							Pox_i++;
						}
					}
				#elif defined(FMAP_WIDEN)
					Loop_Tox: for(int Tox_i=0; Tox_i<Tox/7;Tox_i++){
					#pragma HLS LOOP_TRIPCOUNT min=(TOX_TRIPCOUNT/7) max=(TOX_TRIPCOUNT/7)
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS-1,0) =
														OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][0];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*2-1,SYNTH_BITS) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][1];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*3-1,SYNTH_BITS*2) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][2];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*4-1,SYNTH_BITS*3) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][3];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*5-1,SYNTH_BITS*4) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][4];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*6-1,SYNTH_BITS*5) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][5];
						OfMap[(Tof_i+ofBase*Tof)*Noy*Tox/7 + (Toy_i+yBase*Toy)*Tox/7 +Tox_i].range(SYNTH_BITS*7-1,SYNTH_BITS*6) =
								OutBuf[OutBufNum_i][wrdMap + wrdY + Tox_i][6];
											}	
				#endif
				wrdY += wrd_1rowOut;
			}

			if(OutBufNum_i == OUTBUF_NUM-1){
				OutBufNum_i = 0;
				wrdMap += Toy*wrd_1rowOut;
			}
			else{
				OutBufNum_i++;
			}
		}
		if(tileCount == Nofy_step - 1){
			layerCnfg = 1;
			tileCount = 0;
		}
		else{
			tileCount++;
		}
	}
}
#endif


#if defined(MAXPOOL_INTEGRATION)
void maxPoolTree(px_data_t tmp1[POX], px_data_t tmp2[POX], px_data_t tmp3[POX], 
			px_data_t tmp4[POX], px_data_t max[POX]){
	px_data_t maxTmp[POX*2];
	#pragma HLS ARRAY_PARTITION variable=maxTmp complete dim=1
	for(int i=0;i<POX;i++){
	#pragma HLS UNROLL
		maxTmp[i    ] = (tmp1[i]>tmp3[i]) ? tmp1[i] : tmp3[i];
		maxTmp[i+POX] = (tmp2[i]>tmp4[i]) ? tmp2[i] : tmp4[i];
	}
	for(int i=0;i<POX;i++){
	#pragma HLS UNROLL
		max[i] = (maxTmp[2*i]>maxTmp[2*i+1]) ? maxTmp[2*i] : maxTmp[2*i+1];
	}
}
#endif


#if defined(MAXPOOL_INTEGRATION)
void storeMap(
		const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	static layerNo_dt layerNo = 0; 		// "State" for layer
	static data_bool layerCnfg = 1;		// State for loading layer confiduration data
	static int tileCount;				// Count iterations of tiles for current layer
	static data_bool NofFirst;			// Boolean value for loop order
	static Nofy_step_dt Nofy_step;		// Parameters stored locally (1)
	static Noy_dt Noy;					// Parameters stored locally (3)
	static Noy_dt Noy_map;				// Parameters stored locally (3)
	static Tof_dt Tof;					// Parameters stored locally (4)
	static Toy_dt Toy;					// Parameters stored locally (5)
	static Toy_dt Toy_map;				// Parameters stored locally (6)
	static Tox_dt Tox;					// Parameters stored locally (7)
	static Tox_dt Tox_map;				// Parameters stored locally (8)
	static Tox_step_dt wrd_1rowOut;		// Parameters stored locally (9)

	int ofBase, yBase;
	int base_addr, addr;
	#if defined(HEAD_INTEGRATION)
		acc_data_t acc;
		int addr_gap;
		const acc_data_t reciprocal = (1 << RECIPROCAL_BITS) / (7 * 7); // fixed-point reciprocal
	#endif
	#if defined(FMAP_WIDEN)
		static ap_uint<3> state;
	#endif
	px_data_t tmp1[POX], tmp2[POX], tmp3[POX], tmp4[POX];
	#pragma HLS ARRAY_PARTITION variable=tmp1 complete dim=1
	#pragma HLS ARRAY_PARTITION variable=tmp2 complete dim=1
	#pragma HLS ARRAY_PARTITION variable=tmp3 complete dim=1
	#pragma HLS ARRAY_PARTITION variable=tmp4 complete dim=1
	px_data_t max[POX];
	#pragma HLS ARRAY_PARTITION variable=max complete dim=1
	row_outbuf_i_dt wrdMap, wrdY, wrdX;
	outbufnum_i_dt OutBufNum_i;
	Pox_i_dt Pox_i;

	if(layerCnfg){
		Nofy_step = nofy_step_rom[layerNo];
		Noy = Noy_rom[layerNo];
		if(layerNo==0 || layerNo==2 || layerNo==4 || layerNo==5 ||
			layerNo==7 || layerNo==8 || layerNo==10 || layerNo==11){
			Noy_map = Noy_rom[layerNo];
			Toy_map = Toy_rom[layerNo];
			Tox_map = Tox_rom[layerNo];
		}
		else{
			Noy_map = Noy_rom[layerNo]/2;
			Toy_map = Toy_rom[layerNo]/2;
			Tox_map = Tox_rom[layerNo]/2;
		}
		Tof = Tof_rom[layerNo];
		Toy = Toy_rom[layerNo];
		Tox = Tox_rom[layerNo];
		wrd_1rowOut = tox_step_rom[layerNo];
		layerCnfg = 0;
		tileCount = 0;
		#if defined(FMAP_WIDEN)
			state = 0;
		#endif
		NofFirst = nofFirst[layerNo];
		if(layerNo == LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	else{
		if(NofFirst){
			ofBase = 0;
			yBase = tileCount;
		}
		else{
			ofBase = tileCount;
			yBase = 0;
		}

		base_addr = ofBase*Tof*Noy_map*Tox_map/FMAP_WIDTHFACTOR + yBase*Toy_map*Tox_map/FMAP_WIDTHFACTOR;

		#if defined(HEAD_INTEGRATION)
			addr_gap = tileCount*Tof/FMAP_WIDTHFACTOR;
		#endif

		Loop_Tof_ii: for(int Tof_ii=0; Tof_ii<Tof/OUTBUF_NUM;Tof_ii++){
		#pragma HLS LOOP_TRIPCOUNT min=(TOF_TRIPCOUNT/OUTBUF_NUM) max=(TOF_TRIPCOUNT/OUTBUF_NUM)
			Loop_OutBufNum: for(int OutBufNum_i=0; OutBufNum_i<OUTBUF_NUM;OutBufNum_i++){
			#pragma HLS LOOP_TRIPCOUNT min=OUTBUF_NUM max=OUTBUF_NUM
				addr = base_addr;
				#if defined(HEAD_INTEGRATION)
					acc = 0;
				#endif
				Loop_Toy_step: for(int Toy_i=0; Toy_i<Toy_map;Toy_i++){
				#pragma HLS LOOP_TRIPCOUNT min=TOYMAP_TRIPCOUNT max=TOYMAP_TRIPCOUNT

					Loop_Tox_step: for(int Tox_step_i=0; Tox_step_i<Tox_map/POX;Tox_step_i++){
					#pragma HLS LOOP_TRIPCOUNT min=(TOXMAP_TRIPCOUNT/7) max=(TOXMAP_TRIPCOUNT/7)
					#pragma HLS PIPELINE II=7
						#if not defined(FMAP_WIDEN)
							if(layerNo==1 || layerNo==3 || layerNo==5 || layerNo==6 ||
								layerNo==8 || layerNo==9 || layerNo==11 || layerNo==12){
								Loop_POX0: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
								#pragma HLS PIPELINE II=1
									OfMap[addr] =
										OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + Toy_i*wrd_1rowOut + Tox_step_i][Pox_i];
									addr++;
								}
							}
							else{
								Loop_POX: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS UNROLL
									tmp1[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp2[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
									tmp3[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp4[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
								}
								maxPoolTree(tmp1, tmp2, tmp3, tmp4, max);
								#if not defined(HEAD_INTEGRATION)
									Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
									#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
										OfMap[addr] =
											max[Pox_i];
										addr++;
									}
								#else
									if(layerNo==0){
										for(int Pox_i=0;Pox_i<POX;Pox_i++){
											acc += max[Pox_i];
										}
									}
									else{
										Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
										#pragma HLS LOOP_TRIPCOUNT min=POX max=POX
											OfMap[addr] =
												max[Pox_i];
											addr++;
										}
									}
								#endif
							}
						#elif defined(FMAP_WIDEN)
							if(layerNo==1 || layerNo==3 || layerNo==5 || layerNo==6 ||
								layerNo==8 || layerNo==9 || layerNo==11 || layerNo==12){
								Loop_POX0: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS UNROLL
									OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
										OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + Toy_i*wrd_1rowOut + Tox_step_i][Pox_i];
								}
								addr++;
							}
							else{
								Loop_POX: for(int Pox_i=0; Pox_i<POX;Pox_i++){
								#pragma HLS UNROLL
									tmp1[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp2[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut +  2*Toy_i   *wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
									tmp3[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i  ][Pox_i];
									tmp4[Pox_i] = OutBuf[OutBufNum_i][Tof_ii*Toy*wrd_1rowOut + (2*Toy_i+1)*wrd_1rowOut + 2*Tox_step_i+1][Pox_i];
								}
								maxPoolTree(tmp1, tmp2, tmp3, tmp4, max);
								#if not defined(HEAD_INTEGRATION)
									Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
									#pragma HLS UNROLL
										OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
											max[Pox_i];
									}
									addr++;
								#else
									if(layerNo==0){
										for(int Pox_i=0;Pox_i<POX;Pox_i++){
											acc += max[Pox_i];
										}
									}
									else{
										Loop_POX2: for(int Pox_i=0; Pox_i<POX;Pox_i++){
										#pragma HLS UNROLL
											OfMap[addr].range(SYNTH_BITS*(Pox_i+1)-1, SYNTH_BITS*Pox_i) =
												max[Pox_i];
										}
										addr++;
									}
								#endif
							}
						#endif
					}

				}
				
				base_addr += Noy_map*Tox_map/FMAP_WIDTHFACTOR;
				#if defined(HEAD_INTEGRATION)
					if(layerNo==0){
						acc *= reciprocal;
						#if defined(FMAP_WIDEN)
							OfMap[addr_gap].range(SYNTH_BITS*(state+1)-1, SYNTH_BITS*state) = (acc >> RECIPROCAL_BITS);
							if(state==6){
								addr_gap++;
								state = 0;
							}
							else{
								state++;
							}
							acc = 0;
						#else
							OfMap[addr_gap] = (acc >> RECIPROCAL_BITS);
							addr_gap++;
							acc = 0;
						#endif
					}
				#endif
			}
		}
		if(tileCount == Nofy_step - 1){
			layerCnfg = 1;
			tileCount = 0;
		}
		else{
			tileCount++;
		}
	}
}
#endif


void ConvLayerScdl(
		// Parameter Loading State
		data_bool NofFirst,
		Noy_step_dt Noy_step,
		Nofy_step_dt nofy_step,
		Tiy_dt Tiy,
		// Intermediate (Buffered) Data
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		// Inputs
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Output
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	loadIfMap(IfMap, InBuf);
	loadWtMap(WtMap, WtBuf);
	loadBiasTile(BiasBuf);
	tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
	storeMap(OutBuf, OfMap);

	if(NofFirst==1){
		loadWtMap(WtMap, WtBuf);
		if(nofy_step==1){
			loadIfMap(IfMap, InBuf);
			loadBiasTile(BiasBuf);
			tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
			storeMap(OutBuf, OfMap);
		}
		else if(nofy_step==2){
			for(int nofy_step_i=0;nofy_step_i<2;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
		else if(nofy_step==4){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
		else if(nofy_step==8){
			for(int nofy_step_i=0;nofy_step_i<8;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}	
		}
		else if(nofy_step==16){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}	
		}			
	}
	else{
		loadIfMap(IfMap, InBuf);
		if(nofy_step==1){
			loadWtMap(WtMap, WtBuf);
			loadBiasTile(BiasBuf);
			tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
			storeMap(OutBuf, OfMap);
		}
		else if(nofy_step==2){
			for(int nofy_step_i=0;nofy_step_i<2;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
		else if(nofy_step==4){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
		else if(nofy_step==8){
			for(int nofy_step_i=0;nofy_step_i<8;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
		else if(nofy_step==16){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf);
				loadBiasTile(BiasBuf);
				tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
				storeMap(OutBuf, OfMap);
			}
		}
	}
}


void ConvLayerScdlDB(
		// Parameter Loading State
		data_bool NofFirst,
		Noy_step_dt Noy_step,
		Nofy_step_dt nofy_step,
		Tiy_dt Tiy,
		// Intermediate (Buffered) Data
		px_data_t InBuf1[POY][WRD_INBUF][POX],
		wt_data_t WtBuf1[WRD_WTBUF][POF],
		px_data_t OutBuf1[OUTBUF_NUM][WRD_OUTBUF][POX],
		b_data_t BiasBuf1[BIASBUF_LENGTH],
		px_data_t InBuf2[POY][WRD_INBUF][POX],
		wt_data_t WtBuf2[WRD_WTBUF][POF],
		px_data_t OutBuf2[OUTBUF_NUM][WRD_OUTBUF][POX],
		b_data_t BiasBuf2[BIASBUF_LENGTH],
		// Inputs
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Output
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
	loadIfMap(IfMap, InBuf1);
	loadWtMap(WtMap, WtBuf1);
	loadBiasTile(BiasBuf1);
	tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
	storeMap(OutBuf1, OfMap);

	if(NofFirst==1){
		loadWtMap(WtMap, WtBuf1);
		if(nofy_step==2){
			// Simple Loop
			for(int nofy_step_i=0;nofy_step_i<1;nofy_step_i++){
				loadIfMap(IfMap, InBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadIfMap(IfMap, InBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf2, WtBuf1, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}
		}
		else if(nofy_step==4){
			for(int nofy_step_i=0;nofy_step_i<2;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadIfMap(IfMap, InBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf2, WtBuf1, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}	
		}
		else if(nofy_step==8){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadIfMap(IfMap, InBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadIfMap(IfMap, InBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf2, WtBuf1, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}				
		}		
	}
	else{
		loadIfMap(IfMap, InBuf1);
		if(nofy_step==2){
			for(int nofy_step_i=0;nofy_step_i<1;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadWtMap(WtMap, WtBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf1, WtBuf2, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}
		}
		else if(nofy_step==4){
			for(int nofy_step_i=0;nofy_step_i<2;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadWtMap(WtMap, WtBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf1, WtBuf2, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}	
		}
		else if(nofy_step==8){
			for(int nofy_step_i=0;nofy_step_i<4;nofy_step_i++){
			#pragma HLS UNROLL
				loadWtMap(WtMap, WtBuf1);
				loadBiasTile(BiasBuf1);
				tileClc(InBuf1, WtBuf1, BiasBuf1, OutBuf1);
				storeMap(OutBuf1, OfMap);
				loadWtMap(WtMap, WtBuf2);
				loadBiasTile(BiasBuf2);
				tileClc(InBuf1, WtBuf2, BiasBuf2, OutBuf2);
				storeMap(OutBuf2, OfMap);
			}
		}
	}
}


void ConvLayerScdlDFL(
		int loop_limit,
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Output
		px_data_t_port *OfMap
	){
	#pragma HLS INLINE off
		for(int nofy_step_i=0;nofy_step_i<loop_limit;nofy_step_i++){
		#pragma HLS DATAFLOW
		// Intermediate (Buffered) Data
		px_data_t InBuf[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
		static wt_data_t WtBuf[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
		static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
		b_data_t BiasBuf[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf
			mem2Buf(IfMap, WtMap, InBuf, WtBuf);
			loadBiasTile(BiasBuf);
			tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
			storeMap(OutBuf, OfMap);
		}
}
#endif
// ********  Overlap Case Region 3 END  *******

void ConvLayer(
		//Inputs
		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		const wt_data_t_port *WtMap, 	// [NOF][NIF][NKY][NKX]
		//Output
		px_data_t_port *OfMap 			// [NOF][NOY][NOX]
	){
	#pragma HLS INLINE off
	#pragma HLS INTERFACE m_axi port=IfMap depth=FMAP_MEMSIZE_WIDENED bundle=IFMAP
	#pragma HLS INTERFACE m_axi port=WtMap depth=WTMAP_MEMSIZE_WIDENED bundle=WTMAP
	#pragma HLS INTERFACE m_axi port=OfMap depth=FMAP_MEMSIZE_WIDENED bundle=OFMAP
	static layerNo_dt layerNo = 0; 				// State of number of convolutional layer
// Sequential
	#if defined(REG3_SEQ)

	#if not defined(INTERNAL_CONFIG_LAYER)
		px_data_t InBuf[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
		static wt_data_t WtBuf[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
		static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
		b_data_t BiasBuf[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf
		mem2Buf(/*layerCnfg=*/1, IfMap, WtMap, InBuf, WtBuf);
		loadBiasTile(/*layerCnfg=*/1, BiasBuf);
		tileClc(/*layerCnfg=*/1, InBuf, WtBuf, BiasBuf, OutBuf);
		storeMap(/*layerCnfg=*/1, OutBuf, OfMap);
		for(int nofy_step_i=0;nofy_step_i<nofy_step_rom[layerNo];nofy_step_i++){
			mem2Buf(/*layerCnfg=*/0, IfMap, WtMap, InBuf, WtBuf);
			loadBiasTile(/*layerCnfg=*/0, BiasBuf);
			tileClc(/*layerCnfg=*/0, InBuf, WtBuf, BiasBuf, OutBuf);
			storeMap(/*layerCnfg=*/0, OutBuf, OfMap);
		}
	#endif
	#if defined(INTERNAL_CONFIG_LAYER)
		px_data_t InBuf[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
		static wt_data_t WtBuf[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
		static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
		b_data_t BiasBuf[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf
		for(int nofy_step_i=0;nofy_step_i<nofy_step_rom[layerNo]+1;nofy_step_i++){
		// Intermediate (Buffered) Data
			mem2Buf(IfMap, WtMap, InBuf, WtBuf);
			loadBiasTile(BiasBuf);
			tileClc(InBuf, WtBuf, BiasBuf, OutBuf);
			storeMap(OutBuf, OfMap);
		}
	#endif

	#endif
// Half Overlapping Parallelism (Dual Port)
	#if defined(REG3_OVLP)

	#if defined(INTERNAL_CONFIG_LAYER)
		px_data_t InBuf[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
		static wt_data_t WtBuf[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
		static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
		b_data_t BiasBuf[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf
		ConvLayerScdl(
			nofFirst[layerNo], noy_step_rom[layerNo], nofy_step_rom[layerNo], tiy_rom[layerNo], 
			InBuf, WtBuf, OutBuf, BiasBuf, 
			IfMap, WtMap, OfMap
		);
	#endif

	#endif
// Full overlapping parallelism (Dual Buffering)
	#if defined(REG3_FOVLP)

	#if defined(INTERNAL_CONFIG_LAYER)
		px_data_t InBuf1[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf1 complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf1 complete dim=3
		static wt_data_t WtBuf1[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf1 complete dim=2
		static px_data_t OutBuf1[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf1 complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf1 complete dim=3
		b_data_t BiasBuf1[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf1
		px_data_t InBuf2[POY][WRD_INBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=InBuf2 complete dim=1
		#pragma HLS ARRAY_PARTITION variable=InBuf2 complete dim=3
		static wt_data_t WtBuf2[WRD_WTBUF][POF];
		#pragma HLS ARRAY_PARTITION variable=WtBuf2 complete dim=2
		static px_data_t OutBuf2[OUTBUF_NUM][WRD_OUTBUF][POX];
		#pragma HLS ARRAY_PARTITION variable=OutBuf2 complete dim=1
		#pragma HLS ARRAY_PARTITION variable=OutBuf2 complete dim=3
		b_data_t BiasBuf2[BIASBUF_LENGTH];
		#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf2
		ConvLayerScdlDB(
			nofFirst[layerNo], noy_step_rom[layerNo], nofy_step_rom[layerNo], tiy_rom[layerNo],
			InBuf1, WtBuf1, OutBuf1, BiasBuf1,
			InBuf2, WtBuf2, OutBuf2, BiasBuf2,
			IfMap, WtMap, OfMap
		);
	#endif

	#endif
// Dataflow
	#if defined(REG3_DFL)
	ConvLayerScdlDFL(
			(nofy_step_rom[layerNo]+1), IfMap, WtMap, OfMap
	);
	#endif
// Layer count
	if(layerNo == LAYERS - 1){
		layerNo = 0;
	}
	else{
		layerNo++;
	}

}


void ConvX(
		//Inputs
		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		const wt_data_t_port *WtMap, 	// [NOF][NIF][NKY][NKX]
		const wt_data_t      *WtMapFc,
		//Output
		px_data_t_port *OfMap 			// [NOF][NOY][NOX]
	){
	#pragma HLS INTERFACE m_axi port=IfMap depth=FMAP_MEMSIZE_WIDENED 	bundle=IFMAP
	#pragma HLS INTERFACE m_axi port=WtMap depth=WTMAP_MEMSIZE_WIDENED 	bundle=WTMAP
	#pragma HLS INTERFACE m_axi port=WtMap depth=(512*256) 				bundle=WTMAP
	#pragma HLS INTERFACE m_axi port=OfMap depth=FMAP_MEMSIZE_WIDENED 	bundle=OFMAP
	static layerNo_dt layerNo = 0; 				// State of number of convolutional layer
	#if defined(HEAD_INTEGRATION)
		if(layerNo==LAYERS){
			fcLayersOFBlock(IfMap, WtMapFc, OfMap);
			layerNo = 0;
		}
		else{
			ConvLayer(IfMap, WtMap, OfMap);
			layerNo++;
		}
	#elif not defined(HEAD_INTEGRATION)
		if(layerNo==LAYERS+1){
			fcLayersOFBlock(IfMap, WtMapFc, OfMap);
			layerNo = 0;
		}
		else if(layerNo==LAYERS){
			gap(IfMap, OfMap);
			layerNo++;
		}
		else{
			ConvLayer(IfMap, WtMap, OfMap);
			layerNo++;
		}
	#endif
}


// void CNN_Base(
// 		//Inputs
// 		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
// 		const wt_data_t_port *WtMap, 	// [NOF][NIF][NKY][NKX]
// 		const wt_data_t      *WtMapFc,
// 		//Output
// 		px_data_t_port *OfMap 			// [NOF][NOY][NOX]
// 	){
// 	#pragma HLS INTERFACE m_axi port=IfMap depth=FMAP_MEMSIZE_WIDENED 	bundle=IFMAP
// 	#pragma HLS INTERFACE m_axi port=WtMap depth=WTMAP_MEMSIZE_WIDENED 	bundle=WTMAP
// 	#pragma HLS INTERFACE m_axi port=WtMap depth=(512*256) 				bundle=WTMAP
// 	#pragma HLS INTERFACE m_axi port=OfMap depth=FMAP_MEMSIZE_WIDENED 	bundle=OFMAP
// 	static int layerNo = 0; 	
// // Case 1	
// 	// ConvLayer(IfMap, WtMap, OfMap);
// 	// if(layerNo==LAYERS){
// 	// 	layerNo = 0;
// 	// }
// 	// else{
// 	// 	layerNo++;			// State of number of convolutional layer
// 	// }

// // Case 2
// 	// if(layerNo==LAYERS){
// 	// 	fcLayersOFBlock(IfMap, WtMapFc, OfMap);
// 	// 	layerNo = 0;
// 	// }
// 	// else{
// 	// 	ConvLayer(IfMap, WtMap, OfMap);
// 	// 	layerNo++;
// 	// }

// // Case 3
// 	if(layerNo==LAYERS+5){
// 		fcLayersOFBlock(IfMap, WtMapFc, OfMap);
// 		layerNo = 0;
// 	}
// 	else if(layerNo==2 || layerNo==5 || layerNo==9 || layerNo==13 || layerNo==17){
// 		if(layerNo==2){
// 			maxpool2x2(IfMap, 64, 224, 224, OfMap);
// 		}
// 		else if(layerNo==5){
// 			maxpool2x2(IfMap, 128, 112, 112, OfMap);
// 		}
// 		else if(layerNo==9){
// 			maxpool2x2(IfMap, 256, 56, 56, OfMap);
// 		}
// 		else if(layerNo==13){
// 			maxpool2x2(IfMap, 512, 28, 28, OfMap);
// 		}
// 		else if(layerNo==17){
// 			maxpool2x2(IfMap, 512, 14, 14, OfMap);
// 		}
// 		layerNo++;
// 	}
// 	else{
// 		ConvLayer(IfMap, WtMap, OfMap);
// 		layerNo++;
// 	}
// }

void gap(const px_data_t *in, px_data_t *out){
	// int input[512 * 7 * 7]; // CHW format
	// int output[512];        // GAP output
	const ap_int<32> reciprocal = (1 << RECIPROCAL_BITS) / (7 * 7); // fixed-point reciprocal

	gap_c: for (int c = 0; c < 512; c++) {
	#pragma HLS UNROLL off
		acc_data_t sum = 0;
		int offset = c * 7 * 7;
		gap_xy: for (int i = 0; i < 7 * 7; i++) {
		#pragma HLS PIPELINE II=1
			sum += in[offset + i];
		}
		out[c] = (sum * reciprocal) >> RECIPROCAL_BITS; // Tranformed division to mul and right bit shift
	}
}


void fcLayersOF(
		/*Inputs*/ px_data_t *IfMap, wt_data_t *WtMap,
		/*Output*/ px_data_t finalOut[17]){
	px_data_t outPx1[256];
	int min, max, minWt, maxWt;

	fcLayer(IfMap, WtMap, 512, 256, 0, outPx1);
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*256, minWt, maxWt);
		findMinMax(outPx1, 256, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "20) After fc1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetFC[0];
	fcLayer(outPx1, WtMap, 256, 17, 1, finalOut);
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 256*CLASSES, minWt, maxWt);
		findMinMax(finalOut, CLASSES, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "21) After fc2: min=" << min << ", max=" << max << "\n";
		std::cout << "Finished second fc layer!" << std::endl;
	#endif
}


void fcLayersOFBlock(
		/*Inputs*/ const px_data_t_port *IfMap, const wt_data_t *WtMap,
		/*Output*/ px_data_t_port *OfMap){
	#pragma HLS INLINE off
	#ifndef __SYNTHESIS__
		int min, max, minWt, maxWt;
	#endif
	px_data_t inPx[512];
	px_data_t outPx1[256];
	px_data_t finalOut[17];
	#if not defined(FMAP_WIDEN)
		for(int i=0;i<512;i++){
			inPx[i] = IfMap[i];
		}
	#else
		for(int i=0;i<(512/FMAP_WIDTHFACTOR);i++){
			inPx[i*FMAP_WIDTHFACTOR+0] = IfMap[i].range(SYNTH_BITS-1,0);
			inPx[i*FMAP_WIDTHFACTOR+1] = IfMap[i].range(SYNTH_BITS*2-1,SYNTH_BITS);
			inPx[i*FMAP_WIDTHFACTOR+2] = IfMap[i].range(SYNTH_BITS*3-1,SYNTH_BITS*2);
			inPx[i*FMAP_WIDTHFACTOR+3] = IfMap[i].range(SYNTH_BITS*4-1,SYNTH_BITS*3);
			inPx[i*FMAP_WIDTHFACTOR+4] = IfMap[i].range(SYNTH_BITS*5-1,SYNTH_BITS*4);
			inPx[i*FMAP_WIDTHFACTOR+5] = IfMap[i].range(SYNTH_BITS*6-1,SYNTH_BITS*5);
			inPx[i*FMAP_WIDTHFACTOR+6] = IfMap[i].range(SYNTH_BITS*7-1,SYNTH_BITS*6);
		}
		inPx[511] = IfMap[73].range(SYNTH_BITS-1, 0);
	#endif


	fcLayer(inPx, WtMap, 512, 256, 0, outPx1);
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*256, minWt, maxWt);
		findMinMax(outPx1, 256, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "20) After fc1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetFC[0];
	fcLayer(outPx1, WtMap, 256, 17, 1, finalOut);
	#if not defined(FMAP_WIDEN)
		for(int i=0;i<17;i++){
			OfMap[i] = finalOut[i];
		}
	#else
		for(int i=0;i<my_ceil(17, FMAP_WIDTHFACTOR);i++){
			OfMap[i].range(SYNTH_BITS-1,0) 				= finalOut[i*7+0];
			OfMap[i].range(SYNTH_BITS*2-1,SYNTH_BITS) 	= finalOut[i*7+1];
			OfMap[i].range(SYNTH_BITS*3-1,SYNTH_BITS*2) = finalOut[i*7+2];
			OfMap[i].range(SYNTH_BITS*4-1,SYNTH_BITS*3) = finalOut[i*7+3];
			OfMap[i].range(SYNTH_BITS*5-1,SYNTH_BITS*4) = finalOut[i*7+4];
			OfMap[i].range(SYNTH_BITS*6-1,SYNTH_BITS*5) = finalOut[i*7+5];
			OfMap[i].range(SYNTH_BITS*7-1,SYNTH_BITS*6) = finalOut[i*7+6];
		}
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 256*CLASSES, minWt, maxWt);
		findMinMax(finalOut, CLASSES, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "21) After fc2: min=" << min << ", max=" << max << "\n";
		std::cout << "Finished second fc layer!" << std::endl;
	#endif
}


void fcLayers(
		// Inputs
		px_data_t *IfMap, wt_data_t *WtMap,
		// Output
		px_data_t finalOut[1000]
	){
	px_data_t outPx1[4096], outPx2[4096];
	// int WtMapOffsetFC[3] = {25088*4096, 4096*4096, 4096*1000};

	fcLayer(IfMap, WtMap, 25088, 4096, 0, outPx1);
	WtMap += WtMapOffsetFC[0];

	fcLayer(outPx1, WtMap, 4096, 4096, 1, outPx2);
	WtMap += WtMapOffsetFC[1];

	fcLayer(outPx2, WtMap, 4096, 1000, 2, finalOut);
}


void fcLayer(
		// Inputs
		px_data_t inPx[], const wt_data_t WtMap[],
		int inLength, int outLength, int layerNo,
		// Output
		px_data_t outPx[]
	){
		#pragma HLS INLINE
	// // Add below only, when it is synthesized
	// #pragma HLS INTERFACE m_axi depth=   512    port=inPx  		// offset=slave bundle=IFMAP
	// #pragma HLS INTERFACE m_axi depth=(512*256) port=WtMap 		// offset=slave bundle=WTMAP
	// #pragma HLS INTERFACE m_axi depth=   256    port=outPx 		// offset=slave bundle=OFMAP
	acc_data_t tmpOutPx;
	int mapAddress;
	mapAddress = 0;
	fclayer_loop_1: for(int i=0;i<outLength;i++){
		tmpOutPx = 0;
		fclayer_loop_2: for(int j=0;j<inLength;j++){
		#pragma HLS PIPELINE
			tmpOutPx += WtMap[mapAddress] * inPx[j];
			mapAddress++;
		}
		// tmpOutPx = tmpOutPx / (1 << bit_shift_fc_rom[layerNo]);
		tmpOutPx = tmpOutPx >> bit_shift_fc_rom[layerNo];
		tmpOutPx = tmpOutPx + biasFC[layerNo][i];
		if (tmpOutPx<0){
			tmpOutPx = 0;
		}
		outPx[i] = tmpOutPx;
	}
}

// void fcLayer(
	// 		// Inputs
	// 		px_data_t inPx[], wt_data_t WtMap[],
	// 		int inLength, int outLength, int layerNo,
	// 		// Output
	// 		px_data_t outPx[]
	// 	){
	// 	#pragma HLS INTERFACE m_axi port=inPx   bundle=IFMAP
	// 	#pragma HLS INTERFACE m_axi port=WtMap  bundle=WTMAP
	// 	#pragma HLS INTERFACE m_axi port=outPx  bundle=OFMAP
	//     #pragma HLS INTERFACE s_axilite port=inLength   bundle=control
	//     #pragma HLS INTERFACE s_axilite port=outLength  bundle=control
	//     #pragma HLS INTERFACE s_axilite port=layerNo    bundle=control
	//     #pragma HLS INTERFACE s_axilite port=return     bundle=control
	// 	// #pragma HLS INTERFACE m_axi port=inPx bundle=IFMAP
	// 	// #pragma HLS INTERFACE m_axi port=WtMap bundle=WTMAP
	// 	// #pragma HLS INTERFACE m_axi port=outPx bundle=OFMAP
	// 	acc_data_t tmpOutPx;
	// 	fclayer_loop_1: for(int i=0;i<outLength;i++){
	// 		tmpOutPx = 0;
	// 		fclayer_loop_2: for(int j=0;j<inLength;j++){
	// 			tmpOutPx += WtMap[i*inLength + j] * inPx[j];
	// 			// std::cout << "Address value:" << i*inLength + j << std::endl;
	// 			// std::cout << "j value:" << j << std::endl;
	// 			// std::cout << "WtMap value:" << WtMap[i*inLength + j] << std::endl;
	// 			// std::cout << "inPx value:" << inPx[j] << std::endl;
	// 			// std::cout << "tmpOutPx value:" << tmpOutPx << std::endl;
	// 			// std::cout << "\n\n";
	// 		}
	// 		// tmpOutPx = tmpOutPx / (1 << bit_shift_fc_rom[layerNo]);
	// 		// tmpOutPx = tmpOutPx >> bit_shift_fc_rom[layerNo];
	// 		tmpOutPx = tmpOutPx + biasFC[layerNo][i];
	// 		if (tmpOutPx<0){
	// 			tmpOutPx = 0;
	// 		}
	// 		// outPx[i] = tmpOutPx.range(31, 16);
	// 		outPx[i] = (px_data_t)tmpOutPx;
	// 	}
	// }

// void fcLayer(
    // px_data_t *inPx,
    // wt_data_t *WtMap,
    // int inLength,
    // int outLength,
    // int layerNo,
    // px_data_t *outPx
	// 	) {
	// 		#pragma HLS INTERFACE m_axi depth=512 port=inPx  // offset=slave bundle=IFMAP
	// 		#pragma HLS INTERFACE m_axi depth=512 port=WtMap // offset=slave bundle=WTMAP
	// 		#pragma HLS INTERFACE m_axi depth=512 port=outPx
	// 		// #pragma HLS INTERFACE s_axilite port=inLength   bundle=CTRL
	// 		// #pragma HLS INTERFACE s_axilite port=outLength  bundle=CTRL
	// 		// #pragma HLS INTERFACE s_axilite port=layerNo    bundle=CTRL
	// 		// #pragma HLS INTERFACE s_axilite port=return     bundle=CTRL

	// 		acc_data_t tmpOutPx;

	// 		fclayer_loop_1: for (int i = 0; i < outLength; i++) {
	// 			tmpOutPx = 0;
	// 			fclayer_loop_2: for (int j = 0; j < inLength; j++) {
	// 				tmpOutPx += (acc_data_t)WtMap[i*inLength + j] * (acc_data_t)inPx[j];
	// 			}

	// 			// tmpOutPx = tmpOutPx + biasFC[layerNo][i];

	// 			if (tmpOutPx < 0){
	// 				tmpOutPx = 0;
	// 			}
	// 			outPx[i] = (px_data_t)tmpOutPx;
	// 		}
	// 	}


void maxPool(
		// Inputs
		px_data_t *IfMap, int channels, int yDim_out, int xDim_out,
		// Output
		px_data_t *OfMap
	){
	px_data_t maxval, maxval1, maxval2;
	int yDim_in, xDim_in;
	yDim_in = yDim_out*2; xDim_in = xDim_out*2;
	px_data_t point1, point2, point3, point4;

	for(int c = 0; c < channels; c++){
		for(int h = 0; h < yDim_out; h++){
			for(int w = 0; w < xDim_out; w++){
				point1 = *(IfMap + c*yDim_in*xDim_in + h*2*xDim_in + w*2);
				point2 = *(IfMap + c*yDim_in*xDim_in + h*2*xDim_in + w*2+1);
				point3 = *(IfMap + c*yDim_in*xDim_in + (h*2+1)*xDim_in + w*2);
				point4 = *(IfMap + c*yDim_in*xDim_in + (h*2+1)*xDim_in + w*2+1);
				maxval1 = (point1>point2) ? point1 : point2;
				maxval2 = (point3>point4) ? point3 : point4;
				maxval = (maxval1 > maxval2) ? maxval1 : maxval2;
				*(OfMap + c*yDim_out*xDim_out + h*xDim_out + w) = maxval;
			}
		}
	}
}


// void maxpool2x2(
// 	const px_data_t *IfMap, int C, int H, int W,
// 	px_data_t *OfMap){
// 	#pragma HLS INTERFACE m_axi port=IfMap offset=slave bundle=gmem0 depth=FMAP_MEMSIZE
// 	#pragma HLS INTERFACE m_axi port=OfMap offset=slave bundle=gmem1 depth=(FMAP_MEMSIZE/4)
// 	#pragma HLS INTERFACE s_axilite port=IfMap bundle=control
// 	#pragma HLS INTERFACE s_axilite port=OfMap bundle=control
// 	#pragma HLS INTERFACE s_axilite port=H bundle=control
// 	#pragma HLS INTERFACE s_axilite port=W bundle=control
// 	#pragma HLS INTERFACE s_axilite port=C bundle=control
// 	#pragma HLS INTERFACE s_axilite port=return bundle=control

// 	int outH = H / 2;
// 	int outW = W / 2;

// 	for (int c = 0; c < C; ++c) {
// 		for (int y = 0; y < outH; ++y) {
// 			int in_y = y * 2;
// 			for (int x = 0; x < outW; ++x) {
// 			#pragma HLS PIPELINE II=4
// 				int in_x = x * 2;

// 				int base = (c * H + in_y) * W + in_x;
// 				px_data_t a = IfMap[base];
// 				px_data_t b = IfMap[base + 1];
// 				px_data_t c0 = IfMap[base + W];
// 				px_data_t d = IfMap[base + W + 1];

// 				px_data_t m1 = (a > b) ? a : b;
// 				px_data_t m2 = (c0 > d) ? c0 : d;
// 				px_data_t mout = (m1 > m2) ? m1 : m2;

// 				int out_index = (c * outH + y) * outW + x;
// 				OfMap[out_index] = mout;
// 			}
// 		}
// 	}
// }


// void maxpool2x2(const px_data_t *IfMap, ap_uint<10> C,
//               ap_uint<8> H, ap_uint<8> W, px_data_t *OfMap){
// 	#pragma HLS INTERFACE m_axi port=IfMap offset=slave bundle=gmem0 depth=FMAP_MEMSIZE
// 	#pragma HLS INTERFACE m_axi port=OfMap offset=slave bundle=gmem1 depth=(FMAP_MEMSIZE/4)
// 	#pragma HLS INTERFACE s_axilite port=IfMap bundle=control
// 	#pragma HLS INTERFACE s_axilite port=OfMap bundle=control
// 	#pragma HLS INTERFACE s_axilite port=H bundle=control
// 	#pragma HLS INTERFACE s_axilite port=W bundle=control
// 	#pragma HLS INTERFACE s_axilite port=C bundle=control
// 	#pragma HLS INTERFACE s_axilite port=return bundle=control

// 	ap_uint<7> outH = H / 2;
// 	ap_uint<7> outW = W / 2;
// 	int row0_addr = 0;
// 	int row1_addr = 0;
// 	ap_uint<21> offset0;
// 	#pragma HLS BIND_OP variable=offset0 op=mul impl=fabric
// 	offset0 = H * W;
// 	ap_uint<19> offset1 = H * W / 4;
// 	#pragma HLS BIND_OP variable=offset1 op=mul impl=fabric
// 	offset1 = outH * outW;
// 	ap_uint<11> addr0, addr0W;
// 	ap_uint<11> addr1;
// 	Channel: for (int c = 0; c < C; ++c) {
// 		addr0  = 0;
// 		addr0W = W;
// 		addr1  = 0;
// 		Ydim: for (int y = 0; y < outH; ++y) {
// 			Xdim: for (int x = 0; x < outW; ++x) {
// 			#pragma HLS PIPELINE II=4
// 				px_data_t a  = IfMap[row0_addr + addr0];
// 				px_data_t b  = IfMap[row0_addr + addr0 + 1];
// 				px_data_t c0 = IfMap[row0_addr + addr0W];
// 				px_data_t d  = IfMap[row0_addr + addr0W + 1];
// 				px_data_t m1 = ( a>b) ?  a: b;
// 				px_data_t m2 = (c0>d) ? c0: d;
// 				OfMap[row1_addr + addr1] = (m1>m2) ? m1 : m2;

// 				addr0 += 2; // move to next 2x2 window
// 				addr1++;
// 			}
// 			addr0 += W; addr0W += W;
// 		}
// 		row0_addr += offset0;
// 		row1_addr += offset1;
// 	}
// }

#if (not defined(FMAP_WIDEN) && not defined(WTMAP_WIDEN))
void maxpool2x2(const px_data_t *IfMap, ap_uint<10> C,
				ap_uint<8> H, ap_uint<8> W, px_data_t *OfMap){
	#pragma HLS INTERFACE m_axi port=IfMap offset=slave bundle=gmem0 depth=FMAP_MEMSIZE
	#pragma HLS INTERFACE m_axi port=OfMap offset=slave bundle=gmem1 depth=(FMAP_MEMSIZE/4)
	#pragma HLS INTERFACE s_axilite port=IfMap bundle=control
	#pragma HLS INTERFACE s_axilite port=OfMap bundle=control
	#pragma HLS INTERFACE s_axilite port=H bundle=control
	#pragma HLS INTERFACE s_axilite port=W bundle=control
	#pragma HLS INTERFACE s_axilite port=C bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	ap_uint<7> outH = H / 2;
	ap_uint<7> outW = W / 2;
	ap_uint<21> addr0, addr0W;
	ap_uint<21> addr1;
	addr0  = 0;
	addr0W = W;
	addr1  = 0;
	Channel: for (int c = 0; c < C; ++c) {
		Ydim: for (int y = 0; y < outH; ++y) {
			Xdim: for (int x = 0; x < outW; ++x) {
			#pragma HLS PIPELINE II=4
				px_data_t a  = IfMap[addr0];
				px_data_t b  = IfMap[addr0 + 1];
				px_data_t c = IfMap[addr0W];
				px_data_t d  = IfMap[addr0W + 1];
				px_data_t m1 = ( a>b) ?  a: b;
				px_data_t m2 = ( c>d) ?  c: d;
				OfMap[addr1] = (m1>m2) ? m1 : m2;

				addr0 += 2; // move to next 2x2 window
				addr1++;
			}
			addr0 += W; addr0W += W;
		}
	}
}
#endif


void tlModelTop(px_data_t *Map1, wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *Map2, px_data_t finalOut[17]
	){
	#ifndef __SYNTHESIS__
		int min, max, minWt, maxWt;
		findMinMax(Map1, 3*224*224, min, max);
		std::cout << "Input Range : min=" << min << " ,max=" << max << "\n";
	#endif
	
	// First Conv. Block
	convChoice(Map1, WtMap, Map2, 0);
	WtMap += WtMapOffsetConv[0];
	convChoice(Map2, WtMap, Map1, 1);
	#if defined(MAXPOOL_INTEGRATION)
		swapPointers(Map1, Map2);
	#else
		maxPool(Map1, 64, 112, 112, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map2, 64*112*112, min, max);
		std::cout << "After maxpool1: min=" << min << ", max=" << max << "\n";
	#endif

	// Second Conv. Block
	WtMap += WtMapOffsetConv[1];
	convChoice(Map2, WtMap, Map1, 2);
	WtMap += WtMapOffsetConv[2];
	convChoice(Map1, WtMap, Map2, 3);
	#if defined(MAXPOOL_INTEGRATION)
		swapPointers(Map2, Map1);
	#else
		maxPool(Map2, 128, 56, 56, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 128*56*56, min, max);
		std::cout << "After maxpool2: min=" << min << ", max=" << max << "\n";
	#endif

	// Third Conv. Block
	WtMap += WtMapOffsetConv[3];
	convChoice(Map1, WtMap, Map2, 4);
	WtMap += WtMapOffsetConv[4];
	convChoice(Map2, WtMap, Map1, 5);
	WtMap += WtMapOffsetConv[5];
	convChoice(Map1, WtMap, Map2, 6);
	#if defined(MAXPOOL_INTEGRATION)
		swapPointers(Map2, Map1);
	#else
		maxPool(Map2, 256, 28, 28, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 256*28*28, min, max);
		std::cout << "After maxpool3: min=" << min << ", max=" << max << "\n";
	#endif

	// Fourth Conv. Block
	WtMap += WtMapOffsetConv[6];
	convChoice(Map1, WtMap, Map2, 7);
	WtMap += WtMapOffsetConv[7];
	convChoice(Map2, WtMap, Map1, 8);
	WtMap += WtMapOffsetConv[8];
	convChoice(Map1, WtMap, Map2, 9);
	#if defined(MAXPOOL_INTEGRATION)
		swapPointers(Map2, Map1);
	#else
		maxPool(Map2, 512, 14, 14, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 512*14*14, min, max);
		std::cout << "After maxpool4: min=" << min << ", max=" << max << "\n";
	#endif

	// Fifth Conv. Block
	WtMap += WtMapOffsetConv[9];
	convChoice(Map1, WtMap, Map2, 10);
	WtMap += WtMapOffsetConv[10];
	convChoice(Map2, WtMap, Map1, 11);
	WtMap += WtMapOffsetConv[11];
	convChoice(Map1, WtMap, Map2, 12);
	#if defined(MAXPOOL_INTEGRATION)
		swapPointers(Map2, Map1);
	#else
		maxPool(Map2, 512, 7, 7, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 512*7*7, min, max);
		std::cout << " After maxpool5: min=" << min << ", max=" << max << "\n";
	#endif

	#if defined(HEAD_INTEGRATION)
		swapPointers(Map1, Map2);
	#else
		gap(Map1, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(Map2, 512, min, max);
		std::cout << "19) After gap: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[12];
	#if not defined(CONVX)
		fcLayersOF(Map2, WtMap, finalOut);
	#else
		fcChoice(Map2, WtMap, finalOut);
	#endif
	#ifndef __SYNTHESIS__
		std::cout << "Finished all fc layers!" << std::endl;
	#endif
	
}


void convChoice(px_data_t *IfMap, const wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *OfMap, int layerNo){
	#if defined(WTMAP_WIDEN)
		wt_data_t* WtMap_reordered = nullptr;
		WtMap_reordered = new wt_data_t[WTMAP_MEMSIZE];
		wt_data_t_port* WtMap_port = nullptr;
		WtMap_port = new wt_data_t_port[WTMAP_MEMSIZE_WIDENED];
	#else
		const wt_data_t_port* WtMap_port;
	#endif
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#ifndef __SYNTHESIS__
		int min, max, minWt, maxWt;
	#endif


	#if defined(WTMAP_WIDEN)
		wt_reorder(WtMap, WtMap_reordered, layerNo);
		pack<wt_data_t_port>(WtMap_reordered, WtMap_port, WTMAP_WIDTHFACTOR, WTMAP_MEMSIZE_WIDENED);
	#else
		WtMap_port = WtMap;
	#endif
	#if defined(FMAP_WIDEN)
		pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
		#if not defined(CONVX)
			ConvLayer(IfMap_port, WtMap_port, OfMap_port);
		#else
			wt_data_t *WtMapFC = nullptr;
			ConvX(IfMap_port, WtMap_port, WtMapFC, OfMap_port);
		#endif
		unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, FMAP_MEMSIZE_WIDENED);
	#else
		#if not defined(CONVX)
			ConvLayer(IfMap, WtMap_port, OfMap);
		#else
			wt_data_t *WtMapFC = nullptr;
			ConvX(IfMap, WtMap_port, WtMapFC, OfMap);
		#endif
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, Nif_rom[layerNo]*Nof_step_rom[layerNo]*Tof_rom[layerNo]*NKY*NKX, minWt, maxWt);
		findMinMax(OfMap, Nof_step_rom[layerNo]*Tof_rom[layerNo]*Tox_rom[layerNo]*Noy_rom[layerNo], min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "Conv" << layerNo << ": min=" << min << ", max=" << max << "\n";
	#endif

	#if defined(WTMAP_WIDEN)
		delete[] WtMap_reordered;
		WtMap_reordered = nullptr;
		delete[] WtMap_port;
		WtMap_port = nullptr;
	#endif
}


void fcChoice(px_data_t *IfMap, const wt_data_t *WtMapFC, 	// [NOF][NIF][NKY][NKX]
		px_data_t *OfMap){
	// Only for ConvX case
	#if defined(FMAP_WIDEN)
		static px_data_t_port IfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
		static px_data_t_port OfMap_port[FMAP_MEMSIZE_WIDENED] = {0};
	#endif
	#if defined(FMAP_WIDEN)
		// pack<px_data_t_port>(IfMap, IfMap_port, FMAP_WIDTHFACTOR, 74);
		for(int i=0;i<73;i++){
			for(int factor_i=0;factor_i<7;factor_i++){
				IfMap_port[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i) = 
					IfMap[i*7+factor_i];
			}
		}
		IfMap_port[74].range(SYNTH_BITS*6-1,SYNTH_BITS) = 0;
		IfMap_port[74].range(SYNTH_BITS-1,0) = IfMap[511];
		wt_data_t_port *WtMap = nullptr;
		ConvX(IfMap_port, WtMap, WtMapFC, OfMap_port);
		std::cout << "REached Here77!" << std::endl;
		// unpack<px_data_t_port>(OfMap_port, OfMap, FMAP_WIDTHFACTOR, 3);
		for(int i=0;i<2;i++){
			for(int factor_i=0;factor_i<7;factor_i++){
				OfMap[i*7+factor_i] = 
					OfMap_port[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i);
			}
		}
		OfMap[14] = OfMap_port[2].range(SYNTH_BITS-1,0);
		OfMap[15] = OfMap_port[2].range(SYNTH_BITS*2-1,SYNTH_BITS);
		OfMap[16] = OfMap_port[2].range(SYNTH_BITS*3-1,SYNTH_BITS*2);
	#else
		wt_data_t_port *WtMap = nullptr;
		ConvX(IfMap, WtMap, WtMapFC, OfMap);
	#endif
}


void swapPointers(px_data_t *&a, px_data_t *&b){
    px_data_t *temp = a;
    a = b;
    b = temp;
}