typedef struct {
	char name[50];
	char value[255];
} config_opt;

int parse_error(char *, char *, int);

int parse_config_file(char *, char *, config_opt [], int);
