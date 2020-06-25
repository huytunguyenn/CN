#include "Server.h"

Server::Server() {

}

Server::~Server() {

}

int Server::initWinSock() {
	WSADATA wsData;                     // winsock startup data
	WORD ver = MAKEWORD(2, 2);          // su dung phien ban 2.2 (0x0202)

	int ret = WSAStartup(ver, &wsData); // ret = return value
	if (ret != 0) {
		cerr << "Khong the khoi tao winsock! ERROR:" << ret << endl;
		return 1;
	}
	return 0;
}

int Server::initSocket() {
	this->listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Khong the khoi tao socket! ERROR: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	return 0;
}

int Server::init() {
	if (this->initWinSock())
		return 1;
	if (this->initSocket())
		return 1;
	return 0;
}

int Server::Bind() {
	//this->hint = {};
	this->hint.sin_family = AF_INET;
	this->hint.sin_port = htons(PORT);
	this->hint.sin_addr.S_un.S_addr = INADDR_ANY;		// chon IP cua chinh server, dung inet_pton cung duoc

	int ret = bind(this->listening, (sockaddr*)&(this->hint), sizeof(this->hint));
	if (ret == SOCKET_ERROR) {
		cerr << "Khong the bind socket! ERROR: " << WSAGetLastError() << endl;
		this->close();
		return 1;
	}
	return 0;
}

int Server::Listen() {
	int ret = listen(listening, MAXCLIENT);

	if (ret == SOCKET_ERROR) {
		cerr << "Khong the lang nghe socket! ERROR: " << WSAGetLastError() << endl;
		this->close();
		return 1;
	}
	return 0;
}

void Server::accept_sendClient() {
	sockaddr_in client = {};            // ip va port cua client
	int clientSize = sizeof(client);    // tham so ham accept
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Khong the accept client! ERROR: " << WSAGetLastError() << endl;
		this->close();
		return;
	}

	char host[NI_MAXHOST] = {};			// client's remote name
	char service[NI_MAXSERV] = {};		// service (port) the client is connect on
	//ZeroMemory(host, NI_MAXHOST);		// = memset(host,0,NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);
	int ret = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, NULL, 0, 0);
	if (ret != 0) {
		cerr << "Khong the lay ten client! ERROR: " << ret << endl;
		inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);      // ko lay dc ten client -> chuyen IPv4 thanh dang text, dat thanh ten cho client
	}
	cout << "Client: " << host << ", Connected on port: " << ntohs(client.sin_port) << endl;


	// header
	char arrHeaders[] = "HTTP/1.1 200 OK\r\n"
		"Server: HUYTU-PC Web Server\r\n"
		"Date: Fri, 21 June 2020\r\n"
		"Content-Type: text/html\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 187\r\n"
		"\r\n";

	// body
	char arrData[] = "<html>"
		"<head><title>18120254-18120264</title></head> \r\n"
		"<body>\r\n"
		"<h1>HELLO WORLD !</h1>\r\n"
		"<br>\r\n"
		"4 tien phong 4 bi an ham loz vc<br> \r\n"
		"18120254@student.hcmus.edu.vn\r\n"
		"</body>\r\n"
		"</html>\r\n\n";


	char buf[MAXBUFLEN];
	int bytesReceived;
	while (true) {
		cout << endl << "Wait for client to send data..." << endl;

		ZeroMemory(buf, MAXBUFLEN);
		bytesReceived = recv(clientSocket, buf, sizeof(buf), 0);

		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Khong doc duoc client! ERROR: " << WSAGetLastError() << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client Disconnected" << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived) << endl;

		// send to client
		if (send(clientSocket, arrHeaders, sizeof(arrHeaders), 0) != SOCKET_ERROR) {
			if (send(clientSocket, arrData, sizeof(arrData), 0) != SOCKET_ERROR) {
			}
		}
		else {
			cerr << "Khong gui duoc toi client! Error: " << WSAGetLastError() << endl;
			//break;
		}
	}
	closesocket(clientSocket);
}



void Server::closeSocket() {
	closesocket(this->listening);
}

void Server::closeWinSock() {
	WSACleanup();
}

void Server::close() {
	this->closeSocket();
	this->closeWinSock();
}
