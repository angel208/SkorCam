#include "Figura.h"


Figura::Figura(int tipo, cv::Point centro){
	Tipo = tipo;
	Centro = centro;
}

Figura::Figura() {
	Tipo = 0;
	Centro = cv::Point(0, 0);
}

void Figura::SetZero() {
	Tipo = 0;
	Centro = cv::Point(0, 0);
}

void Figura::Set(int tipo, cv::Point centro) {
	Tipo = tipo;
	Centro = centro;
}