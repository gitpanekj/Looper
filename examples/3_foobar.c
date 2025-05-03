typedef unsigned int uint;

//Complexity: 4n
void foobar(uint n) {
	int x = n;
	int y = n;
	int p = 0;
	while(x > 0) {
        x--;
		int r = 0;
		while (y > 0) {
			y--;
			r++;
		}
		if (n == 54) {
			p = r;
		}
		while (p > 0) {
			p--;
		}
		p = r;
	}
}