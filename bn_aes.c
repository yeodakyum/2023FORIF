#include <stdio.h>
#include <openssl/bn.h>
#include <openssl/aes.h>

int main(int argc, char* argv[]) {
	BIGNUM *key = BN_new();
	unsigned char user_key[16]; 
	int size = 128; // bit length
	
	AES_KEY enc_key; // AES encryption key
	AES_KEY dec_key; // AES decryption key
	
	BN_rand(key, size, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY); // get random BN key
	BN_bn2bin(key, user_key); // convert BN key to binary form
	
	AES_set_encrypt_key(user_key, size, &enc_key);
	AES_set_decrypt_key(user_key, size, &dec_key);
	
	unsigned char m[16] = "hello"; // key size = message size = 128bit
	unsigned char enc[16];
	unsigned char dec[16];
	
	AES_encrypt(m, enc, &enc_key);
	printf("enc: %s\n", enc);
	AES_decrypt(enc, dec, &dec_key);
	printf("dec: %s\n", dec);
	
	BN_free(key);
	
	return 0;
}