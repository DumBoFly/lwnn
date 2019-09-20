/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */
#ifndef NN_THIRD_PARTY_IMAGE_IMAGE_H_
#define NN_THIRD_PARTY_IMAGE_IMAGE_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdint.h>
/* ============================ [ MACROS    ] ====================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/* ============================ [ TYPES     ] ====================================================== */
typedef struct {
	int w;
	int h;
	int c;
	uint8_t *data;
} image_t;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
image_t* image_open(const char *filename);
image_t* image_create(int w, int h, int c);
void image_close(image_t *im);
void image_save(image_t* im, const char *filename);
void image_fill(image_t* im, uint8_t color);
image_t* image_resize(image_t* im, int w, int h);
void image_embed(image_t* dest, image_t* source);
image_t* image_letterbox(image_t* im, int w, int h);
#ifdef __cplusplus
}
#endif
#endif /* NN_THIRD_PARTY_IMAGE_IMAGE_H_ */
