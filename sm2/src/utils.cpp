#include "dataType.h"
#include "ecc.h"
#include <time.h>

#include <random>

void random_fill_non_zero(u1 * buffer, size_t len)
{
	std::random_device rd;

	forloop(i, 0, len)
	{
		do
		{
			buffer[i] = rd() & 0xff;
		} while (buffer[i] == 0);
	}
}

void random_fill(u1 * buffer, size_t len)
{
	std::random_device rd;

	forloop (i, 0, len)
		buffer[i] = rd() & 0xff;
}

uint32_t rol(const uint32_t value, const size_t bits)
{
	return (value << bits) | (value >> (32 - bits));
}

#define U256_DISPLAY
void print_u32(const u32 & input)
{
#ifdef U256_DISPLAY
	printf("%016llx%016llx%016llx%016llx\n", input.v[3], input.v[2], input.v[1], input.v[0]);
#else
	printf("0x%016llx, 0x%016llx, 0x%016llx 0x%016llx\n", input.v[0], input.v[1], input.v[2], input.v[3]);
#endif
}

void print_affine_point(const AFPoint & point)
{
	printf("%016llx %016llx %016llx %016llx\n", point.x.v[3], point.x.v[2], point.x.v[1], point.x.v[0]);
	printf("%016llx %016llx %016llx %016llx\n", point.y.v[3], point.y.v[2], point.y.v[1], point.y.v[0]);
}

void print_jacobian_point(const JPoint & point)
{
	printf("%016llx %016llx %016llx %016llx\n", point.x.v[3], point.x.v[2], point.x.v[1], point.x.v[0]);
	printf("%016llx %016llx %016llx %016llx\n", point.y.v[3], point.y.v[2], point.y.v[1], point.y.v[0]);
	printf("%016llx %016llx %016llx %016llx\n", point.z.v[3], point.z.v[2], point.z.v[1], point.z.v[0]);
}

u8 u8_rand()
{
	u8 r;

	random_fill( (u1 *)(&r), 8 );

	return r;
}

void u32_rand(u32 & input)
{
	input = { u8_rand(), u8_rand(), u8_rand(), u8_rand() };
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
	str_reverse_in_place(input, 32);
	memcpy(&result, input, 32);
}

void u4_to_u32(u4 input[8], u32 & result)
{
	/*
	result.v[0] = *(u8 *)(input + 0);
	result.v[1] = *(u8 *)(input + 2);
	result.v[2] = *(u8 *)(input + 4);
	result.v[3] = *(u8 *)(input + 6);
	*/

	result.v[0] = ((u8)input[1] << 32) + (u8)input[0];
	result.v[1] = ((u8)input[3] << 32) + (u8)input[2];
	result.v[2] = ((u8)input[5] << 32) + (u8)input[4];
	result.v[3] = ((u8)input[7] << 32) + (u8)input[6];
}

