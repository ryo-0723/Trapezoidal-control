#include <Siv3D.hpp> // OpenSiv3D v0.6.5
#include"Plotter.h"

double max_speed = 2.0;
double start_speed = 0.00;
double end_speed = 0.0;
unsigned long times = 0;
unsigned long oldtimes = 0;
double limit_speed = 0.00;
double acc = 1.00;
double up_times, down_times, max_power_times, all_times;
double L1, L3;
constexpr double PI = 3.141592653589793238462643383279502884L;
double dddis[] = { 1.5,4.5};
double kakudo,dir;
unsigned long millis() {
	return (unsigned long)Time::GetMillisec();
}
double constrain(const double x, const double a, const double b) {
	if (x < a) {
		return a;
	}
	else if (b < x) {
		return b;
	}
	else
		return x;
}
double sq(double r) {
	return r * r;
}
bool Stop_flag, Reset_flag, Timer_state;
void timer_start() {
	if (Timer_state == 0) {
		times = millis();
	}
	Timer_state = 1;
	Stop_flag = 0;
	Reset_flag = 0;
}
void timer_stop() {
	Timer_state;
	if (Stop_flag == 0) {
		times = millis() - times;
	}
	Stop_flag = 1;
}
void timer_reset() {
	Reset_flag = 1;
}
unsigned long read_ms() {
	if (Reset_flag)return 0;
	if (Stop_flag)return times;
	return millis() - times;// ms
}

double dis_cal() {
	double rr=sq(dddis[0]) + sq(dddis[1]);
	double	dis = sqrt(rr);
	//このrを台形制御する
	return dis;
}
double dir_cal() {
	double s_ta = atan2(dddis[1], dddis[0]);//返り値はラジアン
	s_ta = s_ta * (180.00 / PI);//この状態では右が０ラジアン
	if(s_ta>0){
		kakudo = s_ta - 90.0;
	}
	else if (s_ta<0&& s_ta >= -89.99) {
		kakudo = s_ta - 90.00;
	}
	else if(s_ta<=-89.99){
		kakudo = 270.00+s_ta;
	}
	return kakudo;
	//正面を0ラジアンとする
//上から見て反時計回りを＋180,時計回りを-180として考える
}

/*
double dir_cal() {
	double s_ta = atan2(dddis[1], dddis[0]);//返り値はラジアン
	kakudo= s_ta * (180.00 / PI)*cos(s_ta);//この状態では右が０ラジアン
	return kakudo;
	//三角関数からの誤差の影響で目標座標に届かない
}
*/

void time_cal() {
	up_times = (max_speed - start_speed) / acc; //最高速度までの加速にかかる時間 /s
	down_times = (max_speed - end_speed) / acc; //減速にかかる時間 /s

	L1 = (start_speed * up_times) + (acc * sq(up_times)) / 2.00; //加速時における移動距離 /m
	L3 = (end_speed * down_times) + (acc * sq(down_times)) / 2.00; //減速時における移動距離/m

	if (L1 + L3 > abs(dis_cal())) {
		//disの台形ができなくなり、三角形になるときの制御 まだあああ
		limit_speed = (2.00 * acc * abs(dis_cal()) + sq(start_speed) + sq(end_speed)) / 2.00;
		limit_speed = sqrt(limit_speed);
		up_times = (limit_speed - start_speed) / acc;
		down_times = (limit_speed - end_speed) / acc;
		all_times = up_times + down_times;
	}
	else {//台形が作れる場合の制御  いけた
		max_power_times = (abs(dis_cal()) - L1 - L3) / max_speed; //最高速度での移動時間 /s
		all_times = up_times + down_times + max_power_times; //移動にかかる合計時間 / s
	}
}

double target_point(int axis) {
	//  X=Vot+(1/2)*at^2;
	double ut = constrain(read_ms() / 1000.0, 0.00, up_times);
	double mt = constrain(read_ms() / 1000.0 - up_times, 0.00, max_power_times);
	double dt = constrain(read_ms() / 1000.0 - (up_times + max_power_times), 0.00, down_times);
	double up = (acc * sq(ut)) / 2.00 + start_speed * ut;
	double mp = max_speed * mt;
	double dp = ((acc * sq(down_times)) / 2.00) - ((acc * sq(down_times - dt)) / 2.00) + end_speed * dt;
	double target = up + mp + dp;
	if (dis_cal() < 0)
		target *= -1;
	 dir = dir_cal() * (PI / 180.0);
	if (axis==0) {
		target=target* cos(dir);
	}
	else {
		target=-1*target* sin(dir);
	}
	return target;
}

void Main()
{
	int a = 0;
	double tar = 0;
	double mil = 0;
	double get = 0;
	Plotter plotter1, plotter2;
	while (System::Update())
	{
		if (millis() > 3000) {
			a++;
		}
		if (a == 2) {
			timer_reset();
			dis_cal();
			time_cal();
			timer_start();
		}
		Print << target_point(1);
		get = (target_point(1) - tar) / (((double)millis() / 1000.0) - mil);//m/s
		plotter1
			.resize(Scene::Rect())
			.plot(target_point(1))
			.maximum(5)
			.draw(Palette::Aqua)
			.drawGrid();

		plotter2
			.resize(Scene::Rect())
			.plot(target_point(0))
			.maximum(5)
			.draw(Palette::Red)
			.drawGrid();

		tar = target_point(1);
		mil = (double)millis() / 1000.0;
	}
}
