/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "nn_test_util.h"
#include "bbox_util.hpp"
#include "image.h"
#include <chrono>
/* ============================ [ MACROS    ] ====================================================== */
#define NNT_MNIST_NOT_FOUND_OKAY FALSE
#define NNT_MNIST_TOP1 0.9

#define NNT_UCI_INCEPTION_NOT_FOUND_OKAY TRUE
#define NNT_UCI_INCEPTION_TOP1 0.85

#define NNT_SSD_NOT_FOUND_OKAY TRUE
#define NNT_SSD_TOP1 0.9

#define NNT_YOLOV3_NOT_FOUND_OKAY TRUE
#define NNT_YOLOV3_TOP1 0.9

#define NNT_YOLOV3TINY_NOT_FOUND_OKAY TRUE
#define NNT_YOLOV3TINY_TOP1 0.9

#define NNT_VEHICLE_ATTR_NOT_FOUND_OKAY TRUE
#define NNT_VEHICLE_ATTR_TOP1 0.9

#define NNT_ENET_NOT_FOUND_OKAY TRUE
#define NNT_ENET_TOP1 0.9

#define NNT_KWS_NOT_FOUND_OKAY TRUE
#define NNT_KWS_TOP1 0.9

#define NNT_DS_NOT_FOUND_OKAY TRUE
#define NNT_DS_TOP1 0.9

#define NNT_MASKRCNN_NOT_FOUND_OKAY TRUE
#define NNT_MASKRCNN_TOP1 0.9

#define NNT_FACENET_NOT_FOUND_OKAY TRUE
#define NNT_FACENET_TOP1 0.9

#define ARGS_PREFETCH_ALL 0x01
#define ARGS_COMPARE_ALL_AT_END 0x02
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	void* data;
	size_t size;
} wav_t;

typedef struct {
	void* (*load_input)(nn_t* nn, const char* path, int id, size_t* sz);
	void* (*load_output)(const char* path, int id, size_t* sz);
	int (*compare)(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
	size_t n;
	size_t flags;
} nnt_model_args_t;
/* ============================ [ DECLARES  ] ====================================================== */
int mtcnn_predict(nn_t* PNet, nn_t* RNet, nn_t* ONet, image_t* im, float** p_points, size_t* p_number);
extern "C" int ROIAlign_forward_cpu(float* o, const float* in,
                                    const float* boxes, const int* indices,
                                    NHWC_t* onhwc, NHWC_t* inhwc);
static void* load_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_ssd_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_yolov3_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_vehicle_attributes_recognition_barrier_0039_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_enet_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_kws_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_ds_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_maskrcnn_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_facenet_input(nn_t* nn, const char* path, int id, size_t* sz);
static void* load_output(const char* path, int id, size_t* sz);
static int ssd_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
static int yolov3_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg);
static int vehicle_attributes_recognition_barrier_0039_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg);
static int enet_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
static int kws_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
static int ds_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
static int maskrcnn_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
static int facenet_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg);
/* ============================ [ DATAS     ] ====================================================== */
const char* g_InputImagePath = NULL;

static const char* voc_names_for_ssd[] = {"background","aeroplane", "bicycle", "bird", "boat","bottle", "bus", "car", "cat", "chair","cow", "diningtable", "dog", "horse","motorbike", "person", "pottedplant","sheep", "sofa", "train", "tvmonitor"};
static const char* coco_names_for_yolo[] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck","boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush" };

NNT_CASE_DEF(MNIST) =
{
	NNT_CASE_DESC(mnist),
};

NNT_CASE_DEF(UCI_INCEPTION) =
{
	NNT_CASE_DESC(uci_inception),
};

static const nnt_model_args_t nnt_ssd_args =
{
	load_ssd_input,
	load_output,
	ssd_compare,
	7	/* 7 test images */
};

NNT_CASE_DEF(SSD) =
{
	NNT_CASE_DESC_ARGS(ssd),
};

static const nnt_model_args_t nnt_yolov3_args =
{
	load_yolov3_input,
	load_output,
	yolov3_compare,
	1	/* 1 test images */
};

static const nnt_model_args_t nnt_yolov3_tiny_args =
{
	load_yolov3_input,
	load_output,
	yolov3_compare,
	1	/* 1 test images */
};

NNT_CASE_DEF(YOLOV3) =
{
	NNT_CASE_DESC_ARGS(yolov3),
};

NNT_CASE_DEF(YOLOV3TINY) =
{
	NNT_CASE_DESC_ARGS(yolov3_tiny),
};

static const nnt_model_args_t nnt_vehicle_attributes_recognition_barrier_0039_args =
{
	load_vehicle_attributes_recognition_barrier_0039_input,
	load_output,
	vehicle_attributes_recognition_barrier_0039_compare,
	1
};

NNT_CASE_DEF(VEHICLE_ATTR) =
{
	NNT_CASE_DESC_ARGS(vehicle_attributes_recognition_barrier_0039),
};

static const nnt_model_args_t nnt_enet_args =
{
	load_enet_input,
	load_output,
	enet_compare,
	1
};

NNT_CASE_DEF(ENET) =
{
	NNT_CASE_DESC_ARGS(enet),
};

static const nnt_model_args_t nnt_kws_args =
{
	load_kws_input,
	load_output,
	kws_compare,
	1
};

NNT_CASE_DEF(KWS) =
{
	NNT_CASE_DESC_ARGS(kws),
};

static const nnt_model_args_t nnt_deepspeech_args =
{
	load_ds_input,
	load_output,
	ds_compare,
	1,
	ARGS_PREFETCH_ALL|ARGS_COMPARE_ALL_AT_END
};

NNT_CASE_DEF(DSMFCC) =
{
	NNT_CASE_DESC(dsmfcc),
};

NNT_CASE_DEF(DS) =
{
	NNT_CASE_DESC_ARGS(deepspeech),
};

static const nnt_model_args_t nnt_maskrcnn_args =
{
	load_maskrcnn_input,
	load_output,
	maskrcnn_compare,
	1
};

NNT_CASE_DEF(MASKRCNN) =
{
	NNT_CASE_DESC_ARGS(maskrcnn),
};
#if 0
NNT_CASE_DEF(PNET) =
{
	NNT_CASE_DESC(PNet),
};

NNT_CASE_DEF(RNET) =
{
	NNT_CASE_DESC(RNet),
};

NNT_CASE_DEF(ONET) =
{
	NNT_CASE_DESC(ONet),
};
#else
NNT_CASE_DEF(FACEDETECTNET) =
{
	NNT_CASE_DESC(facedetectnet),
};
#endif
static const nnt_model_args_t nnt_facenet_args =
{
	load_facenet_input,
	load_output,
	facenet_compare,
	1,
	ARGS_PREFETCH_ALL|ARGS_COMPARE_ALL_AT_END
};

NNT_CASE_DEF(FACENET) =
{
	NNT_CASE_DESC_ARGS(facenet),
};
/* ============================ [ LOCALS    ] ====================================================== */
static void* load_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	char name[256];
	snprintf(name, sizeof(name), "%s/input%d.raw", path, id);

	return nnt_load(name, sz);
}

static void* load_ssd_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	image_t* resized_im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;

	EXPECT_EQ(context->nhwc.C, 3);

	if(g_InputImagePath != NULL)
	{
		printf("loading %s for %s\n", g_InputImagePath, nn->network->name);
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		resized_im = image_resize(im, context->nhwc.W, context->nhwc.H);
		assert(resized_im != NULL);
		float* input = (float*)malloc(sizeof(float)*NHWC_BATCH_SIZE(context->nhwc));

		for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc); i++)
		{
			input[i] = 0.007843*(resized_im->data[i]-127.5);
		}

		image_close(im);
		image_close(resized_im);

		*sz = sizeof(float)*NHWC_BATCH_SIZE(context->nhwc);
		return (void*) input;
	}
	else
	{
		return load_input(nn, path, id, sz);
	}
}

static void* load_yolov3_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	image_t* resized_im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;

	EXPECT_EQ(context->nhwc.C, 3);

	if(g_InputImagePath != NULL)
	{
		printf("loading %s for %s\n", g_InputImagePath, nn->network->name);
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		resized_im = image_letterbox(im, context->nhwc.W, context->nhwc.H, 127);
		assert(resized_im != NULL);
		float* input = (float*)malloc(sizeof(float)*NHWC_BATCH_SIZE(context->nhwc));

		for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc); i++)
		{
			input[i] = resized_im->data[i]/255.f;
		}
		image_close(im);
		image_close(resized_im);

		*sz = sizeof(float)*NHWC_BATCH_SIZE(context->nhwc);
		return (void*) input;
	}
	else
	{
		return load_input(nn, path, id, sz);
	}
}

static void* load_vehicle_attributes_recognition_barrier_0039_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	image_t* resized_im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;

	EXPECT_EQ(context->nhwc.C, 3);

	assert(g_InputImagePath != NULL);

	printf("loading %s for %s\n", g_InputImagePath, nn->network->name);
	im = image_open(g_InputImagePath);
	assert(im != NULL);
	resized_im = image_resize(im, context->nhwc.W, context->nhwc.H);
	assert(resized_im != NULL);
	float* input = (float*)malloc(sizeof(float)*NHWC_BATCH_SIZE(context->nhwc));

	for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc); i++)
	{
		input[i] = resized_im->data[i];
	}
	image_close(im);
	image_close(resized_im);

	*sz = sizeof(float)*NHWC_BATCH_SIZE(context->nhwc);
	return (void*) input;
}

static void* load_enet_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	image_t* resized_im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;

	EXPECT_EQ(context->nhwc.C, 3);

	if(g_InputImagePath != NULL)
	{
		printf("loading %s for %s\n", g_InputImagePath, nn->network->name);
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		resized_im = image_resize(im, context->nhwc.W, context->nhwc.H);
		assert(resized_im != NULL);
		float* input = (float*)malloc(sizeof(float)*NHWC_BATCH_SIZE(context->nhwc));

		for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc)/3; i++)
		{	/* BGR */
			input[3*i] = resized_im->data[3*i+2];
			input[3*i+1] = resized_im->data[3*i+1];
			input[3*i+2] = resized_im->data[3*i];
		}
		image_close(im);
		image_close(resized_im);

		*sz = sizeof(float)*NHWC_BATCH_SIZE(context->nhwc);
		return (void*) input;
	}
	else
	{
		return load_input(nn, path, id, sz);
	}
}
/* http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/ */
static void* load_kws_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	assert(g_InputImagePath != NULL);

	void* wav_data = nnt_load(g_InputImagePath, sz);

	wav_t* wav = (wav_t*)malloc(sizeof(wav_t));
	wav->data = wav_data;
	wav->size = *sz;

	printf("load %s @%p with size %d\n", g_InputImagePath, wav_data, (int)*sz);

	return wav;
}

static void* load_ds_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	void* dll;
	const network_t* network;
	nn_t* dsnn;
	const char* netpath = DSMFCC_cases[0].networkFloat;
	wav_t* wav;
	float* outputs = NULL;
	int r;
	char* pos;

	assert(g_InputImagePath != NULL);
	printf("loading %s for %s\n", g_InputImagePath, nn->network->name);

	pos = strstr((char*)g_InputImagePath, ".raw");
	if(NULL != pos) {
		return nnt_load(g_InputImagePath, sz);
	}
	network = nnt_load_network(netpath, &dll);
	if(NULL != network) {
		wav = (wav_t*)network->inputs[0]->data;
		wav->data = nnt_load(g_InputImagePath, &wav->size);
		if(wav->data != NULL) {
			dsnn = nn_create(network, RUNTIME_CPU);
			if(NULL != dsnn) {
				auto trun_s = std::chrono::high_resolution_clock::now();
				r = nn_predict(dsnn);
				auto trun_e = std::chrono::high_resolution_clock::now();
				auto trun_sum = std::chrono::duration_cast<std::chrono::nanoseconds>(trun_e-trun_s).count();
				EXPECT_EQ(r, 0);
				if(0 == r) {
					size_t bs = NHWC_SIZE(network->outputs[0]->layer->C->context->nhwc);
					*sz = bs*sizeof(float);
					outputs = (float*)malloc(*sz);
					if(NULL != outputs) {
						memcpy(outputs, network->outputs[0]->layer->C->context->out[0], *sz);
					}
					printf("Feature extraction cost total %.3fms\n",(float)trun_sum/1000000);
				}
				nn_destory(dsnn);
			}
			dlclose(dll);
		} else {
			free(wav->data);
		}
	}

	return outputs;
}

static void* load_maskrcnn_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	image_t* resized_im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;
	char* pos;
	float* input;

	EXPECT_EQ(context->nhwc.C, 3);

	assert(g_InputImagePath != NULL);

	printf("loading %s for %s\n", g_InputImagePath, nn->network->name);

	pos = strstr((char*)g_InputImagePath, ".raw");
	if(NULL != pos) {
		input = (float*)nnt_load(g_InputImagePath, sz);
	} else {
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		resized_im = image_letterbox(im, context->nhwc.W, context->nhwc.H, 0);
		assert(resized_im != NULL);
		input = (float*)malloc(sizeof(float)*NHWC_BATCH_SIZE(context->nhwc));
		for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc); i+=3)
		{
			input[i] = resized_im->data[i]-123.7;
			input[i+1] = resized_im->data[i+1]-116.8;
			input[i+2] = resized_im->data[i+2]-103.9;
		}

		image_close(im);
		image_close(resized_im);
		*sz = sizeof(float)*NHWC_BATCH_SIZE(context->nhwc);
	}

	return (void*) input;
}
#if 0
static void* load_facenet_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;
	float* input = NULL;
	void *dllP=NULL,*dllR=NULL,*dllO=NULL;
	const network_t* network;
	nn_t *PNet=NULL,*RNet=NULL,*ONet=NULL;

	network = nnt_load_network(PNET_cases[0].networkFloat, &dllP);
	if(NULL != network) {
		PNet = nn_create(network, RUNTIME_CPU);
	}
	network = nnt_load_network(RNET_cases[0].networkFloat, &dllR);
	if(NULL != network) {
		RNet = nn_create(network, RUNTIME_CPU);
	}
	network = nnt_load_network(ONET_cases[0].networkFloat, &dllO);
	if(NULL != network) {
		ONet = nn_create(network, RUNTIME_CPU);
	}

	assert(g_InputImagePath != NULL);
	printf("loading %s for %s\n", g_InputImagePath, nn->network->name);

	if((PNet!=NULL) && (RNet!=NULL) && (ONet!=NULL)) {
		im = image_open(g_InputImagePath);
		assert(im != NULL);

		size_t number = 0;
		float* points = NULL;
		int r = mtcnn_predict(PNet, RNet, ONet, im, &points, &number);

		EXPECT_EQ(0, r);

		if(points) delete [] points;
		image_close(im);
	} else {
		printf("loading MTCNN failed\n");
	}

	if(PNet) nn_destory(PNet);
	if(RNet) nn_destory(RNet);
	if(ONet) nn_destory(ONet);

	if(dllP) dlclose(dllP);
	if(dllR) dlclose(dllR);
	if(dllO) dlclose(dllO);
	return input;
}
#else
static void prewhiten(float* im, int H, int W, int C) {
	float mean = 0.0f;
	for (int i = 0; i < H*W*C; ++i) {
		mean += im[i];
	}
	mean = mean/(H*W*C);
	float std = 0.0f;
	for (int i = 0; i < H*W*C; ++i) {
		std += (im[i]-mean)*(im[i]-mean);
	}
	std = std/(H*W*C);
	std = (float)std::fmax(std::sqrt(std), 1.0f / std::sqrt(H*W*C));
	for (int i = 0; i < H*W*C; ++i) {
		im[i] = (im[i] - mean) / std;
	}

	NNLOG(NN_DEBUG, (" mean=%f, std=%f\n", mean, std));
}

static void* load_facenet_input(nn_t* nn, const char* path, int id, size_t* sz)
{
	image_t* im, *resized_im;
	float* input = NULL;
	void *dllFDN=NULL;
	layer_context_t* context;
	const network_t* network;
	nn_t *FDN=NULL;

	network = nnt_load_network(FACEDETECTNET_cases[0].networkFloat, &dllFDN);
	if(NULL != network) {
		FDN = nn_create(network, RUNTIME_CPU);
	}

	assert(g_InputImagePath != NULL);
	printf("loading %s for %s\n", g_InputImagePath, nn->network->name);

	if(FDN!=NULL) {
		context = (layer_context_t*)FDN->network->inputs[0]->layer->C->context;
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		resized_im = image_resize(im, context->nhwc.W, context->nhwc.H);
		assert(resized_im != NULL);
		float* fdn_input = (float*)FDN->network->inputs[0]->data;

		for(int i=0; i<NHWC_BATCH_SIZE(context->nhwc)/3; i++) { /* BGR */
			fdn_input[3*i] = (resized_im->data[3*i+2]-104.0);
			fdn_input[3*i+1] = (resized_im->data[3*i+1]-117.0);
			fdn_input[3*i+2] = (resized_im->data[3*i]-123.0);
		}

		int r = nn_predict(FDN);
		EXPECT_EQ(0, r);
		int num_det = FDN->network->outputs[0]->layer->C->context->nhwc.N;
		float* output = (float*)FDN->network->outputs[0]->data;

		NHWC_t inhwc = {1, im->h, im->w, im->c};
		NHWC_t onhwc = nn->network->inputs[0]->layer->C->context->nhwc;
		assert(3 == onhwc.C);

		*sz = num_det*NHWC_BATCH_SIZE(onhwc)*sizeof(float);
		input = (float*)malloc(*sz);
		assert(input != NULL);
		float* im_data = new float[im->h*im->w*im->c];
		assert(im_data != NULL);
		for(int i=0; i<im->h*im->w*im->c; i++) {
			im_data[i] = (float)im->data[i];
		}

		for(int i=0; (i<num_det) && (0==r); i++) {
			float prop = output[7*i+2];
			int x1 = output[7*i+3]*im->w;
			int y1 = output[7*i+4]*im->h;
			int x2 = output[7*i+5]*im->w;
			int y2 = output[7*i+6]*im->h;
			int w = x2 - x1;
			int h = y2 - y1;
			int l = std::max(w,h);
			float* o = input + i*NHWC_BATCH_SIZE(onhwc);
			float boxes[4] = { (float)(y1-(l-h)/2)/im->h,(float)(x1-(l-w)/2)/im->w,
							   (float)(y2+(l-h)/2)/im->h,(float)(x2+(l-w)/2)/im->w };
			int indices = 0;
			r = ROIAlign_forward_cpu(o, im_data, boxes, &indices, &onhwc, &inhwc);
			if(0 == r) {
				prewhiten(o, onhwc.H, onhwc.W, onhwc.C);
			}
			image_draw_rectange(im, x1, y1, w, h, 0x00FF00);
			char text[128];
			snprintf(text, sizeof(text), "%d/%d %.1f%%", i, num_det, prop*100);
			image_draw_text(im, (int)x1, (int)y1, text,  0xFF0000);
		}

		delete [] im_data;
		image_save(im, "predictions.png");
		image_close(im);
		image_close(resized_im);
		printf("found %d faces:\n", num_det);
	} else {
		printf("loading face detect net failed\n");
	}

	if(FDN) nn_destory(FDN);
	if(dllFDN) dlclose(dllFDN);
	return input;
}
#endif
static void* load_output(const char* path, int id, size_t* sz)
{
	char name[256];
	snprintf(name, sizeof(name), "%s/output%d.raw", path, id);

	return nnt_load(name, sz);
}

static int ssd_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg)
{
	int r = 0;
	int i;
	float IoU;
	int num_det = nn->network->outputs[0]->layer->C->context->nhwc.N;
	image_t* im;

	if(g_InputImagePath != NULL)
	{
		im = image_open(g_InputImagePath);
		assert(im != NULL);

		for(int i=0; i<num_det; i++)
		{
			float batch = output[7*i];
			int label = output[7*i+1];
			float prop = output[7*i+2];

			int x = output[7*i+3]*im->w;
			int y = output[7*i+4]*im->h;
			int w = output[7*i+5]*im->w-x;
			int h = output[7*i+6]*im->h-y;

			const char* name = "unknow";
			if(label < ARRAY_SIZE(voc_names_for_ssd))
			{
				name = voc_names_for_ssd[label];
			}

			printf("predict L=%s(%d) P=%.2f @%d %d %d %d\n", name, label, prop, x, y, w, h);
			image_draw_rectange(im, x, y, w, h, 0x00FF00);

			char text[128];

			snprintf(text, sizeof(text), "%s %.1f%%", name, prop*100);
			image_draw_text(im, x, y, text,  0xFF0000);
		}

		image_save(im, "predictions.png");
		printf("checking predictions.png for %s\n", g_InputImagePath);
#ifdef _WIN32
		system("predictions.png");
#else
		system("eog predictions.png");
#endif
		image_close(im);
	}
	else
	{
		EXPECT_EQ(num_det, szg/7);

		for(i=0; i<num_det; i++)
		{
			IoU = ssd::JaccardOverlap(&output[7*i+3], &gloden[7*i+3]);

			EXPECT_EQ(output[7*i], gloden[7*i]);	/* batch */
			EXPECT_EQ(output[7*i+1], gloden[7*i+1]); /* label */
			EXPECT_NEAR(output[7*i+2], gloden[7*i+2], 0.05); /* prop */
			EXPECT_GT(IoU, 0.9);

			if(output[7*i] != gloden[7*i])
			{
				r = -1;
			}

			if(output[7*i+1] != gloden[7*i+1])
			{
				r = -2;
			}

			if(std::fabs(output[7*i+2]-gloden[7*i+2]) > 0.05)
			{
				r = -3;
			}

			if(IoU < 0.9)
			{
				r = -4;
			}
		}

		if(0 != r)
		{
			printf("output for image %d is not correct\n", id);
		}
	}

	return r;
}

static int yolov3_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg)
{
	int r = 0;
	image_t* im;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;

	int netw = context->nhwc.W;
	int neth = context->nhwc.H;

	EXPECT_EQ(context->nhwc.C, 3);
	if(g_InputImagePath != NULL)
	{
		im = image_open(g_InputImagePath);
		assert(im != NULL);
		int num_det = nn->network->outputs[0]->layer->C->context->nhwc.N;

		for(int i=0; i<num_det; i++)
		{
			float batch = output[7*i];
			int label = output[7*i+1];
			float prop = output[7*i+2];

			int new_w=0;
			int new_h=0;
			if (((float)netw/im->w) < ((float)neth/im->h)) {
				new_w = netw;
				new_h = (im->h * netw)/im->w;
			} else {
				new_h = neth;
				new_w = (im->w * neth)/im->h;
			}

			float bx = output[7*i+3];
			float by = output[7*i+4];
			float bw = output[7*i+5];
			float bh = output[7*i+6];
			bx =  (bx - (netw - new_w)/2./netw) / ((float)new_w/netw);
			by =  (by - (neth - new_h)/2./neth) / ((float)new_h/neth);
			bw *= (float)netw/new_w;
			bh *= (float)neth/new_h;

			int x = bx*im->w;
			int y = by*im->h;
			int w = bw*im->w;
			int h = bh*im->h;

			x -= w/2;
			y -= h/2;

			const char* name = "unknow";
			if(label < ARRAY_SIZE(coco_names_for_yolo))
			{
				name = coco_names_for_yolo[label];
			}

			printf("predict L=%s(%d) P=%.2f @%d %d %d %d\n", name, label, prop, x, y, w, h);
			image_draw_rectange(im, x, y, w, h, 0x00FF00);

			char text[128];

			snprintf(text, sizeof(text), "%s %.1f%%", name, prop*100);
			image_draw_text(im, x, y, text, 0xFF0000);
		}

		image_save(im, "predictions.png");
		printf("checking predictions.png for %s\n", g_InputImagePath);
#ifdef _WIN32
		system("predictions.png");
#else
		system("eog predictions.png");
#endif
		image_close(im);
	}

	return r;
}

static int vehicle_attributes_recognition_barrier_0039_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg)
{
	static const char* types[] = { "car", "bus", "truck", "van" };
	static const char* colors[] = { "white", "gray", "yellow", "red", "green", "blue", "black" };

	printf("type: ");
	for(int i=0; i<szo; i++)
	{
		printf("%s: %.3f, ", types[i], output[i]);
	}
	printf("\n");

	float* out = (float*)nn->network->outputs[1]->data;

	printf("color: ");
	for(int i=0; i<7; i++)
	{
		printf("%s: %.3f, ", colors[i], out[i]);
	}
	printf("\n");
	return 0;
}

static int enet_compare(nn_t* nn, int id, float* output, size_t szo, float* gloden, size_t szg)
{
	NHWC_t* nhwc = &(nn->network->outputs[0]->layer->C->context->nhwc);
	const char* labels = "gtest/models/enet/cityscapes19.png";
	image_t* color_im = image_open(labels);
	if(color_im != NULL) {
		image_t* im = image_create(nhwc->W, nhwc->H, 3);
		for(int h=0; h<nhwc->H; h++) {
			for(int w=0; w<nhwc->W; w++) {
				int index = 0;
				int offset = (h*nhwc->W+w)*nhwc->C;
				float max = output[offset];
				for(int c=1; c<nhwc->C; c++) {	/* argmax */
					float value = output[offset+c];
					if(value > max) {
						max = value;
						index = c;
					}
				}
				uint32_t color = (color_im->data[3*index]<<16) + (color_im->data[3*index+1]<<8) + color_im->data[3*index+2];
				image_draw_pixel(im, w, h, color);
			}
		}
		image_save(im, "predictions.png");
		printf("checking predictions.png for %s\n", g_InputImagePath?:"null");
		#ifdef _WIN32
		system("predictions.png");
		#else
		system("eog predictions.png");
		#endif
		image_close(im);
		image_close(color_im);
	} else {
		printf("file %s not exists\n", labels);
	}
	return 0;
}
static int kws_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg)
{
	static const char* kws[] = {"Silence", "Unknown","yes","no","up","down","left","right","on","off","stop","go"};
	printf("kws: ");
	for(int i=0; i<szo; i++)
	{
		printf("%s: %.3f, ", kws[i], output[i]);
	}
	printf("\n");
	return 0;
}

static int ds_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg)
{
	static const char* alphabet[] = {" ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "'"};
	int classes = NHWC_BATCH_SIZE(nn->network->outputs[0]->layer->C->context->nhwc);
	int n = szo / classes;
	printf("stt %d/%d:", n, classes);
	for(int i=0; i<n; i++) {
		float max = output[i*classes];
		int argmax = 0;
		for(int j=1; j<classes; j++) {
			if(output[i*classes+j] > max) {
				argmax = j;
				max = output[i*classes+j];
			}
		}
		if(argmax < ARRAY_SIZE(alphabet)) {
			printf("%s", alphabet[argmax]);
		} else {
			printf(" ");
		}
	}
	printf("\n");
	return 0;
}

static int maskrcnn_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg)
{
	image_t* im = NULL;
	layer_context_t* context = (layer_context_t*)nn->network->inputs[0]->layer->C->context;
	char* pos;
	float* mrcnn_detection = (float*)nn->network->outputs[1]->data;
	float* mrcnn_mask = (float*)nn->network->outputs[3]->data;
	int num_det = nn->network->outputs[1]->layer->C->context->nhwc.N;
	static const char* class_names[] = {"BG", "person", "bicycle", "car", "motorcycle", "airplane",
			"bus", "train", "truck", "boat", "traffic light",
			"fire hydrant", "stop sign", "parking meter", "bench", "bird",
			"cat", "dog", "horse", "sheep", "cow", "elephant", "bear",
			"zebra", "giraffe", "backpack", "umbrella", "handbag", "tie",
			"suitcase", "frisbee", "skis", "snowboard", "sports ball",
			"kite", "baseball bat", "baseball glove", "skateboard",
			"surfboard", "tennis racket", "bottle", "wine glass", "cup",
			"fork", "knife", "spoon", "bowl", "banana", "apple",
			"sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza",
			"donut", "cake", "chair", "couch", "potted plant", "bed",
			"dining table", "toilet", "tv", "laptop", "mouse", "remote",
			"keyboard", "cell phone", "microwave", "oven", "toaster",
			"sink", "refrigerator", "book", "clock", "vase", "scissors",
			"teddy bear", "hair drier", "toothbrush"};
	int H = nn->network->inputs[0]->layer->C->context->nhwc.H;
	int W = nn->network->inputs[0]->layer->C->context->nhwc.W;
	int mH = nn->network->outputs[3]->layer->C->context->nhwc.H;
	int mW = nn->network->outputs[3]->layer->C->context->nhwc.W;
	int mC = nn->network->outputs[3]->layer->C->context->nhwc.C;
	assert(mC == ARRAY_SIZE(class_names));
	pos = strstr((char*)g_InputImagePath, ".raw");
	if(NULL != pos) {
		im = image_create(W,H,3);
		if(NULL != im) {
			float* input = (float*)nn->network->inputs[0]->data;
			for(int i=0; i<W*H*3; i+=3)
			{
				im->data[i] = input[i]+123.7;
				im->data[i+1] = input[i+1]+116.8;
				im->data[i+2] = input[i+2]+103.9;
			}
		}
	} else {
		im = image_open(g_InputImagePath);
	}
	if(NULL != im) {
		float scale = std::min((float)H/im->h, (float)W/im->w);
		float y_top = (H-scale*im->h)/2;
		float x_top = (W-scale*im->w)/2;
		float y_bottom = y_top + scale*im->h;
		float x_bottom = x_top + scale*im->w;

		for(int i=0; i<num_det; i++)
		{
			float batch = mrcnn_detection[7*i];
			int label = mrcnn_detection[7*i+1];
			float prop = mrcnn_detection[7*i+2];

			int y1 = (int)std::round((mrcnn_detection[7*i+3]*H-y_top)/scale);
			int x1 = (int)std::round((mrcnn_detection[7*i+4]*W-x_top)/scale);
			int y2 = (int)std::round((mrcnn_detection[7*i+5]*H-y_top)/scale);
			int x2 = (int)std::round((mrcnn_detection[7*i+6]*W-x_top)/scale);

			int x = std::max(x1,0);
			int y = std::max(y1,0);
			int w = std::min(std::max(x2-x1,0), im->w);
			int h = std::min(std::max(y2-y1,0), im->h);

			const char* name = "unknow";
			if(label < ARRAY_SIZE(class_names))
			{
				name = class_names[label];
			}

			printf("predict L=%s(%d) P=%.2f @%d %d %d %d\n", name, label, prop, x, y, w, h);
			image_draw_rectange(im, x, y, w, h, 0x00FF00);
			char text[128];
			snprintf(text, sizeof(text), "%s %.1f%%", name, prop*100);
			image_draw_text(im, x, y, text,  0xFF0000);

			float* o = new float[w*h];
			float* in = new float[mW*mH];
			float boxes[4] = {0.f,0.f,1.f,1.f};
			int indices[1] = {0};
			NHWC_t onhwc = {1,h,w,1};
			NHWC_t inhwc = {1,mH,mW,1};
			for(int mx=0; mx<mH; mx++) {
				for(int my=0; my<mW; my++) {
					in[my*mW+mx] = mrcnn_mask[((i*mH+my)*mW+mx)*mC+label];
				}
			}
			(void)ROIAlign_forward_cpu(o, in, boxes, indices, &onhwc, &inhwc);
			for(int dx=0; dx<w; dx++) {
				for(int dy=0; dy<h; dy++) {
					float mask = o[dy*w+dx];
					if(mask > 0.5) {
						image_draw_pixel(im, x+dx, y+dy, 0x4000FF00);
					}
				}
			}
			delete []o;
			delete []in;
		}

		image_save(im, "predictions.png");
		printf("checking predictions.png for %s\n", g_InputImagePath?:"null");
		#ifdef _WIN32
		system("predictions.png");
		#else
		system("eog predictions.png");
		#endif
		image_close(im);
	}

	return 0;
}

static float euclidean_distances(float *a, float *b, int size) {
	float dis = 0.0f;

	for(int i=0; i<size; i++) {
		float delta = a[i]-b[i];
		dis += delta*delta;
	}

	return dis;
}
static int facenet_compare(nn_t* nn, int id, float * output, size_t szo, float* gloden, size_t szg)
{
	int features = NHWC_BATCH_SIZE(nn->network->outputs[0]->layer->C->context->nhwc);
	int n = szo / features;
	const float threshold = 1.05;

	for(int i=1; i<n; i++) {
		printf("%d:", i);
		for(int j=0; j<i; j++) {
			float dis = euclidean_distances(&output[i*features], &output[j*features], features);
			if(dis < threshold) {
				printf(" %d(%.2f)", j, dis);
			}
		}
		printf("\n");
	}
	return 0;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void ModelTestMain(runtime_type_t runtime,
		const network_t* network,
		const char* input,
		const char* output,
		const nnt_model_args_t* args,
		float mintop1)
{
	int r = 0;
	size_t x_test_sz;
	size_t y_test_sz;
	float* x_test = NULL;
	int32_t* y_test = NULL;

	const nn_input_t* const * inputs = network->inputs;
	const nn_output_t* const * outputs = network->outputs;

	int H,W,C,B;
	int classes;

	auto tcreate_s = std::chrono::high_resolution_clock::now();
	nn_t* nn = nn_create(network, runtime);
	ASSERT_TRUE(nn != NULL);
	auto tcreate_e = std::chrono::high_resolution_clock::now();
	double tcreate = std::chrono::duration_cast<std::chrono::nanoseconds>(tcreate_e-tcreate_s).count();

	double trun_sum = 0;
	size_t fps = 0;

	if(NULL == nn)
	{
		return;
	}
	/* This demo code is really a mess, didn't care about memory leak issue, just for test */
	H = inputs[0]->layer->C->context->nhwc.H;
	W = inputs[0]->layer->C->context->nhwc.W;
	C = inputs[0]->layer->C->context->nhwc.C;
	classes = NHWC_BATCH_SIZE(outputs[0]->layer->C->context->nhwc);
	if(NULL == args)
	{
		x_test = (float*)nnt_load(input, &x_test_sz);
		y_test = (int32_t*)nnt_load(output,&y_test_sz);
		B = x_test_sz/(H*W*C*sizeof(float));
		ASSERT_EQ(B, y_test_sz/sizeof(int32_t));
	}
	else if((args->flags&ARGS_PREFETCH_ALL) != 0)
	{
		x_test = (float*)args->load_input(nn, input, -1, &x_test_sz);
		ASSERT_TRUE(x_test != NULL);
		B = x_test_sz/(H*W*C*sizeof(float));
	}
	else
	{
		B = args->n;
	}

	void* IN;

	size_t top1 = 0;
	for(int i=0; (i<B) && (r==0); i++)
	{
		if(g_CaseNumber != -1)
		{
			i = g_CaseNumber;
		}
		float* in;
		size_t sz_in;
		float* golden = NULL;
		size_t sz_golden;

		if((NULL == args) || ((args->flags&ARGS_PREFETCH_ALL) != 0))
		{
			in = x_test+H*W*C*i;
		}
		else
		{
			in = (float*)args->load_input(nn, input, i, &sz_in);
			if(inputs[0]->layer->op != L_OP_MFCC) {
				EXPECT_EQ(sz_in, H*W*C*sizeof(float));
			}
			if(NULL == g_InputImagePath)
			{
				golden = (float*)args->load_output(input, i, &sz_golden);
				ASSERT_TRUE(golden != NULL);
			}
		}

		if(inputs[0]->layer->op == L_OP_MFCC)
		{
			sz_in = sizeof(void*)*2;
			IN = in;
		}
		else if(network->type== NETWORK_TYPE_Q8)
		{
			sz_in = H*W*C;
			IN = nnt_quantize8(in, H*W*C, LAYER_Q(inputs[0]->layer));
			ASSERT_TRUE(IN != NULL);
		}
		else if(network->type== NETWORK_TYPE_S8)
		{
			sz_in = H*W*C;
			IN = nnt_quantize8(in, H*W*C, LAYER_Q(inputs[0]->layer),
						LAYER_Z(inputs[0]->layer),
						(float)LAYER_S(inputs[0]->layer)/NN_SCALER);
			ASSERT_TRUE(IN != NULL);
		}
		else if(network->type== NETWORK_TYPE_Q16)
		{
			sz_in = H*W*C*sizeof(int16_t);
			IN = nnt_quantize16(in, H*W*C, LAYER_Q(inputs[0]->layer));
			ASSERT_TRUE(IN != NULL);
		}
		else
		{
			sz_in = H*W*C*sizeof(float);
			IN = in;
		}

		memcpy(inputs[0]->data, IN, sz_in);

		auto trun_s = std::chrono::high_resolution_clock::now();
		r = nn_predict(nn);
		EXPECT_EQ(0, r);
		auto trun_e = std::chrono::high_resolution_clock::now();

		trun_sum = std::chrono::duration_cast<std::chrono::nanoseconds>(trun_e-trun_s).count();
		fps ++;

		if(0 == r)
		{
			int y=-1;
			float prob = 0;
			float* out = (float*)outputs[0]->data;
			if( (outputs[0]->layer->op == L_OP_DETECTIONOUTPUT) ||
				(outputs[0]->layer->op == L_OP_YOLOOUTPUT))
			{
				/* already in float format */
			}
			else if(network->type== NETWORK_TYPE_Q8)
			{
				out = nnt_dequantize8((int8_t*)out, classes, LAYER_Q(outputs[0]->layer));
			}
			else if(network->type== NETWORK_TYPE_S8)
			{
				out = nnt_dequantize8((int8_t*)out, classes, LAYER_Q(outputs[0]->layer),
						LAYER_Z(outputs[0]->layer),
						(float)LAYER_S(outputs[0]->layer)/NN_SCALER);
			}
			else if(network->type== NETWORK_TYPE_Q16)
			{
				out = nnt_dequantize16((int16_t*)out, classes, LAYER_Q(outputs[0]->layer));
			}

			if(NULL == args)
			{
				for(int j=0; j<classes; j++)
				{
					if(out[j] > prob)
					{
						prob = out[j];
						y = j;
					}
				}

				EXPECT_GE(y, 0);

				if(y == y_test[i])
				{
					top1 ++;
				}
			}
			else if((args->flags&ARGS_COMPARE_ALL_AT_END) != 0)
			{
				static float* final_o = NULL;
				if(0 == i) {
					final_o = (float*)malloc(B*classes*sizeof(float));
				}
				memcpy(&final_o[i*classes], out, classes*sizeof(float));

				if(i == (B-1)) {
					y = args->compare(nn, -1, final_o, B*classes, NULL, 0);
					free(final_o);
				}
			}
			else
			{
				y = args->compare(nn, i, out, classes, golden, sz_golden/sizeof(float));
				if(0 == y)
				{
					top1 ++;
				}

				free(in);
				if(NULL != golden) free(golden);
			}

			if(out != outputs[0]->data)
			{
				free(out);
			}
			if((args != NULL) && ((args->flags&ARGS_COMPARE_ALL_AT_END) != 0)) {
				/* pass */
			}
			else if((g_CaseNumber != -1) || (g_InputImagePath != NULL))
			{
				if(NULL == args)
				{
					printf("image %d predict as %d%s%d with prob=%.2f\n", i, y, (y==y_test[i])?"==":"!=", y_test[i], prob);
				}
				break;
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

	if((-1 == g_CaseNumber) && (NULL == g_InputImagePath))
	{
		printf("LWNN TOP1 is %f\n", (float)top1/B);
		EXPECT_GT(top1, B*mintop1);
	}
	nn_destory(nn);

	if(NULL != x_test)
	{
		free(x_test);
	}
	if(NULL != y_test)
	{
		free(y_test);
	}

	printf("Create cost %.3fms , Inference cost avg %.3fms, total %.3fms\n", tcreate/1000000, trun_sum/fps/1000000, trun_sum/1000000);
}

void NNTModelTestGeneral(runtime_type_t runtime,
		const char* netpath,
		const char* input,
		const char* output,
		const void* args,
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
	ModelTestMain(runtime, network, input, output, (const nnt_model_args_t*)args, mintop1);
	dlclose(dll);
}

NNT_MODEL_TEST_ALL(MNIST)

NNT_MODEL_TEST_ALL(UCI_INCEPTION)

NNT_MODEL_TEST_ALL(SSD)

NNT_MODEL_TEST_ALL(YOLOV3)

NNT_MODEL_TEST_ALL(YOLOV3TINY)

NNT_MODEL_TEST_ALL(VEHICLE_ATTR)

NNT_MODEL_TEST_ALL(ENET)

NNT_MODEL_TEST_ALL(KWS)

NNT_MODEL_TEST_ALL(DS)

NNT_MODEL_TEST_ALL(MASKRCNN)

NNT_MODEL_TEST_ALL(FACENET)
