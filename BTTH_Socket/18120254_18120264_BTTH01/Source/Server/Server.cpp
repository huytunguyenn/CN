#include "Server.h"

Server::Server() {

}

Server::~Server() {

}

int Server::initWinSock() {
	WSADATA wsData;                     // winsock startup data
	WORD ver = MAKEWORD(2, 2);          // sử dụng phiên bản 2.2 (0x0202)

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
	this->hint.sin_addr.S_un.S_addr = INADDR_ANY;		// chọn IP của chính server (127.0.0.1), dùng inet_pton cũng được

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
	// accept client kết nối tới
	sockaddr_in client = {};            // ip và port của client
	int clientSize = sizeof(client);    // tham số hàm accept
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Khong the accept client! ERROR: " << WSAGetLastError() << endl;
		this->close();
		return;
	}

	// lấy thông tin client
	char host[NI_MAXHOST] = {};			// tên client
	char service[NI_MAXSERV] = {};		// service (port) client đang kết nối
	int ret = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, NULL, 0, 0);
	if (ret != 0) {
		cerr << "Khong the lay ten client! ERROR: " << ret << endl;
		inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);      // ko lấy dc tên client -> chuyển IPv4 thành dạng text, đặt thành tên cho client 
	}
	cout << "Client: " << host << ", Connected on port: " << ntohs(client.sin_port) << endl;


	// nhận thông điệp client gửi đến
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

		// in ra thông điệp client gửi đến
		cout << string(buf, 0, bytesReceived) << endl;

		// xử lý yêu cầu của client
		string output;
		int size;
		this->handleClientRequest(buf, output, size);

		// send nội dung client yêu cầu
		if (send(clientSocket, output.c_str(), size, 0) == SOCKET_ERROR) {
			cerr << "Khong gui duoc toi client! Error: " << WSAGetLastError() << endl;
			break;
		}
	}
	closesocket(clientSocket);
}

void Server::handleClientRequest(char buf[MAXBUFLEN], string& output, int& size) {
	// phân tích thông điệp client gửi đến											(e.g: GET /index.html HTTP/1.1)
	istringstream iss(buf);
	vector<string> parsed((istream_iterator<string>(iss)), istream_iterator<string>());

	
	// nếu client yêu cầu 1 file không tồn tại thì in ra 404						(e.g: GET /abc.xyz HTTP/1.1)
	string content = "<h1>404 Not Found</h1>";	// nội dung gửi lại cho client
	int errorCode = 404;


	// nếu client không yêu cầu file gì thì mặc định gửi cho client index.html		(e.g: GET / HTTP/1.1)
	string htmlFile = "/index.html";


	// xử lý HTTP Request
	if (parsed.size() >= 3 && parsed[0] == "GET") {// nếu là GET method 
		// tên file client yêu cầu
		htmlFile = parsed[1];
		// trường hợp client không yêu cầu file thì mặc định là index.html
		if (htmlFile == "/") {
			htmlFile = "/index.html";
		}
	}
	else if (parsed.size() >= 3 && parsed[0] == "POST") {// nếu là POST method
		// kiểm tra đăng nhập
		if (parsed.back() == "user=admin&pass=admin") {
			htmlFile = "/info.html";
		}
		else { // đăng nhập sai
			htmlFile = "/404.html";
		}

	}


	// đọc file html ở local
	ifstream f(".\\html" + htmlFile);				
	if (f.good()) {
		string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		content = str;
		errorCode = 200;
	}
	f.close();


	// khởi tạo thông điệp trả ra cho client
	ostringstream oss;
	// header
	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
	oss << "Server: HUYTU-PC Web Server\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "\r\n";
	// body (nội dung html)
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
