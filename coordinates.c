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
    float h = 1;
    
    
    if (in == NULL){
        printf("Error en la lectura del archivo");
        return -1;
    }
    
    while ((line = fgets(buffer,sizeof(buffer),in))!=NULL){
        record = strtok(line,",");
        while (record != NULL){
            //printf("\nLa variable en la posición %d , %d es: %s\n",i,j,record);
            if (j!=0){
                datos[i][j-1] = atof(record);
            }
            record = strtok(NULL,",");
            ++j;
        }
        j=0;
        ++i;
    }
        
    /*Inicializo un arreglo de tipo float llamado: masas de tamaño [10] que contendrá las masas de los 10 cuerpos*/
    
    float masas[10];
    
    /*Inicializo una matriz de tipo float matriz: "mtz_almacen_temp" de tamaño [91]x[10]. La matriz mtz_almacen_temp contendrá en sus primeras 90 filas la información correspondiente a la posición, velocidad y aceleración de los 10 cuerpos en las 3 dimensiones espaciales de tal manera que se organizan de la siguiente manera:
    
    mtz_almacen_temp[0][:] = posic_x_Sol
    mtz_almacen_temp[1][:] = posic_y_Sol
    mtz_almacen_temp[2][:] = posic_z_Sol
    mtz_almacen_temp[3][:] = vel_x_Sol
    .
    .
    .
    mtz_almacen_temp[8][:] = acel_z_Sol
    mtz_almacen_temp[9][:] = posic_x_Mercurio
    .
    .
    .
    mtz_almacen_temp[88][:] = acel_y_Pluton
    mtz_almacen_temp[89][:] = acel_z_Pluton
    
    Por ultimo la fila final de la mtz_almacen_temp ([90]) contendrá la información referente al tiempo:
    
    mtz_almacen_temp[90][:] = tiempo
    
    Debido a la cantidad de datos se planea unicamente llenarla para 10 tiempos (10 columnas) y cada vez se acabe su espacio se almacenará la información en un archivo llamado: "info_planetas.txt"
    */
    
    float mtz_almacen_temp[91][10];
    
    /*Inicializo una matriz de tipo float llamada: "mtz_F" de tamaño [30]x[10]. Esta matriz es en realidad un grupo de 3 matrices [10]x[10] donde cada una representa las fuerzas experimentadas por los cuerpos en las componentes x,y & z. 
    Las filas representarian el cuerpo a analizar en una componente espacial determinada mientras la columna indicaría el cuerpo que está influyendo sobre el analizado.
    La matriz "mtz_F" estaría organizada de la siguiente manera:
    
    Columnas (Indica qué cuerpo está afectando al analizado):
    [0] = Sol
    [1] = Mercurio
    [2] = Venus
    .
    .
    .
    [8] = Neptuno
    [9] = Plutón
    
    Filas (Indica qué cuerpo está siendo analizado y en qué componente):
    [(10*0)+0] = F experimentada por Sol en eje x según [#] 
    [(10*0)+1] = F experimentada por Mercurio en eje x según [#] 
    [(10*0)+2] = F experimentada por Venus en eje x según [#] 
    .
    .
    .
    [(10*1)+0] = F experimentada por Sol en eje y según [#] 
    [(10*1)+1] = F experimentada por Mercurio en eje y según [#] 
    .
    .
    [(10*1)+9] = F experimentada por Plutón en eje y según [#] 
    [(10*2)+0] = F experimentada por Sol en eje z según [#] 
    .
    .
    .
    [(10*2)+7] = F experimentada por Urano en eje z según [#] 
    [(10*2)+8] = F experimentada por Neptuno en eje z según [#] 
    [(10*2)+9] = F experimentada por Plutón en eje z según [#]
    
    La matriz "mtz_F" se utilizaría de la siguiente manera:
    
    *** La fuerza experimentada por jupiter en la componente "y" según neptuno:
    Jupiter tiene la posición [5] en la lista de cuerpos
    Neptuno tiene la posición [8] en la lista de cuerpos
    ==> mtz_F[(10*1)+5][8]
    
    *** La fuerza experimentada por el Sol en la componente "z" según plutón:
    Sol tiene la posición [0] en la lista de cuerpos
    Plutón tiene la posición [9] en la lista de cuerpos
    ==> mtz_F[(10*2)+0][9]
    
    *** La fuerza "TOTAL" experimentada por la Tierra en la componente y:
    Tierra tiene la posición [3] en la lista de cuerpos
    Como en este caso se busca la fuerza total, esto es equivalente a la suma del aporte de cada cuerpo
    ==> mtz_F[(10*1)+3][0] + mtz_F[(10*1)+3][1]+mtz_F[(10*1)+3][2]+mtz_F[(10*1)+3][3]+mtz_F[(10*1)+3][4]+mtz_F[(10*1)+3][5]+mtz_F[(10*1)+3][6]+mtz_F[(10*1)+3][7]+mtz_F[(10*1)+3][8]+mtz_F[(10*1)+3][9]
    */
    
    float mtz_F[30][10];
    
    /*Inicializo un arreglo de tipo float llamado: "array_F" de tamaño [30] que contendrá las fuerzas experimentadas por cada cuerpo debido al efecto generado por la presencia de todos los demas en cada componente espacial para determinado tiempo t y se organizará de la siguiente manera:
    
    array_F[0] = fuerza_x_Sol
    array_F[1] = fuerza_y_Sol
    .
    .
    .
    array_F[16] = fuerza_y_Jupiter
    .
    .
    .
    array_F[28] = fuerza_y_Plutón
    array_F[29] = fuerza_y_Plutón
    
    Este arreglo resulta de sumar todos los elementos de cada fila de la matriz "mtz_F" generando así a "array_F"
    */
    
    float array_F[30];
    
    /*Se llena al arreglo "masas con los datos correspondientes*/
    
    int p=0;
    
    for (p=0;p<10;p++){
        masas[p] = datos[p][0];
        /*El siguiente print se hace con el fin de verificar que los datos ingresados a el arreglo masas sean correctos*/
        //printf("\nLa masa del cuerpo %d es: %f\n",p,masas[p]);
    }
    
    /*Se ingresan los datos iniciales de posición y velocidad en la matriz mtz_almacen_temp*/
    
    int m=0; //va a indicar el cuerpo
    int n=0; //va a indicar el tipo de dato a ingresar
    
    for (m=0;m<10;m++){
        for (n=0;n<6;n++){
          mtz_almacen_temp[(9*m)+n][0] = datos[m][n+1];
          printf("\nEl dato en la posición %d , 0 de la matriz mtz_almacen_temp es: %f\n",(9*m)+n,mtz_almacen_temp[(9*m)+n][0]);
          printf("El valor de n (data type) es %d y el valor de m (cuerpo) es %d",n,m);
        }        
    }
    
    /*Se calculan las aceleraciones iniciales de la mtz_almacen_temp*/
    
    for (m=0;m<10;m++){
        
    }
    
    return 0;
}