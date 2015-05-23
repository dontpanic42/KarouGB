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
    
    /* Multi-Tests */
    void test_inc_rxx(u16i & reg, u08i opcode)
    {
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
