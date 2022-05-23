#ifndef NETWORK_H
#define NETWORK_H
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <sys/socket.h>
#include "message.h"
#include <time.h>

#define SA struct sockaddr
#define MAX_OUT_CONNECTION 10
#define MAX_SIZE 268435456

enum conn_dir { IN, OUT };

typedef struct {
  char *msg_type;
  char *data;
  int cur;
  int size;
} msg_buffer;

typedef struct {
  struct sockaddr_in6 addr;
  int64_t timestamp;
  enum conn_dir dir;
  struct bufferevent *bev;
  msg_buffer buffer;
} peer;

struct sockaddr_in6 create_addr(int port, char *address);
void error_cb(struct evconnlistener *listener, void *ctx);
void read_cb(struct bufferevent *bev, void *ctx);
void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *address, int socklen, void *ctx);
int connect_node(struct event_base *base, int port, char address[]);
void node_init(int serv_port, int conn_port);
void *event_thread(void *arg);
#endif
