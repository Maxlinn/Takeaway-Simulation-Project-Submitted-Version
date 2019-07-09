#include <stdlib.h>
#include <stdio.h>
int main()
{
	system("gcc -o simulation.exe file.c main.c display.c strategy.c");
	system("simulation.exe");
	printf("\n________________________________END OF PROGRAM_____________________\n");
	system("pause");
	return 0;
}
