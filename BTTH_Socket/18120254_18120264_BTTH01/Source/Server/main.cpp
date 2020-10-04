#include "Header.h"

int main() {
	Server s;
	// khởi tạo socket listening của server
	if (s.init())
		return 1;
	if (s.Bind())
		return 1;
	// đợi client kết nối tới server
	s.Listen();
	// server đang chạy
	bool running = true;
	while (running) {
		// tạo các thread phục vụ client
		spawnThreads(MAXCLIENT, s);
	}
	// đóng socket listening của server
	s.close();
	return 0;
}