#pragma comment(lib,"ws2_32")

#include <stdio.h>
#include <winsock2.h>

#define BUFFSIZE 1024

int main()
{
    WORD wVersionRequested;                                 // ˫�ֽ�
    WSADATA wsaData;                                        // WinSock ��汾�������Ϣ
    wVersionRequested = MAKEWORD( 1, 1 );                   // ʹ�õ� WinSock �İ汾

    // �����׽��ֿ�
    int err = WSAStartup( wVersionRequested, &wsaData );    // ���� WinSock �Ⲣȷ�� WinSock �汾��ϵͳ����������� wsaData ��
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

    // �����׽���,socket()�����������ֱ�Ϊ AF_INET����ʾʹ�õ��� TCP/IP ��ַ�壩��Socket���ͣ���ʽ�����ݱ��������ض��ĵ�ַ����ص�Э�飨��ָ��Ϊ0��ϵͳ���Զ�ѡ��һ�����ʵ�Э�飩
    SOCKET socketServer;
    if((socketServer = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
    {
        printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
        return 0;
    }

    // ���SOCKADDR_IN�ṹ
    SOCKADDR_IN addrServer;                                   // �������˽ṹ��
    addrServer.sin_family = AF_INET;                          // ��Ϊ AF_INET������WinSockʹ�õ��� IP ��ַ��
    addrServer.sin_port = htons(6000);                        // ����Ҫ����ͨѶ�Ķ˿ںţ�short���ͣ�htons()�����������˿ڱ����������ֽ�˳��ת��Ϊ�����ֽڵ�˳��
    addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);      // ����ͨѶ�� IP ��ַ��Ϣ��long���ͣ� htonl()��������ͬ��,INADDR_ANY ����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ,Ҳ���Ǳ�ʾ����������IP

    // ���׽��ֵ�һ�� IP ��ַ��һ���˿���
    if(bind(socketServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("BIND_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    // ���׽�������Ϊ����ģʽ�ȴ��������󣬴˴�ʹ��5��backlog���ò���ָ��������ɶ��еĳ��ȣ��ο����ӣ�https://zhuanlan.zhihu.com/p/78879634
    if(listen(socketServer,5) == SOCKET_ERROR)
    {
        printf("LISTEN_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    SOCKADDR_IN addrClient;                                 // �ͻ��˽ṹ��
    int len = sizeof(SOCKADDR);

    printf("������...\n");
    // �������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽���
    SOCKET socketConn;
    // ����ͻ���û����������ô����һֱͣ���ڸú�����
    if((socketConn = accept(socketServer,(SOCKADDR*)&addrClient,&len)) == INVALID_SOCKET)   // &addrClient �ǻ�������ַ�������˿ͻ��˵�IP�Ͷ˿ڵ���Ϣ
    {
        printf("ACCPET_ERROR: %d\n", INVALID_SOCKET);
        closesocket(socketConn);
        return 0;
    }
    printf("������! �ͻ��� IP: %s Port:%d ������ quit ���ɶϿ�����\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

    // �÷��ص��׽��ֺͿͻ��˽���ͨ��
    char recvBuf[BUFFSIZE], sendBuf[BUFFSIZE];
    int status = 0;
    while(true)
    {

        // ��������
        status = recv(socketConn, recvBuf, BUFFSIZE, 0);
        if(status > 0)
            printf("Client: %s\n", recvBuf);
        else if(status < 0)
            printf("RECV_ERROR: %d\n",  SOCKET_ERROR);
        else
        {
            printf("�Է��Ͽ����ӣ�ͨ�Ž���\n");
            break;
        }

        // ��������
        printf("Server: ");
        scanf("%[^\n]", sendBuf);
        getchar();
        if(strcmp(sendBuf, "quit") == 0)   // �˳�
            break;
        if(send(socketConn, sendBuf, BUFFSIZE, 0) == SOCKET_ERROR)
        {
            printf("��Ϣ����ʧ��!\n");
            break;
        }
    }

    // �ر��׽���
    closesocket(socketConn);
    closesocket(socketServer);

    // �رռ��ص��׽��ֿ�
    WSACleanup();

    return 0;
}
