/*
[����������ƮI]
������Ʈ�̸� : [�� ������Ʈ 2 ] ����, ���� ����� Ȱ���� �ܹ��� ����� ����+ä�� ���α׷�
���� : ����, ���� ����� Ȱ���ؼ� �ܹ��� ����� ���Ӱ� ä�� ���α׷� �ΰ��� ���ÿ� ������ ���α׷��� �����.
���α׷� ����� ������ �ܹ��� ����� ����, ä�� ���α׷� �� ���ϴ� ���α׷��� ������ �� ������
���α׷� ���� ���߿� �ٸ� ���α׷����� ��ȯ�� �����ϴ�.

���� �������� : 2017.06. 02 (��)
���� �Ϸ����� : 2017.06, 20 (ȭ)
������ / �й� : ��ۺ� / 2013244128 , ����� / 2013244036

[���� �κ�]
���� ��� �κ� : http://remocon33.tistory.com/465 ����
�ܹ��� ���� ���α׷� : ��ۺ�
�޴� ��ȣ�ۿ�, �������̽� : ��ۺ�

�ܹ��� ���� ��( calculator() ) ���α׷� : �����
���ھ� ���α׷� : �����

** ���α׷� �۵��� �ȵɽ� : ������Ʈ �Ӽ� -> ��Ŀ  -> �Է�   -> �������  ->   wsock32.lib; ���̺귯�� �߰�
*/


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <time.h>
#define whileOn 1 // while ���� �����Ҷ� 1�� ����Ͽ� ����� ����
#define whileOff 0 // while �� �����ų�� 0�� ����Ͽ� ����� ����
#define STACK_SIZE 7 // stack �� ũ��


#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg);//������ �����Լ�
unsigned WINAPI RecvMsg(void* arg);//������ �����Լ�
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

	printf("������������������������������\n");
	printf("�������� IP �ּҸ� �Է��ϼ��� : ");
	gets(myIp);
	printf("������������������������������\n");
	printf("�������� port �� �Է��ϼ���   : ");
	gets(port);
	printf("������������������������������\n");
	printf("������Ͻ� ID �� �Է��ϼ���   : ");
	gets(inputName);
	printf("������������������������������\n");


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)// ������ ������ ����Ѵٰ� �ü���� �˸�
		ErrorHandling("WSAStartup() error!");

	sprintf(name, "[%s]", inputName);
	sock = socket(PF_INET, SOCK_STREAM, 0);//������ �ϳ� �����Ѵ�.

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(myIp);
	serverAddr.sin_port = htons(atoi(port));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//������ �����Ѵ�.
		ErrorHandling("connect() error");

	//���ӿ� �����ϸ� �� �� �Ʒ��� ����ȴ�.

	sendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&sock, 0, NULL);//�޽��� ���ۿ� �����尡 ����ȴ�.
	recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&sock, 0, NULL);//�޽��� ���ſ� �����尡 ����ȴ�.

	WaitForSingleObject(sendThread, INFINITE);//���ۿ� �����尡 �����ɶ����� ��ٸ���./
	WaitForSingleObject(recvThread, INFINITE);//���ſ� �����尡 �����ɶ����� ��ٸ���.
											  //Ŭ���̾�Ʈ�� ���Ḧ �õ��Ѵٸ� ���� �Ʒ��� ����ȴ�.
	closesocket(sock);//������ �����Ѵ�.
	WSACleanup();//������ ���� ��������� �ü���� �˸���.
	return 0;
}

unsigned WINAPI SendMsg(void* arg) {//���ۿ� �������Լ�
	SOCKET sock = *((SOCKET*)arg);//������ ������ �����Ѵ�.
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int start, end;
	int count = 1; //������ 
	int standardScore = 100; // �⺻������ 100���� �ְ���� 
	int data, temp, sw, stack[STACK_SIZE];
	int top_pos = -1; // ������ TOP�κ��� -1�� �ʱ�ȭ�س���
	int pop(), top();
	char menu;// �޴����� (��������, ��������, �������)    // opcode
	char menuSelect[4]; // �Է¹��� �޴��� ������ �迭      //inp_string
	int make[7];
	int plus = 0;
	char gameChoose[4]; // ������ ������ �迭
	char gameMenu; // ���Ӽ���

MENUSELECT: // goto���� ������
	printf("�������������� �ܹ��� ���� + ä�� ���α׷���������������\n");
	printf("��   �ܹ��� ���Ӱ� ä���߿� ������ �Ͻ��� �����ϼ���  ��\n");
	printf("��      �ܹ��� ���� : H         ä�� ���α׷� : C     ��\n");
	printf("��                 ���α׷� ���� : X                  ��\n");
	printf("�� �� ä�����α׷����� 'q' �Է½� �޴��� ���ƿɴϴ� ! ��\n");
	printf("��������������������������������������������������������\n\n");

	scanf("%s", gameChoose);

	gameMenu = gameChoose[0];
	sw = whileOn;
	while (sw) {       // �޴������� ���� WHILE �� 
		switch (gameMenu)
		{
		case 'H': // 'H' �� �Է��ϸ� goto���� ���� up: �� ��ġ�� �̵��ϰ� �ܹ��� ������ �����Ѵ�.
			goto	up;
			break;

		case 'C': // 'C' �� �Է��ϸ� ä�� ���α׷��� �����Ѵ�.
		{SOCKET sock = *((SOCKET*)arg);//������ ������ �����Ѵ�.
		char nameMsg[NAME_SIZE + BUF_SIZE];
		while (1) {//�ݺ�
			fgets(msg, BUF_SIZE, stdin);//�Է��� �޴´�.
			if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {//q�� �Է��ϸ� �����Ѵ�.
															 //closesocket(sock);
				goto MENUSELECT;
				exit(0);
			}
			sprintf(nameMsg, "%s %s", name, msg);//nameMsg�� �޽����� �����Ѵ�.
			send(sock, nameMsg, strlen(nameMsg), 0);//nameMsg�� �������� �����Ѵ�.
		}
		return 0; }
		case 'X':  // 'X' �� �Է��ϸ� ���α׷� ���� 
			exit(0);
			sw = whileOff;

		default:
			printf("\n �޴� ������ �߸��ϼ̽��ϴ�. \n"); // H, C, X �̿��� ���ĺ� �Է����� �� ���
			goto MENUSELECT;
			break;

		}
	}
	sw = whileOn;
	void push(); // push����

	printf("�������������ܹ��� ����� ���α׷�������������\n");
	printf("�� ȭ�鿡�� �����ִ� �ܹ��Ÿ� ������ּ���. ��\n");
	printf("��    ���ϴ� ���빰�� ��ȣ�� �Է��Ͻø�     ��\n");
	printf("�� ���빰�� �׿��� �ܹ��Ű� ����� ���ϴ� ! ��\n");
	printf("�� ��Ḧ �������� ���� ������ �־��ּ���.��\n");
	printf("����������������������������������������������\n\n");
	printf("���������������� ���� ���޹�Ħ���������������\n");
	printf("��       �ð��� ���� ������ ���� ����       ��\n");
	printf("��  �⺻������ 100��, ���� ��� �ϳ��� 20�� ��\n");
	printf("��    �ʹ� ������ ���� - �� ���� �־�� !   ��\n");
	printf("����������������������������������������������\n\n");
up:// goto�� ������
	while (sw) {
		Make(); // MAKE �Լ�ȣ��. ���ӹ������ 
		calculator(make); //������Լ� ȣ��
		printf("������������\n");

		for (int i = 1; i < 6; i++) {  // �������� ������ ��ȣ�� �´� ���� ���

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
		printf("������������\n");
		start = (int)time(NULL);
		while (sw) {
			printf("�����������������������������������޴����æ�����������������������������������\n");  // Q W E R Z�� �޴���ȣ �� ������ ������ �ӵ��� ���̱� ����
			printf("��   Q : ���ֱ�  W :��ứ��  E:������  R :�ϼ�  Z : �޴����� ���ư���  ��\n");  // ������ �پ��ִ� ���ĺ����� ���
			printf("������������������������������������������������������������������������������\n");
			scanf("%s", menuSelect);
			menu = menuSelect[0];
			switch (menu)
			{
			case 'Q': //������ push
				printf("�ܹ��ſ� ���� ����� ��ȣ�� �Է����ּ��� : ");
				scanf("%d", &data);
				// printf("������ ��� = %d\n", data);
				push(stack, data, &top_pos);
				break;

			case 'W': // ������ pop
				data = pop(stack, &top_pos);
				printf("���� �������� ���� %d �� ��Ḧ �����մϴ� ! \n", data);
				break;

			case 'E':  // ���� ���
				temp = top_pos;
				while (temp >= 0) //stack �� top �� 0�̵� �� ���� �����
					printf("%3d \n", stack[temp--]);
				break;

			case 'R':
				for (int i = 0; i < 7; i++) {//�񱳺κ�
					if (make[i] == stack[i]) {
						printf("%d��° ��� ����\n", count);
						printf("���� : %d�Է� : %d\n", make[i], stack[i]);
						count++;
						plus = plus + 20;
					}
					else if (make[i] != stack[i]) {
						printf("%d��° ��� Ʋ��\n", count);
						printf("���� : %d�Է� : %d\n", make[i], stack[i]);
						count++;
					}
					data = pop(stack, &top_pos); // ��Ḧ POP �����־ �������� �� ������ �� �� ���� �ʵ��� �� FOR ���ȿ��� �ݺ��ϸ� ������ ���

				}

				end = (int)time(NULL);
				printf("%s�� ���� ���� : %d\n", name, plus + (standardScore - (end - start)));
				plus = 0;
				count = 1;

				goto up;// up: �κ����� �ö󰣴�

			case 'Z'://����
				goto MENUSELECT;
				break;
				sw = whileOff; //����ġ off

			default:
				printf("\n �޴� ������ �߸��ϼ̽��ϴ�. \n"); // Q, W, E, R �̿��� �޴� ���ý� ������� ����
				break;
			}
		}
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg) {

	SOCKET sock = *((SOCKET*)arg);//������ ������ �����Ѵ�.
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen;
	while (1) {//�ݺ�
		strLen = recv(sock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);//�����κ��� �޽����� �����Ѵ�.
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;//���ڿ��� ���� �˸��� ���� ����
		if (!strcmp(nameMsg, "X")) {
			printf("����ڰ� ä���� �������ϴ�..,t\n");
			closesocket(sock);
			exit(0);
		}
		fputs(nameMsg, stdout);//�ڽ��� �ֿܼ� ���� �޽����� ����Ѵ�.
	}
	return 0;
}

void ErrorHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void Make() { // ����⼳���Լ�

	printf("1��.������������ (�ܹ��� ����) \n2��. ~~~~~~~~~~ (������) \n3��. ========== (�����Ƽ)");
	printf("\n4��. ---------- (ġ��) \n5��. ~=~=~=~=~= (�����) \n6��. :::::::::: (������Ƽ) \n7��.������������ (�ܹ��� �Ʒ���)\n");
	printf("���� ����� ��翡 ���缭 ��ġ�ϴ� ��ȣ�� �Է����ּ���!\n\n");
	printf("�� �ܹ��Ÿ� �ֹ��Ҳ��� \n\n");
}

int* calculator(int make[]) {
	make[0] = 1;
	make[6] = 7;

	srand((int)time(NULL));

	for (int i = 1; i < 6; i++) {//�������� ��� ���� ����                                        
		make[i] = rand() % 5 + 2;
		for (int j = 1; j < i; j++) { //�ߺ� ����
			if (make[i] == make[j]) { //�ߺ��̸� iī��Ʈ�� ���� �ٽý���
				make[i] = rand() % 5 + 2;
				i--;
			}
		}
	}
	return make;
}

void push(int stack[], int data, int *top_pos) {
	if (*top_pos == STACK_SIZE - 1)
		printf("��ᰡ �� á�׿�...\n"); // stack �� �� á���� ����� ����
	(*top_pos)++;
	stack[*top_pos] = data;
}

int pop(int stack[], int *top_pos) {
	int data;
	if (*top_pos < 0)
		printf("��ᰡ �ϳ��� �����...\n"); // stack�� �� ��������� ����� ����
	data = stack[*top_pos];
	(*top_pos)--;
	return(data);
}