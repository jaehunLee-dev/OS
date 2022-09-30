#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

typedef struct _ps {
	int pid;
	int at;
	int initq;
	int cycle;
	int* burst_time;
	int c_index;
	int num1;
}ps;

typedef struct _Node {
	struct Node* next;
	ps* data;
}Node;


int ps_num;
int global;
int slice;



Node* RQ_0;
Node* RQ_1;
Node* RQ_2;
Node* RQ_3;
ps** befQ;
ps** ioQ;

ps* schedule();
ps* setPs(int pid, int at, int initq, int cycle);
int move_queue(ps* ps);
int io_check();


int main() {
	int* turn_around;
	int* burst;
	int* pidinfo;

	FILE* file;
	file = fopen("input.txt", "r");
	fscanf(file, "%d", &ps_num);

	befQ = (ps**)malloc(sizeof(ps)*ps_num);
	ioQ = (ps**)malloc(sizeof(ps)*ps_num);
	pidinfo = (int*)malloc(sizeof(int)*ps_num);
	turn_around = (int*)malloc(sizeof(int)*ps_num);
	burst = (int*)malloc(sizeof(int)*ps_num);

	for (int i = 0; i < ps_num; i++) {
		turn_around[i] = 0;
		burst[i] = 0;
	}



	for (int i = 0; i < ps_num; i++)
		ioQ[i] = NULL;

	RQ_0 = (Node*)malloc(sizeof(Node));
	RQ_0->data = NULL;
	RQ_0->next = NULL;
	RQ_1 = (Node*)malloc(sizeof(Node));
	RQ_1->data = NULL;
	RQ_1->next = NULL;
	RQ_2 = (Node*)malloc(sizeof(Node));
	RQ_2->data = NULL;
	RQ_2->next = NULL;
	RQ_3 = (Node*)malloc(sizeof(Node));
	RQ_3->data = NULL;
	RQ_3->next = NULL;


	ps* current;
	current = NULL;

	int pid, at, initq, cycle;
	ps* newPs;

	for (int i = 0; i < ps_num; i++) {
		fscanf(file, "%d %d %d %d", &pid, &at, &initq, &cycle);
		pidinfo[i] = pid;
		newPs = setPs(pid, at, initq, cycle);
		newPs->num1 = i;
		for (int j = 0; j < cycle * 2 - 1; j++) {
			fscanf(file, "%d", &(newPs->burst_time[j]));
			burst[i] += newPs->burst_time[j];
		}


		befQ[i] = newPs;

	}

	printf("pid  :      ");
	for (int i = 0; i<ps_num; i++)
		printf("        %d", befQ[i]->pid);
	printf("\n\n");

	while (1) {
		for (int i = 0; i < ps_num; i++) {
			if (befQ[i] != NULL && befQ[i]->at == global) {
				move_queue(befQ[i]);
				befQ[i] = NULL;
			}
		}

		int endbefQ = 0;
		int io_ch = 0;

		//check if there is no ps in befQ 
		for (int i = 0; i < ps_num; i++) {
			if (befQ[i] != NULL) {
				endbefQ = 0;
				break;
			}
			endbefQ = 1;
		}

		//check if there is no ps in ioQ
		for (int i = 0; i < ps_num; i++) {
			if (ioQ[i] != NULL) {
				io_ch = 0;
				break;
			}
			io_ch = 1;
		}

		io_check();

		//finish program if there is no ps
		if (current == NULL) {
			current = schedule();
			if (current == NULL) {
				if (endbefQ == 1 && io_ch == 1) {
					printf("time :  %d--------------- end scheduling---------------\n", global);
					break;
				}
			}
		}
		//print current ps
		if (current != NULL) {
			printf("time :%5d", global);
			for (int i = 0; i < current->num1 + 1; i++)
				printf("         ");
			printf("O\n");
		}
		else
			printf("time :%5d\t--------cpu is idle--------\n", global);

		//time go
		global += 1;
		slice -= 1;
		for (int i = 0; i < ps_num; i++) {
			if (ioQ[i] != NULL) {
				(ioQ[i]->burst_time[ioQ[i]->c_index])--;
			}
		}

		//cpu burst
		if (current != NULL) {
			current->burst_time[current->c_index] -= 1;
			if (current->burst_time[current->c_index] == 0) {

				current->c_index++;
				if (current->c_index == (current->cycle) * 2 - 1) {
					turn_around[current->num1] = (global - current->at);
					free(current->burst_time);
					free(current);
					current = NULL;
				}

				else if (current->initq < 3 && slice == 0)
					current->initq += 1;

				else {
					ioQ[current->num1] = current;

					current = NULL;
				}
			}

			else {
				if (slice == 0) {
					if (current->initq < 3)
						current->initq += 1;
					move_queue(current);
					current = NULL;
				}
			}
		}
	}

	//print turnaround time and waiting time
	double turn_avg = 0, wait_avg = 0;

	printf("\n");
	for (int i = 0; i < ps_num; i++) {
		printf("pid : %d\t turnaround time : %d\t waiting time : %d\n", pidinfo[i], turn_around[i], turn_around[i] - burst[i]);
		turn_avg += turn_around[i];
		wait_avg += turn_around[i] - burst[i];
	}
	turn_avg /= ps_num;
	wait_avg /= ps_num;

	printf("\nturnaround time average : %f", turn_avg);
	printf("\nwaiting time average : %f\n", wait_avg);

	fclose(file);


	free(befQ);
	free(ioQ);
	free(RQ_0);
	free(RQ_1);
	free(RQ_2);
	free(RQ_3);
	free(burst);
	free(turn_around);
	free(pidinfo);

	return 0;
}



ps* setPs(int pid, int at, int initq, int cycle) {
	ps* newPs = (ps*)malloc(sizeof(ps));
	newPs->pid = pid;
	newPs->at = at;
	newPs->initq = initq;
	newPs->cycle = cycle;
	int arrnum = cycle * 2 - 1;
	newPs->burst_time = (int*)malloc(sizeof(int)*arrnum);
	newPs->c_index = 0;
	return newPs;
}


int move_queue(ps* ps) {
	Node* head;
	Node* newPs;
	int num2;
	num2 = ps->initq;

	if (num2 == 0)
		head = RQ_0;

	else if (num2 == 1)
		head = RQ_1;

	else if (num2 == 2)
		head = RQ_2;

	else if (num2 == 3)
		head = RQ_3;

	else return -1;
	newPs = (Node*)malloc(sizeof(Node));

	newPs->data = ps;
	newPs->next = NULL;

	while (head->next != NULL) {
		head = head->next;
	}
	head->next = newPs;
	return 0;

}

int io_check() {
	for (int i = 0; i < ps_num; i++) {
		if (ioQ[i] != NULL) {
			int time = ioQ[i]->burst_time[ioQ[i]->c_index];
			if (time == 0) {
				ioQ[i]->c_index += 1;
				int queue = ioQ[i]->initq;
				if (queue>0 && queue<3)
					ioQ[i]->initq = queue - 1;
				if(move_queue(ioQ[i])==-1)
					return 0;
				ioQ[i] = NULL;
			}
		}
	}
	return (0);
}

ps* schedule() {
	ps* result;
	Node* head;
	Node* remove;

	head = RQ_0;
	if (head == NULL || head->next == NULL) {
		head = RQ_1;
		if (head == NULL || head->next == NULL) {
			head = RQ_2;
			if (head == NULL || head->next == NULL) {
				head = RQ_3;
				if (head == NULL || head->next == NULL)
					return NULL;
				slice = -1;
			}
			else slice = 4;
		}
		else slice = 2;
	}
	else slice = 1;

	remove = head->next;
	result = remove->data;
	head->next = remove->next;
	free(remove);

	return result;
}