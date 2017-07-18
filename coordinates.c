#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
Manera de revisar errores en el codigo
copilar como: 
gcc file.c -g file.x
gdb ./file.x
...
>>> run
*/

//masa en kilogramos
//
int main(){
    
    FILE *in = fopen("coordinates.csv","r");
    char buffer[1024];
    char *record,*line;
    int i=0,j=0;
    float datos[10][7];
    FILE *fp;
    float h = 1;
    float masa_solar = 1.98 * (pow(10.0,30.0));
    /*Constante gravitacionale en (Newton*(UA^2))/(Masas Solares)*/
    float G_cte = 1.1799 * (pow(10.0,28.0));
    
    
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
        masas[p] = (datos[p][0])/(masa_solar);
        /*El siguiente print se hace con el fin de verificar que los datos ingresados a el arreglo masas sean correctos*/
        /*###########################################################printf("\nLa masa del cuerpo %d es: %f\n",p,masas[p]); ###########################################################*/
    }
    
    /*Se ingresan los datos iniciales de posición y velocidad en la matriz mtz_almacen_temp*/
    
    int m=0; //va a indicar el cuerpo
    int n=0; //va a indicar el tipo de dato a ingresar
    
    for (m=0;m<10;m++){
        for (n=0;n<6;n++){
          mtz_almacen_temp[(9*m)+n][0] = datos[m][n+1];
          /*############################################################ printf("\nEl dato en la posición %d , 0 de la matriz mtz_almacen_temp es: %f\n",(9*m)+n,mtz_almacen_temp[(9*m)+n][0]);
          printf("El valor de n (data type) es %d y el valor de m (cuerpo) es %d",n,m); ##############################################################*/
        }        
    }
    
    /*Se calculan las aceleraciones iniciales de la mtz_almacen_temp*/
    /*En primera instancia se llena la matriz de fuerzas*/
    
    int px = 0;
    int py = 1;
    int pz = 2;
    int vx = 3;
    int vy = 4;
    int vz = 5;
    int ax = 6;
    int ay = 7;
    int az = 8;
    float norm_dif_x;
    float norm_dif_y;
    float norm_dif_z;
    float norm_tot;
    
    for (m=0;m<10;m++){
        /*El primer "for" va a recorrer la matriz "mtz_F a lo largo de sus 10 columnas*/ 
        for (n=0;n<30;n++){
            /*El segundo "for" va a recorrer la matriz "mtz_F" a lo largo de sus 30 filas*/
            
            /*Este primer if va a llenar las primeras 10 filas de mtz_F referentes a las componentes de las fuerzas en el eje x*/
            if (n>=0 && n<10){
                /*Este if garantiza que la fuerza graviotacional que ejerce un cuerpo sobre el mismo es 0*/
                if (n==m){
                    mtz_F[n][m] = 0;
                }
                if (n!=m){
                    norm_dif_x = mtz_almacen_temp[(9*m)+px][0]-mtz_almacen_temp[(9*n)+px][0];
                    
                    norm_dif_y =mtz_almacen_temp[(9*m)+py][0]-mtz_almacen_temp[(9*n)+py][0];
                    
                    norm_dif_z =mtz_almacen_temp[(9*m)+pz][0]-mtz_almacen_temp[(9*n)+pz][0];
                    
                    norm_tot = pow(((pow(norm_dif_x,2.0))+(pow(norm_dif_y,2.0))+(pow(norm_dif_z,2.0))),(0.5));
                    
                    mtz_F[n][m] = ((G_cte)*(masas[n]*masas[m])*(norm_dif_x))/(pow(norm_tot,3.0));
                    
                    /*################################################# printf("\nLa fuerza en la posición %d , %d es: %f\n",n,m,mtz_F[n][m]); #################################################*/
                }
                /*else{
                    printf("Error en el primer if de la inicialización de la matriz mtz_F\n");
                }*/
                
            }
            /*Este segundo if va a llenar las segundas 10 filas de mtz_F referentes a las componentes de las fuerzas en el eje y*/
            if (n>=10 && n<20){
                /*Este if garantiza que la fuerza graviotacional que ejerce un cuerpo sobre el mismo es 0*/
                if ((n-10)==m){
                    mtz_F[n][m] = 0;
                }
                if ((n-10)!=m){
                    norm_dif_x = mtz_almacen_temp[(9*m)+px][0]-mtz_almacen_temp[(9*(n-10))+px][0];
                    
                    norm_dif_y =mtz_almacen_temp[(9*m)+py][0]-mtz_almacen_temp[(9*(n-10))+py][0];
                    
                    norm_dif_z =mtz_almacen_temp[(9*m)+pz][0]-mtz_almacen_temp[(9*(n-10))+pz][0];
                    
                    norm_tot = pow(((pow(norm_dif_x,2.0))+(pow(norm_dif_y,2.0))+(pow(norm_dif_z,2.0))),(0.5));
                    
                    mtz_F[n][m] = ((G_cte)*(masas[(n-10)]*masas[m])*(norm_dif_x))/(pow(norm_tot,3.0));
                    
                    /*################################################# printf("\nLa fuerza en la posición %d , %d es: %f\n",n,m,mtz_F[n][m]); ##################################################*/
                }
                /*else {
                    printf("Error en el segundo if de la inicializacion de la matriz mtz_F\n");
                }*/
            }
            /*Este tercer if va a llenar las terceras 10 filas de mtz_F referentes a las componentes de las fuerzas en el eje z*/
            if (n>=20 && n<30){
                /*Este if garantiza que la fuerza graviotacional que ejerce un cuerpo sobre el mismo es 0*/
                if ((n-20)==m){
                    mtz_F[n][m] = 0;
                }
                if ((n-20)!=m){
                    norm_dif_x = mtz_almacen_temp[(9*m)+px][0]-mtz_almacen_temp[(9*(n-20))+px][0];
                    
                    norm_dif_y =mtz_almacen_temp[(9*m)+py][0]-mtz_almacen_temp[(9*(n-20))+py][0];
                    
                    norm_dif_z =mtz_almacen_temp[(9*m)+pz][0]-mtz_almacen_temp[(9*(n-20))+pz][0];
                    
                    norm_tot = pow(((pow(norm_dif_x,2.0))+(pow(norm_dif_y,2.0))+(pow(norm_dif_z,2.0))),(0.5));
                    
                    mtz_F[n][m] = ((G_cte)*(masas[(n-20)]*masas[m])*(norm_dif_x))/(pow(norm_tot,3.0));
                    
                    /*################################################# printf("\nLa fuerza en la posición %d , %d es: %f\n",n,m,mtz_F[n][m]); ###################################################*/
                }
                /*else {
                    printf("Error en el tercer if de la inicializacion de la matriz mtz_F\n");
                }*/
            }
        }
    }
    
    /*Ahora se procede a llenar el arreglo "array F"*/
    
    float suma;
    
    /*Este primer for va a recorrer las filas de mtz_F*/
    for (n=0;n<30;n++){
        suma = 0.0;
        /*Este segundo for va a recorrer las columnas de mtz_F con el fin de sumar sus componentes y hallar la fuerza que siente un cuerpo en determinado momento segundo los demas*/
        for (m=0;m<10;m++){
            suma = suma + mtz_F[n][m];
        }
        array_F[n] = suma;
        printf("\nLa fuerza total en la posición %d es: %f \n",n,array_F[n]);
    }
    
    /*Ahora si se pueden hallar las aceleraciones de cada cuerpo en cada componente*/
    
    for (){
        
    }
    
    return 0;
}