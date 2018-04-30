#ifndef __emulator_h
#define __emulator_h

#include <string>
#include <memory>
#include "io/io_provider.h"
#include "cpu/cpu.h"
#include "gpu.h"
#include "buttons.h"
#include "timer.h"
#include "mem/cart_loader.h"
#include "mem/mem.h"

namespace emu {
	class Emulator {
	private:
		std::shared_ptr<IOProvider> io_provider;
		std::string cart_file_name;

		std::unique_ptr<cpu::Context> cpu_context;
		std::unique_ptr<GPU> gpu;
		std::unique_ptr<Buttons> buttons;
		std::unique_ptr<Timer> timer;

		std::shared_ptr<KCartridgeLoader> loader;

	public:
		std::shared_ptr<KMemory> mmu;
		std::shared_ptr<cpu::Z80> cpu;
		std::unique_ptr<cpu::Context> cpuContext;

		Emulator(std::shared_ptr<IOProvider> io_provider, std::string cart_file_name);
		bool initialize();
		void tick();
	};
}

#endif //defined(__emulator_h)