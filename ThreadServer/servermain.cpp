#include "Server.h"

int main(void)
{
	Server sv;
	sv.ServerInit();
	while (1)
	{
		sv.Accept();
	}

	return 0;
}