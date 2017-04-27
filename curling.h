#ifndef _CURLING_
#define _CURLING_
#include <iostream>
#include <vector>



class curling{
public:
	curling(double x, double z, bool team){
		radius = 0.4;
		mass = 1;
		stop = true;
		this->x = x;
		this->z = z;
		this->team = team;
	}
	~curling(){
	}
	//Trivial Interface
	double get_X(){
		return x;
	}

	double get_Z(){
		return z;
	}

	double get_MASS(){
		return mass;
	}

	double get_VX(){
		return vx;
	}

	double get_VZ(){
		return vz;
	}

	bool get_Stop(){
		return stop;
	}

	double get_RADIUS(){
		return radius;
	}
	
	void set_X(double x){
		this->x = x;
	}

	void set_Z(double z){
		this->z = z;
	}

	void set_VX(double vx){
		this->vx = vx;
	}

	void set_VZ(double vz){
		this->vz = vz;
	}

	void set_RADIUS(double r){
		this->radius = r;
	}

	bool get_Team(){
		return team;
	}

	void set_Stop(bool stop){
		this->stop = stop;
	}


private:
	//Location of this curling
	double x;
	double z;

	//Mass of the curling
	double mass;

	//Radius of the curling
	double radius;

	//Speed of the curling
	double vx;
	double vz;

	//Team of the curling
	bool team;

	//If the curling is stopped
	bool stop;
};

#endif

