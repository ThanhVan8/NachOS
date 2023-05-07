#include "syscall.h"

int main()
{
	int idIn, idOut;
	char buff[256];
	int len;

	CreateFile("file_out.txt");

	idIn = Open("file_in.txt", 1);
	if(idIn == -1)
		return 0;
	idOut = Open("file_out.txt", 0);
	if(idOut == -1)
		return 0;

	len = Read(buff, 256, idIn);
	Write(buff, len, idOut);
	Close(idIn);
	Close(idOut);
}