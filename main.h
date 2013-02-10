#ifndef MAIN_H
#define MAIN_H

struct customerlist {
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

struct track { //list for single track
		struct customerlist* customer;
		float cur_cost;
		struct track *next;
};

struct sol_list { //list of tracks
		struct track *start;
		struct sol_list *next;
};

struct endwindows {
		int l;
		struct customerlist* customer;
};

struct tabu {
		int pos1, pos2;
		struct tabu* next;
};

#endif // MAIN_H
