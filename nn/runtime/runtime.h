/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
#ifndef NN_RUNTIME_H_
#define NN_RUNTIME_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdint.h>
/* ============================ [ MACROS    ] ====================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/* ============================ [ TYPES     ] ====================================================== */
typedef enum {
	RUNTIME_CPU,
	RUNTIME_OPENCL
} runtime_type_t;

typedef void* runtime_t;

typedef struct nn nn_t;

typedef int (*runtime_layer_action_t)(const nn_t*, const layer_t*);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
runtime_t runtime_create(const nn_t* nn);
int runtime_init(const nn_t* nn);
int runtime_execute(const nn_t* nn);
void runtime_destory(const nn_t* nn);

int runtime_do_for_each_layer(const nn_t* nn, runtime_layer_action_t action);
#ifdef __cplusplus
}
#endif
#endif /* NN_RUNTIME_H_ */
