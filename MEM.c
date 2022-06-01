//메모리
unsigned char progMEM[0x100000], dataMEM[0x100000], stakMEM[0x100000];

unsigned int MEM(unsigned int A, int V, int nRW, int S)
{
	unsigned int sel, offset;
	unsigned char* pM;
	sel = A >> 20; offset = A & 0xFFFFF;
	if (sel == 0x004) //program memory
		pM = progMEM;
	else if (sel == 0x100) //data memory
		pM = dataMEM;
	else if (sel == 0x7FF) //stack
		pM = stakMEM;
	else
	{
		printf("No memory\n");
		return 1;
	}

	pM += offset; //읽거나 쓸 메모리 주소 설정
	if (S == 0) //byte
	{
		if (nRW == 0) //read
		{
			return (char)pM[0];
		}
		else if (nRW == 1) //write
		{
			pM[0] = V;

			return 0;
		}
	}
	else if (S == 1) //half word
	{
		if (nRW == 0) //read
		{
			return (short)(pM[0] << 8) + pM[1]; //Big Endian 방식으로 읽기
		}
		else if (nRW == 1) //write
		{
			pM[0] = V >> 8; pM[1] = V; //Big Endian 방식으로 쓰기

			return 0;
		}
	}
	else if (S == 2) //word
	{
		if (nRW == 0) //read
		{
			return (pM[0] << 24) + (pM[1] << 16) + (pM[2] << 8) + pM[3];
		}
		else if (nRW == 1) //write
		{
			pM[0] = V >> 24; pM[1] = V >> 16; pM[2] = V >> 8; pM[3] = V;

			return 0;
		}
	}
	else //S가 유효하지 않은 값일 경우 오류
	{
		printf("No Size\n");
		return 1;
	}
}
