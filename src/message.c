#include "message.h"

#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "hash.h"
#include "stdio.h"
#include "string.h"

struct VersionMessage *version_msg_new(char addr_recv[128], uint16_t port) {
    struct VersionMessage *version_msg = malloc(sizeof(*version_msg));
    version_msg->version = 1;
    version_msg->services = 1;
    version_msg->addr_recv_port = port;
    version_msg->timestamp = (int)time(NULL);
    strcpy(version_msg->addr_recv, addr_recv);
    return version_msg;
}

void version_to_string(struct VersionMessage *version, char *string) {
    sprintf(string, "%.2d\r\n%ld\r\n%d\r\n%ld\r\n\r\n%s\r\n", version->version, version->services, version->addr_recv_port, version->timestamp, version->addr_recv);
}

struct MessageHeader *header_msg_new(uint32_t payload_size, char *data) {
    struct MessageHeader *msg_header = malloc(sizeof(*msg_header));
    strcpy(msg_header->network, "1");
    strcpy(msg_header->command, "version");
    msg_header->payload_size = payload_size;
    char sha_buff[65];
    double_sha256((unsigned char *)data, sha_buff);
    for (int i = 0; i < 4; i++) {
        msg_header->checksum[i] = sha_buff[i];
    }
    return msg_header;
}

void msg_to_string(struct MessageHeader *msg_header, char message[], char *string) {
    sprintf(string, "%.4s\r\n%s\r\n%d\r\n%s\r\n\r\n%s\r\n", msg_header->network, msg_header->command, msg_header->payload_size, msg_header->checksum, message);
}
