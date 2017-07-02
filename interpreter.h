/* see LICENSE file for copyright and license details */

void interpret(struct AST* n);
int parse_and_execute(string_port *port, char **string_capture);
void interpreter_loop(FILE *f);
