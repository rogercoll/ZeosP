#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  write(1,"hola",strlen("hola"));
	
  int a = fork();
  char buf[256];
  char pi[256];
  itoa(a, buf);
  if(a != 0) exit(); //és el pare ja que li retorna el pid del fill creat
  write(1, buf, strlen(buf));
  int count = 0;
  int b = fork();
  while(1) {
	 if(count%5000000==0){
			int pid = getpid();
			itoa(pid,pi);
			char pidBuffer[]="PID es: ";
			write(1,pidBuffer,strlen(pidBuffer));
			write(1,pi,strlen(pi));
			write(1,"        \n",10);
		}
		count++;

  }
}
