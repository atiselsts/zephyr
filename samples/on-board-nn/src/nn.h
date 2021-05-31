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

#endif
