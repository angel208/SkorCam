#include <opencv2\core\core.hpp>

class NotaMusical
{
public:
	cv::Point Posicion;
	int Tipo;

	NotaMusical();
	NotaMusical(int tipo, cv::Point pos);

	void Set(int tipo, cv::Point pos);
	void SetZero();

};