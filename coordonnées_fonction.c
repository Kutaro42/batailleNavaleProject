#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


int main() {
    char buffer[5];
    int valide = 0;
    int x = 0, y = 0;
    int coord = 0;
    while (!valide) {
        printf("Entrez une coordonnée (x;y) : ");
        int coord = read(0, buffer, 5);
        if(sscanf(buffer, "%d;%d", &x, &y) != 2){
            printf("les coordonnées x et y doivent être des entiers");
        }
        else if(x < 1 || x > 50){
            printf("x doit être copris entre 1 et 50");
        }
        else if(y < 1 || y > 50){
            printf("y doit être copris entre 1 et 50");
        }
        else{
            valide = 1;
        }
        

    }
    int df = open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(df, buffer, coord);
}
