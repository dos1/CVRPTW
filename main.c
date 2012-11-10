#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct customerlist{
	int i; 		//number
	int x,y; 	//coordinates
	int q;		//demand
	int e;		//ready time
	int l;		//due date
	int d;		//service time
	struct customerlist* next;
};

void Push(struct customerlist** head_reference,int i, int x, int y, int q, int e, int l, int d){
	struct customerlist* newcustomer = malloc(sizeof(struct customerlist));
	newcustomer->i = i;
	newcustomer->x = x;
	newcustomer->y = y;
	newcustomer->q = q;
	newcustomer->e = e;
	newcustomer->l = l;
	newcustomer->d = d;

	newcustomer->next = *head_reference;
	*head_reference = newcustomer;
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
	if(argc<3) {
		printf("Usage: %s input_file output_file\n",argv[0] );
		return 1;
	}

	int Q=0; //vehicle capacity
	int x_0,y_0,e_0,l_0; //data for depot
	int i=0,x=0,y=0,q=0,e=0,l=0,d=0; //data of current imput i-number; x,y-coordinates; q-demand; e-ready; l-due date; d-service time;
	struct customerlist *head=NULL; //pointers

	/** reading input file **/
	FILE *input_file=NULL;
	char buf[1024];
	input_file=fopen(argv[1],"r");
	if(input_file==NULL){perror("Error opening input file "); return 1;}
	while((fgets(buf,1024,input_file)!=NULL) && (strcmp(buf,"NUMBER     CAPACITY\n")!=0) ){ //reaching the needed part in file (probably this is lame )
		printf("line %s \n",buf); 
	}
	fscanf(input_file,"%*d %d",&Q); //whole while above can be done with fscanf in simmilar fasion to the one down
	printf("Q %d\n",Q);

	fscanf(input_file," CUSTOMER\nCUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME 0 %d %d %*d %d %d %*d\n",&x_0,&y_0,&e_0,&l_0);
	printf("read for depot : x %d, y %d,  servStart %d, servEND %d\n",x_0,y_0,e_0,l_0);
	while(!feof(input_file)){
		fscanf(input_file," %d %d %d %d %d %d %d\n",&i,&x,&y,&q,&e,&l,&d);
		printf("read current : i %d, x %d, y %d, DEMAND %d,  servStart %d, servEND %d, servTIME %d\n",i,x,y,q,e,l,d);
		Push(&head,i,x,y,q,e,l,d);
	}
	fclose(input_file);
	printf("CVRPTW - done\n");

	FreeList(&head);
	return 0;
}
