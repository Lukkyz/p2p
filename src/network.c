#include "network.h"
#include "message.h"

#include <arpa/inet.h>
#include <errno.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <msgpack.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

peer *peers[100];
int peer_num = 0;

void add_peer(peer *new_peer) { peers[peer_num++] = new_peer; };

struct sockaddr_in6 create_addr(int port, char *address) {
  struct sockaddr_in6 addr;
  addr.sin6_family = AF_INET6;
  addr.sin6_port = htons(port);
  inet_pton(AF_INET6, address, &addr.sin6_addr);
  return addr;
}

void error_cb(struct evconnlistener *listener, void *ctx) {
  struct event_base *base = evconnlistener_get_base(listener);
  int err = EVUTIL_SOCKET_ERROR();
  fprintf(stderr, "Got an error %d on the listener. Shutting down", err);
  event_base_loopexit(base, NULL);
}

void read_cb(struct bufferevent *bev, void *ctx) {
  struct evbuffer *input = bufferevent_get_input(bev);
  struct evbuffer *output = bufferevent_get_input(bev);
  int length = evbuffer_get_length(input);
  char buff[length];
  peer *peer = ctx;
  evbuffer_remove(input, buff, length);
  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);
  size_t off = 0;
  printf("%d\n", peer->buffer.cur);
  if (peer->buffer.cur == 0) {
    if (msgpack_unpack_next(&msg, buff, length, &off)) {
      msgpack_object root = msg.data;
      int l = root.via.map.ptr[1].val.via.u64;
      const char *msg_type = root.via.map.ptr[2].val.via.str.ptr;
      peer->buffer.msg_type = malloc(sizeof(msg_type));
      strcpy(peer->buffer.msg_type, msg_type);
      printf("%s\n", peer->buffer.msg_type);
      printf("L: %d Off: %lu SIZE: %d CUR: %d\n",length, off, l, peer->buffer.cur);
      peer->buffer.size = l;
      peer->buffer.data = malloc(sizeof(l));
      int new_s = length - off;
      printf("%lu %d\n", off, new_s);
      char *test = malloc(sizeof(l));
      memcpy(peer->buffer.data, buff+off, l);
      peer->buffer.cur += l;
    }
  } else {
    memcpy(peer->buffer.data+peer->buffer.cur, buff, length);
  }
  if (peer->buffer.size == peer->buffer.cur) {
    printf("%d %d\n", peer->buffer.size, peer->buffer.cur);
    msgpack_unpacked msg_two;
    msgpack_unpacked_init(&msg_two);
    if (msgpack_unpack_next(&msg_two, peer->buffer.data, peer->buffer.size, NULL)) {
      printf("xccece\n");
      msgpack_object root = msg_two.data;
      printf("%ld\n", root.via.map.ptr[2].val.via.u64);
    }
  }

}

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *address, int socklen, void *ctx) {
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *bev =
      bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  struct sockaddr_in6 *conn_addr = (struct sockaddr_in6 *)address;
  char ip[128];
  inet_ntop(AF_INET6, &(conn_addr->sin6_addr), ip, 128);
  peer *new_peer = malloc(sizeof(peer));
  new_peer->addr = *conn_addr;
  new_peer->timestamp = (int)time(NULL);
  new_peer->dir = IN;
  new_peer->bev = bev;
  new_peer->buffer.cur = 0;
  new_peer->buffer.size = 0;
  add_peer(new_peer);
  // printf("%s %d\n", ip, conn_addr->sin6_port);
  bufferevent_setcb(bev, read_cb, NULL, NULL, new_peer);
  bufferevent_enable(bev, EV_READ);
  // bufferevent_write(bev, string, 1024);
}

int connect_node(struct event_base *base, int port, char address[]) {
  int sockfd, connfd;
  struct sockaddr_in6 connaddr = create_addr(port, address);
  sockfd = socket(AF_INET6, SOCK_STREAM, 0);
  int res_connect = connect(sockfd, (SA *)&connaddr, sizeof(connaddr));
  printf("State connection Node IP(%s) PORT(%d) : %s\n", address, port,
         strerror(errno));
  if (res_connect != 0) {
    printf("Connection with the server failed.\n");
    return -1;
  }

  struct bufferevent *bev =
      bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
  char ip[128];
  inet_ntop(AF_INET6, &(connaddr.sin6_addr), ip, 128);
  VersionMessage *version_msg = version_msg_new(ip, port);
  msgpack_sbuffer *buff = pack_version_msg(version_msg);
  printf("%zu\n", buff->size);
  if (bufferevent_write(bev, buff->data, buff->size) == 0) {
    peer new_peer = {
        .addr = connaddr, .dir = OUT, .timestamp = (int)time(NULL), .bev = bev};

    add_peer(&new_peer);
    bufferevent_enable(bev, EV_READ);
    bufferevent_setcb(bev, read_cb, NULL, NULL, &new_peer);
    return 0;
  };
  return -1;
  // Build msgpack
}

void *event_thread(void *arg) {
  struct event_base *base = arg;
  event_base_dispatch(base);
  return 0;
}

void *get_in(void *arg) {
  while (1) {
    sleep(2);
  }
}

void node_init(int serv_port, int conn_port) {
  struct event_base *base = event_base_new();
  struct evconnlistener *listener;
  struct sockaddr_in6 servaddr = create_addr(serv_port, "::1");
  listener = evconnlistener_new_bind(
      base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE, -1,
      (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (!listener) {
    printf("Couldn't create listener.");
  } else {
    /** printf("Connected to the server.\n");
     FILE* fp = fopen("peer.conf", "a");
     fprintf(fp, "%d\n", PORT);
     fclose(fp);**/
  }

  connect_node(base, conn_port, "::1");
  evconnlistener_set_error_cb(listener, error_cb);

  pthread_t id[5];
  int ret_thread[5];
  //
  pthread_create(&id[0], NULL, event_thread, base);
  pthread_create(&id[1], NULL, get_in, NULL);

  pthread_join(id[0], (void **)&ret_thread[0]);
  pthread_join(id[1], (void **)&ret_thread[0]);
}
