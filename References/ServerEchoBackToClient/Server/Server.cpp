#include "Server.h"

int main() {

	// init winsock
	WSADATA wsData;                     // winsock startup data
	WORD ver = MAKEWORD(2, 2);          // su dung phien ban 2.2 (0x0202)

	int ret = WSAStartup(ver, &wsData);
	if (ret != 0) {
		cerr << "Khong the khoi tao winsock! ERROR:" << ret << endl;
		return 1;
	}

	// tao socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Khong the khoi tao socket! ERROR: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// bind 
	sockaddr_in hint = {};						
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;		// chon IP cua chinh server, dung inet_pton cung duoc

	ret = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (ret == SOCKET_ERROR) {
		cerr << "Khong the bind socket! ERROR: " << WSAGetLastError() << endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	}


	// listen
	ret = listen(listening, SOMAXCONN); 

	if (ret == SOCKET_ERROR) {
		cerr << "Khong the lang nghe socket! ERROR: " << WSAGetLastError() << endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	}

	// doi client ket noi...

	sockaddr_in client = {};            // ip va port cua client
	int clientSize = sizeof(client);    // tham so ham accept

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Khong the accept client! ERROR: " << WSAGetLastError() << endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	}

	char host[NI_MAXHOST] = {};			// client's remote name
	char service[NI_MAXSERV] = {};		// service (port) the client is connect on

	//ZeroMemory(host, NI_MAXHOST); // = memset(host,0,NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	ret = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, NULL, 0, 0);
	if (ret != 0) {
		cerr << "Khong the lay ten client! ERROR: " << ret << endl;
		inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);      // ko lay dc ten client -> chuyen IPv4 thanh dang text, dat thanh ten cho client
	}

	cout << "Client: " << host << ", Connected on port: " << ntohs(client.sin_port) << endl;

	// dong socket lang nghe
	closesocket(listening);

	// loop: nhan va nghe thong diep
	char buf[4096];
	int bytesReceived;
	while (true)
	{
		// Wait for client to send data
		bytesReceived = recv(clientSocket, buf, sizeof(buf), 0);

		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Khong doc duoc client! ERROR: " << WSAGetLastError() << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client Disconnected" << endl;
			break;
		}

		cout << string(buf,0,bytesReceived)<<endl;

		// Echo back to client
		ret = send(clientSocket, buf, bytesReceived +1, 0);
		if (ret == SOCKET_ERROR) {
		    cerr << "Khong gui duoc toi client! Error: "<< WSAGetLastError()<<endl;
		    break;
		}
	}
	// dong socket
	closesocket(clientSocket);
	// tat winsock
	WSACleanup();
	return 0;
}