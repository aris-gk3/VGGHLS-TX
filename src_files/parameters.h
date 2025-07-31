#ifndef PARAMETERS_H
#define PARAMETERS_H
#include <iostream>
#include <iomanip>

// Define ceiling calculations of division
#define my_ceil( a, b ) ( (a%b) ? a/b + 1 : a/b )

// Activation precisions
#define NUM_BITS 8
#define SYMM_RANGE 127

#define MODEL 4
// 0 -> Toy Example 1
// 1 -> Toy Example 2
// 2 -> Toy Example 3
// 3 -> Toy Example 4 (*)
// 4 -> Toy Example 5 (*)
// 5 -> First 2 Conv. layers from VGG-16 (*)
// 6 -> Conv. layers 8,9 from VGG-16 (*)
// 7 -> VGG-16 Parameters (*)
#define SOLUTION 2
// 0 -> VGG-16 Parameters Solution 1 -> Pof=32, Poy=Pox=7
// 1 -> VGG-16 Parameters Solution 2 -> Pof=16, Poy=Pox=7 (*)
// 2 -> VGG-16 Parameters Solution 3 -> Pof=8 , Poy=Pox=7 (*)
#define DATA_WIDTH 1
// 0 -> We choose parameters fit for VGG-16, Solution 2
// 1 -> We choose parameters fit for model chosen
// 2 -> We choose 32 digits for all parameters

// *****  Bit precision  *****
#if (DATA_WIDTH == 0) // VGG-16 -> Solution 2
	#define LAYER_B           4
	#define NKY_B             2
	#define NKX_B             2

	#define NIF_B            10
	#define NIF_I_B           9
	#define NIY_B             8
	#define NOY_B             8
	#define NOF_STEP_B        3
	#define NOF_STEP_I_B      2
	#define NOY_STEP_B        4
	#define NOY_STEP_I_B      3
	#define NOFY_STEP_B       4
	#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

	#define TIY_B             5
	#define TIX_B             8
	#define TOF_B             9
	#define TOY_B             5
	#define TOX_B             8
	#define TOF_STEP_B        6
	#define TOF_STEP_I_B      5
	#define TOY_STEP_B        3
	#define TOY_STEP_I_B      2
	#define TOX_STEP_B        6
	#define TOX_STEP_I_B      5

	#define POF_B             4
	#define POF_I_B           3
	#define POY_B             3
	#define POY_I_B           3
	#define POX_B             3
	#define POX_I_B           3
	#define POFBANK_STEP_I_B  2

	#define WRD_1ROW_B        6
	#define ROW_1MAP_B        3

	#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
	#define TILE_LOOP_B      11 // Tof_step*Toy_step*Tox_step
	#define WND_LOOP_B       13 // Nif*NKY*NKX
	#define WTBUF2PE_LOOP_B  17 // Tof_step*Nif*NKY*NKX-1

	#define ROW_INBUF_I_B    14 // based on WRD_INBUF
	#define WRD_WTBUF_I_B    17 // based on WRD_WTBUF
	#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
	#define WRD_BIASBUF_I_B   8 // based on BIASBUF_LENGTH
	#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
#elif (DATA_WIDTH == 2) // 32 bits
	#define LENGTH 32

	#define LAYER_B           LENGTH
	#define NKY_B             LENGTH
	#define NKX_B             LENGTH

	#define NIF_B             LENGTH
	#define NIF_I_B           LENGTH
	#define NIY_B             LENGTH
	#define NOY_B             LENGTH
	#define NOF_STEP_B        LENGTH
	#define NOF_STEP_I_B      LENGTH
	#define NOY_STEP_B        LENGTH
	#define NOY_STEP_I_B      LENGTH
	#define NOFY_STEP_B       LENGTH
	#define NOFY_STEP_I_B     LENGTH // Max of Nof_step_i & Noy_step_i

	#define TIY_B             LENGTH
	#define TIX_B             LENGTH
	#define TOF_B             LENGTH
	#define TOY_B             LENGTH
	#define TOX_B             LENGTH
	#define TOF_STEP_B        LENGTH
	#define TOF_STEP_I_B      LENGTH
	#define TOY_STEP_B        LENGTH
	#define TOY_STEP_I_B      LENGTH
	#define TOX_STEP_B        LENGTH
	#define TOX_STEP_I_B      LENGTH

	#define POF_B             LENGTH
	#define POF_I_B           LENGTH
	#define POY_B             LENGTH
	#define POY_I_B           LENGTH
	#define POX_B             LENGTH
	#define POX_I_B           LENGTH
	#define POFBANK_STEP_I_B  LENGTH

	#define WRD_1ROW_B        LENGTH
	#define ROW_1MAP_B        LENGTH

	#define CONV_LOOP_B       LENGTH // bigger of Noy_step, Nof_step
	#define TILE_LOOP_B       LENGTH // Tof_step*Toy_step*Tox_step
	#define WND_LOOP_B        LENGTH // Nif*NKY*NKX
	#define WTBUF2PE_LOOP_B   LENGTH // Tof_step*Nif*NKY*NKX-1

	#define ROW_INBUF_I_B     LENGTH // based on WRD_INBUF
	#define WRD_WTBUF_I_B     LENGTH // based on WRD_WTBUF
	#define WRD_OUTBUF_I_B    LENGTH // based on WRD_OUTBUF
	#define WRD_BIASBUF_I_B   LENGTH // based on BIASBUF_LENGTH
	#define OUTBUFNUM_I_B     LENGTH // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
#endif

// *****  Parameters for base of Model  *****
#if ( MODEL == 0 ) // Toy Example 1
	// *****  Toy Example 1  *****
	// 3x26x26 -> 6x26x26 -> 5x26x26
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 3
	#define POY 3
	#define POX 3
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {2, 1};/*Nof/Tof*/
	const int Nif_rom[LAYERS] = {3, 6};
	const int Noy_rom[LAYERS] = {26, 26};
	const int Tof_rom[LAYERS] = {3, 5};
	const int Toy_rom[LAYERS] = {13, 26};
	const int Tox_rom[LAYERS] = {26, 26};
	// Buffer Sizing
	const int WRD_INBUF = 600; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 108; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 702; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 12 // Sum of Nof
	#define BIASBUF_LENGTH 6  // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 0}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {0, 1};
	const int fulBufPx[LAYERS] = {0, 1};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 4056
	#define WTMAP_MEMSIZE 270
	#define OFMAP_MEMSIZE 4056
	#define MAP_SIZE 4056
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {2, 1};
	const int nofy_step_rom[LAYERS] = {2, 1}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {26, 26};
	const int tof_step_rom[LAYERS] = {1, 2};
	const int toy_step_rom[LAYERS] = {5, 9};
	const int tox_step_rom[LAYERS] = {9, 9};
	const int tiy_rom[LAYERS] = {15, 28};
	const int tix_rom[LAYERS] = {28, 28};
	const int row_1map_rom[LAYERS] = {5, 10};
	const int wrd_1row_rom[LAYERS] = {10, 10};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {26, 53};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {26, 107};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {27, 54};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {45, 162};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {19, -215};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {0, 234};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {117, 234};
	#if (DATA_WIDTH == 1) // Model == 0
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             3
		#define NIF_I_B           3
		#define NIY_B             5
		#define NOY_B             5
		#define NOF_STEP_B        2
		#define NOF_STEP_I_B      1
		#define NOY_STEP_B        2
		#define NOY_STEP_I_B      1
		#define NOFY_STEP_B       2
		#define NOFY_STEP_I_B     1 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             5
		#define TIX_B             5
		#define TOF_B             3
		#define TOY_B             5
		#define TOX_B             5
		#define TOF_STEP_B        2
		#define TOF_STEP_I_B      1
		#define TOY_STEP_B        4
		#define TOY_STEP_I_B      4
		#define TOX_STEP_B        4
		#define TOX_STEP_I_B      4

		#define POF_B             2
		#define POF_I_B           2
		#define POY_B             2
		#define POY_I_B           2
		#define POX_B             2
		#define POX_I_B           2
		#define POFBANK_STEP_I_B  1

		#define WRD_1ROW_B        4
		#define ROW_1MAP_B        4

		#define CONV_LOOP_B       2 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       8 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B        6 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B   7 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B    10 // based on WRD_INBUF
		#define WRD_WTBUF_I_B     7 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B   10 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   3 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 1 ) // Toy Example 2
	// *****  Toy Example 2  *****
	// 6x12x12 -> 3x12x12 -> 9x12x12
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 3
	#define POY 3
	#define POX 3
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {1, 3};/*Nof/Tof*/
	const int Nif_rom[LAYERS] = {6, 3};
	const int Noy_rom[LAYERS] = {12, 12};
	const int Tof_rom[LAYERS] = {3, 3};
	const int Toy_rom[LAYERS] = {6, 4};
	const int Tox_rom[LAYERS] = {12, 12};
	// Buffer Sizing
	const int WRD_INBUF = 90; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 54; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 48; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 13 // Sum of Nof
	#define BIASBUF_LENGTH 4 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 1}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {1, 0};
	const int fulBufPx[LAYERS] = {0, 0};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 1008
	#define WTMAP_MEMSIZE 243
	#define OFMAP_MEMSIZE 1296
	#define MAP_SIZE 1296
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {2, 3};
	const int nofy_step_rom[LAYERS] = {2, 3}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {12, 12};
	const int tof_step_rom[LAYERS] = {1, 1};
	const int toy_step_rom[LAYERS] = {2, 2};
	const int tox_step_rom[LAYERS] = {4, 4};
	const int tiy_rom[LAYERS] = {8, 6};
	const int tix_rom[LAYERS] = {14, 14};
	const int row_1map_rom[LAYERS] = {3, 2};
	const int wrd_1row_rom[LAYERS] = {5, 5};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {53, 26};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {53, 26};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {54, 27};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {8, 8};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {9, 9};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {0, 0};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {24, 16};
	#if (DATA_WIDTH == 1) // Model == 0
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             3
		#define NIF_I_B           3
		#define NIY_B             4
		#define NOY_B             4
		#define NOF_STEP_B        2
		#define NOF_STEP_I_B      2
		#define NOY_STEP_B        2
		#define NOY_STEP_I_B      2
		#define NOFY_STEP_B       2
		#define NOFY_STEP_I_B     2 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             4
		#define TIX_B             4
		#define TOF_B             2
		#define TOY_B             3
		#define TOX_B             4
		#define TOF_STEP_B        1
		#define TOF_STEP_I_B      1
		#define TOY_STEP_B        2
		#define TOY_STEP_I_B      1
		#define TOX_STEP_B        3
		#define TOX_STEP_I_B      2

		#define POF_B             2
		#define POF_I_B           2
		#define POY_B             2
		#define POY_I_B           2
		#define POX_B             2
		#define POX_I_B           2
		#define POFBANK_STEP_I_B  1

		#define WRD_1ROW_B        3
		#define ROW_1MAP_B        2

		#define CONV_LOOP_B       2 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       4 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B        6 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B   6 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B     7 // based on WRD_INBUF
		#define WRD_WTBUF_I_B     6 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B    6 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   2 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables

#elif ( MODEL == 2 ) // Toy Example 3
	// *****  Toy Example 3  *****
	// 24x120x120 -> 96x120x120 -> 96x120x120
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 3
	#define POY 3
	#define POX 3
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {1, 6};/*Nof/Tof*/
	const int Nif_rom[LAYERS] = {24, 51};
	const int Noy_rom[LAYERS] = {60, 60};
	const int Tof_rom[LAYERS] = {51, 18};
	const int Toy_rom[LAYERS] = {15, 60};
	const int Tox_rom[LAYERS] = {60, 60};
	// Buffer Sizing
	const int WRD_INBUF = 22491; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 3672; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 10800; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 160 // Sum of Nof
	#define BIASBUF_LENGTH 52 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 0}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {1, 0};
	const int fulBufPx[LAYERS] = {0, 1};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 183600
	#define WTMAP_MEMSIZE 49572
	#define OFMAP_MEMSIZE 388800
	#define MAP_SIZE 388800
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {4, 1};
	const int nofy_step_rom[LAYERS] = {4, 6}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {60, 60};
	const int tof_step_rom[LAYERS] = {17, 6};
	const int toy_step_rom[LAYERS] = {5, 20};
	const int tox_step_rom[LAYERS] = {20, 20};
	const int tiy_rom[LAYERS] = {17, 62};
	const int tix_rom[LAYERS] = {62, 62};
	const int row_1map_rom[LAYERS] = {6, 21};
	const int wrd_1row_rom[LAYERS] = {21, 21};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {215, 458};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {3671,2753};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {216, 459};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {1700, 2400};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {-4759, -5959};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {4800, 6000};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {300, 1200};
	#if (DATA_WIDTH == 1) // Model == 0
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             6
		#define NIF_I_B           6
		#define NIY_B             6
		#define NOY_B             6
		#define NOF_STEP_B        3
		#define NOF_STEP_I_B      3
		#define NOY_STEP_B        3
		#define NOY_STEP_I_B      2
		#define NOFY_STEP_B       3
		#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             6
		#define TIX_B             6
		#define TOF_B             6
		#define TOY_B             6
		#define TOX_B             6
		#define TOF_STEP_B        5
		#define TOF_STEP_I_B      5
		#define TOY_STEP_B        5
		#define TOY_STEP_I_B      5
		#define TOX_STEP_B        5
		#define TOX_STEP_I_B      5

		#define POF_B             2
		#define POF_I_B           2
		#define POY_B             2
		#define POY_I_B           2
		#define POX_B             2
		#define POX_I_B           2
		#define POFBANK_STEP_I_B  1

		#define WRD_1ROW_B        5
		#define ROW_1MAP_B        5

		#define CONV_LOOP_B       3 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B      12 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B        9 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B  12 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B    15 // based on WRD_INBUF
		#define WRD_WTBUF_I_B    12 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B   14 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   6 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 3 ) // Toy Example 4
	// *****  Toy Example 4  *****
	// 96x112x112 -> 96x112x112 -> 192x112x112
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 32
	#define POY 7
	#define POX 7
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {1, 6};/*Nof/Tof*/
	const int Nif_rom[LAYERS] = {96, 96};
	const int Noy_rom[LAYERS] = {112, 112};
	const int Tof_rom[LAYERS] = {96, 32};
	const int Toy_rom[LAYERS] = {14, 112};
	const int Tox_rom[LAYERS] = {112, 112};
	// Buffer Sizing
	const int WRD_INBUF = 27744; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 2592; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 28672; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 288 // Sum of Nof
	#define BIASBUF_LENGTH 96 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 0}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {1, 0};
	const int fulBufPx[LAYERS] = {0, 1};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 1204224
	#define WTMAP_MEMSIZE 165888
	#define OFMAP_MEMSIZE 2408448
	#define MAP_SIZE 2408448
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {8, 1};
	const int nofy_step_rom[LAYERS] = {8, 6}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {112, 112};
	const int tof_step_rom[LAYERS] = {3, 1};
	const int toy_step_rom[LAYERS] = {2, 16};
	const int tox_step_rom[LAYERS] = {16, 16};
	const int tiy_rom[LAYERS] = {16, 114};
	const int tix_rom[LAYERS] = {114, 114};
	const int row_1map_rom[LAYERS] = {3, 17};
	const int wrd_1row_rom[LAYERS] = {17, 17};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {863, 863};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {2591, 863};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {864, 864};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {96, 256};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {-7071, 97};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {7168, 0};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {3584, 28672};
	#if (DATA_WIDTH == 1) // Model == 0
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             7
		#define NIF_I_B           7
		#define NIY_B             7
		#define NOY_B             7
		#define NOF_STEP_B        3
		#define NOF_STEP_I_B      3
		#define NOY_STEP_B        4
		#define NOY_STEP_I_B      3
		#define NOFY_STEP_B       4
		#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             7
		#define TIX_B             7
		#define TOF_B             7
		#define TOY_B             7
		#define TOX_B             7
		#define TOF_STEP_B        2
		#define TOF_STEP_I_B      2
		#define TOY_STEP_B        5
		#define TOY_STEP_I_B      4
		#define TOX_STEP_B        5
		#define TOX_STEP_I_B      4

		#define POF_B             6
		#define POF_I_B           5
		#define POY_B             3
		#define POY_I_B           3
		#define POX_B             3
		#define POX_I_B           3
		#define POFBANK_STEP_I_B  4

		#define WRD_1ROW_B        5
		#define ROW_1MAP_B        5

		#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       9 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B       10 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B  12 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B    15 // based on WRD_INBUF
		#define WRD_WTBUF_I_B    12 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   7 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 4 ) // Toy Example 5
	// *****  Toy Example   *****
	// 3x12x12 -> 12x12x12 -> 12x12x12
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 6
	#define POY 3
	#define POX 3
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {1, 2};/*Nof/Tof*/
	const int Nif_rom[LAYERS] = {3, 12};
	const int Noy_rom[LAYERS] = {12, 12};
	const int Tof_rom[LAYERS] = {12, 6};
	const int Toy_rom[LAYERS] = {6, 12};
	const int Tox_rom[LAYERS] = {12, 12};
	// Buffer Sizing
	const int WRD_INBUF = 300; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 216; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 144; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 24 // Sum of Nof
	#define BIASBUF_LENGTH 12 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 0}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {1, 0};
	const int fulBufPx[LAYERS] = {0, 1};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 1728
	#define WTMAP_MEMSIZE 1296
	#define OFMAP_MEMSIZE 1728
	#define MAP_SIZE 1728
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {2, 1};
	const int nofy_step_rom[LAYERS] = {2, 2}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {12, 12};
	const int tof_step_rom[LAYERS] = {2, 1};
	const int toy_step_rom[LAYERS] = {2, 4};
	const int tox_step_rom[LAYERS] = {4, 4};
	const int tiy_rom[LAYERS] = {8, 14};
	const int tix_rom[LAYERS] = {14, 14};
	const int row_1map_rom[LAYERS] = {3, 5};
	const int wrd_1row_rom[LAYERS] = {5, 5};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {26, 107};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {53, 107};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {27, 108};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {16, 16};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {-63, 9};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {72, 0};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {72, 144};
	// Binary Length of variables
	#if (DATA_WIDTH == 1)
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             4
		#define NIF_I_B           4
		#define NIY_B             4
		#define NOY_B             4
		#define NOF_STEP_B        2
		#define NOF_STEP_I_B      1
		#define NOY_STEP_B        2
		#define NOY_STEP_I_B      1
		#define NOFY_STEP_B       2
		#define NOFY_STEP_I_B     1 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             4
		#define TIX_B             4
		#define TOF_B             4
		#define TOY_B             4
		#define TOX_B             4
		#define TOF_STEP_B        2
		#define TOF_STEP_I_B      1
		#define TOY_STEP_B        3
		#define TOY_STEP_I_B      2
		#define TOX_STEP_B        3
		#define TOX_STEP_I_B      2

		#define POF_B             3
		#define POF_I_B           3
		#define POY_B             2
		#define POY_I_B           2
		#define POX_B             2
		#define POX_I_B           2
		#define POFBANK_STEP_I_B  2

		#define WRD_1ROW_B        3
		#define ROW_1MAP_B        3

		#define CONV_LOOP_B       2 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       5 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B        7 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B   7 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B     9 // based on WRD_INBUF
		#define WRD_WTBUF_I_B     8 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B    8 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   4 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 5 ) // First 2 Layers
	//   3 x 224 x 224 ->  64 x 224 x 224 (1)
	//  64 x 224 x 224 ->  64 x 224 x 224 (2)
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 32
	#define POY 7
	#define POX 7
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {  1,   1};/*Nof/Tof*/
	const int Nif_rom[LAYERS]      = {  3,  64};
	const int Noy_rom[LAYERS]      = {224, 224};
	const int Tof_rom[LAYERS]      = { 64,  64};
	const int Toy_rom[LAYERS]      = { 28,  28};
	const int Tox_rom[LAYERS]      = {224, 224};
	// Buffer Sizing
	const int WRD_INBUF = 10560; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 1152; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 28672; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 128 // Sum of Nof
	#define BIASBUF_LENGTH 64 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {1, 1}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {1, 1};
	const int fulBufPx[LAYERS] = {0, 0};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 3211264
	#define WTMAP_MEMSIZE 36864
	#define OFMAP_MEMSIZE 3211264
	#define MAP_SIZE 3211264 // should be max of IFMAP_MEMSIZE and OFMAP_MEMSIZE
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {8, 8};
	const int nofy_step_rom[LAYERS] = {8, 8}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {224, 224};
	const int tof_step_rom[LAYERS] = {2,  2};
	const int toy_step_rom[LAYERS] = {4,  4};
	const int tox_step_rom[LAYERS] = {32, 32};
	const int tiy_rom[LAYERS] = {30, 30};
	const int tix_rom[LAYERS] = {226,226};
	const int row_1map_rom[LAYERS] = {5,  5};
	const int wrd_1row_rom[LAYERS] = {33, 33};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {26, 575};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {53, 1151};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {27, 576};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {256, 256};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {-14143, -14143};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {14336, 14336};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {14336, 14336};
	// Binary Length of variables
	#if (DATA_WIDTH == 1)
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B             7
		#define NIF_I_B           6
		#define NIY_B             8
		#define NOY_B             8
		#define NOF_STEP_B        1
		#define NOF_STEP_I_B      1
		#define NOY_STEP_B        4
		#define NOY_STEP_I_B      3
		#define NOFY_STEP_B       4
		#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             5
		#define TIX_B             8
		#define TOF_B             7
		#define TOY_B             5
		#define TOX_B             8
		#define TOF_STEP_B        2
		#define TOF_STEP_I_B      1
		#define TOY_STEP_B        3
		#define TOY_STEP_I_B      2
		#define TOX_STEP_B        6
		#define TOX_STEP_I_B      5

		#define POF_B             6
		#define POF_I_B           5
		#define POY_B             3
		#define POY_I_B           3
		#define POX_B             3
		#define POX_I_B           3
		#define POFBANK_STEP_I_B  4

		#define WRD_1ROW_B        6
		#define ROW_1MAP_B        3

		#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       9 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B       10 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B  11 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B    14 // based on WRD_INBUF
		#define WRD_WTBUF_I_B    11 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   6 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 6 ) // Conv. Layer 8,9
	// 256 x  28 x  28 -> 512 x  28 x  28 (8)
	// 512 x  28 x  28 -> 512 x  28 x  28 (9)
	#define LAYERS 2
	#define NKX 3
	#define NKY 3
	#define PIF 1
	#define POF 32
	#define POY 7
	#define POX 7
	#define PIY POY
	#define PIX POX
	#define S 1
	#define ZERO_PAD 1
	// Necessary Parameters
	const int Nof_step_rom[LAYERS] = {   4,   4};/*Nof/Tof*/
	const int Nif_rom[LAYERS]      = { 256, 512};
	const int Noy_rom[LAYERS]      = {  28,  28};
	const int Tof_rom[LAYERS]      = { 128, 128};
	const int Toy_rom[LAYERS]      = {  28,  28};
	const int Tox_rom[LAYERS]      = {  28,  28};
	// Buffer Sizing
	const int WRD_INBUF = 12800; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
	const int WRD_WTBUF = 18432; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
	const int WRD_OUTBUF = 7168; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
	#define OUTBUF_NUM 2 // Number of Output Buffer Banks
	// Secondary Parameters
	#define BIASMEM_LENGTH 1024 // Sum of Nof
	#define BIASBUF_LENGTH 128 // Max Tof
	const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
	const int nofFirst[LAYERS] = {0, 0}; // I execute first the loop with one iteration, if there is one
	const int fulBufWt[LAYERS] = {0, 0};
	const int fulBufPx[LAYERS] = {1, 1};
	const int bit_shift_rom[LAYERS] = {0, 0};
	// Software Parameters
	#define IFMAP_MEMSIZE 401408
	#define WTMAP_MEMSIZE 2359296
	#define OFMAP_MEMSIZE 401408
	#define MAP_SIZE 401408 // should be max of IFMAP_MEMSIZE and OFMAP_MEMSIZE
	// Parameters that are calculated from necessary parameters
	const int noy_step_rom[LAYERS] = {1, 1};
	const int nofy_step_rom[LAYERS] = {4, 4}; // max of Nof_step, Noy_step
	const int niy_rom[LAYERS] = {28, 28};
	const int tof_step_rom[LAYERS] = {4, 4};
	const int toy_step_rom[LAYERS] = {4, 4};
	const int tox_step_rom[LAYERS] = {4, 4};
	const int tiy_rom[LAYERS] = {30, 30};
	const int tix_rom[LAYERS] = {30, 30};
	const int row_1map_rom[LAYERS] = {5, 5};
	const int wrd_1row_rom[LAYERS] = {5, 5};
	// pe_loop_limit = Nif*NKX*NKY-1
	const int pe_loop_limit_rom[LAYERS] = {2303, 4607};
	// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
	const int wtbuf2pe_loop_limit_rom[LAYERS] = {9215, 18431};
	// wndclc_loop_limit_rom = Nif*Nky*Nkx
	const int wndclc_loop_limit_rom[LAYERS] = {2304, 4608};
	// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
	const int tileclc_loop_limit_rom[LAYERS] = {64, 64};
	// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset1_rom[LAYERS] = {-5351, -5351};
	// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset2_rom[LAYERS] = {5376, 5376};
	// (POF/OUTBUF_NUM)*Toy*Tox_step
	const int pe2buf_addr_offset3_rom[LAYERS] = {1792, 1792};
	// Binary Length of variables
	#if (DATA_WIDTH == 1)
		#define LAYER_B           1
		#define NKY_B             2
		#define NKX_B             2

		#define NIF_B            10
		#define NIF_I_B           9
		#define NIY_B             5
		#define NOY_B             5
		#define NOF_STEP_B        3
		#define NOF_STEP_I_B      2
		#define NOY_STEP_B        1
		#define NOY_STEP_I_B      1
		#define NOFY_STEP_B       3
		#define NOFY_STEP_I_B     2 // Max of Nof_step_i & Noy_step_i

		#define TIY_B             5
		#define TIX_B             5
		#define TOF_B             8
		#define TOY_B             5
		#define TOX_B             5
		#define TOF_STEP_B        3
		#define TOF_STEP_I_B      2
		#define TOY_STEP_B        3
		#define TOY_STEP_I_B      2
		#define TOX_STEP_B        3
		#define TOX_STEP_I_B      2

		#define POF_B             6
		#define POF_I_B           5
		#define POY_B             3
		#define POY_I_B           3
		#define POX_B             3
		#define POX_I_B           3
		#define POFBANK_STEP_I_B  4

		#define WRD_1ROW_B        3
		#define ROW_1MAP_B        3

		#define CONV_LOOP_B       3 // bigger of Noy_step, Nof_step
		#define TILE_LOOP_B       7 // Tof_step*Toy_step*Tox_step
		#define WND_LOOP_B       13 // Nif*NKY*NKX
		#define WTBUF2PE_LOOP_B  15 // Tof_step*Nif*NKY*NKX-1

		#define ROW_INBUF_I_B    14 // based on WRD_INBUF
		#define WRD_WTBUF_I_B    15 // based on WRD_WTBUF
		#define WRD_OUTBUF_I_B   13 // based on WRD_OUTBUF
		#define WRD_BIASBUF_I_B   7 // based on BIASBUF_LENGTH
		#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM
	// If I add new binary length, I should add it to Check_Binary_Lengths function
	#endif // For length of variables
#elif ( MODEL == 7 || MODEL ==8 || MODEL == 9 ) // VGG-16 or VGG-Base or Model from Trasfer Learning
	//  *****  Parameters for VGG-16 Conv. Layers  *****
	//   3 x 224 x 224 ->  64 x 224 x 224 (1)
	//  64 x 224 x 224 ->  64 x 224 x 224 (2)
	//  64 x 112 x 112 -> 128 x 112 x 112 (3)
	// 128 x 112 x 112 -> 128 x 112 x 112 (4)
	// 128 x  56 x  56 -> 256 x  56 x  56 (5)
	// 256 x  56 x  56 -> 256 x  56 x  56 (6)
	// 256 x  56 x  56 -> 256 x  56 x  56 (7)
	// 256 x  28 x  28 -> 512 x  28 x  28 (8)
	// 512 x  28 x  28 -> 512 x  28 x  28 (9)
	// 512 x  28 x  28 -> 512 x  28 x  28 (10)
	// 512 x  14 x  14 -> 512 x  14 x  14 (11)
	// 512 x  14 x  14 -> 512 x  14 x  14 (12)
	// 512 x  14 x  14 -> 512 x  14 x  14 (13)

	#if (SOLUTION == 0) // Solution 1 -> Pof=32, Poy=Pox=7
		#define LAYERS 13
		#define NKX 3
		#define NKY 3
		#define PIF 1
		#define POF 32
		#define POY 7
		#define POX 7
		#define PIY POY
		#define PIX POX
		#define S 1
		#define ZERO_PAD 1
		// Necessary Parameters
		const int Nof_step_rom[LAYERS] = {  1,   1,   1,   1,   1,   1,   1,   4,   4,   4,   4,   4,   4};/*Nof/Tof*/
		const int Nif_rom[LAYERS]      = {  3,  64,  64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
		const int Noy_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		const int Tof_rom[LAYERS]      = { 64,  64, 128, 128, 256, 256, 256, 128, 128, 128, 128, 128, 128};
		const int Toy_rom[LAYERS]      = { 28,  28,  28,  28,  28,  28,  28,  28,  28,  28,  14,  14,  14};
		const int Tox_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		// Buffer Sizing
		const int WRD_INBUF = 12800; // Should be able to fit input pixels for all layers
			// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
		const int WRD_WTBUF = 18432; // Should be able to fit weights for all layers
			// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
		const int WRD_OUTBUF = 28672; // Should be able to fit output pixels for all layers
			// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
		#define OUTBUF_NUM 2 // Number of Output Buffer Banks
		// Secondary Parameters
		#define BIASMEM_LENGTH 4224 // Sum of Nof
		#define BIASBUF_LENGTH 256 // Max Tof
		const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
		// I execute first the loop with one iteration, if there is one
		const int nofFirst[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufWt[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufPx[LAYERS] = 		{ 0,  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
		const int bit_shift_rom[LAYERS] = 	{10, 10, 9, 9, 8, 8, 8, 8, 7, 8, 8, 8, 7};
		// Software Parameters
		#define IFMAP_MEMSIZE 3211264
		#define WTMAP_MEMSIZE 2359296
		#define OFMAP_MEMSIZE 3211264
		#define MAP_SIZE 3211264 // should be max of IFMAP_MEMSIZE and OFMAP_MEMSIZE
		// Parameters that are calculated from necessary parameters
		const int noy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1};
		const int nofy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4}; // max of Nof_step, Noy_step
		const int niy_rom[LAYERS] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
		const int tof_step_rom[LAYERS] = {2, 2, 4, 4, 8, 8, 8, 4, 4, 4, 4, 4, 4};
		const int toy_step_rom[LAYERS] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};
		const int tox_step_rom[LAYERS] = {32, 32, 16, 16, 8, 8, 8, 4, 4, 4, 2, 2, 2};
		const int tiy_rom[LAYERS] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 16, 16, 16};
		const int tix_rom[LAYERS] = {226, 226, 114, 114, 58, 58, 58, 30, 30, 30, 16, 16, 16};
		const int row_1map_rom[LAYERS] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3};
		const int wrd_1row_rom[LAYERS] = {33, 33, 17, 17, 9, 9, 9, 5, 5, 5, 3, 3, 3};
		// pe_loop_limit = Nif*NKX*NKY-1
		const int pe_loop_limit_rom[LAYERS] = {26, 575, 575, 1151, 1151, 2303, 2303,
											2303, 4607, 4607, 4607, 4607, 4607};
		// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
		const int wtbuf2pe_loop_limit_rom[LAYERS] = {53, 1151, 2303, 4607, 9215, 18431, 18431,
													9215, 18431, 18431, 18431, 18431, 18431};
		// wndclc_loop_limit_rom = Nif*Nky*Nkx
		const int wndclc_loop_limit_rom[LAYERS] = {27, 576, 576, 1152, 1152, 2304, 2304,
													2304, 4608, 4608, 4608, 4608, 4608};
		// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
		const int tileclc_loop_limit_rom[LAYERS] = {256, 256, 256, 256, 256, 256, 256,
													64, 64, 64, 16, 16, 16};
		// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset1_rom[LAYERS] = {-14143, -14143, -21407, -21407, -25039, -25039, -25039,
													-5351, -5351, -5351, -1331, -1331, -1331};
		// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset2_rom[LAYERS] = {14336, 14336, 21504, 21504, 25088, 25088, 25088,
													5376, 5376, 5376, 1344, 1344, 1344};
		// (POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset3_rom[LAYERS] = {14336, 14336, 7168, 7168, 3584, 3584, 3584,
													1792, 1792, 1792, 448, 448, 448};
		// Binary Length of variables
		#if (DATA_WIDTH == 1)
			#define LAYER_B           4
			#define NKY_B             2
			#define NKX_B             2

			#define NIF_B            10
			#define NIF_I_B           9
			#define NIY_B             8
			#define NOY_B             8
			#define NOF_STEP_B        3
			#define NOF_STEP_I_B      2
			#define NOY_STEP_B        4
			#define NOY_STEP_I_B      3
			#define NOFY_STEP_B       4
			#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

			#define TIY_B             5
			#define TIX_B             8
			#define TOF_B             9
			#define TOY_B             5
			#define TOX_B             8
			#define TOF_STEP_B        4
			#define TOF_STEP_I_B      3
			#define TOY_STEP_B        3
			#define TOY_STEP_I_B      2
			#define TOX_STEP_B        6
			#define TOX_STEP_I_B      5

			#define POF_B             6
			#define POF_I_B           5
			#define POY_B             3
			#define POY_I_B           3
			#define POX_B             3
			#define POX_I_B           3
			#define POFBANK_STEP_I_B  4

			#define WRD_1ROW_B        6
			#define ROW_1MAP_B        3

			#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
			#define TILE_LOOP_B       9 // Tof_step*Toy_step*Tox_step
			#define WND_LOOP_B       13 // Nif*NKY*NKX
			#define WTBUF2PE_LOOP_B  15 // Tof_step*Nif*NKY*NKX-1

			#define ROW_INBUF_I_B    14 // based on WRD_INBUF
			#define WRD_WTBUF_I_B    15 // based on WRD_WTBUF
			#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
			#define WRD_BIASBUF_I_B   8 // based on BIASBUF_LENGTH
			#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM

			#define BIT_SHIFT_B 	  4
		// If I add new binary length, I should add it to Check_Binary_Lengths function
		#endif // For length of variables
	#elif (SOLUTION == 1) // Solution 2 -> Pof=16, Poy=Pox=7
		#define LAYERS 13
		#define NKX 3
		#define NKY 3
		#define PIF 1
		#define POF 16
		#define POY 7
		#define POX 7
		#define PIY POY
		#define PIX POX
		#define S 1
		#define ZERO_PAD 1
		// Necessary Parameters
		const int Nof_step_rom[LAYERS] = {  1,   1,   1,   1,   1,   1,   1,   4,   4,   4,   4,   4,   4};/*Nof/Tof*/
		const int Nif_rom[LAYERS]      = {  3,  64,  64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
		const int Noy_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		const int Tof_rom[LAYERS]      = { 64,  64, 128, 128, 256, 256, 256, 128, 128, 128, 128, 128, 128};
		const int Toy_rom[LAYERS]      = { 28,  28,  28,  28,  28,  28,  28,  28,  28,  28,  14,  14,  14};
		const int Tox_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		// Buffer Sizing
		const int WRD_INBUF = 12800; // Should be able to fit input pixels for all layers
			// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
		const int WRD_WTBUF = 36864; // Should be able to fit weights for all layers
			// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
		const int WRD_OUTBUF = 28672; // Should be able to fit output pixels for all layers
			// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
		#define OUTBUF_NUM 2 // Number of Output Buffer Banks
		// Secondary Parameters
		#define BIASMEM_LENGTH 4224
		#define BIASBUF_LENGTH 256
		const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
		// I execute first the loop with one iteration, if there is one
		const int nofFirst[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufWt[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufPx[LAYERS] = 		{ 0,  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
		const int bit_shift_rom[LAYERS] = 	{10, 10, 9, 9, 8, 8, 8, 8, 7, 8, 8, 8, 7};
		// Software Parameters
		#define IFMAP_MEMSIZE 3211264
		#define WTMAP_MEMSIZE 2359296
		#define OFMAP_MEMSIZE 3211264
		#define MAP_SIZE 3211264 // should be max of IFMAP_MEMSIZE and OFMAP_MEMSIZE
		// Parameters that are calculated from necessary parameters
		const int noy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1};
		const int nofy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4}; // max of Nof_step, Noy_step
		const int niy_rom[LAYERS] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
		const int tof_step_rom[LAYERS] = {4, 4, 8, 8, 16, 16, 16, 8, 8, 8, 8, 8, 8};
		const int toy_step_rom[LAYERS] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};
		const int tox_step_rom[LAYERS] = {32, 32, 16, 16, 8, 8, 8, 4, 4, 4, 2, 2, 2};
		const int tiy_rom[LAYERS] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 16, 16, 16};
		const int tix_rom[LAYERS] = {226, 226, 114, 114, 58, 58, 58, 30, 30, 30, 16, 16, 16};
		const int row_1map_rom[LAYERS] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3};
		const int wrd_1row_rom[LAYERS] = {33, 33, 17, 17, 9, 9, 9, 5, 5, 5, 3, 3, 3};
		// pe_loop_limit = Nif*NKX*NKY-1
		const int pe_loop_limit_rom[LAYERS] = {26, 575, 575, 1151, 1151, 2303, 2303,
											2303, 4607, 4607, 4607, 4607, 4607};
		// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
		const int wtbuf2pe_loop_limit_rom[LAYERS] = {107, 2303, 4607, 9215, 18431, 36863, 36863,
													18431, 36863, 36863, 36863, 36863, 36863};
		// wndclc_loop_limit_rom = Nif*Nky*Nkx
		const int wndclc_loop_limit_rom[LAYERS] = {27, 576, 576, 1152, 1152, 2304, 2304,
													2304, 4608, 4608, 4608, 4608, 4608};
		// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
		const int tileclc_loop_limit_rom[LAYERS] = {512, 512, 512, 512, 512, 512, 512,
													128, 128, 128, 32, 32, 32};
		// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset1_rom[LAYERS] = {-21311, -21311, -24991, -24991, -26831, -26831, -26831,
													-6247, -6247, -6247, -1555, -1555, -1555};
		// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset2_rom[LAYERS] = {21504, 21504, 25088, 25088, 26880, 26880, 26880,
													6272, 6272, 6272, 1568, 1568, 1568};
		// (POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset3_rom[LAYERS] = {7168, 7168, 3584, 3584, 1792, 1792, 1792,
													896, 896, 896, 224, 224, 224};
		// Binary Length of variables
		#if (DATA_WIDTH == 1)
			#define LAYER_B           4
			#define NKY_B             2
			#define NKX_B             2

			#define NIF_B            10
			#define NIF_I_B           9
			#define NIY_B             8
			#define NOY_B             8
			#define NOF_STEP_B        3
			#define NOF_STEP_I_B      2
			#define NOY_STEP_B        4
			#define NOY_STEP_I_B      3
			#define NOFY_STEP_B       4
			#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

			#define TIY_B             5
			#define TIX_B             8
			#define TOF_B             9
			#define TOY_B             5
			#define TOX_B             8
			#define TOF_STEP_B        5
			#define TOF_STEP_I_B      4
			#define TOY_STEP_B        3
			#define TOY_STEP_I_B      2
			#define TOX_STEP_B        6
			#define TOX_STEP_I_B      5

			#define POF_B             5
			#define POF_I_B           4
			#define POY_B             3
			#define POY_I_B           3
			#define POX_B             3
			#define POX_I_B           3
			#define POFBANK_STEP_I_B  3

			#define WRD_1ROW_B        6
			#define ROW_1MAP_B        3

			#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
			#define TILE_LOOP_B      10 // Tof_step*Toy_step*Tox_step
			#define WND_LOOP_B       13 // Nif*NKY*NKX
			#define WTBUF2PE_LOOP_B  16 // Tof_step*Nif*NKY*NKX-1

			#define ROW_INBUF_I_B    14 // based on WRD_INBUF
			#define WRD_WTBUF_I_B    16 // based on WRD_WTBUF
			#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
			#define WRD_BIASBUF_I_B   8 // based on BIASBUF_LENGTH
			#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM

			#define BIT_SHIFT_B 	  4
		// If I add new binary length, I should add it to Check_Binary_Lengths function
		#endif // For length of variables
	#elif (SOLUTION == 2) // Solution 3 -> Pof=8 , Poy=Pox=7
		#define LAYERS 13
		#define NKX 3
		#define NKY 3
		#define PIF 1
		#define POF 8
		#define POY 7
		#define POX 7
		#define PIY POY
		#define PIX POX
		#define S 1
		#define ZERO_PAD 1
		// Necessary Parameters
		const int Nof_step_rom[LAYERS] = {  1,   1,   1,   1,   1,   1,   1,   4,   4,   4,   4,   4,   4};/*Nof/Tof*/
		const int Nif_rom[LAYERS]      = {  3,  64,  64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
		const int Noy_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		const int Tof_rom[LAYERS]      = { 64,  64, 128, 128, 256, 256, 256, 128, 128, 128, 128, 128, 128};
		const int Toy_rom[LAYERS]      = { 28,  28,  28,  28,  28,  28,  28,  28,  28,  28,  14,  14,  14};
		const int Tox_rom[LAYERS]      = {224, 224, 112, 112,  56,  56,  56,  28,  28,  28,  14,  14,  14};
		// Buffer Sizing
		const int WRD_INBUF = 12800; // Should be able to fit input pixels for all layers
		// WRD_INBUF = max(WRD_INBUF = WRD_1ROW[layer_no] * ROWS_1MAP[layer_no] * TIF[layer_no]), for every layer_no
		const int WRD_WTBUF = 73728; // Should be able to fit weights for all layers
		// WRD_WTBUF = max(NKX * NKY * TIF[layer_no] * my_ceil(TOF[layer_no],POF)), for every layer_no
		const int WRD_OUTBUF = 28672; // Should be able to fit output pixels for all layers
		// WRD_OUTBUF =   max(my_ceil(Tox[layerNo],POX)*Toy*my_ceil(TOF,OUTBUF_NUM))
		#define OUTBUF_NUM 2 // Number of Output Buffer Banks
		// Secondary Parameters
		#define BIASMEM_LENGTH 4224
		#define BIASBUF_LENGTH 256
		const int POFBANK_STEP = my_ceil(POF, OUTBUF_NUM);
		// I execute first the loop with one iteration, if there is one
		const int nofFirst[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufWt[LAYERS] = 		{ 1,  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
		const int fulBufPx[LAYERS] = 		{ 0,  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
		const int bit_shift_rom[LAYERS] = 	{10, 10, 9, 9, 8, 8, 8, 8, 7, 8, 8, 8, 7};
		// Software Parameters
		#define IFMAP_MEMSIZE 3211264
		#define WTMAP_MEMSIZE 2359296
		#define OFMAP_MEMSIZE 3211264
		#define MAP_SIZE 3211264 // should be max of IFMAP_MEMSIZE and OFMAP_MEMSIZE
		// Parameters that are calculated from necessary parameters
		const int noy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1};
		const int nofy_step_rom[LAYERS] = {8, 8, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4}; // max of Nof_step, Noy_step
		const int niy_rom[LAYERS] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
		const int tof_step_rom[LAYERS] = {8, 8, 16, 16, 32, 32, 32, 16, 16, 16, 16, 16, 16};
		const int toy_step_rom[LAYERS] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};
		const int tox_step_rom[LAYERS] = {32, 32, 16, 16, 8, 8, 8, 4, 4, 4, 2, 2, 2};
		const int tiy_rom[LAYERS] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 16, 16, 16};
		const int tix_rom[LAYERS] = {226, 226, 114, 114, 58, 58, 58, 30, 30, 30, 16, 16, 16};
		const int row_1map_rom[LAYERS] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3};
		const int wrd_1row_rom[LAYERS] = {33, 33, 17, 17, 9, 9, 9, 5, 5, 5, 3, 3, 3};
		// pe_loop_limit = Nif*NKX*NKY-1
		const int pe_loop_limit_rom[LAYERS] = {26, 575, 575, 1151, 1151, 2303, 2303,
										2303, 4607, 4607, 4607, 4607, 4607};
		// wtbuf2pe_loop_limit = Tof_step*Nif*NKY*NKX-1
		const int wtbuf2pe_loop_limit_rom[LAYERS] = {215, 4607, 9215, 18431, 36863, 73727, 73727,
												36863, 73727, 73727, 73727, 73727, 73727};
		// wndclc_loop_limit_rom = Nif*Nky*Nkx
		const int wndclc_loop_limit_rom[LAYERS] = {27, 576, 576, 1152, 1152, 2304, 2304,
													2304, 4608, 4608, 4608, 4608, 4608};
		// tileclc_loop_limit_rom = Tof_step*Toy_step*Tox_step
		const int tileclc_loop_limit_rom[LAYERS] = {1024, 1024, 1024, 1024, 1024, 1024, 1024,
													256, 256, 256, 64, 64, 64};
		// POY*Tox_step - (Tox_step-1) - (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset1_rom[LAYERS] = {-24895, -24895, -26783, -26783, -27727, -27727, -27727,
													-6695, -6695, -6695, -1667, -1667, -1667};
		// (Tof_step-1)*(POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset2_rom[LAYERS] = {25088, 25088, 26880, 26880, 27776, 27776, 27776,
													6720, 6720, 6720, 1680, 1680, 1680};
		// (POF/OUTBUF_NUM)*Toy*Tox_step
		const int pe2buf_addr_offset3_rom[LAYERS] = {3584, 3584, 1792, 1792, 896, 896, 896, 448,
													448, 448, 112, 112, 112};
		// Binary Length of variables
		#if (DATA_WIDTH == 1)
			#define LAYER_B           4
			#define NKY_B             2
			#define NKX_B             2

			#define NIF_B            10
			#define NIF_I_B           9
			#define NIY_B             8
			#define NOY_B             8
			#define NOF_STEP_B        3
			#define NOF_STEP_I_B      2
			#define NOY_STEP_B        4
			#define NOY_STEP_I_B      3
			#define NOFY_STEP_B       4
			#define NOFY_STEP_I_B     3 // Max of Nof_step_i & Noy_step_i

			#define TIY_B             5
			#define TIX_B             8
			#define TOF_B             9
			#define TOY_B             5
			#define TOX_B             8
			#define TOF_STEP_B        6
			#define TOF_STEP_I_B      5
			#define TOY_STEP_B        3
			#define TOY_STEP_I_B      2
			#define TOX_STEP_B        6
			#define TOX_STEP_I_B      5

			#define POF_B             4
			#define POF_I_B           3
			#define POY_B             3
			#define POY_I_B           3
			#define POX_B             3
			#define POX_I_B           3
			#define POFBANK_STEP_I_B  2

			#define WRD_1ROW_B        6
			#define ROW_1MAP_B        3

			#define CONV_LOOP_B       4 // bigger of Noy_step, Nof_step
			#define TILE_LOOP_B      11 // Tof_step*Toy_step*Tox_step
			#define WND_LOOP_B       13 // Nif*NKY*NKX
			#define WTBUF2PE_LOOP_B  17 // Tof_step*Nif*NKY*NKX-1

			#define ROW_INBUF_I_B    14 // based on WRD_INBUF
			#define WRD_WTBUF_I_B    17 // based on WRD_WTBUF
			#define WRD_OUTBUF_I_B   15 // based on WRD_OUTBUF
			#define WRD_BIASBUF_I_B   8 // based on BIASBUF_LENGTH
			#define OUTBUFNUM_I_B     1 // based on OUTBUF_NUM

			#define BIT_SHIFT_B 	  4
		// If I add new binary length, I should add it to Check_Binary_Lengths function
		#endif // For length of variables
	#endif // For solution
#endif

// *****  Parameters for head of Model  *****
#if ( MODEL == 7 ) // VGG-16 head
	#define CLASSES 1000
	const int HEAD_IN_SIZE[3] = {512*7*7, 4096, 1000};
	const int HEAD_OUT_SIZE[3] = {4096, 4096, 1000};
	const int bit_shift_fc_rom[3] = {0};	
#elif ( MODEL == 9 ) // Oxford flowers model head
	#define CLASSES 17
	const int HEAD_IN_SIZE[2] = {512, 256};
	const int HEAD_OUT_SIZE[2] = {256, 17};
	const int bit_shift_fc_rom[2] = {5, 9};
#else
	#define CLASSES 2
	const int HEAD_IN_SIZE[2] = {0};
	const int HEAD_OUT_SIZE[2] = {0};
	const int bit_shift_fc_rom[2] = {0};
#endif

#endif // PARAMETERS_H
