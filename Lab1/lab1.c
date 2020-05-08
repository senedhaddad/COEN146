#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *src = fopen(argv[1], "r");
	FILE *dest = fopen(argv[2], "w");
	char buffer[10];
	int bytes;
	
	while(1)
	{	
		bytes = fread(buffer, 1, sizeof(buffer), src);
		
		if(bytes == 0)
			break;
		
		fwrite(buffer, 1, bytes, dest);
	}
	
	fclose(src);
	fclose(dest);
	
	return 0;
}
