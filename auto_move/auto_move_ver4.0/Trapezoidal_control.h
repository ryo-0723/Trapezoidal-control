#pragma once
/*
* 現状の課題点は次の順路に移行したときに、がたつきが見られるところ
* ↑仕方ない気がしてる
* あとすること
*     円を描く動作を描くこと
* 
* 距離の積算方式はずれが大きいとのことで断念した
* 代わりに毎周期距離を一から計算している
*/
#include <Siv3D.hpp>
#define DEG_TO_RAD  0.01745329251994329576923690768489L;
#define RAD_TO_DEG  57.295779513082320876798154814105L;
#define PI  3.141592653589793238462643383279502884L;

class Trapezoidal {
private:
	double end_s = 0, start_s = 0, max_s = 0, limit_s = 0.00;
	double cal_time[4] = {};
	/*[0]=up , [1]=max , [2]=down , [3]=all*/
	double L1 = 0, L3 = 0;
	double dir = 0, dis = 0;
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

	void calculation(double start_s, double end_s,double dis_[2],double now_p[2]) {
		if (cal_state) 	return;
		//switch文によって何度呼び出されても一度だけ動作するようにするもの
		this->start_s = start_s;
		this->end_s = end_s;
		end_potion[0] = now_p[0];//手動で動かした後に自動に切り替えた時のつじつま合わせ
		end_potion[1] = now_p[1];
		dis_[0] = dis_[0] - now_p[0];
		dis_[1] = dis_[1] - now_p[1];
		dis=sq(dis_[0]) + sq(dis_[1]);
		dis = sqrt(dis);//極座標でのrを求める
		dir = atan2(dis_[1], dis_[0]);//返り値はラジアン
		//座標から角度を求める
		//おそらくatan2に式を入れてしまうと結果がバグる！！！！
		cal_time[0] = (max_s - start_s) * acc_; //最高速度までの加速にかかる時間 /s
		cal_time[2] = (max_s - end_s) * acc_; //減速にかかる時間 /s
		L1 = start_s * cal_time[0] + acc * sq(cal_time[0]) *0.5; //加速時における移動距離 /m
		L3 = end_s * cal_time[2] + acc * sq(cal_time[2]) * 0.5; //減速時における移動距離/m
		if (L1 + L3 > abs(dis)) {
			//disの台形ができなくなり、三角形になるときの制御
			limit_s = (2.00 * acc * abs(dis) + sq(start_s) + sq(end_s)) * 0.50;
			limit_s = sqrt(limit_s);
			cal_time[0] = (limit_s - start_s) * acc_;
			cal_time[1] = 0.0;
			cal_time[2] = (limit_s - end_s) * acc_;
			cal_time[3] = cal_time[0] + cal_time[2];

		}
		else {//台形が作れる場合の制御 
			cal_time[1] = (abs(dis) - L1 - L3) / max_s; //最高速度での移動時間 /s
			cal_time[3] = cal_time[0] + cal_time[1] + cal_time[2] ; //移動にかかる合計時間 / s
		}
		cal_state = true;
	}

	
	void calculation(double start_s, double end_s, double radius,double angle, double dis_[2], double now_p[2]) {
		//関数の多重定義だお
		//角度の遷移を台形制御する
		this->start_s = start_s;
		this->end_s = end_s;
		double cal_max_p_s = max_s / radius;//台形制御のときの最大速度（角速度）






	}

	void tar_point() {
		//  X=Vot+(1/2)*at^2;
		if (cal_state) timer_start();//タイマーを開始する　経路計算が終わった後だけ実行する
		double t= read_ms() * 0.001;


		if (dis < 0)    target_ *= -1;
		target[0] = target_ * cos(dir) + end_potion[0];
		target[1] = target_ * sin(dir) + end_potion[1];
		if (cal_time[3] <= t) {
			end_potion [0] =target[0];//一つの経路を巡行し終えた時の座標の情報を保持
			end_potion[1] = target[1];
			timer_stop();
			timer_reset();
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
		return cal_time[3];
	}
	double up__tmie() {
		return cal_time[0];
	}
};
extern Trapezoidal auto_set;

