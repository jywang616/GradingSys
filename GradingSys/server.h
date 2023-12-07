#pragma once
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cerrno>
#include<cstdlib>

#define MY_PORT 6591//�˿ں�
#define BUF_SIZE 1024//��󻺴�
#define MAX_QUEUE_NUM 5//���������

struct Client //����˵Ŀͻ�
{
    int client_sock;
    char buffer[BUF_SIZE]; //�����û�������
    struct sockaddr_in client_addr;//����ͻ��˵�ַ��Ϣ
    socklen_t length = sizeof(client_addr);//��Ҫ���ڴ��С
};

char* parseCommand(char* message); // �����û������룬����������������