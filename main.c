#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	if(argc<3) {
		printf("Usage: %s input_file output_file\n",argv[0] );
		return 1;
	}

	int Q=0; //vehicle capacity
	int x_0,y_0,e_0,l_0; //data for depot
	int i,x,y,q,e,l,d; //data of current imput

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
	//while((fgets(buf,1024,input_file)!=NULL) && (strcmp(buf,"CUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME\n")!=0) ){
	//	printf("read %s\n",buf);
	//}
	fscanf(input_file," CUSTOMER\nCUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME 0 %d %d %*d %d %d %*d\n",&x_0,&y_0,&e_0,&l_0);
	printf("read for depot : x %d, y %d,  servStart %d, servEND %d\n",x_0,y_0,e_0,l_0);
	
	printf("CVRPTW - done\n");
	fclose(input_file);
	return 0;
}
