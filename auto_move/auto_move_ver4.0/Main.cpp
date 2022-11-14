# include <Siv3D.hpp> // OpenSiv3D v0.6.
#include"Trapezoidal_control.h"
#include"Plotter.h"

void Main()
{
	double point1[2] = { 1.5,4.0 };
	double point2[2] = {4.5,8.0};
	double point3[2] = { 1.0,0.5 };
	double now_point1[2] = {0,0};
	int next_point=0;
	int a = 0;
	double tar = 0;
	double mil = 0;
	double get = 0;
	double tar2 = 0;
	double mil2 = 0;
	double get2 = 0;
	unsigned long loop = 0;
	Plotter plotter1, plotter2, plotter3, plotter4;
	Trapezoidal auto_set(1.5,3.0);
	while (System::Update())
	{
		if (auto_set.millis() > 3000) {
			if (auto_set.next_status()) {
				next_point++;
				auto_set.set_next_status();
			}
		}
		switch (next_point) {
		case 1:
			auto_set.calculation(0.0, 0.0, point1, now_point1);
			break;
		case 2:
			auto_set.calculation(0.0, 0.0, point2, point1);
			break;//break入れろやああああああ
		case 3:
			auto_set.calculation(0.0, 0.0, point3, point2);
			break;
		case 4:
			auto_set.calculation(0.0, 0.0, point1, point3);
			break;
		case 5:
			auto_set.calculation(0.0, 0.0, now_point1, point1);
			break;
		}

		auto_set.tar_point();
		//理想ではこの形で自動を回すこと
		loop=auto_set.millis();
		if (loop % 10) {
			get = (auto_set.tar_p_y() - tar) / (((double)loop * 0.001) - mil);//m/s
			get2 = (auto_set.tar_p_x() - tar2) / (((double)loop * 0.001) - mil);//m/s
			tar = auto_set.tar_p_y();
			tar2 = auto_set.tar_p_x();
			mil = (double)loop / 1000.0;
		}




		ClearPrint();
		Print << mil;
		Print << auto_set.all_time();
		Print << auto_set.end_p_x();
		Print << auto_set.end_p_y();
		Print << next_point;
		Print << tar2;
		Print << tar;

		plotter1
			.resize(Scene::Rect())
			.plot(get)
			.maximum(5)
			.minimum(-5)
			.draw(Palette::Aqua)
			.drawGrid();

		plotter2
			.resize(Scene::Rect())
			.plot(get2)
			.maximum(5)
			.minimum(-5)
			.draw(Palette::Red)
			.drawGrid();

		plotter3
			.resize(Scene::Rect())
			.plot(tar2)//x
			.maximum(10)
			.minimum(-10)
			.draw(Palette::Green)
			.drawGrid();

		plotter4
			.resize(Scene::Rect())
			.plot(tar)//y
			.maximum(10)
			.minimum(-10)
			.draw(Palette::Yellow)
			.drawGrid();
	}
}
