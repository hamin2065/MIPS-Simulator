#include <stdio.h>
#include <stdlib.h>

#define REG_SIZE 32

typedef union type {
    unsigned int I;
    struct rtype
    {
        unsigned int funct : 6;
        unsigned int sh : 5;
        unsigned int rd : 5;
        unsigned int rt : 5;
        unsigned int rs : 5;
        unsigned int opcode : 6;
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






int logicOperation(int X, int Y, int s1s0) {
    if (s1s0 < 0 || s1s0 > 3) {
        printf("error in logic\n");
        exit(1);
    }
    if (s1s0 == 0)
        return X & Y;
    else if (s1s0 == 1)
        return X | Y;
    else if (s1s0 == 2)
        return X ^ Y;
    else
        return !(X | Y);
}

int shiftOperation(int X, int Y, int s1s0) {
    int ret;
    if (s1s0 < 0 || s1s0 > 3) {
        printf("error in shift");
        exit(1);
    }
    if (s1s0 == 0) {
        ret = X;
    }
    else if (s1s0 == 1) { // shift left logical
        ret = X << Y;
    }
    else if (s1s0 == 2) { // shift right logical
        ret = (X >> Y) & ~(((0x1 << sizeof(X)) >> Y) << 1);
    }
    else { //shift right arithmatic
        ret = X >> Y;
    }
    return ret;
}

int addSubtract(int X, int Y, int s0) {
    int ret;
    if (s0 < 0 || s0 > 1) {
        printf("Error in addSubtract\n");
        exit(1);
    }
    if (s0 == 0) { // add
        ret = X + Y;
    }
    else { // substract
        ret = X - Y;
    }
    return ret;
}

int checkSetLess(int X, int Y) {
    int ret;

    if (addSubtract(X, Y, 1) < 0)
        ret = 1;
    else
        ret = 0;
    return ret;
}

int checkZero(int S) {
    return S == 0;
}




//ALU
int ALU(int X, int Y, int C, int* Z) {
    int s3s2, s1s0;
    int ret;

    s3s2 = (C >> 2) & 3;
    s1s0 = C & 3;

    if (s3s2 == 0) {
        //shift 연산
        ret = shiftOperation(X, Y, s1s0);
    }
    else if (s3s2 == 1) {
        // set less
        ret = checkSetLess(X, Y);
    }
    else if (s3s2 == 2) {
        //add substract
        *Z = checkZero(s1s0 & 1);
        ret = addSubtract(X, Y, *Z);
    }
    else if (s3s2 == 3) {
        // AND OR XOR NOR
        ret = logicOperation(X, Y, s1s0);
    }
    else {
        printf("error in ALU");
        exit(1);
    }
    return ret;
}


//MEM
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

//REG
unsigned int PC = 0, HI = 0, LO = 0;
unsigned int R[REG_SIZE];

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

void showRegister(void) //레지스터 출력
{
    printf("[REGISTER]\n");
    for (int i = 0; i < REG_SIZE; i++)
        printf("R%d = %x\n", i, REG(i, 0, 0));
    printf("PC: %x\n", PC);
    printf("HI: %x   LO: %x\n", HI, LO);
}

void setPC(unsigned int val) {
    PC = val;
}

//파일 로드
void load()
{
    FILE* pFile = NULL;
    unsigned char tmp[1000];
    char str[20] = "";
    unsigned int index = 0, instNum = 0, dataNum = 0;
    printf("Enter file name: ");
    scanf("%s", &str);

    if ((pFile = fopen(str, "rb")) == NULL) {
        printf("Cannot open file\n");
        return;
    }
    printf("Open success!\n");
    for (int i = 0; fread(&tmp[i], sizeof(tmp), 1, pFile); i++);
    for (; index < 4; index++) {
        instNum = instNum << 8;
        instNum += tmp[index];
    }
    for (; index < 8; index++) {
        dataNum = dataNum << 8;
        dataNum += tmp[index];
    }

    for (int i = 0; i < instNum; i++) {
        int inst = 0;
        for (int j = index; j < index + 4; j++) {
            inst = inst << 8;
            inst += tmp[j];
        }
        index += 4;
        MEM(0x400000 + i * 4, inst, 1, 2);
    }
    for (int i = 0; i < dataNum; i++) {
        int data = 0;
        for (int j = index; j < index + 4; j++) {
            data = data << 8;
            data += tmp[j];
        }
        index += 4;
        MEM(0x10000000 + i * 4, data, 1, 2);
    }
    setPC(0x400000);
    REG(29, 0x80000000, 1);
    fclose(pFile);
}

//STEP
unsigned int getOp(IR ir)
{
    return ir.RI.opcode;
}
unsigned int getRs(IR ir)
{
    return ir.RI.rs;
}
unsigned int getRt(IR ir)
{
    return ir.RI.rt;
}
unsigned int getRd(IR ir)
{
    return ir.RI.rd;
}
unsigned int getSh(IR ir)
{
    return ir.RI.sh;
}
unsigned int getFn(IR ir)
{
    return ir.RI.funct;
}
unsigned int getOperand(IR ir)
{
    return ir.II.operand;
}
unsigned int getOffset(IR ir)
{
    return ir.JI.address;
}
void step(void)
{
    if (PC == 0)
        setPC(0x400000);
    printf("PC: %x   ", PC);
    IR inst;
    inst.I = MEM(PC, 0, 0, 2);
    unsigned int op = getOp(inst), rs = getRs(inst), rt = getRt(inst), rd = getRd(inst),
        sh = getSh(inst), fn = getFn(inst), operand = getOperand(inst), offset = getOffset(inst);
    int Z = 0, changeReg = 0;

    PC += 4;
    //명령어 해석 및 실행
    if (op == 001) {
        printf("bltz &%d, %d", inst.II.rs, inst.II.operand * 4);
        if (ALU(REG(rs, 0, 0), 0, 0x4, &op) == 1)
            setPC(PC + offset * 4 - 4);
    }
    else if (op == 002) {
        printf("j 0x%08x", inst.JI.address * 4);
        setPC(offset * 4);
    }
    else if (op == 003) {
        printf("jal 0x%08x", inst.JI.address * 4);
        REG(31, PC, 1); setPC(offset * 4);
    }
    else if (op == 004) {
        printf("beq $%d, $%d, %d", inst.II.rs, inst.II.rt, inst.II.operand * 4);
        if (ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z) == 0)
            setPC(PC + operand * 4 - 4);
    }
    else if (op == 005) {
        printf("bne $%d, $%d, %d", inst.II.rs, inst.II.rt, inst.II.operand * 4);
        if (ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z) != 0)
            setPC(PC + operand * 4 - 4);
    }

    else if (op == 010) {
        printf("addi $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
        REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0x8, &Z), 1);
    }
    else if (op == 012) {
        printf("slti $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
        REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0x4, &Z), 1);
    }
    else if (op == 014) {
        printf("andi $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
        REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xC, &Z), 1);
    }
    else if (op == 015) {
        printf("ori $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
        REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xD, &Z), 1);
    }
    else if (op == 016) {
        printf("xori $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
        REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xE, &Z), 1);
    }
    else if (op == 017) {
        printf("lui $%d, %d", inst.II.rt, (short)inst.II.operand);
        REG(rt, (short)operand << 16, 1);
    }
    else if (op == 040) {
        printf("lb $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
        REG(rt, (int)MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
    }
    else if (op == 043) {
        printf("lw $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
        REG(rt, MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
    }
    else if (op == 044) {
        printf("lbu $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
        REG(rt, (unsigned int)MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
    }

    else if (op == 050) {
        printf("sb $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
        MEM(REG(rs, 0, 0) + operand, (int)REG(rt, 0, 0), 1, 2);
    }
    else if (op == 053) {
        printf("sw $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
        MEM(REG(rs, 0, 0) + operand, REG(rt, 0, 0), 1, 2);
    }

    else { //opcode에 해당하지 않는 경우
        if (fn == 000) {
            printf("sll $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
            REG(rd, ALU(REG(rt, 0, 0), sh, 0x1, &Z), 1);
        }
        else if (fn == 002) {
            printf("srl $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
            REG(rd, ALU(REG(rt, 0, 0), sh, 0x2, &Z), 1);
        }
        else if (fn == 003) {
            printf("sra $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
            REG(rd, ALU(REG(rt, 0, 0), sh, 0x3, &Z), 1);
        }

        else if (fn == 010) {
            printf("jr $%d", inst.RI.rs);
            setPC(REG(31, 0, 0));
        }
        else if (fn == 014) {
            printf("syscall %d", REG(2, 0, 0));
            if (REG(2, 0, 0) == 10) setPC(0);
        }

        else if (fn == 020) {
            printf("mfhi &%d", inst.RI.rs);
            HI = rs;
        }
        else if (fn == 022) {
            printf("mflo &%d", inst.RI.rs);
            LO = rs;
        }

        else if (fn == 030) {
            printf("mul $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, rs * rt, 1);
        }

        else if (fn == 040) {
            printf("add $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x8, &Z), 1);
        }
        else if (fn == 042) {
            printf("sub $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x9, &Z), 1);
        }
        else if (fn == 044) {
            printf("and $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xC, &Z), 1);
        }
        else if (fn == 045) {
            printf("or $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xD, &Z), 1);
        }
        else if (fn == 046) {
            printf("xor $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z), 1);
        }
        else if (fn == 047) {
            printf("nor $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xF, &Z), 1);
        }

        else if (fn == 052) {
            printf("slt $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
            REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x4, &Z), 1);
        }

        else {
            printf("Undefined Instruction\n");
        }
    }printf("\n");
}

int main() {
    unsigned int BP = 0; //break point

    while (1) {
        char cmd[3];
        unsigned int pc, start, end, value;
        int num;
        printf("> Enter a command : ");
        scanf("%s", &cmd);
        if (cmd[0] == 'l') { //파일 로드
            load();
        }
        else if (cmd[0] == 'j') { //PC 설정
            scanf("%x", &pc);
            setPC(pc);
        }
        else if (cmd[0] == 'g') { //명령어 끝까지 실행
            if (PC == 0 || PC == BP)
                step();
            while (PC != 0 && PC != BP)
                step();
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
        }
        else if (cmd[0] == 'm') { //메모리 범위 출력
            scanf("%x %x", &start, &end);
            for (unsigned int i = start; i <= end; i += 4)
                printf("MEM[%x] = %08x\n", i, MEM(i, 0, 0, 2));
        }
        else if (cmd[0] == 'r') { //레지스터 상태 출력
            showRegister();
        }
        else if (cmd[0] == 'b') { //브레이크 포인트 설정
            scanf("%x", &pc);
            BP = pc;
        }
        else if (cmd[0] == 'x') { //종료
            printf("Exit\n");
            return 0;
        }
    }
}
