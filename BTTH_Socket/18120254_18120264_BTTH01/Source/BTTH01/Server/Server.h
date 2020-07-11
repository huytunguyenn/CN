#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <sstream>  
#include <fstream>
#include <vector>
#include <istream>
#include <iterator>
#pragma comment (lib,"ws2_32.lib")      // liên kết thư viện

using namespace std;

#define PORT 8080		
#define MAXBUFLEN 4096
#define MAXCLIENT 20

class Server {
protected:
    SOCKET listening;   // socket lắng nghe
    sockaddr_in hint;   // chứa địa chỉ ip và port 

public:
	Server();
	~Server();

    int initWinSock();      // khởi tạo winsock                             trả về: 0: thành công, 1: thất bại
    int initSocket();       // khởi tạo socket                              trả về: 0: thành công, 1: thất bại
    int init();             // khởi tạo cả hai                              trả về: 0: thành công, 1: thất bại

    int Bind();             // kết buộc socket Server với port và đ/c ip    trả về: 0: thành công, 1: thất bại
    int Listen();           // lắng nghe tại PORT                           trả về: 0: thành công, 1: thất bại      
    void accept_sendClient();                                            // nhận thông điệp từ client, sau đó xử lý, rồi gửi lại cho client  
    void handleClientRequest(char [MAXBUFLEN], string&, int&);           // xử lý thông điệp client           truyền vào: nội dung nhận từ client, nội dung sẽ gửi đến client, độ dài tham số thứ 2

    void closeSocket();     // đóng socket lắng nghe
    void closeWinSock();    // shutdown WinSock
    void close();           // đóng cả hai
};

#endif
