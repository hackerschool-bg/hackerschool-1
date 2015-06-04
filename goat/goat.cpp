#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
using namespace std;

/*
6 2 26000 7000 10000 30000 5000 4000
6 2 4 8 15 16 23 42
15 3 666000 42000 7000 13000 400000 511000 600000 200000 202000 111000 313000 94000 280000 72000 42000
15 3 666 42 7 13 400 511 600 200 202 111 313 94 280 72 42
*/
int getInt(int min, int max, const char* errMsg) {
	int n;
	cin >> n;
	if (n >= min && n <= max) {
		return n;
	} else {
		cout << errMsg << endl;
		exit(1);
	}
}

int countSets(int capacity, vector<int>& goats, bool* takenArray) {
	int remainingCapacity = capacity;
	int totalGoats = goats.size();
	int remainingGoats = totalGoats;
	memset(takenArray,0,totalGoats*sizeof(bool));
	bool added = false;
	int sets = 0;
	while (remainingGoats > 0) {
		added = false;
		for (int i=0;i<totalGoats;i++) {
			if (takenArray[i]) continue;
			if (goats.at(i) <= remainingCapacity) {
				remainingCapacity -= goats.at(i);
				takenArray[i] = true;
				added = true;
				--remainingGoats;
				//cout << goats[i] << " ";
				break;
			}
		}
		if (!added || remainingGoats == 0) {
			++sets;
			remainingCapacity = capacity;
			//cout << "| ";
		}
	}
	//cout << endl;
	return sets;
}

int main(int argc, char** argv) {
	int n,k;
	n = getInt(1,1000,"Wrong N");
	k = getInt(1,1000,"Wrong K");
	vector<int> goats;
	int sum = 0, goat;
	for (int i=0;i<n;i++) {
		goat = getInt(1,1000000,"Wrong goat");
		goats.push_back(goat);
		sum += goat;
	}
	sort(goats.begin(),goats.end());
	reverse(goats.begin(),goats.end());
	int maxGoat = goats[0];
	bool* takenArray = new bool[n];
	bool found = false;
	for (int testCapacity=maxGoat;testCapacity<=sum;testCapacity++) {
		if (countSets(testCapacity,goats,takenArray) == k) {
			cout << testCapacity << endl;
			found = true;
			break;
		}
	}
	delete [] takenArray;
	if (!found) {
		cout << "Impossible" << endl;
	}
	return 0;
}
