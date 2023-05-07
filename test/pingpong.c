#include "syscall.h"
#include "copyright.h"

int main()
{
	int pingPID, pongPID;
	int ecPing, ecPong; // exit code

	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");

	ecPing = Join(pingPID);
	ecPong = Join(pongPID);

	Exit(ecPing);
	Exit(ecPong);
}