#pragma comment(lib,"ws2_32")

#include<windows.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<process.h>

#define BUFFSIZE 1024

void sendMsg(void* sock);
void recvMsg(void* sock);

bool QUIT = false;

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 1, 1 );                   // ʹ�õ� WinSock �İ汾

    // �����׽��ֿ�
    int err = WSAStartup( wVersionRequested, &wsaData );    // ��ʼ�� WinSock DLL ��
    if ( err != 0 )
    {
        printf("WSASTARTUP_ERROR: %d\n", err);
        return 0;
    }
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 )
    {
        WSACleanup( );
        return 0;
    }

    // �����׽���
    SOCKET socketClient;
    if((socketClient = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
    {
        printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
        return 0;
    }

    // ���SOCKADDR_IN�ṹ
    SOCKADDR_IN addrServer;                                         // �������˽ṹ��
    addrServer.sin_family = AF_INET;                                // ��Ϊ AF_INET������WinSockʹ�õ��� IP ��ַ��
    addrServer.sin_port = htons(6000);                              // ����Ҫ����ͨѶ�Ķ˿ںţ�short���ͣ�htons()�����������˿ڱ����������ֽ�˳��ת��Ϊ�����ֽڵ�˳��
    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");       // ����ͨѶ�� IP ��ַ��Ϣ

    // �������������������
    if(connect(socketClient,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("CONNECT_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }
	else
	{
		printf("������! �������� IP: %s Port:%d ������ quit ���ɶϿ�����\n", inet_ntoa(addrServer.sin_addr), ntohs(addrServer.sin_port));
	}

    // ���߳�
    HANDLE hHandle[2];
    // _beginthread() �������ڴ���һ���̣߳����������ֱ�Ϊ���̺߳����ĵ�ַ�����̵߳Ķ�ջ��С����Ϊ0��ʾ�����߳�ʹ��һ���Ķ�ջ���������б�û�в���ʱΪNULL��
    hHandle[0]=(HANDLE)_beginthread(sendMsg,0,(void*)socketClient);
    if(QUIT)
    {
        closesocket(socketClient);
        WSACleanup();
        return 0;
    }
    hHandle[1]=(HANDLE)_beginthread(recvMsg,0,(void*)socketClient);

    // �ȴ������߳�ִ�����
    WaitForSingleObject(hHandle[0],INFINITE);

    // �ر��׽���
    closesocket(socketClient);

    // �رռ��ص��׽��ֿ�
    WSACleanup();

    return 0;
}

// ���ͺ���
void sendMsg(void* sock){

    SOCKET socketClient = (SOCKET)sock;

    char sendBuf[BUFFSIZE];

    while(true)
    {
        // ��������
        scanf("%[^\n]", sendBuf);
        getchar();
        if(strcmp(sendBuf, "quit") == 0)   // �˳�
        {
            send(socketClient, sendBuf, BUFFSIZE, 0);
            QUIT = true;
            break;
        }
        if(send(socketClient, sendBuf, BUFFSIZE, 0) == SOCKET_ERROR)
        {
            printf("��Ϣ����ʧ��!\n");
            break;
        }
        Sleep(200);     // �Ե�һ�ᣬ���ڽ���֮����Ⱥ�
    }
}

// ���պ���
void recvMsg(void* sock){

    SOCKET socketClient = (SOCKET)sock;

    char recvBuf[BUFFSIZE];
    int status = 0;

    while(true)
    {
        if(QUIT)
            break;
        // ��������
        status = recv(socketClient, recvBuf, BUFFSIZE, 0);
        if(status > 0)
        {
            if(strcmp(recvBuf, "quit") == 0)   // �˳�
            {
                QUIT = true;
                printf("�Է��Ͽ����ӣ�ͨ�Ž���\n");
                break;
            }
            printf("Server: %s\n", recvBuf);
        }
        else if(status < 0)
            printf("RECV_ERROR: %d\n",  SOCKET_ERROR);
        else
        {
            printf("�Ͽ����ӣ�ͨ�Ž���\n");
            break;
        }
    }
}
