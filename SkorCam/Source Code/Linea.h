#pragma once
class Linea
{
public:
	float p1Y;
	float p2Y;
	float p1X;
	float p2X;
	Linea();
	Linea(float p1x, float p1y, float p2x, float p2y);
	void Set(float p1x, float p1y, float p2x, float p2y);
	void SetZero();
	int calcula_altura( int x);
};


