// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>
extern void _pgfault_upcall(void);

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	
	addr = ROUNDDOWN(addr, PGSIZE);
	pte_t tmp = uvpt[PGNUM(addr)];
	int panic_cond = (uvpd[PDX(addr)] & PTE_P) && (tmp & PTE_P) && (tmp & PTE_U) && (tmp & PTE_COW);
	panic_cond = panic_cond && (err & FEC_WR);
	panic_cond = !panic_cond;
	if (panic_cond) 
		panic("pgfault");
	

	


	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	r = sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W);
	if (r < 0) panic("pgfault");

	memmove(PFTEMP, addr, PGSIZE);
	r = sys_page_map(0, PFTEMP, 0, addr, PTE_P | PTE_U | PTE_W);
	if (r < 0) panic("pgfault");

	r = sys_page_unmap(0, PFTEMP);
	if (r < 0) panic("pgfault");

	return;

	panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	
	r = 0;
	void* pg = (void*)(pn * PGSIZE);
	pte_t pte = uvpt[pn];

	assert((pte & PTE_P) && (pte & PTE_U));
	
	if ((pte & PTE_W && !(pte & PTE_SHARE)) || pte & PTE_COW) { //If the page is writable or copy-on-write
		pte_t flag = (pte & PTE_SYSCALL & (~PTE_W)) | PTE_COW;
		r = sys_page_map(0, pg, envid, pg, flag);
		if (r < 0) return r;


		r = sys_page_map(0, pg, 0, pg, flag);
		if (r < 0) return r;
	}
	else {
		pte_t flag = pte & PTE_SYSCALL;
		r = sys_page_map(0, pg, envid, pg, flag);
		if (r < 0) return r;
	}
	return 0;

	panic("duppage not implemented");
	return 0;
	
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);
	envid_t ceid;
	ceid = sys_exofork();
	if (ceid == 0) // Remember to fix "thisenv" in the child process.
		thisenv = &envs[ENVX(sys_getenvid())];
	else if (ceid > 0) {
		size_t pn = 0, lim = UTOP / PGSIZE;
		uint32_t pde, pte;
		// skip exception stack (1PGSIZE)
		while (pn < (lim - 1)) {
			pde = uvpd[pn / NPDENTRIES];
			if (!(pde & PTE_P))  // skip 1024 pages if they do not exist
				pn += NPDENTRIES;
			else {  // else, check all of them (pte)
				size_t next = MIN((lim - 1), pn + NPDENTRIES);
				// duppage
				while (pn < next) {
					pte = uvpt[pn];
					if ((pte & PTE_P) && (pte & PTE_U)) 
						if (duppage(ceid, pn) < 0)	panic("fork");
					++pn;
				}
			}
		}
		// allocate a new page for the child's user exception stack.
		if (sys_page_alloc(ceid, (void*)(UXSTACKTOP - PGSIZE), PTE_P | PTE_U | PTE_W) < 0)
			panic("fork");
		// Set up our page fault handler appropriately.
		if (sys_env_set_pgfault_upcall(ceid, _pgfault_upcall) < 0)
			panic("fork");
		// Then mark the child as runnable and return.
		if (sys_env_set_status(ceid, ENV_RUNNABLE) < 0)
			panic("fork");
	}

	return ceid;
	panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
