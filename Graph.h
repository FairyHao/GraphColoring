#pragma once
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <queue>  
#include <map>
#include <functional>

using namespace std;

class Graph
{
public:
	Graph();
	Graph(int V);
	virtual ~Graph();
	void AddEdge(int u, int v); 
	void RemoveEdge(int u,int v);
	void greedycoloring();
	void DC_Local_Ins(int u,int v);//插入一条边后的重新染色方法
	void DC_Local_del(int u,int v);//删除一条边后的重新染色方法
	void DC_Local_Ins_Batch(string filename);//未测试方法
	void DC_Local_Del_Batch(string filename);//还未写内容
	void Create_DINC_Index();
	void DC_Global_Ins(int u,int v);
	void DC_Global_Ins1(int u, int v);//20180514优化DC_Global_Ins代码;
	void DC_Global_Del(int u,int v);
	void DC_Global_Ins_Batch(map<int, set<int>> InsEdgeMap);
	void DC_Global_Batch( map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap);
	void DC_Global_Batch1(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap);
	void DC_Global_Batch2(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap);
	void DC_Global_Batch3(map<int,set<int>> InsEdgeMap,map<int,set<int>> DelEdgeMap);
	void DC_Global_Batch4(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap);
	void DC_Global_Batch5(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap);

	void Create_Deg_Index();

	void color_Ins(int u,int color);
	void color_Ins1(int u, int color);
	void color_del(int u,int color);
	void color_del1(int u, int color);
	void color_Ins_self(int u,int color,bool flag);
	void color_del_self(int u,int color,bool flag);
	int V;
	int E;
	vector<vector<int>> vertices;
	vector<vector<int>> vertices_in;//根据顺序构造的有向图，的入度节点。
	vector<vector<int>> vertices_out;///根据顺序构造的有向图，的出度节点
	vector<int> colorResult;//每个顶点的染色状况
	vector<int> degrees;//每个顶点的度
	vector<int> ver_seq;//按度从高到低的节点排序，内容是顶点号。
	int maxcolor;//目前还暂未使用。
	set<int> colorIndexSet;//0-deg-(x)未被邻居节点使用的节点。
	//索引结构
	vector<vector<int>> cntVector;//每个顶点的邻接顶点染色情况。
	vector<vector<int>> CandidateVector;//u的候选节点。限定是比u的颜色小的颜色
	vector<int> OrderNums;//每个节点按度排序后，序号是多少？内容是每个顶点的序号。

	//自己的颜色索引结构
	vector<vector<int>> NotUsedColorIndex;//入度节点没有使用的颜色；使用最小堆；0-degree-size
	vector<vector<int>> UsedColorIndex;//入度节点已经使用的颜色；增序；
	void Create_Self_DINC_Index();

	set<int> ver_set;//要重新染色节点5/11或许要被废弃
	vector<map<int, set<int>>> PosDegIndex;//key 为+；5/11或许要被废弃
	vector<map<int, set<int>>> NegDegIndex;//key 为-；5/11或许要被废弃

	//vector<priority_queue<int, vector<int>, Compare1>> vertices_in_Batch;
	//vector<priority_queue<int, vector<int>, Compare2>> vertices_out_Batch;
private:
	set<int>* getSc(int u);
	void Recolor(int u);
	void Recolor(set<int> edgeSet);//未测试
	void SmallestUnassignedColor(int u);
	void getOrderNum();
	void Maintain_Deg();
	int GetDegCha(int u,int v);
	void Create_Deg_Index(int u,int v);
	bool IsBefore(int u,int v);
};

class Compare
{
public:
	Graph *ggg;
	bool operator()(int x1, int x2)
	{
		return ggg->OrderNums[x1] > ggg->OrderNums[x2];//从大到小
	}
};

class Compare1
{
public:
	Graph *ggg;
	bool operator()(int u, int v)
	{
		if (ggg->degrees[u] < ggg->degrees[v] || (ggg->degrees[u] == ggg->degrees[v] && u > v))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Compare2
{
public:
	Graph *ggg;
	bool operator()(int u, int v)
	{
		if (ggg->degrees[u] > ggg->degrees[v] || (ggg->degrees[u] == ggg->degrees[v] && u < v))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
};