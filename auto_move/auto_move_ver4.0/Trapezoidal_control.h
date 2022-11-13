#pragma once
/*
* 現状の課題点は次の順路に移行したときに、がたつきが見られるところ
* 円を描く動作を描くこと
* 
*/
#include <Siv3D.hpp>
#define DEG_TO_RAD  0.01745329251994329576923690768489L;
#define RAD_TO_DEG  57.295779513082320876798154814105L;
#define PI  3.141592653589793238462643383279502884L;

class Trapezoidal {
private:
	double end_s = 0, start_s = 0, max_s = 0, limit_s = 0.00;
	double up_t = 0, down_t = 0, max_power_t = 0, all_t = 0;
	double L1 = 0, L3 = 0;
	double kakudo = 0, dir = 0, dis = 0;
	double acc_, acc;
	double target[2] = { 0,0 };
	double end_potion[2] = { 0,0 };
	unsigned long times = 0;
	bool Stop_flag = false, Reset_flag = false, Timer_state = false;
	bool cal_state = false, next_state = true;

public:
	Trapezoidal(double acc, double max_s){
		this->acc=acc;
		this->max_s = max_s;
		acc_ = 1.00 / acc;
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
		Timer_state=0;
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

	void calculation(double s_s, double e_s,double dis_[2],double now_p[2]) {
		if (cal_state) {//switch文で何度呼び出されても一度だけ動作するようにするもの
			return;
		}
		start_s = s_s;
		end_s = e_s;
		dis_[0] = dis_[0] - now_p[0];
		dis_[1] = dis_[1] - now_p[1];
		//おそらくatan2に式を入れてしまうと結果がバグる！！！！！
		dis = sqrt(sq(dis_[0]) + sq(dis_[1]));//極座標でのrを求める

		double s_ta = atan2((dis_[1]), (dis_[0]));//返り値はラジアン
		//座標から角度を求める
		s_ta = s_ta * RAD_TO_DEG;//この状態では右が０ラジアン
		if (s_ta > 0) {
			kakudo = s_ta - 90.0000;
		}
		else if (s_ta < 0 && s_ta >= -89.9999) {
			kakudo = s_ta - 90.0000;
		}
		else if (s_ta <= -89.9999) {
			kakudo = 270.0000 + s_ta;
		}
		dir = kakudo * DEG_TO_RAD;
		//正面を0ラジアンとする
        //上から見て反時計回りを＋180,時計回りを-180として考える

		up_t = (max_s - start_s) * acc_; //最高速度までの加速にかかる時間 /s
		down_t = (max_s - end_s) * acc_; //減速にかかる時間 /s
		L1 = start_s * up_t + acc * sq(up_t) *0.5; //加速時における移動距離 /m
		L3 = end_s * down_t + acc * sq(down_t)*0.5; //減速時における移動距離/m
		if (L1 + L3 > abs(dis)) {
			//disの台形ができなくなり、三角形になるときの制御
			limit_s = (2.00 * acc * abs(dis) + sq(start_s) + sq(end_s)) *0.50;
			limit_s= sqrt(limit_s);
			up_t= (limit_s - start_s) *acc_;
			down_t= (limit_s - end_s) * acc_;
			all_t = up_t + down_t;
		}
		else {//台形が作れる場合の制御 
			max_power_t = (abs(dis) - L1 - L3) / max_s; //最高速度での移動時間 /s
			all_t = up_t + down_t + max_power_t; //移動にかかる合計時間 / s
		}
		cal_state = true;
	}

	void tar_point() {
		//  X=Vot+(1/2)*at^2;
		if (cal_state) {//タイマーを開始する　何度呼び出しても一度だけ実行する
			timer_start();
		}
		double t= read_ms() * 0.001;
		double ut = constrain(t, 0.00, up_t);
		double dt = constrain(t- (up_t + max_power_t), 0.00, down_t);

		double target_ = acc * sq(ut) *0.50 + start_s * ut
			+ max_s * constrain(t - up_t, 0.00, max_power_t)
			+ (acc * sq(down_t) * 0.50 - acc * sq(down_t - dt) * 0.50) + end_s * dt;
		/*
		double up = acc * sq(ut) * 0.50 + start_s * ut;
		double mp = max_s * constrain(t - up_t, 0.00, max_power_t);
		double dp = (acc * sq(down_t) * 0.50 - acc * sq(down_t - dt) * 0.50) + end_s * dt;
		double target = up + mp + dp;
		*/
		if (dis < 0)    target_ = target_ * -1;
			target[0] = target_ * cos(dir)+ end_potion[0];
			target[1] = target_ * -1 * sin(dir)+ end_potion[1];


		if (all_t <= t) {
			end_potion [0] =target[0];//一つの経路を巡行し終えた時の座標の情報を保持
			end_potion[1] = target[1];
			timer_stop();

			cal_state = false;
			next_state = true;
		}
	}
	bool next_status() {//今回の経路を巡行し終えたらtrueになる
		return next_state;
	}
	void set_next_status() {//次の経路をセットしたら必ず行うこと
		next_state = false;
	}
	double end_p_x() {
		return end_potion[0];
	}
	double end_p_y() {
		return end_potion[1];
	}

	double tar_p_x() {
		return target[0];
	}
	double tar_p_y() {
		return target[1];
	}

	double all_time() {
		return all_t;
	}
};
extern Trapezoidal auto_set;

