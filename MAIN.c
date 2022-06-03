#include <stdio.h>
#include <stdlib.h>

#define REG_SIZE 32
const int M_SIZE = 1024;
unsigned int PC = 0, HI = 0, LO = 0;

typedef union type{
	unsigned int I;
	struct rtype
	{
		unsigned int opcode: 6;
		unsigned int rs: 5;
		unsigned int rt: 5;
		unsigned int rd: 5;
        unsigned int funct: 6;
        unsigned int shamt: 5;
	}RI;
	struct itype
	{
		unsigned int operand : 16;
		unsigned int rt : 5;
		unsigned int rs : 5;
		unsigned int opcode : 6;
	}II;
	struct jtype
	{
		unsigned int address : 26;
		unsigned int opcode : 6;
	}JI;
}IR;


void setPC(unsigned int val) {
	PC = val;
}

//파일 로드
void load()
{
	FILE *pFile = NULL;
	unsigned char tmp[M_SIZE];
	char str[20];
	unsigned int index = 0, instNum = 0, dataNum = 0;
	printf("Enter file name: ");
	scanf("%s", &str);
	if (fopen_s(&pFile, str, "rb")) {
		printf("Cannot open file\n");
		return 1;
	}
	for (int i = 0; fread(&tmp[i], sizeof(tmp), 1, pFile); i++);
	for (; index < 4; index++) {
		instNum = instNum<<8;
		instNum += tmp[index];
	}
	for (; index < 8; index++) {
		dataNum = dataNum<<8;
		dataNum += tmp[index];
	}

	for (int i = 0; i < instNum; i++) {
		int inst = 0;
		for (int j = index; j < index+4; j++) {
			inst = inst<<8;
			inst += tmp[j];
		}
		index += 4;
		MEM(0x400000+i*4, inst, 1, 2);
	}
	for (int i = 0; i < dataNum; i++) {
		int data = 0;
		for (int j = index; j < index+4; j++) {
			data = data<<8;
			data+=tmp[j];
		}
		index += 4;
		MEM(0x10000000+i*4, data, 1, 2);
	}
	setPC(0x400000);
	REG(29, 0x80000000, 1);
	fclose(pFile);
}

int main() {
	unsigned int BP = 0; //break point

	while (1) {
		char cmd[3];
		unsigned int pc, start, end, value;
		int num;

		scanf("%s", &cmd);
		if (cmd[0] == 'l') { //파일 로드
			load();
			break;
		}
		else if (cmd[0] == 'j') { //PC 설정
			scanf("%x", &pc);
			setPC(pc);
			break;
		}
		else if (cmd[0] == 'g') { //명령어 끝까지 실행
			if (PC == 0 || PC == BP)
				step();
			while (PC != 0 && PC != BP)
				step();
			break;
		}
		else if (cmd[0] == 's') { 
			if (cmd[1] == 'r') { //레지스터 값 설정
				scanf("%d %x", &num, &value);
				REG(num, value, 1);
			}
			else if (cmd[1] == 'm') { //메모리 값 설정
				scanf("%x %x", &num, &value);
				MEM(num, value, 1, 2);
			}
			else //스텝
				step();
			break;
		}
		else if (cmd[0] == 'm') { //메모리 범위 출력
			scanf("%x %x", &start, &end);
			for (unsigned int i = start; i <= end; i+=4)
				printf("MEM[%x] = %08x\n", i, MEM(i, 0, 0, 2));
			break;
		}
		else if (cmd[0] == 'r') { //레지스터 상태 출력
			showRegister();
			break;
		}
		else if (cmd[0] == 'b') { //브레이크 포인트 설정
			scanf("%x", &pc);
			BP = pc;
			break;
		}
		else if (cmd[0] == 'x') { //종료
			printf("Exit\n");
			return 0;
		}
	}
}
