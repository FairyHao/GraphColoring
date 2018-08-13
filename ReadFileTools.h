#pragma once
#include "Graph.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

class ReadFileTools
{
public:
	ReadFileTools();
	static Graph readFileUdg(string fileName);
	static void DealDataDup();
	static void DealData();
	static void writeFile(vector<int>* result, string fileName);
	static map<int, set<int>> readEdge(string fileName);
	~ReadFileTools();
};