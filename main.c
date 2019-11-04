/*
   Unix System Programming (Spring, 2019)
   -Simple Web Server Team Project-
	
    201414771 김서진
	201511033 최세진 
 */
//header include
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <netinet/tcp.h>

#define MAX_THREAD 1000

char wcwd[BUFSIZ];	//현재 서버 경로 저장

pthread_mutex_t m_lock;

void *t_func(void *data)
{
	int i,j;	//임시변수
	char *catch = (char *) data;	//data 값 배열로 캐스팅
	char *ctns = strtok(catch, " ");	//tns 값을 저장. 띄어쓰기가 구분자 역할.
	int tns = atoi(ctns);		//정수형으로 변환
	ctns=strtok(NULL, " ");		//구분자 이후의 값(클라이언트 서버 ip 주소)을 ctns에 저장

	char buf[256]={0};	//클라이언트에서 보내온 내용
	char buf2[256]={0};	//클라이언트에서 요청한 파일명 or 로그에 기록할 내용
	char buffer[300000]={0};	//파일 내용을 저장할 버퍼
	char vuf[1000]={0};		//HTML 헤더를 저장할 버퍼
	char log[BUFSIZ]={0};		//log 전용 버퍼
	char fr[10];	//from 숫자 저장
	char bk[10];	//to 숫자 저장
	char pwd[BUFSIZ]={0};	//log 기록 경로
	int a,b,c;		//total.cgi 수식 계산에 필요한 변수
	int size=0;		//파일 size 저장
	FILE* fp;		//불러올 파일 포인터
	FILE* lf;		//log 전용 파일 포인터

	
	if (recv(tns,buf,sizeof(buf) + 1,0) ==-1){	//클라이언트로부터 데이터 받기
		perror("recv");
		return 0;
	}
	
	if(strlen(buf)<3){			//클라이언트로부터 길이 3 이하의 내용을 받을 경우 
		goto INDEX_LOAD;		//index.html 전송
	}
	
	for(i=0;i<strlen(buf);i++){		//클라이언트가 보내온 정보에서 요청한 파일 분리 과정
		if(buf[i]=='\n'){		//GET부분만 추출하기 위해 GET까지 i값 증가
			break;
		}
	}
	strncpy(buf2,buf+5,i-15);		//GET /요청파일명 HTTP/1.1 에서 요청파일명 부분만 buf2에 copy
	if(buf[5]==' '){			//공백을 전송받을 경우 buf2를 비워준다
		strcpy(buf2,"");
	}		
	if(strstr(buf,"total.cgi")!=NULL){	//total.cgi를 요청받을 경우
		for(i=0;i<(int)strlen(buf2);i++){	//숫자 부분 추출
			if(buf2[i]=='='){		//처음 나온 등호 부호 기준으로
				for(j=1;j<strlen(buf2)-i;j++)	
					if(buf2[i+j]=='&')	//'&' 문자가 나올 때까지 위치 추출
						break;
				strncpy(fr,buf2+i+1,j);		//from에 입력된 수를 fr에 copy
				strncpy(bk,buf2+i+1+j+3,strlen(buf2)-(i+j+3));	//to에 입력된 수를 bk에 copy 
				break;
			}
		}
		a= atoi(fr);	//fr 정수형 변환
		b= atoi(bk);	//bk 정수형 변환
		if(a>b){	//from 수가 to 수보다 클 경우 예외처리
			strcpy(vuf,"HTTP/1.1 200 OK\r\nContent-Length: 32\r\nContent-Type: text/html\r\n\r\n<h1>WrongNumber(front>back)</h1>");
		}
		else{	//정상적인 경우
			c=0;
			for(i=a;i<=b;i++){	//누적 합 계산 후 c에 담기
				c=c+i;
				}	
			sprintf(buffer,"<h1>%d from_to %d = %d</h1>",a,b,c);	//출력할 결과를 buffer에 담기
			sprintf(buf2,"%s", buffer);	//로그 작성을 위해 buf2에도 기록
			size = strlen(buffer);		//전송을 위해 size 측정
			sprintf(vuf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",size);
		}
		goto SEND_CLIENT;	//send 부분으로 이동
	}
	else// tgi가 아닌 나머지 경우
	{
		if(strcmp(buf2,"")==0 || strcmp(buf2," ")==0 || strcmp(buf2,"favicon.ico")==0 ){	//요청한 파일이 없거나 favicon인 경우
			if((fp=fopen("index.html","r"))==NULL){
				strcpy(vuf,"HTTP/1.1 200 OK\r\nContent-Length: 9\r\nContent-Type: text/html\r\n\r\nNot Found");
				strcpy(buf2, "Not Found");
				goto SEND_CLIENT;
			}
			strcpy(buf2, "index.html");	//log 작성용 buf2 생성
		}
		else{
			if((fp =fopen(buf2,"rb"))==NULL){	//요청한 파일 open 실패 시 하단 수행
				if((fp=fopen("index.html","r"))==NULL){		//요청한 파일이 없고 index.html조차 없으면 Not Found 전송
					strcpy(vuf,"HTTP/1.1 200 OK\r\nContent-Length: 9\r\nContent-Type: text/html\r\n\r\nNot Found");
					strcpy(buf2, "Not Found");
					goto SEND_CLIENT;
				}
			}
		}
	
		fseek(fp,0,SEEK_END);		//파일 끝 부분 포인터 이동
		size= ftell(fp);		//파일 사이즈 측정
		fseek(fp,0,SEEK_SET);		//파일 첫 부분 포인터 이동
		fread((void*)buffer,size,1,fp);	//파일을 파일 크기만큼 읽어 buffer에 저장
		fclose(fp);			//파일 닫기
		
		if(buf2[strlen(buf2)-1]=='g'){		//jpeg 파일인 경우 html 헤더를 image/jpeg로 설정
			sprintf(vuf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: image/jpeg\r\n\r\n",size);
		}
		else if(buf2[strlen(buf2)-1]=='f'){	//gif 파일인 경우 html 헤더를 image/gif로 설정
			sprintf(vuf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: image/gif\r\n\r\n",size);
		}
		else if(buf2[strlen(buf2)-1]=='l' || buf2[strlen(buf2)-1]=='m' ){	//html, htm 파일인 경우
			sprintf(vuf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",size);
		}
		else{
INDEX_LOAD:
			if((fp=fopen("index.html","r"))==NULL){
				strcpy(vuf,"HTTP/1.1 200 OK\r\nContent-Length: 9\r\nContent-Type: text/html\r\n\r\nNot Found");
				strcpy(buf2, "Not Found");	//로드에 not found 기록용 buf2 생성
				goto SEND_CLIENT;
			}
			strcpy(buf2, "index.html");
			fseek(fp,0,SEEK_END);	//index.html 크기 측정
			size= ftell(fp);
			fseek(fp,0,SEEK_SET);	
			fread((void*)buffer,size,1,fp);	//index.html 읽어오기
			fclose(fp);	//파일 닫기
			sprintf(vuf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",size);
		}
SEND_CLIENT:
		if (send(tns,vuf,strlen(vuf),0)==-1){	//html 헤더 전송
			perror("send");
			close(tns);
			free(data);
			return 0;
		}
		if (send(tns,buffer,sizeof(buffer),0)==-1){	//파일 내용 전송
			perror("send");
			close(tns);
			free(data);
			return 0;
		}

		
	}

	sprintf(log, "%s\t%s\t%ld\n", ctns, buf2, (size + strlen(vuf)));	
	//log 문자열에 클라이언트IP, 전송한파일명, 전송한 크기 순으로 저장

	pthread_mutex_lock (&m_lock);	//log 섞임 방지 mutex 시작
	
	sprintf(pwd, "%s/log.txt", wcwd);	//log 파일 경로 
	lf = fopen(pwd, "a");	//log 파일 열기. a 모드로 열어서 덧붙임 가능
	fprintf(lf, "%s", log);	//log 쓰기
	fclose(lf);		//log 닫기

	pthread_mutex_unlock (&m_lock);	//log 섞임 방지 mutex 종료

	close(tns);	//소켓 닫기
	free(data);	//동적할당 free
	return 0;
}

int main(int argc, char* argv[]){
	
	if(argc!=3){	//매개변수가 3개가 아닐 경우
		printf("argument error\n");
		exit(2);	//에러 후 종료
	}
	char *cwd=getcwd(NULL,BUFSIZ);	//현재 경로를 받아옴
	strcpy(wcwd, cwd);	//전역변수에 현재 경로 저장. 스레드가 참조 가능.
	
	remove("log.txt");	//기존 log가 있을 경우 삭제
	chdir(argv[1]);		//매개변수로 지정해 준 곳으로 이동
	int PORTNUM=(atoi(argv[2]));	//포트번호 정수화

	pthread_t tid[MAX_THREAD];	//MAX_THREAD만큼 스레드 생성
	struct sockaddr_in sin, cli;		//소켓통신 변수
	int sd,clientlen = sizeof(cli);		//소켓통신 변수
	
	int *sns;		//accept되면 결과값을 저장할 변수
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	//소켓 생성
		perror("socket");
		exit(1);
	}
	
	int optvalue=1;		//포트번호 재사용 옵션
	setsockopt(sd, SOL_SOCKET, TCP_NODELAY, &optvalue, sizeof(optvalue));	//TCP_NODELAY 로 옵션 변경


	memset((char*)&sin, '\0', sizeof(sin));		//서버 초기 셋팅
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");	//모든 IP에서 접속 가능

	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){	//bind
		perror("bind");
		exit(1);
	}

	if(listen(sd,5)){		//listen
		perror("listen");
		exit(1);
	}

	int tnum;	//스레드 갯수 조절용 변수
	char tempbuf[BUFSIZ];	//sns 값과 클라이언트 ip를 합치기 위한 임시 버퍼
	char* throw;	//스레드로 던져 줄 동적 메모리 할당 변수 값
	
	if (pthread_mutex_init (&m_lock, NULL) != 0)	//스레드 사용 준비
	{
		perror ("Mutex Init failure");
		return 1;
	}
	
	for(tnum=0; ;tnum++){	//요청이 들어올 때마다 스레드 생성할 수 있도록 반복문 사용
		sns = (int*)malloc(sizeof(int));	//다른 스레드와 sns가 섞이지 않도록 주소 고정 작업
		*sns = accept(sd, (struct sockaddr *)&cli, &clientlen);	//accept받아서 sns에 넣기
		sprintf(tempbuf,"%d %s", *sns, inet_ntoa(cli.sin_addr));	//*sns값과 클라이언트 IP를 tempbuf에 합치기
		throw = (char*)malloc(sizeof(char) * (strlen(tempbuf)+1));	//tempbuf 값이 다른 주소와 섞이지 않도록 동적 메모리 할당 
		strcpy(throw, tempbuf);			//throw에 tmepbuf 넣기
		pthread_create(&tid[tnum], NULL, t_func,(void*)throw);	//스레드에 throw를 전송
		if(tnum>=MAX_THREAD-1) tnum=0;		//MAX_THREAD 값만큼 accept가 일어나면 다시 처음 스레드
	}
	pthread_mutex_destroy (&m_lock);	//mutex 해제. 정상적으로 여기로 올 일이 없음
	return 0;
}
