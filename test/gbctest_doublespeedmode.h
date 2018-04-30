//
//  instrtest.h
//  KarouGB
//
//  Created by Daniel on 08.04.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __cgbtest_doublespeedmode_h
#define __cgbtest_doublespeedmode_h

#include <gtest/gtest.h>
#include <memory>
#include "../src/mem/cart_loader.h"
#include "../src/cpu/cpu.h"

using namespace emu;
using namespace cpu;

#define SETUP_OPCODE(x) zero_registers();\
                        c->PC = baseAddress;\
                        mmu->wb(baseAddress, x)
#define SET_IMMEDIATE1(x)  mmu->wb(c->PC + 1, x)
#define SET_IMMEDIATE2(x)  mmu->wb(c->PC + 2, x)
#define RUN(x) cpu->tick(*x)

class CGBTestDoubleSpeedMode : public testing::Test
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
		// Load a random cartridge and force cgb mode
		loader = KCartridgeLoader::load("testroms/cpu_instrs.gb", true);
		ASSERT_TRUE(loader != nullptr);
		mmu = loader->getMemory();
		ASSERT_TRUE(mmu != nullptr);

		cpu = std::make_shared<Z80>(mmu);
		c = std::move(std::unique_ptr<Context>(new Context));

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
};

// Test that cgb mode is enabled
TEST_F(CGBTestDoubleSpeedMode, TestCGBModeEnabled)
{
	ASSERT_TRUE(mmu->isCGB());
	ASSERT_TRUE(mmu->inCGBMode());
}

// Test that cgb mode is enabled
TEST_F(CGBTestDoubleSpeedMode, DoubleSpeedModeOffByDefault)
{
	ASSERT_FALSE(c->double_speed_mode);
}

TEST_F(CGBTestDoubleSpeedMode, TestPrepareNotSet)
{
	c->double_speed_mode = false;
	// Current: Prepare not set 1x speed mode
	u08i reg = 0;
	// Load the key1 register
	mmu->wb(0xFF4D, reg);
	// Write the 'STOP' instruction at the PC so it gets executed next
	mmu->wb(baseAddress, 0x10);

	SETUP_OPCODE(0x10);
	RUN(c);

	u08i newReg = mmu->rb(0xFF4D);

	// We should still be in 1x speed mode
	ASSERT_FALSE(newReg & BIT_7);
	ASSERT_FALSE(c->double_speed_mode);

	// The prepare bit should not be set
	ASSERT_FALSE(newReg & BIT_0);
}

TEST_F(CGBTestDoubleSpeedMode, TestSwitchTo2xMode)
{
	c->double_speed_mode = false;
	// Current: Prepare bit set, 1x speed mode
	u08i reg = BIT_0;

	// Load the key1 register
	mmu->wb(0xFF4D, reg);
	// Write the 'STOP' instruction at the PC so it gets executed next
	mmu->wb(baseAddress, 0x10);

	SETUP_OPCODE(0x10);
	RUN(c);

	u08i newReg = mmu->rb(0xFF4D);

	// We should be in 2x speed mode
	ASSERT_TRUE(newReg & BIT_7);
	ASSERT_TRUE(c->double_speed_mode);

	// The prepare bit should have been reset
	ASSERT_FALSE(newReg & BIT_0);
}

TEST_F(CGBTestDoubleSpeedMode, TestSwitchTo1xMode)
{
	c->double_speed_mode = true;
	// Current: Prepare bit set, 2x speed mode
	u08i reg = BIT_0 | BIT_7;

	// Load the key1 register
	mmu->wb(0xFF4D, reg);
	// Write the 'STOP' instruction at the PC so it gets executed next
	mmu->wb(baseAddress, 0x10);

	SETUP_OPCODE(0x10);
	RUN(c);

	u08i newReg = mmu->rb(0xFF4D);

	// We should be in 2x speed mode
	ASSERT_FALSE(newReg & BIT_7);
	ASSERT_FALSE(c->double_speed_mode);

	// The prepare bit should have been reset
	ASSERT_FALSE(newReg & BIT_0);
}


#endif //defined(__cgbtest_doublespeedmode_h)
