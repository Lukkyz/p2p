#ifndef FILE_H
#define FILE_H
void remove_line(FILE *fp, char *string, char *output);
long size_file(FILE *fp);
void parse_file(FILE *fp, char output[][5], int *total_line);
#endif

