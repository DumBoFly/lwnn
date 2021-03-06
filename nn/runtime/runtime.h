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

#define RTE_FETCH_INT8(blob, at) (((int8_t*)(blob))[at])
#define RTE_FETCH_INT16(blob, at) (((int16_t*)(blob))[at])
#define RTE_FETCH_INT32(blob, at) (((int32_t*)(blob))[at])
#define RTE_FETCH_FLOAT(blob, at) (((float*)(blob))[at])

/* ============================ [ TYPES     ] ====================================================== */
typedef enum {
#define RTE_DEF(rte) RUNTIME_##rte,
	#include "rtedef.h"
#undef RTE_DEF
} runtime_type_t;

typedef void* runtime_t;

typedef struct nn nn_t;

typedef int (*rte_layer_action_t)(const nn_t*, const layer_t*);
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
runtime_t rte_create(const nn_t* nn);
int rte_init(const nn_t* nn);
int rte_execute(const nn_t* nn);
void rte_destory(const nn_t* nn);

int rte_do_for_each_layer(const nn_t* nn, rte_layer_action_t action);
int rte_is_layer_consumed_from(const nn_t* nn, const layer_t* layer, const layer_t* from);
#ifdef __cplusplus
}
#endif
#endif /* NN_RUNTIME_H_ */
