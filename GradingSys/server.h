#pragma once
#include<stdio.h>
#include<string.h>
<<<<<<< Updated upstream
=======
#include<string>
>>>>>>> Stashed changes
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cerrno>
#include<cstdlib>

<<<<<<< Updated upstream
#define MY_PORT 6591//�˿ں�
#define BUF_SIZE 1024//��󻺴�
#define MAX_QUEUE_NUM 5//���������

struct Client //����˵Ŀͻ�
{
    int client_sock;
=======
#define MY_PORT 12345//�������˿ں�
#define BUF_SIZE 1024//��󻺴�
#define MAX_QUEUE_NUM 5//���������

struct Client //����˿ͻ�
{
    int client_sock;
    std::string username;
    std::string passwd;
>>>>>>> Stashed changes
    char buffer[BUF_SIZE]; //�����û�������
    struct sockaddr_in client_addr;//����ͻ��˵�ַ��Ϣ
    socklen_t length = sizeof(client_addr);//��Ҫ���ڴ��С
};
<<<<<<< Updated upstream

char* parseCommand(char* message); // �����û������룬����������������
=======
char* parseCommand(char* message); // �����û������룬����������������
>>>>>>> Stashed changes
