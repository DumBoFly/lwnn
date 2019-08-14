/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn_test_util.h"
/* ============================ [ MACROS    ] ====================================================== */
#define NNT_Conv1D_MAX_DIFF 5.0/100
#define NNT_Conv1D_MAX_QDIFF 0.15
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
NNT_CASE_REF(conv1d_1);
/* ============================ [ DATAS     ] ====================================================== */
NNT_CASE_DEF(Conv1D) =
{
	NNT_CASE_DESC(conv1d_1, add),
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
NNT_TEST_DEF(CPU, Conv1D, Q8)
NNT_TEST_DEF(CPU, Conv1D, Q16)
NNT_TEST_DEF(CPU, Conv1D, Float)
NNT_TEST_DEF(OPENCL, Conv1D, Float)