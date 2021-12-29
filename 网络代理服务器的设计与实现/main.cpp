#pragma comment(lib, "WS2_32")

#include <cstdio>
#include <string>
#include <fstream>
#include <WinSock2.h>
#include <iostream>
#include <io.h>
#include <vector>

using namespace std;

#define BUFFER_SIZE 1024

#define PORT 2021

#define HOST "127.0.0.1"

#define ServerPORT 8000

#define ServerHOST "127.0.0.1"

#define HEADER "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=UTF-8\r\n\
Server: WebProxy DesignedBy Galaxy_Z\r\n\
Content-Length: %ld\r\n\r\n\
"

int main(int argc, char **argv)
{

    WORD wVersionRequested;                                 // ˫�ֽ�
    WSADATA wsaData;                                        // WinSock ��汾�������Ϣ
    wVersionRequested = MAKEWORD( 2, 2 );                   // ʹ�õ� WinSock �İ汾

    // �����׽��ֿ�
    int err = WSAStartup( wVersionRequested, &wsaData );    // ���� WinSock �Ⲣȷ�� WinSock �汾��ϵͳ����������� wsaData ��
    if ( err != 0 )
    {
        printf("WSASTARTUP_ERROR: %d\n", err);
        return 0;
    }

    // �����׽���
    SOCKET socketProxy;
    if((socketProxy = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
    {
        printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
        return 0;
    }

    // proxy��
    sockaddr_in addrProxy;
    addrProxy.sin_family = AF_INET;
    addrProxy.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrProxy.sin_port = htons(PORT);

    // server��
    SOCKADDR_IN addrServer;                                        // �������˽ṹ��
    addrServer.sin_family = AF_INET;                               // ��Ϊ AF_INET������WinSockʹ�õ��� IP ��ַ��
    addrServer.sin_port = htons(ServerPORT);                       // ����Ҫ����ͨѶ�Ķ˿ںţ�short���ͣ�htons()�����������˿ڱ����������ֽ�˳��ת��Ϊ�����ֽڵ�˳��
    addrServer.sin_addr.S_un.S_addr = inet_addr(ServerHOST);       // ����ͨѶ�� IP ��ַ��Ϣ

    // ���׽��ֵ�һ�� IP ��ַ��һ���˿���
    if(bind(socketProxy,(SOCKADDR*)&addrProxy,sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("BIND_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    // ���׽�������Ϊ����ģʽ�ȴ��������󣬴˴�ʹ��10��backlog���ò���ָ��������ɶ��еĳ��ȣ��ο����ӣ�https://zhuanlan.zhihu.com/p/78879634
    if(listen(socketProxy,10) == SOCKET_ERROR)
    {
        printf("LISTEN_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    SOCKADDR_IN addrClient;                                 // �ͻ��˽ṹ��
    int len = sizeof(SOCKADDR),status = 0;;

    while(true)
    {
        printf("    WebProxy������...\n\n");

        // �������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽���
        SOCKET socketConn;
        // ����ͻ���û����������ô����һֱͣ���ڸú�����
        if((socketConn = accept(socketProxy,(SOCKADDR*)&addrClient,&len)) == INVALID_SOCKET)   // &addrClient �ǻ�������ַ�������˿ͻ��˵�IP�Ͷ˿ڵ���Ϣ
        {
            printf("ACCPET_ERROR: %d\n", INVALID_SOCKET);
            closesocket(socketConn);
            return 0;
        }

        // �÷��ص��׽��ֺͿͻ��˽���ͨ��
        char buffer[BUFFER_SIZE], recvBuf[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        memset(recvBuf, 0, BUFFER_SIZE);

        // �������ݣ����ͻ��˷��͵�HTTP������
        status = recv(socketConn, buffer, BUFFER_SIZE, 0);
        if(status > 0)
            printf("���յ��Ŀͻ���HTTP�����ģ�\n%s", buffer);
        else if(status < 0)
        {
            printf("RECV_ERROR: %d\n",  SOCKET_ERROR);
            break;
        }

        // �޸�HTTP�������е�HOST
        int startpos, endpos, i;
        char portTemp[20];
        string RequestHeader = buffer, newHost;
        startpos = RequestHeader.find("Host: ");                                        // ��ȡԭHOSTֵ�ֶε���ʼ����
        endpos = RequestHeader.find("Connection:");
        itoa(ServerPORT,portTemp,10);                                                   // int ת string
        newHost.append(ServerHOST).append(":").append(portTemp).append("\n");           // ƴ�ӵõ���HOSTֵ
        RequestHeader.replace(startpos+6,endpos-startpos-6,newHost);                   // ��Ŀ�ĵ�ַ�滻Ϊ���Ŀ��������ĵ�ַ
        for(i=0;i<RequestHeader.length();i++)
            buffer[i] = RequestHeader[i];
        buffer[i] = '\0';

        // �����׽���
        SOCKET socketProxySend;
        if((socketProxySend = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
        {
            printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
            return 0;
        }

        // �������������������
        if(connect(socketProxySend,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
            printf("CONNECT_ERROR: %d\n", SOCKET_ERROR);
            return 0;
        }
        else
        {
            printf("�����ӵ���������! ���Ŀ������� IP: %s Port:%d \n\n", inet_ntoa(addrServer.sin_addr), ntohs(addrServer.sin_port));
        }

        // ת�����ݣ����ͻ��˷��͵�HTTP������ת�������Ŀ�������
        if(send(socketProxySend, buffer, strlen(buffer), 0) == SOCKET_ERROR)
        {
            printf("�ͻ���HTTP������ת��ʧ��!\n");
            break;
        }
        else
        {
            printf("ת���Ŀͻ���HTTP�����ģ�\n%s", buffer);
        }

        // �������ݣ������Ŀ����������ص�����
        status = recv(socketProxySend, recvBuf, BUFFER_SIZE, 0);
        if(status > 0)
            printf("���յ������Ŀ�������HTTP��Ӧ���ģ�\n%s\n\n", recvBuf);
        else if(status < 0)
            printf("RECV_ERROR: %d\n",  SOCKET_ERROR);

        // ת�����ݣ������Ŀ����������ص�����ת�����ͻ���
        if(send(socketConn, recvBuf, strlen(recvBuf), 0) == SOCKET_ERROR)
        {
            printf("���Ŀ�������HTTP��Ӧ����ת��ʧ��!\n");
            break;
        }
        else
        {
            printf("ת�������Ŀ�������HTTP��Ӧ���ģ�\n%s\n\n", recvBuf);
        }
        memset(recvBuf, 0, BUFFER_SIZE);

        // �������ղ�ת�����ݣ������Ŀ����������ص���ҳ
        while(recv(socketProxySend, recvBuf, BUFFER_SIZE - 1, 0))
        {
            if(send(socketConn, recvBuf, strlen(recvBuf), 0) == SOCKET_ERROR)
            {
                printf("��ҳ����ת��ʧ��!\n");
                break;
            }
            //printf("ת�����ļ����ݣ�\n%s\n\n", recvBuf);
            memset(recvBuf, 0, BUFFER_SIZE);
        }

        // �ر��׽���
        closesocket(socketProxySend);
        closesocket(socketConn);
    }

    closesocket(socketProxy);
    WSACleanup();

    return 0;
}
