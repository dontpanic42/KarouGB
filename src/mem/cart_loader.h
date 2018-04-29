#ifndef mem_factory_h
#define mem_factory_h

#include <memory>

#include "mem.h"
#include "cartridge.h"
#include "cart_mbc.h"

namespace emu
{
    class KCartridgeLoader
    {
    public:
        typedef KMemory memory_t;
        typedef KCartridge cart_t;
    private:
        const std::string cartname;
        std::shared_ptr<memory_t> memory;
        const std::shared_ptr<cart_t> cart;
        
        std::shared_ptr<KMBC> mbc;
        
        void setupMBC();
        
        KCartridgeLoader(const std::string & cartname, const bool forceCGBMode = false);
    public:
        static std::shared_ptr<KCartridgeLoader> load(const std::string & cartname, const bool forceCGBMode = false);
        
        const std::string & getCartridgeName() const;
        const std::shared_ptr<memory_t> & getMemory() const;
        const std::shared_ptr<cart_t> & getCartridge() const;
        const std::shared_ptr<KMBC> & getMBC() const;
        
        std::string createAutosaveFilename();
        bool canSaveState();
        void loadState(const std::string & filename);
        void saveState(const std::string & filename);
    };
}

#endif