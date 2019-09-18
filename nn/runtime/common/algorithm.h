/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
#ifndef NN_ALGORITHM_H_
#define NN_ALGORITHM_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn.h"
/* ============================ [ MACROS    ] ====================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/* adjust pointer address by offset */
#define APABO(addr, offset) ((void*)(((size_t)(addr))+(offset)))
/* ============================ [ TYPES     ] ====================================================== */
typedef enum
{
	ALG_TRANSPOSE_FROM_NHWC_TO_NCHW,
} alg_transpose_t;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int alg_concat(const nn_t* nn, const layer_t* layer, int axis,
		void* pout, void* (*fetch_input)(const nn_t* nn, const layer_t* layer),
		size_t type_size);
int alg_up_sampling(void* pout, void* pin, NHWC_t *outNHWC, NHWC_t *inNHWC, size_t type_size);
int alg_transpose(void* output, const void* input, const NHWC_t *nhwc, size_t type_size, alg_transpose_t transpose);
#ifdef __cplusplus
}
#endif
#endif /* NN_ALGORITHM_H_ */
