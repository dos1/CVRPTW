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

//selection sort
void S_sort(struct endwindows tab[],int n)
{
    unsigned long int i, j, min; 
	struct endwindows tmp;
	for(i=0; i<n; i++)
	{
        min = i;
		for(j=i+1; j<n; j++)
		{
            if(tab[j].l<tab[min].l) 
                min = j;
        }
        tmp = tab[i];          
        tab[i] = tab[min];
        tab[min] = tmp;
    }
}

void customerlist_push_back(struct customerlist** head_reference,struct customerlist ** tail_reference, int i, int x, int y, int q, int e, int l, int d, double road_to_depot){
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

void sol_list_append(struct sol_list** headRef,struct track* begin){
	struct sol_list* cur= *headRef;
	
	struct sol_list* pom = malloc(sizeof(struct sol_list));
	pom->start=begin;
	pom->next=NULL;

	if(cur == NULL){
		*headRef=pom;
	}else{
		while(cur->next != NULL){
			cur=cur->next;
		}
		cur->next = pom;
	}
}

void track_append(struct track** pointer, int i){
	struct track* cur=*pointer;

	struct track* node= malloc(sizeof(struct track));
	node->i=i;
	node->next=NULL;

	if(cur == NULL){
		*pointer=node;
	}else{
		while(cur->next !=NULL){
			cur=cur->next;
		}	
		cur->next=node;
	}
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

	fscanf(input_file,"%*s VEHICLE NUMBER CAPACITY %*d %d",&Q); //whole while above can be done with fscanf in simmilar fasion to the one down
	//printf("Q %d\n",Q);

	fscanf(input_file," CUSTOMER\nCUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME 0 %d %d %*d %d %d %*d\n",&x_0,&y_0,&e_0,&l_0);
	//printf("read for depot : x %d, y %d,  servStart %d, servEND %d\n",x_0,y_0,e_0,l_0);

	while (!feof(input_file) && veh_count != -1) {
		fscanf(input_file," %d %d %d %d %d %d %d\n",&i,&x,&y,&q,&e,&l,&d);
		//printf("read current : i %d, x %d, y %d, DEMAND %d,  servStart %d, servEND %d, servTIME %d\n",i,x,y,q,e,l,d);
		road=sqrt((double)((x_0-x)*(x_0-x)+(y_0-y)*(y_0-y)));
		if ((q>Q) || (road>l) || ((road > e ? road : e) + d + road) > l_0) {
			printf("niedopuszczalne\n");
			veh_count=-1;
		}
		customerlist_push_back(&head,&tail,i,x,y,q,e,l,d,road);
	}
	fclose(input_file);
	//printf("%p \n",head);
	/* creating a additional table of struct endwindows + sorting it */
	//int n=i; //number of customers
	int k=0;
	struct customerlist *pom=NULL;
	pom=head;
	struct endwindows *ew_table=(struct endwindows*)calloc(i,sizeof(struct endwindows));
	while(pom!=NULL){
		ew_table[k].l=pom->l;
		ew_table[k].customer=pom;
		//printf("lol %p - %p\n",pom,ew_table[k].customer);
		pom=pom->next;
		k++;
	}

	S_sort(ew_table, i);


	//for(k=0;k<i;k++) printf ("%d - %p\n",ew_table[k].l,ew_table[k].customer);
	
	/*heuristics algoritm part, generating solution */
	int prev_x=0,prev_y=0, cur_q=0;
	double prev_road0=0;
	double road_next=0;
	struct sol_list *solution=NULL;
	struct track *cur_track=NULL;	
	pom=NULL;
	//int ew_count=0; //iterating thingy for table of endwindows
	while(head!=NULL && processing && veh_count!=-1){
		k=0;
		prev_x=x_0;
		prev_y=y_0;
		cur_q=Q;
		cur_cost=(double)e_0;
		cur_track=NULL;
		//placeholder <-add new track to list of solutions
		veh_count++;
		while(k<i && cur_q>0 && processing){
			while(k<i && (double)ew_table[k].l<cur_cost) k++;
			if(k<i){
				pom=ew_table[k].customer; //printf("%d -- %d",pom,ew_table[k].customer); sleep(200);
				road_next=sqrt((double)((prev_x-pom->x)*(prev_x-pom->x)+(prev_y-pom->y)*(prev_y-pom->y)));
				if( (pom->q<=cur_q) && (road_next+cur_cost<=(double)(pom->l)) && (( road_next+cur_cost>pom->e ? road_next+cur_cost : pom->e) + pom->d + pom->road0 < l_0)){
					prev_x=pom->x;
					prev_y=pom->y;
					prev_road0=pom->road0;
					cur_q-=pom->q;
					if(road_next+cur_cost>(double)pom->e){
						cur_cost+= road_next + (double)pom->d;
					}else cur_cost=(double)(pom->e+pom->d);
					//cur_cost+=(double)(( road_next+cur_cost>(double)pom->e ? road_next : pom->e) + pom->d);
					//printf("%d ",pom->i);//<- placeholder[[add to solution]] sol->i=pom->i;
					track_append(&cur_track,pom->i);
					if((pom->prev)!=NULL){ 
						(pom->prev)->next=pom->next;
					}else head=pom->next;
					if((pom->next)!=NULL){ 
						(pom->next)->prev=pom->prev;
					}else tail=pom->prev;
					if(pom->prev==NULL && pom->next==NULL) head=NULL;
					free(pom);
					ew_table[k].l=-1; //guarnatee of not visiting again
				}
			}
			k++;
		}
		//printf("\n");
		sol_list_append(&solution,cur_track);
	 	//until I do the correct solution saving
		total_cost+=cur_cost+prev_road0-(double)e_0;
	}


	if(!processing && head!=NULL){
		cur_track=NULL;
		pom=head;
		head=head->next;

		track_append(&cur_track,pom->i);
		free(pom);

		sol_list_append(&solution,cur_track);
	}

	/* write solution to file */
	FILE *output_file=NULL;
	output_file=fopen(argv[2],"w");
	if (output_file==NULL) {
		perror("Error opening output file ");
		return 1;
	}	
	fprintf(output_file,"%d %.5f\n",veh_count, total_cost);
	
	cur_track=NULL;
	struct sol_list* cur_out=NULL;
	struct track* last=NULL;
	if(veh_count !=-1){
		while(solution!=NULL){
			cur_track=solution->start;
			while(cur_track->next!=NULL){
				last=cur_track;
				cur_track=cur_track->next;

				fprintf(output_file,"%d ",last->i);
				free(last);
			}
			fprintf(output_file,"%d\n",cur_track->i);
			free(cur_track);
			cur_out=solution;
			solution=solution->next;
			free(cur_out);
		}
	}

	fclose(output_file);
	FreeList(&head);
	return 0;
}
