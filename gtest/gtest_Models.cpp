/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn_test_util.h"
/* ============================ [ MACROS    ] ====================================================== */
#define NNT_MNIST_NOT_FOUND_OKAY FALSE
#define NNT_MNIST_TOP1 0.9

#define NNT_UCI_INCEPTION_NOT_FOUND_OKAY TRUE
#define NNT_UCI_INCEPTION_TOP1 0.85
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
NNT_CASE_DEF(MNIST) =
{
	NNT_CASE_DESC(mnist),
};

NNT_CASE_DEF(UCI_INCEPTION) =
{
	NNT_CASE_DESC(uci_inception),
};
/* ============================ [ FUNCTIONS ] ====================================================== */
void ModelTestMain(runtime_type_t runtime,
		const network_t* network,
		const char* input,
		const char* output,
		float mintop1)
{
	int r = 0;
	size_t x_test_sz;
	size_t y_test_sz;
	float* x_test = (float*)nnt_load(input, &x_test_sz);
	int32_t* y_test = (int32_t*)nnt_load(output,&y_test_sz);

	const nn_input_t* const * inputs = network->inputs;
	const nn_output_t* const * outputs = network->outputs;

	int H = RTE_FETCH_INT32(inputs[0]->layer->dims, 1);
	int W = RTE_FETCH_INT32(inputs[0]->layer->dims, 2);
	int C = RTE_FETCH_INT32(inputs[0]->layer->dims, 3);
	if(C==0)
	{
		C=1;
	}
	int B = x_test_sz/(H*W*C*sizeof(float));

	int classes = RTE_FETCH_INT32(outputs[0]->layer->dims, 1);

	ASSERT_EQ(B, y_test_sz/sizeof(int32_t));

	nn_t* nn = nn_create(network, runtime);
	ASSERT_TRUE(nn != NULL);

	void* IN;

	size_t top1 = 0;
	for(int i=0; (i<B) && (r==0); i++)
	{
		float* in = x_test+H*W*C*i;
		size_t sz_in;
		if(inputs[0]->layer->dtype== L_DT_INT8)
		{
			sz_in = H*W*C;
			IN = nnt_quantize8(in, H*W*C, RTE_FETCH_INT8(inputs[0]->layer->blobs[0]->blob, 0));
			ASSERT_TRUE(IN != NULL);
		}
		else if(inputs[0]->layer->dtype== L_DT_INT16)
		{
			sz_in = H*W*C*sizeof(int16_t);
			IN = nnt_quantize16(in, H*W*C, RTE_FETCH_INT8(inputs[0]->layer->blobs[0]->blob, 0));
			ASSERT_TRUE(IN != NULL);
		}
		else
		{
			sz_in = H*W*C*sizeof(float);
			IN = in;
		}

		memcpy(inputs[0]->data, IN, sz_in);

		r = nn_predict(nn);
		EXPECT_EQ(0, r);

		if(0 == r)
		{
			int y=-1;
			float prob = 0;
			float* out = (float*)outputs[0]->data;
			if(inputs[0]->layer->dtype== L_DT_INT8)
			{
				out = nnt_dequantize8((int8_t*)out, classes, RTE_FETCH_INT8(outputs[0]->layer->blobs[0]->blob, 0));
			}
			else if(inputs[0]->layer->dtype== L_DT_INT16)
			{
				out = nnt_dequantize16((int16_t*)out, classes, RTE_FETCH_INT8(outputs[0]->layer->blobs[0]->blob, 0));
			}

			for(int j=0; j<classes; j++)
			{
				if(out[j] > prob)
				{
					prob = out[j];
					y = j;
				}
			}

			EXPECT_GE(y, 0);

			//printf("image %d predict as %d%s%d with prob=%.2f\n", i, y, (y==y_test[i])?"==":"!=", y_test[i], prob);

			if(y == y_test[i])
			{
				top1 ++;
			}

			if(out != outputs[0]->data)
			{
				free(out);
			}
		}

		if(IN != in)
		{
			free(IN);
		}

		if((i>0) && ((i%1000) == 0))
		{
			printf("LWNN TOP1 is %f on %d test images\n", (float)top1/i, i);
		}
	}

	printf("LWNN TOP1 is %f\n", (float)top1/B);

	EXPECT_GT(top1, B*mintop1);
	nn_destory(nn);

	free(x_test);
	free(y_test);

}

void NNTModelTestGeneral(runtime_type_t runtime,
		const char* netpath,
		const char* input,
		const char* output,
		float mintop1,
		float not_found_okay)
{
	const network_t* network;
	void* dll;
	network = nnt_load_network(netpath, &dll);
	if(not_found_okay == FALSE)
	{
		EXPECT_TRUE(network != NULL);
	}
	if(network == NULL)
	{
		return;
	}
	ModelTestMain(runtime, network, input, output, mintop1);
	dlclose(dll);
}

NNT_MODEL_TEST_DEF(CPU, MNIST, Q8)
NNT_MODEL_TEST_DEF(CPU, MNIST, Q16)
NNT_MODEL_TEST_DEF(CPU, MNIST, Float)
#ifndef DISABLE_RUNTIME_OPENCL
NNT_MODEL_TEST_DEF(OPENCL, MNIST, Float)
#endif

NNT_MODEL_TEST_DEF(CPU, UCI_INCEPTION, Q8)
NNT_MODEL_TEST_DEF(CPU, UCI_INCEPTION, Q16)
NNT_MODEL_TEST_DEF(CPU, UCI_INCEPTION, Float)
#ifndef DISABLE_RUNTIME_OPENCL
NNT_MODEL_TEST_DEF(OPENCL, UCI_INCEPTION, Float)
#endif