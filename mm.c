/**
 * @Program		:	Simulation for Architecture
 * @Author		:	Keen Zhou
 * @Date		:	2014/10/18
 * @Description	:	Complier GCC
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MEM_SIZE 0x010000	//Length of Alloced Memory (1024*64)
#define REG_SIZE 32		//Quantity of Registers
#define MAX_LINE 1000	//max line of instructions
#define EPC 27
#define CO 28
#define HI 29
#define LO 30

//Global Params
static char* mem;
static int* regs;
int jump_target[MAX_LINE];
int jump_pc[MAX_LINE];
int jump_count = 0;
int pc = 0;
int reg_code = 0;
char ins_type[51][6] ={
				"mul","mulu","div","divu","add","addu","sub","subu",
				"and","or","nor","xor","slt","sltu","addi","addiu",
				"slti","sltiu","andi","ori","xori","lui","beq","bne",
				"lw","sw","lbu","lb","sb","blez","bgtz","bltz",
				"add.s","add.d","sub.s","sub.d","mul.s","mul.d","div.s","div.d",
				"lwc1","swc1","j","jal","jr","jalr","mfhi","mflo",
				"mfepc","mfco","nop"
					};
int ins_binary[51] = {
				0b011000,0b011001,0b011010,0b011011,0b100000,0b100001,0b100010,0b100011,
				0b100100,0b100101,0b100111,0b101000,0b101010,0b101011,0b001000,0b001001,
				0b001010,0b001011,0b001100,0b001101,0b001110,0b001111,0b000100,0b000101,
				0b100011,0b101011,0b100100,0b100000,0b101000,0b000110,0b000111,0b000001,
				0b000000,0b000000,0b000001,0b000001,0b000010,0b000010,0b000011,0b000011,
				0b110001,0b111001,0b000010,0b000011,0b001000,0b001001,0b010000,0b010010,
				0b011100,0b011010,0b111111
					};
char ins_type_0[18][6] ={
				"mul","mulu","div","divu","add","addu","sub","subu",
				"and","or","nor","xor","slt","sltu","jr","jalr",
				"mfhi","mflo",
					};
int ins_binary_0[18] = {
				0b011000,0b011001,0b011010,0b011011,0b100000,0b100001,0b100010,0b100011,
				0b100100,0b100101,0b100111,0b101000,0b101010,0b101011,0b001000,0b001001,
				0b010000,0b010010,
					};
char ins_type_1[24][6] ={
				"addi","addiu","slti","sltiu","andi","ori","xori","lui",
				"beq","bne","lw","sw","lbu","lb","sb","blez",
				"bgtz","bltz","lwc1","swc1","j","jal","mfepc","mfco",
					};
int ins_binary_1[24] = {
				0b001000,0b001001,0b001010,0b001011,0b001100,0b001101,0b001110,0b001111,
				0b000100,0b000101,0b100011,0b101011,0b100100,0b100000,0b101000,0b000110,
				0b000111,0b000001,0b110001,0b111001,0b000010,0b000011,0b010000,0b010010,
					};
char ins_type_2[8][6] ={"add.s","add.d","sub.s","sub.d","mul.s","mul.d","div.s","div.d"};
int ins_binary_21[8] = {0b000000,0b000000,0b000001,0b000001,0b000010,0b000010,0b000011,0b000011};
int ins_binary_22[8] = {0b01000100000,0b01000100001,0b01000100000,0b01000100001,
						0b01000100000,0b01000100001,0b01000100000,0b01000100001};
char reg_type_map[32][4] = { //R28-->EPC R28-->CO R29-->Hi	R30-->Lo 
						"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9",
						"R10","R11","R12","R13","R14","R15","R16","R17","R18","R19",
						"R20","R21","R22","R23","R24","R25","R26","R27","R28","R29",
						"R30","R31"
						};
int reg_offset[32] = {
					0b00000,0b00001,0b00010,0b00011,0b00100,0b00101,0b00110,0b00111,0b01000,0b01001,
					0b01010,0b01011,0b01100,0b01101,0b01110,0b01111,0b10000,0b10001,0b10010,0b10011,
					0b10100,0b10101,0b10110,0b10111,0b11000,0b11001,0b11010,0b11011,0b11100,0b11101,
					0b11110,0b11111
					};

//Init and Destoy
void mem_init();
void mem_destroy();
void reg_init();
void reg_destroy();

//Memory 8 bits Write and Read
void write_mem_ubyte(long, unsigned char);
unsigned char read_mem_ubyte(long);
void write_mem_byte(long, char);
char read_mem_byte(long);

//Memory 16 bits Write and Read
void write_mem_uhword(long, unsigned short);
unsigned short read_mem_uhword(long);
void write_mem_hword(long, short);
short read_mem_hword(long);

//Memory 32 bits Write and Read
void write_mem_uword(long, unsigned int);
unsigned int read_mem_uword(long);
void write_mem_word(long, int);
int read_mem_word(long);
void write_mem_float(long, float);
float read_mem_float(long);

//Load and Store Register File
void store_regs_int(long, int);
int load_regs_int(long);
void store_regs_uint(long, int);
int load_regs_uint(long);
void store_regs_float(long, float);
float load_regs_float(long);
void store_regs_double(long, double);
double load_regs_double(long);

//parse reg code
int parse_reg_R2(int,char **);
int parse_reg_R3(int,char **);
int parse_reg_Rf8(int,char **);
int parse_reg_Rf2(int,char **);
int parse_reg_Im(int,char **);
int parse_reg_Ilui(int,char **);
int parse_reg_Io2(int,char **);
int parse_reg_Irmo(int,char **);
int parse_reg_Io1(int,char **);
int	parse_reg_Jtar(int,char **);
int parse_reg_Jlr(int,char **);
int parse_reg_Jr2(int,char **);
int parse_reg_Jrr(int,char **);
int parse_reg_Jmr(int,char **);
int parse_reg_Jend(int,char **);
int parse_complement_16(int);
int parse_source_16(int);
int parse_complement_26(int);
int parse_source_26(int);

//Some logic functions
void parse_ins_str(char [], char **,int);
int parse_ins_reg(char *);
int parse_ins_optype(char *);
int parse_ins_opcode(int,char **);
void execution_pre(char *);
void execution();
void execution_data();
void execution_ret();
int check_ins_type(int);
void execute_step(int,int);
int check_in_jumps(int);

//MIPS Instruction Set
void INSI_ADD(int);
void INSI_ADDU(int);
void INSI_SUB(int);
void INSI_SUBU(int);
void INSI_MUL(int);
void INSI_MULU(int);
void INSI_DIV(int);
void INSI_DIVU(int);
void INSI_AND(int);
void INSI_OR(int);
void INSI_NOR(int);
void INSI_XOR(int);
void INSI_ADDI(int);
void INSI_ADDIU(int);
void INSI_ANDI(int);
void INSI_ORI(int);
void INSI_XORI(int);
void INSI_LW(int);
void INSI_SW(int);
void INSI_LBU(int);
void INSI_LB(int);
void INSI_SB(int);
void INSI_JALR(int);
void INSI_JR(int);
void INSI_JAL(int);
void INSI_J(int);
void INSI_LUI(int);
void INSI_BEQ(int);
void INSI_BNE(int);
void INSI_BLEZ(int);
void INSI_BGTZ(int);
void INSI_BLTZ(int);
void INSI_MFHI(int);
void INSI_MFLO(int);
void INSI_MFEPC(int);
void INSI_MFCO(int);
void INSI_NOP(int);
void INSI_SLT(int);
void INSI_SLTU(int);
void INSI_SLTI(int);
void INSI_SLTIU(int);
void INSI_ADD_D(int);
void INSI_ADD_S(int);
void INSI_SUB_D(int);
void INSI_SUB_S(int);
void INSI_MUL_D(int);
void INSI_MUL_S(int);
void INSI_DIV_D(int);
void INSI_DIV_S(int);
void INSI_LWC1(int);
void INSI_SWC1(int);

int main(int argc,char *argv[])
{	
	mem_init();
	reg_init();

	execution_data();
	execution_pre(argv[1]);
	execution();
	execution_ret();

	reg_destroy();
	mem_destroy();

	return 0;
}

/* Read Instructions from File 
 * Parse Instructions to Mem
 */
void execution_pre(char *ins_fp)
{
	int mem_offset = 0;
	char *result[6];
	char ins_arr[MAX_LINE];
	FILE *ins_file = fopen(ins_fp,"r");

	if(ins_file == NULL)
	{
		printf("File Open Error !!!\n");
		exit(0);
	}
	while (((fgets(ins_arr,MAX_LINE,ins_file)) != NULL) && (strcasecmp(ins_arr,"	exit\n") != 0)) 
	{
		parse_ins_str(ins_arr,result,mem_offset);
		write_mem_word(mem_offset,parse_ins_opcode(parse_ins_optype(result[0]),result));
		mem_offset += 4;
	}
	printf("execution Preparation Successfully ...\n\n");
	printf("#################    Begin to execute    #################\n");
}
/* execute the Instructions */
void execution()
{
	int opcode;
	while((opcode = read_mem_word(pc)) != 0)
		execute_step(check_ins_type(opcode),opcode);
}
/* execution Data Prepare */
void execution_data()
{
	store_regs_int(0,-1);
	store_regs_int(1,2);
	store_regs_int(2,0);
	store_regs_int(3,0);
	store_regs_int(4,-2);
	store_regs_int(5,1000);
}
/* execution Result Show */
void execution_ret()
{
	printf("################# execution Successfully #################\n\n");
	printf("+++++++++++++++++    execution Result    +++++++++++++++++\n");
	printf("R1 %d\n",load_regs_int(1));
	printf("R2 %d\n",load_regs_int(2));
	printf("R3 %d\n",load_regs_int(3));
	printf("R4 %d\n",load_regs_int(4));
	printf("R5 %d\n",load_regs_int(5));
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
}
/* execute Step by Optype */
void execute_step(int optype,int opcode)
{
	printf("---------------------------------------------------------\n");
	printf("PC     %d\n",pc);
	printf("OPTYPE %d %s\n",optype,ins_type[optype]);
	printf("OPCODE %x\n",opcode);
	switch(optype)
	{
		case 0:INSI_MUL(opcode);break;//mul
		case 1:INSI_MULU(opcode);break;//mulu
		case 2:INSI_DIV(opcode);break;//div
		case 3:INSI_DIVU(opcode);break;//divu
		case 4:INSI_ADD(opcode);break;//add
		case 5:INSI_ADDU(opcode);break;//addu
		case 6:INSI_SUB(opcode);break;//sub
		case 7:INSI_SUBU(opcode);break;//subu
		case 8:INSI_AND(opcode);break;//and
		case 9:INSI_OR(opcode);break;//or
		case 10:INSI_NOR(opcode);break;//nor
		case 11:INSI_XOR(opcode);break;//xor
		case 12:INSI_SLT(opcode);break;//slt
		case 13:INSI_SLTU(opcode);break;//sltu
		case 14:INSI_ADDI(opcode);break;//addi
		case 15:INSI_ADDIU(opcode);break;//addiu
		case 16:INSI_SLTI(opcode);break;//slti
		case 17:INSI_SLTIU(opcode);break;//sltiu
		case 18:INSI_ANDI(opcode);break;//andi
		case 19:INSI_ORI(opcode);break;//ori
		case 20:INSI_XORI(opcode);break;//xori
		case 21:INSI_LUI(opcode);break;//lui
		case 22:INSI_BEQ(opcode);break;//beq
		case 23:INSI_BNE(opcode);break;//bne
		case 24:INSI_LW(opcode);break;//lw
		case 25:INSI_SW(opcode);break;//sw
		case 26:INSI_LBU(opcode);break;//lbu
		case 27:INSI_LB(opcode);break;//lb
		case 28:INSI_SB(opcode);break;//sb
		case 29:INSI_BLEZ(opcode);break;//blez
		case 30:INSI_BGTZ(opcode);break;//bgtz
		case 31:INSI_BLTZ(opcode);break;//bltz
		case 32:INSI_ADD_S(opcode);break;//add.s
		case 33:INSI_ADD_D(opcode);break;//add.d
		case 34:INSI_SUB_S(opcode);break;//sub.s
		case 35:INSI_SUB_D(opcode);break;//sub.d
		case 36:INSI_MUL_S(opcode);break;//mul.s
		case 37:INSI_MUL_D(opcode);break;//mul.d
		case 38:INSI_DIV_S(opcode);break;//div.s
		case 39:INSI_DIV_D(opcode);break;//div.d
		case 40:INSI_LWC1(opcode);break;//lwc1
		case 41:INSI_SWC1(opcode);break;//swc1
		case 42:INSI_J(opcode);break;//j
		case 43:INSI_JAL(opcode);break;//jal
		case 44:INSI_JR(opcode);break;//jr
		case 45:INSI_JALR(opcode);break;//jalr
		case 46:INSI_MFHI(opcode);break;//mfhi
		case 47:INSI_MFLO(opcode);break;//mflo
		case 48:INSI_MFEPC(opcode);break;//mfepc
		case 49:INSI_MFCO(opcode);break;//mfco
		case 50:INSI_NOP(opcode);break;//nop
	}
	printf("---------------------------------------------------------\n");
}
/* Check which Instruction Type by Opcode */
int check_ins_type(int optype)
{
	char *optype_str;
	int i;
	if(((optype&0xfc000000)>>26) == 0)				//type 0
	{
		for(i=0;i<=18;i++)
		{
			if((optype&0x0000003f) == ins_binary_0[i])
			{
				optype_str = ins_type_0[i];
				break;
			}
		}
	}
	else if(((optype&0xfc000000)>>26) == 0b010001)	//type 2
	{
		for(i=0;i<=8;i++)
		{
			if((((optype&0xffe00000)>>21) == ins_binary_22[i]) && ((optype&0x3f) == ins_binary_21[i]))
			{
				optype_str = ins_type_2[i];
				break;
			}
		}
	}
	else if(((optype&0xfc000000)>>26) == 0b111111){optype_str="nop";}
	else								//type 1
	{
		for(i=0;i<=24;i++)
		{
			if(((optype&0xfc000000)>>26) == ins_binary_1[i])
			{
				optype_str = ins_type_1[i];
				break;
			}
		}
	}

	i = 0;
	while(strcasecmp(optype_str,ins_type[i]) != 0){i++;}
	return i;
}
/* Parse Input Instruction */
void parse_ins_str(char a[], char **res,int mem_offset)
{
	int i=1;
	char *loop_str = strtok(a,"	"); // tab
	if(loop_str[strlen(loop_str)-1] != ':')
	{
		res[0] = strtok(loop_str," "); // space
	}
	else
	{
		res[0] = strtok(NULL," ");	// space
		loop_str[strlen(loop_str)-1] = '\0';
		loop_str[0] = '0';
		int loc = check_in_jumps(atoi(loop_str));
		jump_target[loc] = atoi(loop_str);
		jump_pc[loc] = mem_offset;
	}
	while((res[i] = strtok(NULL,",")) != NULL){i++;}
	res[i-1][strlen(res[i-1])-1] = '\0';
	if(res[i-1][0] == '#')
	{
		int j;
		for(j=0;j<(strlen(res[i-1])-1);j++)
			res[i-1][j] = res[i-1][j+1];
		res[i-1][j] = '\0';
	}
	if(res[i-1][strlen(res[i-1])-1] == ')')
	{
		res[i-1] = strtok(res[i-1],"(");
		while((res[i] = strtok(NULL,",")) != NULL){i++;}
		res[i-1][strlen(res[i-1])-1] = '\0';
	}
	if((strcasecmp(res[0],"jal") == 0) || (strcasecmp(res[0],"j") == 0))
		res[1][0] = '0';
}
/* String to Int */
/* Check Whether in Jumps */
int check_in_jumps(int target)
{
	int i;
	int is_in = 0;
	for(i=0;i<jump_count;i++)
	{
		if(jump_target[i] == target) 
		{
			is_in = 1;
			break;
		}
	}
	if((i==jump_count) && (is_in==0)) jump_count++;
	return i;
}
/* Get Opcode Type */
int parse_ins_optype(char *op_type)
{
	int i=0;
	while(strcasecmp(op_type,ins_type[i]) != 0){i++;}
	return i;
}
/* Parse and Get Reg_offset */
int parse_ins_reg(char *reg_type)
{
	int i=0;
	while(strcasecmp(reg_type,reg_type_map[i]) != 0){i++;}
	return reg_offset[i];
}
/* According to optype parse by different funcs */
int parse_ins_opcode(int optype,char **res)
{
	int opcode = 0;
	switch(optype)
	{
		case 0:case 1:case 2:case 3: 
			opcode = parse_reg_R2(ins_binary[optype],res);break;
		case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13: 
			opcode = parse_reg_R3(ins_binary[optype],res);break;
		case 14:case 15:case 16:case 17:case 18:case 19:case 20: 
			opcode = parse_reg_Im(ins_binary[optype],res);break;
		case 21:
			opcode = parse_reg_Ilui(ins_binary[optype],res);break;
		case 22:case 23: 
			opcode = parse_reg_Io2(ins_binary[optype],res);break;
		case 24:case 25:case 26:case 27:case 28:
			opcode = parse_reg_Irmo(ins_binary[optype],res);break;
		case 29:case 30:case 31:
			opcode = parse_reg_Io1(ins_binary[optype],res);break;
		case 32:case 33:case 34:case 35:case 36:case 37:case 38:case 39:
			opcode = parse_reg_Rf8(optype-32,res);break;
		case 40:case 41:
			opcode = parse_reg_Rf2(ins_binary[optype],res);break;
		case 42:case 43:
			opcode = parse_reg_Jtar(ins_binary[optype],res);break;
		case 44:
			opcode = parse_reg_Jlr(ins_binary[optype],res);break;
		case 45:
			opcode = parse_reg_Jr2(ins_binary[optype],res);break;
		case 46:case 47:
			opcode = parse_reg_Jrr(ins_binary[optype],res);break;
		case 48:case 49:
			opcode = parse_reg_Jmr(ins_binary[optype],res);break;
		case 50:
			opcode = parse_reg_Jend(ins_binary[optype],res);break;
		default :
			printf("Out of the Given MIPS !!!\n");
	}
	return opcode;
}
/* Parse Int to Complement 16bits */
int parse_complement_16(int parse_int)
{
	int parse_com = parse_int;
	if(parse_int <= 0) 
		parse_com = (parse_int&0x00007fff)+0x8000;
	return parse_com;
}
/* Parse Int to Source 16bits */
int parse_source_16(int parse_int)
{
	int parse_sce = parse_int;
	if((parse_int&0x8000)>>15 == 1) 
		parse_sce = parse_int+0xffff0000;
	return parse_sce;
}
/* Parse Int to Complement 26bits */
int parse_complement_26(int parse_int)
{
	int parse_com = parse_int;
	if(parse_int <= 0) 
		parse_com = (parse_int&0x01ffffff)+0x02000000;
	return parse_com;
}
/* Parse Int to Source 26bits */
int parse_source_26(int parse_int)
{
	int parse_sce = parse_int;
	if((parse_int&0x02000000)>>25 == 1) 
		parse_sce = parse_int+0xfc000000;
	return parse_sce;
}
/* Get the OpCode by optype R+2regs */
int parse_reg_R2(int ins_bin,char **res)
{
	return (parse_ins_reg(res[1]) << 21)
		+ (parse_ins_reg(res[2]) << 16)
		+ ins_bin;
}
/* Get the OpCode by optype R+3regs */
int parse_reg_R3(int ins_bin,char **res)
{
	return (parse_ins_reg(res[2]) << 21)
		+ (parse_ins_reg(res[3]) << 16)
		+ (parse_ins_reg(res[1]) << 11)
		+ ins_bin;
}
/* Get the OpCode by optype R+float+8 */
int parse_reg_Rf8(int ins_bin,char **res)
{
	return (ins_binary_22[ins_bin]<<21)
		+ (parse_ins_reg(res[3]) << 16)
		+ (parse_ins_reg(res[2]) << 11)
		+ (parse_ins_reg(res[1]) << 6)
		+ins_binary_21[ins_bin];
}
/* Get the OpCode by optype R+float+2 */
int parse_reg_Rf2(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[3]) << 21)
		+ (parse_ins_reg(res[1]) << 16)
		+ parse_complement_16(atoi(res[2]));
}
/* Get the OpCode by optype I+immediate */
int parse_reg_Im(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[2]) << 21)
		+ (parse_ins_reg(res[1]) << 16)
		+ parse_complement_16(atoi(res[3]));
}
/* Get the OpCode by optype R+lui */
int parse_reg_Ilui(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[1]) << 16)
		+ parse_complement_16(atoi(res[2]));
}
/* Get the OpCode by optype R+2regs */
int parse_reg_Io2(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[1]) << 21)
		+ (parse_ins_reg(res[2]) << 16)
		+ parse_complement_16(atoi(res[3]));
}
/* Get the OpCode by optype R+2regs+load_mem */
int parse_reg_Irmo(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[3]) << 21)
		+ (parse_ins_reg(res[1]) << 16)
		+ parse_complement_16(atoi(res[2]));
}
/* Get the OpCode by optype R+1regs */
int parse_reg_Io1(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ (parse_ins_reg(res[1]) << 21)
		+ parse_complement_16(atoi(res[2]));
}
/* Get the OpCode by optype J+target */
int	parse_reg_Jtar(int ins_bin,char **res)
{
	return (ins_bin << 26)
		+ parse_complement_26(atoi(res[1]));
}
/* Get the OpCode by optype J+left reg */
int parse_reg_Jlr(int ins_bin,char **res)
{
	return (parse_ins_reg(res[1]) << 21)
		+ ins_bin;
}
/* Get the OpCode by optype J+2 regs */
int parse_reg_Jr2(int ins_bin,char **res)
{
	return (parse_ins_reg(res[1]) << 21)
		+ (parse_ins_reg(res[2]) << 11)
		+ ins_bin;
}
/* Get the OpCode by optype J+right reg */
int parse_reg_Jrr(int ins_bin,char **res)
{
	return (parse_ins_reg(res[1]) << 11)
		+ ins_bin;
}
/* Get the OpCode by optype J+mid reg */
int parse_reg_Jmr(int ins_bin,char **res)
{
	return (0b010000 << 26)
		+ (parse_ins_reg(res[1]) << 16)
		+ (ins_bin << 10);
}
/* Get the OpCode by optype Jend */
int parse_reg_Jend(int ins_bin,char **res)
{
	return 0xffffffff;
}

/*
 * Init and Destroy Functions
 */
/* Init and Alloc Memory */
void mem_init()
{
	mem = (char*)malloc(sizeof(char)*MEM_SIZE);
	if(mem == NULL)
		printf("Fail to Alloc Memory !\n");
	else
		printf("Apply %dB Memory Successfully...\n",MEM_SIZE);
}
/* Destroy the Applied Memory */
void mem_destroy()
{
	free(mem);
	printf("Destroy memory successfully...\n");
}
/* Init and Alloc Registers */
void reg_init()
{
	regs = (int*)malloc(sizeof(int)*REG_SIZE);
	if(regs == NULL)
		printf("Fail to Alloc Registers !\n");
	else
		printf("Apply %d 32bits Registers Successfully...\n",REG_SIZE);
}
/* Destroy All Registers */
void reg_destroy()
{
	free(regs);
	printf("Destroy all registers successfully...\n");
}

/*
 * Memory Write and Read
 */
/* Unsigned char Write and Read */
void write_mem_ubyte(long offset, unsigned char val)
{
	*((unsigned char *)(mem+offset+3)) = val;
}
unsigned char read_mem_ubyte(long offset)
{
	return *((unsigned char *)(mem+offset+3));
}
/* Signed char Write and Read */
void write_mem_byte(long offset, char val)
{
	*((char *)(mem+offset+3)) = val;
}
char read_mem_byte(long offset)
{
	return *((char *)(mem+offset+3));
}
/* Unsigned short Write and Read */
void write_mem_uhword(long offset, unsigned short val)
{
	*((unsigned short *)(mem+offset+2)) = ((val&0x00ff)<<8)+((val&0xff00)>>8);
}
unsigned short read_mem_uhword(long offset)
{
	return (unsigned short)((*(mem+offset+2)<<8)+(*(mem+offset+3)));
}
/* Short int Write and Read */
void write_mem_hword(long offset, short val)
{
	*((short *)(mem+offset+2)) = ((val&0x00ff)<<8)+((val&0xff00)>>8);
}
short read_mem_hword(long offset)
{
	return (short)((*(mem+offset+2)<<8)+(*(mem+offset+3)));
}
/* Unsigned int Write and Read */
void write_mem_uword(long offset, unsigned int val)
{
	*((unsigned int *)(mem+offset)) = ((val&0x000000ff)<<24) + ((val&0x0000ff00)<<8) 
								+ ((val&0x00ff0000)>>8) + ((val&0xff000000)>>24);
}
unsigned int read_mem_uword(long offset)
{
	return (unsigned int)((((*(mem+offset))&0xff)<<24) + (((*(mem+offset+1))&0xff)<<16) 
						+ (((*(mem+offset+2))&0xff)<<8) +(*(mem+offset+3))&0xff);
}
/* Signed int Write and Read */
void write_mem_word(long offset, int val)
{
	*((int *)(mem+offset)) = ((val&0x000000ff)<<24) + ((val&0x0000ff00)<<8) 
								+ ((val&0x00ff0000)>>8) + ((val&0xff000000)>>24);
}
int read_mem_word(long offset)
{
	return (int)((((*(mem+offset))&0xff)<<24) + (((*(mem+offset+1))&0xff)<<16) 
					+ (((*(mem+offset+2))&0xff)<<8) +((*(mem+offset+3))&0xff));
}
/* Float Write and Read */
void write_mem_float(long offset, float val)
{
	*((float *)(mem+offset)) = val;
}
float read_mem_float(long offset)
{
	return *((float *)(mem+offset));
}

/*
 * Load and Store Register File
 */
void store_regs_int(long offset, int val)
{
	*((int *)(regs+offset)) = val;
}
int load_regs_int(long offset)
{
	return *((int *)(regs+offset));
}
void store_regs_uint(long offset, int val)
{
	*((unsigned int *)(regs+offset)) = val;
}
int load_regs_uint(long offset)
{
	return *((unsigned int *)(regs+offset));
}
void store_regs_float(long offset, float val)
{
	*((float *)(regs+offset)) = val;
}
float load_regs_float(long offset)
{
	return *((float *)(regs+offset));
}
void store_regs_double(long offset, double val)
{
	*((double *)(regs+offset)) = val;
}
double load_regs_double(long offset)
{
	return *((double *)(regs+offset));
}

/*
 * MIPS Instruction Set
 */
/* Instruction ADD */
void INSI_ADD(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int((opcode&0x03e00000)>>21) + load_regs_int((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction SUB */
void INSI_SUB(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int((opcode&0x03e00000)>>21) - load_regs_int((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction ADDU */
void INSI_ADDU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_uint((opcode&0x03e00000)>>21) + load_regs_uint((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction SUBU */
void INSI_SUBU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_uint((opcode&0x03e00000)>>21) - load_regs_uint((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction MUL */
void INSI_MUL(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x003e0000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	store_regs_int(HI,((load_regs_int((opcode&0x03e00000)>>21) * load_regs_int(((opcode&0x001f0000)>>16)))&0xffffffff00000000)>>32); //Lo
	store_regs_int(LO,(load_regs_int((opcode&0x03e00000)>>21) * load_regs_int(((opcode&0x001f0000)>>16)))&0xffffffff); //Lo
	pc += 4;
}
/* Instruction DIV */
void INSI_DIV(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x003e0000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	store_regs_int(LO,load_regs_int((opcode&0x03e00000)>>21) / load_regs_int(((opcode&0x001f0000)>>16))); //Lo
	store_regs_int(HI,load_regs_int((opcode&0x03e00000)>>21) % load_regs_int(((opcode&0x001f0000)>>16))); //Hi
	pc += 4;
}
/* Instruction MULU */
void INSI_MULU(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x003e0000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	store_regs_uint(HI,((load_regs_uint((opcode&0x03e00000)>>21) * load_regs_uint(((opcode&0x001f0000)>>16)))&0xffffffff00000000)>>32); //Lo
	store_regs_uint(LO,(load_regs_uint((opcode&0x03e00000)>>21) * load_regs_uint(((opcode&0x001f0000)>>16)))&0xffffffff); //Lo
	pc += 4;
}
/* Instruction DIVU */
void INSI_DIVU(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x003e0000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	store_regs_uint(LO,load_regs_uint((opcode&0x03e00000)>>21) / load_regs_uint(((opcode&0x001f0000)>>16))); //Lo
	store_regs_uint(HI,load_regs_uint((opcode&0x03e00000)>>21) % load_regs_uint(((opcode&0x001f0000)>>16))); //Hi
	pc += 4;
}
/* Instruction AND */
void INSI_AND(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int((opcode&0x03e00000)>>21) & load_regs_int((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction OR */
void INSI_OR(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int((opcode&0x03e00000)>>21) | load_regs_int((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction NOR */
void INSI_NOR(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int(~((opcode&0x03e00000)>>21) | load_regs_int((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction XOR */
void INSI_XOR(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	int *rd = regs + ((opcode&0x0000f800)>>11);
	*rd = load_regs_int((opcode&0x03e00000)>>21) ^ load_regs_int((opcode&0x001f0000)>>16); 
	pc += 4;
}
/* Instruction ADDI */
void INSI_ADDI(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff); 
	pc += 4;
}
/* Instruction ADDIU */
void INSI_ADDIU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = load_regs_uint((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff); 
	pc += 4;
}
/* Instruction ANDI */
void INSI_ANDI(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = load_regs_int((opcode&0x03e00000)>>21) & parse_source_16(opcode&0xffff); 
	pc += 4;
}
/* Instruction ORI */
void INSI_ORI(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = load_regs_int((opcode&0x03e00000)>>21) | parse_source_16(opcode&0xffff); 
	pc += 4;
}
/* Instruction XORI */
void INSI_XORI(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = load_regs_int((opcode&0x03e00000)>>21) ^ parse_source_16(opcode&0xffff); 
	pc += 4;
}
/* Instruction LW */
void INSI_LW(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = read_mem_word(load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff)); 
	pc += 4;
}
/* Instruction SW */
void INSI_SW(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	write_mem_word(load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff),*rd); 
	pc += 4;
}
/* Instruction LBU */
void INSI_LBU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = read_mem_ubyte(load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff)); 
	pc += 4;
}
/* Instruction SB */
void INSI_SB(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	write_mem_byte(load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff),*rd); 
	pc += 4;
}
/* Instruction LB */
void INSI_LB(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	int *rd = regs + ((opcode&0x001f0000)>>16);
	*rd = read_mem_byte(load_regs_int((opcode&0x03e00000)>>21) + parse_source_16(opcode&0xffff)); 
	pc += 4;
}
/* Instruction NOP */
void INSI_NOP(int opcode)
{
	printf("STATE  NULL\n");
	pc += 4;
}
/* Instruction JALR */		
void INSI_JALR(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21));
	store_regs_int(((opcode&0x0000f800)>>11),pc+4);
	pc = load_regs_int((opcode&0x03e00000)>>21);
}
/* Instruction JR */			
void INSI_JR(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x03e00000)>>21));
	pc = load_regs_int((opcode&0x03e00000)>>21);
}
/* Instruction JAL */			
void INSI_JAL(int opcode)
{
	printf("STATE  %d\n",jump_pc[check_in_jumps(parse_source_26(opcode&0x03ffffff))]);
	store_regs_int(31,pc + 4);
	pc = jump_pc[check_in_jumps(parse_source_26(opcode&0x03ffffff))];
}
/* Instruction J */			
void INSI_J(int opcode)
{
	printf("STATE  %d\n",jump_pc[check_in_jumps(parse_source_26(opcode&0x03ffffff))]);
	pc = jump_pc[check_in_jumps(parse_source_26(opcode&0x03ffffff))];
}
/* Instruction LUI */		
void INSI_LUI(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x001f0000)>>16),parse_source_16(opcode&0xffff));
	store_regs_int(((opcode&0x001f0000)>>16),parse_source_16(opcode&0xffff));
	pc += 4;
}
/* Instruction BEQ */			
void INSI_BEQ(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16),parse_complement_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) == load_regs_int((opcode&0x001f0000)>>16))
		pc = pc + ((parse_complement_16(opcode&0xffff))<<2);
	else
		pc += 4;
}
/* Instruction BNE */		
void INSI_BNE(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16),parse_complement_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) != load_regs_int((opcode&0x001f0000)>>16))
		pc = pc + ((parse_complement_16(opcode&0xffff))<<2);
	else
		pc += 4;
}
/* Instruction BLEZ */			
void INSI_BLEZ(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x03e00000)>>21),parse_complement_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) <= 0)
		pc = pc + ((parse_complement_16(opcode&0xffff))<<2);
	else
		pc += 4;
}
/* Instruction BGTZ */		
void INSI_BGTZ(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x03e00000)>>21),parse_complement_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) > 0)
		pc = pc + ((parse_complement_16(opcode&0xffff))<<2);
	else
		pc += 4;
}
/* Instruction BLTZ */			
void INSI_BLTZ(int opcode)
{
	printf("STATE  %d %d\n",load_regs_int((opcode&0x03e00000)>>21),parse_complement_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) < 0)
		pc = pc + ((parse_complement_16(opcode&0xffff))<<2);
	else
		pc += 4;
}
/* Instruction MFHI */			
void INSI_MFHI(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x0000f800)>>11));
	store_regs_int(((opcode&0x0000f800)>>11),load_regs_int(HI));	//Hi
	pc += 4;
}
/* Instruction MFLO */		
void INSI_MFLO(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x0000f800)>>11));
	store_regs_int(((opcode&0x0000f800)>>11),load_regs_int(LO));	//Lo
	pc += 4;
}
/* Instruction MFEPC */			
void INSI_MFEPC(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x001f0000)>>16));
	store_regs_int(((opcode&0x001f0000)>>16),load_regs_int(EPC));	//EPC
	pc += 4;
}
/* Instruction MFCO */		
void INSI_MFCO(int opcode)
{
	printf("STATE  %d\n",load_regs_int((opcode&0x001f0000)>>16));
	store_regs_int(((opcode&0x001f0000)>>16),load_regs_int(CO));	//Co
	pc += 4;
}
/* Instruction SLT */		
void INSI_SLT(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	if(load_regs_int((opcode&0x03e00000)>>21) < load_regs_int((opcode&0x001f0000)>>16))
		store_regs_int(((opcode&0x0000f800)>>11),1);
	else
		store_regs_int(((opcode&0x0000f800)>>11),0);	
	pc += 4;
}
/* Instruction SLTU */		
void INSI_SLTU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x0000f800)>>11),load_regs_int((opcode&0x03e00000)>>21),load_regs_int((opcode&0x001f0000)>>16));
	if(load_regs_uint((opcode&0x03e00000)>>21) < load_regs_uint((opcode&0x001f0000)>>16))
		store_regs_uint(((opcode&0x0000f800)>>11),1);
	else
		store_regs_uint(((opcode&0x0000f800)>>11),0);	
	pc += 4;
}
/* Instruction SLTI */		
void INSI_SLTI(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	if(load_regs_int((opcode&0x03e00000)>>21) < load_regs_int(parse_complement_16(opcode&0xffff)))
		store_regs_int(((opcode&0x001f0000)>>16),1);
	else
		store_regs_int(((opcode&0x001f0000)>>16),0);	
	pc += 4;
}
/* Instruction SLTIU */		
void INSI_SLTIU(int opcode)
{
	printf("STATE  %d %d %d\n",load_regs_int((opcode&0x001f0000)>>16),load_regs_int((opcode&0x03e00000)>>21),parse_source_16(opcode&0xffff));
	if(load_regs_uint((opcode&0x03e00000)>>21) < load_regs_uint(parse_complement_16(opcode&0xffff)))
		store_regs_uint(((opcode&0x001f0000)>>16),1);
	else
		store_regs_uint(((opcode&0x001f0000)>>16),0);	
	pc += 4;
}
/* Instruction ADD_D */		
void INSI_ADD_D(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_double((opcode&0x00007c0)>>6),load_regs_double((opcode&0x0000f800)>>11),load_regs_double((opcode&0x001f0000)>>16));
	store_regs_double((opcode&0x000007c0)>>6,load_regs_double((opcode&0x0000f800)>>11) + load_regs_double((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction ADD_S */		
void INSI_ADD_S(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_float((opcode&0x00007c0)>>6),load_regs_float((opcode&0x0000f800)>>11),load_regs_float((opcode&0x001f0000)>>16));
	store_regs_float((opcode&0x000007c0)>>6,load_regs_float((opcode&0x0000f800)>>11) + load_regs_float((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction SUB_D */		
void INSI_SUB_D(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_double((opcode&0x00007c0)>>6),load_regs_double((opcode&0x0000f800)>>11),load_regs_double((opcode&0x001f0000)>>16));
	store_regs_double((opcode&0x000007c0)>>6,load_regs_double((opcode&0x0000f800)>>11) - load_regs_double((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction SUB_S */		
void INSI_SUB_S(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_float((opcode&0x00007c0)>>6),load_regs_float((opcode&0x0000f800)>>11),load_regs_float((opcode&0x001f0000)>>16));
	store_regs_float((opcode&0x000007c0)>>6,load_regs_float((opcode&0x0000f800)>>11) - load_regs_float((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction MUL_D */		
void INSI_MUL_D(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_double((opcode&0x00007c0)>>6),load_regs_double((opcode&0x0000f800)>>11),load_regs_double((opcode&0x001f0000)>>16));
	store_regs_double((opcode&0x000007c0)>>6,load_regs_double((opcode&0x0000f800)>>11) * load_regs_double((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction MUL_S */		
void INSI_MUL_S(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_float((opcode&0x00007c0)>>6),load_regs_float((opcode&0x0000f800)>>11),load_regs_float((opcode&0x001f0000)>>16));
	store_regs_float((opcode&0x000007c0)>>6,load_regs_float((opcode&0x0000f800)>>11) * load_regs_float((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction DIV_D */		
void INSI_DIV_D(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_double((opcode&0x00007c0)>>6),load_regs_double((opcode&0x0000f800)>>11),load_regs_double((opcode&0x001f0000)>>16));
	store_regs_double((opcode&0x000007c0)>>6,load_regs_double((opcode&0x0000f800)>>11) / load_regs_double((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction DIV_S */		
void INSI_DIV_S(int opcode)
{
	printf("STATE  %f %f %f\n",load_regs_float((opcode&0x00007c0)>>6),load_regs_float((opcode&0x0000f800)>>11),load_regs_float((opcode&0x001f0000)>>16));
	store_regs_float((opcode&0x000007c0)>>6,load_regs_float((opcode&0x0000f800)>>11) / load_regs_float((opcode&0x001f0000)>>16)); 
	pc += 4;
}
/* Instruction LWC1 */		
void INSI_LWC1(int opcode)
{
	printf("STATE  %f %d %d\n",load_regs_float((opcode&0x001f0000)>>16),parse_complement_16(opcode&0xffff),load_regs_int((opcode&0x03e00000)>>21));
	store_regs_float((opcode&0x001f0000)>>16,
			read_mem_float(load_regs_int((opcode&0x03e00000)>>21)
				+parse_complement_16(opcode&0xffff)));
	pc += 4;
}
/* Instruction SWC1 */		
void INSI_SWC1(int opcode)
{
	printf("STATE  %f %d %d\n",load_regs_float((opcode&0x001f0000)>>16),parse_complement_16(opcode&0xffff),load_regs_int((opcode&0x03e00000)>>21));
	write_mem_float(load_regs_int((opcode&0x03e00000)>>21) + parse_complement_16(opcode&0xffff),
			load_regs_float((opcode&0x001f0000)>>16)); 
	pc += 4;
}
