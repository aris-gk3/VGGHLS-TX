#ifndef FUNCTION_DECLARATIONS_H
#define FUNCTION_DECLARATIONS_H

#include "ap_int.h" // For arbitrary precision data types
#include "hls_stream.h" // For Streams/FIFOs
#include "parameters.h"
#include "data_types.h"

// ***** Synthesizable Function Declarations  *****
void ReLu(
		px_data_t in, px_data_t *out);
void bias_ReLu(
		/* "State Input */ Tof_step_i_dt Tof_step_i,
		/* Shift number*/ bit_shift_dt bit_shift,
		/* Inputs */ b_data_t BiasBuf[BIASBUF_LENGTH], acc_data_t pxSerial[OUTBUF_NUM][POX],
		/* Output */ px_data_t ReLuOut[OUTBUF_NUM][POX]);
void Pe(
		/* Inputs */ px_data_t px_stream[POY][POX], wt_data_t wt_stream[POF],
		/* Output */ px_data_t rslt_stream[POF][POY][POX]);
void InBuf2Pe_ctrl(
		/* Parameters */ Nif_dt Nif,
		wrd_1row_dt wrd_1row, row_1map_dt row_1map,
		/* Address & Control Signal Outputs */
		row_inbuf_i_dt *row_px, Pox_i_dt *col, Poy_i_dt *bank,
		data_bool *dct_ld, data_bool *fl_ld, data_bool *wr_fifo);
void InBuf2Pe_wrapper(
		/* Inputs */ row_inbuf_i_dt row, Pox_i_dt col,
		Poy_i_dt bank, data_bool dct_ld, data_bool fl_ld,
		data_bool wr_fifo, const px_data_t InBuf[POY][WRD_INBUF][POX],
		/* Output */ px_data_t px_stream[POY][POX]);
void InBuf2Pe(
		/* Inputs */ row_inbuf_i_dt row, Pox_i_dt col,
		Poy_i_dt bank, data_bool dct_ld, data_bool fl_ld,
		data_bool wr_fifo, const px_data_t InBuf[POY][WRD_INBUF][POX],
		/* Output */ px_data_t px_stream[POY][POX],
		/* Input-Output */ hls::stream<px_data_t,4> FIFO_arr[POY][POX]);
void WtBuf2Pe_ctrl(
		/* Parameter */ wtbuf2pe_loop_dt wtbuf2pe_loop_limit,
		/* Address Output */ row_wtbuf_i_dt *row_wt_o);
void WtBuf2Pe(
		/* Inputs */ const wt_data_t WtBuf[WRD_WTBUF][POF],
		row_wtbuf_i_dt row_wt,
		/* Output */ wt_data_t wt_stream[POF]);
void wndClc_ctrl(
		/* Parameters */ Toy_step_dt Toy_step, Tox_step_dt Tox_step,
		Tof_step_dt Tof_step, wrd_1row_dt wrd_1row,
		/* Address Output */ row_inbuf_i_dt *rowStepAddress);
void wndClc_Dfl(
		/* Parameters */ wnd_loop_dt wndclc_loop_limit_in, wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
		row_inbuf_i_dt rowStepAddress, Nif_dt Nif_in,
		wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
		/* Inputs */ const px_data_t InBuf[POY][WRD_INBUF][POX],
		const wt_data_t WtBuf[WRD_WTBUF][POF],
		/* Output */ acc_data_t rslt_stream_out[POF][POY][POX]);
void wndClc(
		/* Parameters */ wnd_loop_dt wndclc_loop_limit_in, wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
		row_inbuf_i_dt rowStepAddress, Nif_dt Nif_in,
		wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
		/* Inputs */ px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		/* Output */ acc_data_t rslt_stream_out[POF][POY][POX]);
void Pe2Buf(
		/* Parameters*/ Toy_dt Toy, Toy_step_dt Toy_step,
		Tox_step_dt Tox_step, Tof_step_dt Tof_step,
		row_outbuf_i_dt in1, row_outbuf_i_dt in2,
		row_outbuf_i_dt in3,
		bit_shift_dt bit_shift,
		/* Inputs */ acc_data_t px_stream[POF][POY][POX], b_data_t BiasBuf[BIASBUF_LENGTH],
		/* Output */ px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);

void tileClc_Dfl(
		/* Parameters*/ tile_loop_dt tileclc_loop_limit_in, wnd_loop_dt wndclc_loop_limit_in,
		wtbuf2pe_loop_dt wtbuf2pe_loop_limit_in,
		Nif_dt Nif_in, Toy_dt Toy_in,
		Toy_step_dt Toy_step_in, Tox_step_dt Tox_step_in, Tof_step_dt Tof_step_in,
		wrd_1row_dt wrd_1row_in, row_1map_dt row_1map_in,
		row_outbuf_i_dt pe2buf_addr_offset1_in, row_outbuf_i_dt pe2buf_addr_offset2_in,
		row_outbuf_i_dt pe2buf_addr_offset3_in,
		bit_shift_dt bit_shift,
		/* Inputs */ px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		/* Output */ px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);
#if not defined(INTERNAL_CONFIG_LAYER)
void loadIfMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ data_bool northTile, data_bool southTile,
		Niy_dt yBase_in, const px_data_t_port *IfMap,//[NIF][NIX-2*ZERO_PAD][NIY-2*ZERO_PAD]
		/* Output */ px_data_t InBuf[POY][WRD_INBUF][POX]);
void loadWtMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ Nofy_step_dt ofBase, const wt_data_t_port *WtMap,
		/* Output */ wt_data_t WtBuf[WRD_WTBUF][POF]);
void mem2Buf(
		// Parameter Loading State
		data_bool layerCnfg,
		// Inputs
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Outputs
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF]
	);
void loadBiasTile(data_bool layerCnfg,
		b_data_t BiasBuf[BIASBUF_LENGTH]);
void tileClc(
		/* Parameters*/
		data_bool layerCnfg,
		/* Inputs */ px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		/* Output */ px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);
void storeMap(
		/* Parameter Loading State */ data_bool layerCnfg,
		/* Inputs */ const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		/* Output */ px_data_t_port *OfMap);
#elif defined(INTERNAL_CONFIG_LAYER)
void loadIfMap(
		const px_data_t_port *IfMap, //[NIF][NIX-2*ZERO_PAD][NIY-2*ZERO_PAD]
		px_data_t InBuf[POY][WRD_INBUF][POX]
	);
void loadWtMap(
		const wt_data_t_port *WtMap,
		wt_data_t WtBuf[WRD_WTBUF][POF]
	);
void mem2Buf(
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF]
	);
void loadBiasTile(
		b_data_t BiasBuf[BIASBUF_LENGTH]
	);
void tileClc(
		px_data_t InBuf[POY][WRD_INBUF][POX],
		wt_data_t WtBuf[WRD_WTBUF][POF],
		b_data_t BiasBuf[BIASBUF_LENGTH],
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]
	);
void storeMap(
		const px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		px_data_t_port *OfMap
	);
#endif
void ConvLayer_Dfl(
		// Parameter Loading State
		int layerNo,
		int loop_limit,
		// Intermediate (Buffered) Data
		px_data_t InBuf1[POY][WRD_INBUF][POX], px_data_t InBuf2[POY][WRD_INBUF][POX],
		wt_data_t WtBuf1[WRD_WTBUF][POF], wt_data_t WtBuf2[WRD_WTBUF][POF],
		px_data_t OutBuf1[OUTBUF_NUM][WRD_OUTBUF][POX], px_data_t OutBuf2[OUTBUF_NUM][WRD_OUTBUF][POX],
		b_data_t BiasBuf1[BIASBUF_LENGTH], b_data_t BiasBuf2[BIASBUF_LENGTH],
		// Inputs
		const px_data_t_port *IfMap,
		const wt_data_t_port *WtMap,
		// Output
		px_data_t_port *OfMap
	);
void ConvLayer(
		//Inputs
		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		const wt_data_t_port *WtMap, 	// [NOF][NIF][NKY][NKX]
		//Output
		px_data_t_port *OfMap 	// [NOF][NOY][NOX]
	);
void ConvLayer_module(data_bool layerCnfg, int test, int loop_limit_1, int loop_limit_2,
		px_data_t *IfMap,  // [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		wt_data_t *WtMap,  // [NOF][NIF][NKY][NKX]
		px_data_t *OfMap);
		void ConvLayerScdl(
		// Parameter Loading State
		data_bool nofFirst,
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
	);
void ConvLayerScdl(
		// Parameter Loading State
		data_bool nofFirst,
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
	);
void ConvX(
		//Inputs
		const px_data_t_port *IfMap, 	// [NIF][NIY-2*ZERO_PAD][NIX-2*ZERO_PAD]
		const wt_data_t_port *WtMap, 	// [NOF][NIF][NKY][NKX]
		const wt_data_t      *WtMapFc,
		//Output
		px_data_t_port *OfMap 			// [NOF][NOY][NOX]
	);
void gap(px_data_t *in, px_data_t *out);
void fcLayers(px_data_t *IfMap, wt_data_t *WtMap, px_data_t finalOut[1000]);
void fcLayersOF(
		/*Inputs*/ px_data_t *IfMap, wt_data_t *WtMap,
		/*Output*/ px_data_t finalOut[17]);
void fcLayersOFBlock(
		/*Inputs*/ const px_data_t_port *IfMap, const wt_data_t *WtMap,
		/*Output*/ px_data_t_port *OfMap);
void fcLayer(
		// Inputs
		px_data_t inPx[], const wt_data_t WtMap[],
		int inLength, int outLength, int layerNo,
		// Output
		px_data_t outPx[]
	);
void maxPool(px_data_t *IfMap, int channels, int yDim_out, int xDim_out, px_data_t *OfMap);
void vgg16Top(px_data_t *Map1, wt_data_t *WtMap, px_data_t *Map2, px_data_t finalOut[1000]);
void tlModelTop(px_data_t *Map1, wt_data_t *WtMap, // [NOF][NIF][NKY][NKX]
		px_data_t *Map2, px_data_t finalOut[17]);

// ******  Function Declarations for Testing  ******
// ** Printing Functions
void print_ComparedMap(int layer_no, float *Compared);
void printIfMap(int layerNo, px_data_t *IfMap, int minPrint);
void printWtMap(int layerNo, wt_data_t *WtMap, int minPrint);
void printOfMap(int layerNo, px_data_t *OfMap, int minPrint);
void printInBuf(px_data_t InBuf[POY][WRD_INBUF][POX]);
void printWtBuf(wt_data_t WtBuf[WRD_WTBUF][POF]);
void printOutBuf(int layerNo, px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);
void printPxStream(px_data_t PxStream[POY][POX]);
void printWtStream(wt_data_t WtStream[POF]);
void printPeResults(px_data_t px_stream[POF][POY][POX]);

// ** Testbench for Modules Function
void minimalRunSynthConv(int layerNo);
void minimalRunSynth(int layerNo, px_data_t_port* IfMap, 
					wt_data_t_port* WtMap, const wt_data_t* WtMapFc, px_data_t_port* OfMap);
int oxfordFlowers_test(int verbose, int debug, int minPrint, int biasReLuTrue);
int vgg16_test(int verbose, int minPrint, int biasReLuTrue);
int fcLayer_test(int verbose, std::string caseChoice);
int maxPool_test(int verbose);
int convLayer_test(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput);
int convLayer_test(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput);
int convLayer_test_experimental(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput);
int convLayer_test_experimental_new(int verbose, int debug, int minPrint,
					int printErrorOnly, int printLayer, int biasReLuTrue,
					int binInput);
int loadIfMap_test(int verbose, int printLayer, int printNofStep, int printNoyStep);
int loadWtMap_test(int verbose, int printLayer, int printNofStep, int printNoyStep, int printProgress);
int storeMaps_test(int verbose, int printLayer, int printNofStep, int printNoyStep);
int tileClc_test(int verbose, int printErrorOnly, int printLayer, int biasReLuTrue);
int wndClc_test(int verbose, int printLayer);
int loadInBuf2Pe_test(int verbose, int printFlagInfo, int printErrorOnly, int printLayer,
		int printToyStep, int printToxStep, int printTofStep,
		int printNif, int printNky, int printNkx);
int loadWtBuf2Pe_test(int verbose, int printErrorOnly, int printLayer,
		int printToyStep, int printToxStep, int printTofStep,
		int printNif, int printNky, int printNkx);
int storePe2Buf_test(int verbose, int printErrorOnly, int printLayer, int printNofStep, int printNoyStep,
		int printToyStep, int printToxStep, int printTofStep, int biasReLuTrue);
void fcLayer_toyEx();
void maxPool_toyEx();
void gap_test();


// Software Version of Functions
void oxfordFlowers_software(px_data_t* IfMap,
                    wt_data_t** WtMapConv,
                    wt_data_t** WtMapFC,
                    px_data_t* finalOut,
                    int biasReLuTrue);
void vgg16_software(px_data_t* IfMap,
                    wt_data_t** WtMapConv,
                    wt_data_t** WtMapFC,
                    px_data_t* finalOut,
                    int biasReLuTrue);
void convLayer_software(int layerNo,
		px_data_t *IfMap, wt_data_t *WtMap,
		px_data_t *OfMap,
		int biasReLuTrue);
void loadIfMap_software(int layerNo, int Noy_step_i,
		px_data_t *IfMap, px_data_t InBuf[POY][WRD_INBUF][POX]);
void loadWtMap_software(int layerNo, int Nof_step_i,
		wt_data_t *WtMap, wt_data_t WtBuf[WRD_WTBUF][POF]);
void storeMaps_software(int layerNo, int Nof_step_i, int Noy_step_i,
		px_data_t *OfMap /* [NOF][NOY][NOX] */, px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);
void tileClc_software(int layerNo, int Nof_step_i,
		px_data_t InBuf[POY][WRD_INBUF][POX], wt_data_t WtBuf[WRD_WTBUF][POF],
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX],
		int biasReLuTrue);
void wndClc_software(int layerNo, int Tof_step_i, int Toy_step_i, int Tox_step_i,
		px_data_t InBuf[POY][WRD_INBUF][POX], wt_data_t WtBuf[WRD_WTBUF][POF],
		px_data_t rslt_stream[POF][POY][POX]);
void InBuf2Pe_software(int layerNo, int Toy_step_i, int Tox_step_i, int Tof_step_i,
		int Nif_i, int Nky_i, int Nkx_i,
		px_data_t InBuf[POY][WRD_INBUF][POX], px_data_t to_PE[POY][POX]);
void WtBuf2Pe_software(int layerNo, int Toy_step_i, int Tox_step_i, int Tof_step_i,
		int Nif_i, int Nky_i, int Nkx_i,
		wt_data_t WtBuf[WRD_WTBUF][POF], wt_data_t wt_stream[POF]);
void Pe2Buf_software(int layerNo, int Nof_step_i, int Toy_step_i, int Tox_step_i, int Tof_step_i,
		px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX], px_data_t px_stream[POF][POY][POX],
		int biasReLuTrue);
void BiasReLu_software(int layerNo, int Nof_step_i, int Tof_step_i, int Pof_i,
		px_data_t in, px_data_t *out);

// ** Other Testbench Functions

// Calculation Functions
void Auxiliary_Calcultions(int MemoryBufferSizing, int ParameterCalculation,
		int PrintLoopLimits, int PrintAddrOffsets, int PrintBinaryLengths,
		int PrintBias, int PrintBiasFC);
void performanceEstimation(int Layer, int CNN, int CNNAll, int Prefetch, int Overlap, int SemiOverlap, int Seq);
void parameterCalculation(
		const int Noy[LAYERS], const int Tof[LAYERS], const int Toy[LAYERS],
		const int Tox[LAYERS], int layerNo,
		int *Noy_step, int *Niy, int *Tof_step, int *Toy_step, int *Tox_step,
		int *Tiy, int *Tix, int *row_1map,int *wrd_1row);
void memoryBufferSizing();
void calculateParameters();
void calculateLoopLimits();
void calculateAddressOffsets();
void findBinaryLengths();
void findBinaryLength(int *length, int bound);
int returnMax(px_data_t *Map, const int NUM_ELEMENTS);
void findMinMax(const px_data_t *Map, const int NUM_ELEMENTS, int &minVal, int &maxVal);

// Parameter Verification Functions
void printDesignChoice();
int Print_Check_Parameters(int verbose);
int Check_Binary_Lengths();

// Initialization Functions
void initInBuf_undef(px_data_t InBuf[POY][WRD_INBUF][POX]);
void initWtBuf_undef(wt_data_t WtBuf[WRD_WTBUF][POF]);
void initOutBuf_undef(px_data_t OutBuf[OUTBUF_NUM][WRD_OUTBUF][POX]);
void initOfMap_undef(int layerNo, px_data_t *OfMap, int value);
px_data_t* initIfMap(int layerNo, int binInput);
wt_data_t* initWtMap(int layerNo, int binInput);
void MemInit(wt_data_t*& WtMapCNN,  wt_data_t**& WtMapConv, wt_data_t**& WtMapFC,
    px_data_t*& IfMap, px_data_t*& Map2);

// Bin Loading Functions
bool pathExists(const char* path);
std::string getPath();
const std::string& path();
int8_t* imgLoadFromBin(const std::string& filename);
int32_t* wtLoadFromBin(const std::string& filename, size_t num_ints);
px_data_t* imgLoadFromBin_wrapper();
wt_data_t* wtLoadFromBin_wrapper(int layerNo);
px_data_t* biasLoadFromBin_wrapper(int layerNo);
int loadFromBin_wrapper_test();
void wtMemInitBin(wt_data_t*& WtMapCNN, wt_data_t**& WtMapConv, wt_data_t**& WtMapFC);
int MemInitBin_test();

// Misc Functions
void swapPointers(px_data_t *&a, px_data_t *&b);
#if defined(FMAP_WIDEN) || defined(WTMAP_WIDEN)
template <typename data_t_widened>
void pack(px_data_t *Map, data_t_widened *Map_widened,
            int factor, int mem_size_widened){
    for(int i=0;i<mem_size_widened;i++){
        for(int factor_i=0;factor_i<factor;factor_i++){
            Map_widened[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i)
                = Map[i*factor+factor_i];
        }
    }
}
#endif
#if defined(FMAP_WIDEN) || defined(WTMAP_WIDEN)
template <typename data_t_widened>
void unpack(data_t_widened *Map_widened, px_data_t *Map, 
			int factor, int mem_size_widened){
	for(int i=0;i<mem_size_widened;i++){
		for(int factor_i=0;factor_i<factor;factor_i++){
			Map[i*factor+factor_i] = 
				Map_widened[i].range(SYNTH_BITS*(factor_i+1)-1,SYNTH_BITS*factor_i);
		}
	}
}
#endif
void wt_reorder(const wt_data_t *Map, wt_data_t *Map_reordered, int layerNo);
void convChoice(px_data_t *IfMap, const wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *OfMap, int layerNo);
void fcChoice(px_data_t *IfMap, const wt_data_t *WtMap, 	// [NOF][NIF][NKY][NKX]
		px_data_t *OfMap);

#endif // FUNCTION_DECLARATIONS_H
