#include<stdio.h>

typedef int (*two_var_func) (int, int);
typedef int (*one_var_func) (int);

int add_int (int a, int b) {
    return a+b;
}

one_var_func partial (two_var_func f, int a) {
    int g (int b) {
        return f (a, b);
    }
    return g;
}
int (*fg(int (*ft)(int,int), int b)) (int d){
	int g(int c){
		return ft(c,b);
	}
   return g;
}
int main (void) {
    int a = 7;
    int b = 2;

    printf ("%lu, %d\n",sizeof(*fg), fg(add_int,a)(b));

}