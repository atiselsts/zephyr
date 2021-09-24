#ifndef NN_H
#define NN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

int nn_setup(void);
int nn_classify(void);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#define WINDOW_SIZE 256

extern const int8_t raw_data[];

#define NUM_CLASSES 12

// results for 100 tests
#define MODEL_NAME custom_quant_int_128_64_3_8_tflite

/*

The following it time in milliseconds for 100 inferences:

custom_quant_int_128_16_1_12_tflite - 128
custom_quant_int_128_16_1_24_tflite - 96
custom_quant_int_128_16_1_4_tflite  - 352
custom_quant_int_128_16_3_12_tflite - 133
custom_quant_int_128_16_3_24_tflite - 92
custom_quant_int_128_16_3_2_tflite  - 605
custom_quant_int_128_32_1_12_tflite - 218
custom_quant_int_128_32_1_16_tflite - 205
custom_quant_int_128_32_3_1_tflite  - 1926
custom_quant_int_128_32_3_24_tflite - 140
custom_quant_int_128_32_3_8_tflite  - 311
custom_quant_int_128_64_1_12_tflite - 366
custom_quant_int_128_64_1_16_tflite - 352
custom_quant_int_128_64_3_8_tflite  - 548

(Warning - if you compare the numbers with previous emails, know that previously I mostly measured the time of 10 inferences as the models were slower. the paper reports the time of 1 inference, divide the results by 100 to get a value mathching the plots in the paper.)

*/

#endif
