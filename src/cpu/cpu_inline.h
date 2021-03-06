//
//  cpu_inline.h
//  mygb
//
//  Created by Daniel on 21.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_cpu_inline_h
#define mygb_cpu_inline_h
namespace opfuncs
{
    template<typename T>
    inline void push(Context & c, KMemory & mmu, T value)
    {
        throw std::runtime_error("Not implemented");
    }
    
    /* Schreibt einen 8-Bit Wert in den Stack und dekrementiert den SP */
    template<>
    inline void push<u08i>(Context & c, KMemory & mmu, u08i value)
    {
        mmu.wb(--c.SP, value);
    }
    
    /* Schreibt einen 16-Bit Wert in den Stack und dekrementiert den SP */
    template<>
    inline void push<u16i>(Context & c, KMemory & mmu, u16i value)
    {
        /* High byte */
        mmu.wb(--c.SP, (value >> 8) & 0xFF);
        /* Low byte */
        mmu.wb(--c.SP, (value & 0xFF));
    }
    
    template<typename T>
    inline T pop(Context & c, KMemory & mmu)
    {
        throw std::runtime_error("Not implemented");
    }
    
    /* Ließt einen 8-Bit Wert vom Stack und inkrementiert den SP */
    template<>
    inline u08i pop<u08i>(Context & c, KMemory & mmu)
    {
        return mmu.rb(c.SP++);
    }
    
    /* Ließt einen 16-Bit Wert vom Stack und inkrementiert den SP */
    template<>
    inline u16i pop<u16i>(Context & c, KMemory & mmu)
    {
        /* Low byte */
        u16i lo = static_cast<u16i>(mmu.rb(c.SP++));
        /* High byte */
        u16i hi = static_cast<u16i>(mmu.rb(c.SP++)) << 8;
        return lo | hi;
    }
    
    inline u08i add(Context & c, u08i a, u08i b)
    {
        int result = a + b;
        int carry = a ^ b ^ result;
        flag::clear_all(c);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::CARRY, (carry & 0x100) != 0);
        flag::set_if(c, flag::HALFCARRY, (carry & 0x10) != 0);
        return static_cast<u08i>(result);
    }
    
    //Equivalent zu add(a, b=1) ohne carry flag beeinträchtigung
    inline u08i inc(Context & c, u08i a)
    {
        int result = a + 1;
        int carry = a ^ 1 ^ result;
        flag::clear(c, flag::SUBTRACT);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::HALFCARRY, (carry & 0x10) != 0);
        return static_cast<u08i>(result);
    }
    
    inline u08i add_c(Context & c, u08i a, u08i b)
    {
        int carry = (flag::is_set(c, flag::CARRY))? 1 : 0;
        int result = a + b + carry;
        flag::clear_all(c);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::CARRY, result > 0xFF);
        flag::set_if(c, flag::HALFCARRY, ((a & 0x0F) + (b & 0x0F) + carry) > 0x0F);
        return static_cast<u08i>(result);
    }
    
    inline u16i add16(Context & c, u16i a, u16i b)
    {
        int result = a + b;
        flag::set_if(c, flag::CARRY, result & 0x10000);
        flag::set_if(c, flag::HALFCARRY, (a ^ b ^ (result & 0xFFFF)) & 0x1000);
        flag::clear(c, flag::SUBTRACT);
        
        return static_cast<u16i>(result);
    }
    
    inline u08i sub(Context & c, u08i a, u08i b)
    {
        int result = a - b;
        int carry = a ^ b ^ result;
        flag::set(c, flag::SUBTRACT);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::CARRY, (carry & 0x100) != 0);
        flag::set_if(c, flag::HALFCARRY, (carry & 0x10) != 0);
        return static_cast<u08i>(result);
    }
    
    //Equivalent zu add(a, b=1) ohne carry flag beeinträchtigung
    inline u08i dec(Context & c, u08i a)
    {
        int result = a - 1;
        int carry = a ^ 1 ^ result;
        flag::set(c, flag::SUBTRACT);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::HALFCARRY, (carry & 0x10) != 0);
        return static_cast<u08i>(result);
    }
    
    inline u08i sub_c(Context & c, u08i a, u08i b)
    {
        int carry = (flag::is_set(c, flag::CARRY))? 1 : 0;
        int result = a - b - carry;
        flag::set(c, flag::SUBTRACT);
        flag::set_if(c, flag::ZERO, static_cast<u08i>(result) == 0);
        flag::set_if(c, flag::CARRY, result < 0);
        flag::set_if(c, flag::HALFCARRY, ((a & 0x0F) - (b & 0x0F) - carry) < 0);
        
        return static_cast<u08i>(result);
    }
    
    inline u08i rlca(Context & c, u08i a)
    {
        u08i result = a;
        result <<= 1;
        result |= (a & BIT_7)? BIT_0 : 0;
        
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, (a & BIT_7));
        
        return result;
    }
    
    inline u08i rla(Context & c, u08i a)
    {
        u08i result = a;
        result <<= 1;
        result |= (flag::is_set(c, flag::CARRY))? BIT_0 : 0;
        
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, a & BIT_7);
        
        return result;
    }
    
    inline u08i rrca(Context & c, u08i a)
    {
        u08i result = a;
        result >>= 1;
        result |= (a & BIT_0)? BIT_7 : 0;
        
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, a & BIT_0);
        
        return result;
    }
    
    inline u08i rra(Context & c, u08i a)
    {
        u08i result = a;
        result >>= 1;
        result |= (flag::is_set(c, flag::CARRY))? BIT_7 : 0;
        
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, a & BIT_0);
        
        return result;
    }
    
    inline u08i sla(Context & c, u08i a)
    {
        u08i result = a;
        result <<= 1;
        
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, a & BIT_7);
        flag::set_if(c, flag::ZERO, result == 0);

        return result;
    }
    
    inline u08i srl(Context & c, u08i a)
    {
        flag::clear_all(c);
        flag::set_if(c, flag::CARRY, a & 0x01);
        a >>= 1;
        flag::set_if(c, flag::ZERO, a == 0);
        return a;
    }
    
    inline u08i sra(Context & c, u08i a)
    {
        u08i result = a;
        result >>= 1;
        result |= (a & BIT_7)? BIT_7 : 0;
        
        flag::clear_all(c);
        flag::set_if(c, flag::ZERO, result == 0);
        flag::set_if(c, flag::CARRY, a & BIT_0);
        
        return result;
    }
    
    //Undocumented, see http://www.myquest.nl/z80undocumented/z80-documented-v0.91.pdf
    inline u08i sll(Context & c, u08i a)
    {
        flag::set_if(c, flag::CARRY , a & 0x80);
        flag::clear(c, flag::HALFCARRY);
        flag::clear(c, flag::SUBTRACT);
        return (a << 1) | 1;
    }
    
    inline void ret_if(Context & c, KMemory & mmu, bool condition)
    {
        if(condition)
        {
            c.PC = pop<u16i>(c, mmu);
            c.branch_taken = true;
        }
    }
    
    inline void call_if(Context & c, KMemory & mmu, bool condition)
    {
        
        if(condition)
        {
            push<u16i>(c, mmu, c.PC + 2);
            c.PC = mmu.rw(c.PC);
            c.branch_taken = true;
        }
        else
        {
            c.PC += 2;
        }
    }
    
    inline void jmp_if(Context & c, KMemory & mmu, bool condition)
    {
        if(condition)
        {
            c.PC = mmu.rw(c.PC);
            c.branch_taken = true;
        }
        else
        {
            c.PC += 2;
        }
    }
    
    inline void jr_if(Context & c, KMemory & mmu, bool condition)
    {
        if(condition)
        {
            c.PC += static_cast<s08i>(mmu.rb(c.PC));
            c.branch_taken = true;
        }
        
        c.PC++;
    }

}


#endif
