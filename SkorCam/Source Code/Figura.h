#include <opencv2\core\core.hpp>

class Figura
{
public:
	cv::Point Centro;
	int Tipo;

	Figura();
	Figura(int tipo, cv::Point centro);

	void Set(int tipo, cv::Point centro);
	void SetZero();

};