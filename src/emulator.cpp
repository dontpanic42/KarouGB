#include "emulator.h"

namespace emu {
	Emulator::Emulator(std::shared_ptr<IOProvider> io_provider, std::string cart_file_name)
		: io_provider(io_provider)
		, cart_file_name(cart_file_name)
	{
	}

	bool Emulator::initialize() 
	{
		try
		{
			loader = KCartridgeLoader::load(cart_file_name);
			mmu = loader->getMemory();

			cpu = std::move(std::make_shared<cpu::Z80>(mmu));
			cpu_context = std::move(std::make_unique<cpu::Context>());
			gpu = std::move(std::make_unique<GPU>(mmu, io_provider, cpu, mmu->isCGB(), mmu->inCGBMode()));
			buttons = std::move(std::make_unique<Buttons>(mmu, io_provider, cpu));
			timer = std::move(std::make_unique<Timer>(mmu, cpu));
			debugger = std::move(std::make_unique<Debugger>(cpu, mmu, *cpu_context));

			io_provider->init(std::string());
		}
		catch (std::exception & exception)
		{
			io_provider->handleError(exception);
			return false;
		}

		return true;
	}

	void Emulator::tick() 
	{
		if (io_provider->isClosed())
		{
			return;
		}

		if (!debugger->poll())
		{
			return;
		}

		timer->tick(*cpu_context);
		cpu->execute(*cpu_context);
		gpu->step(*cpu_context);
	}
}