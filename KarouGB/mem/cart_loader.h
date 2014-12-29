#ifndef mem_factory_h
#define mem_factory_h

#include <memory>

#include "mem.h"
#include "cartridge.h"
#include "mem_mbc.h"
#include "cart_mbc1.h"

class KCartridgeLoader
{
public:
    typedef KMemory memory_t;
    typedef KCartridge cart_t;
private:
    const std::string cartname;
    const std::shared_ptr<memory_t> memory;
    const std::shared_ptr<cart_t> cart;
    
    std::shared_ptr<KMBC> mbc;
    
    void setupMBC()
    {
        using namespace kmbc_impl;
        
        mbc = std::make_shared<KMBC1>(getMemory(), getCartridge());
        mbc->setup();
    }
    
    KCartridgeLoader(const std::string & cartname)
    : cartname(cartname)
    , memory(new memory_t())
    , cart(new cart_t())
    , mbc(nullptr)
    {
        cart->load(cartname);
        setupMBC();
    }
public:
    static std::shared_ptr<KCartridgeLoader> load(const std::string & cartname)
    {
        std::shared_ptr<KCartridgeLoader> loader(new KCartridgeLoader(cartname));
        return loader;
    }
    
    const std::string & getCartridgeName() const
    {
        return cartname;
    }
    
    const std::shared_ptr<memory_t> & getMemory() const
    {
        return memory;
    }
    
    const std::shared_ptr<cart_t> & getCartridge() const
    {
        return cart;
    }
    
    const std::shared_ptr<KMBC> & getMBC() const
    {
        return mbc;
    }
};


#endif