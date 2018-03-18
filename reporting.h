/* see LICENSE file for copyright and license details */
/* report errors or warnings */

extern char *argv0;
extern int debug;
extern int interactive_mode;

#define reportprint(E, M, ...) do {                                                   \
	if (debug)                                                                    \
		fprintf(stderr, "%s:%d: %s: " M "\n", __FILE__, __LINE__,             \
		        !E || interactive_mode ? "warning" : "error", ##__VA_ARGS__); \
	else                                                                          \
		fprintf(stderr, "%s: " M "\n", argv0, ##__VA_ARGS__);                 \
} while(0)

#define reporterr(M, ...) do {            \
	reportprint(1, M, ##__VA_ARGS__); \
	exit(1);                          \
} while(0)
#define _reporterr(M, ...) do {           \
	reportprint(1, M, ##__VA_ARGS__); \
	_exit(1);                         \
} while(0)
#define report(M, ...) do {               \
	reportprint(0, M, ##__VA_ARGS__); \
	if (!interactive_mode)            \
		exit(1);                  \
} while(0)
#define reportret(R, M, ...) do {         \
	reportprint(0, M, ##__VA_ARGS__); \
	if (!interactive_mode)            \
		exit(1);                  \
	else                              \
		return R;                 \
} while(0)
#define reportvar(V, M) do { \
	V = M;               \
	return NULL;         \
} while(0)
