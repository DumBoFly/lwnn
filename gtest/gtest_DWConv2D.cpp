/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn_test_util.h"
/* ============================ [ MACROS    ] ====================================================== */
#define NNT_DWConv2D_MAX_DIFF 5.0/100
#define NNT_DWConv2D_MAX_QDIFF 0.15
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
NNT_CASE_DEF(DWConv2D) =
{
	NNT_CASE_DESC(dwconv2d_1),
	NNT_CASE_DESC(dwconv2d_2),
	NNT_CASE_DESC(dwconv2d_3)
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
NNT_TEST_ALL(DWConv2D)
