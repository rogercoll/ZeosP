#include <asm.h>
#include <segment.h>
#include <errno.h>

ENTRY(write)
	pushl %ebp;
	movl %esp,%ebp;
	pushl %ebx;
	pushl %ecx;
	pushl %edx;
	movl 8(%ebp), %ebx;
	movl 12(%ebp), %ecx;
	movl 16(%ebp), %edx;
	movl $4, %eax;
	int $0x80;
	cmpl $0,%eax;
	jge final;
	negl %eax;
  movl %eax, errno;
	movl $-1, %eax;
final:
	popl %edx;
	popl %ecx;
	popl %ebx;
	popl %ebp;
	ret;

ENTRY(get_time)
	pushl %ebp;
	movl %esp,%ebp;
	movl $10, %eax;
	int $0x80;
	cmpl $0,%eax;
	jge final2;
	negl %eax;
	movl %eax, errno;
	movl $-1, %eax;
final2:
	popl %ebp;
	ret;

ENTRY(getpid)
	pushl %ebp;
	movl %esp, %ebp;
	movl $20, %eax;
	int $0x80;
	cmpl $0,%eax;
	jge final3; 
	neg %eax;
	movl %eax,errno;
	movl $-1,%eax;
final3:   
	popl %ebp;
	ret;

ENTRY(fork)
	pushl %ebp;
	movl %esp, %ebp;
	movl $2, %eax;
	int $0x80;
	cmpl $0,%eax;
	jge final4; 
	neg %eax;
	movl %eax,errno;
	movl $-1,%eax;
final4:   
	popl %ebp;
	ret;


ENTRY(exit)
	pushl %ebp;
	movl %esp, %ebp;
	movl $1, %eax;
	int $0x80;
	cmpl $0,%eax;
	jge final5; 
	neg %eax;
	movl %eax,errno;
	movl $-1,%eax;
final5:   
	popl %ebp;
	ret;