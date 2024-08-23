#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Rformat(unsigned int value, unsigned int opcode);
void Iformat(unsigned int value, unsigned int opcode);
void Sformat(unsigned int value, unsigned int opcode);
void Uformat(unsigned int value, unsigned int opcode);
void Jformat(unsigned int value, unsigned int opcode);
void Bformat(unsigned int value, unsigned int opcode);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
	}
	else
	{
		char *input = argv[1];
		FILE *file = fopen(input, "rb");
		unsigned char buffer[4];
		unsigned int opcode;
		int inst_num = 0;

		if (file == NULL)
		{
			fprintf(stderr, "Cannot open file\n");
			return 1;
		}

		while (fread(buffer, sizeof(buffer), 1, file))
		{

			printf("inst %d: ", inst_num);

			unsigned int value = 0;

			value |= buffer[0];
			value |= buffer[1] << 8;
			value |= buffer[2] << 16;
			value |= buffer[3] << 24;

			// for (int bit = 31; bit >= 0; bit--) {
			//     printf("%d", (value >> bit) & 1);
			// }
			for (int i = 7; i >= 0; i--)
			{													   // 32비트는 8자리의 16진수이므로, i는 7부터 시작하여 0까지 갑니다.
				unsigned int hexDigit = (value >> (i * 4)) & 0x0F; // 4비트씩 추출합니다.
				printf("%x", hexDigit);							   // 추출한 값을 16진수로 출력합니다.
			}
			printf(" ");

			opcode = value & 0x7F;
			if (opcode == 51)
			{
				Rformat(value, opcode);
			}
			else if (opcode == 3 || opcode == 103 || opcode == 19)
			{
				Iformat(value, opcode);
			}
			else if (opcode == 35)
			{
				Sformat(value, opcode);
			}
			else if (opcode == 55 || opcode == 23)
			{
				Uformat(value, opcode);
			}
			else if (opcode == 111)
			{
				Jformat(value, opcode);
			}
			else if (opcode == 99)
			{
				Bformat(value, opcode);
			}
			else
			{
				printf("unknown instruction");
			}
			inst_num += 1;
			printf("\n");
		}
	}

	return 0;
}

void Rformat(unsigned int value, unsigned int opcode)
{
	unsigned int rd;
	unsigned int funct3;
	unsigned int rs1;
	unsigned int rs2;
	unsigned int funct7;
	char *inst;

	rd = (value >> 7) & 0x1F;
	funct3 = (value >> 12) & 0x07;
	rs1 = (value >> 15) & 0x1F;
	rs2 = (value >> 20) & 0x1F;
	funct7 = (value >> 25) & 0x7F;

	if (funct3 == 0)
	{
		if (funct7 == 0)
		{
			inst = "add";
		}
		else if (funct7 == 32)
		{
			inst = "sub";
		}
		else
		{
			inst = "unknown instruction";
		}
	}
	else if (funct3 == 1)
	{
		inst = "sll";
	}
	else if (funct3 == 2)
	{
		inst = "slt";
	}
	else if (funct3 == 3)
	{
		inst = "sltu";
	}
	else if (funct3 == 4)
	{
		inst = "xor";
	}
	else if (funct3 == 5)
	{
		if (funct7 == 0)
		{
			inst = "srl";
		}
		else if (funct7 == 32)
		{
			inst = "sra";
		}
		else
		{
			inst = "unknown instruction";
		}
	}
	else if (funct3 == 6)
	{
		inst = "or";
	}
	else if (funct3 == 7)
	{
		inst = "and";
	}
	else
	{
		inst = "unknown instruction";
	}

	if (strcmp(inst, "unknown instruction") == 0)
	{
		printf("%s", inst);
	}
	else
	{
		printf("%s x%d, x%d, x%d", inst, rd, rs1, rs2);
	}
}

void Iformat(unsigned int value, unsigned int opcode)
{
	unsigned int rd;
	unsigned int funct3;
	unsigned int rs1;
	signed int imm;
	char *inst;

	rd = (value >> 7) & 0x1F;
	funct3 = (value >> 12) & 0x07;
	rs1 = (value >> 15) & 0x1F;
	imm = (value >> 20) & 0xFFF;

	if (imm & (1 << 11))
	{
		imm |= 0xFFFFF000; // Set the upper 20 bits to 1 for negative numbers
	}

	if (opcode == 3)
	{
		if (funct3 == 0)
		{
			inst = "lb";
		}
		else if (funct3 == 1)
		{
			inst = "lh";
		}
		else if (funct3 == 2)
		{
			inst = "lw";
		}
		else if (funct3 == 4)
		{
			inst = "lbu";
		}
		else if (funct3 == 5)
		{
			inst = "lhu";
		}
		else
		{
			inst = "unknwon instruction";
		}
	}
	else if (opcode == 19)
	{
		if (funct3 == 0)
		{
			inst = "addi";
		}
		else if (funct3 == 2)
		{
			inst = "slti";
		}
		else if (funct3 == 3)
		{
			inst = "sltiu";
		}
		else if (funct3 == 4)
		{
			inst = "xori";
		}
		else if (funct3 == 6)
		{
			inst = "ori";
		}
		else if (funct3 == 7)
		{
			inst = "andi";
		}
		else if (funct3 == 1 || funct3 == 5)
		{

			unsigned int shamt = (value >> 20) & 0x1F;	// shamt 필드는 5비트입니다.
			unsigned int funct7 = (value >> 25) & 0x7F; // funct7 필드는 7비트입니다.

			if (funct3 == 1)
			{
				inst = "slli";
				printf("%s x%d, x%d, %d", inst, rd, rs1, shamt);
				return;
			}
			else if (funct3 == 5)
			{

				if (funct7 == 0)
				{
					inst = "srli";
					printf("%s x%d, x%d, %d", inst, rd, rs1, shamt);
					return;
				}
				else if (funct7 == 32)
				{
					inst = "srai";
					printf("%s x%d, x%d, %d", inst, rd, rs1, shamt);
					return;
				}
				else
				{
					inst = "unknown instruction";
				}
			}
		}
	}
	else if (opcode == 103)
	{
		inst = "jalr";
	}
	else
	{
		inst = "unknwon instruction";
	}

	if (strcmp(inst, "unknwon instruction") == 0)
	{
		printf("%s", inst);
	}
	else if (opcode == 3 || opcode == 103)
	{
		printf("%s x%d, %d(x%d)", inst, rd, imm, rs1);
	}
	else
	{
		printf("%s x%d, x%d, %d", inst, rd, rs1, imm);
	}
}

void Sformat(unsigned int value, unsigned int opcode)
{

	unsigned int rs2, rs1, funct3;
	signed int imm; // I 타입 명령어의 12비트 즉흥 필드를 저장할 변수
	char *inst;

	// 필드 추출
	rs2 = (value >> 20) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'
	rs1 = (value >> 15) & 0x1F;
	funct3 = (value >> 12) & 0x07; // 마스크 0x07는 '00000 00000 00000 00000 00000 00111'

	// imm 필드 추출: 두 부분을 합쳐 12비트 값을 얻습니다.
	unsigned int imm4_0 = (value >> 7) & 0x1F;	 // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'
	unsigned int imm11_5 = (value >> 25) & 0x7F; // 마스크 0x7F는 '00000 00000 00000 00000 01111 11111'

	// imm 필드 합치기
	imm = (imm11_5 << 5) | imm4_0;

	// 만약 imm 필드가 부호 있는 수라면, 부호 확장을 해야 할 수도 있습니다.
	// 여기서 imm이 음수라면 높은 비트(비트 11)를 전체 32비트 변수에 확장해야 합니다.
	if (imm & (1 << 11))
	{					   // 만약 비트 11이 설정되어 있다면
		imm |= 0xFFFFF000; // 상위 비트를 1로 설정합니다.
	}

	if (funct3 == 0)
	{
		inst = "sb";
	}
	else if (funct3 == 1)
	{
		inst = "sh";
	}
	else if (funct3 == 2)
	{
		inst = "sw";
	}
	else
	{
		inst = "unknwon instruction";
	}

	if (strcmp(inst, "unknown instruction") == 0)
	{
		printf("%s", inst);
	}
	else
	{
		printf("%s x%d, %d(x%d)", inst, rs2, imm, rs1);
	}
}

void Uformat(unsigned int value, unsigned int opcode)
{

	// 가정: instruction 변수에 이미 어떤 32비트 명령어 값이 저장되어 있다.

	unsigned int rd;
	unsigned int imm; // U 타입 명령어의 20비트 즉흥 필드를 저장할 변수
	char *inst;

	// 필드 추출
	rd = (value >> 7) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'

	// imm 필드 추출
	imm = (value >> 12) & 0xFFFFF; // 마스크 0xFFFFF는 '00000 11111 11111 11111 11111 11111'

	// 만약 imm 필드가 부호 있는 수라면, 부호 확장을 해야 할 수도 있습니다.
	// 여기서 imm이 음수라면 높은 비트(비트 31)를 전체 32비트 변수에 확장해야 합니다.
	// if (imm & (1 << 19)) { // 만약 비트 19이 설정되어 있다면
	//     imm |= 0xFFF00000; // 상위 비트를 1로 설정합니다.
	// }

	imm = imm << 12;

	if (opcode == 55)
	{
		inst = "lui";
	}
	else if (opcode == 23)
	{
		inst = "auipc";
	}
	else
	{
		inst = "unknown instruction";
	}

	if (strcmp(inst, "unknown instruction") == 0)
	{
		printf("%s", inst);
	}
	else
	{
		printf("%s x%d, %d", inst, rd, imm);
	}
}

void Jformat(unsigned int value, unsigned int opcode)
{
	// 가정: instruction 변수에 이미 어떤 32비트 명령어 값이 저장되어 있다.

	unsigned int rd;
	signed int imm; // J 타입 명령어의 20비트 즉흥 필드를 저장할 변수
	char *inst;

	// 필드 추출
	rd = (value >> 7) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'

	// imm 필드 추출
	// imm  = (value & 0xFF000000) >> 12; // imm[19:12] -> imm[31:24]
	// imm |= (value & 0x000FF000);       // imm[19:12]
	// imm |= ((value >> 9) & 0x800);     // imm[11]    -> imm[11]
	// imm |= ((value >> 20) & 0x7FE);    // imm[10:1]  -> imm[10:1]

	imm = ((value >> 31) & 0x1) << 20	  // imm[20]
		  | ((value >> 21) & 0x3FF) << 1  // imm[10:1]
		  | ((value >> 20) & 0x1) << 11	  // imm[11]
		  | ((value >> 12) & 0xFF) << 12; // imm[19:12]

	// 만약 imm 필드가 부호 있는 수라면, 부호 확장을 해야 할 수도 있습니다.
	// 여기서 imm이 음수라면 높은 비트(비트 31)를 전체 32비트 변수에 확장해야 합니다.
	if (imm & (1 << 20))
	{					   // 만약 비트 20이 설정되어 있다면
		imm |= 0xFFE00000; // 상위 비트를 1로 설정합니다.
	}

	inst = "jal";

	printf("%s x%d, %d", inst, rd, imm);
}

void Bformat(unsigned int value, unsigned int opcode)
{

	unsigned int rs1, rs2, funct3;
	signed int imm; // 이 변수는 12비트 즉시 값(imm)을 저장합니다.
	char *inst;

	// 필드 추출
	funct3 = (value >> 12) & 0x7; // 마스크 0x7는  '0000000000000000000000000111'
	rs1 = (value >> 15) & 0x1F;	  // 마스크 0x1F는 '000000000000000000000111111'
	rs2 = (value >> 20) & 0x1F;	  // 마스크 0x1F는 '000000000000000000000111111'

	// imm 필드의 비트를 추출
	imm = (value >> 31) & 0x800;  // imm[12]    -> imm[11]
	imm |= (value >> 7) & 0x1E;	  // imm[4:1]   -> imm[4:1]
	imm |= (value >> 20) & 0x7E0; // imm[10:5]  -> imm[10:5]
	imm |= (value << 4) & 0x800;  // imm[11]    -> imm[11]

	// 만약 imm 필드가 부호 있는 수라면, 부호 확장을 수행해야 합니다.
	if (imm & (1 << 11))
	{					   // imm[11]이 1이라면
		imm |= 0xFFFFF000; // 상위 비트를 1로 설정하여 부호 확장을 합니다.
	}

	if (funct3 == 0)
	{
		inst = "beq";
	}
	else if (funct3 == 1)
	{
		inst = "bne";
	}
	else if (funct3 == 4)
	{
		inst = "blt";
	}
	else if (funct3 == 5)
	{
		inst = "bge";
	}
	else if (funct3 == 6)
	{
		inst = "bltu";
	}
	else if (funct3 == 7)
	{
		inst = "bgeu";
	}
	else
	{
		inst = "unknown instruction";
	}

	if (strcmp(inst, "unknown instruction") == 0)
	{
		printf("%s", inst);
	}
	else
	{
		printf("%s x%d, x%d, %d", inst, rs1, rs2, imm);
	}
}
