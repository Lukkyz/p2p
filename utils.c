#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void remove_line(FILE *fp, char *string, char *output) {
    char buffer[7];  // 7 because len of each line is 5 + trailing + NUL
		     // terminator
    int i = 0;
    while (fgets(buffer, 7, fp) != NULL) {
	char temp[7];
	strcpy(temp, buffer);
	temp[strcspn(temp, "\n")] =
	    0;	// We need to remove end of line for comparing line to a string
	if (strcmp(temp, string) != 0) {
	    strcat(output, buffer);
	    i++;
	}
    }
}

long size_file(FILE *fp) {
    long size;
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}

// Parse peer file
void parse_file(FILE *fp, char output[][5], int *total_line) {
    char buffer[7];
    int i = 0;
    while (fgets(buffer, 7, fp) != NULL) {
	buffer[strcspn(buffer, "\n")] =
	    0;	// We need to remove end of line for comparing line to a string
	strcpy(output[i], buffer);
	i++;
    }
    *total_line = i;
}
