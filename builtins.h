typedef struct {
	char *name;
	void (*func)(char **);
} Builtin;

int perform_builtin(struct AST *n);

void builtin_cd(char **argv);
void builtin_set(char **argv);
void builtin_unset(char **argv);
void builtin_source(char **argv);
void builtin_exit(char **argv);
