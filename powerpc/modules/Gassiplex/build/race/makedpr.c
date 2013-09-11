#include <stdio.h>

int main(void)
{
	unsigned short address;
	for(address = 0; address < 8191; address++)
	{
		printf("0x%04x ", address);
	}

	return 0;
}	
