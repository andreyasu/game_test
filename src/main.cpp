#include <Core/Rng.hpp>
#include <Core/Simulation.hpp>
#include <Features/Crow.hpp>
#include <Features/Healer.hpp>
#include <Features/Hunter.hpp>
#include <Features/Mine.hpp>
#include <Features/Swordsman.hpp>
#include <Features/Tower.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnCrow.hpp>
#include <IO/Commands/SpawnHealer.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnMine.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Commands/SpawnTower.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <cstdint>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>

namespace
{

	/// Optional "--seed N" makes a run reproducible for debugging; without it the
	/// simulation is non-deterministic, as the task allows.
	struct CommandLine
	{
		std::string scenarioPath;
		std::optional<uint32_t> seed;
	};

	CommandLine parseCommandLine(int argc, char** argv)
	{
		CommandLine result;
		for (int i = 1; i < argc; ++i)
		{
			const std::string arg = argv[i];
			if (arg == "--seed" && i + 1 < argc)
			{
				result.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
			}
			else
			{
				result.scenarioPath = arg;
			}
		}

		if (result.scenarioPath.empty())
		{
			throw std::runtime_error("Error: No file specified in command line argument");
		}
		return result;
	}

}  // namespace

int main(int argc, char** argv)
{
	using namespace sw;

	const CommandLine cli = parseCommandLine(argc, argv);

	std::ifstream file(cli.scenarioPath);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + cli.scenarioPath);
	}

	EventLog eventLog;
	Simulation simulation(eventLog, cli.seed ? Rng{*cli.seed} : Rng{});

	// Registering a new unit is exactly this: one line mapping its SPAWN command to
	// its constructor. Core is never touched.
	io::CommandParser parser;
	parser.add<io::CreateMap>([&](auto cmd) { simulation.createMap(cmd.width, cmd.height); })
		.add<io::SpawnSwordsman>(
			[&](auto cmd) {
				simulation.addUnit(
					std::make_shared<Swordsman>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.strength));
			})
		.add<io::SpawnHunter>(
			[&](auto cmd)
			{
				simulation.addUnit(std::make_shared<Hunter>(
					cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.agility, cmd.strength, cmd.range));
			})
		.add<io::SpawnTower>(
			[&](auto cmd)
			{ simulation.addUnit(std::make_shared<Tower>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.power)); })
		.add<io::SpawnHealer>(
			[&](auto cmd)
			{ simulation.addUnit(std::make_shared<Healer>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.spirit)); })
		.add<io::SpawnCrow>(
			[&](auto cmd)
			{ simulation.addUnit(std::make_shared<Crow>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.agility)); })
		.add<io::SpawnMine>(
			[&](auto cmd)
			{ simulation.addUnit(std::make_shared<Mine>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.power)); })
		.add<io::March>([&](auto cmd) { simulation.setMarch(cmd.unitId, cmd.targetX, cmd.targetY); });

	parser.parse(file);
	simulation.run();

	return 0;
}
