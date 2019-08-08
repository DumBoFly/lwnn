/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn_test_util.h"
/* ============================ [ MACROS    ] ====================================================== */
#define NNT_Pad_MAX_DIFF 5.0/100
#define NNT_Pad_MAX_QDIFF 0.05
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
NNT_CASE_REF(pad_1);
NNT_CASE_REF(pad_2);
/* ============================ [ DATAS     ] ====================================================== */
NNT_CASE_DEF(Pad) =
{
	NNT_CASE_DESC(pad_1, BiasAdd),
	NNT_CASE_DESC(pad_2, BiasAdd),
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
NNT_TEST_DEF(CPU, Pad, Q8)
NNT_TEST_DEF(CPU, Pad, Q16)
NNT_TEST_DEF(CPU, Pad, Float)
NNT_TEST_DEF(OPENCL, Pad, Float)
