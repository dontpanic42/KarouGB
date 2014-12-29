#include "cart_mbc0.h"

namespace kmbc_impl
{
    
    KMBC0::KMBC0(const std::shared_ptr<memory_t> & memory,
                 const std::shared_ptr<cart_t> & cartridge)
    : KMBC(memory, cartridge)
    {
        
    }
    
    void KMBC0::setup()
    {
    }
}