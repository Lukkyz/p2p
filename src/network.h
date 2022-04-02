#ifndef NETWORK_H
#define NETWORK_H

struct sockaddr_in6 create_addr(int port, char* address);
void error_cb(struct evconnlistener* listener, void* ctx);
void read_cb(struct bufferevent* bev, void* ctx);
void accept_conn_cb(struct evconnlistener* listener, evutil_socket_t fd,
                    struct sockaddr* address, int socklen, void* ctx);
void connect_node(struct event_base* base, int port, char address[]);
void node_init(int serv_port);
#endif
