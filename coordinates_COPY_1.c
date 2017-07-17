#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
Manera de revisar errores en el codigo
copilar como: 
gcc file.c -g file.x
gdb ./file.x
...
>>> run
*/


int main(){
    
    FILE *in = fopen("coordinates.csv","r");
    char buffer[1024];
    char *record,*line;
    int i=0,j=0;
    float datos[10][7];
    FILE *fp;
    
    
    if (in == NULL){
        printf("Error en la lectura del archivo");
        return -1;
    }
    
    // test fprintf: fp = fopen("test.txt","w+");
    
    while ((line = fgets(buffer,sizeof(buffer),in))!=NULL){
        
        record = strtok(line,",");
        
        while (record != NULL){
            
            printf("\nLa variable en la posición %d , %d es: %s\n",i,j,record);
            
            if (j!=0){
                
                datos[i][j] = atof(record);
                
                // test fprintf: if (j!=7){
                    
                    // test fprintf: fprintf(fp,"%f %s",datos[i][j],",");
                // test fprintf: }
                // test fprintf: else {
                    // test fprintf: fprintf(fp,"%f \n",datos[i][j]);
                // test fprintf: }
                            
            }
            
            record = strtok(NULL,",");
            ++j;
        }
        j=0;
        ++i;
    }
    // test fprintf: fclose(fp);
    
    
    
    return 0;
}