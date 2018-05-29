#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <queue>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

#define norm_rand() (rand()/(RAND_MAX + 1.0))

double expntl(double);

// 도착 시간. 평균 200 마다 손님이 도착.
#define ARRIVAL_TIME 12.5
// 서비스 시간. 평균 100초 후에 손님이 떠남.
#define SERVICE_TIME 100.0
// 전체 시뮬레이션 시간.
#define SIMULATION_TIME 200000.0
// 좌석의 개수.
#define NUMBER_OF_SEATS 10

int main()
{
	// 큐 구현
	// 가게에 도착 및 떠나는 시간 순서는 먼저 온 순서 이므로 도착 시간을 기준으로 우선순위 큐를 사용.
	priority_queue<double, vector<double>, greater<double> > arrivalQueue;
	priority_queue<double, vector<double>, greater<double> > serviceQueue;

	// n: 현재 가게에 있는 손님 수
	int n;
	// Ta: 평균 Ta 초 마다 손님이 도착.
	// Ts: 평균 Ts 초 후에 손님이 떠남.
	double Ta = ARRIVAL_TIME, Ts = SERVICE_TIME, simulation_time = SIMULATION_TIME, next_arrival_time, next_departure_time,
		elapsed_time;

	// arrival_rate: 도착률, λ, 1초에 도착할 확률
	// service_rate: 서비스율,μ, 1초에 떠날 확률
	double arrival_rate = 1 / ARRIVAL_TIME, service_rate = 1 / SERVICE_TIME;
	// servers: 좌석의 개수
	int numberOfSeats = NUMBER_OF_SEATS;

	// B: 좌석을 사용한 시간
	// C: 서비스 종료된 고객 수
	// L: 가게에 있는 평균 고객 수
	// S: 그래프의 넓이 (시간, 손님수 그래프), 시간으로 나누면 가게 평균 고객의 수.
	// tn: 현재부터 이전 이벤트 간의 시간 (도착, 떠남)
	// U: utilization. 시스템 사용률, 좌석의 사용률
	// W: 고객이 평균 머물러 있는 시간
	// X: throughput. 시간당 총 떠난 고객 수
	double B, C, L, s, tn, U, W, X;

	// Lq (평균 대기 고객 수), Ls (평균 서비스 중인 고객 수)를 구하기 위한 변수.
	double  LqS = 0.0, LsS = 0.0;

	// 도착한 고객 수.
	double A = 0;

	n = 0; next_arrival_time = 0.0; next_departure_time = simulation_time; elapsed_time = 0.0;
	B = s = 0.0; C = 0; tn = elapsed_time;

	/* Seed the random-number generator with current time so that the number will be dif ferent every time we run.*/
	srand((unsigned)time(NULL));

	while (elapsed_time < simulation_time)
	{
		// 손님이 도착.
		if (serviceQueue.empty() || next_arrival_time < serviceQueue.top())
		{
			/* event 1 : customer arrival */
			// 현재 가게에 있는 손님 수는 대기 줄의 손님 수 + 서비스 중인 손님 수.
			n = arrivalQueue.size() + serviceQueue.size();
			
			// 경과한 시간 = 도착 시간
			elapsed_time = next_arrival_time;

			// s, LqS, LsS = 그래프의 넓이 (시간, 손님수 그래프)
			// n = 현재 있는 고객의 수
			// elapsed_time - tn = 현재부터 이전 이벤트 간의 시간
			s += n * (elapsed_time - tn);			
			LqS += arrivalQueue.size() * (elapsed_time - tn);
			LsS += serviceQueue.size() * (elapsed_time - tn);

			// 시간 업데이트.
			tn = elapsed_time;

			// 손님 한 명 도착.
			arrivalQueue.push(elapsed_time);
			A++;

			// 다음 도착 시간은 경과한 시간 + 랜덤 평균 도착 시간
			next_arrival_time = elapsed_time + expntl(Ta);

			// 가게에 빈 좌석이 있으면
			if (serviceQueue.size() < numberOfSeats)
			{
				// 바로 서비스 시작
				serviceQueue.push(elapsed_time + expntl(Ts));
				arrivalQueue.pop();
			}
		}
		// 고객이 서비스 받고 떠남.
		else
		{
			/* event 2 : customer departure */
			// 현재 가게에 있는 손님 수는 대기 줄의 손님 수 + 서비스 중인 손님 수.
			n = arrivalQueue.size() + serviceQueue.size();

			// 경과한 시간 = 고객이 떠나는 시간
			elapsed_time = serviceQueue.top();

			// s, LqS, LsS = 그래프의 넓이 (시간, 손님수 그래프)
			// n = 현재 있는 고객의 수
			// elapsed_time - tn = 현재부터 이전 이벤트 간의 시간
			s += n * (elapsed_time - tn);
			LqS += arrivalQueue.size() * (elapsed_time - tn);
			LsS += (serviceQueue.size()) * (elapsed_time - tn);


			// 시간 업데이트.
			tn = elapsed_time;

			// 한 명 떠남.
			serviceQueue.pop();
			C++;

			// 빈 좌석이 생기면 대기하던 손님 서비스 시작.
			if (arrivalQueue.empty() == false && serviceQueue.size() < numberOfSeats)
			{
				// 바로 서비스 시작
				serviceQueue.push(elapsed_time + expntl(Ts));
				arrivalQueue.pop();
			}
		}
	}

	// 좌석을 사용한 시간은 고객이 서비스된 시간.
	B = LsS;
	// 이론상의 람다와 뮤 값.
	double real_Lambda = 1 / Ta;
	double real_Mew = 1 / Ts;
	// 실제 시뮬레이션의 람다, 뮤(X) 값.
	double Lambda = A / elapsed_time;
	X = C / elapsed_time;
	// 좌석의 이용률은 전체 사용한 시간을 좌석 수로 나눈 값.
	U = B / elapsed_time / numberOfSeats;
	// 가게에 있는 평균 고객 수, 대기 고객 수, 서비스 고객 수는 그래프의 넓이를 시간으로 나눈 값.
	L = s / elapsed_time;
	double Lq = LqS / elapsed_time;
	double Ls = LsS / elapsed_time;
	// 고객이 머무는 시간은 가게의 평균 고객 수를 떠난 시간 당 고객 수로 나눈 값.
	W = L / X;
	// 고객 당 서비스 시간은 좌석의 사용 시간을 떠난 고객 수로 나눈 값.
	double service_time_per_customer = B / C;
	// 고객이 대기하는 시간은 대기하는 평균 고객 수를 시간당 도착 고객 수로 나눈 값.
	double Wq = Lq / Lambda;

	// 출력.
	puts("로봇우동가게 시뮬레이션. By 2013111995 ver2.0");
	puts("-----------------------------------------------------------------");
	printf("Ta = %f, Lambda = %f\n", Ta, real_Lambda);
	printf("Ts = %f, Mew = %f\n", Ts, real_Mew);
	printf("S = %d\n", numberOfSeats);
	printf("elapsed_time = %f\n", elapsed_time);
	puts("-----------------------------------------------------------------");
	printf("총 도착한 고객 수, A = %.0lf\n", A);
	printf("총 서비스한 고객 수, C = %.0lf\n", C);
	printf("도착률과 처리율, λ(arrival_rate) = %f, X(throughput) = %f\n", Lambda, X);
	puts("-----------------------------------------------------------------");
	printf("좌석이 점유되는 시간, B = %f, %f\n", B, LsS);
	printf("utilization, 좌석의 이용률 U = %f, %f\n", U, X * service_time_per_customer / numberOfSeats);
	printf("손님 한 사람당 서비스한 시간 τ = %f, %f\n", service_time_per_customer, U / X * numberOfSeats);
	puts("-----------------------------------------------------------------");
	printf("mean customer no. in store, 평균 고객 수 L = %f, %f, %f, %f\n", L, W * C / elapsed_time, X * W, Lq + Ls);
	printf("줄에 서서 기다리는 평균 고객 수, Lq = %f\n", Lq);
	printf("서비스 하는 평균 고객 수, Ls = %f\n", Ls);
	printf("가게에 머무는 평균 시간 W = %f\n", W);
	printf("대기하는 시간 Wq = %f\n", Wq);
	puts("-----------------------------------------------------------------");
}

// 난수 생성 함수.
double expntl(double x)
{
	/* 'expntl' returns a psuedo -random variate from a negative exponen tial distribution with mean x */
	return(-x*log(norm_rand()));
}