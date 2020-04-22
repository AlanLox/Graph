#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <string>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <limits>

using namespace std;

template <typename Vertex, typename Edges>
struct edges {
	Edges way;
	const Vertex* to;
	edges(Edges way, const Vertex* to) : way(way), to(to) {}

};

template <typename Vertex, typename Edges>
class hashEdges {
public:
	hashEdges() {}

	size_t operator () (const edges<Vertex, Edges>& key) const {
		return hash<const Vertex*>{}(key.to);
	}

};

template <typename Vertex, typename Edges>
bool operator ==(const edges<Vertex, Edges>& key, const edges<Vertex, Edges>& key2) {
	if (key.to == key2.to) return true;
	else return false;
}

template <typename Vertex, typename Edges>
bool operator !=(const edges<Vertex, Edges>& key, const edges<Vertex, Edges>& key2) {
	return !(key == key2);
}

//шаблонный класс графа
template <typename Vertex, typename Edges, typename Hash>
class Graph {

	unordered_map<Vertex, unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>, Hash> graph;

public:

	void addVertex(Vertex& v) {
		graph.insert(pair<Vertex, unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>>(move(v), unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>()));
	}

	void addEdge(Vertex& from, Vertex& to, Edges way) {
		auto it = graph.find(from);
		auto jt = graph.find(to);
		if ((it != graph.end()) && (jt != graph.end()))
			it->second.insert(edges<Vertex, Edges>(way, &graph.find(to)->first));
	}

	//здесь и далее предполагается, что numeric_limits<Edges>::max() определен для типа Edges, т.к. используется в алгоритмах поиска
	void deleteVertex(Vertex& v) {
		auto it = graph.find(v);
		if (it != graph.end()) {
			for (auto i = graph.begin(); i != graph.end(); ++i)
				i->second.erase(edges<Vertex, Edges>(numeric_limits<Edges>::max(), &it->first));
			graph.erase(it);
		}
	}

	void deleteEdge(Vertex& from, Vertex& to) {
		auto it = graph.find(from);
		auto it2 = graph.find(to);
		if (it != graph.end() && it2 != graph.end())
			it->second.erase(edges<Vertex, Edges>(numeric_limits<Edges>::max(), &it2->first));
	}

	//возвращает итератор вершины и найдена ли она
	pair<typename unordered_map<Vertex, unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>, Hash>::iterator, bool> getVertex(Vertex& v) {
		auto it = graph.find(v);
		return pair<typename unordered_map<Vertex, unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>, Hash>::iterator, bool>(it, it != graph.end());
	}

	//т.к. вершина является константным ключом, для изменения данных заменяем вершину на другую и восстанавливаем все рёбра
	void changeVertex(Vertex& changed, Vertex& newVertex) {
		auto it = graph.find(changed);
		auto tmpTo = it->second;
		vector<pair<const Vertex*, Edges>> tmpFrom;
		for (auto i = graph.begin(); i != graph.end(); ++i) {
			auto j = i->second.find(edges<Vertex, Edges>(numeric_limits<Edges>::max(), &it->first));
			if (j != i->second.end()) {
				auto p = j->way;
				tmpFrom.push_back(pair<const Vertex*, Edges>(&i->first, p));
				i->second.erase(j);
			}
		}
		graph.erase(it);
		auto j = graph.insert(pair<Vertex, unordered_set<edges<Vertex, Edges>, hashEdges<Vertex, Edges>>>(newVertex, tmpTo));
		for (auto i = tmpFrom.begin(); i != tmpFrom.end(); ++i) {
			graph.find(*i->first)->second.insert(edges<Vertex, Edges>(i->second, &j.first->first));
		}
	}

	//отладочная печать, принимает функцию печати одного узла с рёбрами
	template <typename F>
	void print(F printFunction) {
		for (auto it = graph.begin(); it != graph.end(); ++it) {
			printFunction(it);
		}
	}

private:
	//вспомогательная функция для рекурсивного обхода
	template <typename F>
	void rec(stack<const Vertex*>& stk, unordered_set<const Vertex*>& st, F function) {
		auto it = graph.find(*stk.top());
		auto it2 = it->second.begin();
		while (it2 != it->second.end()) {
			if (st.find(it2->to) == st.end()) {
				stk.push(it2->to);
				st.insert(it2->to);
				function(*stk.top());
				rec(stk, st, function);
				stk.pop();
			}
			++it2;
		}

	}

public:
	//рекурсивный обход в глубину
	template <typename F>
	void depthRec(Vertex& start, F function) {
		auto it = graph.find(start);
		stack<const Vertex*> stk;
		unordered_set<const Vertex*> st;

		if (it != graph.end()) {
			auto ptr = &it->first;
			stk.push(ptr);
			st.insert(ptr);
			function(*stk.top());
			rec(stk, st, function);
		}
	}

	//нерекурсивный обход в глубину
	//с английским беда
	template <typename F>
	void depth(Vertex start, F function) {
		auto it = graph.find(start);
		stack<const Vertex*> stk;
		unordered_set<const Vertex*> st;

		if (it != graph.end()) {
			stk.push(&it->first);
			st.insert(&it->first);
			function(*stk.top());

			while (!stk.empty()) {
				auto ptr = stk.top();
				auto it2 = graph.find(*ptr);
				auto it3 = it2->second.begin();
				while (it3 != it2->second.end() && st.find(it3->to) != st.end()) {
					++it3;
				}
				if (it3 == it2->second.end()) {
					stk.pop();
					continue;
				}
				stk.push(it3->to);
				st.insert(it3->to);
				function(*stk.top());
			}
		}
	}

	//обход в ширину
	template <typename F>
	void width(Vertex& start, F function) {
		auto it = graph.find(start);
		queue<const Vertex*> q, q2;
		queue<const Vertex*>* ptr1 = &q, * ptr2 = &q2;
		unordered_set<const Vertex*> st;
		if (it != graph.end()) {
			auto ptr = &it->first;
			q.push(ptr);
			st.insert(ptr);

			while (!ptr1->empty() || !ptr2->empty()) {
				while (!ptr1->empty()) {
					auto i = graph.find(*ptr1->front());
					function(*ptr1->front());
					auto it2 = i->second.begin();
					while (it2 != i->second.end()) {
						auto p = st.find(it2->to);
						if (p == st.end()) {
							st.insert(it2->to);
							ptr2->push(it2->to);
						}
						++it2;
					}
					ptr1->pop();
				}
				auto tmp = ptr2;
				ptr2 = ptr1;
				ptr1 = tmp;
			}
		}
	}

	//алгоритм Дейкстры
	//вместо поиска элемента с минимальным значением среди всех вершин, ищем среди тех, которые были просмотрены, но не были помечены
	pair<vector<Vertex>, Edges> Dijkstra(Vertex& start, Vertex& end) {
		vector<Vertex> result;
		result.insert(result.begin(), end);

		unordered_map<const Vertex*, pair<bool, Edges>> all;
		unordered_map <const Vertex*, const Vertex*> reverse;
		list<pair<const Vertex*, Edges>> selected;

		for (auto it = graph.begin(); it != graph.end(); ++it) {
			all.insert(pair<const Vertex*, pair<bool, Edges>>(&it->first, pair<bool, Edges>(false, numeric_limits<Edges>::max())));
		}

		auto first = all.find(&graph.find(start)->first);
		first->second.second = numeric_limits<Edges>::denorm_min();
		selected.push_back(pair<const Vertex*, Edges>(first->first, first->second.second));
		while (!selected.empty()) {
			auto selectedIt = selected.begin();
			for (auto currentIt = selected.begin(); currentIt != selected.end(); ++currentIt) {
				if (selectedIt->second > currentIt->second) selectedIt = currentIt;
			}
			auto gVertex = graph.find(*selectedIt->first);
			auto currentVertex = all.find(selectedIt->first);
			currentVertex->second.first = true;
			for (auto nextVertexIt = gVertex->second.begin(); nextVertexIt != gVertex->second.end(); ++nextVertexIt) {
				auto nextVertex = all.find(nextVertexIt->to);
				if (!nextVertex->second.first && nextVertex->second.second > currentVertex->second.second + nextVertexIt->way) {
					nextVertex->second.second = currentVertex->second.second + nextVertexIt->way;
					auto revIt = reverse.find(nextVertexIt->to);
					if (revIt != reverse.end()) revIt->second = (currentVertex->first);
					else reverse.insert(pair<const Vertex*, const Vertex*>(nextVertexIt->to, currentVertex->first));
				}
				if (!nextVertex->second.first)
					selected.push_back(pair<const Vertex*, Edges>(nextVertexIt->to, nextVertex->second.second));
			}
			selected.erase(selectedIt);
		}
		//возвращаем расстояние до нужной вершины и восстанавливаем путь
		auto resVertex = end;
		while (resVertex != start) {
			auto tmp = reverse.find(&graph.find(resVertex)->first);
			if (tmp == reverse.end()) {
				result.insert(result.begin(), start);
				break;
			}
			resVertex = *tmp->second;
			result.insert(result.begin(), resVertex);
		}
		return pair<vector<Vertex>, Edges>(result, all.find(&graph.find(end)->first)->second.second);
	}

	//алгоритм Беллмана-Форда
	pair<vector<Vertex>, Edges> BellmanFord(Vertex& start, Vertex& end) {
		vector<Vertex> result;
		result.insert(result.begin(), end);
		unordered_map<const Vertex*, Edges> all;
		unordered_map <const Vertex*, const Vertex*> reverse;

		for (auto it = graph.begin(); it != graph.end(); ++it) {
			all.insert(pair<const Vertex*, Edges>(&it->first, numeric_limits<Edges>::max()));
		}
		all.find(&graph.find(start)->first)->second = numeric_limits<Edges>::denorm_min();

		for (size_t i = 0; i < graph.size() - 1; ++i) {
			bool flag = false;
			for (auto selected = graph.begin(); selected != graph.end(); ++selected) {

				auto currentVertex = all.find(&selected->first);
				for (auto nextVertexIt = selected->second.begin(); nextVertexIt != selected->second.end(); ++nextVertexIt) {
					auto nextVertex = all.find(nextVertexIt->to);
					if (nextVertex->second > currentVertex->second + nextVertexIt->way) {
						nextVertex->second = currentVertex->second + nextVertexIt->way;
						auto revIt = reverse.find(nextVertexIt->to);
						if (revIt != reverse.end()) revIt->second = currentVertex->first;
						else reverse.insert(pair<const Vertex*, const Vertex*>(nextVertexIt->to, currentVertex->first));
						flag = true;
					}
				}
			}
			if (flag == false) break;
		}

		//проверка на отрицательный цикл
		unordered_set<const Vertex*> check;
		for (auto selected = graph.begin(); selected != graph.end(); ++selected) {
			auto currentVertex = all.find(&selected->first);
			for (auto nextVertexIt = selected->second.begin(); nextVertexIt != selected->second.end(); ++nextVertexIt) {
				auto nextVertex = all.find(nextVertexIt->to);
				if (nextVertex->second > currentVertex->second + nextVertexIt->way) {
					check.insert(nextVertex->first);

					nextVertex->second = currentVertex->second + nextVertexIt->way;
					auto revIt = reverse.find(nextVertexIt->to);
					if (revIt != reverse.end()) revIt->second = currentVertex->first;
					else reverse.insert(pair<const Vertex*, const Vertex*>(nextVertexIt->to, currentVertex->first));
				}
			}
		}
		//возвращаем расстояние до нужной вершины и восстанавливаем путь
		auto resVertex = end;
		while (resVertex != start) {
			auto tmp = reverse.find(&graph.find(resVertex)->first);
			if (check.find(&graph.find(resVertex)->first) != check.end())
				return pair<vector<Vertex>, Edges>(result, numeric_limits<Edges>::min());
			if (tmp == reverse.end()) {
				result.insert(result.begin(), start);
				break;
			}
			resVertex = *tmp->second;
			result.insert(result.begin(), resVertex);
		}
		return pair<vector<Vertex>, Edges>(result, all.find(&graph.find(end)->first)->second);
	}
};
