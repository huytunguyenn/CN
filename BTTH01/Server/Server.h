#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#pragma comment (lib,"ws2_32.lib")      // lien ket thu vien

using namespace std;

#define PORT 54000		
#define MAXBUFLEN 4096

class Server {
protected:
    SOCKET listening;   // socket lang nghe
    sockaddr_in hint;   // ip va port 

public:
	Server();
	~Server();

    int InitWinSock();      // khoi tao winsock                             tra ve: 0: thanh cong, 1: that bai
    int InitSocket();       // khoi tao socket                              tra ve: 0: thanh cong, 1: that bai
    int Init();             // khoi tao ca hai                              tra ve: 0: thanh cong, 1: that bai

    int Bind();             // ket buoc socket Server voi port va IP        tra ve: 0: thanh cong, 1: that bai
    int Listen();           // lang nghe tai PORT                           tra ve: 0: thanh cong, 1: that bai      
    void AcceptAndSend();   // 
  

    void CloseSocket();     // dong socket lang nghe
    void CloseWinSock();    // shutdown WinSock
    void Close();           // dong ca hai
};

#endif
