#define MAX_TOKS_PER_LINE 4096

typedef enum {
	NODE_COMMAND = 0,
	NODE_PIPE = 1,
	NODE_CONJ = 2,
	NODE_DISJ = 3,
} NodeType;

struct AST {
	NodeType type;
	union {
		char **tokens;
		struct {
			struct AST *l;
			struct AST *r;
		} child;
	} node;
};

void free_ast(struct AST *);

char *operator_for(NodeType ty);

char** read_tokens(FILE *f, char **envp);
struct AST* parse_tokens(char **tokens);

struct AST* parse(FILE *f, char **envp);
