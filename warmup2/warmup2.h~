#ifndef _WARMUP2_H_
#define _WARMUP2_H_

#include <stdio.h>
#include "my402list.h"

typedef struct MyArrivalElem{
	unsigned sec;
	unsigned millisec;
	unsigned arrival_time;			//microsec
	float formatted_arrival_time;		//millisec
	float formatted_Q1_arrival_time;
	float formatted_Q1_leaving_time;
	float formatted_Q2_arrival_time;
	float formatted_Q2_leaving_time;
	float formatted_S_leaving_time;

	float inter_arrival_time;
	float time_in_Q2;
	float service_time;

	int packet_service_time;
	float lambda;
	int P;
	int num;
}MyArrival;

typedef struct MyTokenElem{
	long sec;
	int millisec;
	unsigned arrival_time;
	float formatted_arrival_time;
	int num;
}MyToken;


typedef struct MyTraceElem{
	int packet_num;
	int packet_inter_arrival_time;
	int tokens_required;
	int packet_service_time;
}MyTrace;

int MODE;
My402List *Q1,*Q2,*TokenQ,*AllPackets,*TraceList_Arrival,*TraceList_Server;
pthread_cond_t cv;
int Q2_empty;	//guard variable
int arrival_exit;
int DroppedPackets;
struct timeval T_Start;
float LAMBDA;
float MU;
float R;
int P;
int NUM;
int B;
int tokens_in_bucket;
int packets_to_be_served;

int COMPLETED_PACKETS;
int DROPPED_PACKETS;
int CANCELLED_PACKETS;
int DROPPED_TOKENS;
int TOTAL_TOKENS;
double packet_inter_arrival_time;
double packet_service_time;
double packet_in_Q1;
double packet_in_Q2;
double packet_in_system;
double sq_packet_in_system;

int ctrl_c,arrival_ctrl_c,handler_ctrl,arrival_exit;//,server_ctrl_c;
struct sigaction act;
sigset_t new;

void handler_Arrival();
void handler_Token();
void handler_Server();
void handler_Ctrl();
void interrupt(int sig);

void TraverseArrival(My402List *list);
void TraverseToken(My402List *list);
void TraverseTrace(My402List *list);
void checkQ1();
void checkQ1_token();



void InitArrival(MyArrival *MyPtr,float lambda, int P, int i);
void InitToken(MyToken *MyPtr,int i);
void trim(char * string);
#endif /*_WARMUP2_H_*/

