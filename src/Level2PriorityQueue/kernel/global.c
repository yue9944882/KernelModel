
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	int			PQueueLv1[NR_TASKS];
PUBLIC	int			PQueueLv2[NR_TASKS];
PUBLIC	int			PQueueLv3[NR_TASKS];

PUBLIC	int			Lv1Size=0;
PUBLIC	int			Lv2Size=0;
PUBLIC	int			Lv3Size=0;

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {{TestA, STACK_SIZE_TESTA, "TestA"},
					{TestB, STACK_SIZE_TESTB, "TestB"},
					{TestC, STACK_SIZE_TESTC, "TestC"},
					{TestD,STACK_SIZE_TESTD,"TestD"},
					{TestE,STACK_SIZE_TESTE,"TestE"},
					{TestF,STACK_SIZE_TESTF,"TestF"}};

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks};

