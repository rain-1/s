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

extern char *operator_for[];

void free_ast(struct AST *);

struct AST* parse_tokens(char **tokens, int *bg_flag, char **redir_in_flag, char **redir_out_flag);

struct AST* parse(FILE *f, int *bg_flag, char **redir_in_flag, char **redir_out_flag);
