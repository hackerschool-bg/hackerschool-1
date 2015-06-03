#include <iostream>
#include <cmath>
using namespace std;

int getSquaredDigits(double d)
{
	if (d == 1.0) return 1;
	return floor(2.0*log10(d))+1;
}

int main() {

	int n;
	cin >> n;
	double i=1;
	int iSquaredDigits;
	
	while (n > 0) {
		iSquaredDigits = getSquaredDigits(i);
		n-=iSquaredDigits;
		i+=1.0;
	}
	i-=1.0;
	long int squared = (long int)(i*i);
	int digit = squared%10;
	while (n<0) {
		squared /= 10;
		digit = squared%10;
		n++;
	}
	
	cout << digit << endl;

	return 0;
}
