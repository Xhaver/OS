//this os a quine file<enter>
#include <stdio.h>
int main (void) 
{ char q='"'; puts("#include <stdio.h>"); char *x = "int main (void) { char q='%c'; puts(%c#include <stdio.h>%c); char *x = %c%s%c ; printf (x,q,q,q,q,x,q,q,q); puts(%c%c); return 0; }"; 
	printf (x,q,q,q,q,x,q,q,q); 
	puts(""); 
	return 0; 
}

