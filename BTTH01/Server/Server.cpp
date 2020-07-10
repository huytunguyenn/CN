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
	this->hint.sin_family = AF_INET;
	this->hint.sin_port = htons(PORT);
	this->hint.sin_addr.S_un.S_addr = INADDR_ANY;		// chon IP cua chinh server (127.0.0.1), dung inet_pton cung duoc

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
	// accept client ket noi toi
	sockaddr_in client = {};            // ip va port cua client
	int clientSize = sizeof(client);    // tham so ham accept
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Khong the accept client! ERROR: " << WSAGetLastError() << endl;
		this->close();
		return;
	}

	// lay thong tin client
	char host[NI_MAXHOST] = {};			// ten client
	char service[NI_MAXSERV] = {};		// service (port) client dang ket noi
	int ret = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, NULL, 0, 0);
	if (ret != 0) {
		cerr << "Khong the lay ten client! ERROR: " << ret << endl;
		inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);      // ko lay dc ten client -> chuyen IPv4 thanh dang text, dat thanh ten cho client
	}
	cout << "Client: " << host << ", Connected on port: " << ntohs(client.sin_port) << endl;


	// nhan thong diep client gui den
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

		// in ra thong diep cua client
		cout << string(buf, 0, bytesReceived) << endl;

		// xu ly yeu cau cua client
		string output;
		int size;
		this->handleClientRequest(buf, output, size);

		// send noi dung client yeu cau
		if (send(clientSocket, output.c_str(), size, 0) == SOCKET_ERROR) {
			cerr << "Khong gui duoc toi client! Error: " << WSAGetLastError() << endl;
			break;
		}
	}
	closesocket(clientSocket);
}

void Server::handleClientRequest(char buf[MAXBUFLEN], string& output, int& size) {
	// phan tich yeu cau client (e.g: GET /index.html HTTP/1.1)
	istringstream iss(buf);
	vector<string> parsed((istream_iterator<string>(iss)), istream_iterator<string>());

	
	// neu client yeu cau file khong ton tai thi in ra 404
	string content = "<h1>404 Not Found</h1>";
	int errorCode = 404;


	// html mac dinh la index.html
	string htmlFile = "/index.html";


	// xu ly HTTP Request
	if (parsed.size() >= 3 && parsed[0] == "GET") {// neu la GET 
		// html client yeu cau
		htmlFile = parsed[1];
		// client khong yeu cau file html thi mac dinh la index.html
		if (htmlFile == "/") {
			htmlFile = "/index.html";
		}
	}
	else if (parsed.size() >= 3 && parsed[0] == "POST") {// neu la POST 
		// xu ly check u_name=admin&pass=admin


	}


	// doc file html
	ifstream f(".\\html" + htmlFile);				
	if (f.good()) {
		string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		content = str;
		errorCode = 200;
	}
	f.close();


	// khoi tao message gui cho client
	ostringstream oss;
	// header
	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
	oss << "Server: HUYTU-PC Web Server\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "\r\n";
	// body (noi dung html)
	oss << content;

	output = oss.str();
	size = output.size() + 1;
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
