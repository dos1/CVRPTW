#ifndef MAIN_H
#define MAIN_H

struct customerlist{
		int i; 		//number
		int x,y; 	//coordinates
		int q;		//demand
		int e;		//ready time
		int l;		//due date
		int d;		//service time
		double road0; //road to depot
		struct customerlist* next;
		struct customerlist* prev;
};

struct track{ //list for single track
	int i;
	struct track *next;
};

struct sol_list{ //list of tracks
	struct track *head;
	struct sol_list *next;
};

struct endwindows{
	int l;
	struct customerlist* customer;
};

#endif // MAIN_H
