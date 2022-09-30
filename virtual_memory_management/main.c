#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main() {
	int N, M, W, K;
	int* pg_ref;
	int *memory;
	int *timestamp_load;
	int *timestamp_ref;
	int *ref_cnt;
	int* ws;
	
	FILE* file;
	file = fopen("input.txt", "r");
	fscanf(file, "%d", &N);
	fscanf(file, "%d", &M);
	fscanf(file, "%d", &W);
	fscanf(file, "%d", &K);
	pg_ref = (int*)malloc(sizeof(int)*K);
	memory = (int*)malloc(sizeof(int)*M);
	timestamp_load = (int*)malloc(sizeof(int)*M);
	timestamp_ref = (int*)malloc(sizeof(int)*M);
	ref_cnt = (int*)malloc(sizeof(int)*N);
	ws = (int*)malloc(sizeof(int)*N); //The working set changes in size, but the maximum size is N.

	for (int i = 0; i < K; i++) {
		fscanf(file, "%d", &pg_ref[i]);
	}
	fclose(file);

	for (int i = 0; i < M; i++) {
		memory[i] = -1;
		timestamp_load[i] = 0;
		timestamp_ref[i] = 0;
	}

	for (int i = 0; i < N; i++) {
		ref_cnt[i] = 0;
		ws[i] = 0;
	}

	int fault_min = 0;
	int fault_fifo = 0;
	int fault_lru = 0;
	int fault_lfu = 0;
	int fault_ws = 0;	

	printf("MIN Algorithm starts\n");
	for (int time = 1; time < K + 1; time++) {	//MIN algorithm start

		int exist = 0;
		int* Df = (int*)malloc(sizeof(int)*M);

		for (int i = 0; i < M; i++) {
			if (memory[i] == pg_ref[time-1]) {
				exist = 1;
			}
		}

		if (exist == 0) {
			fault_min += 1;
			int empty = 0;

			for (int i = 0; i < M; i++) {
				if (memory[i] == -1) {
					memory[i] = pg_ref[time - 1];
					empty = 1;
					break;
				}
			}

			if (empty == 0) {
				for (int i = 0; i < M; i++) {
					Df[i] = 1001; //because max page reference string length is 1000
				}

				for (int i = 0; i < M; i++) {
					for (int j = time; j < K; j++) {
						if (memory[i] == pg_ref[j]) {
							Df[i] = j - time;
							break;
						}
					}
				}

				int victim_idx = 0;

				for (int i = 1; i < M; i++) {
					if (Df[i] == 1001) {
						if (memory[victim_idx] > memory[i])
							victim_idx = i;
					}
					if (Df[i] > Df[victim_idx]) {
						victim_idx = i; //tie-breaking rule : Replace the smallest page
					}
				}

				free(Df);
				memory[victim_idx] = pg_ref[time - 1];
			}
		}
		printf("time: %4d ", time);
	
		printf("memory: ");
		for (int i = 0; i < M; i++) {
			if (memory[i] != -1)
				printf(" %2d ", memory[i]);
			else
				printf(" -- ");
		}

		if (exist == 0)
			printf("\tpage fualt!\t");
		printf("\n");
	}
	printf("total page fault : %d\n", fault_min);

	printf("\nFIFO Algorithm starts\n");
	for (int i=0; i<M; i++)		//clear page frame(memory) for next algorithm
		memory[i] = -1;

	for (int time = 1; time < K + 1; time++) { //FIFO algorithm start, use timestamp_load
		int exist = 0;
		for (int i = 0; i < M; i++) {
			if (memory[i] == pg_ref[time - 1]) {
				exist = 1;
			}
		}

		if (exist == 0) {
			fault_fifo += 1;
			int empty = 0;

			for (int i = 0; i < M; i++) {
				if (memory[i] == -1) {
					memory[i] = pg_ref[time - 1];
					timestamp_load[i] = time;
					empty = 1;
					break;
				}
			}
			
			if (empty == 0) {
				int victim_idx = 0;
				for (int i = 1; i < M; i++) {
					if (timestamp_load[victim_idx] > timestamp_load[i])
						victim_idx = i;
				}
				memory[victim_idx] = pg_ref[time - 1];
				timestamp_load[victim_idx] = time;
			}
		}
		printf("time: %4d ", time);

		printf("memory: ");
		for (int i = 0; i < M; i++) {
			if (memory[i] != -1)
				printf(" %2d ", memory[i]);
			else
				printf(" -- ");
		}

		if (exist == 0)
			printf("\tpage fualt!\t");
		printf("\n");
	}

	printf("total page fault : %d\n", fault_fifo);

	for (int i = 0; i<M; i++)		//clear page frame(memory) for next algorithm
		memory[i] = -1;

	printf("\nLRU Algorithm starts\n");
	for (int time = 1; time < K + 1; time++) { //LRU algorithm start, use timestamp_ref
		int exist = 0;
		for (int i = 0; i < M; i++) {
			if (memory[i] == pg_ref[time - 1]) {
				exist = 1;
				timestamp_ref[i] = time;
				break;
			}
		}

		if (exist == 0) {
			fault_lru += 1;
			int empty = 0;

			for (int i = 0; i < M; i++) {
				if (memory[i] == -1) {
					memory[i] = pg_ref[time - 1];
					timestamp_ref[i] = time;
					empty = 1;
					break;
				}
			}

			if (empty == 0) {
				int victim_idx = 0;
				for (int i = 1; i < M; i++) {
					if (timestamp_ref[victim_idx] > timestamp_ref[i])
						victim_idx = i;
				}
				memory[victim_idx] = pg_ref[time - 1];
				timestamp_ref[victim_idx] = time;
			}
		}
		printf("time: %4d ", time);

		printf("memory: ");
		for (int i = 0; i < M; i++) {
			if (memory[i] != -1)
				printf(" %2d ", memory[i]);
			else
				printf(" -- ");
		}
		if (exist == 0)
			printf("\tpage fualt!\t");
		printf("\n");
	}

	printf("total page fault : %d\n", fault_lru);

	for (int i = 0; i < M; i++) {		//clear page frame(memory) and timestamp_ref for next algorithm
		memory[i] = -1;
		timestamp_ref[i] = 0;
	}

	printf("\nLFU Algorithm starts\n");
	for (int time = 1; time < K + 1; time++) { //LFU algorithm start, use ref_cnt, (timestamp_ref for tie-breaking)
		int exist = 0;
		ref_cnt[pg_ref[time - 1]] += 1;
		for (int i = 0; i < M; i++) {
			if (memory[i] == pg_ref[time - 1]) {
				exist = 1;
				timestamp_ref[i] = time;
				break;
			}
		}

		if (exist == 0) {
			fault_lfu += 1;
			int empty = 0;

			for (int i = 0; i < M; i++) {
				if (memory[i] == -1) {
					memory[i] = pg_ref[time - 1];
					timestamp_ref[i] = time;
					empty = 1;
					break;
				}
			}

			if (empty == 0) {
				int victim_idx = 0;
				int min_ref = ref_cnt[memory[0]];
				for (int i = 1; i < M; i++) {
					if (min_ref > ref_cnt[memory[i]]) {
						min_ref = ref_cnt[memory[i]];
						victim_idx = i;
					}
					else if (min_ref == ref_cnt[memory[i]] && timestamp_ref[victim_idx] > timestamp_ref[i]) {	//tie-breaking rule : LRU
						min_ref = ref_cnt[memory[i]];
						victim_idx = i;
					}
				}
				memory[victim_idx] = pg_ref[time - 1];
				timestamp_ref[victim_idx] = time;
			}
		}
		printf("time: %4d ", time);

		printf("memory: ");
		for (int i = 0; i < M; i++) {
			if (memory[i] != -1)
				printf(" %2d ", memory[i]);
			else
				printf(" -- ");
		}
		if (exist == 0)
			printf("\tpage fualt!\t");
		printf("\n");
	}
	printf("total page fault : %d\n", fault_lfu);
	//FA ends, VA starts
	
	printf("\nWS Algorithm starts\n");
	for (int time = 1; time < K+1; time++) {
		int exist = 0;
		if (time > W + 2)
			ws[pg_ref[time - 3 - W]] -= 1;
		if (ws[pg_ref[time - 1]] != 0)
			exist = 1;
		else fault_ws++;

		ws[pg_ref[time - 1]] += 1;
		printf("time: %4d ", time);
		printf("memory: ");
		for (int i = 0; i < N; i++) {
			if (ws[i] > 0)
				printf(" %2d ", i);
			else printf(" -- ");
		}
		if (exist == 0)
			printf("\tpage fault!");
		printf("\n");
	}
	printf("total page fault: %d\n", fault_ws);

	free(pg_ref);
	free(memory);
	free(timestamp_load);
	free(timestamp_ref);
	free(ref_cnt);
}