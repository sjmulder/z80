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
	LD_A_ind_BC = 0x0A,
	LD_ind_DE_A = 0x12,
	LD_A_ind_DE = 0x1A,
	LD_ext_A = 0x32,
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
	EXT_DD = 0xDD,
	EXT_ED = 0xED,
	EXT_FD = 0xFD
};

enum DDOp : uint8_t {
	DD_LD_D_imm = 0x1B,
	DD_LD_E_imm = 0x1E,
	DD_LD_H_imm = 0x2B,
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
	DD_LD_B_imm = 0xD5,
	DD_LD_C_imm = 0xDE
};

enum EDOp : uint8_t {
	ED_LD_imp_I_A = 0x47,
	ED_LD_imp_R_A = 0x4F
};

enum FDOp : uint8_t {
	FD_LD_A_imm = 0x2E,
	FD_LD_idx_IY_imm = 0x36,
	FD_LD_A_ext = 0x3A,
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
	FD_LD_A_idx_IX = 0x7E
};

class Z80 {
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


string Z80::pc_str()
{
	uint8_t old_pc = rpc_;
	uint8_t code;
	stringstream str;
	
	str << hex << setfill('0');
	
	switch (code = next()) {
		case NOOP: str << "noop"; break;
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
				default: str << "0x" << (0xFD00 & code); break;
			}
			break;
			
		default:
			str << "0x" << code;;
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
		<< "PC: 0x"  << setw(4) << rpc_ << endl;

	cout << str.str();
}

void Z80::step()
{
	uint8_t code;
	switch (code = next()) {
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
					
				default:
					cerr << "unknown 0xDD opcode: 0x" << code << endl;
					break;
			}
			break;
			
		case EXT_ED:
			switch (next()) {
				case ED_LD_imp_I_A: ri_ = ra_; break;
				case ED_LD_imp_R_A: rr_ = ra_; break;
					
				default:
					cerr << "unknown 0xED opcode: 0x" << code << endl;
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
					
				default:
					cerr << "unknown 0xFD opcode: 0x" << code << endl;
					break;
			}
			break;
			
		default:
			cerr << "unknown opcode: 0x" << code << endl;
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
		EXT_FD, FD_LD_A_imm, 0x01,
		EXT_DD, DD_LD_B_imm, 0x0A,
		LD_C_A,
		LD_D_B,
		EXT_FD, FD_LD_A_ext, 0x00, 0x03,
		NOOP
	};

	cpu.run_to_nop(true);
}
