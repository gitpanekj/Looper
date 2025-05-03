typedef unsigned int uint;

//Complexity: 2n
void xnu(uint n) {
	int beg = 0;
	int end = 0;
	int i = 0;
	while(i < n) {
		i++;
		if (n == 42) //L2
			end = i;
		// L3
		if (n == 24) { //L4
			int k = beg;
			while (k < end) {
				k++;
			}
			end = i;
			beg = end;
		} else {
			if(n == 54) {
				end = i;
				beg = end;
			}
		}
	}
}