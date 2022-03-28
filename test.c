#include <netdb.h>
#include <stdbool.h>
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
#include "hash.h"
#include "utils.h"

int main(int argc, char *argv[])
{
	FILE *f;
	f = fopen("file", "a+");
	char *output = NULL;
	char line[] = "hello";
	remove_line(f, line, output);
	fclose(f);
}
