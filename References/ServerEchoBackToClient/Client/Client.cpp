#include "Client.h"

int main() {
	string ipAddress = "127.0.0.1";		    // Dia chi IP cua Server

	// init winsock
    WSADATA data;                           
    WORD ver = MAKEWORD(2, 2);              

    int ret = WSAStartup(ver, &data);
    if (ret != 0) {
        cerr << "Khong the khoi tao winsock! ERROR:" << ret << endl;
        return 1;
    }

    // tao socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Khong the khoi tao socket! ERROR: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
	
    sockaddr_in hint = {};            
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr); 

	// ket noi toi server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        cerr << "Khong the ket noi toi server! ERROR: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

	// do-while loop de send va recv
    char buf[4096];
    string userInput;
    do {
        cout << "> ";
        getline(cin, userInput);
        // user nhap
        if (userInput.size() > 0) {
            // send 
            int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);   // ki tu /0 ket thuc
            if (sendResult != SOCKET_ERROR) {
                // wait for respone
                ZeroMemory(buf, sizeof(buf));
                int bytesReceived = recv(sock, buf, sizeof(buf), 0);
                if (bytesReceived > 0) {
                    // echo
                    cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
                }
            }
        }

    } while (userInput.size() > 0);


	// dong socket
    closesocket(sock);
    // tat winsock
    WSACleanup();
	return 0;
}