#include "message.h"

#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "hash.h"
#include "stdio.h"
#include "string.h"
#include <msgpack.h>

VersionMessage *version_msg_new(char addr_recv[128], uint16_t port) {
  VersionMessage *version_msg = malloc(sizeof(*version_msg));
  version_msg->version = 1;
  version_msg->services = 1;
  version_msg->addr_recv_port = port;
  version_msg->timestamp = (int)time(NULL);
  strcpy(version_msg->addr_recv, addr_recv);
  return version_msg;
}

msgpack_sbuffer *pack_version_msg(VersionMessage *version_msg) {
  msgpack_sbuffer *sbuf = malloc(sizeof(msgpack_sbuffer));
  msgpack_sbuffer_init(sbuf);

  msgpack_packer pk;
  msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);

  msgpack_pack_map(&pk, 2);
  msgpack_pack_str(&pk, 8);
  msgpack_pack_str_body(&pk, "version", 8);
  msgpack_pack_int(&pk, 1);

  msgpack_pack_str(&pk, 6);
  msgpack_pack_str_body(&pk, "check", 6);
  msgpack_pack_int(&pk, 1);

  msgpack_sbuffer *header_buff = pack_header(sbuf->size);
  printf("%zu\n", header_buff->size);
  msgpack_sbuffer_write(header_buff, sbuf->data, sbuf->size);
  return header_buff;
}

MessageHeader *header_msg_new(uint32_t payload_size, char *data) {
  MessageHeader *msg_header = malloc(sizeof(*msg_header));
  strcpy(msg_header->network, "1");
  strcpy(msg_header->command, "version");
  msg_header->payload_size = payload_size;
  // char sha_buff[65];
//~  double_sha256((unsigned char *)data, sha_buff);
  return msg_header;
}

msgpack_sbuffer *pack_header(int size) {
  msgpack_sbuffer *sbuf = malloc(sizeof(msgpack_sbuffer));
  msgpack_sbuffer_init(sbuf);

  msgpack_packer pk;
  msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);

  msgpack_pack_map(&pk, 3);

  msgpack_pack_str(&pk, 8);
  msgpack_pack_str_body(&pk, "network", 8);
  msgpack_pack_str(&pk, 5);
  msgpack_pack_str_body(&pk, "main", 5);

  msgpack_pack_str(&pk, 5);
  msgpack_pack_str_body(&pk, "size", 5);
  msgpack_pack_int(&pk, size);

  msgpack_pack_str(&pk, 13);
  msgpack_pack_str_body(&pk, "message_type", 13);
  msgpack_pack_str(&pk, 8);
  msgpack_pack_str_body(&pk, "version", 8);

    return sbuf;
}

