#ifndef MESSAGE_H
#define MESSAGE_H

struct MessageHeader {
    char network[4];
    char command[12];
    uint32_t payload_size;
    char checksum[4];
};

struct VersionMessage {
    int32_t version;
    uint64_t services;
    int64_t timestamp;
	uint16_t addr_recv_port;
    char addr_recv[16];
};

struct VersionMessage *version_msg_new(char addr_recv[128], uint16_t port);
struct MessageHeader *header_msg_new(uint32_t payload_size, char *data); 
void msg_to_string(struct MessageHeader *msg_header, char message[], char *string);
void version_to_string(struct VersionMessage *version, char *string);
#endif
