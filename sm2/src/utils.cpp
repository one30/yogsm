#include "dataType.h"
#include "ecc.h"
#include <time.h>

uint32_t rol(const uint32_t value, const size_t bits)
{
	return (value << bits) | (value >> (32 - bits));
}

void print_u32(const u32 & input)
{
	printf("%016llX %016llX %016llX %016llX\n", input.v[3], input.v[2], input.v[1], input.v[0]);
}

void print_affine_point(const affine_point & point)
{
	printf("%016llX %016llX %016llX %016llX\n", point.x.v[3], point.x.v[2], point.x.v[1], point.x.v[0]);
	printf("%016llX %016llX %016llX %016llX\n", point.y.v[3], point.y.v[2], point.y.v[1], point.y.v[0]);
}

void print_jacobian_point(const jacobian_point & point)
{
	printf("%016llX %016llX %016llX %016llX\n", point.x.v[3], point.x.v[2], point.x.v[1], point.x.v[0]);
	printf("%016llX %016llX %016llX %016llX\n", point.y.v[3], point.y.v[2], point.y.v[1], point.y.v[0]);
	printf("%016llX %016llX %016llX %016llX\n", point.z.v[3], point.z.v[2], point.z.v[1], point.z.v[0]);
}

// not safe
u8 llrand()
{
	u8 r = 0;
	srand((int)time(0));
	for (int i = 0; i < 5; ++i) {
		r = (r << 15) | (rand() & 0x7FFF);
	}

	return r & 0xFFFFFFFFFFFFFFFFULL;
}

void rand_element(u32 & input)
{
	input = {llrand(), llrand(), llrand(), llrand()};
}


void str_reverse_in_place(u1 *str, int len)
{
	u1 *p1 = str;
	u1 *p2 = str + len - 1;

	while (p1 < p2)
	{
		u1 tmp = *p1;
		*p1++ = *p2;
		*p2-- = tmp;
	}
}


void u1_to_u32(u1 input[32], u32 & result)
{
	/*
	for (int i = 3; i >= 0; i--)
	{
		(*result).v[i] = ((u8)x[(3 - i) * 8] << 56) + ((u8)x[(3 - i) * 8 + 1] << 48) + 
			((u8)x[(3 - i) * 8 + 2] << 40) + ((u8)x[(3 - i) * 8 + 3] << 32) + 
			((u8)x[(3 - i) * 8 + 4] << 24) + ((u8)x[(3 - i) * 8 + 5] << 16) + 
			((u8)x[(3 - i) * 8 + 6] << 8) + ((u8)x[(3 - i) * 8 + 7]);
	}
	*/
	str_reverse_in_place(input, 32);
	memcpy(&result, input, 32);
}

void u4_to_u32(u4 input[8], u32 & result)
{
	result.v[0] = ((u8)input[1] << 32) + (u8)input[0];
	result.v[1] = ((u8)input[3] << 32) + (u8)input[2];
	result.v[2] = ((u8)input[5] << 32) + (u8)input[4];
	result.v[3] = ((u8)input[7] << 32) + (u8)input[6];
}

