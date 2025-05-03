typedef unsigned int uint;

//Complexity: 2n
void foo(uint n) {
	int x = n;
	int r = n;
	while(x > 0) {
        x--;
		if(n == 42) {
			int p = r;
			while(p > 0) {
				p--;
			}
			r = 0;
		}
	}
}