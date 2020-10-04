#include "Header.h"

void service(Server s){
	// nhan va doc/gui thong diep
	s.accept_sendClient();
}


void spawnThreads(int n, Server s) {
    vector<thread> threads(n);
    // tao n thread
    for (int i = 0; i < n; i++) {
        threads[i] = thread(service, s);
    }
    // join thread
    for (auto& th : threads) {
        th.join();
    }
}

