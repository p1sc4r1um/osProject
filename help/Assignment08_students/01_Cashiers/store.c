#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define CASHIERS_NUM 2

typedef struct {
	long mtype;
	int customer_id;
	int num_products;
} customer_msg;

int mq_id;

void cleanup(int signum) {
	printf("\n\n[%d] Cleanup and exit!\n", getpid());

	/* Cleanup */

}

// Generate customer requests
void generateCustomers() {
	printf("[%d] Store is now open!\n", getpid());

	srand(time(NULL));
	int customer_id = 0;

	while (1) {
		customer_msg customer;
		long mtype = rand()%CASHIERS_NUM+1; // Select between available cashiers (mtype>0)
		int num_products = rand()%8+2; // Products between 2 and 10

		/* Create customer */

		/* Print customer details */

		/* Add customer to Message Queue */

		// Time between customers arrival
		sleep(rand()%8+2);
	}
}

// Cachiers process
void cashier(int i) {
	printf("[%d] Cashier %d open!\n", getpid(), i);

	customer_msg customer;

	while (1) {
		// Process next customer in cachier queue
		printf("[%d] Cashier %d waiting for next customer.\n", getpid(), i);

		/* Get next customer from message queue */

		/* Print customer details */

		/* Process customer products */
	}
}

void main(int argc, char* argv[]) {
	pid_t cashiers[CASHIERS_NUM];
	int i;

	/* Handle ^C */

	/* Create Message Queue */


	/* Create cashiers processes */

	/* Generate customer requests */
}
