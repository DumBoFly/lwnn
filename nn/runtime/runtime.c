/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn.h"
/* ============================ [ MACROS    ] ====================================================== */
#define DECLARE_RUNTIME(name)									\
	extern runtime_t rte_##name##_create(const nn_t* nn);	\
	extern int rte_##name##_init(const nn_t* nn);			\
	extern int rte_##name##_execute(const nn_t* nn);		\
	extern void rte_##name##_destory(const nn_t* nn)


#define RUNTIME_REF(name)				\
	{									\
		rte_##name##_create,		\
		rte_##name##_init,			\
		rte_##name##_execute,		\
		rte_##name##_destory		\
	}
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	runtime_t (*create)(const nn_t*);
	int (*init)(const nn_t*);
	int (*execute)(const nn_t*);
	void (*destory)(const nn_t*);
} rte_ops_t;
/* ============================ [ DECLARES  ] ====================================================== */
#define RTE_DEF(rte) DECLARE_RUNTIME(rte);
	#include "rtedef.h"
#undef RTE_DEF
/* ============================ [ DATAS     ] ====================================================== */
static const rte_ops_t rte_ops[] =
{
#define RTE_DEF(rte) RUNTIME_REF(rte),
	#include "rtedef.h"
#undef RTE_DEF
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
runtime_t rte_create(const nn_t* nn)
{
	runtime_t runtime = NULL;

	if(nn->runtime_type < ARRAY_SIZE(rte_ops))
	{
		runtime = rte_ops[nn->runtime_type].create(nn);
	}

	return runtime;
}

int rte_init(const nn_t* nn)
{
	int r = NN_E_INVALID_RUNTIME;

	if(nn->runtime_type < (sizeof(rte_ops)/sizeof(rte_ops_t)))
	{
		r = rte_ops[nn->runtime_type].init(nn);
	}

	return r;
}
void rte_destory(const nn_t* nn)
{
	if(nn->runtime_type < ARRAY_SIZE(rte_ops))
	{
		rte_ops[nn->runtime_type].destory(nn);
	}
}

int rte_execute(const nn_t* nn)
{
	int r = NN_E_INVALID_RUNTIME;

	if(nn->runtime_type < ARRAY_SIZE(rte_ops))
	{
		r = rte_ops[nn->runtime_type].execute(nn);
	}

	return r;
}


int rte_do_for_each_layer(const nn_t* nn, rte_layer_action_t action)
{
	int r = 0;

	const layer_t* const* layers;
	const layer_t* layer;

	layers = nn->network->layers;

	layer = *layers++;
	while((NULL != layer) && (0 == r))
	{
		r = action(nn, layer);
		layer = *layers++;
	}

	return r;
}

int rte_is_layer_consumed_from(const nn_t* nn, const layer_t* layer, const layer_t* from)
{
	int r = FALSE;

	const layer_t* const* layers;
	const layer_t** inputs;

	layers = nn->network->layers;
	while((NULL != (*layers)) && ((*layers) != from))
	{
		layers++;
	}

	do
	{
		inputs = (*layers)->inputs;
		while((inputs != NULL) && (*inputs != NULL) && (FALSE == r))
		{
			if(*inputs == layer)
			{
				r = TRUE;
			}

			inputs++;
		}
		layers++;
	} while((NULL != (*layers)) && (FALSE == r));

	return r;
}



#ifndef DISABLE_NN_DDO
#include <sys/stat.h>
#ifndef DISABLE_RUNTIME_CPU
#include "cpu/runtime_cpu.h"
#endif
#ifndef DISABLE_RUNTIME_OPENCL
#include "opencl/runtime_opencl.h"
#endif
int rte_load_raw(const char* name, void* data, size_t sz)
{
	int r = 0;
	FILE* fp;
	size_t fsz;
	fp = fopen(name, "rb");

	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		fsz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if(fsz != sz)
		{
			printf("invalid raw %s: file size %d != %d\n", name, (int)fsz, (int)sz);
		}
		else
		{
			fread(data, 1, sz, fp);
		}
		fclose(fp);
	}
	else
	{
		printf("failed to load raw %s\n", name);
	}

	return r;
}

void rte_save_raw(const char* name, void* data, size_t sz)
{
	FILE* fp;
	fp = fopen(name, "ab+");
	size_t r = 0;
	if(fp != NULL)
	{
		r = fwrite(data, sz, 1, fp);
		if(1 != r) {
			r = fwrite(data, sz/2, 1, fp);
		}
		if(1 != r) {
			r = fwrite(data, sz/4, 1, fp);
		}

		fclose(fp);
	}

	if(1 != r)
	{
		printf("failed to save raw %s\n", name);
	}
}
void rte_ddo_save_raw(const nn_t* nn, const layer_t* layer, int i, void* data, size_t sz)
{
	char name[128];

	int offset;
	offset = snprintf(name, sizeof(name), "tmp/%s-%s-%d", nn->network->name, layer->name, i);

	if(nn->network->type != NETWORK_TYPE_FLOAT)
	{
		offset += snprintf(&name[offset], sizeof(name)-offset, "-Q%d", LAYER_Q(layer));
		if(NETWORK_TYPE_S8 == nn->network->type)
		{
			offset += snprintf(&name[offset], sizeof(name)-offset, "-Z%d-S%d", LAYER_Z(layer), LAYER_S(layer));
		}
	}

	offset += snprintf(&name[offset], sizeof(name)-offset, ".raw");
	#ifdef _WIN32
	mkdir("tmp");
	#else
	mkdir("tmp", S_IRWXU);
	#endif

	rte_save_raw(name, data, sz);
}

void rte_ddo_save(const nn_t* nn, const layer_t* layer)
{
	size_t sz = layer_get_size(layer);
	int i;
	switch(layer->C->context->dtype) {
		case L_DT_INT16:
		case L_DT_UINT16:
			sz = sz * sizeof(int16_t);
			break;
		case L_DT_INT32:
		case L_DT_UINT32:
			sz = sz * sizeof(int32_t);
			break;
		case L_DT_FLOAT:
			sz = sz * sizeof(float);
			break;
		case L_DT_STRING:
			return;
		default:
			break;
	}

#ifndef DISABLE_RUNTIME_CPU
	if((RUNTIME_CPU == nn->runtime_type)
#ifdef ENABLE_RUNTIME_HALIDE
	&& (RUNTIME_HALIDE == nn->runtime_type)
#endif
		)
	{
		layer_cpu_q_context_t* context = (layer_cpu_q_context_t*)layer->C->context;
		for(i=0; i<context->nout; i++)
		{
			rte_ddo_save_raw(nn, layer, i, context->out[i], sz);
		}
	}
#endif
#ifndef DISABLE_RUNTIME_OPENCL
	if(RUNTIME_OPENCL == nn->runtime_type)
	{
		int r = 0;
		layer_cl_context_t* context;
		void* data = malloc(sz);
		if(NULL != data)
		{
			context = (layer_cl_context_t*)layer->C->context;
			for(i=0; i<context->nout; i++)
			{
				if(L_OP_YOLO == layer->op)
				{
					memcpy(data, context->out[i], sz);
				}
				else
				{
					r = rte_cl_image2d_copy_out(nn, context->out[i], (float*)data, &(context->nhwc));
				}
				if(0 == r)
				{
					rte_ddo_save_raw(nn, layer, i, data, sz);
				}
				else
				{
					printf("failed to fetch CL output %s\n", layer->name);
				}
			}
			free(data);
		}
	}
#endif
}
#endif
