#pragma once

#include <Siv3D.hpp>

#define DEG_TO_RAD  0.01745329251994329576923690768489L;
#define RAD_TO_DEG  57.295779513082320876798154814105L;
#define PI  3.141592653589793238462643383279502884L;

class Trapezoidal {
	double end_s, start_s, max_s;

private:
	unsigned long times = 0;
	double limit_s = 0.00;
	double up_t, down_t, max_power_t, all_t;
	double L1, L3;
	double kakudo, dir, dis;
	bool Stop_flag, Reset_flag, Timer_state;
	double acc_;
	double acc;

public:
	Trapezoidal(double acc, double max_s) {
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

	void calculation(double start_s,double end_s,double dis_[2]) {
		dis = sqrt(sq(dis_[0]) + sq(dis_[1]));

		double s_ta = atan2(dis_[1], dis_[0]);//�Ԃ�l�̓��W�A��
		s_ta = s_ta * RAD_TO_DEG;//���̏�Ԃł͉E���O���W�A��
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
		//���ʂ�0���W�A���Ƃ���
        //�ォ�猩�Ĕ����v�����{180,���v����-180�Ƃ��čl����

		up_t = (max_s - start_s) * acc_; //�ō����x�܂ł̉����ɂ����鎞�� /s
		down_t = (max_s - end_s) * acc_; //�����ɂ����鎞�� /s
		L1 = start_s * up_t + acc * sq(up_t) *0.5; //�������ɂ�����ړ����� /m
		L3 = end_s * down_t + acc * sq(down_t)*0.5; //�������ɂ�����ړ�����/m
		if (L1 + L3 > abs(dis)) {
			//dis�̑�`���ł��Ȃ��Ȃ�A�O�p�`�ɂȂ�Ƃ��̐���
			limit_s = (2.00 * acc * abs(dis) + sq(start_s) + sq(end_s)) *0.50;
			limit_s= sqrt(limit_s);
			up_t= (limit_s - start_s) *acc_;
			down_t= (limit_s - end_s) * acc_;
			all_t = up_t + down_t;
		}
		else {//��`������ꍇ�̐��� 
			max_power_t = (abs(dis) - L1 - L3) / max_s; //�ō����x�ł̈ړ����� /s
			all_t = up_t + down_t + max_power_t; //�ړ��ɂ����鍇�v���� / s
		}
	}

	double tar_point(int axis) {
		//  X=Vot+(1/2)*at^2;
		double t= read_ms() * 0.001;
		double ut = constrain(t, 0.00, up_t);
		double dt = constrain(t- (up_t + max_power_t), 0.00, down_t);

		double target = acc * sq(ut) *0.50 + start_s * ut
			+ max_s * constrain(t - up_t, 0.00, max_power_t)
			+ (acc * sq(down_t) * 0.50 - acc * sq(down_t - dt) * 0.50) + end_s * dt;
		/*
		double up = acc * sq(ut) * 0.50 + start_s * ut;
		double mp = max_s * constrain(t - up_t, 0.00, max_power_t);
		double dp = (acc * sq(down_t) * 0.50 - acc * sq(down_t - dt) * 0.50) + end_s * dt;
		double target = up + mp + dp;
		*/
		if (dis < 0)    target *= -1;
		if (axis == 0) 	target *= cos(dir);
		else            target *= -1 * sin(dir);
		return target;
	}
};

