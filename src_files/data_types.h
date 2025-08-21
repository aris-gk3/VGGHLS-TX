#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <ap_fixed.h>
//#include <ap_float.h>
#include "parameters.h"
#define DATA_TYPE 3
// 0 -> all int
// 1 -> 8,16-bit integer for data
// 2 -> 16,32-bit integer for data
// 3 -> ap_int<32> for data
// 4 -> 16-bit fixed integr for data
#define DATA_TYPE_PAR 0
// Data types for parameters and internal variables, counters
// 0 -> all int
// 1 -> Just enough arbitrary precision, manually calculated

#if (DATA_TYPE == 0)
	constexpr int SYNTH_BITS=32;
	typedef int px_data_t;
	#if defined(FMAP_WIDEN)
		typedef ap_int<224> px_data_t_port;
	#else
		typedef px_data_t px_data_t_port;
	#endif
	typedef int acc_data_t;
	typedef int wt_data_t;
	#if defined(WTMAP_WIDEN)
		typedef ap_int<(32*WTMAP_WIDTHFACTOR)> wt_data_t_port;
	#else
		typedef wt_data_t wt_data_t_port;
	#endif
	typedef int b_data_t;
	typedef int bfc_data_t;
	typedef int mul_data_t;
	typedef int acc_data_t;
#elif (DATA_TYPE == 1)
	constexpr int SYNTH_BITS=8;
	typedef ap_int<8> px_data_t;
	#if defined(FMAP_WIDEN)
		typedef ap_int<56> px_data_t_port;
	#else
		typedef px_data_t px_data_t_port;
	#endif
	typedef ap_int<18> acc_data_t;
	typedef ap_int<8> wt_data_t;
	#if defined(WTMAP_WIDEN)
		typedef ap_int<(SYNTH_BITS*WTMAP_WIDTHFACTOR)> wt_data_t_port;
	#else
		typedef wt_data_t wt_data_t_port;
	#endif
	typedef ap_int<8> b_data_t;
	typedef ap_int<8> bfc_data_t;
	typedef ap_int<18> mul_data_t;
	typedef ap_int<18> acc_data_t;
#elif (DATA_TYPE == 2)
	constexpr int SYNTH_BITS=16;
	typedef ap_int<16> px_data_t;
	#if defined(FMAP_WIDEN)
		typedef ap_int<112> px_data_t_port;
	#else
		typedef px_data_t px_data_t_port;
	#endif
	typedef ap_int<32> acc_data_t;
	typedef ap_int<16> wt_data_t;
	#if defined(WTMAP_WIDEN)
		typedef ap_int<(SYNTH_BITS*WTMAP_WIDTHFACTOR)> wt_data_t_port;
	#else
		typedef wt_data_t wt_data_t_port;
	#endif
	typedef ap_int<16> b_data_t;
	typedef ap_int<16> bfc_data_t;
	typedef ap_int<32> mul_data_t;
	typedef ap_int<32> acc_data_t;
#elif (DATA_TYPE == 3)
	constexpr int SYNTH_BITS=32;
	typedef ap_int<32> px_data_t;
	#if defined(FMAP_WIDEN)
		typedef ap_int<224> px_data_t_port;
	#else
		typedef px_data_t px_data_t_port;
	#endif
	typedef ap_int<32> acc_data_t;
	typedef ap_int<32> wt_data_t;
	#if defined(WTMAP_WIDEN)
		typedef ap_int<(SYNTH_BITS*WTMAP_WIDTHFACTOR)> wt_data_t_port;
	#else
		typedef wt_data_t wt_data_t_port;
	#endif
	typedef ap_int<32> b_data_t;
	typedef ap_int<32> bfc_data_t;
	typedef ap_int<32> mul_data_t;
	typedef ap_int<32> acc_data_t;
#endif // Data types

// Parameters
#if (DATA_TYPE_PAR == 0)
	typedef int data_bool;
	typedef int layerNo_dt;
	typedef int Nky_dt;
	typedef int Nkx_dt;

	typedef int Nif_dt;
	typedef int Nif_i_dt;
	typedef int Niy_dt;
	typedef int Noy_dt;
	typedef int Nof_step_dt;
	typedef int Nof_step_i_dt;
	typedef int Noy_step_dt;
	typedef int Noy_step_i_dt;
	typedef int Nofy_step_dt;
	typedef int Nofy_step_i_dt;

	typedef int Tiy_dt;
	typedef int Tix_dt;
	typedef int Tof_dt;
	typedef int Toy_dt;
	typedef int Tox_dt;
	typedef int Tof_step_dt;
	typedef int Tof_step_i_dt;
	typedef int Toy_step_dt;
	typedef int Toy_step_i_dt;
	typedef int Tox_step_dt;
	typedef int Tox_step_i_dt;

	typedef int Pof_dt;
	typedef int Pof_i_dt;
	typedef int Poy_dt;
	typedef int Poy_i_dt;
	typedef int Pox_dt;
	typedef int Pox_i_dt;
	typedef int PofBank_step_i_dt;

	typedef int wrd_1row_dt;
	typedef int row_1map_dt;

	typedef int conv_loop_dt;
	typedef int tile_loop_dt;
	typedef int wnd_loop_dt;
	typedef int wtbuf2pe_loop_dt;

	typedef int row_inbuf_i_dt;
	typedef int row_wtbuf_i_dt;
	typedef int row_outbuf_i_dt;
	typedef int row_biasbuf_i_dt;
	typedef int outbufnum_i_dt;

	typedef int bit_shift_dt;

#elif (DATA_TYPE_PAR == 1)
	typedef ap_uint<1> data_bool;
	typedef ap_uint<LAYER_B> layerNo_dt;
	typedef ap_uint<NKY_B> Nky_dt;
	typedef ap_uint<NKX_B> Nkx_dt;

	typedef ap_uint<NIF_B> Nif_dt;
	typedef ap_uint<NIF_I_B> Nif_i_dt;
	typedef ap_uint<NIY_B> Niy_dt;
	typedef ap_uint<NOY_B> Noy_dt;
	typedef ap_uint<NOF_STEP_B> Nof_step_dt;
	typedef ap_uint<NOF_STEP_I_B> Nof_step_i_dt;
	typedef ap_uint<NOY_STEP_B> Noy_step_dt;
	typedef ap_uint<NOY_STEP_I_B> Noy_step_i_dt;
	typedef ap_uint<NOFY_STEP_B> Nofy_step_dt;
	typedef ap_uint<NOFY_STEP_I_B> Nofy_step_i_dt;

	typedef ap_uint<TIY_B> Tiy_dt;
	typedef ap_uint<TIX_B> Tix_dt;
	typedef ap_uint<TOF_B> Tof_dt;
	typedef ap_uint<TOY_B> Toy_dt;
	typedef ap_uint<TOX_B> Tox_dt;
	typedef ap_uint<TOF_STEP_B> Tof_step_dt;
	typedef ap_uint<TOF_STEP_I_B> Tof_step_i_dt;
	typedef ap_uint<TOY_STEP_B> Toy_step_dt;
	typedef ap_uint<TOY_STEP_I_B> Toy_step_i_dt;
	typedef ap_uint<TOX_STEP_B> Tox_step_dt;
	typedef ap_uint<TOX_STEP_I_B> Tox_step_i_dt;

	typedef ap_uint<POF_B> Pof_dt;
	typedef ap_uint<POF_I_B> Pof_i_dt;
	typedef ap_uint<POY_B> Poy_dt;
	typedef ap_uint<POY_I_B> Poy_i_dt;
	typedef ap_uint<POX_B> Pox_dt;
	typedef ap_uint<POX_I_B> Pox_i_dt;
	typedef ap_uint<POFBANK_STEP_I_B> PofBank_step_i_dt;

	typedef ap_uint<WRD_1ROW_B> wrd_1row_dt;
	typedef ap_uint<ROW_1MAP_B> row_1map_dt;

	typedef ap_uint<CONV_LOOP_B> conv_loop_dt;
	typedef ap_uint<TILE_LOOP_B> tile_loop_dt;
	typedef ap_uint<WND_LOOP_B> wnd_loop_dt;
	typedef ap_uint<WTBUF2PE_LOOP_B> wtbuf2pe_loop_dt;

	typedef ap_uint<ROW_INBUF_I_B> row_inbuf_i_dt;
	typedef ap_uint<WRD_WTBUF_I_B> row_wtbuf_i_dt;
	typedef ap_uint<WRD_OUTBUF_I_B> row_outbuf_i_dt;
	typedef ap_uint<WRD_BIASBUF_I_B> row_biasbuf_i_dt;
	typedef ap_uint<OUTBUFNUM_I_B> outbufnum_i_dt;

	typedef ap_uint<BIT_SHIFT_B> bit_shift_dt;

#endif // Data types for Parameters

#endif // For header file redefinition

