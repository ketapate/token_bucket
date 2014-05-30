/*
	DOUBTS:
1) avg inter-arrival time lesser than req.
2) packet inter-arrival time = sum of inter-arrival time of both completed & dropped pkts/
				(completed+dropped+cancelled pkts)
3) service-time = service-time for completed pkts/
			(completed+dropped+cancelled pkts)
4) avg# pkts in Q1 = time in Q1 of those pkts that move to Q2/
			(completed + dropped pkts)
5) avg# pkts in Q2 = time in Q2 of those pkts that move to S/
			(completed + dropped pkts)
6) avg# pkts in S = time in S/
			(completed + dropped pkts)
7) avg time in system = server-exit - arrival time for completed pkts/
			(emulation time)

*/


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#include "warmup2.h"



pthread_mutex_t Q1_mutex;
pthread_mutex_t Q2_mutex;
pthread_mutex_t token_mutex;

int main(int argc,char *argv[]){
	if(argc>15){
		fprintf(stderr,"USAGE: ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t testfile]\n");
		exit(1);
	}
	if(argc%2==0){
		fprintf(stderr,"USAGE: ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t testfile]\n");
		exit(1);
	}

	LAMBDA = .5;
	P = 3;
	NUM = 20;
	B = 10;
	R = 1.5;
	MU = 0.35;
	
	
	
	char *t,*text="abc.txt";
	FILE *fptr;
	MODE=1;
	int i;

	//check for Deterministic or Trace-Driven mode from the Commandline option
	for(i=1;i<argc;i=i+2){
		if(	strcmp(argv[i],"-t")==0	){
			text = argv[i+1];
			MODE=2;break;
		}
	}
	int flag=0;
	if(MODE==2){
		for(i=1;i<argc;i=i+2){
			flag=0;
			if( strcmp(argv[i],"-lambda")==0 ){flag=1;}
			else if( strcmp(argv[i],"-mu")==0 ){flag=1;}
			else if( strcmp(argv[i],"-P")==0 ){t=argv[i+1];	P=atoi(t);	flag=1;
				if(P<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}
			else if( strcmp(argv[i],"-n")==0 ){t=argv[i+1];	NUM=atoi(t);	flag=1;
				if(NUM<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}
			else if(strcmp(argv[i],"-t")==0 ){flag=1;}
			else if( strcmp(argv[i],"-r")==0 ){t=argv[i+1]; R=atof(t);	flag=1;
				if((1/R)>10){R=0.1;}
			}
			else if( strcmp(argv[i],"-B")==0 ){t=argv[i+1];	B=atoi(t); 	flag=1;
				if(B<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}

			if(flag==0){
				fprintf(stderr,"Error: Invalid commandline option/s\n");
				exit(1);
			}
		}
	}

	
	//Deterministic
	flag=0;
	if(MODE==1){
		for(i=1;i<argc;i=i+2){
			flag=0;
			if( strcmp(argv[i],"-lambda")==0 ){t=argv[i+1];	LAMBDA=atof(t);	flag=1;}
			else if( strcmp(argv[i],"-mu")==0 ){t=argv[i+1];MU=atof(t);	flag=1;}
			else if( strcmp(argv[i],"-r")==0 ){t=argv[i+1]; R=atof(t);	flag=1;
				if((1/R)>10){R=0.1;}
			}
			else if( strcmp(argv[i],"-B")==0 ){t=argv[i+1];	B=atoi(t); 	flag=1;
				if(B<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}
			else if( strcmp(argv[i],"-P")==0 ){t=argv[i+1];	P=atoi(t);	flag=1;
				if(P<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}
			else if( strcmp(argv[i],"-n")==0 ){t=argv[i+1];	NUM=atoi(t);	flag=1;
				if(NUM<0){fprintf(stderr,"Error: B must be a positive integer value\n");exit(1);}
			}
			if(flag==0){
				fprintf(stderr,"Error: Invalid commandline options\n");
				exit(1);
			}
		}
	}

	//Trace-Driven
	else if(MODE==2){
		TraceList_Arrival = (My402List*)malloc(sizeof(My402List));
		TraceList_Server = (My402List*)malloc(sizeof(My402List));		
		MyTrace* TraceElem;

		fptr = fopen(text,"r");
		if(!fptr){fprintf(stderr,"Error: Transaction File entered is not a valid file\n");exit(1);}

		char * line;
		char buf[1042];
		int lineCount =0;

		fgets(buf,1024,fptr);
		line=(char*)&buf;
		trim(line);
		NUM = atoi(line);
		
		char * tmp;
		char Tmp[1024];
		int z,tabCount=0,W1,W2,W3;

		for(z=0;z<NUM;z++){
			tabCount=0;
			char *ret = fgets(buf,1024,fptr);
			if(ret==NULL){
				fprintf(stderr,"Error: Tracefile has missing packet data\n");
				exit(1);
			}
			line=(char*)&buf;
			trim(line);
			int i,j,space=0;


			for(i=0,j=0;i<strlen(buf);i++){
				if(!isspace(line[i])&&(space==1)){
					space=0;			
				}

				if( isspace(line[i])&&(space==0) ){
					space=1;
					int x,y;
					for(x=0,y=j;y<i;y++,x++){Tmp[x]=line[y];}
					Tmp[x]='\0';
					tmp = Tmp;
					trim(tmp);
					j=i+1;

					if(tabCount==0){
						W1 = atoi(tmp);
						//printf("inter-arrival: %d. %s.\n",W1,tmp);
					}
					if(tabCount==1){
						W2 = atoi(tmp);
						//printf("req tokens: %d. %s.\n",W2,tmp);
					}


					tabCount++;	
				}
			}

			if(tabCount==2){
					int x,y;
					for(x=0,y=j;y<i;y++,x++){Tmp[x]=line[y];}
					Tmp[x]='\0';
					tmp = Tmp;
					trim(tmp);

					W3 = atoi(tmp);
					//printf("service time: %d. %s.\n",W3,tmp);
			}
			//printf("%s. tabcount=%d\n",line,tabCount);
	
			TraceElem = (MyTrace*)malloc(sizeof(MyTrace));
			TraceElem->packet_num = lineCount;
			TraceElem->packet_inter_arrival_time = W1;
			TraceElem->tokens_required = W2;
			TraceElem->packet_service_time = W3;
			My402ListAppend(TraceList_Arrival,(void*)TraceElem);
			My402ListAppend(TraceList_Server,(void*)TraceElem);

			lineCount++;
		}
		if(tabCount!=2){fprintf(stderr,"Error: Trace file malformed\n");exit(1);}
		if(lineCount==0){
			fprintf(stderr,"Error: No Data in the Trace file %s\n",text);
			exit(1);
		}

		fclose(fptr);

	}
	//----------------------------------

	printf("Emulation Parameters:\n");
	if(MODE==1){
		printf("\tlambda = %f\n",LAMBDA);		//???????????????????????? precision floating point
		printf("\tmu = %f\n",MU);			//????????????????????????
		printf("\tr = %f\n",R);				//????????????????????????
		printf("\tB = %d\n",B);
		printf("\tP = %d\n",P);
		printf("\tnumber to arrive = %d\n",NUM);
	}
	else{
//		printf("\tlambda = %f\n",LAMBDA);		//???????????????????????? precision floating point
//		printf("\tmu = %f\n",MU);			//????????????????????????
		printf("\tr = %f\n",R);				//????????????????????????
		printf("\tB = %d\n",B);
//		printf("\tP = %d\n",P);
		printf("\tnumber to arrive = %d\n",NUM);

	}
	printf("\n-------------------------------\n");







	if(pthread_mutex_init(&Q1_mutex,NULL)!=0){
		fprintf(stderr,"Error: Mutex for Q1 failed to initialize\n");
		exit(1);
	}

	if(pthread_mutex_init(&Q2_mutex,NULL)!=0){
		fprintf(stderr,"Error: Mutex for Q2 failed to initialize\n");
		exit(1);
	}

	if(pthread_mutex_init(&token_mutex,NULL)!=0){
		fprintf(stderr,"Error: Mutex for Token failed to initialize\n");
		exit(1);
	}


	
	gettimeofday(&T_Start,NULL);

	pthread_t Arrival_Thread;
	pthread_t Token_Arrival_Thread;
	pthread_t Server_Thread;
	pthread_t Ctrl_Thread;


	//Initialize the 1st packets of the structures
	AllPackets = (My402List*)malloc(sizeof(My402List));
	Q1 = (My402List*)malloc(sizeof(My402List));
	Q2 = (My402List*)malloc(sizeof(My402List));
	TokenQ = (My402List*)malloc(sizeof(My402List));
	My402ListInit(Q1);
	My402ListInit(Q2);
	My402ListInit(TokenQ);
	pthread_cond_init(&cv,NULL);
	Q2_empty=1;
	packets_to_be_served=0;
	COMPLETED_PACKETS=0;
	DROPPED_PACKETS=0;
	CANCELLED_PACKETS=0;
	DROPPED_TOKENS=0;
	TOTAL_TOKENS=0;
	packet_inter_arrival_time=0;
	packet_service_time=0;
	packet_in_Q1=0;
	packet_in_Q2=0;
	packet_in_system=0;
	sq_packet_in_system=0;



	ctrl_c=0;
	arrival_ctrl_c=0;
	handler_ctrl=0;
	arrival_exit=0;

	sigemptyset(&new);
	sigaddset(&new,SIGINT);
	pthread_sigmask(SIG_BLOCK,&new,NULL);
	pthread_create(&Ctrl_Thread,NULL,(void*)handler_Ctrl,NULL);

	printf("00000000.000ms: emulation begins\n");
	pthread_create(&Token_Arrival_Thread,NULL,(void*)handler_Token,NULL);
	pthread_create(&Arrival_Thread,NULL,(void*)handler_Arrival,NULL);
	pthread_create(&Server_Thread,NULL,(void*)handler_Server,NULL);





	pthread_join(Arrival_Thread,NULL);
	printf("Arrival_Thread:%u exited!\n",(unsigned)Arrival_Thread);
	arrival_exit=1;

	pthread_join(Token_Arrival_Thread,NULL);
	printf("Token_Arrival_Thread:%u exited!\n",(unsigned)Token_Arrival_Thread);

	pthread_join(Server_Thread,NULL);
	printf("Server_Thread:%u exited!\n",(unsigned)Server_Thread);

	handler_ctrl=1;
	pthread_join(Ctrl_Thread,NULL);

	pthread_mutex_destroy(&Q1_mutex);
	pthread_mutex_destroy(&Q2_mutex);
	pthread_mutex_destroy(&token_mutex);
	pthread_cond_destroy(&cv);


	My402ListUnlinkAll(Q1);
	free(Q1);

	My402ListUnlinkAll(Q2);
	free(Q2);

	My402ListUnlinkAll(TokenQ);
	free(TokenQ);

	My402ListUnlinkAll(AllPackets);
	free(AllPackets);







	struct timeval T_End;
	gettimeofday(&T_End,NULL);
	unsigned start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
	unsigned end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;	
	double emulation_time=(float)(end-start)*0.001;

	printf("Statistics:\n");
	int TOTAL_PACKETS = COMPLETED_PACKETS+DROPPED_PACKETS+CANCELLED_PACKETS;
	printf("	average packet inter-arrival time = %.6g sec\n",(packet_inter_arrival_time/(TOTAL_PACKETS*1000)) );
	printf("	average packet service time = %.6g sec\n",(packet_service_time/(TOTAL_PACKETS*1000)) );

	printf("\n	average number of packets in Q1 = %.6g\n",((double)packet_in_Q1/(double)(emulation_time)) );
	printf("	average number of packets in Q2 = %.6g\n",((double)packet_in_Q2/(double)(emulation_time)) );
	printf("	average number of packets in S = %.6g\n",((double)packet_service_time/(double)(emulation_time)));

	printf("\n	average time a packet spent in system = %.06g sec\n",(double)packet_in_system/((double)TOTAL_PACKETS*1000));
	double mean = packet_in_system/(TOTAL_PACKETS*1000);
	double mean_of_sq = sq_packet_in_system/(TOTAL_PACKETS*1000*1000);
	double sum = sqrt(mean_of_sq-(mean*mean));

	printf("	standard deviation for time spent in system = %.06g sec\n",sum);

	double token_drop_probability = (double)DROPPED_TOKENS/(double)(TOTAL_TOKENS);
	double packet_drop_probability = (double)DROPPED_PACKETS/(double)(TOTAL_PACKETS);
	if(TOTAL_TOKENS==0){printf("\n	token drop probability = NAN\n");}
	else{printf("\n	token drop probability = %.6g\n",token_drop_probability);}
	printf("	packet drop probability = %.6g\n",packet_drop_probability);


//printf("\ncompleted pkts=%d\ndropped pkts=%d\ncancelled pkts=%d\n",COMPLETED_PACKETS,DROPPED_PACKETS,CANCELLED_PACKETS);
//printf("dropped tokens=%d, total tokens=%d\n",DROPPED_TOKENS,TOTAL_TOKENS);


	printf("MAIN thread: exited\n");
	return 0;
}

void handler_Ctrl(){
	while(TRUE){
		if(handler_ctrl==1){break;}
		act.sa_handler = interrupt;
		sigaction(SIGINT,&act,NULL);
		pthread_sigmask(SIG_UNBLOCK,&new,NULL);
	}
}

void interrupt(int sig){
	ctrl_c=1;
	handler_ctrl=1;
}

void handler_Arrival(){
	struct timeval T_End;
	gettimeofday(&T_End,NULL);
	MyArrival *arrival;

	int i;	
	unsigned prev_time,cur_time,diff_time,start,end;
	float float_diff_time,sleep_time=1000000/LAMBDA;
//printf("\nsleep time=%f\n",sleep_time);



	for(i=0;i<NUM;i++){
		//read from the structure MyTrace from the list TraceList;		
		if(MODE==1){
			if(i==0){usleep(sleep_time);}
		}
		if(MODE==2){
			My402ListElem *Elem;
			MyTrace *TraceElem;
			
			Elem = My402ListFirst(TraceList_Arrival);
			TraceElem = (MyTrace*)Elem->obj;
//printf("****%d, %d, %d, %d\n",TraceElem->packet_num,TraceElem->packet_inter_arrival_time,TraceElem->tokens_required,TraceElem->packet_service_time);
			My402ListUnlink(TraceList_Arrival,Elem);

			P = TraceElem->tokens_required;
			sleep_time = 1000*(double)(TraceElem->packet_inter_arrival_time);
			if(i==0){usleep(sleep_time);}
		}
		



		arrival = (MyArrival*)malloc(sizeof(MyArrival));

		//drop packets which need more tokens than the bucket can hold
		if(P>B){
//			printf("packet p%d is dropped\n",i+1);
//****************************VAR			
			DROPPED_PACKETS++;
//****************************VAR

			pthread_mutex_lock(&Q2_mutex);
				ctrl_c=1;
				Q2_empty=0;
				pthread_cond_broadcast(&cv);								
			pthread_mutex_unlock(&Q2_mutex);
		
			packet_inter_arrival_time+=sleep_time;
			if(MODE==1){usleep(sleep_time);}
			else if(MODE==2){
				My402ListElem *Next = My402ListFirst(TraceList_Arrival);
				if(Next!=NULL){
					MyTrace *TraceElem = (MyTrace*)Next->obj;
					sleep_time = 1000*TraceElem->packet_inter_arrival_time;
				}
				else{break;}

//		printf("%012.03fms: p%d arrives, needs %d tokens, inter-arrival time:%.03f\n",arrival->formatted_arrival_time,arrival->num,arrival->P,arrival->inter_arrival_time);

				usleep(sleep_time);
			}
			continue;
		}


		InitArrival(arrival,LAMBDA,P,i+1);

		//Implementing Q1
		if(i!=0){
			My402ListElem *Prev = My402ListFirst(Q1);
			if(Prev!=NULL){
				MyArrival *PrevPacket = (MyArrival*)Prev->obj;
				prev_time = PrevPacket->arrival_time;
			}

			cur_time = arrival->arrival_time;
			diff_time = cur_time-prev_time;
			float_diff_time=diff_time*0.001;

		}
		else{
			diff_time = arrival->arrival_time;
			float_diff_time=diff_time*0.001;
		}
		prev_time=arrival->arrival_time;
		arrival->inter_arrival_time=float_diff_time;

		packet_inter_arrival_time+=float_diff_time;
	
		printf("%012.03fms: p%d arrives, needs %d tokens, inter-arrival time:%.03f\n",arrival->formatted_arrival_time,arrival->num,arrival->P,arrival->inter_arrival_time);


		
		gettimeofday(&T_End,NULL);
		start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
		end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
		arrival->formatted_Q1_arrival_time=(float)(end-start)*0.001;

		printf("%012.03fms: p%d enters Q1\n",arrival->formatted_Q1_arrival_time,arrival->num);

		//mutex for Q1
		pthread_mutex_lock(&Q1_mutex);
			packets_to_be_served++;
			My402ListPrepend(Q1,(void*)arrival);
			checkQ1();
		pthread_mutex_unlock(&Q1_mutex);

		if(MODE==2){
			My402ListElem *Next = My402ListFirst(TraceList_Arrival);
			if(Next!=NULL){
				MyTrace *TraceElem = (MyTrace*)Next->obj;
				sleep_time = 1000*TraceElem->packet_inter_arrival_time;
			}
			else{break;}

			usleep(sleep_time);
		}
		else if(MODE==1){
			usleep(sleep_time-float_diff_time);		
		}

		//handle CTRL-C
		if(arrival_ctrl_c==1){
			break;
		}		
	}
	return;
}



void handler_Token(){
	MyToken *token;
	
	int i=0;
	unsigned prev_time=0,cur_time=0,diff_time=0;
	float float_diff_time;
	float sleep_time=1000000/R;

	while(TRUE){
		//handle CTRL-C
		if(ctrl_c==1){
			break;
		}
		TOTAL_TOKENS++;
		pthread_mutex_lock(&token_mutex);
		if(TokenQ->num_members==B){
			DROPPED_TOKENS++;
			pthread_mutex_unlock(&token_mutex);
			usleep(sleep_time);
			continue;			
		}
		else{
			pthread_mutex_unlock(&token_mutex);
			token = (MyToken*)malloc(sizeof(MyToken));
			InitToken(token,i+1);

			if(My402ListEmpty(TokenQ)==0){
				My402ListElem *Prev = My402ListFirst(TokenQ);
				if(Prev!=NULL){
					MyToken *PrevToken = (MyToken*)Prev->obj;
					prev_time = PrevToken->arrival_time;
				}
				cur_time = token->arrival_time;
				diff_time = cur_time-prev_time;
				float_diff_time=diff_time*0.001;
			}
			else{
				diff_time = token->arrival_time;
				float_diff_time=diff_time*0.001;
			}

			pthread_mutex_lock(&token_mutex);
				if(tokens_in_bucket<B){
					My402ListPrepend(TokenQ,(void*)token);
					prev_time=cur_time;
					tokens_in_bucket++;
					printf("%012.03fms: token t%d arrives, token bucket now has %d tokens\n",token->formatted_arrival_time,token->num,tokens_in_bucket);
				}
			pthread_mutex_unlock(&token_mutex);



			usleep(sleep_time-float_diff_time);

			pthread_mutex_lock(&Q1_mutex);
				checkQ1();
				if((arrival_exit==1)&&(My402ListEmpty(Q1)==1)){pthread_mutex_unlock(&Q1_mutex);break;}
			pthread_mutex_unlock(&Q1_mutex);
			i++;
		}
	}
	return;
}

void handler_Server(){
	float sleep_time = 1000000/MU;
	struct timeval T_End;
	unsigned start,end;
	float service_time,time_in_system;


	while(TRUE){
		if(ctrl_c==1){break;}
		else if(ctrl_c==0){

			pthread_mutex_lock(&Q2_mutex);
			while(Q2_empty){
				pthread_cond_wait(&cv,&Q2_mutex);
			}
			if(ctrl_c==1){
//*******************************VAR
				if(CANCELLED_PACKETS==0){
					CANCELLED_PACKETS = Q1->num_members + Q2->num_members;
				}
//*******************************VAR
				pthread_mutex_unlock(&Q2_mutex);
				break;
			}

//*******************************VAR
			COMPLETED_PACKETS++;
//*******************************VAR

			My402ListElem *elem = My402ListLast(Q2);
			MyArrival *head = (MyArrival*)elem->obj;

			gettimeofday(&T_End,NULL);
			start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
			end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
			head->formatted_Q2_leaving_time=(float)(end-start)*0.001;

		
			My402ListUnlink(Q2,elem);

			if(My402ListEmpty(Q2)==1){
				Q2_empty=1;
			}
			float diff=head->formatted_Q2_leaving_time - head->formatted_Q2_arrival_time;
			printf("%012.03fms: p%d begins service at S, time in Q2 = %.03fms\n",head->formatted_Q2_leaving_time,head->num,diff);
	

			packet_in_Q2+=diff;

			pthread_mutex_unlock(&Q2_mutex);


			if(MODE==2){
				My402ListElem *Elem;
				MyTrace *TraceElem;
			
				Elem = My402ListFirst(TraceList_Server);
				TraceElem = (MyTrace*)Elem->obj;
				My402ListUnlink(TraceList_Server,Elem);

				sleep_time = 1000*(double)(TraceElem->packet_service_time);
			}




			usleep(sleep_time);

			gettimeofday(&T_End,NULL);
			start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
			end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
			head->formatted_S_leaving_time=(float)(end-start)*0.001;
			service_time = head->formatted_S_leaving_time - head->formatted_Q2_leaving_time;

			time_in_system = (head->formatted_Q1_leaving_time - head->formatted_Q1_arrival_time) + 
					(head->formatted_Q2_arrival_time - head->formatted_Q1_leaving_time) +
					(head->formatted_Q2_leaving_time - head->formatted_Q2_arrival_time) +
					(head->formatted_S_leaving_time - head->formatted_Q2_leaving_time);

			printf("%012.03fms: p%d departs from S, service time = %.03fms, time in system = %.03fms\n",head->formatted_S_leaving_time,head->num,service_time,time_in_system);

			packet_service_time+=service_time;
			packet_in_system+=(head->formatted_S_leaving_time - head->formatted_arrival_time);
			sq_packet_in_system+=pow(head->formatted_S_leaving_time - head->formatted_arrival_time,2);


			pthread_mutex_lock(&Q1_mutex);
				if(ctrl_c==1){packets_to_be_served=0;}
				if(packets_to_be_served!=0){packets_to_be_served--;}
				if(packets_to_be_served==0){
//******************************VAR
					if(CANCELLED_PACKETS==0){
						CANCELLED_PACKETS = Q1->num_members + Q2->num_members;
					}
//******************************VAR
					pthread_mutex_unlock(&Q1_mutex);
					break;
				}
			pthread_mutex_unlock(&Q1_mutex);		
		}
	}

	return;
}

void InitArrival(MyArrival *MyPtr, float lambda, int P, int i){
	MyPtr->num=i;
	MyPtr->lambda = lambda;
	MyPtr->P = P;

	struct timeval T_End;
	gettimeofday(&T_End,NULL);
	MyPtr->sec = (unsigned)T_End.tv_sec-(unsigned)T_Start.tv_sec;
	MyPtr->millisec = abs((unsigned)T_End.tv_usec-(unsigned)T_Start.tv_usec);

	unsigned start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
	unsigned end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
	MyPtr->arrival_time = end - start;
	MyPtr->formatted_arrival_time = MyPtr->arrival_time*0.001;

}


void InitToken(MyToken *MyPtr,int i){
	MyPtr->num=i;

	struct timeval T_End;
	gettimeofday(&T_End,NULL);
	MyPtr->sec = (unsigned)T_End.tv_sec-(unsigned)T_Start.tv_sec;
	MyPtr->millisec = (unsigned)T_End.tv_usec-(unsigned)T_Start.tv_usec;

	unsigned start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
	unsigned end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
	MyPtr->arrival_time = end - start;
	MyPtr->formatted_arrival_time = MyPtr->arrival_time*0.001;

/*	pthread_mutex_lock(&token_mutex);
		tokens_in_bucket++;
	pthread_mutex_unlock(&token_mutex);
*/
}



void TraverseArrival(My402List *list){
        My402ListElem *elem=NULL;
	if(My402ListEmpty(list)==1){
		printf("List is Empty\n");
		return;
	}
	printf("\n----------\n");
	for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) {
		MyArrival* foo=(MyArrival*)(elem->obj);
		printf("****%d.\n",foo->num);
	}
}

void TraverseToken(My402List *list){
        My402ListElem *elem=NULL;
	if(My402ListEmpty(list)==1){
		printf("List is Empty\n");
		return;
	}
	printf("\n----------\n");
	for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) {
		MyToken* foo=(MyToken*)(elem->obj);
		printf("****%d.\n",foo->num);
	}
}

void TraverseTrace(My402List *list){
        My402ListElem *elem=NULL;
	if(My402ListEmpty(list)==1){
		printf("List is Empty\n");
		return;
	}
	printf("\n----------\n");
	for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) {
		MyTrace* foo=(MyTrace*)(elem->obj);
		printf("****%d, %d, %d, %d\n",foo->packet_num,foo->packet_inter_arrival_time,foo->tokens_required,foo->packet_service_time);
	}
}


void checkQ1(){
	//handle CTRL-C
	if(ctrl_c==1){
		arrival_ctrl_c=1;
//		server_ctrl_c=1;

		packets_to_be_served=0;

//******************************VAR
		if(CANCELLED_PACKETS==0){
			CANCELLED_PACKETS = Q1->num_members + Q2->num_members;
		}
//******************************VAR

		My402ListUnlinkAll(Q1);

		pthread_mutex_unlock(&Q2_mutex);
			My402ListUnlinkAll(Q2);
			Q2_empty=0;
			pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&Q2_mutex);
	}


	else if( My402ListEmpty(Q1)==0 ){
		My402ListElem *A = My402ListLast(Q1);
		
		if(A!=NULL){
			MyArrival *ElemQ1 = (MyArrival*)A->obj;
			My402ListElem *T;

			pthread_mutex_lock(&token_mutex);
			if(ElemQ1->P <= tokens_in_bucket){
				tokens_in_bucket -= ElemQ1->P;

				struct timeval T_End;
				unsigned start,end;
				gettimeofday(&T_End,NULL);
				start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
				end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
				ElemQ1->formatted_Q1_leaving_time=(float)(end-start)*0.001;

				float diff = ElemQ1->formatted_Q1_leaving_time - ElemQ1->formatted_Q1_arrival_time;
printf("%012.03fms: p%d leaves Q1, time in Q1 = %.03fms, token bucket now has %d tokens\n",ElemQ1->formatted_Q1_leaving_time,ElemQ1->num,diff,tokens_in_bucket);


				packet_in_Q1+=diff;

				My402ListUnlink(Q1,A);

				int i;
				for(i=0;i<ElemQ1->P;i++){
					T=My402ListLast(TokenQ);
					if(T==NULL){break;}
					My402ListUnlink(TokenQ,T);
				}

				gettimeofday(&T_End,NULL);
				start = (unsigned)T_Start.tv_sec*1000000 + (unsigned)T_Start.tv_usec;
				end = (unsigned)T_End.tv_sec*1000000 + (unsigned)T_End.tv_usec;
				ElemQ1->formatted_Q2_arrival_time=(float)(end-start)*0.001;
printf("%012.03fms: p%d enters Q2\n",ElemQ1->formatted_Q2_arrival_time,ElemQ1->num);



				pthread_mutex_lock(&Q2_mutex);
					Q2_empty=0;
					pthread_cond_broadcast(&cv);								
					My402ListPrepend(Q2,ElemQ1);
				pthread_mutex_unlock(&Q2_mutex);

			}
			pthread_mutex_unlock(&token_mutex);
		}
	}
}


void trim(char * string){
	char * startSpace = string;
	while( (*startSpace != '\0')&&(isspace(*startSpace)) ){
		startSpace++;		
	}
	
	//copy the string without the leading spaces into 'string'
	size_t len = (size_t)(strlen(startSpace));
	memmove(string, startSpace, len);

	char * endSpace = string + (len - 1);
	while( (string<endSpace)&&(isspace(*endSpace)) ){
		endSpace--;
	}
	*(endSpace+1) = '\0';
}



