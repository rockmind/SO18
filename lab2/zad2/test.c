#include<stdio.h>

//typedef int (*two_var_func) (int, int, int*);
//typedef int (*one_var_func) (int);
//
//one_var_func partial (two_var_func f, int a) {
//    int g (int b,int* c) {
//        return f (a, b, c);
//    }
//    return g;
//}

int add_int (int a, int b,int* c) {
    return a+b+*c;
}

int (*fpf())();/* funkcja 'fpf', która zwraca wskaźnik na funkcję, która zwraca dane typu int */
int main (void) {
    int a = 7;
    int b = 2;
	int t =5;
    int *c = &t;
    printf ("%lu, %d\n",sizeof(*fpf), (*fpf)(add_int,a)(b,c));
//    printf ("%d\n", *s);

}
int (*fpf(int (*ft)(), int c,int* k))(int,int*){
	int (*h)();
	int g(int c,int* k){
		return (*ft)(c,1,k);
	}
	h=g;
   return h;
}