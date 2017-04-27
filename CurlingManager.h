#ifndef _CURLINGMANAGER_
#define _CURLINGMANAGER_
#include <iostream>
#include "curling.h"
#include "Drawer.h"
#include <vector>
using namespace std;
class CurlingManager
{
public:
	CurlingManager(){
		score0 = 0;
		score1 = 0;
		CurlingSet.clear();
		state = 0;
	}
	~CurlingManager();

	//add a new curling
	void addCurling(double x, double y, bool team);

	//update the locations of all curlings
	void update(double time, double fraction);

	//check collision
	bool check_collision(curling c1, curling c2);

	//collision result
	void collide(curling* c1, curling* c2);
	void collide(curling* c1);

	//Calculate and reset the Scores
	void Calculate_Scores();

	//show all curlings();
	void show_all_curlings();

	//set the intial speed
	void SetInitSpeed(int index, double theta, double Force);

	int size(){
		return CurlingSet.size();
	}

	int getscore0(){
		return score0;
	}

	int getscore1(){
		return score1;
	}

	void teamchange(){
		state = !state;
	}

	bool getState(){
		return state;
	}

	curling getcurling(int i){
		return *CurlingSet[i];
	}

private:
	vector<curling*> CurlingSet;
	int score1;
	int score0;
	bool state; //which team is playing.
	
};

#endif
