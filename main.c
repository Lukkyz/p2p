#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "message.h"

#define SA struct sockaddr

void read_cb(struct bufferevent *bev, void *ctx) {
    char buff[1024];
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_input(bev);
    evbuffer_remove(input, buff, 1024);
    printf("%s", buff);
	printf("%d", ctx[0]);
}
    
void accept_conn_cb(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *ctx) {
        struct event_base *base = evconnlistener_get_base(listener);
        struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, read_cb, NULL, NULL, NULL);
        bufferevent_enable(bev, EV_READ);
}

void accept_error_cb(struct evconnlistener *listener, void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d on the listener. Shutting down", err);
    event_base_loopexit(base, NULL);
}

void connect_to(struct event_base *base, int port) {
	int sockfd, connfd;
    struct sockaddr_in6 connaddr, cli; 
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
    } else {
        printf("Socket successfully created\n");
    }
    bzero(&connaddr, sizeof(connaddr));

    connaddr.sin6_family = AF_INET6;
    connaddr.sin6_port = htons(port);
    inet_pton(AF_INET6, "::1", &connaddr.sin6_addr);

    if (connect(sockfd, (SA*)&connaddr, sizeof(connaddr)) != 0) {
        printf("Connection with the server failed.\n");
    } else {
        printf("Connected to the server.");
        struct MessageHeader *msg_header = header_msg_new(124, "abcdefgh");
        char buff[300];
        msg_to_string(msg_header, "abcdefgh", buff);
        struct bufferevent *bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_enable(bev, EV_READ);
		bufferevent_setcb(bev, read_cb, NULL, NULL, port);
        bufferevent_write(bev, buff, 300);
    }
}

int main(int argc, char *argv[])
{
    int server_port = atoi(argv[1]);
    int conn_port = atoi(argv[2]);
    int conn_port_scnd = atoi(argv[3]);

    // Create server
    struct event_base *base = event_base_new();
    struct evconnlistener *listener;
    struct sockaddr_in6 servaddr;
  
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(server_port);
  
    // Add listener for incoming connection
    listener = evconnlistener_new_bind(base, accept_conn_cb,NULL, LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (!listener) {
        printf("Couldn't create listener.");
        return 1;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);

	connect_to(base, conn_port);
	connect_to(base, conn_port_scnd);
    // Create a connection
    
    event_base_dispatch(base);
    return 0;
}
