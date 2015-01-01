//
//  cpu_ops.h
//  mygb
//
//  Created by Daniel on 28.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_cpu_ops_h
#define mygb_cpu_ops_h

namespace opfuncs
{
    
#define OPFSIG (Z80 & cpu, Context & c, KMemory & mmu)
#define OPFRET void
#define OP(x) OPFRET x OPFSIG
    /** Beginn cpu instructions **/
    
    using namespace flag;
    
    OP(LDrr_aa) { c.A = c.A; }
    OP(LDrr_ab) { c.A = c.B; }
    OP(LDrr_ac) { c.A = c.C; }
    OP(LDrr_ad) { c.A = c.D; }
    OP(LDrr_ae) { c.A = c.E; }
    OP(LDrr_ah) { c.A = c.H; }
    OP(LDrr_al) { c.A = c.L; }
    
    OP(LDrr_ba) { c.B = c.A; }
    OP(LDrr_bb) { c.B = c.B; }
    OP(LDrr_bc) { c.B = c.C; }
    OP(LDrr_bd) { c.B = c.D; }
    OP(LDrr_be) { c.B = c.E; }
    OP(LDrr_bh) { c.B = c.H; }
    OP(LDrr_bl) { c.B = c.L; }
    
    OP(LDrr_ca) { c.C = c.A; }
    OP(LDrr_cb) { c.C = c.B; }
    OP(LDrr_cc) { c.C = c.C; }
    OP(LDrr_cd) { c.C = c.D; }
    OP(LDrr_ce) { c.C = c.E; }
    OP(LDrr_ch) { c.C = c.H; }
    OP(LDrr_cl) { c.C = c.L; }
    
    OP(LDrr_da) { c.D = c.A; }
    OP(LDrr_db) { c.D = c.B; }
    OP(LDrr_dc) { c.D = c.C; }
    OP(LDrr_dd) { c.D = c.D; }
    OP(LDrr_de) { c.D = c.E; }
    OP(LDrr_dh) { c.D = c.H; }
    OP(LDrr_dl) { c.D = c.L; }
    
    OP(LDrr_ea) { c.E = c.A; }
    OP(LDrr_eb) { c.E = c.B; }
    OP(LDrr_ec) { c.E = c.C; }
    OP(LDrr_ed) { c.E = c.D; }
    OP(LDrr_ee) { c.E = c.E; }
    OP(LDrr_eh) { c.E = c.H; }
    OP(LDrr_el) { c.E = c.L; }
    
    OP(LDrr_ha) { c.H = c.A; }
    OP(LDrr_hb) { c.H = c.B; }
    OP(LDrr_hc) { c.H = c.C; }
    OP(LDrr_hd) { c.H = c.D; }
    OP(LDrr_he) { c.H = c.E; }
    OP(LDrr_hh) { c.H = c.H; }
    OP(LDrr_hl) { c.H = c.L; }
    
    OP(LDrr_la) { c.L = c.A; }
    OP(LDrr_lb) { c.L = c.B; }
    OP(LDrr_lc) { c.L = c.C; }
    OP(LDrr_ld) { c.L = c.D; }
    OP(LDrr_le) { c.L = c.E; }
    OP(LDrr_lh) { c.L = c.H; }
    OP(LDrr_ll) { c.L = c.L; }
    
    OP(LDrHLm_a){ c.A = mmu.rb(c.HL); }
    OP(LDrHLm_b){ c.B = mmu.rb(c.HL); }
    OP(LDrHLm_c){ c.C = mmu.rb(c.HL); }
    OP(LDrHLm_d){ c.D = mmu.rb(c.HL); }
    OP(LDrHLm_e){ c.E = mmu.rb(c.HL); }
    OP(LDrHLm_h){ c.H = mmu.rb(c.HL); }
    OP(LDrHLm_l){ c.L = mmu.rb(c.HL); }
    
    OP(LDHLmr_a){ mmu.wb(c.HL, c.A); }
    OP(LDHLmr_b){ mmu.wb(c.HL, c.B); }
    OP(LDHLmr_c){ mmu.wb(c.HL, c.C); }
    OP(LDHLmr_d){ mmu.wb(c.HL, c.D); }
    OP(LDHLmr_e){ mmu.wb(c.HL, c.E); }
    OP(LDHLmr_h){ mmu.wb(c.HL, c.H); }
    OP(LDHLmr_l){ mmu.wb(c.HL, c.L); }
    
    OP(LDrn_a)  { c.A = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_b)  { c.B = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_c)  { c.C = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_d)  { c.D = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_e)  { c.E = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_h)  { c.H = mmu.rb(c.PC); c.PC++; }
    OP(LDrn_l)  { c.L = mmu.rb(c.PC); c.PC++; }
    
    OP(LDSPHL)  { c.SP = c.HL; }
    OP(LDHLmn)  { mmu.wb(c.HL, mmu.rb(c.PC)); c.PC++;}
    OP(LDBCmA)  { mmu.wb(c.BC, c.A);  }
    OP(LDDEmA)  { mmu.wb(c.DE, c.A);  }
    OP(LDmmA)   { mmu.wb(mmu.rw(c.PC), c.A); c.PC += 2;}
    OP(LDABCm)  { c.A = mmu.rb(c.BC); }
    OP(LDADEm)  { c.A = mmu.rb(c.DE); }
    OP(LDAmm)   { c.A = mmu.rb(mmu.rw(c.PC)); c.PC += 2;}
    
    OP(LDBCnn)  { c.BC = mmu.rw(c.PC); c.PC += 2;}
    OP(LDDEnn)  { c.DE = mmu.rw(c.PC); c.PC += 2;}
    OP(LDHLnn)  { c.HL = mmu.rw(c.PC); c.PC += 2;}
    
    OP(LDSPnn)  { c.SP = mmu.rw(c.PC); c.PC += 2;}
    OP(LDHLmm)  { u16i i = mmu.rw(c.PC); c.PC += 2; c.L = mmu.rb(i); c.H = mmu.rb(i+1);}
    OP(LDmmHL)  { u16i i = mmu.rw(c.PC); c.PC += 2; mmu.ww(i, c.HL);}
    OP(LDmmSP)  { u16i i = mmu.rw(c.PC); c.PC += 2; mmu.ww(i, c.SP);}
    OP(LDHLIA)  { mmu.wb(c.HL, c.A); c.HL++; }
    OP(LDAHLI)  { c.A = mmu.rb(c.HL); c.HL++; }
    OP(LDHLDA)  { mmu.wb(c.HL, c.A); c.HL--; }
    OP(LDAHLD)  { c.A = mmu.rb(c.HL); c.HL--; }
    
    OP(LDAIOn)  { c.A = mmu.rb(0xFF00 + mmu.rb(c.PC)); c.PC++;}
    OP(LDIOnA)  { mmu.wb(0xFF00 + mmu.rb(c.PC), c.A); c.PC++;}
    OP(LDAIOC)  { c.A = mmu.rb(0xFF00 + c.C); }
    OP(LDIOCA)  { mmu.wb(0xFF00 + c.C, c.A); }
    OP(LDHLSPn)
    {
        s08i n = mmu.rb(c.PC++);
        u16i result = c.SP + n;
        
        clear_all(c);
        
        if(((c.SP ^ n ^ result) & 0x100) == 0x100)
        {
            set(c, CARRY);
        }
        
        if(((c.SP ^ n ^ result) & 0x10) == 0x10)
        {
            set(c, HALFCARRY);
        }
        
        c.HL = result;
        c.T = 12;
    }
    
    
    OP(SWAPr_a)
    {
        u08i i = c.A;
        c.A = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.A == 0);
        
    }
    
    OP(SWAPr_b)
    {
        u08i i = c.B;
        c.B = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.B == 0);
        
    }
    
    OP(SWAPr_c)
    {
        u08i i = c.C;
        c.C = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.C == 0);
        
    }
    
    OP(SWAPr_d)
    {
        u08i i = c.D;
        c.D = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.D == 0);
        
    }
    
    OP(SWAPr_e)
    {
        u08i i = c.E;
        c.E = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.E == 0);
        
    }
    
    OP(SWAPr_h)
    {
        u08i i = c.H;
        c.H = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.H == 0);
        
    }
    
    OP(SWAPr_l)
    {
        u08i i = c.L;
        c.L = (((i & 0x0F ) << 4) | ((i & 0xF0) >> 4));
        clear_all(c);
        set_if(c, ZERO, c.L == 0);
        
    }
    
    OP(SWAPHLm)
    {
        u08i num = mmu.rb(c.HL);
        u08i hi = (num >> 4) & 0x0F;
        u08i lo = (num << 4) & 0xF0;
        mmu.wb(c.HL, hi + lo);
        
        clear_all(c);
        set_if(c, ZERO, (hi + lo) == 0);
        
        c.T = 16;
    }
    
    OP(ADDr_a)  { c.A = add(c, c.A, c.A); }
    OP(ADDr_b)  { c.A = add(c, c.A, c.B); }
    OP(ADDr_c)  { c.A = add(c, c.A, c.C); }
    OP(ADDr_d)  { c.A = add(c, c.A, c.D); }
    OP(ADDr_e)  { c.A = add(c, c.A, c.E); }
    OP(ADDr_h)  { c.A = add(c, c.A, c.H); }
    OP(ADDr_l)  { c.A = add(c, c.A, c.L); }
    
    
    OP(ADDHL)   { c.A = add(c, c.A, mmu.rb(c.HL)); }
    OP(ADDn)    { c.A = add(c, c.A, mmu.rb(c.PC)); c.PC++; }
    OP(ADDHLBC) { c.HL = add16(c, c.HL, c.BC); }
    OP(ADDHLDE) { c.HL = add16(c, c.HL, c.DE); }
    OP(ADDHLHL) { c.HL = add16(c, c.HL, c.HL); }
    OP(ADDHLSP) { c.HL = add16(c, c.HL, c.SP); }
    
    OP(ADDSPn)
    {
        s08i number = static_cast<s08i>(mmu.rb(c.PC));
        int result = c.SP + number;
        clear_all(c);
        
        if(((c.SP ^ number ^ (result & 0xFFFF)) & 0x100) == 0x100)
        {
            set(c, CARRY);
        }
        
        if(((c.SP ^ number ^ (result & 0xFFFF)) & 0x10) == 0x10)
        {
            set(c, HALFCARRY);
        }
        
        c.SP = static_cast<u16i>(result);
        c.PC++;
    }
    
    
    OP(ADCr_a)  { c.A = add_c(c, c.A, c.A); }
    OP(ADCr_b)  { c.A = add_c(c, c.A, c.B); }
    OP(ADCr_c)  { c.A = add_c(c, c.A, c.C); }
    OP(ADCr_d)  { c.A = add_c(c, c.A, c.D); }
    OP(ADCr_e)  { c.A = add_c(c, c.A, c.E); }
    OP(ADCr_h)  { c.A = add_c(c, c.A, c.H); }
    OP(ADCr_l)  { c.A = add_c(c, c.A, c.L); }
    OP(ADCHL)   { c.A = add_c(c, c.A, mmu.rb(c.HL)); }
    OP(ADCn)    { c.A = add_c(c, c.A, mmu.rb(c.PC)); c.PC ++; }
    
    OP(SUBr_a)  { c.A = sub(c, c.A, c.A); }
    OP(SUBr_b)  { c.A = sub(c, c.A, c.B); }
    OP(SUBr_c)  { c.A = sub(c, c.A, c.C); }
    OP(SUBr_d)  { c.A = sub(c, c.A, c.D); }
    OP(SUBr_e)  { c.A = sub(c, c.A, c.E); }
    OP(SUBr_h)  { c.A = sub(c, c.A, c.H); }
    OP(SUBr_l)  { c.A = sub(c, c.A, c.L); }
    OP(SUBHL)   { c.A = sub(c, c.A, mmu.rb(c.HL)); }
    OP(SUBn)    { c.A = sub(c, c.A, mmu.rb(c.PC)); c.PC++; }
    
    OP(SBCr_a)  { c.A = sub_c(c, c.A, c.A); }
    OP(SBCr_b)  { c.A = sub_c(c, c.A, c.B); }
    OP(SBCr_c)  { c.A = sub_c(c, c.A, c.C); }
    OP(SBCr_d)  { c.A = sub_c(c, c.A, c.D); }
    OP(SBCr_e)  { c.A = sub_c(c, c.A, c.E); }
    OP(SBCr_h)  { c.A = sub_c(c, c.A, c.H); }
    OP(SBCr_l)  { c.A = sub_c(c, c.A, c.L); }
    OP(SBCHL)   { c.A = sub_c(c, c.A, mmu.rb(c.HL)); }
    OP(SBCn)    { c.A = sub_c(c, c.A, mmu.rb(c.PC)); c.PC++; }
    
    OP(CPr_a)   {
        u08i number = c.A;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_b)   {
        u08i number = c.B;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_c)   {
        u08i number = c.C;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_d)   {
        u08i number = c.D;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_e)   {
        u08i number = c.E;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_h)   {
        u08i number = c.H;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPr_l)   {
        u08i number = c.L;
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPHL)   {
        u08i number = mmu.rb(c.HL);
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
    }
    
    OP(CPn)   {
        u08i number = mmu.rb(c.PC);
        set_if(c, CARRY, c.A < number);
        set_if(c, ZERO, c.A == number);
        set_if(c, HALFCARRY, ((c.A - number) & 0xF) > (c.A & 0xF));
        set(c, SUBTRACT);
        c.PC++;
    }
    
    OP(ANDr_a)  { c.A &= c.A; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_b)  { c.A &= c.B; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_c)  { c.A &= c.C; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_d)  { c.A &= c.D; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_e)  { c.A &= c.E; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_h)  { c.A &= c.H; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDr_l)  { c.A &= c.L; clear_all(c); set_if(c, ZERO, c.A == 0); set(c, HALFCARRY); }
    OP(ANDHL)   { c.A &= mmu.rb(c.HL); set_if(c, ZERO, c.A == 0); clear(c, SUBTRACT); set(c, HALFCARRY); clear(c, CARRY); }
    OP(ANDn)    { c.A &= mmu.rb(c.PC); c.PC++; set_if(c, ZERO, c.A == 0); clear(c, SUBTRACT); set(c, HALFCARRY); clear(c, CARRY); }
    
    OP(ORr_a)   { c.A |= c.A; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_b)   { c.A |= c.B; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_c)   { c.A |= c.C; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_d)   { c.A |= c.D; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_e)   { c.A |= c.E; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_h)   { c.A |= c.H; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORr_l)   { c.A |= c.L; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORHL)    { c.A |= mmu.rb(c.HL); clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(ORn)     { c.A |= mmu.rb(c.PC); c.PC++; clear_all(c); set_if(c, ZERO, c.A == 0); }
    
    OP(XORr_a)  { c.A ^= c.A; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_b)  { c.A ^= c.B; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_c)  { c.A ^= c.C; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_d)  { c.A ^= c.D; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_e)  { c.A ^= c.E; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_h)  { c.A ^= c.H; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORr_l)  { c.A ^= c.L; clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORHL)   { c.A ^= mmu.rb(c.HL); clear_all(c); set_if(c, ZERO, c.A == 0); }
    OP(XORn)    { c.A ^= mmu.rb(c.PC); c.PC++; clear_all(c); set_if(c, ZERO, c.A == 0); }
    
    OP(INCr_a)  { c.A = inc(c, c.A); }
    OP(INCr_b)  { c.B = inc(c, c.B); }
    OP(INCr_c)  { c.C = inc(c, c.C); }
    OP(INCr_d)  { c.D = inc(c, c.D); }
    OP(INCr_e)  { c.E = inc(c, c.E); }
    OP(INCr_h)  { c.H = inc(c, c.H); }
    OP(INCr_l)  { c.L = inc(c, c.L); }
    OP(INCHLm)  { mmu.wb(c.HL, inc(c, mmu.rb(c.HL)));}
    
    OP(DECr_a)  { c.A = dec(c, c.A); }
    OP(DECr_b)  { c.B = dec(c, c.B); }
    OP(DECr_c)  { c.C = dec(c, c.C); }
    OP(DECr_d)  { c.D = dec(c, c.D); }
    OP(DECr_e)  { c.E = dec(c, c.E); }
    OP(DECr_h)  { c.H = dec(c, c.H); }
    OP(DECr_l)  { c.L = dec(c, c.L); }
    OP(DECHLm)  { mmu.wb(c.HL, dec(c, mmu.rb(c.HL)));}
    
    OP(INCBC)   { c.BC++; }
    OP(INCDE)   { c.DE++; }
    OP(INCHL)   { c.HL++; }
    OP(INCSP)   { c.SP++; }
    
    //KEine Flags!!!
    OP(DECBC)   { c.BC--; }
    OP(DECDE)   { c.DE--; }
    OP(DECHL)   { c.HL--; }
    OP(DECSP)   { c.SP--; }
    
    OP(BIT0a)   { set_if(c, ZERO, (c.A & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0b)   { set_if(c, ZERO, (c.B & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0c)   { set_if(c, ZERO, (c.C & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0d)   { set_if(c, ZERO, (c.D & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0e)   { set_if(c, ZERO, (c.E & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0h)   { set_if(c, ZERO, (c.H & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0l)   { set_if(c, ZERO, (c.L & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT0n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x01) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT1a)   { set_if(c, ZERO, (c.A & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1b)   { set_if(c, ZERO, (c.B & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1c)   { set_if(c, ZERO, (c.C & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1d)   { set_if(c, ZERO, (c.D & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1e)   { set_if(c, ZERO, (c.E & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1h)   { set_if(c, ZERO, (c.H & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1l)   { set_if(c, ZERO, (c.L & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT1n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x02) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT2a)   { set_if(c, ZERO, (c.A & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2b)   { set_if(c, ZERO, (c.B & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2c)   { set_if(c, ZERO, (c.C & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2d)   { set_if(c, ZERO, (c.D & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2e)   { set_if(c, ZERO, (c.E & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2h)   { set_if(c, ZERO, (c.H & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2l)   { set_if(c, ZERO, (c.L & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT2n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x04) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT3a)   { set_if(c, ZERO, (c.A & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3b)   { set_if(c, ZERO, (c.B & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3c)   { set_if(c, ZERO, (c.C & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3d)   { set_if(c, ZERO, (c.D & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3e)   { set_if(c, ZERO, (c.E & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3h)   { set_if(c, ZERO, (c.H & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3l)   { set_if(c, ZERO, (c.L & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT3n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x08) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT4a)   { set_if(c, ZERO, (c.A & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4b)   { set_if(c, ZERO, (c.B & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4c)   { set_if(c, ZERO, (c.C & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4d)   { set_if(c, ZERO, (c.D & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4e)   { set_if(c, ZERO, (c.E & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4h)   { set_if(c, ZERO, (c.H & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4l)   { set_if(c, ZERO, (c.L & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT4n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x10) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT5a)   { set_if(c, ZERO, (c.A & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5b)   { set_if(c, ZERO, (c.B & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5c)   { set_if(c, ZERO, (c.C & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5d)   { set_if(c, ZERO, (c.D & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5e)   { set_if(c, ZERO, (c.E & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5h)   { set_if(c, ZERO, (c.H & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5l)   { set_if(c, ZERO, (c.L & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT5n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x20) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT6a)   { set_if(c, ZERO, (c.A & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6b)   { set_if(c, ZERO, (c.B & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6c)   { set_if(c, ZERO, (c.C & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6d)   { set_if(c, ZERO, (c.D & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6e)   { set_if(c, ZERO, (c.E & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6h)   { set_if(c, ZERO, (c.H & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6l)   { set_if(c, ZERO, (c.L & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT6n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x40) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(BIT7a)   { set_if(c, ZERO, (c.A & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7b)   { set_if(c, ZERO, (c.B & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7c)   { set_if(c, ZERO, (c.C & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7d)   { set_if(c, ZERO, (c.D & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7e)   { set_if(c, ZERO, (c.E & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7h)   { set_if(c, ZERO, (c.H & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7l)   { set_if(c, ZERO, (c.L & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY); }
    OP(BIT7n)   { set_if(c, ZERO, (mmu.rb(c.HL) & 0x80) == 0); clear(c, SUBTRACT); set(c, HALFCARRY);}
    
    OP(RLA)     { c.A = rla(c, c.A); }
    OP(RLCA)    { c.A = rlca(c, c.A); }
    OP(RRA)     { c.A = rra(c, c.A); }
    OP(RRCA)    { c.A = rrca(c, c.A); }
    
    OP(RLr_a)   { c.A = rla(c, c.A); set_if(c, ZERO, c.A == 0); }
    OP(RLr_b)   { c.B = rla(c, c.B); set_if(c, ZERO, c.B == 0); }
    OP(RLr_c)   { c.C = rla(c, c.C); set_if(c, ZERO, c.C == 0); }
    OP(RLr_d)   { c.D = rla(c, c.D); set_if(c, ZERO, c.D == 0); }
    OP(RLr_e)   { c.E = rla(c, c.E); set_if(c, ZERO, c.E == 0); }
    OP(RLr_h)   { c.H = rla(c, c.H); set_if(c, ZERO, c.H == 0); }
    OP(RLr_l)   { c.L = rla(c, c.L); set_if(c, ZERO, c.L == 0); }
    OP(RLHL)    { u16i addr = c.HL; mmu.wb(addr, rla(c, mmu.rb(addr))); set_if(c, ZERO, mmu.rb(addr) == 0);}
    
    ///nH = 0, CZ wird gesetzt
    OP(RLCr_a)  { c.A = rlca(c, c.A); set_if(c, ZERO, c.A == 0); }
    OP(RLCr_b)  { c.B = rlca(c, c.B); set_if(c, ZERO, c.B == 0); }
    OP(RLCr_c)  { c.C = rlca(c, c.C); set_if(c, ZERO, c.C == 0); }
    OP(RLCr_d)  { c.D = rlca(c, c.D); set_if(c, ZERO, c.D == 0); }
    OP(RLCr_e)  { c.E = rlca(c, c.E); set_if(c, ZERO, c.E == 0); }
    OP(RLCr_h)  { c.H = rlca(c, c.H); set_if(c, ZERO, c.H == 0); }
    OP(RLCr_l)  { c.L = rlca(c, c.L); set_if(c, ZERO, c.L == 0); }
    OP(RLCHL)   { u16i addr = c.HL; mmu.wb(addr, rlca(c, mmu.rb(addr))); set_if(c, ZERO, mmu.rb(addr) == 0);}
    
    OP(RRr_a)   { c.A = rra(c, c.A); set_if(c, ZERO, c.A == 0); }
    OP(RRr_b)   { c.B = rra(c, c.B); set_if(c, ZERO, c.B == 0); }
    OP(RRr_c)   { c.C = rra(c, c.C); set_if(c, ZERO, c.C == 0); }
    OP(RRr_d)   { c.D = rra(c, c.D); set_if(c, ZERO, c.D == 0); }
    OP(RRr_e)   { c.E = rra(c, c.E); set_if(c, ZERO, c.E == 0); }
    OP(RRr_h)   { c.H = rra(c, c.H); set_if(c, ZERO, c.H == 0); }
    OP(RRr_l)   { c.L = rra(c, c.L); set_if(c, ZERO, c.L == 0); }
    OP(RRHL)    { u16i addr = c.HL; mmu.wb(addr, rra(c, mmu.rb(addr))); set_if(c, ZERO, mmu.rb(addr) == 0);}
    
    OP(RRCr_a)  { c.A = rrca(c, c.A); set_if(c, ZERO, c.A == 0); }
    OP(RRCr_b)  { c.B = rrca(c, c.B); set_if(c, ZERO, c.B == 0); }
    OP(RRCr_c)  { c.C = rrca(c, c.C); set_if(c, ZERO, c.C == 0); }
    OP(RRCr_d)  { c.D = rrca(c, c.D); set_if(c, ZERO, c.D == 0); }
    OP(RRCr_e)  { c.E = rrca(c, c.E); set_if(c, ZERO, c.E == 0); }
    OP(RRCr_h)  { c.H = rrca(c, c.H); set_if(c, ZERO, c.H == 0); }
    OP(RRCr_l)  { c.L = rrca(c, c.L); set_if(c, ZERO, c.L == 0); }
    OP(RRCHL)   { u16i addr = c.HL; mmu.wb(addr, rrca(c, mmu.rb(addr))); set_if(c, ZERO, mmu.rb(addr) == 0);}
    
    OP(SLAr_a)  { c.A = sla(c, c.A); }
    OP(SLAr_b)  { c.B = sla(c, c.B); }
    OP(SLAr_c)  { c.C = sla(c, c.C); }
    OP(SLAr_d)  { c.D = sla(c, c.D); }
    OP(SLAr_e)  { c.E = sla(c, c.E); }
    OP(SLAr_h)  { c.H = sla(c, c.H); }
    OP(SLAr_l)  { c.L = sla(c, c.L); }
    OP(SLAHL)  { mmu.wb(c.HL, sla(c, mmu.rb(c.HL)));}
    
    OP(SLLr_a)  { c.A = sll(c, c.A); }
    OP(SLLr_b)  { c.B = sll(c, c.B); }
    OP(SLLr_c)  { c.C = sll(c, c.C); }
    OP(SLLr_d)  { c.D = sll(c, c.D); }
    OP(SLLr_e)  { c.E = sll(c, c.E); }
    OP(SLLr_h)  { c.H = sll(c, c.H); }
    OP(SLLr_l)  { c.L = sll(c, c.L); }
    
    OP(SRAr_a)  { c.A = sra(c, c.A); }
    OP(SRAr_b)  { c.B = sra(c, c.B); }
    OP(SRAr_c)  { c.C = sra(c, c.C); }
    OP(SRAr_d)  { c.D = sra(c, c.D); }
    OP(SRAr_e)  { c.E = sra(c, c.E); }
    OP(SRAr_h)  { c.H = sra(c, c.H); }
    OP(SRAr_l)  { c.L = sra(c, c.L); }
    OP(SRAHL)   { mmu.wb(c.HL, sra(c, mmu.rb(c.HL)));}
    
    OP(SRLr_a)  { c.A = srl(c, c.A); }
    OP(SRLr_b)  { c.B = srl(c, c.B); }
    OP(SRLr_c)  { c.C = srl(c, c.C); }
    OP(SRLr_d)  { c.D = srl(c, c.D); }
    OP(SRLr_e)  { c.E = srl(c, c.E); }
    OP(SRLr_h)  { c.H = srl(c, c.H); }
    OP(SRLr_l)  { c.L = srl(c, c.L); }
    OP(SRLHL)   { mmu.wb(c.HL, srl(c, mmu.rb(c.HL)));}
    
    OP(CPL)     { c.A = ~c.A; set(c, HALFCARRY); set(c, SUBTRACT); }
    OP(CCF)     { flip(c, CARRY); clear(c, SUBTRACT); clear(c, HALFCARRY); }
    OP(SCF)     { set(c, CARRY); clear(c, SUBTRACT); clear(c, HALFCARRY); }
    
    OP(PUSHAF)  { push<u16i>(c, mmu, c.AF);}
    OP(PUSHBC)  { push<u16i>(c, mmu, c.BC);}
    OP(PUSHDE)  { push<u16i>(c, mmu, c.DE);}
    OP(PUSHHL)  { push<u16i>(c, mmu, c.HL);}
    
    OP(POPAF)   { c.AF = pop<u16i>(c, mmu); c.FLAG &= FLAG_ALWAYS_ZERO_MASK;}
    OP(POPBC)   { c.BC = pop<u16i>(c, mmu);}
    OP(POPDE)   { c.DE = pop<u16i>(c, mmu);}
    OP(POPHL)   { c.HL = pop<u16i>(c, mmu);}
    
    OP(NOP)     { }
    OP(HALT)    { c.halt = 1; }
    OP(DI)      { c.ime = 0; }
    OP(EI)      { c.ime = 1; }
    
    OP(RET)     { ret_if(c, mmu, true);}
    OP(RETI)    { ret_if(c, mmu, true); c.ime = 1;}
    OP(RETNZ)   { ret_if(c, mmu, !is_set(c, ZERO)); }
    OP(RETZ)    { ret_if(c, mmu, is_set(c, ZERO)); }
    OP(RETC)    { ret_if(c, mmu, is_set(c, CARRY)); }
    OP(RETNC)   { ret_if(c, mmu, !is_set(c, CARRY)); }
    
    OP(RST00)   { push<u16i>(c, mmu, c.PC); c.PC = 0x00;}
    OP(RST08)   { push<u16i>(c, mmu, c.PC); c.PC = 0x08;}
    OP(RST10)   { push<u16i>(c, mmu, c.PC); c.PC = 0x10;}
    OP(RST18)   { push<u16i>(c, mmu, c.PC); c.PC = 0x18;}
    OP(RST20)   { push<u16i>(c, mmu, c.PC); c.PC = 0x20;}
    OP(RST28)   { push<u16i>(c, mmu, c.PC); c.PC = 0x28;}
    OP(RST30)   { push<u16i>(c, mmu, c.PC); c.PC = 0x30;}
    OP(RST38)   { push<u16i>(c, mmu, c.PC); c.PC = 0x38;}
    OP(RST40)   { push<u16i>(c, mmu, c.PC); c.PC = 0x40;}
    OP(RST48)   { push<u16i>(c, mmu, c.PC); c.PC = 0x48;}
    OP(RST50)   { push<u16i>(c, mmu, c.PC); c.PC = 0x50;}
    OP(RST58)   { push<u16i>(c, mmu, c.PC); c.PC = 0x58;}
    OP(RST60)   { push<u16i>(c, mmu, c.PC); c.PC = 0x60;}
    
    OP(CALLnn)  { push<u16i>(c, mmu, c.PC + 2); c.PC = mmu.rw(c.PC); }
    OP(CALLNZnn){ call_if(c, mmu, !is_set(c, ZERO)); }
    OP(CALLZnn) { call_if(c, mmu, is_set(c, ZERO)); }
    OP(CALLNCnn){ call_if(c, mmu, !is_set(c, CARRY)); }
    OP(CALLCnn) { call_if(c, mmu, is_set(c, CARRY)); }
    
    OP(JPnn)    { jmp_if(c, mmu, true); }
    OP(JPHL)    { c.PC = c.HL; }
    OP(JPNZnn)  { jmp_if(c, mmu, !is_set(c, ZERO)); }
    OP(JPZnn)   { jmp_if(c, mmu, is_set(c, ZERO)); }
    OP(JPNCnn)  { jmp_if(c, mmu, !is_set(c, CARRY)); }
    OP(JPCnn)   { jmp_if(c, mmu, is_set(c, CARRY)); }
    
    OP(JRn)     { jr_if(c, mmu, true); }
    OP(JRNZn)   { jr_if(c, mmu, !is_set(c, ZERO)); }
    OP(JRZn)    { jr_if(c, mmu, is_set(c, ZERO)); }
    OP(JRNCn)   { jr_if(c, mmu, !is_set(c, CARRY)); }
    OP(JRCn)    { jr_if(c, mmu, is_set(c, CARRY)); }
    
    OP(RES0a)   { c.A &= ~BIT_0; }
    OP(RES0b)   { c.B &= ~BIT_0; }
    OP(RES0c)   { c.C &= ~BIT_0; }
    OP(RES0d)   { c.D &= ~BIT_0; }
    OP(RES0e)   { c.E &= ~BIT_0; }
    OP(RES0h)   { c.H &= ~BIT_0; }
    OP(RES0l)   { c.L &= ~BIT_0; }
    OP(RES0m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_0); }
    
    OP(RES1a)   { c.A &= ~BIT_1; }
    OP(RES1b)   { c.B &= ~BIT_1; }
    OP(RES1c)   { c.C &= ~BIT_1; }
    OP(RES1d)   { c.D &= ~BIT_1; }
    OP(RES1e)   { c.E &= ~BIT_1; }
    OP(RES1h)   { c.H &= ~BIT_1; }
    OP(RES1l)   { c.L &= ~BIT_1; }
    OP(RES1m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_1); }
    
    OP(RES2a)   { c.A &= ~BIT_2; }
    OP(RES2b)   { c.B &= ~BIT_2; }
    OP(RES2c)   { c.C &= ~BIT_2; }
    OP(RES2d)   { c.D &= ~BIT_2; }
    OP(RES2e)   { c.E &= ~BIT_2; }
    OP(RES2h)   { c.H &= ~BIT_2; }
    OP(RES2l)   { c.L &= ~BIT_2; }
    OP(RES2m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_2); }
    
    OP(RES3a)   { c.A &= ~BIT_3; }
    OP(RES3b)   { c.B &= ~BIT_3; }
    OP(RES3c)   { c.C &= ~BIT_3; }
    OP(RES3d)   { c.D &= ~BIT_3; }
    OP(RES3e)   { c.E &= ~BIT_3; }
    OP(RES3h)   { c.H &= ~BIT_3; }
    OP(RES3l)   { c.L &= ~BIT_3; }
    OP(RES3m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_3); }
    
    OP(RES4a)   { c.A &= ~BIT_4; }
    OP(RES4b)   { c.B &= ~BIT_4; }
    OP(RES4c)   { c.C &= ~BIT_4; }
    OP(RES4d)   { c.D &= ~BIT_4; }
    OP(RES4e)   { c.E &= ~BIT_4; }
    OP(RES4h)   { c.H &= ~BIT_4; }
    OP(RES4l)   { c.L &= ~BIT_4; }
    OP(RES4m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_4); }
    
    OP(RES5a)   { c.A &= ~BIT_5; }
    OP(RES5b)   { c.B &= ~BIT_5; }
    OP(RES5c)   { c.C &= ~BIT_5; }
    OP(RES5d)   { c.D &= ~BIT_5; }
    OP(RES5e)   { c.E &= ~BIT_5; }
    OP(RES5h)   { c.H &= ~BIT_5; }
    OP(RES5l)   { c.L &= ~BIT_5; }
    OP(RES5m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_5); }
    
    OP(RES6a)   { c.A &= ~BIT_6; }
    OP(RES6b)   { c.B &= ~BIT_6; }
    OP(RES6c)   { c.C &= ~BIT_6; }
    OP(RES6d)   { c.D &= ~BIT_6; }
    OP(RES6e)   { c.E &= ~BIT_6; }
    OP(RES6h)   { c.H &= ~BIT_6; }
    OP(RES6l)   { c.L &= ~BIT_6; }
    OP(RES6m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_6); }
    
    OP(RES7a)   { c.A &= ~BIT_7; }
    OP(RES7b)   { c.B &= ~BIT_7; }
    OP(RES7c)   { c.C &= ~BIT_7; }
    OP(RES7d)   { c.D &= ~BIT_7; }
    OP(RES7e)   { c.E &= ~BIT_7; }
    OP(RES7h)   { c.H &= ~BIT_7; }
    OP(RES7l)   { c.L &= ~BIT_7; }
    OP(RES7m)   { mmu.wb(c.HL, mmu.rb(c.HL) & ~BIT_7); }
    
    OP(SET0a)   { c.A |= BIT_0; }
    OP(SET0b)   { c.B |= BIT_0; }
    OP(SET0c)   { c.C |= BIT_0; }
    OP(SET0d)   { c.D |= BIT_0; }
    OP(SET0e)   { c.E |= BIT_0; }
    OP(SET0h)   { c.H |= BIT_0; }
    OP(SET0l)   { c.L |= BIT_0; }
    OP(SET0m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_0); }
    
    OP(SET1a)   { c.A |= BIT_1; }
    OP(SET1b)   { c.B |= BIT_1; }
    OP(SET1c)   { c.C |= BIT_1; }
    OP(SET1d)   { c.D |= BIT_1; }
    OP(SET1e)   { c.E |= BIT_1; }
    OP(SET1h)   { c.H |= BIT_1; }
    OP(SET1l)   { c.L |= BIT_1; }
    OP(SET1m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_1); }
    
    OP(SET2a)   { c.A |= BIT_2; }
    OP(SET2b)   { c.B |= BIT_2; }
    OP(SET2c)   { c.C |= BIT_2; }
    OP(SET2d)   { c.D |= BIT_2; }
    OP(SET2e)   { c.E |= BIT_2; }
    OP(SET2h)   { c.H |= BIT_2; }
    OP(SET2l)   { c.L |= BIT_2; }
    OP(SET2m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_2); }
    
    OP(SET3a)   { c.A |= BIT_3; }
    OP(SET3b)   { c.B |= BIT_3; }
    OP(SET3c)   { c.C |= BIT_3; }
    OP(SET3d)   { c.D |= BIT_3; }
    OP(SET3e)   { c.E |= BIT_3; }
    OP(SET3h)   { c.H |= BIT_3; }
    OP(SET3l)   { c.L |= BIT_3; }
    OP(SET3m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_3); }
    
    OP(SET4a)   { c.A |= BIT_4; }
    OP(SET4b)   { c.B |= BIT_4; }
    OP(SET4c)   { c.C |= BIT_4; }
    OP(SET4d)   { c.D |= BIT_4; }
    OP(SET4e)   { c.E |= BIT_4; }
    OP(SET4h)   { c.H |= BIT_4; }
    OP(SET4l)   { c.L |= BIT_4; }
    OP(SET4m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_4); }
    
    OP(SET5a)   { c.A |= BIT_5; }
    OP(SET5b)   { c.B |= BIT_5; }
    OP(SET5c)   { c.C |= BIT_5; }
    OP(SET5d)   { c.D |= BIT_5; }
    OP(SET5e)   { c.E |= BIT_5; }
    OP(SET5h)   { c.H |= BIT_5; }
    OP(SET5l)   { c.L |= BIT_5; }
    OP(SET5m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_5); }
    
    OP(SET6a)   { c.A |= BIT_6; }
    OP(SET6b)   { c.B |= BIT_6; }
    OP(SET6c)   { c.C |= BIT_6; }
    OP(SET6d)   { c.D |= BIT_6; }
    OP(SET6e)   { c.E |= BIT_6; }
    OP(SET6h)   { c.H |= BIT_6; }
    OP(SET6l)   { c.L |= BIT_6; }
    OP(SET6m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_6); }
    
    OP(SET7a)   { c.A |= BIT_7; }
    OP(SET7b)   { c.B |= BIT_7; }
    OP(SET7c)   { c.C |= BIT_7; }
    OP(SET7d)   { c.D |= BIT_7; }
    OP(SET7e)   { c.E |= BIT_7; }
    OP(SET7h)   { c.H |= BIT_7; }
    OP(SET7l)   { c.L |= BIT_7; }
    OP(SET7m)   { mmu.wb(c.HL, mmu.rb(c.HL) | BIT_7); }
    
    OP(DAA)     {
        
        int a = c.A;
        
        if(!is_set(c, SUBTRACT))
        {
            if(is_set(c, HALFCARRY) || ((a & 0xF) > 9))
            {
                a += 0x06;
            }
            
            if(is_set(c, CARRY) || (a > 0x9F))
            {
                a += 0x60;
            }
        }
        else
        {
            if(is_set(c, HALFCARRY))
            {
                a = (a - 6) & 0xFF;
            }
            
            if(is_set(c, CARRY))
            {
                a -= 0x60;
            }
        }
        
        clear(c, HALFCARRY);
        
        set_if(c, CARRY, ((a & 0x100) == 0x100));
        
        a &= 0xFF;
        
        set_if(c, ZERO, a == 0);
        
        c.A = static_cast<u08i>(a);
    }
    
    OP(STOP) {
        //TODO: Implement!!
        
        /* For now: Endless loop */
        //c.PC--;
        
        std::printf("PC: 0x%x, OP: 0x%x, OP-PC-1: 0x%x\n", c.PC - 1, mmu.rb(c.PC - 1), mmu.rb(c.PC -2));
        std::cout << "Stopped!!" << std::endl;
        //Debugger::getInstance()->interrupt();
        
        /* Todo: Check for button presses! */
    }
    
    /** End cpu instructions **/
#undef OPFSIG
#undef OPFRET
#undef OPFUNC
    
    
}


#endif
