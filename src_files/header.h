#ifndef HEADER_H
#define HEADER_H

// #define DEBUG_MODE      // Checks for out of bounds values or indices
// #define DEBUG_PRINTS    // Prints values fofr debugging

#include "ap_int.h"                 // For arbitrary precision data types
#include "hls_stream.h"             // For Streams/FIFOs

#include "impl_choice.h"            // Instructions for processor to choose implementation
       
#include "parameters.h"

#include "data_types.h"

#include "function_declarations.h"
#include "value_init.h"
#include "bin_parameters.h"

#endif // HEADER_H