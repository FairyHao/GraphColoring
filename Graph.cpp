#include "stdafx.h"
#include "Graph.h"
#include <iostream>
#include "set"
#include "map"
#include <windows.h>
#include <algorithm>
using namespace std;

Graph::Graph()
{
}
Graph::Graph(int V)
{
	this->V = V;
	this->E = 0;
	this->vertices = vector<vector<int>>(this->V);
	colorResult = vector<int>(this->V, -1);
	degrees = vector<int>(this->V);
}
void Graph::AddEdge(int u, int v)
{
	vertices[u].push_back(v);
	vertices[v].push_back(u);
	degrees[u]++;
	degrees[v]++;
	this->E += 2;
}
void Graph::RemoveEdge(int u,int v)
{
	for (int i = 0; i < vertices[u].size(); i++)
	{
		if (vertices[u][i] == v)
		{
			vertices[u].erase(vertices[u].begin() + i);
			break;
		}
	}
	for (int i = 0; i < vertices[v].size(); i++)
	{
		if (vertices[v][i] == u)
		{
			vertices[v].erase(vertices[v].begin() + i);
			break;
		}
	}
	degrees[u]--;
	degrees[v]--;
}
//贪婪算法：按照度降序排序，然后按此顺序对每个节点赋值；比如对节点u，赋值原则是：邻居没有使用的最小原则。
//增加构造有向图G*
void Graph::greedycoloring()
{
	set<int> colors;
	cout << "0" << endl;
	//1、按照度对节点降序排序？？？？？？？？？20180505这里是否可以更改为使用STL里make_heap()方法。重新定义sort方法。
	for (int u = 0; u < this->V; u++)
	{
		//ver_seq是部分排好序的节点
		vector<int>::iterator iter = ver_seq.begin();
		//插入排序；
		for (; iter != ver_seq.end();iter++)
		{
			if (degrees[u] > degrees[*iter])//给节点找到合适的位置
			{
				break;                     
			}
		}
		ver_seq.insert(iter, u);//插入节点
	}
	cout << "1"<<endl;
	//按照已排好序的顺序遍历节点
	vertices_in = vector<vector<int>>(this->V);
	vertices_out = vector<vector<int>>(this->V);
	
	//根据顶点顺序
	for (vector<int>::iterator ver_iter = ver_seq.begin();
		ver_iter != ver_seq.end(); ver_iter++)
	{
		int u = *ver_iter;
		vector<int> adj_ver = vertices[u];//邻居节点
		vector<bool> available(adj_ver.size()+1);//(0，degree-1)哪个颜色未被使用。
		for (vector<int>::iterator adj_iter = adj_ver.begin()
			; adj_iter != adj_ver.end(); adj_iter++)
		{
			int v = *adj_iter;
			if (this->colorResult[v] != -1 && this->colorResult[v] <= adj_ver.size())//标记邻居节点已经使用的颜色。
			{
				available[colorResult[v]] = true;
			}
			/*根据节点的度大小顺序，构造有向图start*/
			if (degrees[u] > degrees[v])
			{
				vertices_out[u].push_back(v);
			}
			else if (degrees[u] < degrees[v])
			{
				vertices_in[u].push_back(v);
			}
			else if (degrees[u] == degrees[v] && u > v)
			{
				vertices_in[u].push_back(v);
			}
			else
			{
				vertices_out[u].push_back(v);
			}
			/*根据节点的度大小顺序，构造有向图end*/
		}
		
		for (int i = 0; i <= adj_ver.size(); i++)
		{
			if (!available[i])
			{
				colorResult[u] = i;
				colors.insert(i);
				break;
			}
		}
	}
	//this->getOrderNum();
}
//方法功能：新增一条边，如何重新着色？
//1、如果新增边两个邻接顶点的颜色相同，则需要对其中一个顶点重新复制
	//a 计算两个顶点的SC（获得集合(color(u.邻居))）
	//b 对SC的小的那个顶点重新着色
//2、cmin<cmax;则直接着色。
//3、cmin>cmax;则从邻居节点中，找一个可以使用的颜色ccand。
	//a 如果ccand<cmin;则顶点染色ccand，重新对使用ccand颜色的顶点找可以使用的最小颜色着色。
	//b 如果ccand>cmin,则顶点直接染色cmin。
void Graph::DC_Local_Ins(int u,int v)//插入一条u->v的边
{
	this->AddEdge(u,v);//这里假定已存在的边b
	if (colorResult[u] == colorResult[v])
	{
		//求u的sc,sc(u)即u的所有邻居使用的颜色集合
		set<int> *sc1 = getSc(u);
		set<int> *sc2 = getSc(v);
		if (sc1->size() > sc2->size())
		{
			Recolor(v);
		} 
		else 
		{
			Recolor(u);
		}
	}
}

void Graph::DC_Local_del(int u, int v)
{
	RemoveEdge(u, v);
	Recolor(u);
	Recolor(v);
}

set<int>* Graph::getSc(int u)
{
	set<int> *sc = new set<int>();
	vector<int> adj_ver = this->vertices[u];//邻居节点
	vector<int>::iterator adj_iter = adj_ver.begin();
	for (; adj_iter != adj_ver.end(); adj_iter++)
	{
		sc->insert(colorResult[*adj_iter]);
	}
	return sc;
}
void Graph::Recolor(int u)
{
	int cmin = 100000000;
	int cmax = -1;
	int ccand = -1;
	set<int> *sc = getSc(u);
	for (int i = 0; i <= vertices[u].size(); i++)//0~degree(u)
	{
		if (sc->find(i) != sc->end())
		{
			if (i > cmax)
			{
				cmax = i;
			}
		}
		else
		{
			if (cmin > i)
			{
				cmin = i;
			}
		}
	}
	if (cmin < cmax)
	{
		cout << u << "的颜色从" <<colorResult[u];
		colorResult[u] = cmin;
		cout << "更改为" << colorResult[u];
	}
	else//即cmin>cmax的情况，不可能存在cmin=cmax的情况
	{
		vector<int> mcolor(cmax+1, 0);
		vector<int> adj_ver = this->vertices[u];//邻居节点
		vector<int>::iterator adj_iter = adj_ver.begin();
		int minmcolor = 100000000;
		for (; adj_iter != adj_ver.end(); adj_iter++)
		{
			int v = *adj_iter;
			int tempcolor = colorResult[v];
			int tempSCSize = getSc(v)->size();
			mcolor[tempcolor] = tempSCSize > mcolor[tempcolor] ? tempSCSize : mcolor[tempcolor];

			if (mcolor[tempcolor] < minmcolor)
			{
				minmcolor = mcolor[tempcolor];
				ccand  = tempcolor;
			}
		}
		if (minmcolor < cmin)//minmcolor:邻居最小sc。cmin:u的sc
		{
			cout << u << "的颜色从" << colorResult[u];
			colorResult[u] = ccand;
			cout << "更改为" << colorResult[u];
			vector<int> adj_ver = this->vertices[u];//邻居节点
			for (vector<int>::iterator adj_iter = adj_ver.begin();
				adj_iter != adj_ver.end(); adj_iter++)
			{
				int v = *adj_iter;
				if (colorResult[v] == ccand)
				{
					SmallestUnassignedColor(v);
				}
			}
		}
		else
		{
			cout << u << "的颜色从" << colorResult[u];
			colorResult[u] = cmin;
			cout << "更改为" << colorResult[u];
		}
	}
	
}

void Graph::SmallestUnassignedColor(int u)
{
	set<int> *sc = getSc(u);
	for (int i = 0; i <= vertices[u].size(); i++)
	{
		if (sc->find(i) == sc->end())
		{
			cout << u << "的颜色从" << colorResult[u];
			colorResult[u] = i;
			cout << "更改为" << colorResult[u];
			break;
		}
	}
}
Graph::~Graph()
{
}
//未测试方法
void Graph::DC_Local_Ins_Batch(string fileName)
{
	set<int> addEdgeSet;
	ifstream infile;
	infile.open(fileName);
	int i = 0;
	int u, v;
	string s;
	if (!infile)
	{
		cout << "error";
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
			if (colorResult[u] == colorResult[v])
			{
				AddEdge(u, v);
				if (degrees[u] > degrees[v])
				{
					addEdgeSet.insert(v);
				}
				else
				{
					addEdgeSet.insert(u);
				}
			}
			i = 0;
		}
		Recolor(addEdgeSet);
	}
	infile.close();
}
//未测试方法
void Graph::Recolor(set<int> edgeSet)
{
	for (set<int>::iterator iter = edgeSet.begin();
		iter != edgeSet.end(); iter++)
	{
		int u = *iter;
		set<int> *sc = getSc(u);
		for (int i = 0; i <= vertices[u].size(); i++)
		{
			if (sc->find(i) == sc->end())
			{
				colorResult[u] = i;
				if (i > maxcolor)
				{
					this->greedycoloring();
				}
				break;
			}
		}
	}
}
//这个索引有没有改进的空间呢？总感觉很麻烦的样子？
void Graph::Create_DINC_Index()
{
	//节点都是list，可不可以不要用list；
	cntVector = vector<vector<int>>(this->V);
	CandidateVector = vector<vector<int>>(this->V);
	//对所有的节点的0-indeg(i)
	for (int u = 0; u < V; u++)
	{
		cntVector[u] = vector<int>(vertices_in[u].size()+1);
		for (int i = 0; i < vertices_in[u].size(); i++)
		{
			int v = vertices_in[u][i];
			if (colorResult[v]<=vertices_in[u].size())
			{
				cntVector[u][colorResult[v]]++;
			}
		}
		/*如果再贪婪算法之后求索引，则这部分一定都为0，所以写不写下面的代码都可以
		for (int color = 0; color < colorResult[u]; color++)
		{
			if (cntVector[u][color]==0)
			{
				CandidateVector[u].push_back(color);
			}
		}*/
	}
}
void Graph::DC_Global_Ins_Batch(map<int, set<int>> InsEdgeMap)
{
	/*cout << endl;
	for (int i = 0;i<V;i++)
	{
	cout <<colorResult[i]<<" ";
	}*/
	int i = 0;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		//cout << u<<" ";
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			//cout << v << endl;
			DC_Global_Ins1(u, v);
		}
		//cout <<u<<" "<<i++ << endl;
	}
	/*cout << endl;
	for (int i = 0;i<V;i++)
	{
	cout <<colorResult[i]<<" ";
	}*/
}
void Graph::DC_Global_Ins(int u, int v)
{
	AddEdge(u, v);
	//设置u的度总是大于v的度
	int Uorder = OrderNums[u];
	int Vorder = OrderNums[v];
	if (Uorder > Vorder)//这个if未测试4/25 16:57
	{
		int temp = u;
		u = v;
		v = temp;
		temp = Uorder;
		Uorder = Vorder;
		Vorder = temp;
	}
	set<int> removedIterSet;

	if (!((Uorder > 0 && degrees[u] < degrees[ver_seq[Uorder - 1]])
		|| Uorder == 0 ||
		(Uorder > 0 && degrees[u] == degrees[ver_seq[Uorder - 1]] && u > ver_seq[Uorder-1])))
	{
		ver_set.insert(u);

		//重新给U安排位置
		for (int i = Uorder-1; i >= 0;i--)
		{
			if (degrees[u] > degrees[ver_seq[i]] 
				|| (degrees[u] == degrees[ver_seq[i]] && u < ver_seq[i]))
			{
				ver_seq[i+1] = ver_seq[i];
				OrderNums[ver_seq[i]]++;
			}
			else
			{
				ver_seq[i + 1] = u;
				OrderNums[u] = i + 1;
				break; 
			}
		}
		//u的出入度重新更改。
		for (vector<int>::iterator iter = vertices_in[u].begin()
			; iter != vertices_in[u].end(); iter++)
		{
			if (degrees[u] > degrees[*iter] || (degrees[u] == degrees[*iter] && u < (*iter)))
			{
				//更新入边和出边
				vertices_out[u].push_back(*iter);
				vertices_in[*iter].push_back(u);
				ver_set.insert(*iter);
				//color更新
				color_Ins(*iter, colorResult[u]);
				color_del(u, colorResult[*iter]);
				vertices_out[*iter].erase(std::remove(vertices_out[*iter].begin(), vertices_out[*iter].end(), u), vertices_out[*iter].end());
				removedIterSet.insert(*iter);
			}
		}
		for (set<int>::iterator setIter = removedIterSet.begin(); setIter != removedIterSet.end(); setIter++)
		{
			vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
		}
	}
	removedIterSet.clear();
	if (!((Vorder > 0 && degrees[v] < degrees[ver_seq[Vorder - 1]])
		|| Vorder == 0
		||(Vorder > 0 && degrees[v] == degrees[ver_seq[Vorder - 1]] && v > ver_seq[Vorder - 1])))
	{
		ver_set.insert(v);
		//重新给U安排位置
		for (int i = Vorder - 1; i >= 0; i--)
		{
			if (degrees[v] > degrees[ver_seq[i]]
				|| (degrees[v] == degrees[ver_seq[i]]
				&& v < ver_seq[i]))
			{
				ver_seq[i + 1] = ver_seq[i];
				OrderNums[ver_seq[i]]++;
			}
			else
			{
				ver_seq[i + 1] = v;
				OrderNums[v] = i+1; 
				break;
			}
		}
		//v的出入度重新更改。
		for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
		{
			if (degrees[v] > degrees[*iter] 
		|| (degrees[v] == degrees[*iter] && v < (*iter)))
			{
				//有向图更新，即出入度更新
				vertices_out[v].push_back(*iter);
				vertices_in[*iter].push_back(v);
				ver_set.insert(*iter);
				//color更新
				color_Ins(*iter,colorResult[v]);
				vertices_out[*iter].erase(remove(vertices_out[*iter].begin(), vertices_out[*iter].end(), v), vertices_out[*iter].end());
				removedIterSet.insert(*iter);
			}
		}
		for (set<int>::iterator setIter = removedIterSet.begin(); setIter != removedIterSet.end(); setIter++)
		{
			vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), *setIter), vertices_in[v].end());
			color_del(v, colorResult[*setIter]);
		}
	}
	
	//有向图中增加u，v边
	vertices_in[v].push_back(u);
	vertices_out[u].push_back(v);
	color_Ins(v,colorResult[u]);
	//v
	
	for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
	{
		if (colorResult[*iter]==vertices_in[v].size())
		{
			color_Ins(v, colorResult[*iter]);
		}
	}
	Compare com;
	com.ggg = this;
	priority_queue<int, std::vector<int>, Compare> q = priority_queue<int, std::vector<int>,Compare>(com);
	for (set <int>::iterator s1_Iter = ver_set.begin(); s1_Iter != ver_set.end(); s1_Iter++)
	{
		q.push(*s1_Iter);
	}
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	while (!q.empty())
	{
		boolean flag = false;
		u = q.top();
		q.pop();
		ver_set.erase(u);
		oldcolor = colorResult[u];
		if (!CandidateVector[u].empty())
		{
			int min = 10000000000000000;
			for (vector<int>::iterator iter = CandidateVector[u].begin(); iter != CandidateVector[u].end(); iter++)
			{
				if (*iter < min)
				{
					min = *iter;
				}
			}
			flag = true;
			colorResult[u] = min;
			newcolor = min;
		}
		else
		{
			for (int i = 0; i != cntVector[u].size(); i++)
			{
				if (cntVector[u][i] == 0)
				{
					if (colorResult[u] == i)
					{
						break;
					}
					else
					{
						flag = true;
						colorResult[u] = i;
						newcolor = i;
						break;
					}
				}
			}
		}
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				color_del(*iter, oldcolor);
				color_Ins(*iter, newcolor);
				if ((ver_set.find(*iter) == ver_set.end())
					&& (colorResult[u] == colorResult[*iter]
						|| oldcolor < colorResult[*iter]))
				{
					ver_set.insert(*iter);
					q.push(*iter);
				}
			}
			CandidateVector[u].clear();
		}
	}
}
void Graph::DC_Global_Ins1(int u, int v)
{
	//如果v->u;则u,v变量互换。
	if (!IsBefore(u,v))
	{
		int temp = u;
		u = v;
		v = temp;
	}
	AddEdge(u, v);
	set<int> tempset;
	set<int> queueset;//队列中需要放入的元素
	queueset.insert(u);
	queueset.insert(v);
	//cout << "iterator 1 ";
	//u的出入度更新
	for (vector<int>::iterator iter = vertices_in[u].begin(); iter != vertices_in[u].end(); iter++)
	{
		int k = *iter;//k->u;
		if (IsBefore(u,k))//u before k
		{
			//更新入边和出边
			vertices_out[u].push_back(k);
			vertices_in[k].push_back(u);
			//color更新
			color_Ins(k, colorResult[u]);
			vertices_out[k].erase(std::remove(vertices_out[k].begin(), vertices_out[k].end(), u), vertices_out[k].end());
			tempset.insert(k);//需从vertices_in[u]中删除的节点
			queueset.insert(k);
		}
	}
	for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
	{
		vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
		color_del(u, colorResult[*setIter]);
	}
	
	for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
	{
		for (vector<int>::iterator iter = vertices_in[*setIter].begin(); iter != vertices_in[*setIter].end(); iter++)
		{
			if (colorResult[*iter] == vertices_in[*setIter].size() && (u != (*iter)))
			{
				color_Ins(*setIter, colorResult[*iter]);
			}
		}
	}
	tempset.clear();
	//v的出入度重新更改。
	//cout << "iterator 2 ";
	for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
	{
		int k = *iter;
		if (IsBefore(v, k))//k->v变为v->k
		{
			//有向图更新，即出入度更新
			vertices_out[v].push_back(k);
			vertices_in[k].push_back(v);
			//color更新
			color_Ins(k, colorResult[v]);
			vertices_out[k].erase(remove(vertices_out[k].begin(), vertices_out[k].end(), v), vertices_out[k].end());
			queueset.insert(k);
			tempset.insert(k);
		}
	}
	for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
	{
		vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), *setIter), vertices_in[v].end());
		color_del(v, colorResult[*setIter]);
	}

	for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
	{
		for (vector<int>::iterator iter = vertices_in[*setIter].begin(); iter != vertices_in[*setIter].end(); iter++)
		{
			int k = *iter;
			if (colorResult[k] == vertices_in[*setIter].size()&& (v != k))
			{
				color_Ins(*setIter, colorResult[*iter]);
			}
		}
	}
	//有向图中增加u，v边
	vertices_in[v].push_back(u);
	vertices_out[u].push_back(v);
	color_Ins(v, colorResult[u]);
	
	//v的入度顶点处理
	for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
	{
		if (colorResult[*iter] == vertices_in[v].size())
		{
			color_Ins(v, colorResult[*iter]);
		}
	}
	

	Compare1 com;
	com.ggg = this;
	priority_queue<int, std::vector<int>, Compare1> q = priority_queue<int, std::vector<int>, Compare1>(com);
	for (set <int>::iterator s1_Iter = queueset.begin(); s1_Iter != queueset.end(); s1_Iter++)
	{
		q.push(*s1_Iter);
	}
	//cout << "Can method ";
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	while (!q.empty())
	{
		boolean flag = false;
		u = q.top();
		q.pop();
		//cout << "queue size" << queueset.size()<<"u"<<u;
		queueset.erase(u);
		oldcolor = colorResult[u];
		if (!CandidateVector[u].empty())
		{
			int min = INT_MAX;
			for (vector<int>::iterator iter = CandidateVector[u].begin(); iter != CandidateVector[u].end(); iter++)
			{
				if (*iter < min)
				{
					min = *iter;
				}
			}
			flag = true;
			colorResult[u] = min;
			newcolor = min;
		}
		else
		{
			for (int i = 0; i < cntVector[u].size(); i++)
			{
				if (cntVector[u][i] == 0)
				{
					if (colorResult[u] == i)
					{
						break;
					}
					else
					{
						flag = true;
						colorResult[u] = i;
						newcolor = i;
						break;
					}
				}
			}
		}
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				color_del(*iter, oldcolor);
				color_Ins(*iter, newcolor);
				if ((queueset.find(*iter) == queueset.end()) && (colorResult[u] == colorResult[*iter]|| oldcolor < colorResult[*iter]))
				{
					//cout << "queue input" << *iter;
					queueset.insert(*iter);
					q.push(*iter);
				}
			}
		}
		CandidateVector[u].clear();
	}
	//cout << "Can method end" << endl;
}

void Graph::DC_Global_Del(int u,int v)
{

}
//获得每个节点的排序
void Graph::getOrderNum()
{
	OrderNums = vector<int>(this->V);
	int i = 0;
	for (vector<int>::iterator iter = ver_seq.begin();
		iter != ver_seq.end(); iter++)
	{
		OrderNums[*iter] = i++;
	}
}
//论文中的color_Ins方法
void Graph::color_Ins(int u, int color)
{
	if (cntVector[u].size() < vertices_in[u].size()+1)
	{
		cntVector[u].push_back(0);
	}
	if (color <= vertices_in[u].size())
	{
		cntVector[u][color]++;
		CandidateVector[u].erase(std::remove(CandidateVector[u].begin(), CandidateVector[u].end(), color), CandidateVector[u].end());
	}
}
void Graph::color_del(int u, int color)
{
	if (cntVector[u].size() > vertices_in[u].size() + 1)
	{
		cntVector[u].erase(cntVector[u].end() - 1);
	}
	if (color <= vertices_in[u].size())
	{
		cntVector[u][color]--;
		if (cntVector[u][color] == 0 && color<colorResult[u])
		{
			CandidateVector[u].push_back(color);
		}
	}
}
//自己的方法，增加了堆排序
void Graph::color_Ins1(int u, int color)
{
	if (cntVector[u].size() < vertices_in[u].size() + 1)
	{
		cntVector[u].push_back(0);
	}
	if (color <= vertices_in[u].size())
	{
		cntVector[u][color]++;
		CandidateVector[u].erase(std::remove(CandidateVector[u].begin(), CandidateVector[u].end(), color), CandidateVector[u].end());
		//push_heap(CandidateVector[u].begin(), CandidateVector[u].end(), greater<int>());
	}
}
void Graph::color_del1(int u, int color)
{
	if (cntVector[u].size() > vertices_in[u].size() + 1)
	{
		cntVector[u].erase(cntVector[u].end() - 1);
	}
	if (color <= vertices_in[u].size())
	{
		cntVector[u][color]--;
		if (cntVector[u][color]==0 && color<colorResult[u])
		{
			CandidateVector[u].push_back(color);
			push_heap(CandidateVector[u].begin(), CandidateVector[u].end(), greater<int>());
		}
	}
}
//1、重新排序，找到需要重新着色的节点入队列2、对所有队列里的节点重新着色，方法同单个着色。
//所以改进的重点是重新排序
void Graph::DC_Global_Batch(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	//map<int, int> RealChangeMap;

	//Compare com;
	//com.ggg = this;
	//std::priority_queue<int, std::vector<int>, Compare> q(com);

	////获得每个节点度的变化
	//for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	//{
	//	int u = (*iter).first;
	//	for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
	//	{
	//		int v = *siter;
	//		AddEdge(u, v);
	//		RealChangeMap[u]++;
	//		RealChangeMap[v]++;
	//	}
	//}
	//for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	//{
	//	int u = (*iter).first;
	//	for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
	//	{
	//		int v = *siter;
	//		RemoveEdge(u, v);
	//		RealChangeMap[u]--;
	//		RealChangeMap[v]--;
	//	}
	//}
	//
	////处理出入度，并将需要update color 的节点入优先队列
	//for (map<int, int>::iterator iter = RealChangeMap.begin(); iter != RealChangeMap.end(); iter++)
	//{
	//	int u = iter->first;//获取节点
	//	int RealChangeNum = iter->second;//节点的改变情况
	//	q.push(u);
	//	//如果度没有改变，则放入队列即可
	//	if (RealChangeNum == 0)
	//	{
	//		continue;
	//	}
	//	if (RealChangeNum > 0)//入度顶点。即判断 v->u 是否要改为 u->v; 
	//	{
	//		for (map<int, set<int>>::iterator mapiter = PosDegIndex[u].begin();
	//			mapiter != PosDegIndex[u].end() && mapiter->first <= RealChangeNum; mapiter++)
	//		{
	//			int TheoryChangeNum = mapiter->first;
	//			for (set<int>::iterator siter = mapiter->second.begin();
	//				siter != mapiter->second.end(); siter++)
	//			{
	//				int v = *siter;
	//				//u,v的度都改变，那么重新计算两个顶点的出入度关系
	//				if (RealChangeMap.find(v) != RealChangeMap.end() && RealChangeMap.find(v)->second!=0)
	//				{
	//					((set<int>)(PosDegIndex[u].find(RealChangeMap)->second)).erase(v);
	//					((set<int>)(DegIndex[v].find(-DegChangeNum)->second)).erase(u);
	//					Create_Deg_Index(u, v);
	//					Create_Deg_Index(v, u);
	//					//if (IsBefore(u, v) && DegChangeNum < 0)now：u->v old:u->v,则出入度关系不用更改
	//					if (IsBefore(u, v) && DegChangeNum > 0)//now:u->v old:v->u;  1.更改出入度关系；2. 维护Degree Index
	//					{
	//						vertices_in[u].erase(remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
	//						vertices_out[v].erase(remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
	//						vertices_out[u].push_back(v);
	//						vertices_in[v].push_back(u);
	//					}
	//					else if (!IsBefore(u, v) && DegChangeNum < 0)//now:v->u; old:u->v;
	//					{
	//						vertices_in[v].erase(remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
	//						vertices_out[u].erase(remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
	//						vertices_out[v].push_back(u);
	//						vertices_in[u].push_back(v);
	//					}
	//					//else//now:v->u;old:v->u;
	//				}
	//				else
	//				{
	//					vertices_in[u].erase(remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
	//					vertices_out[v].erase(remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
	//					vertices_out[u].push_back(v);
	//					vertices_in[v].push_back(u);
	//					//维护Degree Index 1.删掉旧的关系2、建立新关系
	//					((set<int>)(PosDegIndex[u].find(TheoryChangeNum)->second)).erase(v);
	//					((set<int>)(NegDegIndex[v].find(-TheoryChangeNum)->second)).erase(u);
	//					Create_Deg_Index(u, v);
	//					Create_Deg_Index(v, u);
	//					
	//				}
	//				q.push(v);
	//			}
	//		}
	//	}
	//	else//RealChangeNum<0;
	//	{
	//		 
	//	}
	//	//出度入度更改
	//}
	////处理新增、删除节点
	//for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	//{
	//	u = iter->first;
	//	set<int> tempset = iter->second;
	//	for (set<int>::iterator ssiter = tempset.begin();ssiter!=tempset.end();ssiter++)
	//	{
	//		v = *ssiter;
	//		//u->v
	//		if (degrees[u] > degrees[v] || (degrees[u] == degrees[v] && u < v))
	//		{
	//			vertices_out[u].push_back(v);
	//			vertices_in[v].push_back(u);
	//		}
	//		else//v->u;
	//		{
	//			vertices_in[u].push_back(v);
	//			vertices_out[v].push_back(u);
	//		}
	//	}
	//	
	//}
	//for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	//{
	//	u = iter->first;
	//	set<int> tempset = iter->second;
	//	for (set<int>::iterator ssiter = tempset.begin(); ssiter != tempset.end(); ssiter++)
	//	{
	//		v = *ssiter;
	//		//u->v
	//		if (degrees[u] + DegChangeMap[u] > degrees[v]+DegChangeMap[v] || (degrees[u]+DegChangeMap[u] == degrees[v]+DegChangeMap[v] && u < v))
	//		{
	//			//删除u，v的出入度关系。
	//			vertices_out[u].erase(remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
	//			vertices_in[v].erase(remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
	//		}
	//		else//v->u;
	//		{
	//			//删除u，v的出入度关系。
	//			vertices_out[v].erase(remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
	//			vertices_in[u].erase(remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
	//		}
	//	}
	//}
}

//1.出入度的存储使用优先队列。20180513剪枝出入度的想法放弃
void Graph::DC_Global_Batch1(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	//Create_InNeighbor_Index 因为priority queue 的关系,不应该在这个方法里。
	//vector<priority_queue<int, vector<int>, Compare1>> vertices_in_Batch(this->V);//将入度顶点从小到大排序
	//vector<priority_queue<int, vector<int>, Compare2>> vertices_out_Batch(this->V);//讲出度顶点从大到小排序
	//Compare1 com1;
	//com1.ggg = this;
	//Compare2 com2;
	//com2.ggg = this;
	//for (int i = 0; i < this->V; i++)
	//{
	//	priority_queue<int, vector<int>, Compare1> pq1(com1);
	//	priority_queue<int, vector<int>, Compare2> pq2(com2);

	//	int u = i;
	//	vector<int> adjVex = vertices[i];
	//	for (vector<int>::iterator iter = adjVex.begin(); iter != adjVex.end(); iter++)
	//	{
	//		int v = *iter;
	//		if (IsBefore(u, v))
	//		{
	//			pq2.push(v);
	//		}
	//		else
	//		{
	//			pq1.push(v);
	//		}
	//	}
	//	vertices_in_Batch[u] = pq1;
	//	vertices_out_Batch[u] = pq2;
	//}

	////新增、删除边
	//for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	//{
	//	int u = (*iter).first;
	//	for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
	//	{
	//		int v = *siter;
	//		AddEdge(u, v);
	//	}
	//}
	//for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	//{
	//	int u = (*iter).first;
	//	for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
	//	{
	//		int v = *siter;
	//		RemoveEdge(u, v);
	//	}
	//}


}
/*
1、增加、删除边
		(1)无向图中增加、删除边
		(2)有向图中删除边，颜色索引的维护
2、新增边修改为有向图;（因为必须根据增加删除边后的度来确定有向图的方向）
   颜色索引的维护;
3、度有变动的顶点的出入度处理
	a. 如果一个顶点的度的改变>0
	   （1）入度顶点遍历 处理；
	   （2）颜色索引的维护
	   （3）入优先队列
	b. 如果一个顶点的度的改变<0
		（1）出度顶点遍历处理
		（2）颜色索引的维护；
		（3）入优先队列； 
*/
void Graph::DC_Global_Batch2(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	DWORD start, end;
	start = GetTickCount();
	map<int, int> changemap;//<顶点，改变数>；
	//1、新增边；
	vector<int> degreesTemp = degrees;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			AddEdge(u, v);//无向图中增加边
			changemap[u]++;
			changemap[v]++;
		}
	}
	//1、删除边
	for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			RemoveEdge(u, v);//无向图中删除
			changemap[u]--;
			changemap[v]--;
			//有向图中删除，并维护颜色索引。
			if (degreesTemp[u] > degreesTemp[v] || (degreesTemp[u] == degreesTemp[v] && u < v))//u is before v;
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
				vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
				color_del(v, colorResult[u]);
			}
			else//v is before u; v->u;
			{
				vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
				color_del(u, colorResult[v]);
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增删除边 run time(ms):" << double(end - start) << endl;
	
	start = GetTickCount();
	//新增边的出入度增加;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			if (IsBefore(u, v))//u->v;
			{
				vertices_out[u].push_back(v);
				vertices_in[v].push_back(u);
				color_Ins(v, colorResult[u]);
				//v的入度增加1,维护颜色索引
				for (vector<int>::iterator vviter = vertices_in[v].begin(); vviter != vertices_in[v].end(); vviter++)
				{
					int k = *vviter;
					if (colorResult[k] == vertices_in[v].size() && k != u)
					{
						color_Ins(v, colorResult[k]);
					}
				}
			}
			else//v->u;
			{
				vertices_out[v].push_back(u);
				vertices_in[u].push_back(v);
				color_Ins(u, colorResult[v]);
				//u的入度增加1,维护颜色索引
				for (vector<int>::iterator vviter = vertices_in[u].begin(); vviter != vertices_in[u].end(); vviter++)
				{
					int k = *vviter;
					if (colorResult[k] == vertices_in[u].size() && k!= v)
					{
						color_Ins(u, colorResult[k]);
					}
				}
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增边的出入度增加 run time(ms):" << double(end - start) << endl;
	
	start = GetTickCount();
	Compare1 com;
	com.ggg = this;
	priority_queue<int, vector<int>, Compare1> q = priority_queue<int, vector<int>, Compare1>(com);
	set<int> pqset;//和q中的元素一样；因为q中不能插入重复的元素；这个用来检查元素是否重复；
	//2、度有变动的顶点的出入度处理。
	for (map<int, int>::iterator miter = changemap.begin(); miter != changemap.end(); miter++)
	{
		int u = miter->first;
		int changenum = miter->second;
		if (pqset.find(u) == pqset.end())
		{
			q.push(u);
			pqset.insert(u);
		}
		set<int> tempset;//u的入度、出度顶点中需要删除的顶点集合。
		if (changenum > 0)
		{
			for (vector<int>::iterator viter = vertices_in[u].begin(); viter != vertices_in[u].end(); viter++)
			{
				int v = *viter;
				if (!IsBefore(u, v))//v->u;
				{
					continue;
				}
				else//由v->u改为u->v;
				{
					//改变u，v的出入度关系
					vertices_out[u].push_back(v);
					vertices_in[v].push_back(u);
					//color更新
					color_Ins(v, colorResult[u]);
					color_del(u, colorResult[v]);
					vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
					tempset.insert(v);
					//color Index 维护
					for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
					{
						int k = *iter;
						if (colorResult[k] == vertices_in[v].size() && (u != k))
						{
							color_Ins(v, colorResult[k]);
						}
					}
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			//u的入度顶点删除。
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
			}
		}
		else
		{
			for (vector<int>::iterator viter = vertices_out[u].begin(); viter != vertices_out[u].end(); viter++)
			{
				int v = *viter;
				if (IsBefore(u, v))//u->v;
				{
					continue;
				}
				else//u-v改为v->u;
				{
					//改变u，v的出入度关系
					vertices_in[u].push_back(v);
					vertices_out[v].push_back(u);
					//color更新
					color_Ins(u, colorResult[v]);
					color_del(v, colorResult[u]);

					//color index 维护
					for (vector<int>::iterator iter = vertices_in[u].begin(); iter != vertices_in[u].end(); iter++)
					{
						int k = *iter;
						if (colorResult[k] == vertices_in[u].size() && k != v)
						{
							color_Ins(u, colorResult[k]);
						}
					}
					vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
					tempset.insert(v);
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), *setIter), vertices_out[u].end());
			}
		}
	}
	end = GetTickCount();
//	cout << endl << "出入度改变 run time(ms):" << double(end - start) << endl;
	
	start = GetTickCount();
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	while (!q.empty())
	{
		boolean flag = false;
		int u = q.top();
		q.pop();
		pqset.erase(u);
		oldcolor = colorResult[u];
		if (!CandidateVector[u].empty())
		{
			int min = MAXINT;
			for (vector<int>::iterator iter = CandidateVector[u].begin(); iter != CandidateVector[u].end(); iter++)
			{
				if (*iter < min)
				{
					min = *iter;
				}
			}
			flag = true;
			colorResult[u] = min;
			newcolor = min;
		}
		else
		{
			for (int i = 0; i != cntVector[u].size(); i++)
			{
				if (cntVector[u][i] == 0)
				{
					if (colorResult[u] == i)
					{
						break;
					}
					else
					{
						flag = true;
						colorResult[u] = i;
						newcolor = i;
						break;
					}
				}
			}
		}
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				color_del(*iter, oldcolor);
				color_Ins(*iter, newcolor);
				if ((pqset.find(*iter) == pqset.end())
					&& (colorResult[u] == colorResult[*iter]
						|| oldcolor < colorResult[*iter]))
				{
					pqset.insert(*iter);
					q.push(*iter);
				}
			}
		}
		CandidateVector[u].clear();
	}
	end = GetTickCount();
	//cout << endl << "CAN run time(ms):" << double(end - start) << endl;
	cout << endl;
	for (int i = 0;i<V;i++)
	{
		cout <<colorResult[i]<<" ";
	}
}

//用了自己的颜色索引，发现不成功，运行时间反倒变长了。
void Graph::DC_Global_Batch3(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	DWORD start, end;
	start = GetTickCount();
	map<int, int> changemap;//<顶点，改变数>；
							//1、新增边；
	vector<int> degreesTemp = degrees;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			AddEdge(u, v);//无向图中增加边
			changemap[u]++;
			changemap[v]++;
		}
	}
	//1、删除边
	for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			RemoveEdge(u, v);//无向图中删除
			changemap[u]--;
			changemap[v]--;
			//有向图中删除，并维护颜色索引。
			if (degreesTemp[u] > degreesTemp[v] || (degreesTemp[u] == degreesTemp[v] && u < v))//u is before v;
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
				vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
				color_del_self(v, colorResult[u],true);
			}
			else//v is before u; v->u;
			{
				vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
				color_del_self(u, colorResult[v],true);
			}
		}
	}
	end = GetTickCount();
	cout << endl << "新增删除边 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//新增边的出入度增加;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
  		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			if (IsBefore(u, v))//u->v;
			{
				vertices_out[u].push_back(v);
				vertices_in[v].push_back(u);
				color_Ins_self(v, colorResult[u],true);
			}
			else//v->u;
			{
				vertices_out[v].push_back(u);
				vertices_in[u].push_back(v);
				color_Ins_self(u, colorResult[v],true);
			}
		}
	}
	end = GetTickCount();
	cout << endl << "新增边的出入度增加 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	Compare1 com;
	com.ggg = this;
	priority_queue<int, vector<int>, Compare1> q = priority_queue<int, vector<int>, Compare1>(com);
	set<int> pqset;//和q中的元素一样；因为q中不能插入重复的元素；这个用来检查元素是否重复；
				   //2、度有变动的顶点的出入度处理。
	for (map<int, int>::iterator miter = changemap.begin(); miter != changemap.end(); miter++)
	{
		int u = miter->first;
		int changenum = miter->second;
		if (pqset.find(u) == pqset.end())
		{
			q.push(u);
			pqset.insert(u);
		}
		set<int> tempset;//u的入度、出度顶点中需要删除的顶点集合。
		if (changenum > 0)
		{
			for (vector<int>::iterator viter = vertices_in[u].begin(); viter != vertices_in[u].end(); viter++)
			{
				int v = *viter;
				if (!IsBefore(u, v))//v->u;
				{
					continue;
				}
				else//由v->u改为u->v;
				{
					//改变u，v的出入度关系
					vertices_out[u].push_back(v);
					vertices_in[v].push_back(u);
					//color更新
					color_Ins_self(v, colorResult[u],true);
					color_del_self(u, colorResult[v],true);
					vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
					tempset.insert(v);
	
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			//u的入度顶点删除。
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
			}
		}
		else
		{
			for (vector<int>::iterator viter = vertices_out[u].begin(); viter != vertices_out[u].end(); viter++)
			{
				int v = *viter;
				if (IsBefore(u, v))//u->v;
				{
					continue;
				}
				else//u-v改为v->u;
				{
					//改变u，v的出入度关系
					vertices_in[u].push_back(v);
					vertices_out[v].push_back(u);
					//color更新
					color_Ins_self(u, colorResult[v],true);
					color_del_self(v, colorResult[u],true);

					vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
					tempset.insert(v);
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), *setIter), vertices_out[u].end());
			}
		}
	}
	end = GetTickCount();
	cout << endl << "出入度改变 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	while (!q.empty())
	{
		boolean flag = false;
		int u = q.top();
		q.pop();
		pqset.erase(u);
		oldcolor = colorResult[u];
		int newcolor = NotUsedColorIndex[u].front();
		if (newcolor != oldcolor)
		{
			flag = true;
			colorResult[u] = newcolor;
			pop_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
			NotUsedColorIndex[u].pop_back();
		}
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				color_del_self(*iter, oldcolor,false);
				color_Ins_self(*iter, newcolor,false);
				if ((pqset.find(*iter) == pqset.end())
					&& (colorResult[u] == colorResult[*iter]
						|| oldcolor < colorResult[*iter]))
				{
					pqset.insert(*iter);
					q.push(*iter);
				}
			}
		}
	}
	end = GetTickCount();
    cout << endl << "CAN run time(ms):" << double(end - start) << endl;
	/*cout << endl;
	for (int i = 0;i<V;i++)
	{
		cout <<colorResult[i]<<" ";
	}
	cout << endl;*/
}
void Graph::Create_Self_DINC_Index()
{
	NotUsedColorIndex = vector<vector<int>>(this->V);
	UsedColorIndex = vector<vector<int>>(this->V);
	for (int u = 0; u < this->V; u++)
	{
		vector<bool> avail = vector<bool>(vertices_in[u].size() + 1);
		for (int i = 0; i < vertices_in[u].size(); i++)
		{
			int v = vertices_in[u][i];
			UsedColorIndex[u].push_back(colorResult[v]);
			if (colorResult[v] <= vertices_in[u].size())
			{
				avail[colorResult[v]] = true;
			}
		}
		for (int i = 0; i < avail.size(); i++)
		{
			if (avail[i] == false)
			{
				NotUsedColorIndex[u].push_back(i);
			}
		}
		//sort(UsedColorIndex[u].begin(), UsedColorIndex[u].end(), less<int>());
		//make_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
	}
}
void Graph::color_Ins_self(int u,int color,bool flag)
{
	vector<int>::iterator it = std::lower_bound(UsedColorIndex[u].begin(), UsedColorIndex[u].end(), color, std::less<int>()); // find proper position in descending order
	UsedColorIndex[u].insert(it, color); // insert before iterator it
	if (color <= vertices_in[u].size())
	{
		vector<int>::iterator iter = remove(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), color);
		if (iter != NotUsedColorIndex[u].end())
		{
			NotUsedColorIndex[u].erase(iter, NotUsedColorIndex[u].end());
			//push_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
		}
	}
	if (flag)
	{
		NotUsedColorIndex[u].push_back(vertices_in[u].size());
		//push_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
	}
}
void Graph::color_del_self(int u,int color,bool flag)
{
	UsedColorIndex[u].erase(find(UsedColorIndex[u].begin(), UsedColorIndex[u].end(), color));
	if (color <= vertices_in[u].size())
	{
		if (find(UsedColorIndex[u].begin(), UsedColorIndex[u].end(), color) == UsedColorIndex[u].end())
		{
			NotUsedColorIndex[u].push_back(color);
			//push_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
		}
	}
	if (flag)
	{
		NotUsedColorIndex[u].erase(remove(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), vertices_in[u].size() + 1), NotUsedColorIndex[u].end());
		//push_heap(NotUsedColorIndex[u].begin(), NotUsedColorIndex[u].end(), greater<int>());
	}
}
//将颜色索引变为堆排序，运行时间和不是堆差不多。
void Graph::DC_Global_Batch4(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	DWORD start, end;
	start = GetTickCount();
	map<int, int> changemap;//<顶点，改变数>；
							//1、新增边；
	vector<int> degreesTemp = degrees;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			AddEdge(u, v);//无向图中增加边
			changemap[u]++;
			changemap[v]++;
		}
	}
	//1、删除边
	for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			RemoveEdge(u, v);//无向图中删除
			changemap[u]--;
			changemap[v]--;
			//有向图中删除，并维护颜色索引。
			if (degreesTemp[u] > degreesTemp[v] || (degreesTemp[u] == degreesTemp[v] && u < v))//u is before v;
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
				vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
				//color_del1(v, colorResult[u]);
			}
			else//v is before u; v->u;
			{
				vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
				//color_del1(u, colorResult[v]);
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增删除边 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//新增边的出入度增加;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			if (IsBefore(u, v))//u->v;
			{
				vertices_out[u].push_back(v);
				vertices_in[v].push_back(u);
				//color_Ins1(v, colorResult[u]);
				//v的入度增加1,维护颜色索引
				//for (vector<int>::iterator vviter = vertices_in[v].begin(); vviter != vertices_in[v].end(); vviter++)
				//{
				//	int k = *vviter;
				//	if (colorResult[k] == vertices_in[v].size() && k != u)
				//	{
				//	//	color_Ins1(v, colorResult[k]);
				//	}
				//}
			}
			else//v->u;
			{
				vertices_out[v].push_back(u);
				vertices_in[u].push_back(v);
				//color_Ins1(u, colorResult[v]);
				//u的入度增加1,维护颜色索引
				//for (vector<int>::iterator vviter = vertices_in[u].begin(); vviter != vertices_in[u].end(); vviter++)
				//{
				//	int k = *vviter;
				//	if (colorResult[k] == vertices_in[u].size() && k != v)
				//	{
				//		//color_Ins1(u, colorResult[k]);
				//	}
				//}
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增边的出入度增加 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	Compare1 com;
	com.ggg = this;
	priority_queue<int, vector<int>, Compare1> q = priority_queue<int, vector<int>, Compare1>(com);
	set<int> pqset;//和q中的元素一样；因为q中不能插入重复的元素；这个用来检查元素是否重复；
				   //2、度有变动的顶点的出入度处理。
	for (map<int, int>::iterator miter = changemap.begin(); miter != changemap.end(); miter++)
	{
		int u = miter->first;
		int changenum = miter->second;
		if (changenum==0)
		{
			continue;
		}
		if (pqset.find(u) == pqset.end())
		{
			q.push(u);
			pqset.insert(u);
		}
		set<int> tempset;//u的入度、出度顶点中需要删除的顶点集合。
		if (changenum > 0)
		{
			for (vector<int>::iterator viter = vertices_in[u].begin(); viter != vertices_in[u].end(); viter++)
			{
				int v = *viter;
				if (!IsBefore(u, v))//v->u;
				{
					continue;
				}
				else//由v->u改为u->v;
				{
					//改变u，v的出入度关系
					vertices_out[u].push_back(v);
					vertices_in[v].push_back(u);
					//color更新
					//color_Ins1(v, colorResult[u]);
					//color_del1(u, colorResult[v]);
					vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
					tempset.insert(v);
					//color Index 维护
					/*for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
					{
						int k = *iter;
						if (colorResult[k] == vertices_in[v].size() && (u != k))
						{
							color_Ins1(v, colorResult[k]);
						}
					}*/
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			//u的入度顶点删除。
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
			}
		}
		else
		{
			for (vector<int>::iterator viter = vertices_out[u].begin(); viter != vertices_out[u].end(); viter++)
			{
				int v = *viter;
				if (IsBefore(u, v))//u->v;
				{
					continue;
				}
				else//u-v改为v->u;
				{
					//改变u，v的出入度关系
					vertices_in[u].push_back(v);
					vertices_out[v].push_back(u);
					//color更新
					//color_Ins1(u, colorResult[v]);
					//color_del1(v, colorResult[u]);

					////color index 维护
					//for (vector<int>::iterator iter = vertices_in[u].begin(); iter != vertices_in[u].end(); iter++)
					//{
					//	int k = *iter;
					//	if (colorResult[k] == vertices_in[u].size() && k != v)
					//	{
					//		color_Ins1(u, colorResult[k]);
					//	}
					//}
					vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
					tempset.insert(v);
					if (pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), *setIter), vertices_out[u].end());
			}
		}
	}
	end = GetTickCount();
	//	cout << endl << "出入度改变 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	int NotSame = 0;
	int Same = 0;
	while (!q.empty())
	{
		boolean flag = false;
		int u = q.top();
		q.pop();
		pqset.erase(u);
		oldcolor = colorResult[u];
	//	cout <<u<< "oldcolor" << oldcolor<<" ";
		//用最原始的方法来找颜色
		vector<bool> avail = vector<bool>(vertices_in[u].size()+1);
		for (int i = 0;i<vertices_in[u].size();i++)
		{
			int tempcolor = colorResult[vertices_in[u][i]];
			if (tempcolor <= vertices_in[u].size())
			{
				avail[tempcolor] = true;
			}
		}
		for (int i = 0;i < avail.size();i++)
		{
			if (!avail[i])
			{
				newcolor = i;
				break;
			}
		}
		if (newcolor != oldcolor)
		{
			NotSame++;
			flag = true;
			colorResult[u] = newcolor;
			//cout <<u<< "newcolor" << newcolor<<endl;
		}
		else
		{
			//cout << "color not change" << endl;
			Same++;
		}
		/*
		if (!CandidateVector[u].empty())
		{
			newcolor = CandidateVector[u].front();
			colorResult[u] = newcolor;
			pop_heap(CandidateVector[u].begin(), CandidateVector[u].end(), greater<int>());
			CandidateVector[u].pop_back();
			flag = true;
		}
		else
		{
			for (int i = 0; i != cntVector[u].size(); i++)
			{
				if (cntVector[u][i] == 0)
				{
					if (colorResult[u] == i)
					{
						break;
					}
					else
					{
						flag = true;
						colorResult[u] = i;
						newcolor = i;
						break;
					}
				}
			}
		}*/
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				//color_del1(*iter, oldcolor);
				//color_Ins1(*iter, newcolor);
			if ((pqset.find(*iter) == pqset.end())
				&& (colorResult[u] == colorResult[*iter]
						|| oldcolor < colorResult[*iter]))
				{
					pqset.insert(*iter);
					q.push(*iter);
				}
			}
		}
		//CandidateVector[u].clear();
	}
	cout << "NotSame" << NotSame << " ";
	cout << "Same" << Same;
	end = GetTickCount();
	cout << endl << "CAN run time(ms):" << double(end - start) << endl;
	/*cout << endl;
	for (int i = 0;i<V;i++)
	{
	  cout <<colorResult[i]<<" ";
	}
	cout << endl;*/
}
//基于原来的方法增加 基于出入度的剪枝
void Graph::DC_Global_Batch5(map<int, set<int>> InsEdgeMap, map<int, set<int>> DelEdgeMap)
{
	DWORD start, end;
	start = GetTickCount();
	map<int, int> changemap;//<顶点，改变数>；
							//1、新增边；
	vector<int> degreesTemp = degrees;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			AddEdge(u, v);//无向图中增加边
			changemap[u]++;
			changemap[v]++;
		}
	}
	//1、删除边
	for (map<int, set<int>>::iterator iter = DelEdgeMap.begin(); iter != DelEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			RemoveEdge(u, v);//无向图中删除
			changemap[u]--;
			changemap[v]--;
			//有向图中删除，并维护颜色索引。
			if (degreesTemp[u] > degreesTemp[v] || (degreesTemp[u] == degreesTemp[v] && u < v))//u is before v;
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), v), vertices_out[u].end());
				vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
				//color_del1(v, colorResult[u]);
			}
			else//v is before u; v->u;
			{
				vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), v), vertices_in[u].end());
				//color_del1(u, colorResult[v]);
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增删除边 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//新增边的出入度增加;
	for (map<int, set<int>>::iterator iter = InsEdgeMap.begin(); iter != InsEdgeMap.end(); iter++)
	{
		int u = (*iter).first;
		for (set<int>::iterator siter = iter->second.begin(); siter != iter->second.end(); siter++)
		{
			int v = *siter;
			if (IsBefore(u, v))//u->v;
			{
				vertices_out[u].push_back(v);
				vertices_in[v].push_back(u);
				//color_Ins1(v, colorResult[u]);
				//v的入度增加1,维护颜色索引
				//for (vector<int>::iterator vviter = vertices_in[v].begin(); vviter != vertices_in[v].end(); vviter++)
				//{
				//	int k = *vviter;
				//	if (colorResult[k] == vertices_in[v].size() && k != u)
				//	{
				//	//	color_Ins1(v, colorResult[k]);
				//	}
				//}
			}
			else//v->u;
			{
				vertices_out[v].push_back(u);
				vertices_in[u].push_back(v);
				//color_Ins1(u, colorResult[v]);
				//u的入度增加1,维护颜色索引
				//for (vector<int>::iterator vviter = vertices_in[u].begin(); vviter != vertices_in[u].end(); vviter++)
				//{
				//	int k = *vviter;
				//	if (colorResult[k] == vertices_in[u].size() && k != v)
				//	{
				//		//color_Ins1(u, colorResult[k]);
				//	}
				//}
			}
		}
	}
	end = GetTickCount();
	//cout << endl << "新增边的出入度增加 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	Compare1 com;
	com.ggg = this;
	priority_queue<int, vector<int>, Compare1> q = priority_queue<int, vector<int>, Compare1>(com);
	set<int> pqset;//和q中的元素一样；因为q中不能插入重复的元素；这个用来检查元素是否重复；
				   //2、度有变动的顶点的出入度处理。
	map<int, set<int>> newEdgeMap;
	for (map<int, int>::iterator miter = changemap.begin(); miter != changemap.end(); miter++)
	{
		int u = miter->first;
		int changenum = miter->second;
		if (changenum == 0)
		{
			continue;
		}
		if (pqset.find(u) == pqset.end())
		{
			q.push(u);
			pqset.insert(u);
		}
		set<int> tempset;//u的入度、出度顶点中需要删除的顶点集合。
		if (changenum > 0)
		{
			int mincolorTemp = MAXINT;
			int mincolortempVertice = MAXINT;
			bool flagtemp = false;
			for (vector<int>::iterator viter = vertices_in[u].begin(); viter != vertices_in[u].end(); viter++)
			{
				int v = *viter;
				if (!IsBefore(u, v))//v->u;
				{
					continue;
				}
				else//由v->u改为u->v;
				{
					flagtemp = true;
					//改变u，v的出入度关系
					vertices_out[u].push_back(v);
					vertices_in[v].push_back(u);
					newEdgeMap[u].insert(v);
					//color更新
					//color_Ins1(v, colorResult[u]);
					//color_del1(u, colorResult[v]);
					vertices_out[v].erase(std::remove(vertices_out[v].begin(), vertices_out[v].end(), u), vertices_out[v].end());
					if (colorResult[v] < mincolorTemp)
					{
						mincolorTemp = colorResult[v];
						mincolortempVertice = v;
					}
					tempset.insert(v);
					//color Index 维护
					/*for (vector<int>::iterator iter = vertices_in[v].begin(); iter != vertices_in[v].end(); iter++)
					{
					int k = *iter;
					if (colorResult[k] == vertices_in[v].size() && (u != k))
					{
					color_Ins1(v, colorResult[k]);
					}
					}*/
				}
			}
			if (flagtemp && pqset.find(mincolortempVertice) == pqset.end())
			{
				q.push(mincolortempVertice);
				pqset.insert(mincolortempVertice);
				//newEdgeMap[u].erase(mincolortempVertice);
			}
			//u的入度顶点删除。
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_in[u].erase(std::remove(vertices_in[u].begin(), vertices_in[u].end(), *setIter), vertices_in[u].end());
			}
		}
		else
		{
			for (vector<int>::iterator viter = vertices_out[u].begin(); viter != vertices_out[u].end(); viter++)
			{
				int v = *viter;
				if (IsBefore(u, v))//u->v;
				{
					continue;
				}
				else//u-v改为v->u;
				{
					//改变u，v的出入度关系
					vertices_in[u].push_back(v);
					vertices_out[v].push_back(u);
					//color更新
					//color_Ins1(u, colorResult[v]);
					//color_del1(v, colorResult[u]);

					////color index 维护
					//for (vector<int>::iterator iter = vertices_in[u].begin(); iter != vertices_in[u].end(); iter++)
					//{
					//	int k = *iter;
					//	if (colorResult[k] == vertices_in[u].size() && k != v)
					//	{
					//		color_Ins1(u, colorResult[k]);
					//	}
					//}
					vertices_in[v].erase(std::remove(vertices_in[v].begin(), vertices_in[v].end(), u), vertices_in[v].end());
					tempset.insert(v);
					newEdgeMap[u].insert(v);
					if (colorResult[u]<colorResult[v]&&pqset.find(v) == pqset.end())
					{
						q.push(v);
						pqset.insert(v);
					}
				}
			}
			for (set<int>::iterator setIter = tempset.begin(); setIter != tempset.end(); setIter++)
			{
				vertices_out[u].erase(std::remove(vertices_out[u].begin(), vertices_out[u].end(), *setIter), vertices_out[u].end());
			}
		}
	}
	end = GetTickCount();
	//	cout << endl << "出入度改变 run time(ms):" << double(end - start) << endl;

	start = GetTickCount();
	//CAN方法的内容，priority_queue那块有点问题，迫于无奈，只能合在一个方法里了
	int oldcolor;
	int newcolor;
	int NotSame = 0;
	int Same = 0;
	int tempi = 0;
	while (!q.empty())
	{
		boolean flag = false;
		int u = q.top();
		q.pop();
		pqset.erase(u);
		oldcolor = colorResult[u];
		//std::cout << u << "oldcolor" << oldcolor << " ";
		//用最原始的方法来找颜色
		vector<bool> avail = vector<bool>(vertices_in[u].size() + 1);
		for (int i = 0; i<vertices_in[u].size(); i++)
		{
			int tempcolor = colorResult[vertices_in[u][i]];
			if (tempcolor <= vertices_in[u].size())
			{
				avail[tempcolor] = true;
			}
		}
		for (int i = 0; i < avail.size(); i++)
		{
			if (!avail[i])
			{
				newcolor = i;
				break;
			}
		}
		if (newcolor != oldcolor)
		{
			NotSame++;
			flag = true;
			colorResult[u] = newcolor;
		   // std::cout << u << "newcolor" << newcolor << endl;
		}
		else
		{
			//std::cout << "color not change" << endl;
			Same++;
		}
		/*
		if (!CandidateVector[u].empty())
		{
		newcolor = CandidateVector[u].front();
		colorResult[u] = newcolor;
		pop_heap(CandidateVector[u].begin(), CandidateVector[u].end(), greater<int>());
		CandidateVector[u].pop_back();
		flag = true;
		}
		else
		{
		for (int i = 0; i != cntVector[u].size(); i++)
		{
		if (cntVector[u][i] == 0)
		{
		if (colorResult[u] == i)
		{
		break;
		}
		else
		{
		flag = true;
		colorResult[u] = i;
		newcolor = i;
		break;
		}
		}
		}
		}*/
		if (flag)
		{
			for (vector<int>::iterator iter = vertices_out[u].begin(); iter != vertices_out[u].end(); iter++)
			{
				if (newEdgeMap[u].find(*iter)!= newEdgeMap[u].end())
				{
					continue;
					tempi++;
				}
				//color_del1(*iter, oldcolor);
				//color_Ins1(*iter, newcolor);
				if ((pqset.find(*iter) == pqset.end())
					&& (colorResult[u] == colorResult[*iter]
						|| oldcolor < colorResult[*iter]))
				{
					pqset.insert(*iter);
					q.push(*iter);
				}
			}
		}
		//CandidateVector[u].clear();
	}
	cout << "NotSame" << NotSame << " ";
	cout << "Same" << Same<<endl; 
		cout << tempi << endl;
	end = GetTickCount();
	cout << endl << "CAN run time(ms):" << double(end - start) << endl;
	cout << endl;
	//for (int i = 0; i<V; i++)
	//{
	//	cout << colorResult[i] << " ";
	//}
	//cout << endl;
}
void Graph::Create_Deg_Index(int u,int v)
{
	int changenum = degrees[v] - degrees[u];
	if (changenum == 0 && u>v)
	{
		changenum++;
	}
	else if (changenum == 0 && u < v)
	{
		changenum--;
	}
	else if (changenum > 0 && u > v)
	{
		changenum++;
	}
	else if (changenum < 0 && u < v)
	{
		changenum--;
	}
	map<int, set<int>> mymap;
	if (changenum>0)
	{
		mymap = PosDegIndex[u];
	}
	else
	{
		mymap = NegDegIndex[u];
	}
	if (mymap.find(changenum) != mymap.end())
	{
		set<int> tempset = (set<int>)(mymap.find(changenum)->second);
		tempset.insert(v);
		mymap[changenum] = tempset;
	}
	else
	{
		set<int> tempset = { v };
		mymap[changenum] = tempset;
	}
	if (changenum>0)
	{
		PosDegIndex[u] = mymap;
	}
	else
	{
		NegDegIndex[u] = mymap;
	}
}
bool Graph::IsBefore(int u, int v)
{
	if (degrees[u] > degrees[v] || (degrees[u] == degrees[v] && u < v))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//创建度的更改引起的出入度的更改。
void Graph::Create_Deg_Index()
{
	NegDegIndex = vector<map<int, set<int>>>(this->V);
	PosDegIndex = vector<map<int, set<int>>>(this->V);
	for (int i = 0; i < this->V; i++)
	{
		int u = i;
		vector<int> adjVex = vertices[i];
		for (vector<int>::iterator iter = adjVex.begin(); iter != adjVex.end(); iter++)
		{
			int v = *iter;
			Create_Deg_Index(u, v);
		}
	}
}



