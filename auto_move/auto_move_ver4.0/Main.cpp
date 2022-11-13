# include <Siv3D.hpp> // OpenSiv3D v0.6.
#include"Trapezoidal_control.h"
#include"Plotter.h"

void Main()
{
	double point1[2] = { 1.5,3.0 };
	double point2[2] = {2.5,6.8};
	double now_point1[2] = {0,0};
	double now_point2[2] = {1.5,3.0};
	int next_point=0;
	int a = 0;
	double tar = 0;
	double mil = 0;
	double get = 0;
	Plotter plotter1, plotter2;
	Trapezoidal auto_set(1.0,2.0);

	auto_set.timer_stop();
	auto_set.timer_reset();
	while (System::Update())
	{
		if (auto_set.millis()>3000) {
			if (auto_set.next_status()) {
				next_point++;
				auto_set.set_next_status();
			}
		}
		
		switch (next_point) {
			
		case 1:
			auto_set.calculation(0.0, 0.0, point1,now_point1);
		
			break;
		case 2:
			auto_set.calculation(0.0, 0.0, point2,now_point2);
			break;//break入れろやああああああ
		}

		auto_set.tar_point();
		//理想ではこの形で自動を回すこと
		Print << auto_set.point_status();
		//Print << auto_set.read_ms()*0.001;
		Print << auto_set.all_time();
		Print << auto_set.end_p();
		Print << next_point;

		get = (auto_set.tar_p_y() - tar) / (((double)auto_set.millis() / 1000.0) - mil);//m/s
		plotter1
			.resize(Scene::Rect())
			.plot(auto_set.tar_p_y())
			.maximum(5)
			.draw(Palette::Aqua)
			.drawGrid();

		plotter2
			.resize(Scene::Rect())
			.plot(auto_set.tar_p_x())
			.maximum(5)
			.draw(Palette::Red)
			.drawGrid();

		tar = auto_set.tar_p_y();
		mil = (double)auto_set.millis() / 1000.0;
	}
}
