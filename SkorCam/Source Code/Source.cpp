#define _CRT_SECURE_NO_WARNINGS

#define TRAINING_DATA_CORCHEA  337
#define TRAINING_DATA_NEGRA    295
#define TRAINING_DATA_REDONDA  0
#define TRAINING_DATA TRAINING_DATA_CORCHEA+TRAINING_DATA_NEGRA+TRAINING_DATA_REDONDA

#define HIERARCHY_PARENT 3
#define HIERARCHY_CHILD 2

#define CORCHEA 0
#define NEGRA 1
#define REDONDA 2
#define BLANCA 3


//#include <stdio.h>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "MP3Player.h"
#include "Linea.h"
#include "Figura.h"
#include "NotaMusical.h"


#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\ml.hpp>
#include <opencv2\ml\ml.hpp>


using namespace cv;
using namespace std;

//valores iniciales de HSV para detectar la lineas (color verde)
int H_MINL = 0;
int H_MAXL = 20;
int S_MINL = 50;
int S_MAXL = 150;
int V_MINL = 125;
int V_MAXL = 200;
//valores iniciales de HSV para detectar la figura(color azul)
int H_MINF = 76;
int H_MAXF = 150;
int S_MINF = 60;
int S_MAXF = 255;
int V_MINF = 45;
int V_MAXF = 190;
//valores iniciales de hsv para el play
int H_MINPlay = 22;
int H_MAXPlay = 52;
int S_MINPlay = 93;
int S_MAXPlay = 129;
int V_MINPlay = 115;
int V_MAXPlay = 183;
//valores iniciales de stop
int H_MINStop = 153;
int H_MAXStop = 178;
int S_MINStop = 157;
int S_MAXStop = 255;
int V_MINStop = 73;
int V_MAXStop = 131; 

Linea Puntos_Lineas[100]; // matriz que contiene las lineas obtenidas por houghlines
Linea Puntos_LineasD[30]; //matriz que contiene unicamente las lineas paralelas (que no se intercecten).
Point Circulos[40];  //matriz que contiene los circulos detectados.
int CantLineas = 0, CantCirculos = 0;
Mat input; 

//utiliza el vector lines ( que es el resultado de houghlines() ) para construir una matriz (Puntos_Lineas) que contiene la informacion de todas las lineas detectadas
//esta informacion es P1x, P1y, P2x, P2y y una bandera que se usa para verificar si esa linea se intersecta con otra.
// P1x = 0 , P1y = 1 , P2x = 2 , P2y = 3
//esto se logra usando los valores (rho, theta) que retorna houghlines para crear los dos puntos de la linea
void GuardarPuntosDetectados(std::vector<Vec2f> lines){
	
	//iniciliza en 0 la matriz en la que se se guardaran las lineas detectadas por houghlines
	for (int i = 0; Puntos_Lineas[i].p1X != 0; i++){
		Puntos_Lineas[i].SetZero();
	}

	//calcula los puntos de cada una de las lineas obtenida por houghlines() y los almacena en la matriz Puntos_Lineas
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;

		//punto 1 de la recta actual
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		//punto 2 de la recta actual
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		//almacena la informacion de los puntos en la matriz
		Puntos_Lineas[i].Set( pt1.x , pt1.y, pt2.x, pt2.y);
	}
}

void GuardarCirculosDetectados(std::vector<Vec3f> circles ){
	
	CantCirculos = 0;

	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		Point center(round(circles[current_circle][0]), round(circles[current_circle][1]));
		int radius = round(circles[current_circle][2]);
		//circle(input, center, 3, Scalar(0, 255, 0), -1, 8, 0);//centro del 
		Circulos[CantCirculos++] = center;
		//circle(input, center, radius, Scalar(0, 255, 0), 5);//radio del circulo
		
	}
	CantCirculos;
}

void DibujarCirculos(std::vector<Vec3f> circles){

	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		Point center(round(circles[current_circle][0]), round(circles[current_circle][1]));
		int radius = round(circles[current_circle][2]);
		circle(input, center, 3, Scalar(0, 255, 0), -1, 8, 0);//centro del circulo
		circle(input, center, radius, Scalar(0, 255, 0), 5);//radio del circulo
	}
}

//encuentra la altura de la linea en e punto x dado, usando la ecuacion de punto-pendiente
int calcular_altura(Linea line, int x){
	return (((line.p1Y - line.p2Y) / (line.p1X - line.p2X))*( x - line.p1X)) + line.p1Y;
}

void shellsortLineas(int size){

	//Ordena las lineas de forma ASCENDENTE! respecto a Y

	int gap, i, j;
	   for (gap = size / 2; gap>0; gap /= 2)
	   for (i = gap; i < size; i++)	  
	   for (j = i - gap; j >= 0 && Puntos_LineasD[j + gap].p2Y < Puntos_LineasD[j].p2Y; j -= gap){
			printf("swap\n");
			   Linea t; // Intercambiar
			   t = Puntos_LineasD[j];
			   Puntos_LineasD[j] = Puntos_LineasD[j + gap];
			   Puntos_LineasD[j + gap] = t;
		   }
	   
}

void shellsortCircles(int size){
	int gap, i, j;
	for (gap = size / 2; gap>0; gap /= 2)
	for (i = gap; i < size; i++)
		//for (j = i - gap; j >= 0 && ( calcular_altura(Puntos_LineasD[j + gap], 0) < calcular_altura(Puntos_LineasD[j], 0) ) ; j -= gap){
	for (j = i - gap; j >= 0 && Circulos[j + gap].x < Circulos[j].x; j -= gap){
		printf("swap\n");
		Point t; // Intercambiar
		t = Circulos[j];
		Circulos[j] = Circulos[j + gap];
		Circulos[j + gap] = t;
	}

}

void shellsortFiguras(Figura f[], int size){
	int gap, i, j;

	for (gap = size / 2; gap>0; gap /= 2)
	for (i = gap; i < size; i++)
	for (j = i - gap; j >= 0 && f[j + gap].Centro.x < f[j].Centro.x; j -= gap){
		printf("swap\n");
		Figura t; // Intercambiar
		t = f[j];
		f[j] = f[j + gap];
		f[j + gap] = t;
	}

}

//verifica si dos lineas se intersectan o no.
//retorna verdadero si se intesectan y falso si no lo hacen
//las dos lineas estan representadas por dos puntos cada una L1 = (o1,p1) y L2 = (o1,p2)
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2)
{
	Point2f x = o2 - o1; //Distancia entre las dos lineas

	Point2f d1 = p1 - o1; //VECTOR DIRECTOR 1
	Point2f d2 = p2 - o2; //VECTOR DIRECTOR 2	

	float cross = d1.x*d2.y - d1.y*d2.x;

	if (abs(cross) < 1e-80)
		return false; //se intersectan

	return true; //no se intersectan
}

//Determina cuales de las lineas de la matriz Puntos_Lineas se intersectan entre si
//a aquellas que lo hagan, se les coloca en 1 la bandera de la posicion 4 del vector para denotar su interseccion con una linea anterior
//luego se pasa una (y solo una ) de las muchas lineas que se intersectan entre si a otra matriz Puntos_LineasD.
//Las lineas contenidas en ese vector son las lineas que de verdad se van a dibujar y las cuales se usaran para tocar las notas
void DepurarLineas(){

	int flag[30];
	for (int i = 0; i < 30; i++){
		flag[i] = 0;
	}

	//se inicializa la matriz Puntos_LineasD en 0
	for (int i = 0; Puntos_LineasD[i].p1X != 0; i++){
		Puntos_LineasD[i].SetZero();
	}

	//se buscan las lineas que se intersecten entre si.
	for (int i = 0; Puntos_Lineas[i].p1X != 0; i++){

		if (flag[i] == 0 )
			for (int j = i+1; Puntos_Lineas[j].p1X != 0; j++){

				if ((flag[j] == 0) && intersection(Point(Puntos_Lineas[i].p1X, Puntos_Lineas[i].p1Y),
												   Point(Puntos_Lineas[i].p2X, Puntos_Lineas[i].p2Y),
												   Point(Puntos_Lineas[j].p1X, Puntos_Lineas[j].p1Y),
												   Point(Puntos_Lineas[j].p2X, Puntos_Lineas[j].p2Y)))
				{
					flag[j] = 1; //si se intersectan, la bandera se coloca en 1
					printf("linea depurada\n");
				}

		    }

	}

	//se pasa una (y solo una) de las lineas del conjunto de lineas que se intersectan entre si a la nueva matriz
	int j = 0;
	for (int i = 0; Puntos_Lineas[i].p1X != 0; i++){
		if (flag[i] == 0){
			Puntos_LineasD[i].Set(Puntos_Lineas[i].p1X, Puntos_Lineas[i].p1Y, Puntos_Lineas[i].p2X, Puntos_Lineas[i].p2Y);
			j++;
		}
		CantLineas = j;

	}
    

}

//dibuja todas las lineas contenidas en el vector Puntos_LineasD en la matriz input.
void DibujarLineas(){

	for (int i = 0; Puntos_LineasD[i].p1X != 0; i++){

		 //saca la informacion de la matriz Puntos_LineasD y la convierte en puntos
		 Point pt1, pt2;
		 pt1.x = Puntos_LineasD[i].p1X;
		 pt1.y = Puntos_LineasD[i].p1Y;
		 pt2.x = Puntos_LineasD[i].p2X;
		 pt2.y = Puntos_LineasD[i].p2Y;

		 line(input, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA); //dibuja una linea a partir de los puntos p1 y p2

		 printf("(%f,%f) y (%f,%f) \n", Puntos_LineasD[i].p1X, Puntos_LineasD[i].p1Y, Puntos_LineasD[i].p2X, Puntos_LineasD[i].p2Y);
	}

}

//dibuja todas las lineas contenidas en el vector Puntos_LineasD en la matriz pasada como parametro
void DibujarLineas(Mat &matriz){

	for (int i = 0; Puntos_LineasD[i].p1X != 0; i++){

		//saca la informacion de la matriz Puntos_LineasD y la convierte en puntos
		Point pt1, pt2;
		pt1.x = Puntos_LineasD[i].p1X;
		pt1.y = Puntos_LineasD[i].p1Y;
		pt2.x = Puntos_LineasD[i].p2X;
		pt2.y = Puntos_LineasD[i].p2Y;

		line(matriz, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA); //dibuja una linea a partir de los puntos p1 y p2

		printf("(%f,%f) y (%f,%f) \n", Puntos_LineasD[i].p1X, Puntos_LineasD[i].p1Y, Puntos_LineasD[i].p2X, Puntos_LineasD[i].p2Y);
	}

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!     OPERACIOES MORFOLOGICAS      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void MorfologiaLinea(Mat &thresh){
	//crea el elemento estructural que se usara al momento de erosionar la imagen, un rectangulo 3x3
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//crea el elemento estructural que se usara al momento de dilatar la imagen, un rectangulo 8x8
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(12, 6));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);
	//morphologyEx(thresh, thresh, MORPH_CLOSE, dilateElement);
	//morphologyEx(thresh, thresh, MORPH_OPEN, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void MorfologiaCirculo(Mat &thresh){
	//crea el elemento estructural que se usara al momento de erosionar la imagen, un circulo 3x3
	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
	//crea el elemento estructural que se usara al momento de dilatar la imagen, un circulo 8x8
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	
	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void MorfologiaFigura(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));


	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!    MAIN     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void TocarNotas( NotaMusical notas[] , int CantNotas ){

    sf::Music music;
	
	for (int j = 0; j < CantNotas; j++ ){

		int duracion = 1000;

		switch (notas[j].Tipo){
		case 0: { duracion = 400;   break; }
		case 1: { duracion = 700;   break; }
		case 2: { duracion = 2500;  break; }
		case 3: { duracion = 1250;  break; }
		default:{ duracion = 1000;  break; }
		}

		//============  Eleccion de la nota a tocar  ================
		if (notas[j].Posicion.y < Puntos_LineasD[0].p2Y){

			printf("toco un SOL3 en figura %d\n", notas[j].Tipo);
			std::string path("NotasMusicales/3G(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[1].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[0].p2Y)){
			std::string path("NotasMusicales/3F(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un FA3 la figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[2].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[1].p2Y)){
			std::string path("NotasMusicales/3E(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un MI3  la figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[3].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[2].p2Y)){
			std::string path("NotasMusicales/3D(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }

			printf("toco un RE3 en figura %d\n",  notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[4].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[3].p2Y)){
			std::string path("NotasMusicales/3C(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un DO3 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[5].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[4].p2Y)){
			std::string path("NotasMusicales/2B(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un SI2 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[6].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[5].p2Y)){
			std::string path("NotasMusicales/2A(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un LA2 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[7].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[6].p2Y)){
			std::string path("NotasMusicales/2G(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un SOL2 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[8].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[7].p2Y)){
			std::string path("NotasMusicales/2F(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un FA2 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y < Puntos_LineasD[9].p2Y) && (notas[j].Posicion.y > Puntos_LineasD[8].p2Y)){
			std::string path("NotasMusicales/2E(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un MI2 en figura %d\n", notas[j].Tipo);
		}
		else if ((notas[j].Posicion.y > Puntos_LineasD[10].p2Y) ){
			std::string path("NotasMusicales/2D(");
			path += std::to_string(notas[j].Tipo);
			path += ").wav";
			if (!music.openFromFile(path)){ printf("fallo"); getchar(); }
			printf("toco un RE2 en figura %d\n", notas[j].Tipo);
		}
		//=========================================

		//===========   Reproduccion ==============================

		music.play();
		Sleep(duracion);

		//===============================================

	}

}

void trainSVM()
{
	//arreglos en donde se guardaran las etiquetas y la data de entrenamiento
	int labels[TRAINING_DATA];
	float trainingData[TRAINING_DATA][7];
	int i = 0;

	Mat BinaryInput;//**
	Mat HSV;


	Moments moment;
	double hu[7];

	//Empieza el entrenamiento con las figuras negras
	printf("NEGRAS:\n");

	for (i = 0; i < TRAINING_DATA_NEGRA; i++){

		//se crea la string que contiene la direccion de la imagen actual que servira como entrenamiento
		std::string path("negraT/negra (");
		path += std::to_string(i + 1);
		path += ").png";

		//se lee la imagen
		Mat input = imread(path, CV_LOAD_IMAGE_COLOR);

		//se procesa la imagen de entrenamiento
		cvtColor(input, HSV, COLOR_BGR2HSV);
		inRange(HSV, Scalar(H_MINF, S_MINF, V_MINF), Scalar(H_MAXF, S_MAXF, V_MAXF), BinaryInput);
		MorfologiaFigura(BinaryInput);

		//se obtienen los contornos
		vector<vector<Point> > Contours;
		vector<Vec4i> hierarchy;
		findContours(BinaryInput, Contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
		if ((i + 1) % 50 == 0) printf("\nContour 'negraT/negra (%d)' size: %d\n", i + 1, Contours.size());
		if (Contours.size() > 1) printf("%d tiene %d contornos\n", i + 1, Contours.size());

		//se obtienen los momentos del contorno
		moment = moments(Contours[0]);
		HuMoments(moment, hu);

		//se setean las caracteristicas (features) del contorno
		for (int j = 0; j <= 6; j++){
			trainingData[i][j] = (float)hu[j];
		}

		//se muestra la imagen recien entrenada
		namedWindow("Components2", CV_WINDOW_AUTOSIZE);
		imshow("Components2", BinaryInput);//*
		namedWindow("Components", CV_WINDOW_AUTOSIZE);
		imshow("Components", HSV);//**
		waitKey(30);

		//se coloca la etiqueta de la imagen a NEGRA (1)
		labels[i] = NEGRA;
	}

	//Empieza el entrenamiento con las figuras Corcheas
	printf("Corcheas:\n");

	for (i = 0; i < TRAINING_DATA_CORCHEA; i++){ //**

		//se crea la string que contiene la direccion de la imagen actual que servira como entrenamiento
		std::string path("corcheaT/Corchea ("); //**
		path += std::to_string(i + 1);
		path += ").png";

		//se lee la imagen
		Mat input = imread(path, CV_LOAD_IMAGE_COLOR); //**

		//se trabaja la imagen
		cvtColor(input, HSV, COLOR_BGR2HSV);
		inRange(HSV, Scalar(H_MINF, S_MINF, V_MINF), Scalar(H_MAXF, S_MAXF, V_MAXF), BinaryInput);
		MorfologiaFigura(BinaryInput);

		//se obtienen los contornos
		vector<vector<Point> > Contours;
		vector<Vec4i> hierarchy;
		findContours(BinaryInput, Contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
		if ((i + 1) % 50 == 0) printf("\nContour 'negraT/negra (%d)' size: %d\n", i + 1, Contours.size());
		if (Contours.size() > 1) printf("%d tiene %d contornos\n", i + 1, Contours.size());

		//se setean los momentos del contorno
		moment = moments(Contours[0]);
		HuMoments(moment, hu);

		//se setean las caracteristicas (features) del contorno
		for (int j = 0; j <= 6; j++){
			trainingData[i + TRAINING_DATA_NEGRA][j] = (float)hu[j];
		}

		//se muestra la imagen recien entrenada
		namedWindow("Components2", CV_WINDOW_AUTOSIZE);
		imshow("Components2", BinaryInput);//*
		namedWindow("Components", CV_WINDOW_AUTOSIZE);
		imshow("Components", HSV);//**
		waitKey(30);

		//se coloca la etiqueta de la imagen actual en CORCHEA(0)
		labels[i + TRAINING_DATA_NEGRA] = CORCHEA;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~COMIENZA EL PROCESO DE ENTRENAMIENTO DE LA SVM~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~``
	Mat labelsMat(TRAINING_DATA, 1, CV_32SC1, labels);
	Mat trainingDataMat(TRAINING_DATA, 7, CV_32FC1, trainingData);

	//se crea la SVM
	Ptr<ml::SVM> svm = ml::SVM::create();

	//se setean los parametros de la SVM
	svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::LINEAR);
	CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	svm->setTermCriteria(criteria);


	//entrenar a la SVM
	svm->train(trainingDataMat, ml::ROW_SAMPLE, labelsMat);

	//Se guarda la SVM entrenada
	svm->save("ConocimientoDeSVM.xml");
	printf("svm guardado\n");

	getchar();
}

int main(int argc, char* argv[])
{

	while (1){
		//..............................................variables..................................................
		Mat HSV;

		Mat thresholdL;
		Mat CannyThreshL;
		Mat HSVF;
		Mat thresholdC;
		Mat rectT;
		std::vector<Vec2f> lines;
		std::vector<Vec3f> circles;

		//objeto VideoCapture que se usara para objener la imagen de la camara
		cv::VideoCapture capture;
		//..............................................^variables^..................................................


		//prende la camara
		capture.open(0);
		//dimensiones de la ventana que se usara para mostrar las imagenes.
		capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

		//Empieza el bucle principal en el que se realizaran todas la operaciones.
		//en este loop se obtiene la imagen (frama) actual de la camara, se procesa, y luego se muestra el resultado final. esto se hace varias veces haciendo que 
		//el procesamiento parezca en tiempo real.

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!while!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		int x = 100;
		while (1){
			Mat HSVP, thresholdP;
			Mat thresholdS;
			//lee del objeto capture (camara) y almacena la imagen en input
			capture.read(input);

			cvtColor(input, HSVP, COLOR_BGR2HSV);
			inRange(HSVP, Scalar(H_MINPlay, S_MINPlay, V_MINPlay), Scalar(H_MAXPlay, S_MAXPlay, V_MAXPlay), thresholdP);
			inRange(HSVP, Scalar(H_MINStop, S_MINStop, V_MINStop), Scalar(H_MAXStop, S_MAXStop, V_MAXStop), thresholdS);
			//aplica las morfologias a la imagen (leer acerca de las operaciones morfologicas en procesamiento de imagenes)
			MorfologiaFigura(thresholdP);
			MorfologiaFigura(thresholdS);

			vector<vector<Point> > ContoursP;
			vector<Vec4i> hierarchyP;
			vector<vector<Point> > ContoursS;
			vector<Vec4i> hierarchyS;

			findContours(thresholdP, ContoursP, hierarchyP, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
			drawContours(input, ContoursP, 0, Scalar(255, 0, 0), 2, 8, hierarchyP, 0, Point());
			findContours(thresholdS, ContoursS, hierarchyS, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));

			if (ContoursP.size() > 0) break;
			if (ContoursS.size() > 0) exit(0);
			//radio de actualizacion (la imagen no se muestra sin esto)
			imshow("input", input);
			waitKey(30);
		}


		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Procesamiento de la imagen Seleccionada!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		while (1){
			capture.read(input);
			//Cambia el rango de colores de la matriz input a HSV (hue,saturation,value) y la almacena en la matriz HSV
			cvtColor(input, HSV, COLOR_BGR2HSV);

			//==================================================TRABAJAR CON LA LINEA========================================================
			//aplica el filtro Threshold con los valores hsv minimos y maximos para el color verde y lo almacena en una matris "threshold"
			inRange(HSV, Scalar(H_MINL, S_MINL, V_MINL), Scalar(H_MAXL, S_MAXL, V_MAXL), thresholdL);
			//aplica las morfologias a la imagen (leer acerca de las operaciones morfologicas en procesamiento de imagenes)
			MorfologiaLinea(thresholdL);
			imshow("Detected red circles on the input image13", thresholdL);

			Mat rect = input.clone();
			Mat rectHSV;
			cvtColor(rect, rectHSV, COLOR_BGR2HSV);
			inRange(rectHSV, Scalar(H_MINL, S_MINL, V_MINL), Scalar(H_MAXL, S_MAXL, V_MAXL), rectT);

			vector<vector<Point> > ContoursL;
			vector<Vec4i> hierarchyL;

			RotatedRect br2;
			findContours(thresholdL, ContoursL, hierarchyL, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
			printf("contornos: %d\n\n", ContoursL.size());
			for (int c = 0; c < ContoursL.size(); c++){
				Rect boundRect = boundingRect(ContoursL[c]);
				//br2 = minAreaRect(ContoursL[c]);
				rectangle(rectT, boundRect.tl(), boundRect.br(), Scalar(255, 255, 255), CV_FILLED, 8, 0);
			}


			//aplicar canny edge
			Canny(rectT, CannyThreshL, 100, 50);
			//aplicar houghline. detecta las lineas y mete la informacion en el vector lines

			HoughLines(CannyThreshL, lines, 1, CV_PI / 180, 170);

			//system("cls");
			//transforma la informacion obtenida de houghlines para crear puntos *mas informacion en la definicion de la funcion*
			GuardarPuntosDetectados(lines);
			//Selecciona las lineas que son importantes *mas informacion en la definicion de la funcion*
			DepurarLineas();
			shellsortLineas(CantLineas);
			//Dibuja las lineas seleccionadas *mas informacion en la definicion de la funcion*
			Mat imagenOriginal2 = input.clone();

			//==================================================TRABAJAR CON LOS CIRCULOS========================================================
			Mat imagenOriginal = input.clone();
			//aplica un filtro que reduce el ruido en la imagen
			// convierte el input en HSV
			cvtColor(imagenOriginal, HSVF, COLOR_BGR2HSV);
			//aplicar threshold a la imagen
			inRange(HSVF, Scalar(H_MINF, S_MINF, V_MINF), Scalar(H_MAXF, S_MAXF, V_MAXF), thresholdC);
			//aplicar un filtro gausseano que "lija" el borde del circulo
			GaussianBlur(thresholdC, thresholdC, cv::Size(9, 9), 2, 2);

			//detectar el circulo
			HoughCircles(thresholdC, circles, CV_HOUGH_GRADIENT, 1, thresholdC.rows / 8, 100, 20, 0, 0);

			// dibujar el circulo
			GuardarCirculosDetectados(circles);

			//ordena los circulos respecto a X
			shellsortCircles(CantCirculos);

			printf("circulos: %d\n", CantCirculos);
			for (int mp = 0; mp < CantCirculos; mp++){
				printf(" pto %d: %d\n", mp, Circulos[mp]);
			}

			//============================================= TRABAJAR CON LAS FIGURAS ==========================================================

			//trainSVM();

			Ptr<ml::SVM> svm = ml::SVM::load<cv::ml::SVM>("ConocimientoDeSVM.xml");

			printf("svm Cargado\n");

			Mat FiguraBin;
			Mat HSV3;
			Figura Arreglo_Figuras[50];

			//=========TRATAR IMAGEN DE PRUEBA===============
			cvtColor(imagenOriginal2, HSV3, COLOR_BGR2HSV);
			inRange(HSV3, Scalar(H_MINF, S_MINF, V_MINF), Scalar(H_MAXF, S_MAXF, V_MAXF), FiguraBin);
			MorfologiaFigura(FiguraBin);
			//========================

			//=======CONSEGUIR CONTORNOS======
			vector<vector<Point> > Contours;
			vector<Vec4i> hierarchy;
			findContours(FiguraBin, Contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
			//================

			int ContornosUsables = 0;
			for (int c = 0; c < Contours.size(); c++){

				//si no es un contorno externo, no se toma en cuenta
				if (hierarchy[c][HIERARCHY_PARENT] != -1) continue;

				//se obitienen los momento del contorno actual
				Moments momentP = moments(Contours[c]);

				//se calculan los hu moments del contorno
				double huP[7];
				HuMoments(momentP, huP);

				//se obtionenen las caracteristicas del contorno (features)
				float feat[7];
				for (int j = 0; j <= 6; j++){
					feat[j] = (float)huP[j];
				}

				//se crea la matriz con las caracteristicas que sera introducida al svm
				Mat PruebaMat(1, 7, CV_32FC1, feat);

				//se usa la SVM para clasificar el contorno
				float respuesta = svm->predict(PruebaMat);

				//se calcula el centro de masa del contorno
				Point2f centro = Point2f(momentP.m10 / momentP.m00, momentP.m01 / momentP.m00);

				//ACA SE CLASIFICA EL CONTORNO DEPENDIENDO DE LA PREDICCION DEL SVM Y DEL HECHO DE SI TIENE HIJOS O NO.!!
				if (respuesta == NEGRA && hierarchy[c][HIERARCHY_CHILD] == -1){

					Arreglo_Figuras[ContornosUsables].Tipo = NEGRA;
					Arreglo_Figuras[ContornosUsables].Centro = centro;

					drawContours(input, Contours, c, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());
					circle(input, centro, 4, Scalar(0, 255, 0), -1, 8, 0);
				}
				else if (respuesta == NEGRA && hierarchy[c][HIERARCHY_CHILD] != -1){

					Arreglo_Figuras[ContornosUsables].Tipo = BLANCA;
					Arreglo_Figuras[ContornosUsables].Centro = centro;

					drawContours(input, Contours, c, Scalar(0, 255, 0), 2, 8, hierarchy, 0, Point());
					circle(input, centro, 4, Scalar(0, 255, 0), -1, 8, 0);
				}
				else if (respuesta == CORCHEA){

					Arreglo_Figuras[ContornosUsables].Tipo = CORCHEA;
					Arreglo_Figuras[ContornosUsables].Centro = centro;

					drawContours(input, Contours, c, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point());
					circle(input, centro, 4, Scalar(0, 255, 0), -1, 8, 0);
				}

				ContornosUsables++;
			}

			shellsortFiguras(Arreglo_Figuras, ContornosUsables);




			NotaMusical notas[50];
			int CantNotas = 0;
			for (int nm = 0; nm < ContornosUsables; nm++){
				CantNotas++;
				notas[nm].Set(Arreglo_Figuras[nm].Tipo, Circulos[nm]);
				printf("\n nota1: tipo %d, tono:%d/n", notas[nm].Tipo, notas[nm].Posicion);
			}





			//====================================Dibujar todo======================================
			if (ContornosUsables != CantCirculos) continue;


			DibujarLineas(input);
			DibujarCirculos(circles);

			imshow("Resultado2", thresholdL);
			imshow("Detected red circles on the input image13", CannyThreshL);
			imshow("Detected red circles on the input image123", rectT);
			imshow("Resultado", input);
			waitKey(30);

			TocarNotas(notas, CantNotas);
			
			
			cvDestroyAllWindows();
			
			break;
		}

		//===================================   Mostrar Pantalla ================================
	}
	
	return 0;
} 