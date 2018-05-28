#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <queue>
using namespace std;

#define norm_rand() (rand()/(RAND_MAX + 1.0))

double expntl(double);

#define ARRIVAL_TIME 200.0
#define SERVICE_TIME 100.0
#define SERVERS 1

int main()
{
	// 큐 구현


	// n: 현재 가게에 있는 손님 수
	int n;
	// Ta: 평균 Ta 초 마다 손님이 도착.
	// Ts: 평균 Ts 초 후에 손님이 떠남.
	double Ta = ARRIVAL_TIME, Ts = SERVICE_TIME, simulation_time = 200000.0, next_arrival_time, next_departure_time,
		elapsed_time;

	// arrival_rate: 도착률, λ, 1 시간에 도착할 확률
	// service_rate: 서비스율,μ, 1 시간에 떠날 확률
	double arrival_rate = 1 / ARRIVAL_TIME, service_rate = 1 / SERVICE_TIME;
	// servers: 서버 개수, 좌석의 개수
	int servers = SERVERS;

	// B: 로봇 사용한 시간
	// C: 서비스 종료된 고객 수
	// L: 시스템에 있는 평균 고객 수
	// S: 그래프의 넓이 (시간, 손님수 그래프), 시간으로 나누면 시스템 평균 고객의 수.
	// tb: 로봇 사용 시작한 시간
	// tn: 현재부터 이전 이벤트 간의 시간 (도착, 떠남)
	// U: utilization. 시스템 사용률, 로봇의 사용률
	// W: 고객의 평균 머물러 있는 시간
	// X: throughput. 시간당 총 떠난 고객 수
	double B, C, L, s, tb, tn, U, W, X;

	// Lq, Ls를 구하기 위한 변수.
	double  LqS = 0.0, LsS = 0.0;

	double A = 0;

	n = 0; next_arrival_time = 0.0; next_departure_time = simulation_time; elapsed_time = 0.0;
	B = s = 0.0; C = 0; tn = elapsed_time;


	/* Seed the random-number generator with current time so that the number will be dif ferent every time we run.*/
	//srand((unsigned)time(NULL));

	while (elapsed_time < simulation_time)
	{
		// 손님이 도착.
		if (next_arrival_time < next_departure_time)
		{
			/* event 1 : customer arrival */
			// 경과한 시간 = 도착 시간
			elapsed_time = next_arrival_time;

			// s = 그래프의 넓이 (시간, 손님수 그래프)
			// n = 현재 있는 고객의 수
			// elapsed_time - tn = 현재부터 이전 이벤트 간의 시간
			s += n*(elapsed_time - tn);
			// 서버 수보다 크면 대기.
			if (n > servers)
			{
				LqS += (n - servers) * (elapsed_time - tn);
			}
			if (n >= 1)
			{
				LsS += (n >= servers ? servers : n) * (elapsed_time - tn);
			}

			n++;
			tn = elapsed_time;
			A++;


			// 다음 도착 시간은 경과한 시간 + 랜덤 평균 도착 시간
			next_arrival_time = elapsed_time + expntl(Ta);

			// 현재 손님이 1명이면
			if (n == 1)
			{
				// tb = 로봇 사용 시작한 시간
				tb = elapsed_time;
				// 다음 손님 떠나는 시간은 현재 시간 + 랜덤 평균 떠나는 시간
				next_departure_time = elapsed_time + expntl(Ts);
			}
		}
		else
		{
			/* event 2 : customer departure */

			elapsed_time = next_departure_time;

			s += n * (elapsed_time - tn);
			// 서버 수보다 크면 대기.
			if (n > servers)
			{
				LqS += (n - servers) * (elapsed_time - tn);
			}
			if (n >= 1)
			{
				LsS += (n >= servers ? servers : n) * (elapsed_time - tn);
			}

			n--;
			tn = elapsed_time;
			C++;


			if (n > 0)
			{
				next_departure_time = elapsed_time + expntl(Ts);
			}
			else
			{
				next_departure_time = simulation_time;
				// B = 로봇 사용한 시간 (서비스한 시간)
				B += elapsed_time - tb;
			}
		}
	}

	double Lambda = 1 / Ta;
	double Mew = 1 / Ts;
	X = C / elapsed_time;
	U = B / elapsed_time;
	L = s / elapsed_time;
	W = L / X;
	double service_time_per_customer = B / C;
	double Lq = LqS / elapsed_time;
	double Ls = LsS / elapsed_time;
	double Wq = W - service_time_per_customer;

	printf("Ta = %lf, Lambda = %lf\n", Ta, Lambda);
	printf("Ts = %lf, Mew = %lf\n", Ts, Mew);
	printf("S = %d\n", servers);
	puts("-----------------------------------------------------------------");
	printf("총 도착한 고객 수, A = %.0lf\n", A);
	printf("총 서비스한 고객 수, C = %.0lf\n", C);
	printf("도착률과 처리율, λ(arrival_rate) = %lf, X(throughput) = %lf\n", Lambda, X);
	puts("-----------------------------------------------------------------");
	printf("로봇이 일하는 시간, B = %.2lf\n", B);
	printf("utilization, 로봇의 이용률 U = %lf, %lf\n", U, X * service_time_per_customer);
	printf("손님 한 사람당 서비스한 시간 τ = %lf, %lf\n", service_time_per_customer, U / X);
	puts("-----------------------------------------------------------------");
	printf("mean customer no. in store, 평균 고객 수 L = %lf, %lf, %lf, %lf\n", L, W * C / elapsed_time, X * W, Lq + Ls);
	printf("줄에 서서 기다리는 평균 고객 수, Lq = %lf\n", Lq);
	printf("서비스 하는 평균 고객 수, Ls = %lf\n", Ls);
	printf("가게에 머무는 평균 시간 W = %lf, %lf\n", W, Wq + service_time_per_customer);
	printf("대기하는 시간 Wq = %lf\n", Wq);

	puts("-----------------------------------------------------------------");
}

// 난수 생성 함수.
double expntl(double x)
{
	/* 'expntl' returns a psuedo -random variate from a negative exponen tial distribution with mean x */

	return(-x*log(norm_rand()));
	//return x;
}