#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#include "main.h"

bool processing = false;

void* timerThread(void *arg) {
	sleep(60*4.5);
	processing = false;
	return NULL;
}

void App_tail(struct customerlist** head_reference,struct customerlist ** tail_reference, int i, int x, int y, int q, int e, int l, int d, double road_to_depot){
	struct customerlist* newcustomer = malloc(sizeof(struct customerlist));
	newcustomer->i = i;
	newcustomer->x = x;
	newcustomer->y = y;
	newcustomer->q = q;
	newcustomer->e = e;
	newcustomer->l = l;
	newcustomer->d = d;
	newcustomer->road0 = road_to_depot;

	newcustomer->next = NULL;
	newcustomer->prev = *tail_reference;
	if(*tail_reference!=NULL) (*tail_reference)->next=newcustomer;
	*tail_reference = newcustomer;
	if(*head_reference==NULL) *head_reference = newcustomer;
}

void FreeList(struct customerlist** head_reference){
	struct customerlist* pom;
	while(*head_reference!=NULL){
		pom=*head_reference;
		*head_reference=(*head_reference)->next;
		free(pom);
	}
}

int main(int argc, char** argv) {
	if (argc<3) {
		printf("Usage: %s input_file output_file\n", argv[0]);
		return 1;
	}

	pthread_t timer;
	if (pthread_create(&timer, NULL, &timerThread, NULL) != 0) {
		printf("[WARNING] Could not create timer thread - processing time is not limited!");
	}

	int Q=0; //vehicle capacity
	int x_0,y_0,e_0,l_0; //data for depot
	int i=0,x=0,y=0,q=0,e=0,l=0,d=0; //data of current imput i-number; x,y-coordinates; q-demand; e-ready; l-due date; d-service time;
	struct customerlist *head=NULL,*tail=NULL; //pointers

	double cur_cost=0, total_cost=0; //current cost/length of track, combined costs of all tracks
	int veh_count=0; // number of vehicles/tracks
	double road=0;
	/* reading input file */
	FILE *input_file=NULL;
	input_file=fopen(argv[1],"r");
	if (input_file==NULL) {
		perror("Error opening input file ");
		return 1;
	}

	processing = true;

	fscanf(input_file,"%*s VEHICLE NUMBER CAPACITY %*d %d",&Q); //whole while above can be done with fscanf in simmilar fasion to the one down
	printf("Q %d\n",Q);

	fscanf(input_file," CUSTOMER\nCUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME 0 %d %d %*d %d %d %*d\n",&x_0,&y_0,&e_0,&l_0);
	printf("read for depot : x %d, y %d,  servStart %d, servEND %d\n",x_0,y_0,e_0,l_0);

	while (!feof(input_file) && veh_count != -1) {
		fscanf(input_file," %d %d %d %d %d %d %d\n",&i,&x,&y,&q,&e,&l,&d);
		printf("read current : i %d, x %d, y %d, DEMAND %d,  servStart %d, servEND %d, servTIME %d\n",i,x,y,q,e,l,d);
		road=sqrt((double)((x_0-x)*(x_0-x)+(y_0-y)*(y_0-y)));
		if ((q>Q) || (road>l) || ((road > e ? road : e) + d + road) > l_0) {
			printf("niedopuszczalne\n");
			veh_count=-1;
		}
		App_tail(&head,&tail,i,x,y,q,e,l,d,road);
	}

	fclose(input_file);
	printf("CVRPTW - done\n");
	FreeList(&head);
	return 0;
}
