typedef struct {
	size_t len;
	size_t alloc_len;
	void **pointers;
} region;

void region_create(region *r);
void* region_malloc(region *r, size_t size);
void* region_realloc(region *r, void *v, size_t size);
void region_free(region *r);

