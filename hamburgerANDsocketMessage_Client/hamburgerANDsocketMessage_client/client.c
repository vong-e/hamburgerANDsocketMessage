/*
[기초프로젝트I]
프로젝트이름 : [팀 프로젝트 2 ] 스택, 소켓 통신을 활용한 햄버거 만들기 게임+채팅 프로그램
설명 : 스택, 소켓 통신을 활용해서 햄버거 만들기 게임과 채팅 프로그램 두개가 동시에 가능한 프로그램을 만든다.
프로그램 실행시 유저는 햄버거 만들기 게임, 채팅 프로그램 중 원하는 프로그램을 선택할 수 있으며
프로그램 실행 도중에 다른 프로그램으로 전환이 가능하다.

개발 시작일자 : 2017.06. 02 (금)
개발 완료일자 : 2017.06, 20 (화)
개발자 / 학번 : 김송봉 / 2013244128 , 윤상근 / 2013244036

[개발 부분]
소켓 통신 부분 : http://remocon33.tistory.com/465 참고
햄버거 스택 프로그램 : 김송봉
메뉴 상호작용, 인터페이스 : 김송봉

햄버거 정답 비교( calculator() ) 프로그램 : 윤상근
스코어 프로그램 : 윤상근

** 프로그램 작동이 안될시 : 프로젝트 속성 -> 링커  -> 입력   -> 출력파일  ->   wsock32.lib; 라이브러리 추가
*/


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <time.h>
#define whileOn 1 // while 문을 실행할때 1을 대신하여 사용할 변수
#define whileOff 0 // while 문 종료시킬때 0을 대신하여 사용할 변수
#define STACK_SIZE 7 // stack 의 크기


#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg);//쓰레드 전송함수
unsigned WINAPI RecvMsg(void* arg);//쓰레드 수신함수
void ErrorHandling(char* msg);
void Make();
int* calculator(int make[]);
char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main() {
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN serverAddr;
	HANDLE sendThread, recvThread;

	char myIp[50];
	char port[50];
	char inputName[20];

	printf("┏━━━━━━━━━━━━━━\n");
	printf("┃서버의 IP 주소를 입력하세요 : ");
	gets(myIp);
	printf("┣━━━━━━━━━━━━━━\n");
	printf("┃서버의 port 를 입력하세요   : ");
	gets(port);
	printf("┣━━━━━━━━━━━━━━\n");
	printf("┃사용하실 ID 를 입력하세요   : ");
	gets(inputName);
	printf("┗━━━━━━━━━━━━━━\n");


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)// 윈도우 소켓을 사용한다고 운영체제에 알림
		ErrorHandling("WSAStartup() error!");

	sprintf(name, "[%s]", inputName);
	sock = socket(PF_INET, SOCK_STREAM, 0);//소켓을 하나 생성한다.

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(myIp);
	serverAddr.sin_port = htons(atoi(port));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//서버에 접속한다.
		ErrorHandling("connect() error");

	//접속에 성공하면 이 줄 아래가 실행된다.

	sendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&sock, 0, NULL);//메시지 전송용 쓰레드가 실행된다.
	recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&sock, 0, NULL);//메시지 수신용 쓰레드가 실행된다.

	WaitForSingleObject(sendThread, INFINITE);//전송용 쓰레드가 중지될때까지 기다린다./
	WaitForSingleObject(recvThread, INFINITE);//수신용 쓰레드가 중지될때까지 기다린다.
											  //클라이언트가 종료를 시도한다면 이줄 아래가 실행된다.
	closesocket(sock);//소켓을 종료한다.
	WSACleanup();//윈도우 소켓 사용중지를 운영체제에 알린다.
	return 0;
}

unsigned WINAPI SendMsg(void* arg) {//전송용 쓰레드함수
	SOCKET sock = *((SOCKET*)arg);//서버용 소켓을 전달한다.
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int start, end;
	int count = 1; //점수비교 
	int standardScore = 100; // 기본점수를 100으로 주고시작 
	int data, temp, sw, stack[STACK_SIZE];
	int top_pos = -1; // 스택의 TOP부분을 -1로 초기화해놓음
	int pop(), top();
	char menu;// 메뉴선택 (삽입할지, 삭제할지, 출력할지)    // opcode
	char menuSelect[4]; // 입력받은 메뉴를 저장할 배열      //inp_string
	int make[7];
	int plus = 0;
	char gameChoose[4]; // 게임을 선택할 배열
	char gameMenu; // 게임선택

MENUSELECT: // goto문의 목적지
	printf("┏━━━━━━ 햄버거 게임 + 채팅 프로그램━━━━━━┓\n");
	printf("┃   햄버거 게임과 채팅중에 무엇을 하실지 선택하세요  ┃\n");
	printf("┃      햄버거 게임 : H         채팅 프로그램 : C     ┃\n");
	printf("┃                 프로그램 종료 : X                  ┃\n");
	printf("┃ ※ 채팅프로그램에서 'q' 입력시 메뉴로 돌아옵니다 ! ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");

	scanf("%s", gameChoose);

	gameMenu = gameChoose[0];
	sw = whileOn;
	while (sw) {       // 메뉴선택을 위한 WHILE 문 
		switch (gameMenu)
		{
		case 'H': // 'H' 를 입력하면 goto문을 따라서 up: 의 위치로 이동하고 햄버거 게임을 시작한다.
			goto	up;
			break;

		case 'C': // 'C' 를 입력하면 채팅 프로그램을 시작한다.
		{SOCKET sock = *((SOCKET*)arg);//서버용 소켓을 전달한다.
		char nameMsg[NAME_SIZE + BUF_SIZE];
		while (1) {//반복
			fgets(msg, BUF_SIZE, stdin);//입력을 받는다.
			if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {//q를 입력하면 종료한다.
															 //closesocket(sock);
				goto MENUSELECT;
				exit(0);
			}
			sprintf(nameMsg, "%s %s", name, msg);//nameMsg에 메시지를 전달한다.
			send(sock, nameMsg, strlen(nameMsg), 0);//nameMsg를 서버에게 전송한다.
		}
		return 0; }
		case 'X':  // 'X' 를 입력하면 프로그램 종료 
			exit(0);
			sw = whileOff;

		default:
			printf("\n 메뉴 선택을 잘못하셨습니다. \n"); // H, C, X 이외의 알파벳 입력했을 떄 출력
			goto MENUSELECT;
			break;

		}
	}
	sw = whileOn;
	void push(); // push정의

	printf("┏━━━━━햄버거 만들기 프로그램━━━━━┓\n");
	printf("┃ 화면에서 보여주는 햄버거를 만들어주세요. ┃\n");
	printf("┃    원하는 내용물의 번호를 입력하시면     ┃\n");
	printf("┃ 내용물이 쌓여서 햄버거가 만들어 집니다 ! ┃\n");
	printf("┃ 재료를 넣을때는 위의 재료부터 넣어주세요.┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━┛\n\n");
	printf("┏━━━━━━━ 점수 지급방식━━━━━━━┓\n");
	printf("┃       시간에 따라 점수가 차등 지급       ┃\n");
	printf("┃  기본점수는 100점, 맞춘 재료 하나당 20점 ┃\n");
	printf("┃    너무 느리면 점수 - 될 수도 있어요 !   ┃\n");
	printf("┗━━━━━━━━━━━━━━━━━━━━━┛\n\n");
up:// goto의 목적지
	while (sw) {
		Make(); // MAKE 함수호출. 게임방법설명 
		calculator(make); //정답비교함수 호출
		printf("┌────┐\n");

		for (int i = 1; i < 6; i++) {  // 랜덤으로 생성한 번호에 맞는 재료들 출력

			if (make[i] == 2)
				printf(" ~~~~~~~~~~\n");
			else if (make[i] == 3)
				printf(" ==========\n");
			else if (make[i] == 4)
				printf(" ----------\n");
			else if (make[i] == 5)
				printf(" ~=~=~=~=~=\n");
			else if (make[i] == 6)
				printf(" ::::::::::\n");
		}
		printf("└────┘\n");
		start = (int)time(NULL);
		while (sw) {
			printf("┏━━━━━━━━━━━━━━━━메뉴선택━━━━━━━━━━━━━━━━━┓\n");  // Q W E R Z로 메뉴번호 한 이유는 게임의 속도를 높이기 위해
			printf("┃   Q : 재료넣기  W :재료빼기  E:재료출력  R :완성  Z : 메뉴선택 돌아가기  ┃\n");  // 가까이 붙어있는 알파벳들을 사용
			printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
			scanf("%s", menuSelect);
			menu = menuSelect[0];
			switch (menu)
			{
			case 'Q': //스택의 push
				printf("햄버거에 넣을 재료의 번호를 입력해주세요 : ");
				scanf("%d", &data);
				// printf("삽입한 재료 = %d\n", data);
				push(stack, data, &top_pos);
				break;

			case 'W': // 스택의 pop
				data = pop(stack, &top_pos);
				printf("가장 마지막에 넣은 %d 번 재료를 제거합니다 ! \n", data);
				break;

			case 'E':  // 스택 출력
				temp = top_pos;
				while (temp >= 0) //stack 의 top 이 0이될 때 까지 출력함
					printf("%3d \n", stack[temp--]);
				break;

			case 'R':
				for (int i = 0; i < 7; i++) {//비교부분
					if (make[i] == stack[i]) {
						printf("%d번째 재료 맞음\n", count);
						printf("샘플 : %d입력 : %d\n", make[i], stack[i]);
						count++;
						plus = plus + 20;
					}
					else if (make[i] != stack[i]) {
						printf("%d번째 재료 틀림\n", count);
						printf("샘플 : %d입력 : %d\n", make[i], stack[i]);
						count++;
					}
					data = pop(stack, &top_pos); // 재료를 POP 시켜주어서 다음라운드 떄 스택이 꽉 차 있지 않도록 함 FOR 문안에서 반복하며 스택을 비움

				}

				end = (int)time(NULL);
				printf("%s의 현재 점수 : %d\n", name, plus + (standardScore - (end - start)));
				plus = 0;
				count = 1;

				goto up;// up: 부분으로 올라간다

			case 'Z'://종료
				goto MENUSELECT;
				break;
				sw = whileOff; //스위치 off

			default:
				printf("\n 메뉴 선택을 잘못하셨습니다. \n"); // Q, W, E, R 이외의 메뉴 선택시 출력해줄 문구
				break;
			}
		}
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg) {

	SOCKET sock = *((SOCKET*)arg);//서버용 소켓을 전달한다.
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen;
	while (1) {//반복
		strLen = recv(sock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);//서버로부터 메시지를 수신한다.
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;//문자열의 끝을 알리기 위해 설정
		if (!strcmp(nameMsg, "X")) {
			printf("사용자가 채팅을 떠났습니다..,t\n");
			closesocket(sock);
			exit(0);
		}
		fputs(nameMsg, stdout);//자신의 콘솔에 받은 메시지를 출력한다.
	}
	return 0;
}

void ErrorHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void Make() { // 만들기설명함수

	printf("1번.┌────┐ (햄버거 윗빵) \n2번. ~~~~~~~~~~ (베이컨) \n3번. ========== (고기패티)");
	printf("\n4번. ---------- (치즈) \n5번. ~=~=~=~=~= (양상추) \n6번. :::::::::: (새우패티) \n7번.└────┘ (햄버거 아랫빵)\n");
	printf("위의 재료의 모양에 맞춰서 일치하는 번호를 입력해주세요!\n\n");
	printf("이 햄버거를 주문할께요 \n\n");
}

int* calculator(int make[]) {
	make[0] = 1;
	make[6] = 7;

	srand((int)time(NULL));

	for (int i = 1; i < 6; i++) {//만들어야할 재료 랜덤 생성                                        
		make[i] = rand() % 5 + 2;
		for (int j = 1; j < i; j++) { //중복 방지
			if (make[i] == make[j]) { //중복이면 i카운트를 빼서 다시실행
				make[i] = rand() % 5 + 2;
				i--;
			}
		}
	}
	return make;
}

void push(int stack[], int data, int *top_pos) {
	if (*top_pos == STACK_SIZE - 1)
		printf("재료가 꽉 찼네요...\n"); // stack 이 꽉 찼을때 출력할 문구
	(*top_pos)++;
	stack[*top_pos] = data;
}

int pop(int stack[], int *top_pos) {
	int data;
	if (*top_pos < 0)
		printf("재료가 하나도 없어요...\n"); // stack이 텅 비어있을때 출력할 문구
	data = stack[*top_pos];
	(*top_pos)--;
	return(data);
}