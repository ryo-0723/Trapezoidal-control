# include <Siv3D.hpp> // OpenSiv3D v0.6.5
#include"Plotter.h"
#include"ヘッダー.h"



void Main()
{
	int a = 0;
	double tar = 0;
	double mil = 0;
	double get = 0;
	Plotter plotter1, plotter2,plotter3;
	while (System::Update())
	{
		if (millis() > 3000) {
			a++;
		}
		if (a == 2) {
			dis_cal();
			timer_reset();
			time_cal();
			timer_start();
		}

		get = (target_point(0) - tar) / (((double)millis() / 1000.0) - mil);//m/s
		plotter1
			.resize(Scene::Rect())
			.plot(all_times)
			.maximum(5)
			.draw(Palette::Aqua)
			.drawGrid();

		plotter2
			.resize(Scene::Rect())
			.plot(target_point(0))
			.maximum(5)
			.draw()
			.drawGrid();

		plotter3
			.resize(Scene::Rect())
			.plot(target_point(1))
			.maximum(5)
			.draw(Palette::Red)
			.drawGrid();
		
		tar = target_point(0);
		mil = (double)millis() / 1000.0;
	}
}
