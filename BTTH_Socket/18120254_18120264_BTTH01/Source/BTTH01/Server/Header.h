#ifndef _HEADER_H_
#define _HEADER_H_
#include <thread>
#include <vector>
#include "Server.h"

void service(Server s);
void spawnThreads(int n, Server s);

#endif