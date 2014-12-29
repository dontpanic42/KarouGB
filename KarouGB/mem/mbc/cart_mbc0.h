#ifndef CART_MBC0_H
#define CART_MBC0_H

#include "mem_mbc.h"

namespace kmbc_impl
{
    /* Stub-Implementierung für Rom-Only carts */
    class KMBC0 : public KMBC
    {
    public:
        KMBC0(const std::shared_ptr<memory_t> & memory,
              const std::shared_ptr<cart_t> & cartridge);
        
        virtual void setup();
    };
}

#endif