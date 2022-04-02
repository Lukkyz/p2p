#include "hash.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

void double_sha256(char *string, char output[65]) {
    unsigned char out[32];
    char hash[65];
    unsigned char scnd_hash[65];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(out, &sha256);
    SHA256_CTX sha256_2;
    SHA256_Init(&sha256_2);
    SHA256_Update(&sha256_2, out, 32);
    SHA256_Final(scnd_hash, &sha256_2);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", scnd_hash[i]);
    }
}
