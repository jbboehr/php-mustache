
#ifndef JSONLINT_H
#define JSONLINT_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <locale.h>
#include <getopt.h>
#include <errno.h>

#include "json.h"


  
struct json_val_elem {
	char *key;
	uint32_t key_length;
	struct json_val *val;
};

typedef struct json_val {
	int type;
	int length;
	union {
		char *data;
		struct json_val **array;
		struct json_val_elem **object;
	} u;
} json_val_t;



static int printchannel(void *userdata, const char *data, uint32_t length);

static int prettyprint(void *userdata, int type, const char *data, uint32_t length);

FILE *open_filename(const char *filename, const char *opt, int is_input);

void close_filename(const char *filename, FILE *file);

int process_file(json_parser *parser, FILE *input, int *retlines, int *retcols);

static int do_verify(json_config *config, const char *filename);

static int do_parse(json_config *config, const char *filename);

static int do_format(json_config *config, const char *filename, const char *outputfile);

static void *tree_create_structure(int nesting, int is_object);

static char *memalloc_copy_length(const char *src, uint32_t n);

static void *tree_create_data(int type, const char *data, uint32_t length);

static int tree_append(void *structure, char *key, uint32_t key_length, void *obj);

static int do_tree(json_config *config, const char *filename, json_val_t **root_structure);

static int print_tree_iter(json_val_t *element, FILE *output);

static int print_tree(json_val_t *root_structure, char *outputfile);

int usage(const char *argv0);

int jsonlint_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif 