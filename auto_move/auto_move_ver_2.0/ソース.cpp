#include"ヘッダー.h"
double point1[] = { 1.4,2.5 };


void dis_cal() {
	dis[0] = point1[0] - 0;//現在地と目標値の距離の計算
	dis[1] = point1[1] - 0;
}
void time_cal() {
	if (dis[0] >= dis[1]) {
		axis = 0;
		re_axis = 1;
	}
	else {
		axis = 1;
		re_axis = 0;
	}
	//距離が長いほうの台形制御の計算
	up_times[axis] = (max_speed[axis] - start_speed[axis]) / acc; //最高速度までの加速にかかる時間 /s
	down_times[axis] = (max_speed[axis] - end_speed[axis]) / acc; //減速にかかる時間 /s
	L1[axis] = (start_speed[axis] * up_times[axis]) + (acc * sq(up_times[axis])) / 2.00; //加速時における移動距離 /m
	L3[axis] = (end_speed[axis] * down_times[axis]) + (acc * sq(down_times[axis])) / 2.00; //減速時における移動距離/m
	if (L1[axis] + L3[axis] > abs(dis[axis])) {
		//disの台形ができなくなり、三角形になるときの制御 たぶんいけてる
		limit_speed[axis] = (2.00 * acc * abs(dis[axis]) + sq(start_speed[axis]) + sq(end_speed[axis])) / 2.00;
		limit_speed[axis] = sqrt(limit_speed[axis]);
		up_times[axis] = (limit_speed[axis] - start_speed[axis]) / acc;
		down_times[axis] = (limit_speed[axis] - end_speed[axis]) / acc;
		all_times = up_times[axis] + down_times[axis];
	}
	else {//台形が作れる場合の制御  いけた
		max_power_times[axis] = (abs(dis[axis]) - L1[axis] - L3[axis]) / max_speed[axis]; //最高速度での移動時間 /s
		all_times = up_times[axis] + down_times[axis] + max_power_times[axis]; //移動にかかる合計時間 / s
	}



	//距離が短いほうの計算
	up_times[re_axis] = (max_speed[re_axis] - start_speed[re_axis]) / acc; //最高速度までの加速にかかる時間(もう片方の軸の時間計算) /s
	down_times[re_axis] = (max_speed[re_axis] - end_speed[re_axis]) / acc; //減速にかかる時間 /s

	if (up_times[re_axis] + down_times[re_axis] > all_times) {//計算してないほうの軸の加速と減速の時間を足して、移動時間が足りない場合の制御
		//いわゆるグラフが三角形になる制御 まだあああああ
		up_times[re_axis] = down_times[re_axis] = all_times / 2.00;
	}
	else { //時間が足りる場合 これも多分ダメ　両方三角形になるときに失敗する
		up_times[re_axis] = up_times[axis];
		down_times[re_axis] = down_times[axis];
		max_power_times[re_axis] = all_times - (up_times[re_axis] + down_times[re_axis]);
	}
	//合計時間が同じになればいい
	//だけど合計時間から最高速度を出してやらないといけない

}

double target_point(int axis) {
	double ut = constrain(read_ms() / 1000.0, 0.00, up_times[axis]);
	double mt = constrain(read_ms() / 1000.0 - up_times[axis], 0.00, max_power_times[axis]);
	double dt = constrain(read_ms() / 1000.0 - (up_times[axis] + max_power_times[axis]), 0.00, down_times[axis]);
	double up = (acc * sq(ut)) / 2.00 + start_speed[axis] * ut;
	double mp = max_speed[axis] * mt;
	double dp = ((acc * sq(down_times[axis])) / 2.00) - ((acc * sq(down_times[axis] - dt)) / 2.00) + end_speed[axis] * dt;
	double target = up + mp + dp;
	if (dis[axis] < 0)
		target *= -1;
	return target;
}




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

