#include <Core/Game.hpp>
#include <Features/Hunter.hpp>
#include <Features/Swordsman.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	EventLog eventLog;
	Game game(eventLog);

	io::CommandParser parser;
	parser.add<io::CreateMap>([&](auto cmd) { game.createMap(cmd.width, cmd.height); })
		.add<io::SpawnSwordsman>(
			[&](auto cmd)
			{ game.addUnit(std::make_shared<Swordsman>(cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.strength)); })
		.add<io::SpawnHunter>(
			[&](auto cmd)
			{
				game.addUnit(std::make_shared<Hunter>(
					cmd.unitId, Position{cmd.x, cmd.y}, cmd.hp, cmd.agility, cmd.strength, cmd.range));
			})
		.add<io::March>([&](auto cmd) { game.setMarch(cmd.unitId, cmd.targetX, cmd.targetY); });

	parser.parse(file);
	game.run();

	return 0;
}
