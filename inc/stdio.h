#ifndef JOS_INC_STDIO_H
#define JOS_INC_STDIO_H

#include <inc/stdarg.h>

#ifndef NULL
#define NULL	((void *) 0)
#endif /* !NULL */

// lib/console.c
//definition for corlours
#define  COLOR_BLACK   0
#define  COLOR_RED     1
#define  COLOR_GREEN   2
#define  COLOR_YELLOW  3
#define  COLOR_BLUE    4
#define  COLOR_MAGENTA 5
#define  COLOR_CYAN    6
#define  COLOR_WHITE   7
#define  COLOR_NUM     8
void	cputchar(int c);
int	getchar(void);
int	iscons(int fd);
// set and reset the foreground color
void set_fgcolor(int color);
void reset_fgcolor();
// set and reset the background color
void set_bgcolor(int color);
void reset_bgcolor();

// lib/printfmt.c
void	printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);
void	vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list);
int	snprintf(char *str, int size, const char *fmt, ...);
int	vsnprintf(char *str, int size, const char *fmt, va_list);

// lib/printf.c
int	cprintf(const char *fmt, ...);
int	vcprintf(const char *fmt, va_list);

// lib/fprintf.c
int	printf(const char *fmt, ...);
int	fprintf(int fd, const char *fmt, ...);
int	vfprintf(int fd, const char *fmt, va_list);

// lib/readline.c
char*	readline(const char *prompt);

#endif /* !JOS_INC_STDIO_H */
