#pragma once
inline double max_speed[] = { 1.50,1.50 };
inline double start_speed[] = { 0.00,0.0 };
inline double end_speed[] = { 0.0,0.0 };
inline unsigned long times = 0;
inline unsigned long oldtimes = 0;
inline double limit_speed[] = { 0.00,0.00 };
inline double dis[2]  ;//目標座標//m
inline double acc = 1.00;
inline double up_times[2], down_times[2], max_power_times[2], all_times;
inline double L1[2], L3[2];
inline bool Stop_flag, Reset_flag, Timer_state;
inline int axis, re_axis;

void time_cal();
void dis_cal();
double target_point(int axis);

unsigned long millis();
double constrain(const double x, const double a, const double b);
double sq(double r);
void timer_start();
void timer_stop();
void timer_reset();
unsigned long read_ms();
