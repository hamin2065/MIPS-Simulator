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
	printf("PC: %x	", PC);
	IR inst;
	inst.I = MEM(PC, 0, 0, 2);
	unsigned int op = getOp(inst), rs = getRs(inst), rt = getRt(inst), rd = getRd(inst),
		sh = getSh(inst), fn = getFn(inst), operand = getOperand(inst), offset = getOffset(inst);
	int Z = 0, changeReg = 0;

	PC += 4;
	//명령어 해석 및 실행
	if (op == 1) {
		printf("bltz &%d, %d", inst.II.rs, inst.II.operand * 4);
		if (ALU(REG(rs, 0, 0), 0, 0x4, 1) == 1)
			setPC(PC + offset * 4 - 4);
	}
	if (op == 2) {
		printf("j 0x%08x", inst.JI.address * 4);
		setPC(offset * 4);
	}
	if (op == 3) {
		printf("jal 0x%08x", inst.JI.address * 4);
		REG(31, PC, 1); setPC(offset * 4);
	}
	if (op == 4) {
		printf("beq $%d, $%d, %d", inst.II.rs, inst.II.rt, inst.II.operand * 4);
		if (ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z) == 0)
			setPC(PC + operand * 4 - 4);
	}
	if (op == 5) {
		printf("bne $%d, $%d, %d", inst.II.rs, inst.II.rt, inst.II.operand * 4);
		if (ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z) != 0)
			setPC(PC + operand * 4 - 4);
	}

	if (op == 10) {
		printf("addi $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
		REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0x8, &Z), 1);
	}
	if (op == 12) {
		printf("slti $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
		REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0x4, &Z), 1);
	}
	if (op == 14) {
		printf("andi $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
		REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xC, &Z), 1);
	}
	if (op == 15) {
		printf("ori $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
		REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xD, &Z), 1);
	}
	if (op == 16) {
		printf("xori $%d, $%d, %d", inst.II.rt, inst.II.rs, (short)inst.II.operand);
		REG(rt, ALU(REG(rs, 0, 0), (short)operand, 0xE, &Z), 1);
	}
	if (op == 17) {
		printf("lui $%d, %d", inst.II.rt, (short)inst.II.operand);
		REG(rt, (short)operand << 16, 1);
	}
	if (op == 40) {
		printf("lb $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
		REG(rt, (int)MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
	}
	if (op == 43) {
		printf("lw $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
		REG(rt, MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
	}
	if (op == 44) {
		printf("lbu $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
		REG(rt, (unsigned int)MEM(REG(rs, 0, 0) + operand, 0, 0, 2), 1);
	}

	if (op == 50) {
		printf("sb $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
		MEM(REG(rs, 0, 0) + operand, (int)REG(rt, 0, 0), 1, 2);
	}
	if (op == 53) {
		printf("sw $%d, %d($%d)", inst.II.rt, inst.II.operand, inst.II.rs);
		MEM(REG(rs, 0, 0) + operand, REG(rt, 0, 0), 1, 2);
	}

	else if (op == 0) { //opcode에 해당하지 않는 경우
		if (fn == 0) {
			printf("sll $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
			REG(rd, ALU(REG(rt, 0, 0), sh, 0x1, &Z), 1);
		}
		else if (fn == 2) {
			printf("srl $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
			REG(rd, ALU(REG(rt, 0, 0), sh, 0x2, &Z), 1);
		}
		else if (fn == 3) {
			printf("sra $%d, $%d, %d", inst.RI.rd, inst.RI.rt, inst.RI.sh);
			REG(rd, ALU(REG(rt, 0, 0), sh, 0x3, &Z), 1);
		}

		else if (fn == 10) {
			printf("jr $%d", inst.RI.rs);
			setPC(REG(31, 0, 0));
		}
		else if (fn == 14) {
			printf("syscall %d", REG(2, 0, 0));
			if (REG(2, 0, 0) == 10) setPC(0);
		}

		else if (fn == 20) {
			printf("mfhi &%d", inst.RI.rs);
			HI = rs;
		}
		else if (fn == 22) {
			printf("mflo &%d", inst.RI.rs);
			LO = rs;
		}

		else if (fn == 30) {
			printf("mul $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, rs * rt, 1);
		}

		else if (fn == 40) {
			printf("add $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x8, &Z), 1);
		}
		else if (fn == 42) {
			printf("sub $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x9, &Z), 1);
		}
		else if (fn == 44) {
			printf("and $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xC, &Z), 1);
		}
		else if (fn == 45) {
			printf("or $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xD, &Z), 1);
		}
		else if (fn == 46) {
			printf("xor $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xE, &Z), 1);
		}
		else if (fn == 47) {
			printf("nor $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0xF, &Z), 1);
		}

		else if (fn == 52) {
			printf("slt $%d, $%d, $%d", inst.RI.rd, inst.RI.rs, inst.RI.rt);
			REG(rd, ALU(REG(rs, 0, 0), REG(rt, 0, 0), 0x4, &Z), 1);
		}

		else {
			printf("Undefined Instruction\n");
		}
	}printf("\n");
}
