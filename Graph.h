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
	void DC_Local_Ins(int u,int v);//����һ���ߺ������Ⱦɫ����
	void DC_Local_del(int u,int v);//ɾ��һ���ߺ������Ⱦɫ����
	void DC_Local_Ins_Batch(string filename);//δ���Է���
	void DC_Local_Del_Batch(string filename);//��δд����
	void Create_DINC_Index();
	void DC_Global_Ins(int u,int v);
	void DC_Global_Ins1(int u, int v);//20180514�Ż�DC_Global_Ins����;
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
	vector<vector<int>> vertices_in;//����˳���������ͼ������Ƚڵ㡣
	vector<vector<int>> vertices_out;///����˳���������ͼ���ĳ��Ƚڵ�
	vector<int> colorResult;//ÿ�������Ⱦɫ״��
	vector<int> degrees;//ÿ������Ķ�
	vector<int> ver_seq;//���ȴӸߵ��͵Ľڵ����������Ƕ���š�
	int maxcolor;//Ŀǰ����δʹ�á�
	set<int> colorIndexSet;//0-deg-(x)δ���ھӽڵ�ʹ�õĽڵ㡣
	//�����ṹ
	vector<vector<int>> cntVector;//ÿ��������ڽӶ���Ⱦɫ�����
	vector<vector<int>> CandidateVector;//u�ĺ�ѡ�ڵ㡣�޶��Ǳ�u����ɫС����ɫ
	vector<int> OrderNums;//ÿ���ڵ㰴�����������Ƕ��٣�������ÿ���������š�

	//�Լ�����ɫ�����ṹ
	vector<vector<int>> NotUsedColorIndex;//��Ƚڵ�û��ʹ�õ���ɫ��ʹ����С�ѣ�0-degree-size
	vector<vector<int>> UsedColorIndex;//��Ƚڵ��Ѿ�ʹ�õ���ɫ������
	void Create_Self_DINC_Index();

	set<int> ver_set;//Ҫ����Ⱦɫ�ڵ�5/11����Ҫ������
	vector<map<int, set<int>>> PosDegIndex;//key Ϊ+��5/11����Ҫ������
	vector<map<int, set<int>>> NegDegIndex;//key Ϊ-��5/11����Ҫ������

	//vector<priority_queue<int, vector<int>, Compare1>> vertices_in_Batch;
	//vector<priority_queue<int, vector<int>, Compare2>> vertices_out_Batch;
private:
	set<int>* getSc(int u);
	void Recolor(int u);
	void Recolor(set<int> edgeSet);//δ����
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
		return ggg->OrderNums[x1] > ggg->OrderNums[x2];//�Ӵ�С
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