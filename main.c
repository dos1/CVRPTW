#include <stdio.h>

int main(int argc, char** argv) {
	if(argc<3) {
		printf("Usage: %s input_file output_file\n",argv[0] );
		return 1;
	}

	FILE *input=NULL;
	input=fopen(argv[1],"r");
	if(input==NULL){perror("Error opening input file "); return 1;}

	printf("CVRPTW - done\n");
	fclose(input);
	return 0;
}
