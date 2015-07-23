
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

PRIVATE int queue_rotate(int,int);

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	///Maximun Process Def : 10
	PROCESS* p;
	PROCESS*tmpPtr;
	/*if(Lv2Size==3){
		if (1) {
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks = p->priority=1;
			}
		}
		Lv1Size=3;Lv2Size=0;
		p_proc_ready=proc_table;
		return;
	}*/
	//Lv1Size=3;Lv2Size=0;	
	/*while(!selected){
		disp_str("\t|");
		int iLoop;
		int tmpIndex;
		int tmpSize;
		char msg[7]={"-nnn-"};
		msg[1]=Lv1Size+0x30;
		msg[2]=Lv2Size+0x30;
		msg[3]=Lv3Size+0x30;
		if(Lv1Size){
			tmpSize=Lv1Size;
			for(iLoop=0;iLoop<tmpSize;iLoop++){
				tmpIndex=PQueueLv1[iLoop];
				tmpPtr=&proc_table[tmpIndex];
				if(tmpPtr->ticks<=0){
					///Priority Switching
					tmpPtr->priority++;
					tmpPtr->ticks=2;
					queue_rotate(1,0,Lv1Size);
					--Lv1Size;
					PQueueLv2[Lv2Size]=tmpIndex;
					++Lv2Size;
				}
			}
			if(Lv1Size){
				tmpIndex=PQueueLv1[0];
				p_proc_ready=&(proc_table[tmpIndex]);
				queue_rotate(1,0,Lv1Size);
		msg[1]=Lv1Size+0x30;
		msg[2]=Lv2Size+0x30;
		msg[3]=Lv3Size+0x30;
				selected=1;disp_str(msg);
				break;
			}else{
				;//Continue
			}
		}

		if(Lv2Size){
			tmpIndex=PQueueLv2[0];
			p_proc_ready=&proc_table[tmpIndex];
			//if(p_proc_ready->ticks<=0)p_proc_ready->ticks=4;
			queue_rotate(2,0,Lv2Size);
		msg[1]=Lv1Size+0x30;
		msg[2]=Lv2Size+0x30;
		msg[3]=Lv3Size+0x30;
			selected=1;disp_str(msg);
			break;
			/*tmpSize=Lv2Size;
			for(iLoop=0;iLoop<tmpSize;iLoop++){
				tmpIndex=PQueueLv2[iLoop];
				tmpPtr=&(proc_table[tmpIndex]);
				if(tmpPtr->ticks<=0){
					///Priority Switching
					tmpPtr->priority++;
					tmpPtr->ticks=4;
					queue_rotate(2,0,Lv2Size--);
					PQueueLv3[Lv3Size++]=tmpIndex;
				}
			}
			if(Lv2Size){
				tmpIndex=PQueueLv2[0];
				p_proc_ready=&proc_table[tmpIndex];
				queue_rotate(2,0,Lv2Size);
				selected=1;disp_str(msg);
				break;
			}else{
				;//Continue
			}*/
			
	/*	}

		if(Lv3Size){
			tmpIndex=PQueueLv3[0];
			p_proc_ready=&proc_table[tmpIndex];
			if(p_proc_ready->ticks<=0)p_proc_ready->ticks=4;
			disp_str("tail");
			queue_rotate(3,0,Lv3Size);
			selected=1;disp_str(msg);
			break;
		}
	}*/
	int selected=0;
	while(!selected){
		disp_str("*aee*");
		int targetIndex=0;
		int totalLen=NR_TASKS/sizeof(PROCESS);
		for(p=proc_table;p<proc_table+NR_TASKS;p++,targetIndex++){
			if(p->priority==1){
				if(p->ticks<=1&&p->ticks>0){
					//Havent Finished Yet,Continue
					p_proc_ready = p;
					queue_rotate(targetIndex,totalLen);
					selected=1;
				//	break;
				}else{
					//Ready to Switch,Switch Priority
					p->priority=2;
					p->ticks=2;
					continue;
				}
			}
		}
		if(selected)break;
		///No Level 1 Process,Scan Level 2
		targetIndex=0;
		for(p=proc_table;p<proc_table+NR_TASKS;p++,targetIndex++){
			if(p->priority==2){
				if(p->ticks<=2&&p->ticks>0){
					p_proc_ready = p;
					queue_rotate(targetIndex,totalLen);
					selected=1;
				//	break;
				}else{
					p->priority=3;
					p->ticks=4;
					continue;
				}
			}
		}
		if(selected)break;
		///No Level 2 Process,Scan Level 3
		///Level 3 Using Rotating Strategy
		disp_str("*bee*");
		for(p=proc_table;p<proc_table+NR_TASKS;p++){
			p_proc_ready=p;
			if(p->ticks==0)p->ticks=8;
			queue_rotate(0,totalLen);
			selected=1;
			break;
		}
	}
	int greatest_ticks=0;
	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks = p->priority;
			}
		}
	}
}


PRIVATE int queue_rotate(int target,int len){
	PROCESS*p=proc_table;
	char msg[4]={"^m^"};
	int tmpVar;
	/*if(index==1){
		int i1;
		tmpVar=PQueueLv1[target];
		for(i1=target+1;i1<len;i1++){
			PQueueLv1[i1-1]=PQueueLv1[i1];
		}PQueueLv1[i1]=tmpVar;
	}else if(index==2){
		int i2;
		tmpVar=PQueueLv2[target];
		for(i2=target+1;i2<len;i2++){
			PQueueLv2[i2-1]=PQueueLv2[i2];
		}PQueueLv2[i2]=tmpVar;
	}else if(index==3){
		disp_str("%3%");
		int i3;
		tmpVar=PQueueLv3[target];
		for(i3=target+1;i3<len;i3++){
			PQueueLv3[i3-1]=PQueueLv3[i3];
		}PQueueLv3[i3]=tmpVar;
	}
	msg[1]=tmpVar+0x30;
	disp_str(msg);*/
	int iLoop;
	for(iLoop=0;iLoop<target;iLoop++){
		p++;
	}

	PROCESS*tail=&proc_table[len-1];
	PROCESS tmpProc={};
	PROCESS*tmpProcPtr=&tmpProc;
	//Inner Stack Swap
	*tmpProcPtr=*p;
	int jLoop;
	for(jLoop=target+1;jLoop<len;jLoop++){
		proc_table[jLoop-1]=proc_table[jLoop];
	}proc_table[jLoop]=*tmpProcPtr;
	
	return 0;
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

