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

#define HOST "127.0.0.1"

#define HEADER "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=UTF-8\r\n\
Server: WebServer OptimizedBy Galaxy_Z\r\n\
Content-Length: %ld\r\n\r\n\
"

// ��ȡ�ļ��Ĵ�С
long GetFileLength(string strPath)
{
    // fstream���к�open()������ͬ�Ĺ��캯������ʵ������ʱ��Ϳ��Դ��ļ���fin�Ǳ���������fstream���һ��ʵ������
    ifstream fin(strPath, ios::in | ios::binary);               // ios::in �ļ������뷽ʽ��(�ļ��������뵽�ڴ�)��ios::binary �Զ����Ʒ�ʽ���ļ���ȱʡ�ķ�ʽ���ı���ʽ��| ��ʾ��
    fin.seekg(0, ios_base::end);                                // ����ָ��ָ���ļ�ĩβ��seekg()�������ö�λ�ã�seekp()��������дλ�ã���һ��������ƫ�������ڶ��������ǻ�׼λ��
    streampos pos = fin.tellg();                                 // ���ص�ǰ get ��ָ���λ�ã��˴��������ļ�βλ��
    long lSize = static_cast<long>(pos);
    fin.close();
    return lSize;
}

// ��ȡָ��Ŀ¼�µ������ļ��������䱣����������
void getFiles(string path, vector<string>& files)
{
    long   hFile = 0;
    struct _finddata_t fileinfo;    // �ļ���Ϣ������һ���洢�ļ���Ϣ�Ľṹ��
    string p;                       // �ַ��������·��                                 // fileinfo����������ļ���Ϣ�Ľṹ���ָ�룬_findfirst�����ɹ�ִ�к󣬺�������ҵ����ļ�����Ϣ��������ṹ����
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)    // string &assign(const string &s)���������ַ���s������ǰ�ַ���
    {                                                                                    // * ��ͨ�������һ�� \ ����ת�壬���磺D:\\test\\*�����ʾD�̵�test�ļ����ڵ������ļ���ʵ���ϵ�·����D:\test\*
        do
        {
            if ((fileinfo.attrib &  _A_SUBDIR))     // �����Ŀ¼,����֮�����ļ����ڻ����ļ��У�
            {
                //�ļ���������"."&&�ļ���������".."  "."��ʾ��ǰĿ¼  ".." ��ʾ��ǰĿ¼�ĸ�Ŀ¼���ж�ʱ�����߶�Ҫ���ԣ���ֹ��ѭ��
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        }
        while (_findnext(hFile, &fileinfo) == 0);   // ��ȡfileinfo����һ���ļ�����Ϣ
        _findclose(hFile);      // _findclose������������
    }
}

int main(int argc, char **argv)
{
    int PORT = 8000;
    string typePort;
    cout<<"��ָ�� WebServer �Ķ˿ںţ�";
    cin>>typePort;
    PORT=atoi(typePort.c_str());
    cout<<"WebServer���ʵ�ַ��"<<HOST<<":"<<PORT<<"\n"<<endl;

    string webServerPath = "",filename = "",loopChoose = "0";
    cout<<"��ָ�� WebServer �ĸ�Ŀ¼������ . ���뵱ǰĿ¼����";
    cin>>webServerPath;

    const char * filePath = webServerPath.c_str();       // c_str()������string�����ת���ɺ�C���Լ��ݵ�char *���͡�����Ϊ����c���Լ��ݣ���c������û��string���ͣ��ʱ���ͨ��string�����ĳ�Ա����c_str()��string ����ת����c�е��ַ�����ʽ
    vector<string> files;

    cout<<"\n��Ŀ¼�µ��ļ��У�"<<endl;
    getFiles(filePath, files);                            // ��ʾ��·���µ������ļ�
    for (int i = 0; i < files.size(); i++)
    {
        cout << files[i].c_str() << endl;
    }
    cout<<"\n";

    /*
    LOOP: cout<<"\n��ѡ����Ҫ��ȡ���ļ���";
    cin>>filename;

    cout<<"\n�Ƿ�ֻ��Ҫ��ȡ���ļ�������1��ʾ�ǣ�0��ʾ�񣩣�";
    cin>>loopChoose;

    string filePathAndName = filePath;
    filePathAndName.append("\\").append(filename);

    // ��ȡ�ļ��Ĵ�С
    cout<<"\n���ļ��Ĵ�СΪ��"<< GetFileLength(filePathAndName) <<"�ֽ�\n"<<endl;
    */

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
    SOCKET socketServer;
    if((socketServer = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)   // INVALID_SOCKET��˼����Ч�׽���
    {
        printf("SOCKET_ERROR: %d\n", INVALID_SOCKET);
        return 0;
    }

    // ���SOCKADDR_IN�ṹ
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
    addrServer.sin_port = htons(PORT);

    // ���׽��ֵ�һ�� IP ��ַ��һ���˿���
    if(bind(socketServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("BIND_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    // ���׽�������Ϊ����ģʽ�ȴ��������󣬴˴�ʹ��10��backlog���ò���ָ��������ɶ��еĳ��ȣ��ο����ӣ�https://zhuanlan.zhihu.com/p/78879634
    if(listen(socketServer,10) == SOCKET_ERROR)
    {
        printf("LISTEN_ERROR: %d\n", SOCKET_ERROR);
        return 0;
    }

    SOCKADDR_IN addrClient;                                 // �ͻ��˽ṹ��
    int len = sizeof(SOCKADDR),status = 0;;

    while(true)
    {
        printf("    WebServer������...\n\n");

        // �������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽���
        SOCKET socketConn;
        // ����ͻ���û����������ô����һֱͣ���ڸú�����
        if((socketConn = accept(socketServer,(SOCKADDR*)&addrClient,&len)) == INVALID_SOCKET)   // &addrClient �ǻ�������ַ�������˿ͻ��˵�IP�Ͷ˿ڵ���Ϣ
        {
            printf("ACCPET_ERROR: %d\n", INVALID_SOCKET);
            closesocket(socketConn);
            return 0;
        }

        // �÷��ص��׽��ֺͿͻ��˽���ͨ��
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        // �������ݣ����ͻ��˷��͵�������
        status = recv(socketConn, buffer, BUFFER_SIZE, 0);
        if(status > 0)
            printf("���յ���HTTP�����ģ�\n%s", buffer);
        else if(status < 0)
        {
            printf("RECV_ERROR: %d\n",  SOCKET_ERROR);
            break;
        }

        int endpos;
        string filePathAndName = filePath,RequestHeader = buffer;
        endpos = RequestHeader.find(" HTTP/1.1");
        filePathAndName.append("\\").append(RequestHeader,5,endpos-4);     // "GET /"ռ4λ
        cout<<"<==========    �ͻ���������ļ���"<<filePathAndName<<"  �ļ���С��"<<GetFileLength(filePathAndName)<<"�ֽ�    ==========>\n"<<endl;

        // �������ݣ���WebServer���͵���Ӧ����
        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, HEADER, GetFileLength(filePathAndName));       // ��� WebServer ����Ӧͷ�еĳ����ֶΣ�����Ӧͷ����buffer������
        if(send(socketConn, buffer, strlen(buffer), 0) == SOCKET_ERROR)
        {
            printf("HTTP��Ӧ������ʧ��!\n");
            break;
        }
        else
        {
            printf("���͵�HTTP��Ӧ���ģ�\n%s", buffer);
        }

        // ��ȡ�ͻ���������ļ�������
        ifstream fin(filePathAndName, ios::in | ios::binary);
        if (fin.is_open())
        {
            memset(buffer, 0, BUFFER_SIZE);
            while (fin.read(buffer, BUFFER_SIZE - 1))           // read()��������ֵΪʵ�ʶ�ȡ���ֽ������ڶ�������Ϊÿ�ζ�ȡ�ĳ���
            {
                if(send(socketConn, buffer, strlen(buffer), 0) == SOCKET_ERROR)
                {
                    printf("�ͻ���������ļ�����ʧ��!\n");
                    break;
                }
                //printf("���͵��ļ����ݣ�\n%s\n\n", buffer);
                memset(buffer, 0, BUFFER_SIZE);
            }
            if(send(socketConn, buffer, strlen(buffer), 0) == SOCKET_ERROR)
            {
                printf("�ͻ���������ļ�����ʧ��!\n");
                break;
            }
            //printf("���͵��ļ����ݣ�\n%s\n\n", buffer);
        }
        fin.close();

        // �ر��׽���
        closesocket(socketConn);

        /*
        if(!loopChoose.compare("0"))         // 0������ѡ���ļ�
        {
            closesocket(socketServer);
            WSACleanup();
            goto LOOP;
        }
        */
    }

    closesocket(socketServer);
    WSACleanup();

    return 0;
}
