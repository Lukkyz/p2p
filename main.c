#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 7777
#define SA struct sockaddr
   
void cb_func(evutil_socket_t fd, short what, void *arg) {
    const char *data = arg;
    printf("Got an event on socket %d:%s%s%s%s [%s]",
            (int) fd,
            (what&EV_TIMEOUT) ? "timeout":"",
            (what&EV_READ) ? "read":"",
            (what&EV_WRITE) ? "write":"",
            (what&EV_SIGNAL) ? "signal":"",
            data
            );
}

void cb_close(evutil_socket_t fd, short what, void *arg) {
    printf("WRITE");
    printf("%d\n",fd);
}

void accept_conn_cb(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *ctx) {
        struct event_base *base = evconnlistener_get_base(listener);
        struct event *ev = event_new(base, fd, EV_WRITE, cb_close, NULL);
        event_add(ev, 0);
}

int main()
{
    struct event_base *base = event_base_new();

    int serv_fd, serv_conn_fd, len;
    struct sockaddr_in servaddr, client;
  
    // Create file descriptor for server
    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
  
    // Add listener for listening to connection, call callback when connection
    struct evconnlistener *listener = evconnlistener_new_bind(base, accept_conn_cb,NULL, LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&servaddr, sizeof(servaddr));
    struct event *ev_wr;


    event_base_dispatch(base);
    for (;;) {}
}
