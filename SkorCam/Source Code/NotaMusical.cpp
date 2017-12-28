#include "NotaMusical.h"

NotaMusical::NotaMusical(int tipo, cv::Point centro){
	Tipo = tipo;
	Posicion = centro;
}

NotaMusical::NotaMusical() {
	Tipo = 0;
	Posicion = cv::Point(0, 0);
}

void NotaMusical::SetZero() {
	Tipo = 0;
	Posicion = cv::Point(0, 0);
}

void NotaMusical::Set(int tipo, cv::Point centro) {
	Tipo = tipo;
	Posicion = centro;
}