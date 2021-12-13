#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * itoa (int n){
    int c = 0;
    int stock = n;

    while(n/10!=0){
        c++;
        n=n/10;
    }
    n = stock;
    char *str = malloc((c+2) *sizeof(char));
    memset(str,0,c+2);
    for(int i=0; i<c+1; i++){
        str[(c - i)] = n%10 + '0';
        n = n/10;
    }
    return str;
}

int main(int argc, char const *argv[]){
    if(argc < 2) return -1;
    printf("%s/n", itoa(atoi(argv[1])));
    return 0;
}