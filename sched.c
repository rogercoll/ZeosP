/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");


	while(1)
	{
					printk("IDLE\n");

	}
}

void init_idle (void)
{
	struct list_head *primer =  list_first(&freequeue);		
	list_del(primer); //treure el proces de la freeque	
	struct task_struct *pcb_idl = list_head_to_task_struct(primer); //agafar el task struct de la pagina									
	pcb_idl->PID = 0;
	pcb_idl->quantum = MAX_QUANTUM;
	allocate_DIR(pcb_idl);
	union task_union *upcb_idl = (union task_union*)pcb_idl;
	upcb_idl->stack[KERNEL_STACK_SIZE-1]=(unsigned long)&cpu_idle; //adreça de retorn cpu_idle
	upcb_idl->stack[KERNEL_STACK_SIZE-2]=0; //ebp
	pcb_idl->esp_kernel = (int)&(upcb_idl->stack[KERNEL_STACK_SIZE-2]);
	
	idle_task = pcb_idl;
}

void init_task1(void)
{
	struct list_head *primer =  list_first(&freequeue);		
	list_del(primer); //treure el proces de la freeque	
	struct task_struct *pcb_init = list_head_to_task_struct(primer); //agafar el task struct de la pagina									
	pcb_init->PID = 1;
	pcb_init->estat = ST_RUN; //RUN O READY?
	pcb_init->quantum = MAX_QUANTUM;
	allocate_DIR(pcb_init);
	set_user_pages(pcb_init);
	union task_union *upcb_init = (union task_union*)pcb_init;
	tss.esp0 = (DWord)&(upcb_init->stack[KERNEL_STACK_SIZE]);
	init_task = pcb_init;
	set_cr3(pcb_init->dir_pages_baseAddr);
}


void init_sched(){
	int i = 0;
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	while(i < NR_TASKS){
		list_add_tail(&task[i].task.list, &freequeue);
		++i;
	}
	npid = 2;
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_switch();


void inner_task_switch(union task_union *t);

void task_switch(union task_union *t);



int get_quantum(struct task_struct *t){
	return t->quantum;
}
void set_quantum(struct task_struct *t,int new_quantum){
	t->quantum = new_quantum;
}



void update_sched_data_rr() {
	--cpu_remaining_ticks;
}

int needs_sched_rr() {
	if (cpu_remaining_ticks <= 0 && !list_empty(&readyqueue)) return 1;
	return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue){
	if (t->estat != ST_RUN) {
		list_del(&t->list);
	}
	if (dst_queue == NULL) t->estat = ST_RUN;
	else {
		if (dst_queue == &(readyqueue)){
		 t->estat = ST_READY;
		 current()->estadistics.system_ticks += get_ticks() - current()->estadistics.elapsed_total_ticks;
		 current()->estadistics.elapsed_total_ticks = get_ticks();
		}
		if (t != idle_task) list_add_tail(&t->list, dst_queue);
	}
}


void sched_next_rr() {
	struct task_struct *proces_to_execute;	
	if (list_empty(&readyqueue)) proces_to_execute = idle_task;
	else {
		proces_to_execute = list_head_to_task_struct(list_first(&readyqueue));
		list_del(list_first(&readyqueue));
	}
	proces_to_execute->estat = ST_RUN;
	task_switch( (union task_union*)proces_to_execute );
}

void schedule() {
	update_sched_data_rr();
	if (needs_sched_rr() == 1) {
		if (list_empty(&readyqueue) && (current()->estat == ST_RUN)) {
			cpu_remaining_ticks = current()->quantum;
			update_process_state_rr(current(), NULL);
		}
		else {
			update_process_state_rr(current(), &readyqueue);
			sched_next_rr();
		}
	}
}


