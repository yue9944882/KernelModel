
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
//#include "stroperation.h"

#define NULL (void*)0


char *strstr(const char *s1, const char *s2)   
{  
    int n;  
    if (*s2)   
    {   
        while (*s1)   
        {   
            for (n = 0; *(s1 + n) == *(s2 + n); n ++)  
            {   
                if (!*(s2 + n + 1))   
                    return (char *)s1;   
            }   
            s1++;   
        }  
        return NULL;   
    }   
    else   
        return (char *)s1;   
}  

char *strtok(char *str, char *ctrl)
{
     char *p=str,*q=ctrl,*r;
     while(p&&q&&*p&&*q){
         for(r=p,q=ctrl;*r&&*q&&*r==*q;r++,q++);
         if(q&&*q) p++; 
         else {*p='\0'; break;} 
     }
     return str;
}

PRIVATE void printline();
PRIVATE void printhello();
PRIVATE void printcat();


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	struct task* p_task;
	struct proc* p_proc= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
        u8    privilege;
        u8    rpl;
	int   eflags;
	int   i, j;
	int   prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio      = 15;
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
			prio      = 5;
                }

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		/* p_proc->nr_tty		= 0; */

		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p_proc->filp[j] = 0;

		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

        /* proc_table[NR_TASKS + 0].nr_tty = 0; */
        /* proc_table[NR_TASKS + 1].nr_tty = 1; */
        /* proc_table[NR_TASKS + 2].nr_tty = 1; */

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
        init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	/*int fd;
	int i, n;

	char filename[MAX_FILENAME_LEN+1] = "blah";
	const char bufw[] = "abcde";
	const int rd_bytes = 3;
	char bufr[rd_bytes];

	assert(rd_bytes <= strlen(bufw));

	fd = open(filename, O_CREAT | O_RDWR);
	assert(fd != -1);
	printl("File created: %s (fd %d)\n", filename, fd);

	n = write(fd, bufw, strlen(bufw));
	assert(n == strlen(bufw));

	close(fd);

	fd = open(filename, O_RDWR);
	assert(fd != -1);
	printl("File opened. fd: %d\n", fd);

	n = read(fd, bufr, rd_bytes);
	assert(n == rd_bytes);
	bufr[n] = 0;
	printl("%d bytes read: %s\n", n, bufr);

	close(fd);

	char * filenames[] = {"/foo", "/bar", "/baz"};

	for (i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++) {
		fd = open(filenames[i], O_CREAT | O_RDWR);
		assert(fd != -1);
		printl("File created: %s (fd %d)\n", filenames[i], fd);
		close(fd);
	}

	char * rfilenames[] = {"/bar", "/foo", "/baz", "/dev_tty0"};

	for (i = 0; i < sizeof(rfilenames) / sizeof(rfilenames[0]); i++) {
		if (unlink(rfilenames[i]) == 0)
			printl("File removed: %s\n", rfilenames[i]);
		else
			printl("Failed to remove file: %s\n", rfilenames[i]);
	}*/

	spin("TestA");
}

/*======================================================================*
                               TestB
 *======================================================================*/
void str_merge_nbsps(char* str)
{
	char* pBuf = str;
	char* pCpy = NULL;

	for (; (pBuf=strstr(pBuf, "  ")); pBuf++){
        for(pCpy=pBuf; *pCpy; pCpy++){
            *pCpy = *(pCpy+1);
        }
        pBuf--;
    }

}

void get_parameter(char* str , int pos , char paras[128])
{
	int cnt = 0; //counter for current parameters

	//copy string for caculation
	char tmp[128];
	strcpy(tmp,str);
	char* para;
	//the break space
	char* nbsp = " ";
	
	if(pos == 0)
	{
		char ch = str[0];
		int index = 0;
		for(; (ch != '\0') && ch != ' '  ;)
		{
			paras[index] = ch;
			ch = str[++index];
		}
		paras[index] = '\0';
		return;
	}
	else 
	{
		char ch = str[0];
		int index = 0;

		for( cnt = 0 ; cnt <= pos && ch != '\0' ; ++cnt )
		{
			int subtitle;
			for(subtitle = 0 ; (ch != '\0') && ch != ' '  ; ++subtitle)
			{
				
				paras[subtitle] = ch;
				ch = str[++index];

			}
			paras[subtitle] = '\0';
			ch = str[++index];
			
		}

		if(cnt < pos)
			paras[0] = '\0';
		return ;

	}

	
}

void TestB()
{
	char tty_name[] = "/dev_tty0";

	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);
	int i;
	char rdbuf[128];
	for(i=0;i<20;i++)printline();
	printhello();
	while (1) {
		printf(" #:");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
		str_merge_nbsps(rdbuf);
		char cmd[128];
		get_parameter(rdbuf , 0 , cmd);
		if (strcmp(cmd, "hello") == 0) 
		{
			printf("hello world!\n");

		}
		else if(strcmp(cmd,"we")==0){
			printcat();
		}
		else if (strcmp(cmd , "touch") == 0) 
		{
			int fd;

			char file_name[70];
			get_parameter(rdbuf , 1 , file_name);
			
			fd = open(file_name, O_CREAT | O_RDWR);
			assert(fd != -1);
			printl("File created: %s (fd %d)\n", file_name, fd);

			/* close */
			close(fd);


		}
		else if(strcmp(cmd,"rm") == 0) 
		{
			int fd;

			char file_name[70];
			get_parameter(rdbuf , 1 , file_name);

			if (unlink(file_name) == 0)
			{
				printl("File removed: %s\n", file_name);
			}
			else
			{
				printl("Failed to remove file: %s\n", file_name);
			}


		}
		else if(strcmp(cmd,"cat") == 0) 
		{
			int fd;
			const int rd_bytes = 20;
			char bufr[rd_bytes];
			int n;

			char file_name[70];
			get_parameter(rdbuf , 1 , file_name);

			fd = open(file_name, O_RDWR);
			assert(fd != -1);
			printl("File opened. fd: %d\n", fd);
		
			n = read(fd, bufr, rd_bytes);
			assert(n == rd_bytes);
			bufr[n] = 0;
			printl("%d bytes read: %s\n", n, bufr);
		
			close(fd);

		}
		else
		{
			char redir_mark[70];
			get_parameter(rdbuf , 1 , redir_mark);

			if(strcmp(redir_mark,">") == 0) 
			{
				int fd;
				int n;

				char file_name[70];
				get_parameter(rdbuf , 1 , file_name);
	
				fd = open(file_name, O_CREAT | O_RDWR);
				assert(fd != -1);

				n = write(fd, cmd, strlen(cmd));
				assert(n == strlen(cmd));

	
				close(fd);

			}
			else 
			{	
				printf("{%s}\n", rdbuf);

			}
		}
		
		printline();

	}

	assert(0); /* never arrive here */
}

/*void TestB()
{
	char tty_name[] = "/dev_tty0";

	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];
	
	int i;
	for(i=0;i<20;i++)printline();
	printhello();
	while (1) {
		printline();
		printf("$ ");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
		
		if (strcmp(rdbuf, "hello") == 0)
			printf("hello world!\n");
		else
			if (rdbuf[0])
				printf("{%s}\n", rdbuf);
	}

	assert(0); /* never arrive here */
//}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestC()
{
	spin("TestC");
	/* assert(0); */
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

PRIVATE void printline(){
	char msg[81]={};
	int i;	
	for(i=0;i<80;i++)msg[i]='*';
	printf(msg);
}

PRIVATE void printhello(){
char msg1[81]={"                                                                                "};
char msg2[81]={"      ******    ****   ****     ******     ****   ****   ****        ******     "};
char msg3[81]={"     **    ***   **     **     **    ***    **     **     **         ******     "};
char msg4[81]={"     **          **     **     **           **     **     **           **       "};
char msg5[81]={"     **          **     **     **           **     **     **           **       "};
char msg6[81]={"     **   ****   **     **     **   ****    **     **     **           **       "};
char msg7[81]={"     **    **     **   **      **    **      **   **      ********   ******     "};
char msg8[81]={"      ******       *****         *****        *****       ********   ******     "};
char msg9[81]={"                                                                                "};
	printf(msg1);	
	printf(msg2);	
	printf(msg3);	
	printf(msg4);	
	printf(msg5);	
	printf(msg6);	
	printf(msg7);	
	printf(msg8);	
	printf(msg9);	
}

	
PRIVATE void printcat(){
printline();
printf("    1352956 Yin Shaohan         1352958 Jin Min           1352960 Zhang Hang    ");
printf("      /\\        /\\              /\\          /\\          /\\            /\\      ");
printf("  |----------------------|   |----------------------|   |----------------------|");
printf("  |                      |   |                      |   |                      |");
printf("  |                      |   |                      |   |                      |");
printf("  |   -              -   |   |   -              -   |   |   -              -   |");
printf("  |==         3        ==|   |==         3        ==|   |==         3        ==|");
printf("  |                      |   |                      |   |                      |");
printf("   ------====--====------     ------====--====-----      ------====--====------  ");
printf("        =    ==    =               =    ==    =               =    ==    =      ");
printf("        =    ==    =               =    ==    =               =    ==    =      ");
printf("        =----------=~~~            =----------=~~~             =----------=~~~   ");

}

