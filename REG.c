//구하민 - 레지스터
#include <stdio.h>

#define REG_SIZE 32

unsigned int R[REG_SIZE], PC = 0, HI = 0, LO = 0; //레지스터, 프로그램 카운터, HI/LO

unsigned int REG(unsigned int A, unsigned int V, int nRW)
{
	if (A > 31)
	{
		printf("Invalid register number\n");
		return 1;
	}

	if (nRW == 0) // Read
		return R[A];
	else if (nRW == 1) // Write
		R[A] = V;
	else
	{
		printf("Invalid nRW value\n");
		return 1;
	}
	return 0;
}

void PrintRegister(void) //레지스터 출력
{
	printf("[REGISTER]\n");
	for (int i = 0; i < REG_SIZE; i++)
		printf("R%d = %x\n", i, REG(i, 0, 0));
	printf("PC: %x\n", PC);
	printf("HI: %x	LO: %x\n", HI, LO);
}

void setPC(unsigned int val) //PC 설정
{
	PC = val;
}
