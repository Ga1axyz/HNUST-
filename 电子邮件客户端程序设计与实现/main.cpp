#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <stdio.h>
#include "unistd.h"

#define BUFFSIZE 2048

using namespace std;

// ���� Base64 ���ݸ�ʽ
struct Base64Date6
{
    unsigned int d4:6;
    unsigned int d3:6;
    unsigned int d2:6;
    unsigned int d1:6;
};

// �������ƴ���ת���� ASCII ��
char ConvertToBase64(char uc)
{
    if(uc <26)
    {
        return'A'+uc;
    }
    if(uc <52)
    {
        return'a'+(uc -26);
    }
    if(uc <62)
    {
        return'0'+(uc -52);
    }
    if(uc ==62)
    {
        return'+';
    }
    if(uc ==63)
    {
        return '/';
    }
}

// Base64 ��ʵ��
void  EncodeBase64(char*dbuf,char*buf128,int len)
{
    struct  Base64Date6*ddd =NULL;
    int i =0;
    char buf[256]= {0};                     // ����uf�����ֵȫ����ʼ��Ϊ0
    char *tmp =NULL;
    char cc ='\0';                          // ��Ӧ��ASCIIֵΪ0�����ַ��������ı�־
    memset(buf,0,256);                      // ��ʼ�������������ǽ�ĳһ���ڴ��е�����ȫ������Ϊָ����ֵ��memset()����ͨ��Ϊ��������ڴ�����ʼ��������
    strcpy(buf,buf128);                     // �Ѵ�buf128��ַ��ʼ�Һ���NULL���������ַ������Ƶ���dest��ʼ�ĵ�ַ�ռ�
    for(i =1; i <=len/3; i++)
    {
        tmp =buf +(i-1)*3;
        cc =tmp[2];
        tmp[2]=tmp[0];
        tmp[0]=cc;
        ddd =(struct Base64Date6*)tmp;
        dbuf[(i-1)*4+0]=ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1]=ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2]=ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i-1)*4+3]=ConvertToBase64((unsigned int)ddd->d4);
    }
    if(len %3==1)
    {
        tmp =buf +(i-1)*3;
        cc =tmp[2];
        tmp[2]=tmp[0];
        tmp[0]=cc;
        ddd =(struct Base64Date6*)tmp;
        dbuf[(i-1)*4+0]=ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1]=ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2]='=';
        dbuf[(i-1)*4+3]='=';
    }
    if(len%3==2)
    {
        tmp =buf+(i-1)*3;
        cc =tmp[2];
        tmp[2]=tmp[0];
        tmp[0]=cc;
        ddd =(struct Base64Date6*)tmp;
        dbuf[(i-1)*4+0]=ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i-1)*4+1]=ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i-1)*4+2]=ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i-1)*4+3]='=';
    }
    return;
}

int main()
{
    string message, info, mail, subject, imail, icode;

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

    HOSTENT* pHostent;                                      // hostent��host entry����д���ýṹ��¼��������Ϣ����������������������ַ���͡���ַ���Ⱥ͵�ַ�б�

    cout<<"�����뷢��/��ѯ�����䣺";
    cin>>imail;
    cout<<"������������Ȩ�룺";          // yffzdykoufhfeabg
    cin>>icode;

    while (true)
    {
        char buff[BUFFSIZE], temp[BUFFSIZE];                                      // ���建����

        cout << "\n��ѡ�����1.�����ʼ� 2.�鿴����  0.�˳���";
        int call;
        cin >> call;
        if (call == 1)
        {
            // �����׽���
            SOCKET sockClient;
            if((sockClient = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
            {
                printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
                return 0;
            }

            HOSTENT* pHostent;                                                   // hostent��host entry����д���ýṹ��¼��������Ϣ����������������������ַ���͡���ַ���Ⱥ͵�ַ�б�
            pHostent = gethostbyname("smtp.qq.com");                             // ���qq����������ĵ�ַ��Ϣ

            // ���SOCKADDR_IN�ṹ
            sockaddr_in addrServer;
            addrServer.sin_family = AF_INET;
            addrServer.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]);          // �õ� SMTP �������������ֽ����ip��ַ
            addrServer.sin_port = htons(25);

            // �������������������
            if(connect(sockClient,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
            {
                printf("CONNECT_ERROR: %d\n", SOCKET_ERROR);
                return 0;
            }
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';                               // ���շ���ֵ

            // ��¼�ʼ�������
            message = "ehlo qq.com\r\n";
            send(sockClient, message.c_str(), message.length(), 0);                         // EHLO
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            message = "auth login\r\n";
            send(sockClient, message.c_str(), message.length(), 0);                         // AUTH LOGIN
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            memset(buff, 0, BUFFSIZE);
            memset(temp, 0, BUFFSIZE);

            for(int i = 0; i < sizeof(imail); i++)
                buff[i] = imail[i];
            EncodeBase64(temp,buff,strlen(buff));                                           // ����������
            sprintf(buff, "%s\r\n", temp);
            send(sockClient, buff, strlen(buff), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            memset(buff, 0, BUFFSIZE);
            memset(temp, 0, BUFFSIZE);

            for(int i = 0; i < sizeof(icode); i++)
                buff[i] = icode[i];
            EncodeBase64(temp,buff,strlen(buff));                                           // ������Ȩ��
            sprintf(buff, "%s\r\n", temp);
            send(sockClient, buff, strlen(buff), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            memset(buff, 0, BUFFSIZE);
            memset(temp, 0, BUFFSIZE);

            cout<<"\n�������ռ������䣺";
            cin >> mail;
            message = "MAIL FROM:<";
            message.append(imail);
            message.append(">\r\nRCPT TO:<");
            message.append(mail);
            message.append("> \r\n");
            send(sockClient, message.c_str(), message.length(), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            // �ʼ�����
            message = "DATA\r\n";
            send(sockClient, message.c_str(), message.length(), 0);                         // DATA
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            message = "From: ";
            message.append(imail);
            message.append("\r\n\To:");
            message.append(mail);
            message.append("\r\n\subject:");

            getchar();
            cout << "�������ʼ����⣺";
            getline(cin,subject);
            message.append(subject);
            message.append("\r\n\r\n");
            cout << "�������ʼ����ݣ�";
            getline(cin,info);
            message.append(info);
            message.append("\r\n.\r\n");
            send(sockClient, message.c_str(), message.length(), 0);

            message = "QUIT\r\n";
            send(sockClient, message.c_str(), message.length(), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            cout << "�ʼ����ͳɹ���" << endl;
        }
        if (call == 2)
        {
            // �����׽���
            SOCKET sockClient;
            if((sockClient = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
            {
                printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
                return 0;
            }

            HOSTENT* pHostent;
            pHostent = gethostbyname("pop.qq.com");

            // ���SOCKADDR_IN�ṹ
            sockaddr_in addrServer;
            addrServer.sin_family = AF_INET;
            addrServer.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]);          // �õ� POP3 �������������ֽ����ip��ַ
            addrServer.sin_port = htons(110);

            // �������������������
            if(connect(sockClient,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
            {
                printf("CONNECT_ERROR: %d\n", SOCKET_ERROR);
                return 0;
            }
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';                               // ���շ���ֵ

            // ��ѯ���˻�
            message = "user ";
            message.append(imail);
            message.append("\r\n");
            send(sockClient, message.c_str(), message.length(), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            memset(buff, 0, BUFFSIZE);
            memset(temp, 0, BUFFSIZE);

            // ��ѯ��������Ȩ��
            message = "pass ";
            message.append(icode);
            message.append("\r\n");
            send(sockClient, message.c_str(), message.length(), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            memset(buff, 0, BUFFSIZE);
            memset(temp, 0, BUFFSIZE);

            // �����ʼ�������ÿ���ʼ��Ĵ�С
            message = "list\r\n";
            send(sockClient, message.c_str(), message.length(), 0);
            buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';

            cout << "\n�ʼ��б� - �ʼ���С��" << buff << endl;
            while (1)
            {
                int num;
                cout << "��������Ŷ�ȡ��Ӧ�ʼ������� 0 �˳���";
                cin >> num;

                if (num == 0)
                    break;

                message = "retr ";
                message.append(to_string(num));
                message.append("\r\n");
                send(sockClient, message.c_str(), message.length(), 0);

                buff[recv(sockClient, buff, BUFFSIZE, 0)] = '\0';
                cout << "���ʼ�����Ϊ��" << buff << endl;

                memset(buff, 0, BUFFSIZE);
            }
        }
        if (call == 0)
        {
            return 0;
        }
    }

    return 0;
}
