#include <msgpack.h>
#include <stdint.h>
#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct {
  char network[4];
  char command[12];
  uint32_t payload_size;
} MessageHeader;

typedef struct  {
  int32_t version;
  uint64_t services;
  int64_t timestamp;
  int addr_recv_port;
  char addr_recv[128];
} VersionMessage;

msgpack_sbuffer *pack_header(int size);
msgpack_sbuffer *pack_version_msg();
VersionMessage *version_msg_new(char addr_recv[128], uint16_t port);
MessageHeader *header_msg_new(uint32_t payload_size, char *data);
#endif
