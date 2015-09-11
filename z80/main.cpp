//
//  main.cpp
//  z80
//
//  Created by Sijmen Mulder on 11-09-15.
//  Copyright (c) 2015 Sijmen Mulder. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>
#include <cstdint>

using namespace std;

const int CPU_HZ = 3580 * 1000;
const int MEM_SIZE = 32 * 1024;

enum Op : uint8_t {
	NOOP = 0x00,
	LD_ind_BC_A = 0x02,
	INC_B = 0x04,
	DEC_B = 0x05,
	LD_A_ind_BC = 0x0A,
	INC_C = 0x0C,
	DEC_C = 0x0D,
	DJNZ = 0x10,
	LD_ind_DE_A = 0x12,
	INC_D = 0x14,
	DEC_D = 0x15,
	JR = 0x18,
	LD_A_ind_DE = 0x1A,
	INC_E = 0x1C,
	DEC_E = 0x1D,
	JR_NZ = 0x20,
	INC_F = 0x24,
	DEC_F = 0x25,
	JR_Z = 0x28,
	INC_L = 0x2C,
	DEC_L = 0x2D,
	JR_NC = 0x30,
	LD_ext_A = 0x32,
	INC_ind_HL = 0x34,
	DEC_ind_HL = 0x35,
	JR_C = 0x38,
	INC_A = 0x3C,
	DEC_A = 0x3D,
	LD_B_B = 0x40,
	LD_B_C,
	LD_B_D,
	LD_B_E,
	LD_B_F,
	LD_B_L,
	LD_B_ind_HL,
	LD_B_A,
	LD_C_B,
	LD_C_C,
	LD_C_D,
	LD_C_E,
	LD_C_F,
	LD_C_L,
	LD_C_ind_HL,
	LD_C_A,
	LD_D_B,
	LD_D_C,
	LD_D_D,
	LD_D_E,
	LD_D_F,
	LD_D_L,
	LD_D_ind_HL,
	LD_D_A,
	LD_E_B,
	LD_E_C,
	LD_E_D,
	LD_E_E,
	LD_E_F,
	LD_E_L,
	LD_E_ind_HL,
	LD_E_A,
	LD_H_B,
	LD_H_C,
	LD_H_D,
	LD_H_E,
	LD_H_F,
	LD_H_L,
	LD_H_ind_HL,
	LD_H_A,
	LD_L_B,
	LD_L_C,
	LD_L_D,
	LD_L_E,
	LD_L_F,
	LD_L_L,
	LD_L_ind_HL,
	LD_L_A,
	LD_ind_HL_B,
	LD_ind_HL_C,
	LD_ind_HL_D,
	LD_ind_HL_E,
	LD_ind_HL_F,
	LD_ind_HL_L = 0x75,
	LD_ind_HL_A = 0x77,
	LD_A_B,
	LD_A_C,
	LD_A_D,
	LD_A_E,
	LD_A_F,
	LD_A_L,
	LD_A_ind_HL,
	LD_A_A,
	ADD_A_B,
	ADD_A_C,
	ADD_A_D,
	ADD_A_E,
	ADD_A_F,
	ADD_A_L,
	ADD_A_ind_HL,
	ADD_A_A,
	ADC_A_B,
	ADC_A_C,
	ADC_A_D,
	ADC_A_E,
	ADC_A_F,
	ADC_A_L,
	ADC_A_ind_HL,
	ADC_A_A,
	SUB_A_B,
	SUB_A_C,
	SUB_A_D,
	SUB_A_E,
	SUB_A_F,
	SUB_A_L,
	SUB_A_ind_HL,
	SUB_A_A,
	SBC_A_B,
	SBC_A_C,
	SBC_A_D,
	SBC_A_E,
	SBC_A_F,
	SBC_A_L,
	SBC_A_ind_HL,
	SBC_A_A,
	AND_A_B,
	AND_A_C,
	AND_A_D,
	AND_A_E,
	AND_A_F,
	AND_A_L,
	AND_A_ind_HL,
	AND_A_A,
	XOR_A_B,
	XOR_A_C,
	XOR_A_D,
	XOR_A_E,
	XOR_A_F,
	XOR_A_L,
	XOR_A_ind_HL,
	XOR_A_A,
	OR_A_B,
	OR_A_C,
	OR_A_D,
	OR_A_E,
	OR_A_F,
	OR_A_L,
	OR_A_ind_HL,
	OR_A_A,
	CP_B,
	CP_C,
	CP_D,
	CP_E,
	CP_F,
	CP_L,
	CP_ind_HL,
	CP_A = 0xBF,
	JP_NZ = 0xC2,
	JP = 0xC3,
	ADD_A_imm = 0xC6,
	JP_Z = 0xCA,
	ADC_A_imm = 0xCE,
	JP_NC = 0xD2,
	SUB_A_imm = 0xD6,
	JP_C = 0xD8,
	EXT_DD = 0xDD,
	SBC_A_imm = 0xDE,
	JP_PO = 0xE2,
	AND_A_imm = 0xE6,
	JP_PE = 0xEA,
	JP_ind_HL = 0xEB,
	EXT_ED = 0xED,
	XOR_A_imm = 0xEE,
	JP_P = 0xF2,
	OR_A_imm = 0xF6,
	JP_M = 0xFA,
	EXT_FD = 0xFD,
	CP_imm = 0xFE
};

enum DDOp : uint8_t {
	DD_LD_D_imm = 0x1B,
	DD_LD_E_imm = 0x1E,
	DD_LD_H_imm = 0x2B,
	DD_INC_idx_IX = 0x34,
	DD_DEC_idx_IX = 0x35,
	DD_LD_idx_IX_imm = 0x36,
	DD_LD_B_idx_IX = 0x46,
	DD_LD_C_idx_IX = 0x4E,
	DD_LD_D_idx_IX = 0x56,
	DD_LD_E_idx_IX = 0x5E,
	DD_LD_H_idx_IX = 0x66,
	DD_LD_L_idx_IX = 0x6E,
	DD_LD_idx_IX_B = 0x70,
	DD_LD_idx_IX_C,
	DD_LD_idx_IX_D,
	DD_LD_idx_IX_E,
	DD_LD_idx_IX_F,
	DD_LD_idx_IX_L,
	DD_LD_idx_IX_A,
	DD_LD_ind_HL_imm = 0x78,
	DD_LD_A_idx_IY = 0x7E,
	DD_ADD_A_idx_IX = 0x86,
	DD_ADC_A_idx_IX = 0x8E,
	DD_SUB_A_idx_IX = 0x96,
	DD_SBC_A_idx_IX = 0x9E,
	DD_AND_A_idx_IX = 0xA6,
	DD_XOR_A_idx_IX = 0xAE,
	DD_OR_A_idx_IX = 0xB6,
	DD_CP_idx_IX = 0xBE,
	DD_LD_B_imm = 0xD5,
	DD_LD_C_imm = 0xDE,
	DD_JP_ind_IX = 0xE9
};

enum EDOp : uint8_t {
	ED_LD_imp_I_A = 0x47,
	ED_LD_imp_R_A = 0x4F
};

enum FDOp : uint8_t {
	FD_LD_A_imm = 0x2E,
	FD_LD_idx_IY_imm = 0x36,
	FD_LD_A_ext = 0x3A,
	FD_INC_idx_IY = 0x34,
	FD_DEC_idx_IY = 0x35,
	FD_LD_B_idx_IY = 0x46,
	FD_LD_C_idx_IY = 0x4E,
	FD_LD_D_idx_IY = 0x56,
	FD_LD_E_idx_IY = 0x5E,
	FD_LD_H_idx_IY = 0x66,
	FD_LD_idx_IY_B = 0x70,
	FD_LD_idx_IY_C,
	FD_LD_idx_IY_D,
	FD_LD_idx_IY_E,
	FD_LD_idx_IY_F,
	FD_LD_idx_IY_L,
	FD_LD_idx_IY_A = 0x77,
	FD_LD_A_idx_IX = 0x7E,
	FD_ADD_A_idx_IY = 0x86,
	FD_ADC_A_idx_IY = 0x8E,
	FD_SUB_A_idx_IY = 0x96,
	FD_SBC_A_idx_IY = 0x9E,
	FD_AND_A_idx_IY = 0xA6,
	FD_XOR_A_idx_IY = 0xAE,
	FD_OR_A_idx_IY = 0xB6,
	FD_CP_idx_IY = 0xBE,
	FD_JP_ind_IY = 0xE9
};

class Z80 {
	const uint8_t FLAG_C = 0x01;
	const uint8_t FLAG_N = 0x02;
	const uint8_t FLAG_PV = 0x04;
	const uint8_t FLAG_H = 0x08;
	const uint8_t FLAG_Z = 0x40;
	const uint8_t FLAG_S = 0x80;
	
	array<uint8_t, MEM_SIZE> ram_;
	
	uint8_t ra_ = 0;
	uint8_t rb_ = 0;
	uint8_t rd_ = 0;
	uint8_t rh_ = 0;
	uint8_t rf_ = 0;
	uint8_t rc_ = 0;
	uint8_t re_ = 0;
	uint8_t rl_ = 0;
	uint8_t ra2_ = 0;
	uint8_t rb2_ = 0;
	uint8_t rd2_ = 0;
	uint8_t rh2_ = 0;
	uint8_t rf2_ = 0;
	uint8_t rc2_ = 0;
	uint8_t re2_ = 0;
	uint8_t rl2_ = 0;
	uint8_t ri_ = 0;
	uint8_t rr_ = 0;
	uint16_t rix_ = 0;
	uint16_t riy_ = 0;
	uint16_t rsp_ = 0;
	uint16_t rpc_ = 0;
	
	uint16_t rhl() const { return (uint16_t)rh_ << 8 & rl_; }
	uint16_t rbc() const { return (uint16_t)rb_ << 8 & rc_; }
	uint16_t rde() const { return (uint16_t)rd_ << 8 & re_; }
	
	bool fc() const { return (rf_ & FLAG_C) > 0; }
	bool fn() const { return (rf_ & FLAG_N) > 0; }
	bool fpv() const { return (rf_ & FLAG_PV) > 0; }
	bool fh() const { return (rf_ & FLAG_H) > 0; }
	bool fz() const { return (rf_ & FLAG_Z) > 0; }
	bool fs() const { return (rf_ & FLAG_S) > 0; }

	uint8_t w_calc_flags(uint16_t result, bool is_sub);
	uint8_t w_logic_flags(uint8_t result);
	
	uint8_t op_add(uint8_t a, uint8_t b) { return w_calc_flags(a + b, false); }
	uint8_t op_adc(uint8_t a, uint8_t b) { return w_calc_flags(a + b + (rf_ & 0x01), false); }
	uint8_t op_sub(uint8_t a, uint8_t b) { return w_calc_flags(a - b, true); }
	uint8_t op_sbc(uint8_t a, uint8_t b) { return w_calc_flags(a - b - (rf_ & 0x01), true); }
	uint8_t op_and(uint8_t a, uint8_t b) { return w_logic_flags(a & b); }
	uint8_t op_xor(uint8_t a, uint8_t b) { return w_logic_flags(a ^ b); }
	uint8_t op_or(uint8_t a, uint8_t b) { return w_logic_flags(a | b); }
	void op_cp(uint8_t a) { w_logic_flags(a); }
	uint8_t op_inc(uint8_t a) { return w_calc_flags(a + 1, false); }
	uint8_t op_dec(uint8_t a) { return w_calc_flags(a - 1, false); }
	
	uint8_t next() { return read(rpc_++); }
	uint16_t next16() { return ((uint16_t)next() << 8) | next(); }
	uint8_t read(uint16_t addr) const { return ram_[addr]; }
	void write(uint16_t addr, uint8_t val) { ram_[addr] = val; }

	string pc_str();
	void dump_regs();
	
public:
	void step();
	void run_to_nop(bool print = false);
	
	array<uint8_t, MEM_SIZE>& ram() { return ram_; }
	
	uint8_t reg_a() const { return ra_; }
	uint8_t reg_b() const { return rb_; }
	uint8_t reg_d() const { return rd_; }
	uint8_t reg_h() const { return rh_; }
	uint8_t reg_f() const { return rf_; }
	uint8_t reg_c() const { return rc_; }
	uint8_t reg_e() const { return re_; }
	uint8_t reg_l() const { return rl_; }
	uint8_t reg_a2() const { return ra2_; }
	uint8_t reg_b2() const { return rb2_; }
	uint8_t reg_d2() const { return rd2_; }
	uint8_t reg_h2() const { return rh2_; }
	uint8_t reg_f2() const { return rf2_; }
	uint8_t reg_c2() const { return rc2_; }
	uint8_t reg_e2() const { return re2_; }
	uint8_t reg_l2() const { return rl2_; }
	uint8_t reg_i() const { return ri_; }
	uint8_t reg_r() const { return rr_; }
	uint16_t reg_ix() const { return rix_; }
	uint16_t reg_iy() const { return riy_; }
	uint16_t reg_sp() const { return rsp_; }
	uint16_t reg_pc() const { return rpc_; }
};


uint8_t Z80::w_calc_flags(uint16_t result, bool is_sub)
{
	rf_ = 0;
	
	if (result & 0x0100) rf_ |= FLAG_C;
	if (is_sub) rf_ |= FLAG_N;
	if (result > 0xFF) rf_ |= FLAG_PV;
	if (result & 0x08) rf_ |= FLAG_H;
	if (!result) rf_ |= FLAG_Z;
	if (result & 0x80) rf_ |= FLAG_S;
	
	return (uint8_t)result;
}

uint8_t Z80::w_logic_flags(uint8_t result)
{
	rf_ = 0;
	
	uint8_t x = result;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	bool parity = (~x) & 0x01;

	if (parity) rf_ |= FLAG_PV;
	if (!result) rf_ |= FLAG_Z;
	if (result & 0x80) rf_ |= FLAG_S;
	
	return result;
}

string Z80::pc_str()
{
	uint8_t old_pc = rpc_;
	uint8_t code;
	stringstream str;
	
	str << hex << setfill('0');
	
	switch (code = next()) {
		case NOOP: str << "noop"; break;
		case LD_A_A: str << "ld a, a"; break;
		case LD_A_B: str << "ld a, b"; break;
		case LD_A_C: str << "ld a, c"; break;
		case LD_A_D: str << "ld a, d"; break;
		case LD_A_E: str << "ld a, e"; break;
		case LD_A_F: str << "ld a, f"; break;
		case LD_A_L: str << "ld a, l"; break;
		case LD_B_A: str << "ld b, a"; break;
		case LD_B_B: str << "ld b, b"; break;
		case LD_B_C: str << "ld b, c"; break;
		case LD_B_D: str << "ld b, d"; break;
		case LD_B_E: str << "ld b, e"; break;
		case LD_B_F: str << "ld b, f"; break;
		case LD_B_L: str << "ld b, l"; break;
		case LD_C_A: str << "ld c, a"; break;
		case LD_C_B: str << "ld c, b"; break;
		case LD_C_C: str << "ld c, c"; break;
		case LD_C_D: str << "ld c, d"; break;
		case LD_C_E: str << "ld c, e"; break;
		case LD_C_F: str << "ld c, f"; break;
		case LD_C_L: str << "ld c, l"; break;
		case LD_D_A: str << "ld d, a"; break;
		case LD_D_B: str << "ld d, b"; break;
		case LD_D_C: str << "ld d, c"; break;
		case LD_D_D: str << "ld d, d"; break;
		case LD_D_E: str << "ld d, e"; break;
		case LD_D_F: str << "ld d, f"; break;
		case LD_D_L: str << "ld d, l"; break;
		case LD_E_A: str << "ld e, a"; break;
		case LD_E_B: str << "ld e, b"; break;
		case LD_E_C: str << "ld e, c"; break;
		case LD_E_D: str << "ld e, d"; break;
		case LD_E_E: str << "ld e, e"; break;
		case LD_E_F: str << "ld e, f"; break;
		case LD_E_L: str << "ld e, l"; break;
		case LD_H_A: str << "ld h, a"; break;
		case LD_H_B: str << "ld h, b"; break;
		case LD_H_C: str << "ld h, c"; break;
		case LD_H_D: str << "ld h, d"; break;
		case LD_H_E: str << "ld h, e"; break;
		case LD_H_F: str << "ld h, f"; break;
		case LD_H_L: str << "ld h, l"; break;
		case LD_L_A: str << "ld l, a"; break;
		case LD_L_B: str << "ld l, b"; break;
		case LD_L_C: str << "ld l, c"; break;
		case LD_L_D: str << "ld l, d"; break;
		case LD_L_E: str << "ld l, e"; break;
		case LD_L_F: str << "ld l, f"; break;
		case LD_L_L: str << "ld l, l"; break;
		case LD_A_ind_HL: str << "ld a, (hl)"; break;
		case LD_A_ind_BC: str << "ld a, (bc)"; break;
		case LD_A_ind_DE: str << "ld a, (de)"; break;
		case LD_B_ind_HL: str << "ld b, (hl)"; break;
		case LD_C_ind_HL: str << "ld c, (hl)"; break;
		case LD_D_ind_HL: str << "ld d, (hl)"; break;
		case LD_E_ind_HL: str << "ld e, (hl)"; break;
		case LD_H_ind_HL: str << "ld h, (hl)"; break;
		case LD_L_ind_HL: str << "ld l, (hl)"; break;
		case LD_ind_HL_A: str << "ld (hl), a"; break;
		case LD_ind_HL_B: str << "ld (hl), b"; break;
		case LD_ind_HL_C: str << "ld (hl), c"; break;
		case LD_ind_HL_D: str << "ld (hl), d"; break;
		case LD_ind_HL_E: str << "ld (hl), e"; break;
		case LD_ind_HL_F: str << "ld (hl), f"; break;
		case LD_ind_HL_L: str << "ld (hl), l"; break;
		case LD_ind_BC_A: str << "ld (bc), a"; break;
		case LD_ind_DE_A: str << "ld (de), a"; break;
		case LD_ext_A: str << "ld (0x" << setw(4) << next16() << "), a"; break;
		case ADD_A_A: str << "add a, a"; break;
		case ADD_A_B: str << "add a, b"; break;
		case ADD_A_C: str << "add a, c"; break;
		case ADD_A_D: str << "add a, d"; break;
		case ADD_A_E: str << "add a, e"; break;
		case ADD_A_F: str << "add a, f"; break;
		case ADD_A_L: str << "add a, l"; break;
		case ADD_A_ind_HL: str << "add a, (hl)"; break;
		case ADD_A_imm: str << "add a, 0x" << setw(2) << (int)next(); break;
		case ADC_A_A: str << "adc a, a"; break;
		case ADC_A_B: str << "adc a, b"; break;
		case ADC_A_C: str << "adc a, c"; break;
		case ADC_A_D: str << "adc a, d"; break;
		case ADC_A_E: str << "adc a, e"; break;
		case ADC_A_F: str << "adc a, f"; break;
		case ADC_A_L: str << "adc a, l"; break;
		case ADC_A_ind_HL: str << "adc a, (hl)"; break;
		case ADC_A_imm: str << "adc a, 0x" << setw(2) << (int)next(); break;
		case SUB_A_A: str << "sub a, a"; break;
		case SUB_A_B: str << "sub a, b"; break;
		case SUB_A_C: str << "sub a, c"; break;
		case SUB_A_D: str << "sub a, d"; break;
		case SUB_A_E: str << "sub a, e"; break;
		case SUB_A_F: str << "sub a, f"; break;
		case SUB_A_L: str << "sub a, l"; break;
		case SUB_A_ind_HL: str << "sub a, (hl)"; break;
		case SUB_A_imm: str << "sub a, 0x" << setw(2) << (int)next(); break;
		case SBC_A_A: str << "sbc a, a"; break;
		case SBC_A_B: str << "sbc a, b"; break;
		case SBC_A_C: str << "sbc a, c"; break;
		case SBC_A_D: str << "sbc a, d"; break;
		case SBC_A_E: str << "sbc a, e"; break;
		case SBC_A_F: str << "sbc a, f"; break;
		case SBC_A_L: str << "sbc a, l"; break;
		case SBC_A_ind_HL: str << "sbc a, (hl)"; break;
		case SBC_A_imm: str << "sbc a, 0x" << setw(2) << (int)next(); break;
		case AND_A_A: str << "and a, a"; break;
		case AND_A_B: str << "and a, b"; break;
		case AND_A_C: str << "and a, c"; break;
		case AND_A_D: str << "and a, d"; break;
		case AND_A_E: str << "and a, e"; break;
		case AND_A_F: str << "and a, f"; break;
		case AND_A_L: str << "and a, l"; break;
		case AND_A_ind_HL: str << "and a, (hl)"; break;
		case AND_A_imm: str << "and a, 0x" << setw(2) << (int)next(); break;
		case XOR_A_A: str << "xor a, a"; break;
		case XOR_A_B: str << "xor a, b"; break;
		case XOR_A_C: str << "xor a, c"; break;
		case XOR_A_D: str << "xor a, d"; break;
		case XOR_A_E: str << "xor a, e"; break;
		case XOR_A_F: str << "xor a, f"; break;
		case XOR_A_L: str << "xor a, l"; break;
		case XOR_A_ind_HL: str << "xor a, (hl)"; break;
		case XOR_A_imm: str << "xor a, 0x" << setw(2) << (int)next(); break;
		case OR_A_A: str << "or a, a"; break;
		case OR_A_B: str << "or a, b"; break;
		case OR_A_C: str << "or a, c"; break;
		case OR_A_D: str << "or a, d"; break;
		case OR_A_E: str << "or a, e"; break;
		case OR_A_F: str << "or a, f"; break;
		case OR_A_L: str << "or a, l"; break;
		case OR_A_ind_HL: str << "or a, (hl)"; break;
		case OR_A_imm: str << "or a, 0x" << setw(2) << (int)next(); break;
		case CP_A: str << "cp a"; break;
		case CP_B: str << "cp b"; break;
		case CP_C: str << "cp c"; break;
		case CP_D: str << "cp d"; break;
		case CP_E: str << "cp e"; break;
		case CP_F: str << "cp f"; break;
		case CP_L: str << "cp l"; break;
		case CP_ind_HL: str << "cp (hl)"; break;
		case CP_imm: str << "cp 0x" << setw(2) << (int)next(); break;
		case INC_A: str << "inc a"; break;
		case INC_B: str << "inc b"; break;
		case INC_C: str << "inc c"; break;
		case INC_D: str << "inc d"; break;
		case INC_E: str << "inc e"; break;
		case INC_F: str << "inc f"; break;
		case INC_L: str << "inc l"; break;
		case INC_ind_HL: str << "inc (hl)"; break;
		case DEC_A: str << "dec a"; break;
		case DEC_B: str << "dec b"; break;
		case DEC_C: str << "dec c"; break;
		case DEC_D: str << "dec d"; break;
		case DEC_E: str << "dec e"; break;
		case DEC_F: str << "dec f"; break;
		case DEC_L: str << "dec l"; break;
		case DEC_ind_HL: str << "dec (hl)"; break;
		case JP: str << "jp 0x" << setw(4) << next16(); break;
		case JP_C: str << "jp c, 0x" << setw(4) << next16(); break;
		case JP_NC: str << "jp nc, 0x" << setw(4) << next16(); break;
		case JP_Z: str << "jp z, 0x" << setw(4) << next16(); break;
		case JP_NZ: str << "jp nz, 0x" << setw(4) << next16(); break;
		case JP_PO: str << "jp po, 0x" << setw(4) << next16(); break;
		case JP_PE: str << "jp pe, 0x" << setw(4) << next16(); break;
		case JP_M: str << "jp m, 0x" << setw(4) << next16(); break;
		case JP_P: str << "jp p, 0x" << setw(4) << next16(); break;
		case JP_ind_HL: str << "jp (hl)"; break;
		case JR: str << "jr 0x" << setw(2) << (int)next(); break;
		case JR_C: str << "jr c, 0x" << setw(2) << (int)next(); break;
		case JR_NC: str << "jr nc, 0x" << setw(2) << (int)next(); break;
		case JR_Z: str << "jr z, 0x" << setw(2) << (int)next(); break;
		case JR_NZ: str << "jr nz, 0x" << setw(2) << (int)next(); break;
		case DJNZ: str << "djnz 0x" << setw(2) << (int)next(); break;
			
		case EXT_DD:
			switch (code = next()) {
				case DD_LD_B_imm: str << "ld b, 0x" << setw(2) << (int)next(); break;
				case DD_LD_C_imm: str << "ld c, 0x" << setw(2) << (int)next(); break;
				case DD_LD_D_imm: str << "ld d, 0x" << setw(2) << (int)next(); break;
				case DD_LD_E_imm: str << "ld e, 0x" << setw(2) << (int)next(); break;
				case DD_LD_H_imm: str << "ld h, 0x" << setw(2) << (int)next(); break;
				case DD_LD_A_idx_IY: str << "ld a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_B_idx_IX: str << "ld b, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_C_idx_IX: str << "ld c, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_D_idx_IX: str << "ld d, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_E_idx_IX: str << "ld e, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_H_idx_IX: str << "ld h, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_L_idx_IX: str << "ld l, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_LD_idx_IX_A: str << "ld (ix + 0x" << setw(2) << (int)next() << "), a"; break;
				case DD_LD_idx_IX_B: str << "ld (ix + 0x" << setw(2) << (int)next() << "), b"; break;
				case DD_LD_idx_IX_C: str << "ld (ix + 0x" << setw(2) << (int)next() << "), c"; break;
				case DD_LD_idx_IX_D: str << "ld (ix + 0x" << setw(2) << (int)next() << "), d"; break;
				case DD_LD_idx_IX_E: str << "ld (ix + 0x" << setw(2) << (int)next() << "), e"; break;
				case DD_LD_idx_IX_F: str << "ld (ix + 0x" << setw(2) << (int)next() << "), f"; break;
				case DD_LD_idx_IX_L: str << "ld (ix + 0x" << setw(2) << (int)next() << "), l"; break;
				case DD_LD_idx_IX_imm: str << "ld (ix + 0x" << setw(2) << (int)next() << "), 0x" << setw(2) << (int)next(); break;
				case DD_LD_ind_HL_imm: str << "ld (hl), 0x" << setw(2) << (int)next(); break;
				case DD_ADD_A_idx_IX: str << "add a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_ADC_A_idx_IX: str << "adc a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_SUB_A_idx_IX: str << "sub a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_SBC_A_idx_IX: str << "sbc a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_AND_A_idx_IX: str << "and a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_XOR_A_idx_IX: str << "xor a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_OR_A_idx_IX: str << "or a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_CP_idx_IX: str << "cp (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case DD_JP_ind_IX: str << "jp (ix)"; break;
				default: str << "0x" << (0xDD00 & code); break;
			}
			break;
			
		case EXT_ED:
			switch (next()) {
				case ED_LD_imp_I_A: str << "ld i, a"; break;
				case ED_LD_imp_R_A: str << "ld r, a"; break;
				default: str << "0x" << (0xED00 & code); break;
			}
			break;
			
		case EXT_FD:
			switch (code = next()) {
				case FD_LD_A_imm: str << "ld a, 0x" << setw(2) << (int)next(); break;
				case FD_LD_A_ext: str << "ld a, (0x" << setw(4) << next16() << ")"; break;
				case FD_LD_A_idx_IX: str << "ld a, (ix + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_B_idx_IY: str << "ld b, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_C_idx_IY: str << "ld c, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_D_idx_IY: str << "ld d, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_E_idx_IY: str << "ld e, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_H_idx_IY: str << "ld h, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_LD_idx_IY_A: str << "ld (iy + 0x" << setw(2) << (int)next() << "), a"; break;
				case FD_LD_idx_IY_B: str << "ld (iy + 0x" << setw(2) << (int)next() << "), b"; break;
				case FD_LD_idx_IY_C: str << "ld (iy + 0x" << setw(2) << (int)next() << "), c"; break;
				case FD_LD_idx_IY_D: str << "ld (iy + 0x" << setw(2) << (int)next() << "), d"; break;
				case FD_LD_idx_IY_E: str << "ld (iy + 0x" << setw(2) << (int)next() << "), e"; break;
				case FD_LD_idx_IY_F: str << "ld (iy + 0x" << setw(2) << (int)next() << "), f"; break;
				case FD_LD_idx_IY_L: str << "ld (iy + 0x" << setw(2) << (int)next() << "), l"; break;
				case FD_LD_idx_IY_imm: str << "ld (iy + 0x" << setw(2) << (int)next() << "), 0x" << setw(2) << (int)next(); break;
				case FD_ADD_A_idx_IY: str << "add a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_ADC_A_idx_IY: str << "adc a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_SUB_A_idx_IY: str << "sub a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_SBC_A_idx_IY: str << "sbc a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_AND_A_idx_IY: str << "and a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_XOR_A_idx_IY: str << "xor a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_OR_A_idx_IY: str << "or a, (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_CP_idx_IY: str << "cp (iy + 0x" << setw(2) << (int)next() << ")"; break;
				case FD_JP_ind_IY: str << "jp (iy)"; break;
				default: str << "0x" << (0xFD00 & code); break;
			}
			break;
			
		default:
			str << "0x" << (int)code;;
			break;
	}
	
	rpc_ = old_pc;
	return str.str();
}

void Z80::dump_regs()
{
	stringstream str;

	str << hex << setfill('0')
		<<   "A: 0x"  << setw(2) << (int)ra_  << "  F: 0x"  << setw(2) << (int)rf_
		<< "  A': 0x" << setw(2) << (int)ra2_ << "  F': 0x" << setw(2) << (int)rf2_ << endl
		<<   "B: 0x"  << setw(2) << (int)rb_  << "  C: 0x"  << setw(2) << (int)rc_
		<< "  B': 0x" << setw(2) << (int)rb2_ << "  C': 0x" << setw(2) << (int)rc2_ << endl
		<<   "D: 0x"  << setw(2) << (int)rd_  << "  E: 0x"  << setw(2) << (int)re_
		<< "  D': 0x" << setw(2) << (int)rd2_ << "  E': 0x" << setw(2) << (int)re2_ << endl
		<<   "H: 0x"  << setw(2) << (int)rh_  << "  L: 0x"  << setw(2) << (int)rl_
		<< "  H': 0x" << setw(2) << (int)rh2_ << "  L': 0x" << setw(2) << (int)rl2_ << endl
		<< "I: 0x"  << setw(2) << (int)ri_  << "  R: 0x"  << setw(2) << (int)rr_ << endl
		<< "IX: 0x"  << setw(4) << rix_ << endl
		<< "IY: 0x"  << setw(4) << riy_ << endl
		<< "SP: 0x"  << setw(4) << rsp_ << endl
		<< "PC: 0x"  << setw(4) << rpc_ << endl
		<<    "S: " << fs()  << "  Z: " << fz() << "  H: " << fh()
		<< "  PV: " << fpv() << "  N: " << fn() << "  C: " << fc() << endl;

	cout << str.str();
}

void Z80::step()
{
	uint8_t tmp;

	uint8_t code;
	switch (code = next()) {
		case LD_A_A: break;
		case LD_A_B: ra_ = rb_; break;;
		case LD_A_C: ra_ = rc_; break;
		case LD_A_D: ra_ = rd_; break;
		case LD_A_E: ra_ = re_; break;
		case LD_A_F: ra_ = rf_; break;
		case LD_A_L: ra_ = rl_; break;
		case LD_B_A: rb_ = ra_; break;
		case LD_B_B: break;
		case LD_B_C: rb_ = rc_; break;
		case LD_B_D: rb_ = rd_; break;
		case LD_B_E: rb_ = re_; break;
		case LD_B_F: rb_ = rf_; break;
		case LD_B_L: rb_ = rl_; break;
		case LD_C_A: rc_ = ra_; break;
		case LD_C_B: rc_ = rb_; break;
		case LD_C_C: break;
		case LD_C_D: rc_ = rd_; break;
		case LD_C_E: rc_ = re_; break;
		case LD_C_F: rc_ = rf_; break;
		case LD_C_L: rc_ = rl_; break;
		case LD_D_A: rd_ = ra_; break;
		case LD_D_B: rd_ = rb_; break;
		case LD_D_C: rd_ = rc_; break;
		case LD_D_D: break;
		case LD_D_E: rd_ = re_; break;
		case LD_D_F: rd_ = rf_; break;
		case LD_D_L: rd_ = rl_; break;
		case LD_E_A: re_ = ra_; break;
		case LD_E_B: re_ = rb_; break;
		case LD_E_C: re_ = rc_; break;
		case LD_E_D: re_ = rd_; break;
		case LD_E_E: break;
		case LD_E_F: re_ = rf_; break;
		case LD_E_L: re_ = rl_; break;
		case LD_H_A: rh_ = ra_; break;
		case LD_H_B: rh_ = rb_; break;
		case LD_H_C: rh_ = rc_; break;
		case LD_H_D: rh_ = rd_; break;
		case LD_H_E: rh_ = re_; break;
		case LD_H_F: rh_ = rf_; break;
		case LD_H_L: rf_ = rl_; break;
		case LD_L_A: rl_ = ra_; break;
		case LD_L_B: rl_ = rb_; break;
		case LD_L_C: rl_ = rc_; break;
		case LD_L_D: rl_ = rd_; break;
		case LD_L_E: rl_ = re_; break;
		case LD_L_F: rl_ = rf_; break;
		case LD_L_L: break;
		case LD_A_ind_HL: ra_ = read(rhl()); break;
		case LD_A_ind_BC: ra_ = read(rbc()); break;
		case LD_A_ind_DE: ra_ = read(rde()); break;
		case LD_B_ind_HL: rb_ = read(rhl()); break;
		case LD_C_ind_HL: rc_ = read(rhl()); break;
		case LD_D_ind_HL: rd_ = read(rhl()); break;
		case LD_E_ind_HL: re_ = read(rhl()); break;
		case LD_H_ind_HL: rh_ = read(rhl()); break;
		case LD_L_ind_HL: rl_ = read(rhl()); break;
		case LD_ind_HL_A: write(rhl(), ra_); break;
		case LD_ind_HL_B: write(rhl(), rb_); break;
		case LD_ind_HL_C: write(rhl(), rc_); break;
		case LD_ind_HL_D: write(rhl(), rd_); break;
		case LD_ind_HL_E: write(rhl(), re_); break;
		case LD_ind_HL_F: write(rhl(), rf_); break;
		case LD_ind_HL_L: write(rhl(), rl_); break;
		case LD_ind_BC_A: write(rbc(), ra_); break;
		case LD_ind_DE_A: write(rde(), ra_); break;
		case LD_ext_A: write(next16(), ra_); break;
		case ADD_A_A: ra_ = op_add(ra_, ra_); break;
		case ADD_A_B: ra_ = op_add(ra_, rb_); break;
		case ADD_A_C: ra_ = op_add(ra_, rc_); break;
		case ADD_A_D: ra_ = op_add(ra_, rd_); break;
		case ADD_A_E: ra_ = op_add(ra_, re_); break;
		case ADD_A_F: ra_ = op_add(ra_, rf_); break;
		case ADD_A_L: ra_ = op_add(ra_, rl_); break;
		case ADD_A_ind_HL: ra_ = op_add(ra_, read(rhl())); break;
		case ADD_A_imm: ra_ = op_add(ra_, next()); break;
		case ADC_A_A: ra_ = op_adc(ra_, ra_); break;
		case ADC_A_B: ra_ = op_adc(ra_, rb_); break;
		case ADC_A_C: ra_ = op_adc(ra_, rc_); break;
		case ADC_A_D: ra_ = op_adc(ra_, rd_); break;
		case ADC_A_E: ra_ = op_adc(ra_, re_); break;
		case ADC_A_F: ra_ = op_adc(ra_, rf_); break;
		case ADC_A_L: ra_ = op_adc(ra_, rl_); break;
		case ADC_A_ind_HL: ra_ = op_adc(ra_, read(rhl())); break;
		case ADC_A_imm: ra_ = op_adc(ra_, next()); break;
		case SUB_A_A: ra_ = op_sub(ra_, ra_); break;
		case SUB_A_B: ra_ = op_sub(ra_, rb_); break;
		case SUB_A_C: ra_ = op_sub(ra_, rc_); break;
		case SUB_A_D: ra_ = op_sub(ra_, rd_); break;
		case SUB_A_E: ra_ = op_sub(ra_, re_); break;
		case SUB_A_F: ra_ = op_sub(ra_, rf_); break;
		case SUB_A_L: ra_ = op_sub(ra_, rl_); break;
		case SUB_A_ind_HL: ra_ = op_sub(ra_, read(rhl())); break;
		case SUB_A_imm: ra_ = op_sub(ra_, next()); break;
		case SBC_A_A: ra_ = op_sbc(ra_, ra_); break;
		case SBC_A_B: ra_ = op_sbc(ra_, rb_); break;
		case SBC_A_C: ra_ = op_sbc(ra_, rc_); break;
		case SBC_A_D: ra_ = op_sbc(ra_, rd_); break;
		case SBC_A_E: ra_ = op_sbc(ra_, re_); break;
		case SBC_A_F: ra_ = op_sbc(ra_, rf_); break;
		case SBC_A_L: ra_ = op_sbc(ra_, rl_); break;
		case SBC_A_ind_HL: ra_ = op_sbc(ra_, read(rhl())); break;
		case SBC_A_imm: ra_ = op_sbc(ra_, next()); break;
		case AND_A_A: ra_ = op_and(ra_, ra_); break;
		case AND_A_B: ra_ = op_and(ra_, rb_); break;
		case AND_A_C: ra_ = op_and(ra_, rc_); break;
		case AND_A_D: ra_ = op_and(ra_, rd_); break;
		case AND_A_E: ra_ = op_and(ra_, re_); break;
		case AND_A_F: ra_ = op_and(ra_, rf_); break;
		case AND_A_L: ra_ = op_and(ra_, rl_); break;
		case AND_A_ind_HL: ra_ = op_and(ra_, read(rhl())); break;
		case AND_A_imm: ra_ = op_and(ra_, next()); break;
		case XOR_A_A: ra_ = op_xor(ra_, ra_); break;
		case XOR_A_B: ra_ = op_xor(ra_, rb_); break;
		case XOR_A_C: ra_ = op_xor(ra_, rc_); break;
		case XOR_A_D: ra_ = op_xor(ra_, rd_); break;
		case XOR_A_E: ra_ = op_xor(ra_, re_); break;
		case XOR_A_F: ra_ = op_xor(ra_, rf_); break;
		case XOR_A_L: ra_ = op_xor(ra_, rl_); break;
		case XOR_A_ind_HL: ra_ = op_xor(ra_, read(rhl())); break;
		case XOR_A_imm: ra_ = op_xor(ra_, next()); break;
		case OR_A_A: ra_ = op_or(ra_, ra_); break;
		case OR_A_B: ra_ = op_or(ra_, rb_); break;
		case OR_A_C: ra_ = op_or(ra_, rc_); break;
		case OR_A_D: ra_ = op_or(ra_, rd_); break;
		case OR_A_E: ra_ = op_or(ra_, re_); break;
		case OR_A_F: ra_ = op_or(ra_, rf_); break;
		case OR_A_L: ra_ = op_or(ra_, rl_); break;
		case OR_A_ind_HL: ra_ = op_or(ra_, read(rhl())); break;
		case OR_A_imm: ra_ = op_or(ra_, next()); break;
		case CP_A: op_cp(ra_); break;
		case CP_B: op_cp(rb_); break;
		case CP_C: op_cp(rc_); break;
		case CP_D: op_cp(rd_); break;
		case CP_E: op_cp(re_); break;
		case CP_F: op_cp(rf_); break;
		case CP_L: op_cp(rl_); break;
		case CP_ind_HL: op_cp(read(rhl())); break;
		case CP_imm: op_cp(next()); break;
		case INC_A: ra_ = op_inc(ra_); break;
		case INC_B: rb_ = op_inc(rb_); break;
		case INC_C: rc_ = op_inc(rc_); break;
		case INC_D: rd_ = op_inc(rd_); break;
		case INC_E: re_ = op_inc(re_); break;
		case INC_F: rf_ = op_inc(rf_); break;
		case INC_L: rl_ = op_inc(rl_); break;
		case INC_ind_HL: tmp = read(rhl()) + next(); write(tmp, op_inc(read(tmp))); break;
		case DEC_A: ra_ = op_dec(ra_); break;
		case DEC_B: rb_ = op_dec(rb_); break;
		case DEC_C: rc_ = op_dec(rc_); break;
		case DEC_D: rd_ = op_dec(rd_); break;
		case DEC_E: re_ = op_dec(re_); break;
		case DEC_F: rf_ = op_dec(rf_); break;
		case DEC_L: rl_ = op_dec(rl_); break;
		case DEC_ind_HL: tmp = read(rhl()) + next(); write(tmp, op_dec(read(tmp))); break;
		case JP: rpc_ = next16(); break;
		case JP_C: tmp = next16(); if (fc()) rpc_ = tmp; break;
		case JP_NC: tmp = next16(); if (!fc()) rpc_ = tmp; break;
		case JP_Z: tmp = next16(); if (fz()) rpc_ = tmp; break;
		case JP_NZ: tmp = next16(); if (!fz()) rpc_ = tmp; break;
		case JP_PO: tmp = next16(); if (!fpv()) rpc_ = tmp; break;
		case JP_PE: tmp = next16(); if (fpv()) rpc_ = tmp; break;
		case JP_M: tmp = next16(); if (fs()) rpc_ = tmp; break;
		case JP_P: tmp = next16(); if (!fs()) rpc_ = tmp; break;
		case JP_ind_HL: rpc_ = read(rhl()); break;
		case JR: tmp = next(); rpc_ += *((int8_t*)&tmp); break;
		case JR_C: tmp = next(); if (fc()) rpc_ += *((int8_t*)&tmp); break;
		case JR_NC: tmp = next(); if (!fc()) rpc_ += *((int8_t*)&tmp); break;
		case JR_Z: tmp = next(); if (fz()) rpc_ += *((int8_t*)&tmp); break;
		case JR_NZ: tmp = next(); if (!fz()) rpc_ += *((int8_t*)&tmp); break;
		case DJNZ: tmp = next(); if (--rb_) rpc_ += *((int8_t*)&tmp); break;
		
		case EXT_DD:
			switch (code = next()) {
				case DD_LD_B_imm: rb_ = next(); break;
				case DD_LD_C_imm: rc_ = next(); break;
				case DD_LD_D_imm: rd_ = next(); break;
				case DD_LD_E_imm: re_ = next(); break;
				case DD_LD_H_imm: rh_ = next(); break;
				case DD_LD_A_idx_IY: ra_ = read(riy_ + next()); break;
				case DD_LD_B_idx_IX: rb_ = read(rix_ + next()); break;
				case DD_LD_C_idx_IX: rc_ = read(rix_ + next()); break;
				case DD_LD_D_idx_IX: rd_ = read(rix_ + next()); break;
				case DD_LD_E_idx_IX: re_ = read(rix_ + next()); break;
				case DD_LD_H_idx_IX: rh_ = read(rix_ + next()); break;
				case DD_LD_L_idx_IX: rl_ = read(rix_ + next()); break;
				case DD_LD_idx_IX_A: write(rix_ + next(), ra_); break;
				case DD_LD_idx_IX_B: write(rix_ + next(), rb_); break;
				case DD_LD_idx_IX_C: write(rix_ + next(), rc_); break;
				case DD_LD_idx_IX_D: write(rix_ + next(), rd_); break;
				case DD_LD_idx_IX_E: write(rix_ + next(), re_); break;
				case DD_LD_idx_IX_F: write(rix_ + next(), rf_); break;
				case DD_LD_idx_IX_L: write(rix_ + next(), rl_); break;
				case DD_LD_idx_IX_imm: write(rix_ + next(), next()); break;
				case DD_LD_ind_HL_imm: write(rhl(), next()); break;
				case DD_ADD_A_idx_IX: ra_ = op_add(ra_, read(rix_ + next())); break;
				case DD_ADC_A_idx_IX: ra_ = op_adc(ra_, read(rix_ + next())); break;
				case DD_SUB_A_idx_IX: ra_ = op_sub(ra_, read(rix_ + next())); break;
				case DD_SBC_A_idx_IX: ra_ = op_sbc(ra_, read(rix_ + next())); break;
				case DD_AND_A_idx_IX: ra_ = op_and(ra_, read(rix_ + next())); break;
				case DD_XOR_A_idx_IX: ra_ = op_xor(ra_, read(rix_ + next())); break;
				case DD_OR_A_idx_IX: ra_ = op_or(ra_, read(rix_ + next())); break;
				case DD_CP_idx_IX: op_cp(read(rix_ + next())); break;
				case DD_JP_ind_IX: rpc_ = read(rix_); break;

				default:
					cerr << "unknown 0xDD opcode: 0x" << (int)code << endl;
					break;
			}
			break;
			
		case EXT_ED:
			switch (next()) {
				case ED_LD_imp_I_A: ri_ = ra_; break;
				case ED_LD_imp_R_A: rr_ = ra_; break;
					
				default:
					cerr << "unknown 0xED opcode: 0x" << (int)code << endl;
					break;
			}
			break;
			
		case EXT_FD:
			switch (code = next()) {
				case FD_LD_A_imm: ra_ = next(); break;
				case FD_LD_A_ext: ra_ = read(next16()); break;
				case FD_LD_A_idx_IX: ra_ = read(rix_ + next()); break;
				case FD_LD_B_idx_IY: rb_ = read(riy_ + next()); break;
				case FD_LD_C_idx_IY: rc_ = read(riy_ + next()); break;
				case FD_LD_D_idx_IY: rd_ = read(riy_ + next()); break;
				case FD_LD_E_idx_IY: re_ = read(riy_ + next()); break;
				case FD_LD_H_idx_IY: rh_ = read(riy_ + next()); break;
				case FD_LD_idx_IY_A: write(riy_ + next(), ra_); break;
				case FD_LD_idx_IY_B: write(riy_ + next(), rb_); break;
				case FD_LD_idx_IY_C: write(riy_ + next(), rc_); break;
				case FD_LD_idx_IY_D: write(riy_ + next(), rd_); break;
				case FD_LD_idx_IY_E: write(riy_ + next(), re_); break;
				case FD_LD_idx_IY_F: write(riy_ + next(), rf_); break;
				case FD_LD_idx_IY_L: write(riy_ + next(), rl_); break;
				case FD_LD_idx_IY_imm: write(riy_ + next(), next()); break;
				case FD_ADD_A_idx_IY: ra_ = op_add(ra_, read(riy_ + next())); break;
				case FD_ADC_A_idx_IY: ra_ = op_adc(ra_, read(riy_ + next())); break;
				case FD_SUB_A_idx_IY: ra_ = op_sub(ra_, read(riy_ + next())); break;
				case FD_SBC_A_idx_IY: ra_ = op_sbc(ra_, read(riy_ + next())); break;
				case FD_AND_A_idx_IY: ra_ = op_and(ra_, read(riy_ + next())); break;
				case FD_XOR_A_idx_IY: ra_ = op_xor(ra_, read(riy_ + next())); break;
				case FD_OR_A_idx_IY: ra_ = op_or(ra_, read(riy_ + next())); break;
				case FD_CP_idx_IY: op_cp(read(riy_ + next())); break;
				case FD_JP_ind_IY: rpc_ = read(riy_); break;
					
				default:
					cerr << "unknown 0xFD opcode: 0x" << (int)code << endl;
					break;
			}
			break;
			
		default:
			cerr << "unknown opcode: 0x" << (int)code << endl;
			break;
	}
}

void Z80::run_to_nop(bool print)
{
	if (print) {
		dump_regs();
		cout << endl;
	}
	
	while (read(rpc_)) {
		if (print) {
			cout << "> " << pc_str() << endl << endl;
		}
		
		step();
		
		if (print) {
			dump_regs();
			cout << endl;
		}
	}
	
	if (print) {
		cout << "> noop" << endl;
	}
}

int main()
{
	Z80 cpu;
	cpu.ram() = {
		JR, 0x0C,                     //  -+
		'H', 'e', 'l', 'l', 'o', ' ', //   |
		'W', 'o', 'r', 'l', 'd', '!', //   |
		                              //   |
		// calculate 3 * 4            //   |
		EXT_FD, FD_LD_A_imm, 0x00,    // <-+
		EXT_DD, DD_LD_B_imm, 0x04,
		EXT_DD, DD_LD_C_imm, 0x03,
		ADD_A_C,                      // <-+
		DJNZ, 0xFD,                   //  -+

		XOR_A_imm, 0xFF,
	
		NOOP
	};

	cpu.run_to_nop(true);
}
