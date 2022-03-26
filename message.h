#ifndef MESSAGE_H
#define MESSAGE_H

struct MessageHeader {
    char network[4];
    char command[12];
    uint32_t payload_size;
    char checksum[4];
};

struct VersionMessage {
    struct MessageHeader *msg_header;
    int32_t version;
    uint64_t services;
    int64_t timestamp;
    char addr_recv[16];
    char addr_sender[16];
};

struct VersionMessage *version_msg_new(char addr_recv[16], char addr_sender[16]);
void sha256_string(char *string, char outputBuff[65]);
struct MessageHeader *header_msg_new(uint32_t payload_size, char *data); 
void msg_to_string(struct MessageHeader *msg_header, char message[], char *string);

#endif
