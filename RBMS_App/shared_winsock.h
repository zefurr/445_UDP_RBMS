#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")	// Winsock Library

#define BUF_LEN 1024

// Calling SendMessage(int) from MainLogic inside the for loop without including this header in Sender.h causes unresolved external.
// Why is that?