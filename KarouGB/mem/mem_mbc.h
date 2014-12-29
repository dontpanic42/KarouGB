#ifndef mem_mbc_h
#define mem_mbc_h

#include "mem.h"
#include "cartridge.h"

class KMBC
{
public:
    typedef KMemory memory_t;
    typedef KCartridge cart_t;
private:
    const std::shared_ptr<memory_t> memory;
    const std::shared_ptr<cart_t> cartridge;
protected:
    const std::shared_ptr<memory_t> & mem() const
    {
        return memory;
    }
    
    const std::shared_ptr<cart_t> & cart() const
    {
        return cartridge;
    }
public:
    KMBC(const std::shared_ptr<memory_t> & memory,
         const std::shared_ptr<cart_t> & cartridge)
    : memory(memory)
    , cartridge(cartridge)
    {
    }
    
    virtual ~KMBC()
    {
    }
    
    virtual void saveState(std::ostream & stream)
    {
    };
    
    virtual void loadState(std::istream & stream)
    {
    };
    
    virtual bool canSaveState()
    {
        return false;
    }
    
    virtual void setup() = 0;
};

#endif