#include "cart_loader.h"
#include "mbc/cart_mbc0.h"
#include "mbc/cart_mbc1.h"
#include "mbc/cart_mbc3.h"
#include <fstream>
#include <cassert>

namespace emu
{
    const std::string TAG("ctld");
    
    std::shared_ptr<KCartridgeLoader> KCartridgeLoader::load(const std::string & cartname)
    {
        std::shared_ptr<KCartridgeLoader> loader(new KCartridgeLoader(cartname));
        return loader;
    }
    
    KCartridgeLoader::KCartridgeLoader(const std::string & cartname)
    : cartname(cartname)
//    , memory(new memory_t())
    , cart(new cart_t())
    , mbc(nullptr)
    {
        
        cart->load(cartname);
        assert(cart != nullptr);
        
        bool cgb = (cart->header().cgb_flag == 0x80) ||
                   (cart->header().cgb_flag == 0xC0);
        
        lg::info(TAG, "CGB/CGBMODE Enabled: %s\n", (cgb)? "Yes." : "No.");
        
        memory = std::make_shared<memory_t>(cgb, cgb);
        
        setupMBC();
    }
    
    void KCartridgeLoader::setupMBC()
    {
        using namespace kmbc_impl;
        
        switch(getCartridge()->header().cart_type)
        {
            case KCartridge::ROM:
            {
                mbc = std::make_shared<KMBC0>(getMemory(), getCartridge());
                break;
            }
            case KCartridge::ROM_MBC1:
            case KCartridge::ROM_MBC1_RAM:
            case KCartridge::ROM_MBC1_RAM_BATT:
            {
                mbc = std::make_shared<KMBC1>(getMemory(), getCartridge());
                break;
            }
            case KCartridge::ROM_MBC3:
            case KCartridge::ROM_MBC3_RAM:
            case KCartridge::ROM_MBC3_TIM_BAT:
            case KCartridge::ROM_MBC3_TIM_BAT_RAM:
            case KCartridge::ROM_MBC3_RAM_BAT:
            {
                mbc = std::make_shared<KMBC3>(getMemory(), getCartridge());
                break;
            }
            default:
            {
                lg::error(TAG, "Unsupported cartridge type: %u\n", getCartridge()->header().cart_type);
                throw std::runtime_error("Unsupported MBC-Type." + std::to_string(getCartridge()->header().cart_type));
            }
        }
        
        mbc->setup();
    }
    
    const std::string & KCartridgeLoader::getCartridgeName() const
    {
        return cartname;
    }
    
    const std::shared_ptr<KCartridgeLoader::memory_t> & KCartridgeLoader::getMemory() const
    {
        return memory;
    }
    
    const std::shared_ptr<KCartridgeLoader::cart_t> & KCartridgeLoader::getCartridge() const
    {
        return cart;
    }
    
    const std::shared_ptr<KMBC> & KCartridgeLoader::getMBC() const
    {
        return mbc;
    }
    
    bool KCartridgeLoader::canSaveState()
    {
        return (mbc && mbc->canSaveState());
    }
    
    void KCartridgeLoader::loadState(const std::string & filename)
    {
        if(!canSaveState())
        {
            lg::info(TAG, "MBC does not support state loading.\n");
            return;
        }
        
        std::ifstream istream;
        istream.open(filename);
        
        if(!istream.is_open())
        {
            lg::warn(TAG, "Cannot open %s for state loading.\n", filename.c_str());
            return;
        }
        
        lg::debug(TAG, "Trying to load state from %s\n", filename.c_str());
        
        mbc->loadState(istream);
        
        istream.close();
        
        lg::info(TAG, "Loaded state from %s\n", filename.c_str());
    }
    
    std::string KCartridgeLoader::createAutosaveFilename()
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%c %Z");
        return getCartridgeName() + "_autosave_" + ss.str() + ".sav";
    }
    
    void KCartridgeLoader::saveState(const std::string & filename)
    {
        if(!canSaveState())
        {
            lg::info(TAG, "MBC does not support state loading.\n");
            return;
        }
        
        std::ofstream ostream;
        ostream.open(filename);
        
        if(!ostream.is_open())
        {
            lg::warn(TAG, "Cannot open %s for state saving.\n", filename.c_str());
            return;
        }
        
        mbc->saveState(ostream);
        
        ostream.close();
        
        lg::info(TAG, "Saved state to %s\n", filename.c_str());
    }
}