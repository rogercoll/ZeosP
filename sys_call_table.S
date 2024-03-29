#include <asm.h>
#include <segment.h>

ENTRY(sys_call_table)
	.long sys_ni_syscall 
	.long sys_exit
	.long sys_fork
	.long sys_ni_syscall
	.long sys_write
	.long sys_ni_syscall // 5
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall	
	.long sys_gettime // 10 
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall // 15
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_getpid // 20
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall // 25
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall // 30
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_get_stats // 35
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall

.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table)/4
