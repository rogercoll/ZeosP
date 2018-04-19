#include <asm.h>
#include <segment.h>
#include <errno.h>

ENTRY(task_switch)
	pushl %ebp;      
  	movl %esp,%ebp;
	pushl %esi;
	pushl %edi;
	pushl %ebx;
  	pushl 8(%ebp);   //parametre new
  	call inner_task_switch;
	addl $4,%esp;
	popl %ebx;
	popl %edi;
	popl %esi;
	popl %ebp
	ret;

ENTRY(inner_task_switch)
	pushl %ebp;
	movl %esp, %ebp;

	call current;
	movl %ebp,4(%eax); //pos del task_struct on hi ha esp_kernel

	movl 8(%ebp),%ebx; //ebx apunta a dalt de kernel_stack, 8 ja que new + ret (4+4)  apunta a @new
	movl 4(%ebx),%edi; 
  


	movl %ebx,%eax;
	addl $4096,%eax;
	addl $4, %ecx;
	movl %eax, tss+4;  //update de la tss(esp0 ha de apuntar a la base de la pila de kernel), punter al kernel stack 
 	
	movl 8(%ebx),%ecx;  //punter al dir_pages_baseAddr
	pushl %ecx; //per passar el parametre a cr3
	call set_cr3;  
	movl %edi,%esp;  
	popl %ebp;
	ret;
