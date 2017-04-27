#include "CurlingManager.h"
#include <math.h>
extern Drawer dr;

extern bool Allstop;

CurlingManager::~CurlingManager()
{
	for (int i = 0; i < CurlingSet.size(); i++)
		delete CurlingSet[i];
}

void CurlingManager::addCurling(double x, double z, bool team){
	CurlingSet.push_back(new curling(x, z, team));
}

void CurlingManager::SetInitSpeed(int index, double theta, double force){
	CurlingSet[index]->set_VX(force*cos(theta));
	CurlingSet[index]->set_VZ(force*sin(theta));
	CurlingSet[index]->set_Stop(false);
}

void CurlingManager::show_all_curlings(){
	for (int i = 0; i < CurlingSet.size(); i++)
		dr.Draw_Curling(CurlingSet[i]);
}

void CurlingManager::update(double time, double fraction)
{
	int i, j;
	Allstop = true;
	for (i = 0; i<CurlingSet.size(); i++)
	{
		if (CurlingSet[i]->get_Stop())
			continue;
		Allstop = false;
		curling* tmp = CurlingSet[i];
		double miu_x = fraction * cos(atan2(tmp->get_VZ(), tmp->get_VX()));
		double miu_z = fraction * sin(atan2(tmp->get_VZ(), tmp->get_VX()));
		//The order of following sentences cannot be changed 
		//upddate position
		tmp->set_X(tmp->get_X() + tmp->get_VX() * time - 0.5 * miu_x * time * time);
		tmp->set_Z(tmp->get_Z() + tmp->get_VZ() * time - 0.5 * miu_z * time * time);
		//update velocity
		tmp->set_VX(tmp->get_VX() - miu_x * time);
		tmp->set_VZ(tmp->get_VZ() - miu_z * time);
		//update stop status
		if (abs(tmp->get_VX()) <= 0.001 && abs(tmp->get_VZ()) <= 0.001) {
			tmp->set_Stop(true);
			tmp->set_VX(0);
			tmp->set_VZ(0);
		}
		else tmp->set_Stop(false);
	}

	//collision test
	for (i = 0; i<this->CurlingSet.size(); i++)
	{
		if (CurlingSet[i]->get_Z() - CurlingSet[i]->get_RADIUS() < -22.0){					//collide with the wall on the front
			cout << "collide the wall!" << endl;
			this->collide(CurlingSet[i]);
		}
		if (abs(CurlingSet[i]->get_X()) + CurlingSet[i]->get_RADIUS() > 4.0){					//collide with the side of the pathway
			cout << "collide the side of the pathway!" << endl;
			this->collide(CurlingSet[i]);
		}
		for (j = i + 1; j<this->CurlingSet.size(); j++)								//collide with another curling
		{
			if (this->check_collision(*CurlingSet[i], *CurlingSet[j]))
			{
				this->collide(CurlingSet[i], CurlingSet[j]);
			}
		}
	}
}

//check collision
bool CurlingManager::check_collision(curling c1, curling c2)
{
	//double r1, r2, x1, x2, z1, z2;
	double r1 = c1.get_RADIUS();
	double r2 = c2.get_RADIUS();
	double x1 = c1.get_X();
	double x2 = c2.get_X();
	double z1 = c1.get_Z();
	double z2 = c2.get_Z();
	double distance = (x1 - x2)*(x1 - x2) + (z1 - z2)*(z1 - z2);
	double collide_distance = (r1 + r2)*(r1 + r2);
	if (distance <= collide_distance)
	{
		cout << "collide!" << endl;
		return true;
	}
	else return false;
}

//collision result
/***** Assume they have the same mass *****/
void CurlingManager::collide(curling* c1, curling* c2)
{
	double v1x = c1->get_VX();
	double v1z = c1->get_VZ();
	double v2x = c2->get_VX();
	double v2z = c2->get_VZ();
	double x1 = c1->get_X();
	double x2 = c2->get_X();
	double z1 = c1->get_Z();
	double z2 = c2->get_Z();
	double theta = atan2(z2 - z1, x2 - x1);
	double v1p = v1x * cos(theta) + v1z * sin(theta);
	double v2p = v2x * cos(theta) + v2z * sin(theta);
	double v1v = v1x * sin(theta) - v1z * cos(theta);
	double v2v = v2x * sin(theta) - v2z * cos(theta);
	//update velocity

	v1x = v2p * cos(theta) + v1v * sin(theta);
	v1z = v2p * sin(theta) - v1v * cos(theta);
	v2x = v1p * cos(theta) + v2v * sin(theta);
	v2z = v1p * sin(theta) - v2v * cos(theta);
	c1->set_VX(v1x);
	c1->set_VZ(v1z);
	c2->set_VX(v2x);
	c2->set_VZ(v2z);
	//update stop status
	if (abs(c1->get_VX()) <= 0.001 && abs(c1->get_VZ()) <= 0.001) {
		c1->set_Stop(true);
		c1->set_VX(0);
		c1->set_VZ(0);
	}
	else c1->set_Stop(false);

	if (abs(c2->get_VX()) <= 0.001 && abs(c2->get_VZ()) <= 0.001) {
		c2->set_Stop(true);
		c2->set_VX(0);
		c2->set_VZ(0);
	} 
	else c2->set_Stop(false);
}

void CurlingManager::collide(curling* c1)
{
	if (c1->get_Z() - c1->get_RADIUS() < -22.0){
		c1->set_VZ(-c1->get_VZ()*0.8);
	}
	else c1->set_VX(-c1->get_VX()*0.8);
}

void CurlingManager::Calculate_Scores(){
	double centerx = 0*0.2;
	double centerz = -6.7*0.33;
	double zbound = 9.6*0.33;
	double xbound = 3.7*0.2;
	double distance[8];
	double INF = 10000;

	score0 = 0;
	score1 = 0;
	int count = 0;
	for (int i = 0; i < this->size(); i++){
		double x = CurlingSet[i]->get_X()*0.2;
		double z = (CurlingSet[i]->get_Z() + 6.7)*0.33;
		//cout << x << z << endl;
		if (x<xbound && x>-xbound && z<zbound && z>-zbound){
			distance[i] = sqrt((x - centerx)*(x - centerx) + (z - centerz)*(z - centerz));
			count++;
		}
		else distance[i] = INF;
	}
	//cout << distance[0] << distance[1] << endl;
	double temp;
	int k;
	for (int i = 0; i < count; i++){
		temp = distance[0];
		k = 0;
		for (int j = 0; j < this->size(); j++){
			if (distance[j] < distance[k])
			{
				k = j;
				temp = distance[j];
			}
		}
		distance[k] = INF;
		if (CurlingSet[k]->get_Team())
			score1 += (8 - i);
		else
			score0 += (8 - i);

	}

}


