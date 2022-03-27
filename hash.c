#include "hash.h"
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>

void digest(char *string, unsigned char output[32]) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(output, &sha256);
}

void sha256_tostring(unsigned char digest[32], char output[65]) {
	for (int i = 0;i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i*2), "%02x", digest[i]);
    }
}

void double_hash(char *string, char *output) {
	unsigned char out[32];
	digest(string, out);
	unsigned char scnd_output[65];
	digest((char*)out, scnd_output);
	char final_hash[65];
	sha256_tostring(scnd_output, final_hash);
	printf("%s", final_hash);

}
