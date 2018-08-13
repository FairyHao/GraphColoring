#include "stdafx.h"
#include "ReadFileTools.h"

using namespace std;

//把一个文件读为无向图
Graph ReadFileTools::readFileUdg(string fileName)
{
	string s;
	ifstream infile;
	infile.open(fileName);
	if (!infile)
	{
		cout << "error";
		exit(-1);
	}
	int Vnum = 0;
	if (infile >> s)
	{
		Vnum = stoi(s);
	}
	Graph g(Vnum);
	int i = 0;
	int u = -1;
	int v = -1;
	while (infile >> s)
	{
		if (i == 0)
		{
			u = stoi(s);
			i++;
		}
		else if (i == 1)
		{
			v = stoi(s);
			g.AddEdge(u, v);
			i = 0;
			getline(infile, s);
		}
		
	}
	infile.close();
	return g;
}
void ReadFileTools::writeFile(vector<int>* result, string fileName)
{
	vector<int>::iterator it;

	ofstream myfile(fileName);
	if (myfile.is_open())
	{
		for (it = (*result).begin(); it != (*result).end(); it++)//根据pre从小到大的顺序
		{
			int x = *it;
			myfile << x;
			myfile << " ";
		}
		myfile.close();
	}
	delete result;
}
ReadFileTools::~ReadFileTools()
{
}

map<int, set<int>> ReadFileTools::readEdge(string fileName)
{
	map<int, set<int>> EdgeMap;
	ifstream infile;
	infile.open(fileName);
	int i = 0;
	int u, v;
	string s;
	if (!infile)
	{
		cout << "DC_Global_Ins_Batch method read file error";
		exit(-1);
	}while (infile >> s)
	{
		if (i == 0)
		{
			u = stoi(s);
			i++;
		}
		else if (i == 1)
		{
			v = stoi(s);
			EdgeMap[u].insert(v);
			i = 0;
			getline(infile, s);
		}
	}
	infile.close();
	return EdgeMap;
}
void ReadFileTools::DealData()
{
	string s;
	ifstream source("out.movielens-10m_rating");
	ofstream dest("out2.movielens-10m_rating");
	
	//source.open(fileName);
	if (!source)
	{
		cout << "error";
		exit(-1);
	}
	int Vnum = 0;
	if (source >> s)
	{
		dest << s << "\n";
	}
	int i = 0;
	while (source >> s)
	{
		if (i == 0)
		{
			dest << s<<" ";
			i++;
		}
		else if (i == 1)
		{
			dest << s<<"\n";
			i = 0;
			getline(source,s);
		}
	}
	source.close();
	dest.close();
	cout << "end";
}

void ReadFileTools::DealDataDup()
{
	string s;
	ifstream source("out.zhishi-baidu-relatedpages");
	ofstream dest("out1.zhishi-baidu-relatedpages");//out2.movielens-10m_rating");
	map<int,set<int>> vertices;
	//source.open(fileName);
	if (!source)
	{
		cout << "error";
		exit(-1);
	}
	if (getline(source,s))
	{
		dest << s << "\n";
	}
	int i = 0;
	int u;
	int v;
	while (getline(source,s))
	{
		istringstream iss(s);
		vector<string> nodes;
		copy(istream_iterator<string>(iss), istream_iterator<string>(),back_inserter(nodes));
		u = stoi(nodes[0]);
		v = stoi(nodes[1]);
		if (u==v)
		{
			continue;
		}
		//vertices[u].insert(v);
		if (vertices[u].find(v) == vertices[u].end() && vertices[v].find(u) == vertices[v].end())//
		{
			dest << s<<"\n";
			vertices[u].insert(v);
		}
	}
	source.close();
	dest.close();
	cout << "end";
}

