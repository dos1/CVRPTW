#ifndef MAIN_H
#define MAIN_H

struct customerlist{
		int i; 		//number
		int x,y; 	//coordinates
		int q;		//demand
		int e;		//ready time
		int l;		//due date
		int d;		//service time
		struct customerlist* next;
};

#endif // MAIN_H
