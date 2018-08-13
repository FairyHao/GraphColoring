// GraphColoring1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ReadFileTools.h"
#include "Graph.h"
#include "map"
#include <windows.h>
#include <algorithm>

/**/
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD start, end;
	start = GetTickCount();
	Graph g = ReadFileTools::readFileUdg("out80%.movielens-10m_rating"); //out1.zhishi - baidu - relatedpages");//
	end = GetTickCount();
	cout << "read file run time(ms):" << double(end - start)<<endl;

	start = GetTickCount();
	g.greedycoloring();
	end = GetTickCount();
	cout <<endl<< "greedy coloring run time(ms):" << double(end - start)<<endl;
	
	//g.Create_DINC_Index();
	g.Create_Self_DINC_Index();
	
	map<int, set<int>> addmap = ReadFileTools::readEdge("out20%.movielens-10m_rating");//outIns.zhishi-baidu-relatedpages");//
	map<int, set<int>> delmap = ReadFileTools::readEdge("testDel.txt");
	cout << endl << "read add edge end" << endl;

	/*
	start = GetTickCount();
	g.DC_Global_Ins_Batch(addmap);
	end = GetTickCount();
	cout <<endl<<"DC_Global Ins one by one(ms):" << double(end - start)<<endl;*/
	
	start = GetTickCount();
	g.DC_Global_Batch3(addmap, {});
	end = GetTickCount();
	cout << endl <<"DC_Global Ins by batch3(ms):" << double(end - start) << endl;
	
	//ReadFileTools::DealDataDup();
	//cout << "end";
	/*test code
	vector<int> xx = {1,2,3,6,5,9,2,3};
	xx.erase(std::remove(xx.begin(), xx.end(), 10), xx.end());
	make_heap(xx.begin(),xx.end(),greater<int>());
	cout << xx.front()<<endl;
	std::pop_heap(xx.begin(), xx.end(), greater<int>());
	xx.pop_back();
	/*xx.push_back(4);
	push_heap(xx.begin(), xx.end(), greater<int>());
	cout << xx.front()<<endl;
	xx.erase(std::remove(xx.begin(), xx.end(), 2), xx.end());
	xx.erase(std::remove(xx.begin(), xx.end(), 7), xx.end());
	push_heap(xx.begin(), xx.end(), greater<int>());
	sort(xx.begin(), xx.end(),less<int>());

	for (int i = 0;i<xx.size();i++)
	{
		cout << xx[i] << " ";
	}
	cout << endl;
	xx.erase(find(xx.begin(),xx.end(),3));
	for (int i = 0; i<xx.size(); i++)
	{
		cout << xx[i] << " ";
	}	cout << endl;

	vector<int>::iterator it = std::lower_bound(xx.begin(), xx.end(), 4, std::less<int>()); // find proper position in descending order
	xx.insert(it, 4); // insert before iterator it

	for (int i = 0; i<xx.size(); i++)
	{
		cout << xx[i] << " ";
	}	cout << endl;
	while (xx.size()>0)
	{
		cout << xx.front()<<endl;
		//std::pop_heap(xx.begin(), xx.end(), greater<int>());
		//xx.pop_back();
	}*/
	while (1) {}
	system("pause");
	return 0;
}

