#include <stdio.h>
#include <stdlib.h>

void illegal_opcode(const char *opcode, const char *description) {
    printf("Illegal opcode %s (%s)\r\n", opcode, description);
    exit(1);
}


void OPCODE_02(void) { illegal_opcode("0x02", "???"); }
void OPCODE_03(void) { illegal_opcode("0x03", "???"); }
void OPCODE_04(void) { illegal_opcode("0x04", "???"); }
void OPCODE_07(void) { illegal_opcode("0x07", "SLO Zero Page"); }
void OPCODE_0B(void) { illegal_opcode("0x0B", "ASL Accumulator"); }
void OPCODE_0C(void) { illegal_opcode("0x0C", "NOP (Absolute)"); }
void OPCODE_0F(void) { illegal_opcode("0x0F", "???"); }
void OPCODE_12(void) { illegal_opcode("0x12", "???"); }
void OPCODE_13(void) { illegal_opcode("0x13", "???"); }
void OPCODE_14(void) { illegal_opcode("0x14", "???"); }
void OPCODE_1A(void) { illegal_opcode("0x1A", "???"); }
void OPCODE_1B(void) { illegal_opcode("0x1B", "NOP"); }
void OPCODE_1C(void) { illegal_opcode("0x1C", "???"); }
void OPCODE_1F(void) { illegal_opcode("0x1F", "???"); }
void OPCODE_22(void) { illegal_opcode("0x22", "KIL"); }
void OPCODE_23(void) { illegal_opcode("0x23", "ASL Accumulator"); }
void OPCODE_27(void) { illegal_opcode("0x27", "RLA Zero Page"); }
void OPCODE_2B(void) { illegal_opcode("0x2B", "ASL Accumulator"); }
void OPCODE_2F(void) { illegal_opcode("0x2F", "???"); }
void OPCODE_32(void) { illegal_opcode("0x32", "KIL"); }
void OPCODE_33(void) { illegal_opcode("0x33", "NOP"); }
void OPCODE_34(void) { illegal_opcode("0x34", "NOP"); }
void OPCODE_39(void) { illegal_opcode("0x39", "NOP"); }
void OPCODE_3A(void) { illegal_opcode("0x3A", "NOP"); }
void OPCODE_3B(void) { illegal_opcode("0x3B", "NOP"); }
void OPCODE_3F(void) { illegal_opcode("0x3F", "NOP"); }
void OPCODE_42(void) { illegal_opcode("0x42", "KIL"); }
void OPCODE_43(void) { illegal_opcode("0x43", "NOP"); }
void OPCODE_44(void) { illegal_opcode("0x44", "NOP"); }
void OPCODE_47(void) { illegal_opcode("0x47", "SRE Zero Page"); }
void OPCODE_4B(void) { illegal_opcode("0x4B", "NOP"); }
void OPCODE_4F(void) { illegal_opcode("0x4F", "NOP"); }
void OPCODE_52(void) { illegal_opcode("0x52", "KIL"); }
void OPCODE_53(void) { illegal_opcode("0x53", "KIL"); }
void OPCODE_54(void) { illegal_opcode("0x54", "NOP"); }
void OPCODE_59(void) { illegal_opcode("0x59", "NOP"); }
void OPCODE_5C(void) { illegal_opcode("0x5C", "NOP"); }
void OPCODE_5B(void) { illegal_opcode("0x5B", "NOP"); }
void OPCODE_5F(void) { illegal_opcode("0x5F", "NOP"); }
void OPCODE_62(void) { illegal_opcode("0x62", "KIL"); }
void OPCODE_63(void) { illegal_opcode("0x63", "NOP"); }
void OPCODE_64(void) { illegal_opcode("0x64", "NOP"); }
void OPCODE_67(void) { illegal_opcode("0x67", "RRA Zero Page"); }
void OPCODE_6B(void) { illegal_opcode("0x6B", "NOP"); }
void OPCODE_73(void) { illegal_opcode("0x73", "NOP"); }
void OPCODE_77(void) { illegal_opcode("0x77", "NOP"); }
void OPCODE_7A(void) { illegal_opcode("0x7A", "NOP"); }
void OPCODE_7B(void) { illegal_opcode("0x7B", "NOP"); }
void OPCODE_7C(void) { illegal_opcode("0x7C", "NOP"); }
void OPCODE_7F(void) { illegal_opcode("0x7F", "NOP"); }
void OPCODE_82(void) { illegal_opcode("0x82", "NOP"); }
void OPCODE_83(void) { illegal_opcode("0x83", "NOP"); }
void OPCODE_87(void) { illegal_opcode("0x87", "NOP"); }
void OPCODE_89(void) { illegal_opcode("0x89", "NOP"); }
void OPCODE_8B(void) { illegal_opcode("0x8B", "NOP"); }
void OPCODE_8F(void) { illegal_opcode("0x8F", "NOP"); }
void OPCODE_92(void) { illegal_opcode("0x92", "KIL"); }
void OPCODE_93(void) { illegal_opcode("0x93", "TXS"); }
void OPCODE_9B(void) { illegal_opcode("0x9B", "TXS"); }
void OPCODE_9C(void) { illegal_opcode("0x9C", "NOP"); }
void OPCODE_9E(void) { illegal_opcode("0x9E", "NOP"); }
void OPCODE_9F(void) { illegal_opcode("0x9F", "NOP"); }
void OPCODE_A3(void) { illegal_opcode("0xA3", "SBC (Indirect), Y"); }
void OPCODE_A7(void) { illegal_opcode("0xA7", "LAX Zero Page"); }
void OPCODE_AB(void) { illegal_opcode("0xAB", "TSX"); }
void OPCODE_AF(void) { illegal_opcode("0xAF", "LAX Absolute"); }
void OPCODE_B2(void) { illegal_opcode("0xB2", "KIL"); }
void OPCODE_B3(void) { illegal_opcode("0xB3", "SBC (Indirect), Y"); }
void OPCODE_B7(void) { illegal_opcode("0xB7", "LAX Zero Page, Y"); }
void OPCODE_BB(void) { illegal_opcode("0xBB", "SBC Absolute, Y"); }
void OPCODE_BF(void) { illegal_opcode("0xBF", "SBC Absolute, Y"); }
void OPCODE_C2(void) { illegal_opcode("0xC2", "KIL"); }
void OPCODE_C3(void) { illegal_opcode("0xC3", "SBC (Indirect), Y"); }
void OPCODE_C7(void) { illegal_opcode("0xC7", "DCP Zero Page"); }
void OPCODE_CB(void) { illegal_opcode("0xCB", "SBC Absolute, Y"); }
void OPCODE_CF(void) { illegal_opcode("0xCF", "DCP Absolute"); }
void OPCODE_D2(void) { illegal_opcode("0xD2", "KIL"); }
void OPCODE_D3(void) { illegal_opcode("0xD3", "SBC (Indirect), Y"); }
void OPCODE_D4(void) { illegal_opcode("0xD4", "SBC (Indirect), Y"); }
void OPCODE_D7(void) { illegal_opcode("0xD7", "SBC Zero Page, Y"); }
void OPCODE_DB(void) { illegal_opcode("0xDB", "SBC Absolute, Y"); }
void OPCODE_DC(void) { illegal_opcode("0xDC", "SBC Absolute, Y"); }
void OPCODE_DF(void) { illegal_opcode("0xDF", "SBC Absolute, X"); }
void OPCODE_E2(void) { illegal_opcode("0xE2", "KIL"); }
void OPCODE_E3(void) { illegal_opcode("0xE3", "KIL"); }
void OPCODE_E7(void) { illegal_opcode("0xE7", "ISC Zero Page"); }
void OPCODE_EF(void) { illegal_opcode("0xEF", "ISC Absolute"); }
void OPCODE_EB(void) { illegal_opcode("0xEB", "???"); }
void OPCODE_F2(void) { illegal_opcode("0xF2", "KIL"); }
void OPCODE_F3(void) { illegal_opcode("0xF3", "KIL"); }
void OPCODE_F4(void) { illegal_opcode("0xF4", "ISC Zero Page, X"); }
void OPCODE_F7(void) { illegal_opcode("0xF7", "ISC Zero Page, X"); }
void OPCODE_FB(void) { illegal_opcode("0xFb", "ISC Zero Page, X"); }


