#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

int ordenar(const void *c1, const void *c2){
    const int *a = (const int *)c1;
    const int *b = (const int *)c2;
    if( a[1] < b[1]){
        return 0;
    }
    return 1;
}

int find_value(int *lista, int largo_lista, int valor){
    if (largo_lista != 0){
        for (int i = 0; i < largo_lista; i++){
            if (lista[i] == valor){
                return 1;
            }
        }
    }
    return 0;
    
}

int cant_examenes(char exam_file[]){
    FILE *fp;
    char c;
    int n_examenes = 0;

    fp = fopen(exam_file, "r");
    for(c = getc(fp); c != EOF; c = getc(fp)){
        if (c == '\n'){
            n_examenes++;
        }
    }
    fclose(fp);
    return n_examenes-1;
}

int **matriz_de_conflicto(int cant_exam, char student_file[]){
    int i,j;
    FILE *fp;
    char linea[100];
    char *datos;
    int posicion;
    char alumno_revisado[50];
    int pos = 0;
    bool primerValor = true;

    int **conflict_matrix = (int **)malloc(cant_exam*sizeof(int*));
    for (i = 0; i < cant_exam; i++){
        conflict_matrix[i] = (int*)malloc(cant_exam*sizeof(int));
        for (j = 0; j < cant_exam; j++){
            conflict_matrix[i][j] = 0;
        }
    }

    int exams[cant_exam];
    for (i = 0; i < cant_exam; i++){
        exams[i] = 0;
    }

    fp = fopen(student_file,"r");
    while(fgets(linea, sizeof(linea), fp) != NULL){
        char alumno_actual[50];
        char examen_actual[50];
        posicion = 0;

        strtok(linea,"\n");
        datos = strtok(linea, " ");
        while(datos != NULL){
            if (posicion == 0){
                strcpy(alumno_actual,datos);
                posicion++;
            }
            else{
                strcpy(examen_actual,datos);
            }
            datos = strtok(NULL, " ");
        }
        int x = atoi(examen_actual);
        if (primerValor){
            primerValor = false;
            strcpy(alumno_revisado, alumno_actual);
        }

        if (strcmp(alumno_actual,alumno_revisado) != 0){
            for (int i = 0; i < cant_exam; i++){
                if (exams[i] != 0){
                    for (int j = 0; j < cant_exam; j++){
                        if (exams[j] != 0){
                            conflict_matrix[exams[i]-1][exams[j]-1] = 1;
                        }
                    }
                }
            }
            for (i = 0; i < cant_exam; i++){
                exams[i] = 0;
            }
            strcpy(alumno_revisado, alumno_actual);
            pos = 0;
            exams[pos] = x;
            pos++;
        }
        else{
            exams[pos] = x;
            pos++;
        }
    }
    for (int i = 0; i < cant_exam; i++){
        if (exams[i] != 0){
            for (int j = 0; j < cant_exam; j++){
                if (exams[j] != 0){
                    conflict_matrix[exams[i]-1][exams[j]-1] = 1;
                }
            }
        }
    }
    fclose(fp);
    return(conflict_matrix);
}

int greedy(int **conflict_matrix, int cant_exam){

    int ultimoConflictos = -1;
    int largo_examenes = 0;
    int exams[cant_exam];
    int solucion[cant_exam][cant_exam];
    int timeSlotMax = 0;

    for (int test = 0; test < cant_exam; test++){
        ultimoConflictos = -1;
        largo_examenes = 0;
        int timeSlot = 0;
        int conflictos[cant_exam];
        for (int conflicto = 0; conflicto < cant_exam; conflicto++){
            if(conflict_matrix[test][conflicto] == 1){
                ultimoConflictos++;
                conflictos[ultimoConflictos] = conflicto;
            }
        }
        for (int conflicto = 0; conflicto < ultimoConflictos; conflicto++){
            int value = find_value(exams,largo_examenes,conflictos[conflicto]+1);
            if(value == 0){
                solucion[timeSlot][conflicto] = conflictos[conflicto] + 1;
                exams[conflicto] = conflictos[conflicto] + 1;
                largo_examenes++;
                timeSlot++;
            }
            if (timeSlot > timeSlotMax){
                timeSlotMax = timeSlot;
            }
        }
    }
    return timeSlotMax;
}

int HCFI(int conflict_list[][2], int **conflict_matrix, int cant_exam, char* archivo){
    int array_solucion[cant_exam][cant_exam];
    for (int i = 0; i < cant_exam; i++){
        array_solucion[i][0] = 0;
    }
    int largo_array = 1;

    int timeSlot;
    int conflict_list_size = cant_exam;
    int mas_conflictivo[] = {0,0};
    int timeSlotMax = 0;
    FILE *fp;

    while(conflict_list_size>1){
        timeSlot = 0;
        mas_conflictivo[0] = conflict_list[conflict_list_size][0];
        mas_conflictivo[1] = conflict_list[conflict_list_size][1];

        for (int a = 0; a < conflict_list_size; a++){
            bool agregar = true;
            for (int b = 0; b < largo_array; b++){
                //printf("largo array: %d\n", largo_array);
                if(array_solucion[a][b] != 0){
                    if(mas_conflictivo[0] > 0 && mas_conflictivo[0] < 1000){
                        if(conflict_matrix[(array_solucion[a][b])][mas_conflictivo[0]-1]){
                            agregar = false;
                            break;
                        }
                    }
                }
            }
            if (agregar){
                largo_array++;
                if(mas_conflictivo[0] > 0 && mas_conflictivo[0] < 1000){
                    //printf("mas conflictivo: %d\n", mas_conflictivo[0]);
                    array_solucion[a][largo_array] = mas_conflictivo[0]-1;
                }
                break;
            }
            else{
                timeSlot++;
            }
        }
        conflict_list_size--;
        if(timeSlot > timeSlotMax){
            timeSlotMax = timeSlot;
        }
    }

    mkdir("./Respuestas", 0700);
    char timeSlotsChar[10];
    char nombre_respuesta[100] = "./Respuestas/";
    strcat(nombre_respuesta,archivo);
    strcat(nombre_respuesta,".res");
    fp = fopen(nombre_respuesta, "w");
    sprintf(timeSlotsChar, "%d", timeSlotMax);
    fputs(timeSlotsChar, fp);
    fclose(fp);

    return timeSlotMax;
}

int main(){
    char nombre_archivo[50];
    char nombre_archivo_copia[50];
    printf("Ingrese el nombre del archivo a revisar (sin extension): ");
    scanf("%s", nombre_archivo);
    strcpy(nombre_archivo_copia,nombre_archivo);

    char exam_file[] = "./Instancias/";
    strcat(exam_file,nombre_archivo);
    strcat(exam_file,".exm");

    int cant_exam = cant_examenes(exam_file);

    char student_file[] = "./Instancias/";
    strcat(student_file,nombre_archivo);
    strcat(student_file,".stu");

    int **conflict_matrix = matriz_de_conflicto(cant_exam,student_file);

    int conflict_list[cant_exam][2];
    for (int a = 0; a < cant_exam; a++){
        int f = 0;
        int suma = 0;
        for (int b = 0; b < cant_exam; b++){
            if (conflict_matrix[a][b] == 1){
                suma++;
            }
        }
        conflict_list[a][f] = a+1;
        conflict_list[a][f+1] = suma;
    }

    qsort(conflict_list,cant_exam,sizeof(int*),ordenar);

    int timeSlotsIniciales = greedy(conflict_matrix, cant_exam);

    printf("Time slots iniciales: %d\n", timeSlotsIniciales);

    int timeSlotsFinales = HCFI(conflict_list, conflict_matrix, cant_exam, nombre_archivo_copia);

    printf("Time slots finales: %d\n", timeSlotsFinales);

    return 0;
}