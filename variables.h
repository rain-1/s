/* see LICENSE file for copyright and license details */

extern char variable_name[TOK_MAX];

void vars_set(char **argv);
void vars_unset(void);
char *expand_variables(region *r, char *tok, int t);
