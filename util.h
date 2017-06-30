/* See LICENSE file for copyright and license details. */

#define MAX(A, B)         ((A) > (B) ? (A) : (B))
#define MIN(A, B)         ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)  ((A) <= (X) && (X) <= (B))
#define LEN(X)            (sizeof(X) / sizeof((X)[0]))

void *ecalloc(size_t nmemb, size_t size);
void *emalloc(size_t size);
void *erealloc(void *p, size_t size);
char *estrdup(char *s);
void efree(void *p);
