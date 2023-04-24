#include "common.h"

static usize
get_file_size(const char* filepath)
{
	usize result = 0;
	FILE* f;
	if(fopen_s(&f, filepath, "r") != 0) {
		fprintf(stderr, "File %s not found\n", filepath);
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	result = ftell(f);
	fseek(f, 0, SEEK_SET);
	return(result);
}

char*
load_file(const char* filepath)
{
	FILE* f;
	if(fopen_s(&f, filepath, "rb") != 0) {
		fprintf(stderr, "File %s not found\n", filepath);
		exit(1);
	}
	usize filesize = get_file_size(filepath);
	char* result = (char*) malloc(sizeof(char) * filesize + 1);
	fread(result, 1, filesize, f);
	fclose(f);
	result[filesize] = '\0';
	return(result);
}
