#include <openssl/sha.h> 
#include "config.h"
#include "string.h"
#include "message.h"
#include <stdlib.h>
#include "stdio.h"
#include <time.h>

struct VersionMessage *version_msg_new(char addr_recv[16], char addr_sender[16]) {
    struct VersionMessage *version_msg = malloc(sizeof(*version_msg)); 
    version_msg->version = VERSION;
    version_msg->services = 1;
    version_msg->timestamp = (int)time(NULL);
    strcpy(version_msg->addr_recv, addr_recv);
    strcpy(version_msg->addr_sender, addr_sender);
    version_msg->msg_header = NULL;
    return version_msg;
}

void sha256_string(char *string, char output_buff[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
    for (int i = 0;i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output_buff + (i*2), "%02x", hash[i]);
    }
}

struct MessageHeader *header_msg_new(uint32_t payload_size, char *data) {
    struct MessageHeader *msg_header = malloc(sizeof(*msg_header));
    strcpy(msg_header->network, NETWORK);
    strcpy(msg_header->command, "version");
    msg_header->payload_size = payload_size;
    char sha_buff[65];
    sha256_string(data, sha_buff);
    for (int i = 0; i < 4; i++) {
        msg_header->checksum[i] = sha_buff[i];
    }
    return msg_header;
}

void msg_to_string(struct MessageHeader *msg_header, char *string) {    
    sprintf(string, "%.4s\r\n%s\r\n%d\r\n%s\r\n", msg_header->network, msg_header->command, msg_header->payload_size, msg_header->checksum);
}
