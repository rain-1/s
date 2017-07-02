/* see LICENSE file for copyright and license details */

typedef enum {
	NODE_COMMAND,
	NODE_PIPE,
	NODE_CONJ,
	NODE_DISJ,
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

