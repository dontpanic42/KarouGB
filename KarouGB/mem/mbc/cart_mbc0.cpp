#include "cart_mbc0.h"
#include <cassert>

namespace emu
{
    namespace kmbc_impl
    {
        const std::string TAG("MBC0");
        
        KMBC0::KMBC0(const std::shared_ptr<memory_t> & memory,
                     const std::shared_ptr<cart_t> & cartridge)
        : KMBC(memory, cartridge)
        {
            
        }
        
        void KMBC0::setup()
        {
            lg::debug(TAG, "Setting up MBC0\n");
            
            assert(cart()->size());
            
            u16i len = cart()->size();
            for(u16i i = 0; i < len; i++)
            {
                mem()->wb(i, (*cart())[i]);
            }
            
            mem()->intercept(0, len, KMemory::WRITER_READ_ONLY);
        }
    }
}