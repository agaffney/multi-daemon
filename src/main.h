struct dispatch_table {
	const char *service;
	int (*func)(int, char**);
};
