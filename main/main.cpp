#include "CServerHTTP.h"


void LauchServer() {
    CServerHTTP();
}

extern "C" void app_main()
{
    LauchServer();
}