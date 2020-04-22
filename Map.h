#include "Graph.h"


using namespace std;


struct settlement {
	string name;
	size_t population;

	settlement(string name, size_t population) : name(name), population(population) {}

};

bool operator ==(const settlement& key, const settlement& key2) {
	if (key.name == key2.name) return true;
	else return false;
}

bool operator !=(const settlement& key, const settlement& key2) {
	return !(key == key2);
}

class hashSettlement {
public:
	hashSettlement() {}

	size_t operator () (const settlement& key) const {
		return hash<string>{}(key.name);
	}

};


//функция, необходимая для отладочной печати
void printSettlemant(unordered_map<settlement, unordered_set<edges<settlement, double>, hashEdges<settlement, double>>, hashSettlement>::iterator& it) {
	cout << it->first.name << ": " << it->first.population << "ppl. ";
	for (auto j = it->second.begin(); j != it->second.end(); ++j)
		cout << j->way << "km to " << j->to->name << "; ";
	cout << endl;
}

//вспомогательный класс под конкретное задание для более удобной работы с графом с использованием названий городов
class roadNetwork {

	Graph<settlement, double, hashSettlement> map;

public:

	void addSettlement(string name, size_t population) {
		settlement* s = new settlement(name, population);
		map.addVertex(*s);
	}

	void addRoad(string from, string to, double length) {
		auto f = settlement(from, 0);
		auto t = settlement(to, 0);
		map.addEdge(f, t, length);
	}

	void deleteSettlement(string name) {
		auto d = settlement(name, 0);
		map.deleteVertex(d);
	}

	void deleteRoad(string from, string to) {
		auto f = settlement(from, 0);
		auto t = settlement(to, 0);
		map.deleteEdge(f, t);
	}


	void changeSettlement(string name, string newName) {
		auto d = settlement(name, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			auto population = it.first->first.population;
			auto newVertex = new settlement(newName, population);
			map.changeVertex(d, *newVertex);
		}
	}

	void changeSettlement(string name, size_t newPopulation) {
		auto d = settlement(name, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			auto newVertex = new settlement(name, newPopulation);
			map.changeVertex(d, *newVertex);
		}
	}

	void changeSettlement(string name, string newName, size_t newPopulation) {
		auto d = settlement(name, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			auto newVertex = new settlement(newName, newPopulation);
			map.changeVertex(d, *newVertex);
		}
	}

	void print() {
		map.print(printSettlemant);
	}


	template <typename F>
	void depthRec(string start, F function) {
		auto d = settlement(start, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			map.depthRec(d, function);
		}
	}

	template <typename F>
	void depth(string start, F function) {
		auto d = settlement(start, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			map.depth(d, function);
		}
	}

	template <typename F>
	void width(string start, F function) {
		auto d = settlement(start, 0);
		auto it = map.getVertex(d);
		if (it.second) {
			map.width(d, function);
		}
	}

	pair<vector<settlement>, double> Dijkstra(string start, string end) {
		auto s = settlement(start, 0);
		auto e = settlement(end, 0);
		return map.Dijkstra(s, e);
	}

	pair<vector<settlement>, double> BellmanFord(string start, string end) {
		auto s = settlement(start, 0);
		auto e = settlement(end, 0);
		return map.BellmanFord(s, e);
	}
};

void printName(const settlement& v) {
	cout << v.name << " ";
}
