#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void initialize_string (char *string, int n){
    int i;
    for (i=0; i<n/2; i++) {
        string[i] = 'A';
    }
    for (i=n/2; i<3*n/4; i++) {
        string[i] = 'C';
    }
    for (i=3*n/4; i<9*n/10; i++) {
        string[i] = 'G';
    }
    for (i=9*n/10; i<n; i++) {
        string[i] = 'T';
    }
}

int main (int argc, char *argv[]) {
    if (argc != 3){
        printf("Incorrect argument number\nsyntaxis should follow: program n L\n  program (program name)\n  n (string size)\n  L (character to look for) (A, C, G o T)\n");
        exit(1);
    }
 
    int i, n, count=0;
    char *string;
    char L;

    n = atoi (argv[1]);
    L = *argv[2];
 
    string = (char *) malloc (n*sizeof (char));
    initialize_string (string, n);
 
    for (i=0; i<n; i++){
      if (string[i] == L){
        count++;
      }
    }
 
    printf ("character %c appears %d times\n", L, count);
    free (string);
    exit (0);
}
