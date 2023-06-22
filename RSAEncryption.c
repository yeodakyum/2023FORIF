#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/dh.h>

typedef unsigned char U8;

typedef struct
{
	BIGNUM *d;
	BIGNUM *x;
	BIGNUM *y;
}BN_dxy;

BN_dxy BN_dxy_new(const BIGNUM *d, const BIGNUM *x, const BIGNUM *y) {
	BN_dxy dxy;
	dxy.d = BN_new(); dxy.x = BN_new(); dxy.y = BN_new();
	if (d == NULL)
		return dxy;
	BN_copy(dxy.d, d);
	BN_copy(dxy.x, x);
	BN_copy(dxy.y, y);
	return dxy;
}

int BN_dxy_copy(BN_dxy * dxy, BIGNUM *d, BIGNUM *x, BIGNUM *y)
{
	BN_copy(dxy->d, d);
	BN_copy(dxy->x, x);
	BN_copy(dxy->y, y);
}

void BN_dxy_free(BN_dxy * dxy)
{
	BN_free(dxy->d);
	BN_free(dxy->x);
	BN_free(dxy->y);
}

BIGNUM* BN_Square_Multi(const BIGNUM *x, const BIGNUM *a, const BIGNUM *n)
{
	BIGNUM *z = BN_new();
	BN_CTX *ctx = BN_CTX_new();
	BN_set_word(z, 1);
	int size = BN_num_bits(a);
	
	for(int i = size - 1; i >= 0; i--){
		BN_mod_sqr(z, z, n, ctx);
		if(BN_is_bit_set(a, i))
			BN_mod_mul(z, z, x, n, ctx);
	}
	
	BN_CTX_free(ctx);
	
	return z;
}

BN_dxy BN_Ext_Euclid(BIGNUM* a, BIGNUM* b) {
	
	BN_dxy dxy;
	
	if (BN_is_zero(b)) {
		dxy = BN_dxy_new(a, BN_value_one(), b);
		return dxy;
	}
	else {
		BIGNUM *temp = BN_new();
		BN_CTX *ctx = BN_CTX_new();

		BN_mod(temp, a, b, ctx);
		dxy = BN_Ext_Euclid(b, temp);

		BN_div(temp, NULL, a, b, ctx);
		BN_mul(temp, temp, dxy.y, ctx);
		BN_sub(temp, dxy.x, temp);
		
		BN_dxy_copy(&dxy, dxy.d, dxy.y, temp);
		
		BN_free(temp);
		BN_CTX_free(ctx);
		
		return dxy;
	}
}

void RSA_setup(BIGNUM *pub_e, BIGNUM* pub_N, BIGNUM* priv)
{
	BIGNUM *p = BN_new();
	BIGNUM *q = BN_new();
	
	// p, q : pick 1024 bits pirme
	BN_generate_prime_ex(p, 1024, 1, NULL, NULL, NULL);
	BN_generate_prime_ex(q, 1024, 1, NULL, NULL, NULL);
	
	// N = p * q
	BN_CTX *ctx = BN_CTX_new();
	BN_mul(pub_N, p, q, ctx);
	
	// phi(N) = (p - 1) * (q - 1)
	BIGNUM *phi = BN_new();
	BN_sub_word(p, 1);
	BN_sub_word(q, 1);
	BN_mul(phi, p, q, ctx);
	
	// e : e and phi(N) are relatively prime
	BN_set_word(pub_e, 65537);
	
	// d : e^-1 mod phi(N)
	BN_dxy dxy;
	dxy = BN_Ext_Euclid(pub_e, phi);
	BN_nnmod(priv, dxy.x, phi, ctx);
	
	BN_dxy_free(&dxy);
	BN_CTX_free(ctx);
	BN_free(p);
	BN_free(q);
	BN_free(phi);
}

U8 * RSA_enc(const U8 * msg, BIGNUM * pub_e, BIGNUM * pub_N)
{
	BIGNUM *C = BN_new();
	BIGNUM *M = BN_new();
	BN_bin2bn(msg, strlen(msg), M);
	U8 *cipher;
	
	// c = m^e mod N
	BN_CTX *ctx = BN_CTX_new();
	BN_mod_exp(C, M, pub_e, pub_N, ctx);
	cipher = BN_bn2hex(C);
	
	BN_CTX_free(ctx);
	BN_free(C);
	BN_free(M);
	
	return cipher;
}
	
int RSA_dec(U8 *dec_msg, const BIGNUM *priv, const BIGNUM *pub_N, const U8 * cipher)
{
	BIGNUM * C = BN_new();
	BIGNUM * M = BN_new();
	BN_hex2bn(&C, cipher);
	
	// m = c^d mod N
	BN_CTX *ctx = BN_CTX_new();
	BN_mod_exp(M, C, priv, pub_N, ctx);
	BN_bn2bin(M, dec_msg);
	int msg_len = strlen(dec_msg);
	
	BN_CTX_free(ctx);
	BN_free(C);
	BN_free(M);	

	return msg_len;
}

int main() {
	U8 *msg = "hello";
	BIGNUM * e = BN_new();
	BIGNUM * d = BN_new();
	BIGNUM * N = BN_new();
	RSA_setup(e, N, d);
	printf("N: %s\ne: %s\nd: %s\n\n", BN_bn2hex(N), BN_bn2hex(e), BN_bn2hex(d));
	U8 * cipher = RSA_enc(msg, e, N);
	printf("Cipher text : %s\n", cipher);
	U8 dec_msg[1024] = { 0 };
	int dec_len = RSA_dec(dec_msg, d, N, cipher);
	printf("dec : %s\nmsg_len : %d\n", dec_msg, dec_len);

	BN_free(e);
	BN_free(N);
	BN_free(d);
	return 0;
}