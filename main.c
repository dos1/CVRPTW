#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#include "main.h"

bool processing = false;

void* timerThread(void *arg) {
	sleep(60*4.5);
	processing = false;
	return NULL;
}

//selection sort
void S_sort(struct endwindows tab[],int n) {
	unsigned long int i, j, min;
	struct endwindows tmp;
	for(i=0; i<n; i++) {
		min = i;
		for(j=i+1; j<n; j++) {
			if(tab[j].l<tab[min].l)
				min = j;
		}
		tmp = tab[i];
		tab[i] = tab[min];
		tab[min] = tmp;
	}
}

void customerlist_push_back(struct customerlist** head_reference,struct customerlist ** tail_reference, int i, int x, int y, int q, int e, int l, int d, double road_to_depot) {
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
	if (*tail_reference!=NULL) (*tail_reference)->next=newcustomer;
	*tail_reference = newcustomer;
	if (*head_reference==NULL) *head_reference = newcustomer;
}

void sol_list_append(struct sol_list** headRef,struct track* begin) {
	struct sol_list* cur = *headRef;
	
	struct sol_list* pom = malloc(sizeof(struct sol_list));
	pom->start=begin;
	pom->next=NULL;

	if(cur == NULL) {
		*headRef=pom;
	} else {
		while (cur->next != NULL) {
			cur=cur->next;
		}
		cur->next = pom;
	}
}

void track_append(struct track** pointer, struct customerlist* customer, int cur_cost) {
	struct track* cur = *pointer;

	struct track* node = malloc(sizeof(struct track));
	node->customer = customer;
	node->cur_cost = cur_cost;
	node->next=NULL;

	if (cur == NULL) {
		*pointer=node;
	} else {
		while (cur->next !=NULL) {
			cur=cur->next;
		}
		cur->next=node;
	}
}

void FreeList(struct customerlist** head_reference) {
	struct customerlist* pom;
	while (*head_reference!=NULL) {
		pom=*head_reference;
		*head_reference=(*head_reference)->next;
		free(pom);
	}
}

// TABU

void generateNext(struct sol_list* track) {

	return;
}

// ----

int main(int argc, char** argv) {
	if (argc<3) {
		printf("Usage: %s input_file output_file\n", argv[0]);
		return 1;
	}

	pthread_t timer;
	if (pthread_create(&timer, NULL, &timerThread, NULL) != 0) {
		printf("[WARNING] Could not create timer thread - processing time won't be limited!");
	}

	struct timeval time_start, time_end;
	gettimeofday(&time_start, NULL);

	int Q=0; //vehicle capacity
	int x_0,y_0,e_0,l_0; //data for depot
	int i=0,x=0,y=0,q=0,e=0,l=0,d=0; //data of current imput i-number; x,y-coordinates; q-demand; e-ready; l-due date; d-service time;
	struct customerlist *head=NULL,*tail=NULL; //pointers for customer list

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

	fscanf(input_file,"%*s VEHICLE NUMBER CAPACITY %*d %d",&Q); //reading Q 
	

	fscanf(input_file," CUSTOMER\nCUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME 0 %d %d %*d %d %d %*d\n",&x_0,&y_0,&e_0,&l_0);
	//reading data of depot ^

	while (!feof(input_file) && veh_count != -1) {
		fscanf(input_file," %d %d %d %d %d %d %d\n",&i,&x,&y,&q,&e,&l,&d); //reading data of customers
		
		road=sqrt((double)((x_0-x)*(x_0-x)+(y_0-y)*(y_0-y)));
		if ((q>Q) || (road>l) || ((road > e ? road : e) + d + road) > l_0) {
			//imposible instance
			veh_count=-1;
		}
		customerlist_push_back(&head,&tail,i,x,y,q,e,l,d,road);
	}
	fclose(input_file);

	/* creating a additional table of struct endwindows + sorting it */
	
	int k=0; //iterator for the table
	struct customerlist *pom=NULL;
	pom=head;
	struct endwindows *ew_table=(struct endwindows*)calloc(i,sizeof(struct endwindows));
	while(pom!=NULL) {
		ew_table[k].l=pom->l;
		ew_table[k].customer=pom;
		
		pom=pom->next;
		k++;
	}

	S_sort(ew_table, i);
	
	/*heuristics algoritm part, generating solution */
	int prev_x=0,prev_y=0, cur_q=0; //coordinates of last visited customer, current capacity of the truck
	double prev_road0=0; //road to depot from the last visited 
	double road_next=0;  //road to currently tested customer
	struct sol_list *solution=NULL; //list holding solutions
	struct track *cur_track=NULL;   // begining of the track
	pom=NULL;
	
	while (head!=NULL && processing && veh_count!=-1) {
		k=0;                  //setting the starting values
		prev_x=x_0;
		prev_y=y_0;
		cur_q=Q;
		cur_cost=(double)e_0;
		cur_track=NULL;
		prev_road0 = 0;
		
		veh_count++;
		while (k<i && cur_q>0 && processing) {
			while (k<i && (double)ew_table[k].l<cur_cost) k++;
			if (k<i) {
				pom=ew_table[k].customer; 
				road_next=sqrt((double)((prev_x-pom->x)*(prev_x-pom->x)+(prev_y-pom->y)*(prev_y-pom->y)));
				if( (pom->q<=cur_q) && (road_next+cur_cost<=(double)(pom->l)) && (( road_next+cur_cost>pom->e ? road_next+cur_cost : pom->e) + pom->d + pom->road0 < l_0)){
					prev_x=pom->x;
					prev_y=pom->y;
					prev_road0=pom->road0;
					cur_q-=pom->q;
					if (road_next+cur_cost>(double)pom->e) {
						cur_cost+= road_next + (double)pom->d;
					} else cur_cost=(double)(pom->e+pom->d);

					track_append(&cur_track,pom, cur_cost);
					if ((pom->prev)!=NULL) {
						(pom->prev)->next=pom->next;
					} else head=pom->next;
					if ((pom->next)!=NULL) {
						(pom->next)->prev=pom->prev;
					} else tail=pom->prev;
					if (pom->prev==NULL && pom->next==NULL) head=NULL;
					//free(pom);
					ew_table[k].l=-1; //guarnatee of not visiting again
				}
			}
			k++;
		}
		
		sol_list_append(&solution,cur_track);
		
		total_cost+=cur_cost+prev_road0-(double)e_0;
	}

	/* complete solution if time limit exceeded */
	if (!processing && head!=NULL){
		cur_track=NULL;
		pom=head;
		head=head->next;

		cur_cost = ( pom->road0 > pom->e ? pom->road0 : pom->e) + pom->d + pom->road0;
		total_cost+= cur_cost;
		track_append(&cur_track,pom,cur_cost);
		//free(pom);

		sol_list_append(&solution,cur_track);
	}

	/* HERE COMES MAGIC */

	while (processing) {
		struct sol_list *pom = solution;
		while (pom) {

			generateNext(pom);

			pom=pom->next;
		}
		// powiedzmy, że to tabu search
		// ta funkcja wyżej wcale nie jest niezaimplementowana
		// srsly
		// 3 pls
		break;
	}

	/* write solution to file */
	FILE *output_file=NULL;
	output_file=fopen(argv[2], "w");
	if (output_file==NULL) {
		perror("Error opening output file ");
		return 1;
	}
	fprintf(output_file,"%d %.5f\n",veh_count, total_cost);
	
	cur_track=NULL;
	struct sol_list* cur_out=NULL;
	struct track* last=NULL;
	if (veh_count !=-1) {
		while (solution!=NULL) {
			cur_track=solution->start;
			while (cur_track->next!=NULL) {
				last=cur_track;
				cur_track=cur_track->next;

				fprintf(output_file,"%d ",last->customer->i);
				free(last);
			}
			fprintf(output_file,"%d\n",cur_track->customer->i);
			free(cur_track);
			cur_out=solution;
			solution=solution->next;
			free(cur_out);
		}
	}

	fclose(output_file);
	FreeList(&head);

	gettimeofday(&time_end, NULL);
	time_t secs = time_end.tv_sec - time_start.tv_sec;
	suseconds_t usecs;
		if (time_end.tv_usec < time_start.tv_usec) {
		secs -= 1;
		usecs = time_start.tv_usec - time_end.tv_usec;
	} else {
		usecs = time_end.tv_usec - time_start.tv_usec;
	}
	printf("%s\n",argv[1]); //printing useful data
	printf("Time\t\tRoutes\t\tCost\n");
	printf("%d.%.6d\t%d\t\t%.5f\n", (int)secs, (int)usecs, veh_count, total_cost);
	return 0;
}
