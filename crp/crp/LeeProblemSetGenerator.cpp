#include "LeeProblemSetGenerator.h"
#include <vector>
#include <filesystem>
#include "LeeProblemInstanceGenerator.h"
#include <sstream>
#include "StringUtils.h"
#include <fstream>

void LeeProblemSetGenerator::Generate(std::string folder)
{

	//original

	std::vector<std::tuple<int, int, int, int>> instanceCharacteristics {
		std::make_tuple(1,16,6,70),
		std::make_tuple(1,16,8,90),
		std::make_tuple(2,16,6,140),
		std::make_tuple(2,16,8,190),
		std::make_tuple(4,16,6,280),
		std::make_tuple(4,16,8,380),
		std::make_tuple(6,16,6,430),
		std::make_tuple(6,16,8,570),
		std::make_tuple(8,16,6,570),
		std::make_tuple(10,16,6,720)
	};

	//float fill = 0.9;
	//std::vector<std::tuple<int, int, int, int>> instanceCharacteristics{
	//std::make_tuple(1,16,6,fill * (1 * 16 * 6 - 6)),
	//std::make_tuple(1,16,8,fill * (1 * 16 * 8 - 8)),
	//std::make_tuple(2,16,6,fill * (2 * 16 * 6 - 6)),
	//std::make_tuple(2,16,8,fill * (2 * 16 * 8 - 8)),
	//std::make_tuple(4,16,6,fill * (4 * 16 * 6 - 6)),
	//std::make_tuple(4,16,8,fill * (4 * 16 * 8 - 8)),
	//std::make_tuple(6,16,6,fill * (6 * 16 * 6 - 6)),
	//std::make_tuple(6,16,8,fill * (6 * 16 * 8 - 8)),
	//std::make_tuple(8,16,6,fill * (8 * 16 * 6 - 6)),
	//std::make_tuple(10,16,6,fill * (10 * 16 * 6 - 6))
	//};

	// for different bay structures

	//std::vector<std::tuple<int, int, int, int>> instanceCharacteristics{
	//std::make_tuple(1,10,8,(int)(fill*(1*10*8-8))),
	//std::make_tuple(2,10,8,(int)(fill * (2 * 10 * 8 - 8))),
	//std::make_tuple(5,10,8,(int)(fill * (5 * 10 * 8 - 8))),
	//std::make_tuple(7,10,8,(int)(fill * (7 * 10 * 8 - 8))),
	//std::make_tuple(10,10,8,(int)(fill * (10 * 10 * 8 - 8))),
	//std::make_tuple(10,1,8,(int)(fill * (1 * 10 * 8 - 8))),
	//std::make_tuple(10,2,8,(int)(fill * (2 * 10 * 8 - 8))),
	//std::make_tuple(10,5,8,(int)(fill * (5 * 10 * 8 - 8))),
	//std::make_tuple(10,7,8,(int)(fill * (7 * 10 * 8 - 8))),

	//	std::make_tuple(1,10,6,(int)(fill * (1 * 10 * 6 - 6))),
	//std::make_tuple(2,10,6,(int)(fill * (2 * 10 * 6 - 6))),
	//std::make_tuple(5,10,6,(int)(fill * (5 * 10 * 6 - 6))),
	//std::make_tuple(7,10,6,(int)(fill * (7 * 10 * 6 - 6))),
	//std::make_tuple(10,10,6,(int)(fill * (10 * 10 * 6 - 6))),
	//std::make_tuple(10,1,6,(int)(fill * (1 * 10 * 6 - 6))),
	//std::make_tuple(10,2,6,(int)(fill * (2 * 10 * 6 - 6))),
	//std::make_tuple(10,5,6,(int)(fill * (5 * 10 * 6 - 6))),
	//std::make_tuple(10,7,6,(int)(fill * (7 * 10 * 6 - 6))),
	//};

	// for different bay load ratios

	
	//int instancesPerParameter = 5;
	int instancesPerParameter = 5;

	std::filesystem::create_directories(folder);


		for (auto inst : instanceCharacteristics)
		{

			int bays = std::get<0>(inst);
			int stacks = std::get<1>(inst);
			int height = std::get<2>(inst);
			int containers = std::get<3>(inst);

			//std::string folder = "lee" + to_string(bays) + "_" + to_string(stacks);

			//std::filesystem::create_directories(folder);

			
			LeeProblemInstanceGenerator generator(bays, stacks, height, containers);
			for (int instance = 0; instance < instancesPerParameter; instance++)
			{
				std::string	instanceString = generator.GenerateInstance();
				std::stringstream fileName;
				std::string bayString = StringUtils::zero_prefix_string(bays,2);
				std::string stackString = StringUtils::zero_prefix_string(stacks, 2);
				std::string heigthString = StringUtils::zero_prefix_string(height, 2);
				std::string containerString = StringUtils::zero_prefix_string(height, 4);
				std::string instanceNumberString = StringUtils::zero_prefix_string(instance, 3);

				std::string instanceFileName = "R" + bayString + stackString + heigthString + "_" + containerString + "_" + instanceNumberString;
				fileName << folder << '\\' <<  instanceFileName << ".txt";
				
				std::ofstream out(fileName.str());

				out << fileName.str() << " " << bays << " " << stacks << " " << height << " " << containers << " " << containers<<endl;
				out << instanceString;
				out.close();
			}
			
		}

}
