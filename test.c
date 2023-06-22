// run gcc test.c -lm -lssl -lcrypto
#include <openssl/bn.h>
#include <stdio.h>

int main(){
    int size = 128;

    
    BIGNUM *a = BN_new();
    BN_set_word(a, 3);
    printf("%s", BN_bn2dec(a));
    BN_free(a);   
}