# JOS Operating System

## Project Introduction

JOS is an educational operating system based on the MIT 6.828 Operating System Engineering course. This project gradually constructs a functioning operating system through a series of labs, helping to understand core OS concepts and implementation details.

## My Contributions

I implemented approximately 2500 lines of critical code throughout the project, filling in all the required functionality across the labs. Key accomplishments include:

- **Completed all core requirements** across all five labs
- **Implemented numerous challenge features** beyond basic requirements for course credit
- **Gained comprehensive understanding** of the entire operating system stack from boot process to high-level abstractions
- **Successfully built a working OS** with memory management, process scheduling, system calls, file system support and etc.

## Features

This operating system implements the following core functionalities:

- Boot loader and kernel initialization
- Memory management and virtual memory
- User environments (processes) and system calls
- Preemptive multitasking
- Basic file system and shell

## Lab Contents

### Lab 1: Booting

- Setting up the development environment
- Implementing a boot loader
- Implementing kernel initialization
- Implementing basic debugging features
- Understanding the PC's physical address space

### Lab 2: Memory Management

- Physical page management
- Virtual memory and page table mapping
- Kernel address space
- User address space design
- Memory allocator implementation

### Lab 3: User Environment

- User environment (process) data structures
- Environment creation and context switching
- Interrupt and exception handling
- System call implementation
- User-level program support

### Lab 4: Preemptive Multitasking

- Multiprocessor support
- Multi-threaded kernel design
- Preemptive scheduling implementation
- Inter-Process Communication (IPC)
- Clock interrupts and timers

### Lab 5: File System and Shell

- Disk driver implementation
- File system implementation
- File descriptors and pipes
- Spawn system call (for creating new processes)
- Shell implementation

## How to Run

To run this operating system, ensure you have the QEMU emulator installed, then execute:

```bash
make
make qemu
```

## Project Source

This project is based on the JOS operating system lab framework from MIT's 6.828 course. For more details, visit:
[MIT 6.828: Operating System Engineering](https://pdos.csail.mit.edu/6.828/2018/overview.html)
