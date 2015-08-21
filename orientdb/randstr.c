#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N_RECORD 100000

void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

void randTeacher() {
        int i, j=0;
        FILE *fp;
        fp = fopen("/data/OrientDB/teacher.txt","a");
        printf("generating string...\n");
      	for (i = 0; i < N_RECORD; i++){
        	for (j = 0; j < 19999; j++){
                fprintf(fp, "%c", rand()%26 + 97);
                	//ch = rand() % 26 + 97;
                	//append(str,ch);
        	}
            fprintf(fp, "\n");
            //str[19999]='\0';
            //printf("%s\n",str);
            printf("#%d\n",i);
            //fprintf(fp,"%s\n",str);
            //str[0] = '\0';
        }
        printf("random string teacher finished\n");
        fclose(fp);
}
void randStudent() {
        int i, j=0;
        FILE *fp;
        fp = fopen("/data/OrientDB/student.txt","a");
        printf("generating string...\n");
        for (i = 0; i < N_RECORD; i++){
                for (j = 0; j < 19999; j++){
                    fprintf(fp, "%c", rand()%26 + 97);
                        //ch = rand() % 26 + 97;
                        //append(str,ch);
                }
            fprintf(fp, "\n");
            //str[19999]='\0';
            //printf("%s\n",str);
            printf("#%d\n",i);
            //fprintf(fp,"%s\n",str);
            //str[0] = '\0';
        }
        printf("random string student finished\n");
        fclose(fp);

}
void randTeach() {
    int i;
    FILE *fp;
    fp = fopen("/data/OrientDB/edge-100k.txt","a");
    printf("generating edge_number...\n");
    for (i = 0; i < N_RECORD; i++){
        //sprintf(num1,"%d",rand()%1000000);
        //sprintf(num2,"%d",rand()%1000000);
        //num2[strlen(num2)]='\0';
        //printf("%s:%s\n",num1,num2);
        printf("#%d\n",i);
        fprintf(fp,"%07d:%07d\n",rand()%N_RECORD,rand()%N_RECORD);
    }
    printf("random edge_number finished\n");
    fclose(fp);
}

int main(){

    srand(time(NULL));
    //randTeacher();
    //randStudent();
    randTeach();

	return 0;
}
