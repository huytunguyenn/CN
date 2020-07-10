#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <sstream>  //
#include <fstream>
#include <vector>
#include <istream>
#include <streambuf> //
#include <iterator>
#pragma comment (lib,"ws2_32.lib")      // lien ket thu vien

using namespace std;

#define PORT 8080		
#define MAXBUFLEN 4096
#define MAXCLIENT 10

class Server {
protected:
    SOCKET listening;   // socket lang nghe
    sockaddr_in hint;   // ip va port 

public:
	Server();
	~Server();

    int initWinSock();      // khoi tao winsock                             tra ve: 0: thanh cong, 1: that bai
    int initSocket();       // khoi tao socket                              tra ve: 0: thanh cong, 1: that bai
    int init();             // khoi tao ca hai                              tra ve: 0: thanh cong, 1: that bai

    int Bind();             // ket buoc socket Server voi port va IP        tra ve: 0: thanh cong, 1: that bai
    int Listen();           // lang nghe tai PORT                           tra ve: 0: thanh cong, 1: that bai      
    void accept_sendClient();                                               // nhan thong diep tu client, sau do xu ly roi gui lai cho client  
    void handleClientRequest(char [MAXBUFLEN], string&, int&);              // xu ly thong diep client

    void closeSocket();     // dong socket lang nghe
    void closeWinSock();    // shutdown WinSock
    void close();           // dong ca hai
};

#endif
