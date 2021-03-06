/**
 * LWNN - Lightweight Neural Network
 * Copyright (C) 2019  Parai Wang <parai@foxmail.com>
 */

__kernel void relu(
		__read_only image2d_t in,
		__write_only image2d_t out)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

	int x = get_global_id(0);
	int y = get_global_id(1);

	float4 a = read_imagef(in, sampler, (int2)(x,y));
	float4 value = fmax(a,(float)0);

	write_imagef(out, (int2)(x,y), value);
}

__kernel void clip(
		__read_only image2d_t in,
		__write_only image2d_t out,
		float min,
		float max)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

	int x = get_global_id(0);
	int y = get_global_id(1);

	float4 a = read_imagef(in, sampler, (int2)(x,y));
	float4 value = fmin(fmax(a, max), min);

	write_imagef(out, (int2)(x,y), value);
}

__kernel void prelu(
		__read_only image2d_t in,
		__read_only image2d_t weights,
		__write_only image2d_t out,
		const int nC4)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	
	int x = get_global_id(0);
	int y = get_global_id(1);

	int c = x%nC4;

	float4 weight = read_imagef(weights, sampler, (int2)(c, 0));
	float4 value = read_imagef(in, sampler, (int2)(x,y));

	float4 out0 = select(weight * value, value, value >= (float)0);

	write_imagef(out, (int2)(x,y), out0);
}
