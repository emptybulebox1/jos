// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/pmap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "Backtrace the call of functions", mon_backtrace},
	{ "rainbow", "show a rainbow", test_rainbow},
	{ "showmap", "show memory mappings, Usage:\nshowmap <strat> [<length>]", mon_showmap},
	{ "setperm", "set perm bit of mem mapping, Usage:\nsetperm <VA> <perm>", mon_setperm},
};

/***** Implementations of basic kernel monitor commands *****/

int mon_showmap(int argc, char** argv, struct Trapframe* tf) {
	if ((argc < 2) || (argc > 3)) {
		cprintf("Usage:\nshowmap <strat> [<length>]\n");
		return 0;
	}
	uintptr_t vstart = (uintptr_t)strtol(argv[1], 0, 0);
	size_t vlen = (argc == 3) ? (size_t)strtol(argv[2], 0, 0) : 1;
	uintptr_t vend = vstart + vlen;
	vstart = ROUNDDOWN(vstart, PGSIZE);
	vend = ROUNDDOWN(vend, PGSIZE);

	while (vstart <= vend) {
		pte_t* pte = pgdir_walk(kern_pgdir, (void*)vstart, 0);
		if (pte && (*pte & PTE_P)) {
			cprintf("VA: 0x%08x     PA: 0x%08x    ", vstart, PTE_ADDR(*pte));
			cprintf("PTE_U: %d, PTE_W: %d\n", !!(*pte & PTE_U), !!(*pte & PTE_W));
		}
		else 
			cprintf("VA: 0x%08x     NO Mapping\n", vstart);

		vstart += PGSIZE;
	}
	return 0;
}

int mon_setperm(int argc, char** argv, struct Trapframe* tf) {
	if (argc != 3) {
		cprintf("Usage:\nsetperm <virtual address> <permission>\n");
		cprintf("PTE_P		0x001	// Present\n");
		cprintf("PTE_W		0x002	// Writeable\n");
		cprintf("PTE_U		0x004	// User\n");
		cprintf("PTE_PWT		0x008	// Write-Through\n");
		cprintf("PTE_PCD		0x010	// Cache-Disable\n");
		cprintf("PTE_A		0x020	// Accessed\n");
		cprintf("PTE_D		0x040	// Dirty\n");
		cprintf("PTE_PS		0x080	// Page Size\n");
		cprintf("PTE_G		0x100	// Global\n");
		return 0;
	}
	uintptr_t va = (uintptr_t)strtol(argv[1], 0, 0);
	uint16_t perm = (uint16_t)strtol(argv[2], 0, 0);
	va = ROUNDDOWN(va, PGSIZE);
	pte_t* pte = pgdir_walk(kern_pgdir, (void*)va, 0);

	if (pte && (*pte & PTE_P))
		*pte = ((*pte & (~0xfff)) | (perm | 0xfff) | PTE_P);
	else
		cprintf("No Mapping\n");
	
	return 0;
}

int
test_rainbow(int argc, char** argv, struct Trapframe* tf)
{
	char msg[] = "rainbow!";
	//check background color
	for (int i = 1; i < COLOR_NUM; ++i) {
		for (int j = 0; j < COLOR_NUM; ++j) {
			set_bgcolor((j + i) % COLOR_NUM);
			cprintf(" ");
		}
		reset_bgcolor();
		cprintf("\n");
	}
	//check foreground color
	for (int i = 1; i < COLOR_NUM; ++i) {
		for (int j = 0; j < COLOR_NUM; ++j) {
			set_fgcolor((i + j) % COLOR_NUM);
			cprintf("%c", msg[j % (sizeof(msg) - 1)]);
		}
		reset_fgcolor();
		cprintf("\n");
	}
	return 0;
}

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	cprintf("Stack backtrace:\n");
	//backtrace eip chain
	for (uint32_t ebp = read_ebp(); ebp; ebp = *(uint32_t*)(ebp)) {
		cprintf("  ebp %08x", ebp);
		cprintf("  eip %08x", ((uint32_t*)ebp)[1]);
		cprintf("  args");
		for (int i = 0; i < 5; ++i) {
			cprintf(" %08x", ((uint32_t*)ebp)[i + 2]);
		}
		cprintf("\n");
		//get eip info
		struct Eipdebuginfo info;
		debuginfo_eip(((uint32_t*)ebp)[1], &info);
		cprintf("         %s:%d: %.*s+%u\n", info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name,
			(((uint32_t*)ebp)[1] - info.eip_fn_addr));//offset (in bytes)
	}
	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");


	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
