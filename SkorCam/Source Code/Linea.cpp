#include "Linea.h"


Linea::Linea(float p1x, float p1y, float p2x, float p2y) {
	p1Y = p1y;
	p2Y = p2y;
	p1X = p1x;
	p2X = p2x;
}

Linea::Linea() {
	p1Y = 0;
	p2Y = 0;
	p1X = 0;
	p2X = 0;
}

int Linea::calcula_altura(int x){
	return (((p1Y - p2Y) / (p1X - p2X))*(x - p1X)) + p1Y;
}

void Linea::SetZero() {
	p1Y = 0;
	p2Y = 0;
	p1X = 0;
	p2X = 0;
}

void Linea::Set(float p1x, float p1y, float p2x, float p2y) {
	p1Y = p1y;
	p2Y = p2y;
	p1X = p1x;
	p2X = p2x;
}