#include <arpa/inet.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "config.h"
#include "hash.h"
#include "message.h"
#include "utils.h"

#define SA struct sockaddr
int PORT;

struct Peer {
    struct sockaddr_in6 *conn_addr;
    int direction;
    bool init;
};

void interrupt_cb(int code) {
    FILE *f = fopen(PEER_LIST, "r");
    long size = size_file(f);
    char string[6];
    sprintf(string, "%d", PORT);
    char *output = calloc(size, 1);
    remove_line(f, string, output);
    fclose(f);
    FILE *fp = fopen(PEER_LIST, "w");
    fprintf(fp, "%s", output);
    fclose(fp);
    exit(1);
}

void read_cb(struct bufferevent *bev, void *ctx) {
    char buff[1024];
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_input(bev);
    evbuffer_remove(input, buff, 1024);
    struct Peer *peer = ctx;
    // printf("Direction : %d", peer->direction);
    printf("%s", buff);
}

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
		    struct sockaddr *address, int socklen, void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev =
	bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    struct sockaddr_in6 *conn_addr = (struct sockaddr_in6 *)address;
    struct Peer *new_peer;
    new_peer = malloc(sizeof(struct Peer));
    new_peer->conn_addr = conn_addr;
    char ip[128];
    inet_ntop(AF_INET6, &(conn_addr->sin6_addr), ip, 128);
    // printf("%s %d\n", ip, conn_addr->sin6_port);
    bufferevent_setcb(bev, read_cb, NULL, NULL, new_peer);
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

    if (connect(sockfd, (SA *)&connaddr, sizeof(connaddr)) != 0) {
	printf("Connection with the server failed.\n");
    } else {
	printf("Connected to the server.");
	char ip[128];
	inet_ntop(AF_INET6, &(connaddr.sin6_addr), ip, 128);
	struct VersionMessage *version =
	    version_msg_new(ip, connaddr.sin6_port);
	char buff[1024];
	struct bufferevent *bev =
	    bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
	version_to_string(version, buff);
	char *string = "hello";
	char final[65];
	double_hash(string, final);
	printf("%s", final);
	bufferevent_setcb(bev, read_cb, NULL, NULL, NULL);
	bufferevent_write(bev, buff, 1025);
    }
}

int main(int argc, char *argv[]) {
    int server_port = atoi(argv[1]);
    int conn_port = atoi(argv[2]);
    int conn_port_scnd = atoi(argv[3]);
    PORT = server_port;

    // Create server
    struct event_base *base = event_base_new();
    struct evconnlistener *listener;
    struct sockaddr_in6 servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(server_port);

    listener = evconnlistener_new_bind(
	base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE, -1,
	(struct sockaddr *)&servaddr, sizeof(servaddr));
    if (!listener) {
	printf("Couldn't create listener.");
	return 1;
    } else {
	FILE *fp = fopen(PEER_LIST, "a");
	fprintf(fp, "%d\n", PORT);
	fclose(fp);
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);

    signal(SIGINT, interrupt_cb);
    connect_to(base, conn_port);
    connect_to(base, conn_port_scnd);
    event_base_dispatch(base);
    return 0;
}
