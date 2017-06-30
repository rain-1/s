
extern char *argv0;
extern int debug;
extern int interactive_mode;

#define reportprint(E, M, ...) {                                                      \
	if (debug)                                                                    \
		fprintf(stderr, "%s:%d: %s: " M "\n", __FILE__, __LINE__,             \
		        !E || interactive_mode ? "warning" : "error", ##__VA_ARGS__); \
	else                                                                          \
		fprintf(stderr, "%s: " M "\n", argv0, ##__VA_ARGS__);                 \
}

#define reporterr(M, ...) {               \
	reportprint(1, M, ##__VA_ARGS__); \
	exit(1);                          \
}
#define _reporterr(M, ...) {              \
	reportprint(1, M, ##__VA_ARGS__); \
	_exit(1);                         \
}
#define report(M, ...) {                  \
	reportprint(0, M, ##__VA_ARGS__); \
	if (!interactive_mode)            \
		exit(1);                  \
}
#define reportret(R, M, ...) {            \
	reportprint(0, M, ##__VA_ARGS__); \
	if (!interactive_mode)            \
		exit(1);                  \
	else                              \
		return R;                 \
}
#define reportvar(V, M) { \
	V = M;            \
	return NULL;      \
}
