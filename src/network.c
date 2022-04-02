#include <arpa/inet.h>
#include <errno.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define SA struct sockaddr
#define MAX_OUT_CONNECTION 10

struct sockaddr_in6 create_addr(int port, char* address) {
    struct evconnlistener* listener;
    struct sockaddr_in6 addr;

    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    inet_pton(AF_INET6, address, &addr.sin6_addr);

    return addr;
}

void error_cb(struct evconnlistener* listener, void* ctx) {
    struct event_base* base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d on the listener. Shutting down", err);
    event_base_loopexit(base, NULL);
}

void read_cb(struct bufferevent* bev, void* ctx) {
    char buff[1024];
    struct evbuffer* input = bufferevent_get_input(bev);
    struct evbuffer* output = bufferevent_get_input(bev);
    evbuffer_remove(input, buff, 1024);
    printf("%s\n", buff);
}

void accept_conn_cb(struct evconnlistener* listener, evutil_socket_t fd,
                    struct sockaddr* address, int socklen, void* ctx) {
    struct event_base* base = evconnlistener_get_base(listener);
    struct bufferevent* bev =
        bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    struct sockaddr_in6* conn_addr = (struct sockaddr_in6*)address;
    char ip[128];
    inet_ntop(AF_INET6, &(conn_addr->sin6_addr), ip, 128);
    // printf("%s %d\n", ip, conn_addr->sin6_port);
    bufferevent_setcb(bev, read_cb, NULL, NULL, NULL);
    bufferevent_enable(bev, EV_READ);
    char string[1024] = "server->host\n";
    bufferevent_write(bev, string, 1024);
}

void connect_node(struct event_base* base, int port, char address[]) {
    int sockfd, connfd;
    struct sockaddr_in6 connaddr = create_addr(port, address);
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    int res_connect = connect(sockfd, (SA*)&connaddr, sizeof(connaddr));
    printf("State connection Node IP(%s) PORT(%d) : %s\n", address, port, strerror(errno));
    if (res_connect != 0) {
        printf("Connection with the server failed.\n");
    } else {
        char buff[1024] = "abcdef";
        struct bufferevent* bev =
            bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_write(bev, buff, 1025);
        bufferevent_enable(bev, EV_READ);
        bufferevent_setcb(bev, read_cb, NULL, NULL, NULL);
        /** char ip[128];
         inet_ntop(AF_INET6, &(connaddr.sin6_addr), ip, 128);
         struct VersionMessage* version =
             version_msg_new(ip, connaddr.sin6_port);
         char buff[1024];
         char string[] = "hello";
         char final[65];
         double_sha256(string, final);
                  version_to_string(version, buff);
         bufferevent_write(bev, buff, 1025); **/
    }
}

void node_init(int serv_port) {
    evthread_use_pthreads();
    struct event_base* base = event_base_new();
    struct evconnlistener* listener;
    struct sockaddr_in6 servaddr = create_addr(serv_port, "::1");
    listener = evconnlistener_new_bind(
        base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (!listener) {
        printf("Couldn't create listener.");
    } else {
        /** printf("Connected to the server.\n");
         FILE* fp = fopen("peer.conf", "a");
         fprintf(fp, "%d\n", PORT);
         fclose(fp);**/
    }
    connect_node(base, (serv_port != 3306 ? 3306 : 4000), "::1");
    evconnlistener_set_error_cb(listener, error_cb);
    event_base_dispatch(base);
}

