#include "CServerHTTP.h"
#include <iostream>

using namespace std;

void LauchServer() {
    CServerHTTP();
}

extern "C" void app_main()
{
    LauchServer();
}