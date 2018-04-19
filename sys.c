/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>
#include <errno.h>
#include <sched.h>
#include <system.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork(){
  return 0;
}

int sys_fork()
{  // creates the child process
	  int PID=-1;
	//MIREM SI HI HA ALGUN PCB LLIURE
	if(list_empty(&freequeue)) return -ENSFP;

	//AGAFEM UN PCB I CREEM ELS UNIONS CORRESPONENTS
	struct list_head *son =  list_first(&freequeue);		
	list_del(son); //treure el proces de la freeque	
	struct task_struct *pcb_s = list_head_to_task_struct(son); //agafar el task struct de la pagina
	union task_union *uson = (union task_union*)pcb_s;
	union task_union *ufather = (union task_union *)current();

	//COPIEM EL UNION DEL PARE AL FILL
	copy_data(ufather,uson,sizeof(union task_union));

	//INICIALITZAR dir_pages_baseAddr
	allocate_DIR(&(uson->task));

	//SEARCH PHYSICAL PAGES TO MAP LOGICAL PAGES FOR DATA+STACK
	int pages[NUM_PAG_DATA];
	int i;
	for (i=0;i<NUM_PAG_DATA;i++){
		pages[i] = alloc_frame();
		if(pages[i] == -1){
			int j;
			for (j = 0; j <= i; j++) {
				free_frame(pages[j]);
			}
			return -ENEFP;
		}
	}

	//INHERIT USER DATA
	page_table_entry *son_PT = get_PT(pcb_s);
	page_table_entry *father_PT = get_PT(current());
	for(i=0;i<NUM_PAG_CODE+NUM_PAG_KERNEL;i++){ //KERNEL EN COMPTES DE DATA???
			set_ss_pag(son_PT,i,get_frame(father_PT,i)); //inicialitzar la taula de pagines del fill
		}
		


	int father_free_entry = PAG_LOG_INIT_DATA + NUM_PAG_DATA;

	for(i = 0; i < NUM_PAG_DATA; ++i){
	  set_ss_pag(son_PT, i+PAG_LOG_INIT_DATA, pages[i]);
	  set_ss_pag(father_PT, father_free_entry, pages[i]);
	  copy_data( (void *)((PAG_LOG_INIT_DATA+i)<<12), (void *)(father_free_entry<<12), PAGE_SIZE);
      	  del_ss_pag(father_PT, father_free_entry);
	  ++father_free_entry;
	}

	set_cr3(current()->dir_pages_baseAddr);
	
	//ASSIGN NEW PID
	PID = ++npid;
	pcb_s->PID = PID;
	pcb_s->estat = ST_READY;
	pcb_s->quantum = MAX_QUANTUM;
	pcb_s->esp_kernel = (int)&(uson->stack[KERNEL_STACK_SIZE-19]);
	
	uson->stack[KERNEL_STACK_SIZE-19] = 0;
	uson->stack[KERNEL_STACK_SIZE-18] = (unsigned long)&ret_from_fork;
	list_add(&(uson->task.list), &readyqueue);
	return uson->task.PID;
	
}


int sys_write(int fd, char * buffer, int size) {
  int x = check_fd(fd,1);
	if (x < 0) return x;
	if (buffer == NULL) return -EBADPOINTER;
	if (size < 0) return -ENEGSIZE;
	char desp[1024];
  int auxsize = size;
  while(auxsize >= 1024){
    copy_from_user (buffer, desp, 256);
    sys_write_console(desp,1024);
    buffer += 1024;
    auxsize -= 1024;
  }
  char desp2[size%1024];
  copy_from_user (buffer, desp2, size%1024);
	sys_write_console(desp2, size);
	return 0;
}

int sys_gettime() {
	return zeos_ticks;
}

void sys_exit()
{
	struct task_struct *pcb_process = current();
	list_add(&(pcb_process->list),&freequeue);	
	free_user_pages(pcb_process);
	sched_next_rr();
}

int sys_get_stats(int pid, struct stats *st){
	return 0;
}
