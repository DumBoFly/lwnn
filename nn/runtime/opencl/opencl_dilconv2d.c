/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2020  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn.h"
#ifndef DISABLE_RUNTIME_OPENCL
#include "runtime_opencl.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
typedef struct {
	LAYER_CL_CONTEXT_MEMBER;
	cl_mem W;
	cl_mem B;
} layer_cl_dilconv2d_context_t;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int layer_cl_DILCONV2D_init(const nn_t* nn, const layer_t* layer)
{
	int r;
	const char* option = "-DIS_DILCONV2D";
	layer_cl_dilconv2d_context_t* context;
	layer_activation_type_t act = RTE_FETCH_INT32(layer->blobs[2]->blob, 6);

	switch(act) {
		case L_ACT_RELU:
			option = "-DIS_DILCONV2D -DRELU";
			break;
		case L_ACT_LEAKY:
			option = "-DIS_DILCONV2D -DLEAKY";
			break;
		default:
			break;
	}

	r = rte_cl_create_layer_common(nn, layer,
			OPENCL_PATH "conv2d.cl", "conv2d", option,
			sizeof(layer_cl_dilconv2d_context_t));

	if(0 == r)
	{
		context = (layer_cl_dilconv2d_context_t*)layer->C->context;

		context->W = rte_cl_create_image2d_from_blob(nn, layer->blobs[0]);
		if(NULL != context->W)
		{
			context->B = rte_cl_create_image2d_from_blob(nn, layer->blobs[1]);
			if(NULL == context->B)
			{
				rte_cl_destory_memory(context->W);
				context->W = NULL;
				r = NN_E_NO_MEMORY;
			}
		}
		else
		{
			context->B = NULL;
			r = NN_E_NO_MEMORY;
		}

		if(0 != r)
		{
			rte_cl_destory_layer_context(nn, layer);
		}
	}

	return r;
}
int layer_cl_DILCONV2D_execute(const nn_t* nn, const layer_t* layer)
{
	int r = 0;
	layer_cl_dilconv2d_context_t* context = (layer_cl_dilconv2d_context_t*)layer->C->context;
	const layer_t* input = layer->inputs[0];
	layer_cl_context_t* input_context;
	int knlX, knlY, padX, padY, strideX, strideY;
	int* ints;
	int dilationX, dilationY;

	input_context = (layer_cl_context_t*)input->C->context;

	NNLOG(NN_DEBUG, ("execute %s\n", layer->name));

	ints = (int*)layer->blobs[0]->dims;
	knlY = ints[1];
	knlX = ints[2];

	ints = (int*)layer->blobs[2]->blob;
	padY = ints[0];
	padX = ints[1];
	strideY = ints[4];
	strideX = ints[5];
	dilationY = ints[7];
	dilationX = ints[8];

	r = rte_cl_set_layer_args(nn, layer, RTE_CL_ARGS_WITH_NHWC, 15,
					sizeof(cl_mem), &(input_context->out[0]),
					sizeof(cl_mem), &(context->W),
					sizeof(cl_mem), &(context->B),
					sizeof(cl_mem), &(context->out[0]),
					sizeof(int), &(input_context->nhwc.W),
					sizeof(int), &(input_context->nhwc.H),
					sizeof(int), &(input_context->nhwc.C),
					sizeof(int), &knlX,
					sizeof(int), &knlY,
					sizeof(int), &padX,
					sizeof(int), &padY,
					sizeof(int), &strideX,
					sizeof(int), &strideY,
					sizeof(int), &dilationX,
					sizeof(int), &dilationY);

	if(0 == r)
	{
		r = rte_cl_execute_layer(nn, layer, RTE_GWT_W_H_C, FALSE, NULL);
	}

	return r;
}
void layer_cl_DILCONV2D_deinit(const nn_t* nn, const layer_t* layer)
{
	layer_cl_dilconv2d_context_t* context;

	context = (layer_cl_dilconv2d_context_t*)layer->C->context;

	if(NULL != context)
	{
		rte_cl_destory_memory(context->W);
		rte_cl_destory_memory(context->B);
	}

	rte_cl_destory_layer_context(nn, layer);
}

#endif /* DISABLE_RUNTIME_OPENCL */