#include "message.h"

#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "hash.h"
#include "stdio.h"
#include "string.h"
#include <msgpack.h>

int process_msg_buffer(char *buff, int length, msg_buffer *buffer) {
  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);
  size_t off = 0;
  if (buffer->cur == 0) {
    if (msgpack_unpack_next(&msg, buff, length, &off)) {
      msgpack_object root = msg.data;
      int payload_size = root.via.map.ptr[2].val.via.u64;
      MessageHeader *header = &buffer->msg->header;
      strcpy(header->network, root.via.map.ptr[0].val.via.str.ptr);
      strcpy(header->command, root.via.map.ptr[1].val.via.str.ptr);
      header->payload_size = root.via.map.ptr[2].val.via.u64;
      buffer->size = payload_size;
      buffer->data = malloc(payload_size);
      memcpy(buffer->data, buff + off, length - off);
      buffer->cur += length - off;
    }
  } else {
    memcpy(buffer->data + buffer->cur, buff, length);
    buffer->cur += length;
  }
  if (buffer->size == buffer->cur) {
    msgpack_unpacked msg_two;
    msgpack_unpacked_init(&msg_two);
    if (msgpack_unpack_next(&msg_two, buffer->data, buffer->size, NULL)) {
      msgpack_object root = msg_two.data;
      unpack_version_msg(root, &buffer->msg->version_msg);
      return 1;
    }
  }
  return -1;
}

void *unpack_version_msg(msgpack_object root, VersionMessage *version_msg) {
  for (int i = 0; i < root.via.map.size; i++) {
    msgpack_object_kv kv = root.via.map.ptr[i];
    const char *key = kv.key.via.str.ptr;
    if (strcmp(key, "version") == 0) {
      version_msg->version = kv.val.via.u64;
    }
    if (strcmp(key, "services") == 0) {
      version_msg->services = kv.val.via.u64;
    }
    if (strcmp(key, "timestamp") == 0) {
      version_msg->timestamp = kv.val.via.u64;
    }
    if (strcmp(key, "addr_recv_port") == 0) {
      version_msg->addr_recv_port = kv.val.via.u64;
    }
    if (strcmp(key, "addr_recv") == 0) {
      strcpy(version_msg->addr_recv, kv.val.via.str.ptr);
    }
  }
  return version_msg;
}

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

  msgpack_pack_map(&pk, 5);

  msgpack_pack_str(&pk, 8);
  msgpack_pack_str_body(&pk, "version", 8);
  msgpack_pack_int(&pk, version_msg->version);

  msgpack_pack_str(&pk, 9);
  msgpack_pack_str_body(&pk, "services", 9);
  msgpack_pack_int(&pk, version_msg->services);

  msgpack_pack_str(&pk, 10);
  msgpack_pack_str_body(&pk, "timestamp", 10);
  msgpack_pack_int(&pk, version_msg->timestamp);

  msgpack_pack_str(&pk, 15);
  msgpack_pack_str_body(&pk, "addr_recv_port", 15);
  msgpack_pack_int(&pk, version_msg->addr_recv_port);

  msgpack_pack_str(&pk, 10);
  msgpack_pack_str_body(&pk, "addr_recv", 10);
  msgpack_pack_str(&pk, strlen(version_msg->addr_recv) + 1);
  msgpack_pack_str_body(&pk, version_msg->addr_recv,
                        strlen(version_msg->addr_recv) + 1);

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

  msgpack_pack_str(&pk, 13);
  msgpack_pack_str_body(&pk, "message_type", 13);
  msgpack_pack_str(&pk, 8);
  msgpack_pack_str_body(&pk, "version", 8);

  msgpack_pack_str(&pk, 5);
  msgpack_pack_str_body(&pk, "size", 5);
  msgpack_pack_int(&pk, size);

  return sbuf;
}
