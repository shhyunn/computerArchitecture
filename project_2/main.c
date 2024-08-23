#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MEMORY_SIZE (64 * 1024) // 64KB

void Rformat(unsigned int value, unsigned int opcode);
void Iformat(unsigned int value, unsigned int opcode);
void Sformat(unsigned int value, unsigned int opcode);
void Uformat(unsigned int value, unsigned int opcode);
void Jformat(unsigned int value, unsigned int opcode);
void Bformat(unsigned int value, unsigned int opcode);

int reg[32] = {0x0};
int PC = 0;
uint8_t memory[MEMORY_SIZE];

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    }
    else
    {
        char *input = argv[1];   // input file
        int inst_num;            // inst 개수
        char *input_data = NULL; // data file
        int inst_temp = 0;       // temp inst 개수

        if (argc == 3)
        { // 데이터가 없는 경우
            inst_num = atoi(argv[2]);
        }
        else if (argc == 4)
        {
            inst_num = atoi(argv[3]);
            input_data = argv[2]; // 데이터 파일 이름
        }

        FILE *file = fopen(input, "rb"); // inst input file

        if (file == NULL)
        {
            fprintf(stderr, "Cannot open file\n");
            return 1;
        }

        FILE *data = NULL; // data file pointer

        memset(memory, 0xFF, MEMORY_SIZE); // memory setting

        if (input_data != NULL)
        {                                   // data file이 null이 아닐 경우
            data = fopen(input_data, "rb"); // 데이터 읽기

            size_t bytes_read = fread(memory, 1, MEMORY_SIZE, data);

            if (bytes_read == 0)
            {
                fputs("Reading file failed", stderr);
                fclose(data);
                return EXIT_FAILURE;
            }

            // 파일 닫기
            fclose(data);
        }

        unsigned char buffer[4];
        unsigned int opcode;

        while (fread(buffer, sizeof(buffer), 1, file))
        {

            if (inst_temp == inst_num)
            {
                break;
            }

            unsigned int value = 0;

            value |= buffer[0];
            value |= buffer[1] << 8;
            value |= buffer[2] << 16;
            value |= buffer[3] << 24;

            // for (int i = 7; i >= 0; i--) {  // 32비트는 8자리의 16진수이므로, i는 7부터 시작하여 0까지 갑니다.
            //     unsigned int hexDigit = (value >> (i * 4)) & 0x0F;  // 4비트씩 추출합니다.
            //     printf("%x\n", hexDigit);  // 추출한 값을 16진수로 출력합니다.
            // }
            // printf(" ");
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

            fseek(file, PC, SEEK_SET);

            inst_temp += 1;

            // if (inst_temp == 34) {
            //     for (int i = 7; i >= 0; i--) {  // 32비트는 8자리의 16진수이므로, i는 7부터 시작하여 0까지 갑니다.
            //         unsigned int hexDigit = (value >> (i * 4)) & 0x0F;  // 4비트씩 추출합니다.
            //         printf("%x\n", hexDigit);  // 추출한 값을 16진수로 출력합니다.
            // }
            // printf(" ");

            // }
            // printf("%d\n", inst_temp);
        }

        for (int i = 0; i < 32; i++)
        {
            int number = reg[i];
            char str[9]; // 8자리 숫자 + 널 종료 문자('\0')를 위한 공간
            sprintf(str, "%08x", number);
            printf("x%d: 0x%s\n", i, str);
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
    // char *inst;

    rd = (value >> 7) & 0x1F;
    funct3 = (value >> 12) & 0x07;
    rs1 = (value >> 15) & 0x1F; // 5bit
    rs2 = (value >> 20) & 0x1F; // 5bit
    funct7 = (value >> 25) & 0x7F;

    if (funct3 == 0)
    {
        if (funct7 == 0)
        {
            // inst = "add";
            reg[rd] = reg[rs1] + reg[rs2];
        }
        else if (funct7 == 32)
        {
            // inst = "sub";
            reg[rd] = reg[rs1] - reg[rs2];
        }
        else
        {
            printf("unknown instruction\n");
            // inst = "unknown instruction";
        }
    }
    else if (funct3 == 1)
    {
        // inst = "sll";
        // unsigned int urs1 = reg[rs1];
        // unsigned int urs2 = reg[rs2] & 0x1F;

        // reg[rd] = urs1 << urs2;

        reg[rd] = reg[rs1] << (reg[rs2] & 0x1F);
    }
    else if (funct3 == 2)
    {
        // inst = "slt";
        if (reg[rs1] < reg[rs2])
        {
            reg[rd] = 0x1;
        }
        else
        {
            reg[rd] = 0x0;
        }
    }
    else if (funct3 == 3)
    {
        // inst = "sltu";
        unsigned int urs1 = reg[rs1];
        unsigned int urs2 = reg[rs2];

        if (urs1 < urs2)
        {
            reg[rd] = 0x1;
        }
        else
        {
            reg[rd] = 0x0;
        }
    }
    else if (funct3 == 4)
    {
        // inst = "xor";
        reg[rd] = reg[rs1] ^ reg[rs2];
    }
    else if (funct3 == 5)
    {
        if (funct7 == 0)
        {
            // inst = "srl";
            unsigned int urs1 = reg[rs1];
            unsigned int urs2 = reg[rs2] & 0x1F;
            reg[rd] = urs1 >> urs2;
        }
        else if (funct7 == 32)
        {
            // inst = "sra";

            reg[rd] = reg[rs1] >> (reg[rs2] & 0x1F);
        }
        else
        {
            printf("unknown instruction");
        }
    }
    else if (funct3 == 6)
    {
        // inst = "or";
        reg[rd] = reg[rs1] | reg[rs2];
    }
    else if (funct3 == 7)
    {
        // inst = "and";
        reg[rd] = reg[rs1] & reg[rs2];
    }
    else
    {
        printf("unknown instruction");
    }

    reg[0] = 0x0;
    PC += 4;
}

void Iformat(unsigned int value, unsigned int opcode)
{
    unsigned int rd;
    unsigned int funct3;
    unsigned int rs1;
    signed int imm;
    // char* inst;

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
        if (funct3 == 2)
        {
            // inst = "lw";
            int base = reg[rs1] - 0x10000000;

            if (reg[rs1] + imm == 0x20000000)
            {
                int user_num;
                scanf("%d", &user_num);
                reg[rd] = user_num;
            }
            else
            {
                // 메모리에서 4바이트 정수 읽기 (little endian 방식)
                int32_t num;
                memcpy(&num, &memory[base + imm], sizeof(int32_t));
                reg[rd] = num;
            }
        }
        else
        {
            printf("unknown instruction");
        }
    }
    else if (opcode == 19)
    {
        if (funct3 == 0)
        {
            // inst = "addi";
            reg[rd] = reg[rs1] + imm;
        }
        else if (funct3 == 2)
        {
            // inst = "slti";
            if (reg[rs1] < imm)
            {
                reg[rd] = 0x1;
            }
            else
            {
                reg[rd] = 0x0;
            }
        }
        else if (funct3 == 4)
        {
            // inst = "xori";
            reg[rd] = reg[rs1] ^ imm;
        }
        else if (funct3 == 6)
        {
            // inst = "ori";
            reg[rd] = reg[rs1] | imm;
        }
        else if (funct3 == 7)
        {
            // inst = "andi";
            reg[rd] = reg[rs1] & imm;
        }
        else if (funct3 == 1 || funct3 == 5)
        {

            unsigned int shamt = (value >> 20) & 0x1F; // shamt 필드는 5비트입니다.
            int sshamt = (value >> 20) & 0x1F;
            unsigned int funct7 = (value >> 25) & 0x7F; // funct7 필드는 7비트입니다.

            if (funct3 == 1)
            {
                // inst = "slli";
                reg[rd] = reg[rs1] << shamt;

                // printf("%s x%d, x%d, %d",inst, rd, rs1, shamt);
                // return;
            }
            else if (funct3 == 5)
            {

                if (funct7 == 0)
                {
                    // inst = "srli";

                    unsigned int urs1 = reg[rs1];
                    reg[rd] = urs1 >> shamt;
                    // printf("%s x%d, x%d, %d",inst, rd, rs1, shamt);
                    // return;
                }
                else if (funct7 == 32)
                {
                    // inst = "srai";
                    reg[rd] = reg[rs1] >> sshamt;

                    // printf("%s x%d, x%d, %d",inst, rd, rs1, shamt);
                    // return;
                }
                else
                {
                    printf("unknown instruction");
                }
            }
        }
    }
    else if (opcode == 103)
    {
        // inst = "jalr";
        reg[rd] = PC + 4;
        PC = reg[rs1] + imm;
    }
    else
    {
        printf("unknown instruction");
    }

    reg[0] = 0x0;

    if (opcode != 103)
    {
        PC += 4;
    }
}

void Sformat(unsigned int value, unsigned int opcode)
{

    unsigned int rs2, rs1, funct3;
    signed int imm; // I 타입 명령어의 12비트 즉흥 필드를 저장할 변수
    // char* inst;

    // 필드 추출
    rs2 = (value >> 20) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'
    rs1 = (value >> 15) & 0x1F;
    funct3 = (value >> 12) & 0x07; // 마스크 0x07는 '00000 00000 00000 00000 00000 00111'

    // imm 필드 추출: 두 부분을 합쳐 12비트 값을 얻습니다.
    unsigned int imm4_0 = (value >> 7) & 0x1F;   // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'
    unsigned int imm11_5 = (value >> 25) & 0x7F; // 마스크 0x7F는 '00000 00000 00000 00000 01111 11111'

    // imm 필드 합치기
    imm = (imm11_5 << 5) | imm4_0;

    // 만약 imm 필드가 부호 있는 수라면, 부호 확장을 해야 할 수도 있습니다.
    // 여기서 imm이 음수라면 높은 비트(비트 11)를 전체 32비트 변수에 확장해야 합니다.
    if (imm & (1 << 11))
    {                      // 만약 비트 11이 설정되어 있다면
        imm |= 0xFFFFF000; // 상위 비트를 1로 설정합니다.
    }

    if (funct3 == 2)
    {
        // inst = "sw";
        int base = reg[rs1] - 0x10000000;

        int32_t reg_num = reg[rs2];

        int8_t num4 = (int8_t)(reg_num >> 0);
        int8_t num3 = (int8_t)(reg_num >> 8);
        int8_t num2 = (int8_t)(reg_num >> 16);
        int8_t num1 = (int8_t)(reg_num >> 24);

        if ((reg[rs1] + imm) == 0x20000000)
        {
            printf("%c", reg_num);
        }
        else
        {
            memory[base + imm + 0] = num4;
            memory[base + imm + 1] = num3;
            memory[base + imm + 2] = num2;
            memory[base + imm + 3] = num1;
        }
    }
    else
    {
        printf("unknown instruction");
    }

    PC += 4;
    reg[0] = 0x0;
}

void Uformat(unsigned int value, unsigned int opcode)
{

    // 가정: instruction 변수에 이미 어떤 32비트 명령어 값이 저장되어 있다.

    unsigned int rd;
    unsigned int imm; // U 타입 명령어의 20비트 즉흥 필드를 저장할 변수
    // char *inst;

    // 필드 추출
    rd = (value >> 7) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'

    // imm 필드 추출
    imm = (value >> 12) & 0xFFFFF; // 마스크 0xFFFFF는 '00000 11111 11111 11111 11111 11111'

    imm = imm << 12;

    if (opcode == 55)
    {
        // inst = "lui";
        reg[rd] = imm;
    }
    else if (opcode == 23)
    {
        // inst = "auipc";
        reg[rd] = PC + imm;
    }
    else
    {
        printf("unknown instruction");
    }

    PC += 4;
    reg[0] = 0x0;
}

void Jformat(unsigned int value, unsigned int opcode)
{
    // 가정: instruction 변수에 이미 어떤 32비트 명령어 값이 저장되어 있다.

    unsigned int rd;
    signed int imm; // J 타입 명령어의 20비트 즉흥 필드를 저장할 변수
    // char* inst;

    // 필드 추출
    rd = (value >> 7) & 0x1F; // 마스크 0x1F는 '00000 00000 00000 00000 00000 11111'

    imm = ((value >> 31) & 0x1) << 20     // imm[20]
          | ((value >> 21) & 0x3FF) << 1  // imm[10:1]
          | ((value >> 20) & 0x1) << 11   // imm[11]
          | ((value >> 12) & 0xFF) << 12; // imm[19:12]

    // 만약 imm 필드가 부호 있는 수라면, 부호 확장을 해야 할 수도 있습니다.
    // 여기서 imm이 음수라면 높은 비트(비트 31)를 전체 32비트 변수에 확장해야 합니다.
    if (imm & (1 << 20))
    {                      // 만약 비트 20이 설정되어 있다면
        imm |= 0xFFE00000; // 상위 비트를 1로 설정합니다.
    }

    // inst = "jal";
    reg[rd] = PC + 4;
    PC += imm;
    reg[0] = 0x0;
}

void Bformat(unsigned int value, unsigned int opcode)
{

    unsigned int rs1, rs2, funct3;
    signed int imm; // 이 변수는 12비트 즉시 값(imm)을 저장합니다.
    // char* inst;

    // 필드 추출
    funct3 = (value >> 12) & 0x7; // 마스크 0x7는  '0000000000000000000000000111'
    rs1 = (value >> 15) & 0x1F;   // 마스크 0x1F는 '000000000000000000000111111'
    rs2 = (value >> 20) & 0x1F;   // 마스크 0x1F는 '000000000000000000000111111'

    // imm 필드의 비트를 추출
    imm = (value >> 31) & 0x800;  // imm[12]    -> imm[11]
    imm |= (value >> 7) & 0x1E;   // imm[4:1]   -> imm[4:1]
    imm |= (value >> 20) & 0x7E0; // imm[10:5]  -> imm[10:5]
    imm |= (value << 4) & 0x800;  // imm[11]    -> imm[11]

    // 만약 imm 필드가 부호 있는 수라면, 부호 확장을 수행해야 합니다.
    if (imm & (1 << 11))
    {                      // imm[11]이 1이라면
        imm |= 0xFFFFF000; // 상위 비트를 1로 설정하여 부호 확장을 합니다.
    }

    if (funct3 == 0)
    {
        // inst = "beq";
        if (reg[rs1] == reg[rs2])
        {
            PC = PC + imm;
        }
        else
        {
            PC = PC + 4;
        }
    }
    else if (funct3 == 1)
    {
        // inst = "bne";
        if (reg[rs1] != reg[rs2])
        {
            PC = PC + imm;
        }
        else
        {
            PC = PC + 4;
        }
    }
    else if (funct3 == 4)
    {
        // inst = "blt";
        if (reg[rs1] - reg[rs2] < 0)
        {
            PC = PC + imm;
        }
        else
        {
            PC = PC + 4;
        }
    }
    else if (funct3 == 5)
    {
        // inst = "bge";
        if (reg[rs1] - reg[rs2] >= 0)
        {
            PC = PC + imm;
        }
        else
        {
            PC = PC + 4;
        }
    }
    else
    {
        printf("unknown instruction");
    }
    reg[0] = 0x0;
}