#include "Server.h"

int main() {
	Server s;
	if (s.Init()) 
		return 1;
	if (s.Bind())
		return 1;

	s.Listen();
	// doi client ket noi & loop: nhan va nghe thong diep
	s.AcceptAndSend();
	// dong socket
	s.Close();
	return 0;
}