#include "Map.h"

using namespace std;

//демонстрация методов
int main() {
	roadNetwork map;

	map.addSettlement("Town1", 1000);
	map.addSettlement("Town2", 2000);
	map.addSettlement("Town3", 3000);
	map.addSettlement("Town4", 4000);
	map.addSettlement("Town5", 5000);
	map.addSettlement("Town6", 6000);
	map.addSettlement("Town7", 7000);

	map.addRoad("Town1", "Town2", 5.0);
	map.addRoad("Town1", "Town3", 10.0);
	map.addRoad("Town2", "Town4", 15.0);
	map.addRoad("Town3", "Town5", 20.0);
	map.addRoad("Town4", "Town5", 25.0);
	map.addRoad("Town5", "Town1", 25.0);
	map.addRoad("Town3", "Town2", 30.0);
	map.addRoad("Town4", "Town2", 15.0);
	map.addRoad("Town7", "Town1", 15.0);

	map.print();
	cout << endl;

	map.deleteSettlement("Town2");
	map.changeSettlement("Town3", "Town8");

	map.print();
	cout << endl;

	map.depthRec("Town1", printName);
	cout << endl;
	map.depth("Town1", printName);
	cout << endl;
	map.width("Town1", printName);
	cout << endl;

	{
		auto res = map.Dijkstra("Town7", "Town5");
		cout << endl << res.second << endl;
		for (auto c : res.first)
			cout << c.name << " ";
	}

	{
		auto res = map.BellmanFord("Town7", "Town5");
		cout << endl << res.second << endl;
		for (auto c : res.first)
			cout << c.name << " ";
	}

}




//проверка отрицательного цикла
/*int main() {
	roadNetwork map;

	map.addSettlement("Town1", 1000);
	map.addSettlement("Town2", 2000);
	map.addSettlement("Town3", 3000);
	map.addSettlement("Town4", 4000);
	map.addSettlement("Town5", 5000);
	map.addSettlement("Town6", 6000);
	map.addSettlement("Town7", 7000);

	map.addRoad("Town1", "Town2", -5.0);
	map.addRoad("Town2", "Town4", 15.0);
	map.addRoad("Town3", "Town5", 20.0);
	map.addRoad("Town4", "Town5", 25.0);
	map.addRoad("Town5", "Town1", 25.0);
	map.addRoad("Town2", "Town3", -30.0);
	map.addRoad("Town3", "Town1", -10.0);
	map.addRoad("Town4", "Town2", 15.0);
	map.addRoad("Town7", "Town1", 15.0);

	map.print();
	cout << endl;
	{
		auto res = map.BellmanFord("Town7", "Town5");
		cout << endl << res.second << endl;
		for (auto c : res.first)
			cout << c.name << " ";
	}
}*/