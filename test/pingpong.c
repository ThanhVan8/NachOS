#include "syscall.h"


void main()
{
	int pingPID, pongPID;
	PrintString("Ping-Pong test starting...\n\n");
	// CreateSemaphore("pong", 0);
	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");
	CreateSemaphore("ping", 0);
	Join(pingPID);
	Join(pongPID);
	Exit(0);
	Exit(0);
}