#include"server.h"
#include"os.h"
#include<limits>
#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<iostream>

const int Superblock_Start_Addr = 0;     //44B:1block
const int InodeBitmap_Start_Addr = 1 * BLOCK_SIZE; //1024B:2block
const int BlockBitmap_Start_Addr = InodeBitmap_Start_Addr + 2 * BLOCK_SIZE;//10240B:20block
const int Inode_Start_Addr = BlockBitmap_Start_Addr + 20 * BLOCK_SIZE;//120<128: �����x��block
const int Block_Start_Addr = Inode_Start_Addr + INODE_NUM / (BLOCK_SIZE / INODE_SIZE) * BLOCK_SIZE;//32*16=512

const int Disk_Size = Block_Start_Addr + BLOCK_NUM * BLOCK_SIZE;
const int File_Max_Size = 10 * BLOCK_SIZE;


int Root_Dir_Addr;							//��Ŀ¼inode��ַ
int Cur_Dir_Addr;							//��ǰĿ¼:��inode��ַ
char Cur_Dir_Name[310];						//��ǰĿ¼��
char Cur_Host_Name[110];					//��ǰ������
char Cur_User_Name[110];					//��ǰ��½�û���
char Cur_Group_Name[110];					//��ǰ��½�û�����
char Cur_User_Dir_Name[310];				//��ǰ��½�û�Ŀ¼��

int nextUID;								//��һ��Ҫ������û���ʶ��
int nextGID;								//��һ��Ҫ������û����ʶ��

bool isLogin;								//�Ƿ����û���½

FILE* fw;									//��������ļ� д�ļ�ָ��
FILE* fr;									//��������ļ� ���ļ�ָ��
SuperBlock* superblock = new SuperBlock;	//������ָ��
bool inode_bitmap[INODE_NUM];				//inodeλͼ
bool block_bitmap[BLOCK_NUM];				//���̿�λͼ

char buffer[10000000] = { 0 };				//10M������������������ļ�
using namespace std;

int Initialize(Client& client)
{
    int client_sock = client.client_sock;
    char buff[] = "GradingSys Greeting!\n";
    send(client_sock, buff, strlen(buff), 0);
    //###############�򲻿��ļ�################
    if ((fr = fopen(GRADE_SYS_NAME, "rb")) == NULL)
    {
        fw = fopen(GRADE_SYS_NAME, "wb");
        if (fw == NULL) {
            strcpy(buff, "Failed to open the virtual disc file!\n");
            send(client_sock, buff, strlen(buff), 0);
            return 0;
        }
        fr = fopen(GRADE_SYS_NAME, "rb");
        strcpy(buff, "Virtual disc file openned successfully!\n");
        send(client_sock, buff, strlen(buff), 0);
        //��ʼ������
        nextUID = 0;
        nextGID = 0;
        isLogin = false;
        strcpy(Cur_User_Name, "root");
        strcpy(Cur_Group_Name, "root");

        //��ȡ������
        memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
        if (gethostname(Cur_Host_Name, sizeof(Cur_Host_Name)) != 0) {
            perror("Error getting hostname");
            return 1;
        }

        Root_Dir_Addr = Inode_Start_Addr;
        Cur_Dir_Addr = Root_Dir_Addr;
        strcpy(Cur_Dir_Name, "/");
        strcpy(buff, "Formatting the file system...\n");
        send(client_sock, buff, strlen(buff), 0);

        //ϵͳ��ʽ��
        if (!Format()) {
            char buff1[] = "Formatting file system failed!\n";
            send(client_sock, buff1, strlen(buff1), 0);
            return 0;
        }
        strcpy(buff, "Formatting done.\n\n");
        send(client_sock, buff, strlen(buff), 0);
        //Install
        if (!Install()) {

            strcpy(buff, "File system installation failure!\n");
            send(client_sock, buff, strlen(buff), 0);
            return 0;
        }
    }
    else
    {
        fw = fopen(GRADE_SYS_NAME, "rb+"); //��ԭ���Ļ������޸��ļ�
        if (fw == NULL) {
            char buff1[] = "Disk files openning failure!\n";
            send(client_sock, buff1, strlen(buff1), 0);
            return false;
        }
        //��ʼ������
        nextUID = 0;
        nextGID = 0;
        isLogin = false;
        strcpy(Cur_User_Name, "root");
        strcpy(Cur_Group_Name, "root");

        //��ȡ������
        memset(Cur_Host_Name, 0, sizeof(Cur_Host_Name));
        if (gethostname(Cur_Host_Name, sizeof(Cur_Host_Name)) != 0) {
            perror("Error getting hostname");
            return 1;
        }

        //��ȡ��Ŀ¼
        Root_Dir_Addr = Inode_Start_Addr;
        Cur_Dir_Addr = Root_Dir_Addr;
        strcpy(Cur_Dir_Name, "/");

        //�Ƿ���Ҫ��ʽ��
        strcpy(buff, "Format the file system? [y/n]");
        send(client_sock, buff, strlen(buff), 0);
        memset(client.buffer, 0, sizeof(client.buffer)); // ��ʼ���û�����buffer
        recv(client_sock, client.buffer, sizeof(client.buffer), 0);
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
        char yes[] = "y";
        if (client.buffer == yes) {
            if (!Format()) {
                char buff1[] = "Failed to format the system!\n";
                send(client_sock, buff1, strlen(buff1), 0);
                return 0;
            }
        }
        strcpy(buff, "Format done!\n");
        send(client_sock, buff, strlen(buff), 0);

        //Install
        if (!Install()) {
            char buff1[] = "File system installation failed!\n";
            send(client_sock, buff1, strlen(buff1), 0);
            return 0;
        }
        strcpy(buff, "File system installation done.\n");
        send(client_sock, buff, strlen(buff), 0);
    }
}
void handleClient(Client& client)
{
    int client_sock = client.client_sock;
    while (1)
    {
        if (isLogin)
        {
            char* p;
            if ((p = strstr(Cur_Dir_Name, Cur_User_Dir_Name)) == NULL)	//��ǰ�Ƿ����û�Ŀ¼��
            {
                char output_buffer[BUF_SIZE];
                // ʹ��snprintf����ʽ�����ַ����洢��output_buffer��
                snprintf(output_buffer, BUF_SIZE, "[%s@%s %s]# ", Cur_Host_Name, Cur_User_Name, Cur_Dir_Name);
                //[Linux@yhl /etc]
                send(client_sock, output_buffer, strlen(output_buffer), 0);
            }
            else
            {
                char output_buffer[BUF_SIZE];
                snprintf(output_buffer, BUF_SIZE, "[%s@%s %s]# ", Cur_Host_Name, Cur_User_Name, Cur_Dir_Name + strlen(Cur_User_Dir_Name));
                //[Linux@yhl ~/app]
                send(client_sock, output_buffer, strlen(output_buffer), 0);
            }
            //gets(str);
            //cmd(str);
            /*useradd("felin", "123", "teacher");
            cd(Cur_Dir_Addr, "..");
            cd(Cur_Dir_Addr, "felin");
            mkdir(Cur_Dir_Addr, "ms");
<<<<<<< Updated upstream
            // ls    
=======
>>>>>>> Stashed changes
            mkfile(Cur_Dir_Addr, "tert", "helloworld");
            rmdir(Cur_Dir_Addr, "felin");
            userdel("felin");*/
            char output_buffer[BUF_SIZE];
            snprintf(output_buffer, BUF_SIZE, "[%s@%s %s]# ", Cur_Host_Name, Cur_User_Name, Cur_Dir_Name + strlen(Cur_User_Dir_Name));
            send(client_sock, output_buffer, strlen(output_buffer), 0);
            // ׼�������û�����
<<<<<<< Updated upstream
            memset(client.buffer, 0, sizeof(client.buffer));
            int len = recv(client_sock, client.buffer, sizeof(client.buffer), 0);
            if (strcmp(client.buffer, "exit\n") == 0 || len <= 0)
            {
                printf("Client %d has logged out the system!\n", client_sock);
                break;
            }
=======
            memset(client.buffer, 0, sizeof(client.buffer)); // ��ʼ���û�����buffer
            int len = recv(client_sock, client.buffer, sizeof(client.buffer), 0);
            if (strcmp(client.buffer, "exit\n") == 0 || len <= 0)
                break;
>>>>>>> Stashed changes
            printf("Client %d send message: %s", client_sock, client.buffer);
            //parseCommand(client.buffer);
            //cmd(client.buffer);
            //send(client_sock, client.buffer, strlen(client.buffer), 0);
            //printf("Send message: %s\n", client.buffer);
        }
        else
        {
            char buff[] = "Welcome to GradingSysOS! Login first, please!\n";
            send(client_sock, buff, strlen(buff), 0);
<<<<<<< Updated upstream
            while (!login(client));
=======
            while (!login(client));	//��½
>>>>>>> Stashed changes
            strcpy(buff, "Successfully logged into our system!\n");
            send(client_sock, buff, strlen(buff), 0);
        }
    }
    close(client_sock);
}

char* parseCommand(char* buffer)
{
    return buffer;
}

int main()
{
<<<<<<< Updated upstream
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//ipv4
    server_sockaddr.sin_port = htons(MY_PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_sock, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1) {//�󶨱���ip��˿�
        perror("Bind Failure\n");
        printf("Error: %s\n", strerror(errno));
        close(server_sock);
=======
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);//������������Ӧsocket
    struct sockaddr_in server_sockaddr;//���汾�ص�ַ��Ϣ
    server_sockaddr.sin_family = AF_INET;//����ipv4
    server_sockaddr.sin_port = htons(MY_PORT);//ָ���˿�
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//��ȡ�������յ�������Ӧ

    if (bind(server_sock, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1) {//�󶨱���ip��˿�
        perror("Bind Failure\n");
        printf("Error: %s\n", strerror(errno));//���������Ϣ
>>>>>>> Stashed changes
        return -1;
    }

    printf("Listen Port : %d\n", MY_PORT);
<<<<<<< Updated upstream
    if (listen(server_sock, MAX_QUEUE_NUM) == -1) {
        perror("Listen Error");
        close(server_sock);
=======
    if (listen(server_sock, MAX_QUEUE_NUM) == -1) {//���ü���״̬
        perror("Listen Error");
>>>>>>> Stashed changes
        return -1;
    }

    printf("Waiting for connection!\n");
    while (true)
    {
        Client client;
        client.client_sock = accept(server_sock, (struct sockaddr*)&client.client_addr, &client.length);
        if (client.client_sock == -1) {
            perror("Connect Error");
            return -1;
        }
        printf("Connection Successful\n");
        if (fork() == 0) {
            // �ӽ���
            close(server_sock); // �ӽ��̹رշ���������
            Initialize(client); // ���³�ʼ��������Client����
            handleClient(client); // ����ͻ�������
            close(client.client_sock); // �ӽ��̴�����Ϻ�ر��׽���
            exit(0); // �ӽ��̴�����Ϻ��˳�
        }
        // �����̼�������������Ҫ����Ĵ���
    }

    close(server_sock);//�رշ�������Ӧsocket
    return 0;
}