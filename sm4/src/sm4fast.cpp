#include <liarod.h>
#include "sm4fast.h"


inline u4 F(u4 b)
{
	return Sbox_T24[get_byte(0,b)] ^ Sbox_T16[get_byte(1,b)] ^ Sbox_T8[get_byte(2,b)] ^ Sbox_T[get_byte(3,b)];
}

// Variant of T for key schedule
inline u4 SM4_Tp(u4 b)
{
	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);

	// L' linear transform
	return t ^ rotl<13>(t) ^ rotl<23>(t);
}

#define SM4_RNDS(B, k0, k1, k2, k3) {   \
  B[0] ^= F(B[1] ^ B[2] ^ B[3] ^ rkey[k0]); \
  B[1] ^= F(B[0] ^ B[2] ^ B[3] ^ rkey[k1]); \
  B[2] ^= F(B[0] ^ B[1] ^ B[3] ^ rkey[k2]); \
  B[3] ^= F(B[0] ^ B[1] ^ B[2] ^ rkey[k3]); \
}

/*
* SM4 Encryption
*/
void SM4_enc_block(u4 block[SM4_BLOCK_SIZE / sizeof u4], const u4 rkey[SM4_RND_KEY_SIZE / sizeof u4])
{
	SM4_RNDS(block, 0,  1,  2,  3);
	SM4_RNDS(block, 4,  5,  6,  7);
	SM4_RNDS(block, 8,  9, 10, 11);
	SM4_RNDS(block, 12, 13, 14, 15);
	SM4_RNDS(block, 16, 17, 18, 19);
	SM4_RNDS(block, 20, 21, 22, 23);
	SM4_RNDS(block, 24, 25, 26, 27);
	SM4_RNDS(block, 28, 29, 30, 31);

	static u4 interim;
	interim = block[0]; block[0] = block[3]; block[3] = interim;
	interim = block[1]; block[1] = block[2]; block[2] = interim;
}

/*
* SM4 Decryption
*/
void SM4_dec_block(u4 block[SM4_BLOCK_SIZE], const u4 rkey[SM4_RND_KEY_SIZE / sizeof u4])
{
	SM4_RNDS(block, 31, 30, 29, 28);
	SM4_RNDS(block, 27, 26, 25, 24);
	SM4_RNDS(block, 23, 22, 21, 20);
	SM4_RNDS(block, 19, 18, 17, 16);
	SM4_RNDS(block, 15, 14, 13, 12);
	SM4_RNDS(block, 11, 10,  9,  8);
	SM4_RNDS(block,  7,  6,  5,  4);
	SM4_RNDS(block,  3,  2,  1,  0);

	static u4 interim;
	interim = block[0]; block[0] = block[3]; block[3] = interim;
	interim = block[1]; block[1] = block[2]; block[2] = interim;
}

#undef SM4_RNDS

static inline u4 rotate(u4 input)
{
	return ((input & 0xff) << 24) | (((input >> 8) & 0xff) << 16) | (((input >> 16) & 0xff) << 8) | (((input >> 24) & 0xff));
}

/*
* SM4 Key Schedule
*/
void SM4_key_schedule(const u1 key[SM4_KEY_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof u4])
{
	// System parameter or family key
	const u4 FK[4] = { 0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc };

	const u4 CK[32] = 
	{
		0x00070E15, 0x1C232A31, 0x383F464D, 0x545B6269,
		0x70777E85, 0x8C939AA1, 0xA8AFB6BD, 0xC4CBD2D9,
		0xE0E7EEF5, 0xFC030A11, 0x181F262D, 0x343B4249,
		0x50575E65, 0x6C737A81, 0x888F969D, 0xA4ABB2B9,
		0xC0C7CED5, 0xDCE3EAF1, 0xF8FF060D, 0x141B2229,
		0x30373E45, 0x4C535A61, 0x686F767D, 0x848B9299,
		0xA0A7AEB5, 0xBCC3CAD1, 0xD8DFE6ED, 0xF4FB0209,
		0x10171E25, 0x2C333A41, 0x484F565D, 0x646B7279
	};

	u4 K[4];

	K[0] = load_be<u4>(key, 0) ^ FK[0];
	K[1] = load_be<u4>(key, 1) ^ FK[1];
	K[2] = load_be<u4>(key, 2) ^ FK[2];
	K[3] = load_be<u4>(key, 3) ^ FK[3];

	forloop (i, 0, 32)
	{
		K[i % 4] ^= SM4_Tp(K[(i+1)%4] ^ K[(i+2)%4] ^ K[(i+3)%4] ^ CK[i]);
		rkey[i] = K[i % 4];
	}

	forloop( i, 0, SM4_RND_KEY_SIZE / sizeof u4 )
		rkey[i] = rotate(rkey[i]);
}

#include <time.h>
int main()
{
	// plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// cipher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46	
	u1 key[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	u1 p[SM4_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	u1 c[SM4_BLOCK_SIZE];

	u4 rkey[SM4_RND_KEY_SIZE / sizeof u4];
	SM4_key_schedule( key, rkey );
	
	u4 block[4];
	memcpy(block, p, 16);
		
	SM4_enc_block( block, rkey );
	memcpy(c, block, 16);

	// should be: 0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
	outputChar(c, sizeof c);

	memcpy(block, c, 16);
	SM4_dec_block( block, rkey );
	memcpy(p, block, 16);
	outputChar(p, sizeof p);

	// return 0;
	
	clock_t t = clock();
	
	forloop (i, 0, 10000000)
	{
		SM4_enc_block( block, rkey );
	}

	printf("time: %d ms\n", clock() - t );
}