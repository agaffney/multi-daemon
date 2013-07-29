typedef struct {
	char name[50];
	char value[255];
} config_opt;

int config_parse_error(char *, char *, int);
int config_parse_file(char *, char *, config_opt [], int);
char * config_get_option(char *, config_opt []);
