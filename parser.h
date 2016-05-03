#define MAX_TOKS_PER_LINE 4096

typedef enum {
	NODE_COMMAND,
	NODE_PIPE
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

char** read_tokens(FILE *f);
struct AST* parse_tokens(char **tokens);

struct AST* parse(FILE *f);
