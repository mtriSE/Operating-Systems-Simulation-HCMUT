#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
	if (q == NULL) return 1;
	return (q->size == 0);
}

void printQueue(struct queue_t * q) {
	printf("[");
	for (int i = 0; i<q->size;++i) {
		printf("(%d)",q->proc[i]->pid);
	}
	printf("]\n");
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
	/* TODO: put a new process to queue [q] */
	q->proc[q->size] = proc;
	q->size += 1;
}

struct pcb_t * dequeue(struct queue_t * q) {
	/* TODO: return a pcb whose prioprity is the highest
		* in the queue [q] and remember to remove it from q
		* */
	if(empty(q)) return NULL;

	// If the queue is not empty ...
	struct pcb_t *temp = q->proc[0];

	for(int i = 0; i < q->size - 1; i++) {
		q->proc[i] = q->proc[i + 1];
	}

	q->proc[q->size - 1] = NULL;
	q->size -= 1;

	return temp;
}

