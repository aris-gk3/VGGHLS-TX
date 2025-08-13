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
	px_data_t pxSerial[OUTBUF_NUM][POX];
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


void loadIfMap(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		data_bool northTile_in, data_bool southTile_in,
		Niy_dt yBase_in, const px_data_t_port IfMap[IFMAP_MEMSIZE], //[NIF][NIX-2*ZERO_PAD][NIY-2*ZERO_PAD]
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
				#if defined(IFMAP_FACTOR7)
					wrdX = 0;
					// std::cout << "calling if_nix" << Poy_i << std::endl;
					If_Nix: for(int xTile_i=0;xTile_i<32;xTile_i++){
					#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
						wrd_i = wrdMap + wrdY + wrdX;
						InBuf[Poy_i][wrd_i][1] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(31,0);
						InBuf[Poy_i][wrd_i][2] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(63,32);
						InBuf[Poy_i][wrd_i][3] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(95,64);
						InBuf[Poy_i][wrd_i][4] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(127,96);
						InBuf[Poy_i][wrd_i][5] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(159,128);
						InBuf[Poy_i][wrd_i][6] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(191,160);
						InBuf[Poy_i][wrd_i+1][0] = IfMap[Nif_i*(Niy)*(Tix-2)/7 + (yBase+yTile_i)*(Tix-2)/7 + xTile_i].range(223,192);
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


void loadWtMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ Nofy_step_dt ofBase, const wt_data_t WtMap[WTMAP_MEMSIZE],
		/* Output */ wt_data_t WtBuf[WRD_WTBUF][POF]
	){
	#pragma HLS INLINE off
	#pragma HLS INTERFACE m_axi port=WtMap max_widen_bitwidth=256
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


void storeMap(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		// Output
		px_data_t *OfMap
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
				Loop_Tox: for(int Tox_i=0; Tox_i<Tox;Tox_i++){
				#pragma HLS LOOP_TRIPCOUNT min=TOX_TRIPCOUNT max=TOX_TRIPCOUNT
					*(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) =
							OutBuf[OutBufNum_i][wrdMap + wrdY + wrdX][Pox_i];
					if( *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) >= (1 << HW_EMUL_NUM_BITS)){
						#ifdef DEBUG_MODE
							std::cout << "Value out of range found in " << (Tof_i+ofBase*Tof) << " Map, "
								<< (Toy_i+yBase*Toy) << " Row, " << Tox_i << " Column\n";
							std::cout << "Value is " << *(OfMap + (Tof_i+ofBase*Tof)*Noy*Tox + (Toy_i+yBase*Toy)*Tox +Tox_i) << "\n";
						#endif
					}
					if(Pox_i == POX-1){
						Pox_i = 0;
						wrdX++;
					}
					else{
						Pox_i++;
					}
				}
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


void mem2Buf(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		const px_data_t_port IfMap[IFMAP_MEMSIZE],
		const wt_data_t WtMap[WTMAP_MEMSIZE],
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


void ConvLayer_Dfl(
		// Parameter Loading State
		data_bool layerCnfg,
		int loop_limit,
		// Intermediate (Buffered) Data
		px_data_t InBuf1[POY][WRD_INBUF][POX], px_data_t InBuf2[POY][WRD_INBUF][POX],
		wt_data_t WtBuf1[WRD_WTBUF][POF], wt_data_t WtBuf2[WRD_WTBUF][POF],
		px_data_t OutBuf1[OUTBUF_NUM][WRD_OUTBUF][POX], px_data_t OutBuf2[OUTBUF_NUM][WRD_OUTBUF][POX],
		b_data_t BiasBuf1[BIASBUF_LENGTH], b_data_t BiasBuf2[BIASBUF_LENGTH],
		// Inputs
		px_data_t_port IfMap[IFMAP_MEMSIZE],
		wt_data_t WtMap[WTMAP_MEMSIZE],
		// Output
		px_data_t OfMap[OFMAP_MEMSIZE]
	){
	#pragma HLS INLINE
	mem2Buf(/*layerCnfg=*/layerCnfg,
			/*normal operation*/ IfMap, WtMap, InBuf1, WtBuf1);
	loadBiasTile(/*layerCnfg=*/layerCnfg, /*normal operation*/ BiasBuf1);
	tileClc(/*layerCnfg=*/layerCnfg,
			/*normal operation*/ InBuf1, WtBuf1, BiasBuf1, OutBuf1);
	storeMap(/*layerCnfg=*/layerCnfg, /*normal operation*/ OutBuf1, OfMap);

	mem2Buf(/*layerCnfg=*/layerCnfg,
			/*normal operation*/ IfMap, WtMap, InBuf2, WtBuf2);
	loadBiasTile(/*layerCnfg=*/layerCnfg, /*normal operation*/ BiasBuf2);
	tileClc(/*layerCnfg=*/layerCnfg,
			/*normal operation*/ InBuf2, WtBuf2, BiasBuf2, OutBuf2);
	storeMap(/*layerCnfg=*/layerCnfg, /*normal operation*/ OutBuf2, OfMap);
}


void ConvLayer(
		//Inputs
		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		const wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		//Output
		px_data_t *OfMap 	// [NOF][NOY][NOX]
	){
	#pragma HLS INTERFACE m_axi port=IfMap depth=MAP_SIZE
	#pragma HLS INTERFACE m_axi port=WtMap depth=WTMAP_MEMSIZE
	#pragma HLS INTERFACE m_axi port=OfMap depth=MAP_SIZE
	static layerNo_dt layerNo = 0; 				// State of number of convolutional layer

	// Intermediate (Buffered) Data
	static px_data_t InBuf[POY][WRD_INBUF][POX];
	#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
	#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
	static wt_data_t WtBuf[WRD_WTBUF][POF];
	#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
	static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
	#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
	#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
	b_data_t BiasBuf[BIASBUF_LENGTH];
	#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf

	mem2Buf(/*layerCnfg=*/1,
			/*normal operation*/ IfMap, WtMap, InBuf, WtBuf);
	#ifndef __SYNTHESIS__
		std::cout << "Loaded parameters in mem2Buf, layer:" << layerNo << std::endl;
	#endif
	loadBiasTile(/*layerCnfg=*/1, /*normal operation*/ BiasBuf);
	#ifndef __SYNTHESIS__
		std::cout << "Loaded parameters in loadBiasTile, layer:" << layerNo << std::endl;
	#endif
	tileClc(/*layerCnfg=*/1,
			/*normal operation*/ InBuf, WtBuf, BiasBuf, OutBuf);
	#ifndef __SYNTHESIS__
		std::cout << "Loaded parameters in tileClc, layer:" << layerNo << std::endl;
	#endif
	storeMap(/*layerCnfg=*/1, /*normal operation*/ OutBuf, OfMap);
	#ifndef __SYNTHESIS__
		std::cout << "Loaded parameters, layer:" << layerNo << std::endl;
	#endif
	data_bool layerCnfg = 0;

	Nofy_step_loop: for(int i=0;i<nofy_step_rom[layerNo];i++){
	#pragma HLS LOOP_TRIPCOUNT min=NOFYSTEP_TRIPCOUNT max=NOFYSTEP_TRIPCOUNT
		mem2Buf(/*layerCnfg=*/layerCnfg,
				/*normal operation*/ IfMap, WtMap, InBuf, WtBuf);

		// std::cout << "***  Printing Input Buffers  ***" << std::endl;
		// for(int Wrd_InBuf_i=0;Wrd_InBuf_i<50;Wrd_InBuf_i++){
		// 	// std::cout << std::setw(4) << Wrd_InBuf_i << " :   "; // Print line number for debugging
		// 	for(int Poy_i=0;Poy_i<POY;Poy_i++){
		// 		for(int Pox_i=0;Pox_i<POX;Pox_i++){
		// 			std::cout << std::setw(4) << InBuf[Poy_i][Wrd_InBuf_i][Pox_i] << "  ";
		// 		}
		// 		std::cout << "|  ";
		// 	}
		// 	std::cout << " " << std::endl;
		// }
		
		#ifndef __SYNTHESIS__
			std::cout << "Finished mem2Buf tile:" << i << std::endl;
		#endif	
		loadBiasTile(/*layerCnfg=*/layerCnfg, /*normal operation*/ BiasBuf);
		#ifdef DEBUG_PRINTS
			std:: cout << "Below is the synth bias array" << std::endl;
			for(int counter=0;counter<256;counter++){
				if(counter%16==0){
					std::cout << std::endl;
				}
				std:: cout << std::setw(5) << BiasBuf[counter];
			}
		#endif
		#ifndef __SYNTHESIS__
			std::cout << "Finished loadBiasTile tile:" << i << std::endl;
		#endif
		tileClc(/*layerCnfg=*/layerCnfg,
				/*normal operation*/ InBuf, WtBuf, BiasBuf, OutBuf);
		#ifndef __SYNTHESIS__
			std::cout << "Finished tileClc tile:" << i << std::endl;
		#endif
		storeMap(/*layerCnfg=*/layerCnfg, /*normal operation*/ OutBuf, OfMap);
		#ifndef __SYNTHESIS__
			std::cout << "Finished tile:" << i << std::endl;
		#endif
	}

	if(layerNo == LAYERS - 1){
		layerNo = 0;
	}
	else{
		layerNo++;
	}
}


/* Not used
void ConvLayer_module(data_bool layerCnfg, int test, int loop_limit_1, int loop_limit_2,
		px_data_t *IfMap,  // [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		wt_data_t *WtMap,  // [NOF][NIF][NKY][NKX]
		px_data_t *OfMap){ // [NOF][NOY][NOX]
	static ap_uint<4> layerNo = 0; // "State" for layer
	static ap_uint<WND_LOOP_B> wndclc_loop_limit;   // Parameters stored locally (1)
	static ap_uint<TILE_LOOP_B> tileclc_loop_limit; // Parameters stored locally (2)
	static ap_uint<NOY_STEP_B> Noy_step; 			// Parameters stored locally (3)
	static ap_uint<TIY_B> Tiy; 						// Parameters stored locally (4)

	data_bool northTile, southTile; // Flags for tile position (for zero padding)
	ap_uint<NIY_B> yBase;           // y offset address for tile of IfMap to be loaded
	ap_uint<NOF_STEP_I_B> ofBase; 	// map offset address for tile of OfMap to be stored

	// Intermediate (Buffered) Data
	static px_data_t InBuf[POY][WRD_INBUF][POX];
	#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=1
	#pragma HLS ARRAY_PARTITION variable=InBuf complete dim=3
	static wt_data_t WtBuf[WRD_WTBUF][POF];
	#pragma HLS ARRAY_PARTITION variable=WtBuf complete dim=2
	static px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX];
	#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=1
	#pragma HLS ARRAY_PARTITION variable=OutBuf complete dim=3
	b_data_t BiasBuf[BIASBUF_LENGTH];
	#pragma HLS ARRAY_PARTITION dim=1 factor=2 type=cyclic variable=BiasBuf

	// Normal tile operations
	yBase = 0;
	if(layerCnfg){
		wndclc_loop_limit = 1;
		tileclc_loop_limit = 1;
	}
	else if(test == 1){
		loop_limit_1 = 0;
		loop_limit_2 = 0;
		wndclc_loop_limit = 1;
		tileclc_loop_limit = 1;
		Tiy = tiy_rom[layerNo];
	}
	else{
		wndclc_loop_limit = wndclc_loop_limit_rom[layerNo];
		tileclc_loop_limit = tileclc_loop_limit_rom[layerNo];
		Tiy = tiy_rom[layerNo];
		if(layerNo==LAYERS-1){
			layerNo = 0;
		}
		else{
			layerNo++;
		}
	}
	for(int Noy_step_i=0;Noy_step_i<loop_limit_1;Noy_step_i++){
	#pragma HLS LOOP_TRIPCOUNT min=2 max=32
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
		if(Noy_step_i == 0){
			yBase = 0;
		}
		else{
			yBase = Noy_step_i*(Tiy - 2) -1;
		}
		loadIfMap(layerCnfg , northTile, southTile, yBase, IfMap, InBuf);
		// For steps in layer
		for(int Nof_step_i=0;Nof_step_i<loop_limit_2;Nof_step_i++){
		#pragma HLS LOOP_TRIPCOUNT min=2 max=64
			ofBase = Nof_step_i;
			loadWtMap(layerCnfg , ofBase, WtMap, WtBuf);
			loadBiasTile(layerCnfg, BiasBuf);
		}
	}
}

*/


void gap(px_data_t *in, px_data_t *out){
	// int input[512 * 7 * 7]; // CHW format
	// int output[512];        // GAP output
	ap_int<16> reciprocal = (1 << 16) / (7 * 7); // fixed-point reciprocal

	gap_c: for (int c = 0; c < 512; c++) {
		acc_data_t sum = 0;
		int offset = c * 7 * 7;
		gap_xy: for (int i = 0; i < 7 * 7; i++) {
			sum += in[offset + i];
		}
		out[c] = (sum * reciprocal) >> 16; // Tranformed division to mul and right bit shift
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
		px_data_t *inPx, wt_data_t *WtMap,
		int inLength, int outLength, int layerNo,
		// Output
		px_data_t *outPx
	){
	fclayer_loop_1: for(int i=0;i<outLength;i++){
		outPx[i] = 0;
		fclayer_loop_2: for(int j=0;j<inLength;j++){
			outPx[i] += *(WtMap + i*inLength + j) * inPx[j];
		}
		outPx[i] = outPx[i] / (1 << bit_shift_fc_rom[layerNo]);
		outPx[i] = outPx[i] + biasFC[layerNo][i];
		if (outPx[i]<0){
			outPx[i] = 0;
		}
	}
}


void maxPool(
		// Inputs
		px_data_t *IfMap, int channels, int yDim_out, int xDim_out,
		// Output
		px_data_t *OfMap
	){
	px_data_t maxval, maxval1, maxval2;
	int yDim_in, xDim_in;
	yDim_in = yDim_out*2; xDim_in = xDim_out*2;
	int point1, point2, point3, point4;

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


void vgg16Top(px_data_t *Map1, wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *Map2, px_data_t finalOut[1000]
	){
	// Outside controller calls this function once, instead
	// calling as many times as the layers.
	#if defined(IFMAP_FACTOR7)
		static px_data_t_port IfMap_port[MAP_SIZE] = {0};
	#endif

	// Block1
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[0];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	WtMap += WtMapOffsetConv[1];
	maxPool(Map1, 64, 112, 112, Map2);

	// Block2
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	WtMap += WtMapOffsetConv[2];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[3];
	maxPool(Map2, 128, 56, 56, Map1);

	// Block3
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[4];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	WtMap += WtMapOffsetConv[5];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[6];
	maxPool(Map2, 256, 28, 28, Map1);

	// Block4
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[7];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	WtMap += WtMapOffsetConv[8];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[9];
	maxPool(Map2, 512, 14, 14, Map1);

	// Block5
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[10];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	WtMap += WtMapOffsetConv[11];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	WtMap += WtMapOffsetConv[12];
	maxPool(Map2, 512, 7, 7, Map1);

	fcLayers(Map1, WtMap, finalOut);
}


void tlModelTop(px_data_t *Map1, wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *Map2, px_data_t finalOut[17]
	){
	#if defined(IFMAP_FACTOR7)
		static px_data_t_port IfMap_port[MAP_SIZE] = {0};
	#endif
	#ifndef __SYNTHESIS__
		int min, max, minWt, maxWt;
		findMinMax(Map1, 3*224*224, min, max);
		std::cout << "Input Range : min=" << min << " ,max=" << max << "\n";
	#endif
	
	// First Conv. Block
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 3*64*3*3, minWt, maxWt);
		findMinMax(Map2, 64*224*224, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "01) After block1_conv1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[0];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 64*64*3*3, minWt, maxWt);
		findMinMax(Map1, 64*224*224, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "02) After block1_conv2: min=" << min << ", max=" << max << "\n";
	#endif
	
	maxPool(Map1, 64, 112, 112, Map2);
	#ifndef __SYNTHESIS__
		findMinMax(Map2, 64*112*112, min, max);
		std::cout << "03) After maxpool1: min=" << min << ", max=" << max << "\n";
	#endif

	// Second Conv. Block
	WtMap += WtMapOffsetConv[1];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 64*128*3*3, minWt, maxWt);
		findMinMax(Map1, 128*112*112, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "04) After block2_conv1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[2];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 128*128*3*3, minWt, maxWt);
		findMinMax(Map2, 128*112*112, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "05) After block2_conv2: min=" << min << ", max=" << max << "\n";
	#endif

	maxPool(Map2, 128, 56, 56, Map1);
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 128*56*56, min, max);
		std::cout << "06) After maxpool2: min=" << min << ", max=" << max << "\n";
	#endif

	// Third Conv. Block
	WtMap += WtMapOffsetConv[3];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 128*256*3*3, minWt, maxWt);
		findMinMax(Map2, 256*56*56, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "07) After block3_conv1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[4];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 256*256*3*3, minWt, maxWt);
		findMinMax(Map1, 256*56*56, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "08) After block3_conv2: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[5];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 256*256*3*3, minWt, maxWt);
		findMinMax(Map2, 256*56*56, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "09) After block3_conv3: min=" << min << ", max=" << max << "\n";
	#endif

	maxPool(Map2, 256, 28, 28, Map1);
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 256*28*28, min, max);
		std::cout << "10) After maxpool3: min=" << min << ", max=" << max << "\n";
	#endif

	// Fourth Conv. Block
	WtMap += WtMapOffsetConv[6];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 256*512*3*3, minWt, maxWt);
		findMinMax(Map2, 512*28*28, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "11) After block4_conv1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[7];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*512*3*3, minWt, maxWt);
		findMinMax(Map1, 512*28*28, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "12) After block4_conv2: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[8];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*512*3*3, minWt, maxWt);
		findMinMax(Map2, 512*28*28, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "13) After block4_conv3: min=" << min << ", max=" << max << "\n";
	#endif

	maxPool(Map2, 512, 14, 14, Map1);
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 512*14*14, min, max);
		std::cout << "14) After maxpool4: min=" << min << ", max=" << max << "\n";
	#endif

	// Fifth Conv. Block
	WtMap += WtMapOffsetConv[9];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*512*3*3, minWt, maxWt);
		findMinMax(Map2, 512*14*14, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "15) After block5_conv1: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[10];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map2, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map1);
	#else
		ConvLayer(Map2, WtMap, Map1);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*512*3*3, minWt, maxWt);
		findMinMax(Map1, 512*14*14, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "16) After block5_conv2: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[11];
	#if defined(IFMAP_FACTOR7)
		pack<px_data_t_port>(Map1, IfMap_port, IFMAP_WIDTHFACTOR, IFMAP_MEMSIZE_WIDENED);
		ConvLayer(IfMap_port, WtMap, Map2);
	#else
		ConvLayer(Map1, WtMap, Map2);
	#endif
	#ifndef __SYNTHESIS__
		findMinMax(WtMap, 512*512*3*3, minWt, maxWt);
		findMinMax(Map2, 512*14*14, min, max);
		std::cout << "Weights: min=" << minWt << ", max=" << maxWt << "\n";
		std::cout << "17) After block5_conv3: min=" << min << ", max=" << max << "\n";
	#endif

	maxPool(Map2, 512, 7, 7, Map1);
	#ifndef __SYNTHESIS__
		findMinMax(Map1, 512*7*7, min, max);
		std::cout << "18) After maxpool5: min=" << min << ", max=" << max << "\n";
	#endif

	gap(Map1, Map2);
	#ifndef __SYNTHESIS__
		findMinMax(Map2, 512, min, max);
		std::cout << "19) After gap: min=" << min << ", max=" << max << "\n";
	#endif

	WtMap += WtMapOffsetConv[12];
	fcLayersOF(Map2, WtMap, finalOut);
	#ifndef __SYNTHESIS__
		std::cout << "Finished all fc layers!" << std::endl;
	#endif
	
}

