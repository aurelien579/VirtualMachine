#include "computer.h"

#include <stdio.h>
#include <string.h>

int main (int argc, char* argv[])
{
	computer_t *computer = computer_create ();
	FILE *fi = fopen ("test.bin", "rb");

	fread (computer->memory->content, 1, 128, fi);

	printf ("[INFO] Running...\n");
	computer_start (computer);
	printf ("[INFO] Stopped\n");


	return 0;
}
