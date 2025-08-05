#include <inc/fs.h>
#include <inc/lib.h>

#define SECTSIZE	512			// bytes per disk sector
#define BLKSECTS	(BLKSIZE / SECTSIZE)	// sectors per block

/* Disk block n, when in memory, is mapped into the file system
 * server's address space at DISKMAP + (n*BLKSIZE). */
#define DISKMAP		0x10000000

/* Maximum disk size we can handle (3GB) */
#define DISKSIZE	0xC0000000

struct Super *super;		// superblock
uint32_t *bitmap;		// bitmap blocks mapped in memory

/* ide.c */
bool	ide_probe_disk1(void);
void	ide_set_disk(int diskno);
void	ide_set_partition(uint32_t first_sect, uint32_t nsect);
int	ide_read(uint32_t secno, void *dst, size_t nsecs);
int	ide_write(uint32_t secno, const void *src, size_t nsecs);

/* bc.c */
void*	diskaddr(uint32_t blockno);
bool	va_is_mapped(void *va);
bool	va_is_dirty(void *va);
void	flush_block(void *addr);
void	bc_init(void);

/* fs.c */
void	fs_init(void);
int	file_get_block(struct File *f, uint32_t file_blockno, char **pblk);
int	file_create(const char *path, struct File **f);
int	file_open(const char *path, struct File **f);
ssize_t	file_read(struct File *f, void *buf, size_t count, off_t offset);
int	file_write(struct File *f, const void *buf, size_t count, off_t offset);
int	file_set_size(struct File *f, off_t newsize);
void	file_flush(struct File *f);
int	file_remove(const char *path);
void	fs_sync(void);

/* int	map_block(uint32_t); */
bool	block_is_free(uint32_t blockno);
int	alloc_block(void);

/* test.c */
void	fs_test(void);


/* bc.c cahllenge */
#define NCACHE 1024
#define NVISIT (NCACHE>>2)

typedef struct buffer_cache {
	struct buffer_cache* bc_free;
	EmbedLink bc_used;
	void* bc_addr;
}buffer_cache;

static buffer_cache bcaches[NCACHE];
static buffer_cache* bc_free;
static EmbedLink Bc_used;

static int ncache = 0;
static int nvisit = 0;

/* Initialize */
static void bufc_init(void);
/* alloc a buffer_cache when a page fault needs a disk block */
static int bufc_alloc(void* addr);
/* increase counter, if nvisit == NVISIT, clear all access bit */
int bufc_visit(void);
/* evict a selected buffer */
static int bufc_evict(void);
static int is_reserved(void* addr);