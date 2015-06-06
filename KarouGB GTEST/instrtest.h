//
//  instrtest.h
//  KarouGB
//
//  Created by Daniel on 08.04.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef KarouGB_instrtest_h
#define KarouGB_instrtest_h

#include <gtest/gtest.h>
#include <memory>
#include "../KarouGB/mem/cart_loader.h"
#include "../KarouGB/cpu/cpu.h"

using namespace cpu;

#define SETUP_OPCODE(x) zero_registers();\
                        c->PC = baseAddress;\
                        mmu->wb(baseAddress, x)
#define SET_IMMEDIATE1(x)  mmu->wb(c->PC + 1, x)
#define SET_IMMEDIATE2(x)  mmu->wb(c->PC + 2, x)
#define RUN(x) cpu->execute(*x)

class instruction_test : public testing::Test
{
public:
    std::shared_ptr<KCartridgeLoader> loader;
    std::shared_ptr<KMemory> mmu;
    std::shared_ptr<Z80> cpu;
    std::unique_ptr<Context> c;
    
    u16i baseAddress;
    
    enum FlagEnum
    {
        CARRY = BIT_4,
        HALFCARRY = BIT_5,
        SUBTRACT = BIT_6,
        ZERO = BIT_7,
    };
    
    void SetUp()
    {
        loader = KCartridgeLoader::load("cpu_instrs.gb");
        ASSERT_TRUE(loader != nullptr);
        mmu = loader->getMemory();
        ASSERT_TRUE(mmu != nullptr);
        
        cpu =       std::make_shared<Z80>(mmu);
        c =         std::move(std::unique_ptr<Context>(new Context));
        
        ASSERT_TRUE(cpu != nullptr);
        ASSERT_TRUE(c != nullptr);
        
        baseAddress = 0xC000;
        c->PC = baseAddress;
        ASSERT_EQ(c->PC, baseAddress);
    }
    
    void TearDown()
    {
    }
    
    void zero_registers()
    {
        c->AF = 0;
        c->BC = 0;
        c->DE = 0;
        c->HL = 0;
        c->PC = 0;
        c->SP = 0;
        c->FLAG = 0;
        c->ime = 0;
    }
    
    /* Multi-Tests */
    
    /* 8-Bit-Register Ladefunktionen. Schema:
        LD r1, r2
        Lade r2 nach r1 */
    void test_ld(u08i & dst, u08i & src, u08i opcode)
    {
        zero_registers();
        
        c->PC = baseAddress;
        c->FLAG = 0;
        mmu->wb(c->PC, opcode);
        
        dst = 0;
        src = 42;
        
        cpu->execute(*c);
        
        ASSERT_EQ(src, dst);
        ASSERT_EQ(c->FLAG, 0);
        ASSERT_EQ(c->PC, baseAddress + 1);
    }
    
    //tests (HL) -> R
    void test_ld_mem_hl(u08i & dst, u08i opcode)
    {
        zero_registers();
        
        c->PC = baseAddress;
        c->FLAG = 0;
        dst = 0;
        
        mmu->wb(c->PC, opcode);
        mmu->wb(0xC200, 42);
        c->HL = 0xC200;
        
        ASSERT_EQ(mmu->rb(c->HL), 42);
        ASSERT_EQ(mmu->rb(c->PC), opcode);
        
        cpu->execute(*c);
                
        ASSERT_EQ(dst, 42);
        ASSERT_EQ(c->FLAG, 0);
        ASSERT_EQ(c->PC, baseAddress + 1);
    }
    
    //tests R -> (HL)
    //Funktioniert NICHT für H -> (HL), L -> (HL)
    void test_ld_hl_mem(u08i & src, u08i opcode)
    {
        zero_registers();
        
        c->PC = baseAddress;
        c->FLAG = 0;
        mmu->wb(c->PC, opcode);
        mmu->wb(0xC200, 0);
        c->HL = 0xC200;
        src = 42;
        
        ASSERT_EQ(mmu->rb(0xC200), 0);
        ASSERT_EQ(mmu->rb(c->PC), opcode);
        
        cpu->execute(*c);
        
        ASSERT_EQ(mmu->rb(0xC200), 42);
        ASSERT_EQ(c->FLAG, 0);
        ASSERT_EQ(c->PC, baseAddress + 1);
    }
    
    void test_ld_mem_a(u16i & src, u08i opcode)
    {
        zero_registers();
        
        c->PC = baseAddress;
        c->FLAG = 0;
        c->A = 0;
        
        mmu->wb(c->PC, opcode);
        mmu->wb(0xC211, 42);
        src = 0xC211;
        
        ASSERT_EQ(mmu->rb(src), 42);
        ASSERT_EQ(mmu->rb(c->PC), opcode);
        
        cpu->execute(*c);
        
        ASSERT_EQ(c->A, 42);
        ASSERT_EQ(c->FLAG, 0);
        ASSERT_EQ(c->PC, baseAddress + 1);
    }
    
    void test_inc_rxx(u16i & reg, u08i opcode)
    {
        zero_registers();
        
        const u08i flag_full = 0xFF;
        const u08i flag_zero = 0x00;
        
        /* Teste INC XX, Reg=0xFFFF, flags sollten sich nicht verändern */
        reg = 0xFFFF;
        c->PC = baseAddress;
        c->FLAG = flag_zero;
        
        // Schreibe den befehl: INC A (0x3C)
        mmu->wb(baseAddress, opcode);
        ASSERT_EQ(mmu->rb(baseAddress), opcode);
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Der Program Counter sollte baseAddres += 1 sein
        ASSERT_EQ(c->PC, baseAddress+1);
        // Register A sollte 1 sein.
        ASSERT_EQ(reg, 0);
        // Subtract-Flag sollte gelöscht sein
        ASSERT_EQ(c->FLAG, flag_zero);
        
        
        /* Teste INC XX, Reg=0, flags sollten sich nicht verändern */
        reg = 0;
        c->PC = baseAddress;
        c->FLAG = flag_full;
        
        // Schreibe den befehl: INC A (0x3C)
        mmu->wb(baseAddress, opcode);
        ASSERT_EQ(mmu->rb(baseAddress), opcode);
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Der Program Counter sollte baseAddres += 1 sein
        ASSERT_EQ(c->PC, baseAddress+1);
        // Register A sollte 1 sein.
        ASSERT_EQ(reg, 1);
        // Subtract-Flag sollte gelöscht sein
        ASSERT_EQ(c->FLAG, flag_full);
    }
    
    void test_inc_rx(u08i & reg, u08i opcode)
    {
        zero_registers();
        
        /* Teste INC A, A=0 */
        reg = 0;
        c->PC = baseAddress;
        c->FLAG = SUBTRACT;
        
        // Schreibe den befehl: INC A (0x3C)
        mmu->wb(baseAddress, opcode);
        ASSERT_EQ(mmu->rb(baseAddress), opcode);
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Der Program Counter sollte baseAddres += 1 sein
        ASSERT_EQ(c->PC, baseAddress+1);
        
        // Register A sollte 1 sein.
        ASSERT_EQ(reg, 1);
        // Subtract-Flag sollte gelöscht sein
        ASSERT_EQ(c->FLAG & SUBTRACT, 0);
        // Zero-Flag sollte gelöscht sein
        ASSERT_EQ(c->FLAG & ZERO, 0);
        // Halfcarry sollte gelöscht sein
        ASSERT_EQ(c->FLAG & HALFCARRY, 0);
        
        
        /* Teste INC A, A=255 */
        reg = 0xFF;
        c->FLAG = 0;
        c->PC = baseAddress;
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Register A sollte 0 sein.
        ASSERT_EQ(reg, 0);
        // Subtract-Flag sollte gelöscht sein
        ASSERT_FALSE(c->FLAG & SUBTRACT);
        // Zero-Flag sollte gesetzt sein
        ASSERT_TRUE(c->FLAG & ZERO);
        // Halfcarry sollte gesetzt sein, da ((a&0xf) + (value&0xf))&0x10
        // mit a=0xFF und value=0x01 gleich 0x10 (true) ist.
        ASSERT_TRUE(c->FLAG & HALFCARRY);
        
        /* Teste INC A, A=14 => HC sollte nicht gesetzt sein */
        reg = 14;
        c->FLAG = HALFCARRY;
        c->PC = baseAddress;
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Register sollte 15 sein
        ASSERT_EQ(reg, 15);
        // Halfcarry sollte nicht gesetzt sein, da kein overflow von BIT3 nach BIT4
        ASSERT_FALSE(c->FLAG & HALFCARRY);
        
        
        /* Teste INC A, A=15 => HC sollt gesetzt sein */
        reg = 15;
        c->FLAG = 0;
        c->PC = baseAddress;
        
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Register sollte 16 sein
        ASSERT_EQ(reg, 16);
        // Halfcarry sollte gesetzt sein, da overflow von BIT3 nach BIT4
        ASSERT_TRUE(c->FLAG & HALFCARRY);
    }
    
    void test_add_rxx(u16i & regA, u16i & regB, u08i opcode)
    {
        zero_registers();
        
        /* Teste INC A, A=0 */
        regA = 0xFFFF;
        regB = 0x0001;
        c->PC = baseAddress;
        c->FLAG = SUBTRACT;
        
        // Schreibe den befehl: INC A (0x3C)
        mmu->wb(baseAddress, opcode);
        ASSERT_EQ(mmu->rb(baseAddress), opcode);
        // Führe den Befehl aus
        cpu->execute(*c);
        
        // Der Program Counter sollte baseAddres += 1 sein
        ASSERT_EQ(c->PC, baseAddress+1);
        
        ASSERT_EQ(regA, 0);
        // Carry sollte gesetzt sein
        ASSERT_TRUE(c->FLAG & CARRY);
        // Subtract sollte nicht gesetzt sein
        ASSERT_FALSE(c->FLAG & SUBTRACT);
        // Das Zero-Flag wird nicht beeinflusst - sollte also 0 sein
        ASSERT_FALSE(c->FLAG & ZERO);
        // Halfcarry sollte gesetzt sein
        ASSERT_TRUE(c->FLAG & HALFCARRY);
    }
};

TEST_F(instruction_test, LoadInstrs_LD_8BIT_REG)
{
    test_ld(c->A, c->A, 0x7F);
    test_ld(c->A, c->B, 0x78);
    test_ld(c->A, c->C, 0x79);
    test_ld(c->A, c->D, 0x7A);
    test_ld(c->A, c->E, 0x7B);
    test_ld(c->A, c->H, 0x7C);
    test_ld(c->A, c->L, 0x7D);
    test_ld_mem_hl(c->A, 0x7E);

    test_ld(c->B, c->A, 0x47);
    test_ld(c->B, c->B, 0x40);
    test_ld(c->B, c->C, 0x41);
    test_ld(c->B, c->D, 0x42);
    test_ld(c->B, c->E, 0x43);
    test_ld(c->B, c->H, 0x44);
    test_ld(c->B, c->L, 0x45);
    test_ld_mem_hl(c->B, 0x46);
    
    test_ld(c->C, c->A, 0x4F);
    test_ld(c->C, c->B, 0x48);
    test_ld(c->C, c->C, 0x49);
    test_ld(c->C, c->D, 0x4A);
    test_ld(c->C, c->E, 0x4B);
    test_ld(c->C, c->H, 0x4C);
    test_ld(c->C, c->L, 0x4D);
    test_ld_mem_hl(c->C, 0x4E);
    
    test_ld(c->D, c->A, 0x57);
    test_ld(c->D, c->B, 0x50);
    test_ld(c->D, c->C, 0x51);
    test_ld(c->D, c->D, 0x52);
    test_ld(c->D, c->E, 0x53);
    test_ld(c->D, c->H, 0x54);
    test_ld(c->D, c->L, 0x55);
    test_ld_mem_hl(c->D, 0x56);
    
    test_ld(c->E, c->A, 0x5F);
    test_ld(c->E, c->B, 0x58);
    test_ld(c->E, c->C, 0x59);
    test_ld(c->E, c->D, 0x5A);
    test_ld(c->E, c->E, 0x5B);
    test_ld(c->E, c->H, 0x5C);
    test_ld(c->E, c->L, 0x5D);
    test_ld_mem_hl(c->E, 0x5E);
    
    test_ld(c->H, c->A, 0x67);
    test_ld(c->H, c->B, 0x60);
    test_ld(c->H, c->C, 0x61);
    test_ld(c->H, c->D, 0x62);
    test_ld(c->H, c->E, 0x63);
    test_ld(c->H, c->H, 0x64);
    test_ld(c->H, c->L, 0x65);
    test_ld_mem_hl(c->H, 0x66);
    
    test_ld(c->L, c->A, 0x6F);
    test_ld(c->L, c->B, 0x68);
    test_ld(c->L, c->C, 0x69);
    test_ld(c->L, c->D, 0x6A);
    test_ld(c->L, c->E, 0x6B);
    test_ld(c->L, c->H, 0x6C);
    test_ld(c->L, c->L, 0x6D);
    test_ld_mem_hl(c->L, 0x6E);
    
    
    test_ld_hl_mem(c->A, 0x77);
    test_ld_hl_mem(c->B, 0x70);
    test_ld_hl_mem(c->C, 0x71);
    test_ld_hl_mem(c->D, 0x72);
    test_ld_hl_mem(c->E, 0x73);
}

TEST_F(instruction_test, TestInstr_LD_A_C)
{
    c->PC = baseAddress;
    mmu->wb(c->PC, 0xF2);
    
    mmu->wb(0xFF01, 42);
    ASSERT_EQ(mmu->rb(0xFF01), 42);
    
    c->C = 0x01;
    c->A = 0x00;
    c->FLAG = 0;
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->C, 0x01);
    ASSERT_EQ(c->FLAG, 0x00);
    ASSERT_EQ(c->PC, baseAddress + 1);
}

TEST_F(instruction_test, TestInstr_LD_C_A)
{
    c->PC = baseAddress;
    mmu->wb(c->PC, 0xE2);
    
    c->A = 42;
    c->C = 01;
    c->FLAG = 0;
    
    mmu->wb(0xFF01, 0);
    ASSERT_EQ(mmu->rb(0xFF01), 0);
    
    cpu->execute(*c);
    
    ASSERT_EQ(mmu->rb(0xFF01), 42);
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->C, 01);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
}

TEST_F(instruction_test, TestInstr_LD_XX_TO_A)
{
    test_ld_mem_a(c->BC, 0x0A);
    test_ld_mem_a(c->DE, 0x1A);
    test_ld_mem_a(c->HL, 0x7E);
}

TEST_F(instruction_test, LDD_A_HL)
{
    zero_registers();
    
    c->HL = 0xC211;
    mmu->wb(c->HL, 42);
    ASSERT_EQ(mmu->rb(0xC211), 42);
    
    c->PC = baseAddress;
    mmu->wb(c->PC, 0x3A);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->HL, 0xC210);
    ASSERT_EQ(c->PC, baseAddress + 1);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, LDD_HL_A)
{
    zero_registers();
    
    mmu->wb(0xC211, 0);
    c->HL = 0xC211;
    c->A = 42;
    
    c->PC = baseAddress;
    mmu->wb(c->PC, 0x32);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(mmu->rb(0xC211), 42);
    ASSERT_EQ(c->HL, 0xC210);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
}

TEST_F(instruction_test, TestInstr_LDI_A_HL)
{
    zero_registers();
    
    mmu->wb(0xC211, 42);
    c->HL = 0xC211;
    
    c->PC = baseAddress;
    mmu->wb(c->PC, 0x2A);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->HL, 0xC212);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
    ASSERT_EQ(mmu->rb(0xC211), 42);
}

TEST_F(instruction_test, TestInstr_LDI_HL_A)
{
    zero_registers();
    
    mmu->wb(0xC211, 0);
    c->HL = 0xC211;
    c->A = 42;
    
    c->PC = baseAddress;
    mmu->wb(c->PC, 0x22);
    
    cpu->execute(*c);
    
    ASSERT_EQ(mmu->rb(0xC211), 42);
    ASSERT_EQ(c->HL, 0xC212);
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
}

TEST_F(instruction_test, TestInstr_LDH_N_A)
{
    SETUP_OPCODE(0xE0);
    SET_IMMEDIATE1(01);
    
    mmu->wb(0xFF01, 0);
    c->A = 42;
    
    RUN(c);
    
    ASSERT_EQ(mmu->rb(0xFF01), 42);
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 2);
}

TEST_F(instruction_test, TestInstr_LDH_A_N)
{
    SETUP_OPCODE(0xF0);
    SET_IMMEDIATE1(01);
    
    mmu->wb(0xFF01, 42);
    
    RUN(c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(mmu->rb(0xFF01), 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 2);
}

TEST_F(instruction_test, TestInstr_LD_BC_NN)
{
    SETUP_OPCODE(0x01);
    SET_IMMEDIATE1(0x11);
    SET_IMMEDIATE2(0xC2);
    RUN(c);
    ASSERT_EQ(c->BC, 0xC211);
    ASSERT_EQ(c->PC, baseAddress + 3);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, TestInstr_LD_DE_NN)
{
    SETUP_OPCODE(0x11);
    SET_IMMEDIATE1(0x11);
    SET_IMMEDIATE2(0xC2);
    RUN(c);
    ASSERT_EQ(c->DE, 0xC211);
    ASSERT_EQ(c->PC, baseAddress + 3);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, TestInstr_LD_HL_NN)
{
    SETUP_OPCODE(0x21);
    SET_IMMEDIATE1(0x11);
    SET_IMMEDIATE2(0xC2);
    RUN(c);
    ASSERT_EQ(c->HL, 0xC211);
    ASSERT_EQ(c->PC, baseAddress + 3);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, TestInstr_LD_SP_NN)
{
    SETUP_OPCODE(0x31);
    SET_IMMEDIATE1(0x11);
    SET_IMMEDIATE2(0xC2);
    RUN(c);
    ASSERT_EQ(c->SP, 0xC211);
    ASSERT_EQ(c->PC, baseAddress + 3);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, TestInstr_LD_SP_HL)
{
    SETUP_OPCODE(0xF9);
    c->HL = 0xC211;
    c->SP = 0;
    RUN(c);
    ASSERT_EQ(c->SP, 0xC211);
    ASSERT_EQ(c->HL, 0xC211);
    ASSERT_EQ(c->PC, baseAddress + 1);
    ASSERT_EQ(c->FLAG, 0);
}

TEST_F(instruction_test, TestInstr_LD_NN_TO_A)
{
    SETUP_OPCODE(0xFA);
    SET_IMMEDIATE1(0x11);
    SET_IMMEDIATE2(0xC2);
    
    mmu->wb(0xC211, 42);
    
    ASSERT_EQ(mmu->rw(c->PC + 1), 0xC211);
    ASSERT_EQ(mmu->rb(0xC211), 42);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 3);
}

TEST_F(instruction_test, TestInstr_LD_Immediate_TO_A)
{
    zero_registers();
    
    c->PC = baseAddress;
    mmu->wb(c->PC, 0x3E);
    mmu->wb(c->PC + 1, 42);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->A, 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 2);
}

TEST_F(instruction_test, TestInstr_LD_H_TO_HL)
{
    zero_registers();
    /* Tests H -> (HL) (opcode:0x74) */
    c->PC = baseAddress;
    c->FLAG = 0;
    mmu->wb(c->PC, 0x74);
    mmu->wb(0xC211, 0);
    c->HL = 0xC211;
    
    ASSERT_EQ(mmu->rb(0xC211), 0);
    ASSERT_EQ(mmu->rb(c->PC), 0x74);
    
    cpu->execute(*c);
    
    ASSERT_EQ(mmu->rb(0xC211), c->H);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
}


TEST_F(instruction_test, TestInstr_LD_L_TO_HL)
{
    zero_registers();
    /* Tests L -> (HL) (opcode:0x75) */
    c->PC = baseAddress;
    c->FLAG = 0;
    mmu->wb(c->PC, 0x75);
    mmu->wb(0xC211, 0);
    c->HL = 0xC211;
    
    ASSERT_EQ(mmu->rb(0xC211), 0);
    ASSERT_EQ(mmu->rb(c->PC), 0x75);
    
    cpu->execute(*c);
    
    ASSERT_EQ(mmu->rb(0xC211), c->L);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 1);
}

TEST_F(instruction_test, TestInstr_LD_n_TO_HL)
{
    zero_registers();
    /* Tests n -> (HL) (opcode:0x36) */
    c->PC = baseAddress;
    c->FLAG = 0;
    mmu->wb(c->PC, 0x36);
    mmu->wb(c->PC + 1, 42);
    
    mmu->wb(0xC211, 0);
    c->HL = 0xC211;
    
    ASSERT_EQ(mmu->rb(0xC211), 0);
    ASSERT_EQ(mmu->rb(c->PC), 0x36);
    
    cpu->execute(*c);
    
    ASSERT_EQ(mmu->rb(0xC211), 42);
    ASSERT_EQ(c->FLAG, 0);
    ASSERT_EQ(c->PC, baseAddress + 2);
}

TEST_F(instruction_test, TestInstr_INC)
{
    // Teste INC A
    test_inc_rx(c->A, 0x3C);
    // Teste INC B
    test_inc_rx(c->B, 0x04);
    // Teste INC C
    test_inc_rx(c->C, 0x0C);
    // Teste INC D
    test_inc_rx(c->D, 0x14);
    
    // Teste INC BC
    test_inc_rxx(c->BC, 0x03);
    // Teste INC DE
    test_inc_rxx(c->DE, 0x13);
    // Teste INC HL
    test_inc_rxx(c->HL, 0x23);
    // Teste INC SP
    test_inc_rxx(c->SP, 0x33);
}

/**
 * TODO: HL, HL Testen
 */
TEST_F(instruction_test, TestInstr_ADD)
{
    // Teste ADD HL, BC
    test_add_rxx(c->HL, c->BC, 0x09);
    // Teste ADD HL, DE
    test_add_rxx(c->HL, c->DE, 0x19);
    // Teste ADD HL, HL
    //test_add_rxx(c->HL, c->HL, 0x29);
    // Teste ADD HL, SP
    test_add_rxx(c->HL, c->SP, 0x39);
}

/**
 If the contents of the Program Counter are 3535H, the contents of the Stack
 Pointer are 2000H, the contents of memory location 2000H are B5H, and
 the contents of memory location of memory location 2001H are 18H. At
 execution of RET the contents of the Stack Pointer is 2002H, and the
 contents of the Program Counter is 18B5H, pointing to the address of the
 next program Op Code to be fetched.
 Quelle: http://www.phy.davidson.edu/FacHome/dmb/py310/Z80.Instruction%20set.pdf
 **/
TEST_F(instruction_test, TestInstr_RET)
{
    c->FLAG = 0;
    c->SP = 0xD000;
    mmu->wb(0xD000, 0xB5);
    mmu->wb(0xD001, 0x18);
    
    ASSERT_EQ(mmu->rb(0xD000), 0xB5);
    ASSERT_EQ(mmu->rb(0xD001), 0x18);
    ASSERT_EQ(c->SP, 0xD000);
    
    c->PC = baseAddress;
    mmu->wb(baseAddress, 0xC9); //RET
    
    ASSERT_EQ(c->PC, baseAddress);
    ASSERT_EQ(mmu->rb(baseAddress), 0xC9);
    
    cpu->execute(*c);
    
    ASSERT_EQ(c->SP, 0xD002);
    ASSERT_EQ(c->PC, 0x18B5);
}
#endif
