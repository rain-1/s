
extern char *argv0;
extern int interactive_mode;

#define reporterr(M, ...) { \
	fprintf(stderr, "Error (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	exit(1); \
}
#define _reporterr(M, ...) { \
	fprintf(stderr, "Error (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	_exit(1); \
}
#define reportret(R, M, ...) { \
	fprintf(stderr, "%s (%s:%d) " M "\n", interactive_mode ? "Warning" : "Error", __FILE__, __LINE__, ##__VA_ARGS__); \
	if (!interactive_mode) \
		exit(1); \
	else \
		return R; \
}
#define report(M, ...) { \
	fprintf(stderr, "%s (%s:%d) " M "\n", interactive_mode ? "Warning" : "Error", __FILE__, __LINE__, ##__VA_ARGS__); \
	if (!interactive_mode) \
		exit(1); \
}
#define reportvar(V, M, ...) { \
	V = M;                 \
	return NULL;           \
}
