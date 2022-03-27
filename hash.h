#ifndef HASH_H
#define HASH_H

void digest(char *string, unsigned char output[32]);
void sha256_tostring(unsigned char digest[32], char output[65]);
void double_hash(char *string, char *output);
#endif
