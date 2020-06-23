#include "Header.h"

int main() {
	Server s;
	if (s.Init()) 
		return 1;
	if (s.Bind())
		return 1;
	// doi client ket noi 
	s.Listen();
	// loop: nhan va nghe thong diep
	bool running = true;
	while (running) {
		thread Run(&Server::AcceptAndSend,&s);
		Run.detach();
		if (Run.joinable()) {
			Run.join();
		}
		
	}
	// dong socket
	s.Close();
	return 0;
}