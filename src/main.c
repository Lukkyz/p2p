#include <arpa/inet.h>
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

#include "../subprojects/inih/ini.h"
#include "file.h"
#include "hash.h"
#include "message.h"
#include "network.h"
#include "utils.h"

#define SA struct sockaddr

typedef struct {
  const char *network;
} configuration;

void interrupt_cb(int code) {
  FILE *f = fopen("peer.conf", "r");
  long size = size_file(f);
  char string[6];
  char *output = calloc(size, 1);
  remove_line(f, string, output);
  fclose(f);
  FILE *fp = fopen("peer.conf", "w");
  fprintf(fp, "%s", output);
  free(output);
  fclose(fp);
  exit(1);
}

static int handler(void *user, const char *section, const char *name,
                   const char *value) {
  configuration *pconfig = (configuration *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("protocol", "network")) {
    pconfig->network = strdup(value);
  } else {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  int serv_port = atoi(argv[1]);
  int conn_port = atoi(argv[2]);

  signal(SIGINT, interrupt_cb);

  configuration config;
  if (ini_parse("../../config.ini", handler, &config) < 0) {
    printf("Can't load config.ini'");
    return 1;
  }
  node_init(serv_port, conn_port, config.network);
  return 0;
}
