#include "Header.h"

int main() {
	Server s;
	// khoi tao socket listening cua server
	if (s.init())
		return 1;
	if (s.Bind())
		return 1;
	// doi client ket noi 
	s.Listen();
	// server dang chay
	bool running = true;
	while (running) {
		// tao cac thread de lang nghe client
		spawnThreads(MAXCLIENT, s);
	}
	// dong socket listening cua server
	s.close();
	return 0;
}