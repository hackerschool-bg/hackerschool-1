#include <iostream>
using namespace std;

// shorthand for checking if coordinates are inside the limits
bool isInside(int n, int x, int y) {
	return (x >= 0 && x<n && y >= 0 && y<n);
}

// shorthand for checking if coordinates x,y need to (and can) be walked
bool check(bool** floorMap, bool** counted, int n, int x, int y) {
	return isInside(n,x,y) && !counted[x][y] && floorMap[x][y];
}

// recursive function for walking an island of boxes
void walk(bool** floorMap, bool** counted, int n, int x, int y) {
	counted[x][y] = true;
	if (!floorMap[x][y]) return;
	if (check(floorMap,counted,n,x-1,y)) walk(floorMap,counted,n,x-1,y);
	if (check(floorMap,counted,n,x+1,y)) walk(floorMap,counted,n,x+1,y);
	if (check(floorMap,counted,n,x,y-1)) walk(floorMap,counted,n,x,y-1);
	if (check(floorMap,counted,n,x,y+1)) walk(floorMap,counted,n,x,y+1);
}

int main() {
	// data input
	int n;
	cin >> n;
	if (n < 1 || n > 100) {
		cout << "Wrong!" << endl;
		return 1;
	}

	bool** floorMap = new bool*[n];
	bool** counted = new bool*[n];
	int t;
	for (int i=0;i<n;i++) {
		floorMap[i] = new bool[n];
		counted[i] = new bool[n];
		for (int j=0;j<n;j++) {
			cin >> t;
			if (t == 0) {
				floorMap[i][j] = false;
			} else if (t == 1) {
				floorMap[i][j] = true;
			} else {
				cout << "Wrong!" << endl;
				return 1;
			}
			counted[i][j] = false;
		}
	}

	// for each taken and unvisited square, walk all connected taken squares, mark them as visited
	// and finally increment the counter (we've walked one 'island')
	int islands = 0;
	for (int i=0;i<n;i++) {
		for (int j=0;j<n;j++) {
			if (floorMap[i][j] && !counted[i][j]) {
				walk(floorMap,counted,n,i,j);
				++islands;
			}
		}
	}

	// count the number of completely free rows and columns (this nested loop can be merged with the
	// the one that walks islands, but this way it's clearer what going on
	int paths=0;
	bool isPathX,isPathY;
	for (int i=0;i<n;i++) {
		isPathX = true;
		isPathY = true;
		for (int j=0;j<n && (isPathX || isPathY);j++) {
			if (floorMap[i][j]) {
				isPathX = false;
			}
			if (floorMap[j][i]) {
				isPathY = false;
			}
		}
		if (isPathX) ++paths;
		if (isPathY) ++paths;
	}

	// output answer
	cout << islands << " " << paths;

	return 0;
}
