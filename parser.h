/* see LICENSE file for copyright and license details */

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

struct AST* parse_tokens(region *r, char **tokens, int *bg_flag);

struct AST* parse(region *r, string_port *port, int *bg_flag);

