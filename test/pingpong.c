#include "syscall.h"
#include "copyright.h"

int main()
{
	int pingPID, pongPID;

	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");

	Join(pingPID);
	Join(pongPID);
	
	Exit(0);
}