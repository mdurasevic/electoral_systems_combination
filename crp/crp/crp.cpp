// crp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once 
#include <iostream>
#include "DataSet.h"
#include "DataSetLoader.h"
#include "ZhuDataSetLoader.h"
#include "CasertaDataSetLoader.h"
#include "LeeDataSetLoader.h"
#include "TLP.h"
#include "MinMax.h"

#include <filesystem>
#include "SolutionStorage.h"
#include "Terminals.h"
#include "Evaluator.h"
#include "RestrictedAutomaticHeuristic.h"
#include "UnrestrictedAutomaticHeuristicAllCombinations.h"
#include "ReshuffleIndex.h"

#include "CasertaProblemSetGenerator.h"
#include "ZhuProblemInstanceGenerator.h"
#include "ZhuProblemSetGenerator.h"
#include "PR1.h"
#include "PR3.h"
#include "PR4.h"
#include "PU1.h"
#include "UnrestrictedAutomaticHeuristic.h"
#include "PU2.h"
#include "UnrestrictedAutomaticHeuristicWithPrelocationRule.h"

#include <chrono>
#include <numeric>
#include <regex>

#include "AntiPlurality.h"
#include "ApprovalVoting.h"
#include "BaldwinMethod.h"
#include "BordaCount.h"
#include "BucklinVoting.h"
#include "CombinedApprovalVoting.h"
#include "CondorcetMethod.h"
#include "ContigentVoting.h"
#include "CoombsMethod.h"
#include "CopelandMethod.h"
#include "LeeProblemInstanceGenerator.h"
#include "LeeProblemSetGenerator.h"
#include "MinimumAggregationStrategy.h"
#include "OneStageEnsembleStrategy.h"
#include "ZhuDuplicateDataSetLoader.h"
#include "ZhuDuplicateProblemSetGenerator.h"
#include "SelectFirstAvailabelContainerStrategy.h"
#include "RestrictedAutomaticDuplicateHeuristic.h"
#include "RestrictedAutomaticEnsembleHeuristic.h"
#include "SumAggregationStrategy.h"
#include "UnrestrictedAutomaticDuplicateHeuristic.h"
#include "VoteAggregationStrategy.h"

#include "RestrictedGlobalRetrievalHeuristic.h"
#include "UnrestrictedGlobalRetrievalHeuristic.h"
#include "GlobalRetrievalHeuristicSettings.h"
#include "InstantRunoff.h"
#include "JudgementMethod.h"
#include "KemenyYoungMethod.h"
#include "MajorityVote.h"
#include "MiniMax.h"
#include "NansonMethod.h"
#include "OklahomaMethod.h"
#include "PluralityVote.h"
#include "PreferentialBlockVoting.h"
#include "RankedPairs.h"
#include "RolloutHeuristic.h"
#include "RolloutHeuristicUnrestricted.h"
#include "SchulzeMethod.h"
#include "ScoreVoting.h"
#include "StarVoting.h"
#include "StringUtils.h"
#include "SumMethod.h"
#include "TidemanAlternativeSmith.h"
#include "UnifiedPrimary.h"
#include "UnrestrictedAutomaticEnsembleHeuristic.h"

using namespace std::chrono;
using namespace std;

void runInstanceGeneration()
{
	//CasertaProblemSetGenerator generator;
	//generator.GenerateInstanceWeights(".\\data\\test");
	//generator.GenerateInstanceWeights(".\\data\\train");
	//generator.GenerateInstanceWeights(".\\data\\validation");

	//ZhuProblemSetGenerator generator;
	//generator.GenerateInstanceWeights(".\\zhuvalidation");
	////generator.GenerateInstanceWeights(".\\zhutest");
	//generator.GenerateInstanceWeights(".\\zhutrain");

	LeeProblemSetGenerator generator;
	//generator.Generate(".\\data\\test");
	generator.Generate(".\\descending\\train");
	generator.Generate(".\\descending\\validation");
	generator.Generate(".\\descending\\test");
}

void evaluateExpressionsFromFile()
{ 
	//DataSetLoader* loader = new CasertaDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\caserta\\clean");
	//auto ds = loader->Load("G:\\CRP\\caserta\\large");

	//DataSetLoader* loader = new ZhuDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");

	DataSetLoader* loader = new LeeDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, random");

	string line;
	ifstream myfile("expressions.txt");
	ofstream out("tested.txt");
	long long totalDuration = 0;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if(!line.empty())
			{
				shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
				RestrictedAutomaticHeuristic urh(line, selection);
				//UnrestrictedAutomaticHeuristic urh(line, selection);

				int rel = 0;
				int time = 0;
				int energy = 0;
				auto start = high_resolution_clock::now();
				for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
				{
					auto problemInstance = ds.GetProblemInstance(i);
					auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30, -1, 1.0);
					
					urh.Solve(yard);

					rel += yard.GetNumberOfMoves();
					time += yard.GetTotalExecutionTime(); 
					energy += yard.GetTotalFuelConsumed();
					//cout << yard.GetNumberOfMoves()<<endl;
				}
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<microseconds>(stop - start);

				std::cout << duration.count() << endl;
				totalDuration +=duration.count();
				std::cout << "Total: tes" << rel << endl;
				out << rel << '\t' << time << '\t' << energy << '\t' << duration.count() << endl;
			}
		}
		myfile.close();
		out.close();
		cout << "Avg duration: " << totalDuration / 30;
	}
}

void evaluateRolloutWithExpressionsFromFile(int argc, char** argv)
{
	DataSetLoader* loader = new CasertaDataSetLoader();
	//auto ds = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\clean");
	auto ds = loader->Load("G:\\CRP\\caserta\\clean");

	//auto ds = loader->Load("G:\\CRP\\caserta\\large");

	//DataSetLoader* loader = new ZhuDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");

	string line;
	ifstream myfile("expressions.txt");
	ofstream out("tested.txt");

	//double rolrat = stod(argv[1]);
	double rolrat = 0.1;


	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (!line.empty())
			{
				shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
				shared_ptr<PriorityHeuristic> heuristic = make_shared<RestrictedAutomaticHeuristic>(line, selection,rolrat);
				shared_ptr<PriorityHeuristic> rl = make_shared<RolloutHeuristic>(heuristic);

				RolloutHeuristic rh1(rl);
				//RestrictedAutomaticHeuristic urh(line, selection);
				//UnrestrictedAutomaticHeuristic urh(line, selection);
				int rel = 0;
				int time = 0;
				int energy = 0;
				auto start = high_resolution_clock::now();
				for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
				{
					auto problemInstance = ds.GetProblemInstance(i);
					auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30, -1, 1);

					rh1.Solve(yard);

					rel += yard.GetNumberOfMoves();
					time += yard.GetTotalExecutionTime();
					energy += yard.GetTotalFuelConsumed();
					//cout << yard.GetNumberOfMoves()<<endl;
				}
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<microseconds>(stop - start);

				std::cout << duration.count() << endl;
				std::cout << "Total: tes" << rel << endl;
				out << rel << '\t' << time << '\t' << energy << '\t' << duration.count() << endl;
			}
		}
		myfile.close();
		out.close();
	}
}

void generateEvaluationMatrix(int argc, char** argv)
{
	DataSetLoader* loader = new CasertaDataSetLoader();
	//auto ds = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\clean");
	auto ds = loader->Load("G:\\CRP\\singlebay\\caserta\\train");

	//auto ds = loader->Load("G:\\CRP\\caserta\\large");

	//DataSetLoader* loader = new ZhuDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");

	string line;
	ifstream myfile("expressions.txt");
	ofstream out("matrix.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (!line.empty())
			{
				shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
				RestrictedAutomaticHeuristic urh(line, selection);
				//UnrestrictedAutomaticHeuristic urh(line, selection);
				int rel = 0;
				int time = 0;
				int energy = 0;
				auto start = high_resolution_clock::now();
				for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
				{
					auto problemInstance = ds.GetProblemInstance(i);
					auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30, -1, 1);

					urh.Solve(yard);

					rel += yard.GetNumberOfMoves();
					time += yard.GetTotalExecutionTime();
					energy += yard.GetTotalFuelConsumed();
					out << yard.GetNumberOfMoves() << " ";
					//cout << yard.GetNumberOfMoves()<<endl;
				}
				out << endl;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<microseconds>(stop - start);

				std::cout << duration.count() << endl;
				std::cout << "Total: tes" << rel << endl;
			}
		}
		myfile.close();
		out.close();
	}
}


void evaluateExpressionsAsEnsemblesFromFile()
{
	DataSetLoader* loader = new CasertaDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\caserta\\clean");

	std::ifstream infile("expressions.txt");

	std::vector<std::string> allStrings;
	std::string line;
	while(std::getline(infile, line))
	{
		if(!line.empty())
		{
			allStrings.push_back(line);
		}
	}

	double best = std::numeric_limits<double>::infinity();

	std::random_device dev;
	std::mt19937 rng(dev());

	vector<int> indices(50);
	std::iota(indices.begin(), indices.end(), 0);

	for (int iter = 0; iter < 50; iter++) {

		int total = 0;
		double totalTime = 0;


		std::shuffle(indices.begin(), indices.end(), rng);

		std::vector<string> expressions;

		for(int i = 0; i<3; i++)
		{
			expressions.push_back(allStrings[indices[i]]);
		}

	
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
		{
			auto problemInstance = ds.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			yard.ShouldStoreMoves(true);

			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			
			shared_ptr<ElectoralSystem> aggregationStrategy = make_shared<PluralityVote>();
			shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(aggregationStrategy, SelectionType::MINIMUM);

			//RestrictedAutomaticEnsembleHeuristic urh(expressions, selection, eds);
			UnrestrictedAutomaticEnsembleHeuristic urh(expressions, selection, eds);
			
			urh.Solve(yard);
			total += yard.GetNumberOfMoves();
			totalTime += yard.GetTotalExecutionTime();

		}

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);

		std::cout << duration.count() << endl;
	}

}

int runMultibay()
{
	DataSetLoader* loader = new LeeDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, random");
	//auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, upside down");

	//auto t1 = std::chrono::high_resolution_clock::now();

	int total = 0;
	double totalTime = 0;

	for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
		//for (int i = 0; i < 1; i++)
		//for (int i = 0; i < 1; i++)

	{
		auto problemInstance = ds.GetProblemInstance(i);
		auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
		yard.ShouldStoreMoves(true);
		TLP tlp;
		ReshuffleIndex ri;
		MinMax mm;
		PR1 pr1;
		PR3 pr3;
		PR4 pr4;
		PU1 pu1;
		PU2 pu2;

		shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
		RestrictedAutomaticHeuristic urh("* + - * DIS RI / DUR * DIS DUR / - EMP - DIS DIFF AVG + / - SH DIFF / * EMP EMP + DUR RI / * DIFF * RI DIFF / CUR * DIS RI", selection);

		mm.Solve(yard);

		std::cout << problemInstance.GetInstanceName() << " ";
		std::cout << yard.GetNumberOfMoves() << " ";
		std::cout << yard.GetTotalExecutionTime() << endl;

		total += yard.GetNumberOfMoves();
		totalTime += yard.GetTotalExecutionTime();
	}

	std::cout << "Total moves: " << total << endl;
	std::cout << "Total time: " << totalTime;

	vector<std::shared_ptr<ContainerMove>> moves;
	std::cout << "";
	return 0;
}

int runMultibayMultiple()
{
	DataSetLoader* loader = new LeeDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, random");
	//auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, upside down");
	auto ds = loader->Load("G:\\CRP\\LeeLee\\generalisation\\struct\\lee_descending");
	//auto ds = loader->Load("G:\\CRP\\LeeLee\\test");
	//auto t1 = std::chrono::high_resolution_clock::now();
	//std::vector<ContainerMove> moves;
	//moves.push_back(ContainerMove(Location(1, 1), Location(1, 2)));
	//moves.push_back(ContainerMove(Location(1, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(0, 1)));
	//moves.push_back(ContainerMove(Location(0, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 2), Location(1, 1)));
	//moves.push_back(ContainerMove(Location(0, 0), Location(1, 1)));
	//moves.push_back(ContainerMove(Location(0, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(1, 1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 1), Location(0, 2)));
	//moves.push_back(ContainerMove(Location(0, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 1), Location(0, 2)));
	//moves.push_back(ContainerMove(Location(0, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 0), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 2), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(1, 1), Location(0, -1)));
	//moves.push_back(ContainerMove(Location(0, 2), Location(0, -1)));



	vector<string> expressionLines;
	std::ifstream file("data1.txt");
	std::string str;
	while (std::getline(file, str))
	{
		if(!str.empty())
		{
			expressionLines.push_back(str);
		}
	}

	for (auto line : expressionLines)
	{
		int total = 0;
		double totalTime = 0;
		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
			//for (int i = 0; i < 1; i++)
			//for (int i = 0; i < 1; i++)
		{
			auto problemInstance = ds.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			//yard.Simulate(moves);
			//cout<<yard.GetTotalExecutionTime();
			//return 0;
			yard.ShouldStoreMoves(true);
			TLP tlp;
			ReshuffleIndex ri;
			MinMax mm;
			PR3 pr3;
			PR4 pr4;
			PU1 pu1;
			PU2 pu2;

			tlp.Solve(yard);
			//ri.Solve(yard);

			//mm.Solve(yard);
			//pr3.Solve(yard);
			//pr4.Solve(yard);

			//pu1.Solve(yard);
			//pu2.Solve(yard);

			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			RestrictedAutomaticHeuristic urh(line, selection);
			//UnrestrictedAutomaticHeuristic urh(line, selection);

			//urh.Solve(yard);

			total += yard.GetNumberOfMoves();
			totalTime += yard.GetTotalExecutionTime();
		}

		std::cout << total << " " << totalTime<<endl;

		vector<std::shared_ptr<ContainerMove>> moves;
	}
	
	return 0;
}


int runDuplicateMultiple()
{
	DataSetLoader* loader = new ZhuDuplicateDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\zhu\\dup\\alpha=0.2");

	vector<string> expressionLines;
	std::ifstream file("data.txt");
	std::string str;
	while (std::getline(file, str))
	{
		if (!str.empty())
		{
			expressionLines.push_back(str);
		}
	}

	for (int j =0; j<expressionLines.size();j+=2)
	{
		int total = 0;
		double totalTime = 0;
		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
			//for (int i = 0; i < 1; i++)
			//for (int i = 0; i < 1; i++)
		{
			auto problemInstance = ds.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			yard.ShouldStoreMoves(true);


			//shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			//RestrictedAutomaticHeuristic urh(line, selection);
			//RestrictedAutomaticDuplicateHeuristic urh(expressionLines[j], expressionLines[j+1], ContainerSelectionType::SELECT_FIRST_AVAILABLE_STRATEGY);
			UnrestrictedAutomaticDuplicateHeuristic urh(expressionLines[j], expressionLines[j + 1], ContainerSelectionType::SELECT_FIRST_AVAILABLE_STRATEGY);

			urh.Solve(yard);

			
			total += yard.GetNumberOfMoves();
			totalTime += yard.GetTotalExecutionTime();
		}

		std::cout << total << " " << totalTime << endl;

		vector<std::shared_ptr<ContainerMove>> moves;
	}

	return 0;
}

int runDuplicate()
{
	DataSetLoader* loader = new ZhuDuplicateDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\zhu\\dup\\alpha=0.2");
	//auto t1 = std::chrono::high_resolution_clock::now();



	vector<PriorityHeuristic*> heuristics;
	heuristics.push_back(new TLP());
	heuristics.push_back(new ReshuffleIndex);
	heuristics.push_back(new MinMax);
	heuristics.push_back(new PR3);
	heuristics.push_back(new PR4);
	heuristics.push_back(new PU1);
	heuristics.push_back(new PU2);


	for (auto heuristic : heuristics)
	{
		int total = 0;
		double totalTime = 0;
		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
		{
			auto problemInstance = ds.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			yard.ShouldStoreMoves(true);

			heuristic->Solve(yard);

			//cout << problemInstance.GetInstanceName() << " ";
			//cout << yard.GetNumberOfMoves() << " ";
			//cout << yard.GetTotalExecutionTime() << endl;

			total += yard.GetNumberOfMoves();
			totalTime += yard.GetTotalExecutionTime();
		}

		std::cout << total << " " << totalTime <<endl;

		/*cout << "Total moves: " << total << endl;
		cout << "Total time: " << totalTime;
		cout << "*********************" << endl;*/
	}
	
	
	vector<std::shared_ptr<ContainerMove>> moves;
	std::cout << "";
	return 0;
}


//void runEnsemble()
//{
//	DataSetLoader* loader = new CasertaDataSetLoader();
//	auto ds = loader->Load("G:\\CRP\\caserta\\clean");
//
//
//	SolutionStorage solStor;
//
//
//
//	//std::ifstream infile("expressions.txt");
//	std::ifstream infile("transformed.txt");
//
//
//	std::vector<std::string> allStrings;
//
//	std::string line;
//	while(std::getline(infile, line))
//	{
//		if(!line.empty())
//		{
//			allStrings.push_back(line);
//		}
//	}
//
//
//	double best = std::numeric_limits<double>::infinity();
//
//	std::random_device dev;
//	std::mt19937 rng(dev());
//	//std::uniform_int_distribution dist(0, 29);
//
//	vector<int> indices(30);
//	std::iota(indices.begin(), indices.end(), 0);
//
//	for (int iter = 0; iter < 1000; iter++) {
//
//		int total = 0;
//		double totalTime = 0;
//
//
//		std::shuffle(indices.begin(), indices.end(), rng);
//
//		std::vector<string> expressions;
//
//		for(int i = 0; i<13; i++)
//		{
//			//expressions.push_back(allStrings[0]);
//
//			expressions.push_back(allStrings[indices[i]]);
//		}
//
//		//expressions.push_back("- + / - * REM REM + DUR RI - - REM REM * EMPTY RI / / / DIFF DUR / DUR RI * / DIFF RI + DUR DUR + * + - REM DUR + REM REM / + EMPTY SH * DIFF RI * * - SH EMPTY / EMPTY RI REM");
//		//expressions.push_back("- - - DUR * - SH REM * EMPTY EMPTY + / + EMPTY RI EMPTY + * EMPTY SH / REM EMPTY / / + / RI DIFF / RI EMPTY - / REM RI / REM DIFF * * / DUR REM + DUR DUR / / SH RI * REM SH");
//		//expressions.push_back("* - / * / SH RI REM * / DIFF RI - DIFF REM - / - DUR RI DUR + / REM RI - SH EMPTY + / * RI REM - * DIFF REM EMPTY / * EMPTY + REM REM - EMPTY REM");
//
//
//		//auto t1 = std::chrono::high_resolution_clock::now();
//		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
//		{
//			auto problemInstance = ds.GetProblemInstance(i);
//			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
//			yard.ShouldStoreMoves(true);
//
//			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
//
//			
//
//			shared_ptr<EnsembleAggregationStrategy> aggregationStrategy = make_shared<SumAggregationStrategy>();
//			//shared_ptr<EnsembleAggregationStrategy> aggregationStrategy = make_shared<VoteAggregationStrategy>();
//			//shared_ptr<EnsembleAggregationStrategy> aggregationStrategy = make_shared<MinimumAggregationStrategy>();
//			shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(aggregationStrategy, SelectionType::MINIMUM);
//
//			//RestrictedAutomaticEnsembleHeuristic urh(expressions, selection, eds);
//			UnrestrictedAutomaticEnsembleHeuristic urh(expressions, selection, eds);
//
//			//RestrictedAutomaticHeuristic urh("- + / - * REM REM + DUR RI - - REM REM * EMPTY RI / / / DIFF DUR / DUR RI * / DIFF RI + DUR DUR + * + - REM DUR + REM REM / + EMPTY SH * DIFF RI * * - SH EMPTY / EMPTY RI REM", selection);
//
//			urh.Solve(yard);
//
//			//cout << problemInstance.GetInstanceName() << " ";
//			//cout << yard.GetNumberOfMoves() << endl;
//			total += yard.GetNumberOfMoves();
//			totalTime += yard.GetTotalExecutionTime();
//
//		}
//
//		if (best > total)
//		{
//			best = total;
//			std::cout << "Total moves: " << total << endl;
//
//		}
//		
//		//std::cout << "Total moves: " << total << endl;
//		//std::cout << "Total time: " << totalTime;
//	}
//
//	std::cout << "Best " << best << endl;
//}

int runGRH()
{
	DataSetLoader* loader = new CasertaDataSetLoader();
	//auto ds = loader->Load("C:\\Users\\Korisnik\\projekti\\CRP_testiranja\\proba");
	auto ds = loader->Load("C:\\Users\\Korisnik\\projekti\\CRP_testiranja\\singlebay_with_weights\\caserta\\train");
	//auto ds = loader->Load("G:\\CRP\\zhu\\dup\\alpha=0.2");
	//auto t1 = std::chrono::high_resolution_clock::now();
	vector<double> params;
	for (int i = 0; i < 12; i++)
		params.push_back((double)0.5);
	shared_ptr<GlobalRetrievalHeuristicSettings> GRHSParam = std::unique_ptr<GlobalRetrievalHeuristicSettings>(new GlobalRetrievalHeuristicSettings(params));
	shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();

	PriorityHeuristic* heuristic(new RestrictedGlobalRetrievalHeuristic(selection, GRHSParam));
	//PriorityHeuristic* heuristic(new UnrestrictedGlobalRetrievalHeuristic(selection, GRHSParam));
	int total = 0;
	double totalTime = 0;
	for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
	{
		auto problemInstance = ds.GetProblemInstance(i);
		auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
		yard.ShouldStoreMoves(true);
		
		//heuristic->Solve(yard);

		cout << problemInstance.GetInstanceName() << " ";
		cout << yard.GetNumberOfMoves() << " ";
		cout << yard.GetTotalExecutionTime() << endl;

		total += yard.GetNumberOfMoves();
		totalTime += yard.GetTotalExecutionTime();
	}

	std::cout << total << " " << totalTime << endl;

	//cout << "Total moves: " << total << endl;
	//cout << "Total time: " << totalTime;
	//cout << "*********************" << endl;
	


	vector<std::shared_ptr<ContainerMove>> moves;
	std::cout << "";
	return 0;
}

void evaluateCorrectMO()
{
	DataSetLoader* loader = new ZhuDataSetLoader();
	//DataSetLoader* loader = new CasertaDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\singlebay\\zhu\\test");




	for (int j = 1; j <= 30; j++)
	{
		cout << j << endl;
		string number = to_string(j);
		if(j<10)
		{
			number = "0" + number;
		}
		ifstream f(string("solution_") + number + ".txt"); //taking file as inputstream
		string s;
		if (f) {
			ostringstream ss;
			ss << f.rdbuf(); // reading data
			s = ss.str();
		}
		



		std::regex rgx(R"(<Tree size="\d*">(.*)?<\/Tree>)");
		std::smatch matches;
		//std::cout << "Match found\n";

		ofstream out;
		out.open("testSet_" + number + ".txt");
		string::const_iterator searchStart(s.cbegin());
		while (std::regex_search(searchStart,s.cend(), matches, rgx)) {


			//for (size_t i = 0; i < matches.size(); ++i) {
				int total = 0;
				double totalTime = 0;
				double totalFuel = 0;
				for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
				{
					auto problemInstance = ds.GetProblemInstance(i);
					auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
					yard.ShouldStoreMoves(true);


					shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
					//RestrictedAutomaticHeuristic urh("NRC", selection);
					
					auto s1 = matches[1];
					auto res = StringUtils::rtrim_copy(s1.str());
					//RestrictedAutomaticHeuristic urh(res, selection);
					UnrestrictedAutomaticHeuristic urh(res, selection);

					urh.Solve(yard);

					//std::cout << problemInstance.GetInstanceName() << " ";
					//std::cout << yard.GetNumberOfMoves() << endl;
					//std::cout << yard.GetTotalFuelConsumed() << endl;
					total += yard.GetNumberOfMoves();
					totalTime += yard.GetTotalExecutionTime();
					totalFuel += yard.GetTotalFuelConsumed();

				}
				out << total << "\t" << totalFuel<<endl;
				out.flush();
			//}

			searchStart = matches.suffix().first;

		}
		
		out.close();
		
	}
	
	std::cout << "";

	
}

void evaluateCorrectMOGA()
{
	//DataSetLoader* loader = new ZhuDataSetLoader();
	DataSetLoader* loader = new CasertaDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\singlebay\\caserta\\test");


	shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();


	for (int j = 1; j <= 30; j++)
	{
		cout << j << endl;
		string number = to_string(j);
		if (j < 10)
		{
			number = "0" + number;
		}
		ifstream f(string("solution_") + number + ".txt"); //taking file as inputstream
		string s;
		if (f) {
			ostringstream ss;
			ss << f.rdbuf(); // reading data
			s = ss.str();
		}




		std::regex rgx(R"(<FloatingPoint size="\d*">(.*)?<\/FloatingPoint>)");
		std::smatch matches;
		//std::cout << "Match found\n";

		ofstream out;
		out.open("testSet_" + number + ".txt");
		string::const_iterator searchStart(s.cbegin());
		while (std::regex_search(searchStart, s.cend(), matches, rgx)) {


				int total = 0;
				double totalTime = 0;
				double totalFuel = 0;
				for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
				{
					auto problemInstance = ds.GetProblemInstance(i);
					auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
					yard.ShouldStoreMoves(true);


					
					//RestrictedAutomaticHeuristic urh("NRC", selection);

					auto s1 = matches[1];
					auto res = StringUtils::rtrim_copy(s1.str());

					auto splitted = StringUtils::Split(res, "\t");

					vector<double> params;
					for (auto element : splitted)
						params.push_back(stod(element));
					shared_ptr<GlobalRetrievalHeuristicSettings> GRHSParam = std::unique_ptr<GlobalRetrievalHeuristicSettings>(new GlobalRetrievalHeuristicSettings(params));
					
					auto heuristic  =UnrestrictedGlobalRetrievalHeuristic(selection, GRHSParam);

					heuristic.Solve(yard);

					//std::cout << problemInstance.GetInstanceName() << " ";
					//std::cout << yard.GetNumberOfMoves() << endl;
					//std::cout << yard.GetTotalFuelConsumed() << endl;
					total += yard.GetNumberOfMoves();
					totalTime += yard.GetTotalExecutionTime();
					totalFuel += yard.GetTotalFuelConsumed();

				}
				out << total << "\t" << totalFuel << endl;
				out.flush();

			searchStart = matches.suffix().first;

		}

		out.close();

	}

	std::cout << "";


}


std::vector<shared_ptr<ElectoralSystem>> getElectoralSystemList()
{
	std::vector<shared_ptr<ElectoralSystem>> electoralSystems;

	electoralSystems.push_back(make_shared<AntiPlurality>());
	electoralSystems.push_back(make_shared<ApprovalVoting>());
	electoralSystems.push_back(make_shared<BaldwinMethod>());
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::StandardBorda));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::BinaryNumberSystem));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::Nauru));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::Eurovision));
	electoralSystems.push_back(make_shared<BucklinVoting>());
	electoralSystems.push_back(make_shared<CombinedApprovalVoting>());
	electoralSystems.push_back(make_shared<CondorcetMethod>());
	electoralSystems.push_back(make_shared<ContigentVoting>());
	electoralSystems.push_back(make_shared<CoombsMethod>());
	electoralSystems.push_back(make_shared<CopelandMethod>());
	electoralSystems.push_back(make_shared<InstantRunoff>());
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::MajorityJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::CentralJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::TypicalJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::UsualJudgement));
	electoralSystems.push_back(make_shared<KemenyYoungMethod>());
	electoralSystems.push_back(make_shared<MajorityVote>());
	electoralSystems.push_back(make_shared<MiniMax>());
	electoralSystems.push_back(make_shared<NansonMethod>());
	electoralSystems.push_back(make_shared<OklahomaMethod>());
	electoralSystems.push_back(make_shared<PluralityVote>());
	electoralSystems.push_back(make_shared<PreferentialBlockVoting>());
	electoralSystems.push_back(make_shared<RankedPairs>());
	electoralSystems.push_back(make_shared<SchulzeMethod>());
	electoralSystems.push_back(make_shared<ScoreVoting>());
	electoralSystems.push_back(make_shared<StarVoting>());
	electoralSystems.push_back(make_shared<TidemanAlternativeSmith>());
	electoralSystems.push_back(make_shared<UnifiedPrimary>());

	return  electoralSystems;
}



shared_ptr<ElectoralSystem> createElectoralSystem()
{
	std::ifstream input(string("electoral.txt"));
	std::string line;
	getline(input, line);

	auto params = StringUtils::Split(line, " ");
	string method = params[0];

	std::transform(method.begin(), method.end(), method.begin(),
		[](unsigned char c) { return std::tolower(c); });

	shared_ptr<ElectoralSystem> electoralSystem;

	if (method == "antiplurality")
	{
		electoralSystem = make_shared<AntiPlurality>();
	}
	else if (method == "approval")
	{
		double approvalRate = std::stod(params[1]);
		bool useRange = std::stoi(params[2]);
		bool secondRound = std::stoi(params[3]);

		electoralSystem = make_shared<ApprovalVoting>(approvalRate, useRange, secondRound);
	}
	else if (method == "baldwin")
	{
		electoralSystem = make_shared<BaldwinMethod>();
	}
	else if (method == "borda")
	{
		string bordaScoringString = params[1];
		std::transform(bordaScoringString.begin(), bordaScoringString.end(), bordaScoringString.begin(),
			[](unsigned char c) { return std::tolower(c); });

		BordaCountStrategy strategy;
		if (bordaScoringString == "standard")
		{
			strategy = BordaCountStrategy::StandardBorda;
		}
		else if (bordaScoringString == "binary")
		{
			strategy = BordaCountStrategy::BinaryNumberSystem;
		}
		else if (bordaScoringString == "nauru")
		{
			strategy = BordaCountStrategy::Nauru;
		}
		else if (bordaScoringString == "eurovision")
		{
			strategy = BordaCountStrategy::Eurovision;
		}
		else
		{
			throw "Unknown borda strategy specified";
		}
		electoralSystem = make_shared<BordaCount>(strategy);
	}
	else if (method == "bucklin")
	{
		electoralSystem = make_shared<BucklinVoting>();
	}
	else if (method == "combinedapproval")
	{
		double approvalRate = std::stod(params[1]);
		double disapprovalRate = std::stod(params[2]);
		double useApprovalInterval = std::stod(params[3]);

		electoralSystem = make_shared<CombinedApprovalVoting>(approvalRate, disapprovalRate, useApprovalInterval);
	}
	else if (method == "condorcet")
	{
		electoralSystem = make_shared<CondorcetMethod>();
	}
	else if (method == "contigent")
	{
		electoralSystem = make_shared<ContigentVoting>();
	}
	else if (method == "coombs")
	{
		electoralSystem = make_shared<CoombsMethod>();
	}
	else if (method == "copeland")
	{
		electoralSystem = make_shared<CopelandMethod>();
	}
	else if (method == "instantrunoff")
	{
		electoralSystem = make_shared<InstantRunoff>();
	}
	else if (method == "judgement")
	{
		int maximumRating = std::stoi(params[1]);
		int minimumRating = std::stoi(params[2]);
		string medianRuleString = params[3];
		MedianRule medianRule;
		if (medianRuleString == "typical")
		{
			medianRule = MedianRule::TypicalJudgement;
		}
		else if (medianRuleString == "usual")
		{
			medianRule = MedianRule::UsualJudgement;
		}
		else if (medianRuleString == "central")
		{
			medianRule = MedianRule::CentralJudgement;
		}
		else if (medianRuleString == "majority")
		{
			medianRule = MedianRule::MajorityJudgement;
		}
		else
		{
			throw "Unknown median rule specified for the judgement method";
		}
		electoralSystem = make_shared<JudgementMethod>(maximumRating, minimumRating, medianRule);
	}
	else if (method == "kemenyyoung")
	{
		int maximumConsideredCandidates = std::stoi(params[1]);
		electoralSystem = make_shared<KemenyYoungMethod>(maximumConsideredCandidates);
	}
	else if (method == "majority")
	{
		electoralSystem = make_shared<MajorityVote>();
	}
	else if (method == "minimax")
	{
		electoralSystem = make_shared<MiniMax>();
	}
	else if (method == "nanson")
	{
		electoralSystem = make_shared<NansonMethod>();
	}
	else if (method == "oklahoma")
	{
		electoralSystem = make_shared<OklahomaMethod>();
	}
	else if (method == "plurality")
	{
		bool useSecondRound = std::stoi(params[1]);
		electoralSystem = make_shared<PluralityVote>(useSecondRound);
	}
	else if (method == "preferential")
	{
		electoralSystem = make_shared<PreferentialBlockVoting>();
	}
	else if (method == "rankedpairs")
	{
		electoralSystem = make_shared<RankedPairs>();
	}
	else if (method == "schulze")
	{
		electoralSystem = make_shared<SchulzeMethod>();
	}
	else if (method == "score")
	{
		int maxScore = std::stoi(params[1]);
		int minScore = std::stoi(params[2]);
		electoralSystem = make_shared<ScoreVoting>(maxScore, minScore);
	}
	else if (method == "star")
	{
		electoralSystem = make_shared<StarVoting>();
	}
	else if (method == "tideman")
	{
		electoralSystem = make_shared<TidemanAlternativeSmith>();
	}
	else if (method == "unifiedprimary")
	{
		double approvalRate = std::stod(params[1]);
		bool useApprovalInterval = std::stoi(params[2]);
		electoralSystem = make_shared<UnifiedPrimary>(approvalRate, useApprovalInterval);
	}
	else if (method == "sum")
	{
		electoralSystem = make_shared<SumMethod>();
	}
	return electoralSystem;
}


int randomEnsembleElectoral(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{

	DataSetLoader* loader = new CasertaDataSetLoader();
	auto ds = loader->Load("G:\\CRP\\singlebay\\caserta\\validation");
	auto dsTest = loader->Load("G:\\CRP\\singlebay\\caserta\\test");

	std::ifstream infile("expressions.txt");

	std::vector<std::string> expressions;

	std::string line;
	while (std::getline(infile, line))
	{
		if (!line.empty())
		{
			expressions.push_back(line);
		}
	}

	ofstream myfile;
	string fileName = "out_" + std::to_string(ensembleSize) + ".txt";
	myfile.open(fileName);

	srand(time(NULL));

	vector<int> indices;
	for (int i = 0; i < 50; i++)
	{
		indices.push_back(i);
	}

	clock_t begin = clock();
	for (int r = 0; r < 30; r++) {

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		for (int k = 0; k < 1000; k++) {
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;
			vector<string> ens;

			while (counter <= ensembleSize)
			{
				int randIndex = rand() % copiedIndices.size();
				int index = copiedIndices[randIndex];
				copiedIndices.erase(copiedIndices.begin() + randIndex);
				selectedIndices.push_back(index);
				counter++;
			}

			for (int i = 0; i < selectedIndices.size(); i++)
			{
				ens.push_back(expressions[selectedIndices[i]]);
			}

			int total = 0;
			double totalTime = 0;

			for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
			{
				auto problemInstance = ds.GetProblemInstance(i);
				auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
				yard.ShouldStoreMoves(false);

				shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
				shared_ptr<ElectoralSystem> electoralSystem = createElectoralSystem();
				shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(electoralSystem, SelectionType::MINIMUM);

				RestrictedAutomaticEnsembleHeuristic urh(ens, selection, eds);

				urh.Solve(yard);

				total += yard.GetNumberOfMoves();
				totalTime += yard.GetTotalExecutionTime();
			}
			//std::cout << total <<endl;
			if (total < bestFit)
			{
				bestFit = total;
				bestEnsembleIndices = vector<int>(selectedIndices);
				std::cout << bestFit << endl;
				for (int l = 0; l < bestEnsembleIndices.size(); l++)
				{
					std::cout << bestEnsembleIndices[l] << " ";
				}
				std::cout << endl;
			}
		}

		vector<string> ensemble;
		for (int i = 0; i < bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(expressions[bestEnsembleIndices[i]]);
		}

		int totalTest = 0;
		double totalTestTime = 0;

		for (int i = 0; i < dsTest.GetProblemInstanceNumber(); i++)
		{
			auto problemInstance = dsTest.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			yard.ShouldStoreMoves(false);

			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			shared_ptr<ElectoralSystem> electoralSystem = createElectoralSystem();
			shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(electoralSystem, SelectionType::MINIMUM);

			RestrictedAutomaticEnsembleHeuristic urh(ensemble, selection, eds);

			urh.Solve(yard);

			totalTest += yard.GetNumberOfMoves();
			totalTestTime += yard.GetTotalExecutionTime();
		}
		
		std::cout << "For run i=" << r << " the result was: " << bestFit<< " " << totalTest << std::endl;
		myfile << r << " " << bestFit << " " << totalTest<< " ";

		for (int l = 0; l < bestEnsembleIndices.size(); l++)
		{
			std::cout << bestEnsembleIndices[l] << " ";
			myfile << bestEnsembleIndices[l] << " ";
		}
		std::cout << endl;
		myfile << endl;
		myfile.flush();
	}
	myfile.close();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Elapsed ensemble " << elapsed_secs;

	ofstream duration;
	string fileNameTime = "out_" + std::to_string(ensembleSize) + "_time.txt";
	duration.open(fileNameTime);
	duration << elapsed_secs << endl;
	duration.close();

	return 0;
}


int randomEnsembleElectoralUnrestricted(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{

	DataSetLoader* loader = new CasertaDataSetLoader();

	auto ds = loader->Load("G:\\CRP\\singlebay\\caserta\\validation");
	auto dsTest = loader->Load("G:\\CRP\\singlebay\\caserta\\test");

	//auto ds = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\validation");
	//auto dsTest = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\clean");

	std::ifstream infile("expressions.txt");

	std::vector<std::string> expressions;

	std::string line;
	while (std::getline(infile, line))
	{
		if (!line.empty())
		{
			expressions.push_back(line);
		}
	}

	ofstream myfile;
	string fileName = "out_" + std::to_string(ensembleSize) + ".txt";
	myfile.open(fileName);

	srand(time(NULL));

	vector<int> indices;
	for (int i = 0; i < 50; i++)
	{
		indices.push_back(i);
	}

	clock_t begin = clock();
	for (int r = 0; r < 30; r++) {

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		for (int k = 0; k < 1000; k++) {
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;
			vector<string> ens;

			while (counter <= ensembleSize)
			{
				int randIndex = rand() % copiedIndices.size();
				int index = copiedIndices[randIndex];
				copiedIndices.erase(copiedIndices.begin() + randIndex);
				selectedIndices.push_back(index);
				counter++;
			}

			for (int i = 0; i < selectedIndices.size(); i++)
			{
				ens.push_back(expressions[selectedIndices[i]]);
			}

			int total = 0;
			double totalTime = 0;

			for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
			{
				auto problemInstance = ds.GetProblemInstance(i);
				auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
				yard.ShouldStoreMoves(false);

				shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
				shared_ptr<ElectoralSystem> electoralSystem = createElectoralSystem();
				shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(electoralSystem, SelectionType::MINIMUM);

				UnrestrictedAutomaticEnsembleHeuristic urh(ens, selection, eds);

				urh.Solve(yard);

				total += yard.GetNumberOfMoves();
				totalTime += yard.GetTotalExecutionTime();
			}
			//std::cout << total <<endl;
			if (total < bestFit)
			{
				bestFit = total;
				bestEnsembleIndices = vector<int>(selectedIndices);
				std::cout << bestFit << endl;
				for (int l = 0; l < bestEnsembleIndices.size(); l++)
				{
					std::cout << bestEnsembleIndices[l] << " ";
				}
				std::cout << endl;
			}
		}

		vector<string> ensemble;
		for (int i = 0; i < bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(expressions[bestEnsembleIndices[i]]);
		}

		int totalTest = 0;
		double totalTestTime = 0;

		for (int i = 0; i < dsTest.GetProblemInstanceNumber(); i++)
		{
			auto problemInstance = dsTest.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30);
			yard.ShouldStoreMoves(false);

			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			shared_ptr<ElectoralSystem> electoralSystem = createElectoralSystem();
			shared_ptr<EnsembleDecisionStrategy> eds = make_shared<OneStageEnsembleDecisionStrategy>(electoralSystem, SelectionType::MINIMUM);

			UnrestrictedAutomaticEnsembleHeuristic urh(ensemble, selection, eds);

			urh.Solve(yard);

			totalTest += yard.GetNumberOfMoves();
			totalTestTime += yard.GetTotalExecutionTime();
		}

		std::cout << "For run i=" << r << " the result was: " << bestFit << " " << totalTest << std::endl;
		myfile << r << " " << bestFit << " " << totalTest << " ";

		for (int l = 0; l < bestEnsembleIndices.size(); l++)
		{
			std::cout << bestEnsembleIndices[l] << " ";
			myfile << bestEnsembleIndices[l] << " ";
		}
		std::cout << endl;
		myfile << endl;
		myfile.flush();
	}
	myfile.close();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Elapsed ensemble " << elapsed_secs;

	ofstream duration;
	string fileNameTime = "out_" + std::to_string(ensembleSize) + "_time.txt";
	duration.open(fileNameTime);
	duration << elapsed_secs << endl;
	duration.close();

	return 0;
}

//int evaluateEnsembleElectoral(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
//{
//	StateP state(new State);
//	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
//	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
//	state->setEvalOp(evalOp);
//
//	TreeP tree(new Tree::Tree);
//	state->initialize(argc, argv);
//
//	vector<IndividualP> individuals;
//
//	AlgorithmP alg = state->getAlgorithm();
//	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();
//
//	vector<double> fitnesses;
//	double time = 0;
//
//	for (int i = 0; i < 50; i++) {
//		std::ostringstream stringStream;
//		stringStream << "./ind" << i << ".txt";
//		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
//		IndividualP ind = (IndividualP) new Individual(state);
//		ind->read(xInd);
//		clock_t tStart = clock();
//		ca->evaluate(ind);
//		clock_t tEnd = clock();
//		time += (double)(clock() - tStart) / CLOCKS_PER_SEC;
//		fitnesses.push_back(ind->fitness->getValue());
//		individuals.push_back(ind);
//	}
//
//	cout << "Individual DR eval time: " << time << endl;
//
//
//	std::string line;
//	std::ifstream nameFileout;
//
//	nameFileout.open("out_" + to_string(ensembleSize) + ".txt");
//
//	ofstream myfile;
//	//ofstream occur;
//	myfile.open("outEval_" + to_string(ensembleSize) + ".txt");
//	//occur.open ("outoccur.txt");
//	time = 0;
//
//	auto electoralSystems = getElectoralSystemList();
//
//	while (std::getline(nameFileout, line))
//	{
//		vector<string> splitted;
//		vector<IndividualP> ensemble;
//
//		//			std::getline(nameFileout, line);
//		splitted = split(line, ' ');
//		for (int i = 2; i < splitted.size(); i++)
//		{
//			ensemble.push_back(individuals[atoi(splitted[i].c_str())]);
//		}
//
//		evalOp->electoralSystem = electoralSystem;
//		ca->Evaluate(ensemble, ensemble.size());
//
//		cout << ensemble[0]->fitness->getValue() << " " << endl;
//		myfile << ensemble[0]->fitness->getValue() << " " << endl;
//	}
//
//	cout << "Ensemble: " << time << endl;
//
//
//	myfile.close();
//	nameFileout.close();
//	return 0;
//}

void runMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	//std::vector<int> ensembleSizes{ 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30,35,40,45 };
	std::vector<int> ensembleSizes{3,5,7,10,15,20,25,30,45 };
	for (int ensemble_size : ensembleSizes)
		//for(int ensemble_size = 2; ensemble_size<=50;ensemble_size++)
	{
		cout << "************************************************************************************" << endl;
		cout << "running for esnemble size " << ensemble_size << endl;
		cout << "************************************************************************************" << endl;
		randomEnsembleElectoralUnrestricted(argc, argv, ensemble_size, electoralSystem);
	}
}

//void evaluateMethod(int argc, char** argv)
//{
//	auto electoralSystem = createElectoralSystem();
//	std::vector<int> ensembleSizes{ 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30, 35, 40, 45 };
//	for (int ensemble_size : ensembleSizes)
//	{
//		cout << "************************************************************************************" << endl;
//		cout << "running for esnemble size " << ensemble_size << endl;
//		cout << "************************************************************************************" << endl;
//		evaluateEnsembleElectoral(argc, argv, ensemble_size, electoralSystem);
//	}
//}



void evaluateRolloutManual(int argc, char** argv)
{
	DataSetLoader* loader = new CasertaDataSetLoader();
	auto ds = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\clean");
	//auto ds = loader->Load("G:\\CRP\\caserta\\clean");

	//auto ds = loader->Load("G:\\CRP\\caserta\\large");

	//DataSetLoader* loader = new ZhuDataSetLoader();
	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");

	string line;
	ifstream myfile("expressions.txt");
	ofstream out("tested.txt");

	//double rolrat = stod(argv[1]);
	double rolrat = 0.0;


				
	shared_ptr<PriorityHeuristic> heuristic = make_shared<TLP>(rolrat);
	shared_ptr<PriorityHeuristic> rl = make_shared<RolloutHeuristic>(heuristic);

	RolloutHeuristic rh1(rl);
	//RestrictedAutomaticHeuristic urh(line, selection);
	//UnrestrictedAutomaticHeuristic urh(line, selection);
	int rel = 0;
	int time = 0;
	int energy = 0;
	auto start = high_resolution_clock::now();
	for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
	{
		auto problemInstance = ds.GetProblemInstance(i);
		auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30, -1, 1);

		rh1.Solve(yard);

		rel += yard.GetNumberOfMoves();
		time += yard.GetTotalExecutionTime();
		energy += yard.GetTotalFuelConsumed();
		//cout << yard.GetNumberOfMoves()<<endl;
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	std::cout << duration.count() << endl;
	std::cout << "Total: tes" << rel << endl;
	out << rel << '\t' << time << '\t' << energy << '\t' << duration.count() << endl;
			

}

int main(int argc, char** argv)
{
	//generateEvaluationMatrix(argc, argv);
	//return 0;

	//runInstanceGeneration();
	//return 0;
	//runMethod(argc, argv);

	//runMultibay();
	//return 0;
	//runMethod(argc, argv);
	//evaluateMethod(argc, argv);
	evaluateExpressionsAsEnsemblesFromFile();
	//evaluateExpressionsFromFile();
	//evaluateCorrectMO();
	//evaluateCorrectMOGA();
	//runGRH();
	//runInstanceGeneration();
	//runEnsemble();

	//evaluateRolloutWithExpressionsFromFile(argc, argv);
	//evaluateRolloutManual(argc, argv);
	return 0;

	//DataSetLoader* loader1 = new ZhuDataSetLoader();
	//auto ds1 = loader1->Load("G:\\CRP\\zhu\\instances");
	//return 0;

	//return 0;


	//runDuplicateMultiple();
	//runMultibayMultiple();

	//LeeProblemInstanceGenerator gen(2,5,5,30);
	//cout<<gen.GenerateInstance()<<endl;

	//LeeProblemSetGenerator gen;
	//gen.Generate();

	//ZhuDuplicateProblemSetGenerator generator;
	//generator.Generate();

	//runDuplicateMultiple();
	//runMultibayMultiple();
	//runMultibay();
	//runDuplicate();
	//runDuplicateMultiple();
	//evaluateExpressionsFromFile();
	//exit(0);
	
   // std::cout << "Hello World!\n";
	
	//ar.SetElement(0, 0, 0, 1);
	//ar.SetElement(0, 0, 1, 5);

	//cout << ar.GetElement(0, 0, 1);
	//cout << ar.GetElement(0, 0, 1);

	//CasertaProblemSetGenerator generator;
	//generator.Generate();
	

	//ZhuProblemSetGenerator generator;
	//generator.Generate();
	
	//DataSetLoader* loader = new ZhuDataSetLoader();
	DataSetLoader* loader = new CasertaDataSetLoader();
	//DataSetLoader* loader = new LeeDataSetLoader();

	//auto ds =  loader->Load("G:\\CRP\\test");
	
//	auto ds = loader->Load("G:\\CRP\\zhu\\large_instances");
//	auto ds = loader->Load("G:\\CRP\\caserta\\large");
//	auto ds = loader->Load("G:\\CRP\\caserta\\normal");
	auto ds = loader->Load("G:\\CRP\\caserta\\clean");
	
	//auto ds = loader->Load("C:\\Users\\Marko\\Desktop\\CRP\\sets\\clean");

	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");

	//auto ds = loader->Load("C:\\Users\\Korisnik\\projekti\\CRP_staro\\Problem instances\\mateja");
	//auto ds = loader->Load("G:\\CRP\\casertaselected");
	//auto ds = loader->Load("G:\\CRP\\oneinsta");
	//auto ds = loader->Load("G:\\CRP\\zhu\\instances");
	//
//	
//	auto ds = loader->Load("G:\\CRP\\LeeLee\\Individual, random");


	//std::filesystem::path path{ "G:\\CRP\\test\\data100-100-1.dat.txt" };

	SolutionStorage solStor;



	
	//for (double rr = 0; rr < 1; rr += 0.1)
	{

		int total = 0;
		double totalTime = 0;
		double totalFuel = 0;
		auto t1 = std::chrono::high_resolution_clock::now();

		//shared_ptr<PriorityHeuristic> tl = make_shared<PU1>(rr);
		//shared_ptr<PriorityHeuristic> rl = make_shared<RolloutHeuristicUnrestricted>(tl);

		//RolloutHeuristicUnrestricted rh1(rl);

		for (int i = 0; i < ds.GetProblemInstanceNumber(); i++)
			//for (int i = 0; i < 1; i++)
			//for (int i = 0; i < 1; i++)

		{ 


			auto problemInstance = ds.GetProblemInstance(i);
			auto yard = problemInstance.CreateContainerYard(3.5, 1.2, 40, 30, -1, 1.0);
			yard.ShouldStoreMoves(false);
			TLP tlp;
			//tlp.Solve(yard);

			ReshuffleIndex ri;
			//ri.Solve(yard);

			//ReshuffleIndex tlp;
			MinMax mm;

			//mm.Solve(yard);
			//PR1 pr1;
			//pr1.Solve(yard);
			//tlp.Solve(yard);

			//PR3 pr3;
			//pr3.Solve(yard);

			PR4 pr4;
			//pr4.Solve(yard);

			PU1 pu1;
			//pu1.Solve(yard);

			PU2 pu2;
			pu2.Solve(yard);


			/*shared_ptr<PriorityHeuristic> tl = make_shared<PR4>();
			shared_ptr<PriorityHeuristic> rl = make_shared<RolloutHeuristic>(tl);

			RolloutHeuristic rh1(rl);*/




			//rh1.Solve(yard); // 22483

			//RestrictedAutomaticHeuristic urh("+ / * pos pos DUR + SH SH pos pos DUR / / / + RI DUR pos RI + * SH SH / SH RI + + + DUR RI / SH RI + + SH RI + SH SH");
			//RestrictedAutomaticHeuristic urh("* + * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR + * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR");
			//RestrictedAutomaticHeuristic urh("- * pos / / WL BLKD min MIN MIN min min / WL BLKD + RI NL - + MIN MIN max RI WL * min / + NL NL + RI RI / / BLKD NL + MIN DUR min / min MIN MIN + RI REM / / RI NL + MIN BLKD");
			//RestrictedAutomaticHeuristic urh("* / + / + EMP EMP / NEXT EMP - - BLKD RI RI - * - BLKD RI + EMP EMP * * DIFF DUR - NL DUR + - / + BLKD RI * WL WL WL / - + WL DUR REM - - BLKD RI / SH SH");
			//RestrictedAutomaticHeuristic urh("* / EMP * - - MIN NEXT / RI REM / * DIFF DIFF DUR - - / / DIS DIFF - NL NEXT / / AVG DUR * MIN MIN - / WL RI * + WL NEXT RI");
			//RestrictedAutomaticHeuristic urh("- * / / RI EMP / AVG MIN EMP / / RI - AVG RI AVG");
			//super stablo
			//UnrestrictedAutomaticHeuristic urh("- * EMP CUR / * - - RI AVG * RI EMP / RI + AVG RI / * * EMP AVG EMP * * EMP MIN RI");
			//UnrestrictedAutomaticHeuristic urh("+ / / + MIN + AVG AVG + / MIN AVG AVG * + CUR / CUR EMP * * MIN MIN RI + / AVG * RI + RI AVG * / EMP AVG - RI AVG");
			//UnrestrictedAutomaticHeuristic urh("/ + - * + EMP MIN * EMP MIN RI EMP + * - * EMP AVG CUR CUR + - * AVG AVG - RI AVG + * AVG AVG / CUR AVG");

			//UnrestrictedAutomaticHeuristicWithPrelocationRule urh("- * + - RI AVG EMP * * RI RI * MIN EMP / + RI BLKD AVG");


			//UnrestrictedAutomaticHeuristic urh("+ / - * EMP / RI EMP / / RI AVG AVG + + + EMP AVG + RI AVG RI * / * * AVG EMP CUR + - CUR MIN + AVG MIN * - RI AVG / + CUR RI * EMP AVG");

			//UnrestrictedAutomaticHeuristic urh("- / - * - EMP NL NL + + DUR EMPTY + CUR BLKD - * * DUR EMPTY * BLKD EMPTY - EMP DUR / * + CUR * DUR REM * + NEXT EMPTY / SH DUR * / + DSM DIFF + CUR SH + REM / REM NEXT");



			//RestrictedAutomaticHeuristic urh("- * + / * EMPTY CUR NL / + RI EMP min NL EMPTY min - - EMPTY WL / RI DUR BLKD + min * + EMPTY CUR / RI DUR / + REM BLKD SH min * / EMP NEXT / RI EMPTY * + EMPTY DIFF max REM BLKD");
			//RestrictedAutomaticHeuristic urh("+ - / / - NEXT CUR / NEXT EMP CUR / / NL * DIFF DSM + * NEXT NL / EMPTY DIFF / * + / EMPTY NL * CUR EMPTY + + EMP DSM - EMPTY SH AVG");

			shared_ptr<ContainerSourceSelectionStrategy> selection = make_shared<SelectFirstAvailabelContainerStrategy>();
			//RestrictedAutomaticHeuristic urh("NRC", selection);


			//auto s1 = "* / MIN DIFF RI ";
			//auto s2 = "/ - * AVG REM + / / REM RI - WL MIN RI - / DIFF REM / DIFF / DIFF REM ";
			//auto s1 = "+ - - * RS SH SH - - SH HS / RS * KS LS - * * RS RS SH - / - RS LS LS + SH HS";
			//auto s2 = "/ * / + MIN DIFF - - BLKD REM / NEXT DIS - + / NEXT DIS * MIN RI / + REM EMP * RI EMP + - + REM * MIN DIFF MIN - * RI RI / - DIS MIN / SH BLKD";
			auto s1 = "* - DIFF REM * SH DIFF ";
			//RestrictedAutomaticHeuristic urh(s1, selection);
			//urh.Solve(yard);


			//urh.Solve(yard);
			//Terminals terminals;

			/*Evaluator evaluator(terminals);

			for(int j = 0; j<1000000; j++)
			{
				string expression = "* + * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR + * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR * - + + SH SH * DUR SH DUR - + + SH SH * DUR SH DUR";
				int expressionLength = 79;
				Location l(0, 0);

				evaluator.ParseExpression(expression, expressionLength);
				terminals.CalculateTerminals(yard, l);
				cout << evaluator.Execute();
			}*/



			//std::cout << problemInstance.GetInstanceName()<<" " ;
			//std::cout << yard.GetNumberOfMoves()<<endl;
			//std::cout << yard.GetTotalFuelConsumed() << endl;
			total += yard.GetNumberOfMoves();
			totalTime += yard.GetTotalExecutionTime();
			totalFuel += yard.GetTotalFuelConsumed();

			for(auto move: yard.GetSolution().getContainerMoves())
			{
				cout << "Move: " << move.GetSourceLocation().GetStack() << " " << move.GetDestinationLocation().GetStack()<<endl;
			}

			//solStor.Save(path, yard->GetSolution());
//			auto solution = solStor.Load(path);
			//yard.Simulate(yard.GetSolution().getContainerMoves());

			//std::cout << problemInstance.GetInstanceName()<<" " ;
			//std::cout << yard.GetNumberOfMoves()<<endl;
			//std::cout << yard.GetTotalFuelConsumed() << endl;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		//std::cout << rr << " " << total << " " << duration << endl;


		std::cout << "Duration: " << duration << endl;

		std::cout << "Total moves: " << total << endl;
		std::cout << "Total time: " << totalTime << endl;
		std::cout << "Total fuel: " << totalFuel << endl;
	}
	
	//vector<std::shared_ptr<ContainerMove>> moves;
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(1, 0, 0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(5, 0, 1,0,0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(6, 0, 1, 0, 0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(2, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(6, 0, 0, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(5, 0, 0, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(7, 0, 0, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(3, 0, 0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(4, 0, 2)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(7, 0, 1, 0, 0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(5, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(6, 0, 1)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(7, 0, 0)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(9, 0, 2, 0, 1)));

	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(8, 0, 2)));
	//moves.push_back(std::shared_ptr<ContainerMove>(new ContainerMove(9, 0, 1)));

//	yard->Simulate(moves);
	
	std::cout << "";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
