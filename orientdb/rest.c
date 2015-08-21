#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

void getname(char *name, int line, char *path) {
    char buf[20000];
    int lines = 0;
    printf("line: %d\n",line);
    FILE *fp = fopen(path, "r");
    do
    {
        if (++lines == line) {
            fgets(buf, sizeof buf, fp);
            break;
        }
        
    }while((fscanf(fp, "%*[^\n]"), fscanf(fp, "%*c")) != EOF);
    if(lines == line){
        fclose(fp);
        sprintf(name,"%s",buf);
        //name[strlen(name)-1]='\0';
    }
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    char name[20001] = "";
    
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
        
        double avg=0;
        clock_t start,end,sum;
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.179.94:2480/command/test_data/sql/");
        curl_easy_setopt(curl,CURLOPT_USERNAME,"root");
        curl_easy_setopt(curl,CURLOPT_PASSWORD,"pimpat");

        /* Now specify the POST data */

        //  create class Teacher
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20class%20Teacher%20extends%20V");
        res = curl_easy_perform(curl);
        printf("\n");
        
        //  create class Student
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20class%20Student%20extends%20V");
        res = curl_easy_perform(curl);
        printf("\n");
        
        //  set property Teacher.name
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20property%20Teacher.name%20string");
        res = curl_easy_perform(curl);
        printf("\n");
        
        //  set property Student.name
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20property%20Student.name%20string");
        res = curl_easy_perform(curl);
        printf("\n=======================================\n");
    
        //  set 1000000 vertex Teacher
        FILE *fp;
        char postthis[20100] = "create%20vertex%20Teacher%20set%20name%20=%20'";
        int i=1;
        //srand(time(NULL));
        fp = fopen("/Users/pimpat/Desktop/teacher.txt","r");
        
        if (fp != NULL) {
            while (fgets(name, 20001, fp) != NULL) {
                name[strlen(name)-1] = '\0';
                //printf("name: %s\n",name);
                strcat(postthis,name);
                strcat(postthis,"'");
                //printf("postfield: %s\n",postthis);
            
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            
                start =clock();
                res = curl_easy_perform(curl);
                end=clock();
                sum = end-start;
                avg=avg+sum;
            
                printf("total time: %f\t#%d\n",(double)sum/CLOCKS_PER_SEC,i++);
                //if(i==101) printf("%s\n",chunk.memory);
                chunk.memory='\0';
                chunk.size='\0';
                postthis[46] = '\0';
                name[0] ='\0';
            }
        }
        fclose(fp);
        printf("\navg time: %f\n",avg/CLOCKS_PER_SEC/1000000);
        fp = fopen("/Users/pimpat/Desktop/result.txt","a");
        fprintf(fp,"create vertex(Teacher) = %f\n", avg/CLOCKS_PER_SEC/1000000);
        fclose(fp);
        sum=0;
        avg=0;
        i=1;
        printf("\n=======================================\n");
        
        //  set 1000000 vertex Student
        char postthis2[20100] = "create%20vertex%20Student%20set%20name%20=%20'";
        fp = fopen("/Users/pimpat/Desktop/student.txt","r");
        
        if (fp != NULL) {
            while (fgets(name, 20001, fp) != NULL) {
                name[strlen(name)-1] = '\0';
                //printf("name: %s\n",name);
                strcat(postthis2,name);
                strcat(postthis2,"'");
                //printf("postfield2: %s\n",postthis2);
            
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis2);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            
                start =clock();
                res = curl_easy_perform(curl);
                end=clock();
                sum = end-start;
                avg=avg+sum;
            
                printf("total time: %f\t#%d\n",(double)sum/CLOCKS_PER_SEC,i++);
                //if(i==101) printf("%s\n",chunk.memory);
                chunk.memory='\0';
                chunk.size='\0';
                postthis2[46] = '\0';
                name[0] ='\0';
            }
        }
        fclose(fp);
        printf("\navg time: %f\n",avg/CLOCKS_PER_SEC/1000000);
        fp = fopen("/Users/pimpat/Desktop/result.txt","a");
        fprintf(fp,"create vertex(Student) = %f\n",avg/CLOCKS_PER_SEC/1000000);
        fclose(fp);
        sum=0;
        avg=0;
        i=1;
        printf("\n=======================================\n");
        

        //  create class Teach
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20class%20Teach%20extends%20E");
        res = curl_easy_perform(curl);
        printf("\n");
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "create%20index%20UniqueTeach%20on%20Teach(out,in)%20unique");
        res = curl_easy_perform(curl);
        printf("\n");

        //  create 1000000 edge
        char postthis3[100] = "create%20edge%20Teach%20from%20#11:";
        char temp[6];
        char buff[20];
        char *edge_out, *edge_in;
        char delimitor[] = ":";
        fp = fopen("/Users/pimpat/Desktop/edge.txt","r");

        if (fp != NULL) {
            while (fgets(buff, 20, fp) != NULL) {
                edge_out = strtok(buff, delimitor);
                edge_in = strtok(NULL, delimitor);
                edge_in[strlen(edge_in)-1] = '\0';
                strcat(postthis3,edge_out);
                strcat(postthis3,"%20to%20#12:");
                strcat(postthis3,edge_in);
                //printf("postfield3: %s\n",postthis3);
                
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis3);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
                
                start =clock();
                res = curl_easy_perform(curl);
                end=clock();
                sum = end-start;
                avg=avg+sum;

                printf("total time: %f\t#%d\n",(double)sum/CLOCKS_PER_SEC,i++);
                //printf("%s\n",chunk.memory);
                chunk.memory='\0';
                chunk.size='\0';
                postthis3[35] = '\0';
            }
        }
        fclose(fp);
        printf("\navg time: %f\n",avg/CLOCKS_PER_SEC/1000000);
        fp = fopen("/Users/pimpat/Desktop/result.txt","a");
        fprintf(fp,"create edge(Teach) = %f\n",avg/CLOCKS_PER_SEC/1000000);
        fclose(fp);
        sum=0;
        avg=0;
        i=1;
        printf("\n=======================================\n");

        //  select data [test]
        char text[100] = "select%20name%20from%20#11:";
        //"select%20name%20from%20(select%20expand(out('Teach'))%20from%20Teacher%20where%20name%20='";
        
        int j;
        char num[10];
        srand(time(NULL));
        for(j=1;j<1000001;j++){
            sprintf(num,"%d",rand()%1000000);
            strcat(text,num);
            printf("text: %s\n",text);
            
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, text);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            
            start =clock();
            res = curl_easy_perform(curl);
            end=clock();
            sum = end-start;
            avg=avg+sum;
            
            printf("total time: %f\t#%d\n",(double)sum/CLOCKS_PER_SEC,j);
            //if(j==99) printf("%s\n",chunk.memory);
            chunk.memory='\0';
            chunk.size='\0';
            text[27] ='\0';
            num[0] ='\0';
        }
        printf("\navg time: %f\n",avg/CLOCKS_PER_SEC/1000000);
        fp = fopen("/Users/pimpat/Desktop/result.txt","a");
        fprintf(fp,"select(1) = %f\n",avg/CLOCKS_PER_SEC/1000000);
        fclose(fp);
        sum=0;
        avg=0;
        printf("\n=======================================\n");
        
//    /* Check for errors */
//      if(res != CURLE_OK){
//          fprintf(stderr, "curl_easy_perform() failed: %s\n",
//          curl_easy_strerror(res));
//      }

    /* always cleanup */
        curl_easy_cleanup(curl);
        if(chunk.memory)
            free(chunk.memory);
}
  curl_global_cleanup();
  return 0;
}
