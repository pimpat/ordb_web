#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#include "reqmsg.h"
#include "swap_endian.h"

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 2424

//#define N_RECORD (10000)
//#define N_RECORD (11)
#define NAME_SIZE (19999)
#define SAMPLING_TIME (1000)

//#define DB_NAME "test-10k"
//#define TEACHER_FILE "/data/OrientDB/teacher.txt"
//#define STUDENT_FILE "/data/OrientDB/student.txt"
//#define EDGE_FILE "/data/OrientDB/edge-100k.txt"

#define DB_NAME "test-new"
#define DIFF_FILE "/Users/pimpat/Desktop/result_diff.txt"

typedef struct Packet Packet;
struct Packet {
    char opType;
    char ssid[4];
    char msg[NAME_SIZE*2];
} PacketDesc;

Packet GPacket;

typedef struct Addr Addr;
struct Addr{
    short cltid;
    long rid;
};

Addr addr;

int Sockfd;
int createVertex(char *command);
int prepareDB();
int connectSocket();
int sendCommand(char *command);
//int testCreateVertex(char *vertex, char *file, int nRecord);
//int testCreateEdge();
int connectServer();
int disconnectServer();
int openDatabase(char *dbName);
int createDatabase(char *dbName);
int createClass(char *class, char *extend);
int loadRecord(short clusterId, long clusterPos);
int queryRecord(char *query);

int main() {
    int ret;
    int size;

//    prepareDB();  // include connectServer()

    Sockfd = connectSocket();
    if (Sockfd < 0) return Sockfd;

    ret = openDatabase(DB_NAME);
    if (ret!=0) {
        printf ("error openDB\n");
        return 1;
    }
    
    ret = createVertex("create vertex Diff set key='newkey', name='newname'");
    ret = createVertex("create vertex Diff set key='newkey', name='newname2'");
    ret = createVertex("create vertex Diff set key='newkey', name='newname3'");
//    ret = queryRecord("traverse * from #11:42");
//    //ret = queryRecord("select classes[name='Testtest'].defaultClusterId from 0:1");
    if (ret!=0) printf ("error\n");
    
    //ret = queryRecord("select expand(in_Link) from Diff limit -1");
    //ret = queryRecord("select @rid from Diff where key='555'");
    //ret = queryRecord("select * from (select expand(in('Link')) from Diff where key ='123')");
    //ret = queryRecord("select * from Diff");
    //ret = queryRecord("select * from Diff where key='0'");
    //ret = sendCommand("delete vertex #11:67");
    //ret = sendCommand("update Diff set key='555', value='666' where key='100'");
    //ret = sendCommand("create edge Link from #11:26 to #11:28");
    //loadRecord(11, 65);
    //connectServer();
    
//    FILE *fp;
//    fp = fopen("/Users/pimpat/Desktop/result_diff.txt","r");
//    char buff[200], temp[200];
//    char postthis[1000] = "CREATE VERTEX Diff set key='";
//    printf("str: %lu\n",strlen(postthis));
//    int i=0;
//    char num[5];
//    
//    if (fp != NULL) {
//        while (fgets(buff, 200, fp) != NULL) {
//            if((i%5)==0){
//                buff[strlen(buff)-1] = '\0';
//                sprintf(num,"%d",i);
//                strcat(postthis,num);
//                strcat(postthis,"', value='");
//                strcat(temp,buff);
//                strcat(postthis,temp);
//                strcat(postthis,"'");
//                printf("postthis: %s\n",postthis);
//                sendCommand(postthis);  // binary
//                temp[0]='\0';
//                postthis[28]='\0';
//                i++;
//            }
//            else {
//                if(buff[0]=='#') {
//                    temp[strlen(temp)-1]='\0';
//                    sprintf(num,"%d",i);
//                    strcat(postthis,num);
//                    strcat(postthis,"', value='");
//                    strcat(postthis,temp);
//                    strcat(postthis,"'");
//                    printf("postthis: %s\n",postthis);
//                    sendCommand(postthis);  // binary
//                    temp[0]='\0';
//                    postthis[28]='\0';
//                    i++;
//                }
//                else
//                    strcat(temp,buff);
//            }
//        }
//    }
//    fclose(fp);
//    printf("\n=======================================\n");
    
//    sendCommand("CREATE EDGE SubDiff from #11:1 to #11:12");
//    sendCommand("CREATE VERTEX Diff set key='7', name='seven'");
//    sendCommand("CREATE VERTEX Diff set key='8', name='eight'");
//    sendCommand("CREATE VERTEX Diff set key='9', name='nine'");
    
    //testCreateVertex("Diff", DIFF_FILE, N_RECORD);
    //testCreateVertex("Teacher", TEACHER_FILE, N_RECORD);
    //testCreateVertex("Student", STUDENT_FILE, N_RECORD);
    //testCreateEdge();

    //loadRecord(11, 0);

    disconnectServer();
    close(Sockfd);

    return 0;
}

int prepareDB() {
    int ret, i;

    Sockfd = connectSocket();
    if (Sockfd < 0) return Sockfd;
    
    ret = connectServer();
    if (ret!=0) return 1;
    
    ret = createDatabase(DB_NAME);
    if (ret!=0) return 1;

    createClass("Diff_tmp", "V");
    if (ret!=0) return 1;

//    ret = sendCommand("CREATE PROPERTY Diff.key STRING");
//    if (ret!=0) return 1;
//    printf("CREATE PROPERTY Diff.key STRING: OK!\n");
//    
//    ret = sendCommand("CREATE PROPERTY Diff.value STRING");
//    if (ret!=0) return 1;
//    printf("CREATE PROPERTY Diff.value STRING: OK!\n");
//    
//    ret = sendCommand("ALTER DATABASE CUSTOM useLightweightEdges=false");
//    if (ret!=0) return 1;
//    printf("ALTER DATABASE CUSTOM useLightweightEdges=false: OK!\n");
    
    disconnectServer();
    close(Sockfd);

    return 0;
}

int connectSocket() {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if ((sockfd) < 0) {
        printf("Error: Could net create socket\n");
        return sockfd;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("inet_pton error occured\n");
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error: Connect Failed\n");
        return -1;
    }
    read(sockfd, &GPacket, 256);
    
    printf("Connect Socket OK\n");

    return sockfd;
}

int sendCommand(char *command) {
    int ret, size;
    GPacket.opType = REQUEST_COMMAND;
    size = reqCommandMsg(GPacket.msg, "c", command);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+256);
    if (ret<0 || GPacket.opType!=0) 
        return 1;
    else 
        return 0;
}

int connectServer() {
    int ret, size;
    GPacket.opType = REQUEST_CONNECT;
    *(int *)GPacket.ssid = -1;
    //size = reqConnectMsg(GPacket.msg, NULL, NULL, 21, NULL, "root", "pimpat");
    size = reqConnectMsg(GPacket.msg, NULL, NULL, 28, NULL, "root", "pimpat");
    ret = write(Sockfd, &GPacket, 5+size);
    ret = read(Sockfd, &GPacket, 5+sizeof(int));
    if (ret<0 || GPacket.opType!=0) return 1;

    //  copy ssid
    memcpy(GPacket.ssid, GPacket.msg, 4);
    int ssid = *(int *)GPacket.ssid;
    swapEndian(&ssid, INT);
    printf("ssid: %d\n", ssid);
    
    //  for version 28
    int token;
    ret = read(Sockfd, &token, 4);

    return 0;
}
int disconnectServer() {
    int ret, size;
    GPacket.opType = 5;
    size = reqDBCloseMsg(GPacket.msg);
    ret = write(Sockfd, &GPacket, 5+size);
    return 0;
}

int openDatabase(char *dbName) {
    printf("Opening Database...\n");
    int ret, size, i;
    GPacket.opType = REQUEST_DB_OPEN;
    *(int *)GPacket.ssid = -1;
    //size = reqDBOpenMsg(GPacket.msg, NULL, NULL, 21, NULL, dbName, "graph", "root", "pimpat");
    size = reqDBOpenMsg(GPacket.msg, NULL, NULL, 28, NULL, dbName, "graph", "root", "pimpat");
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+sizeof(int));
    printf("resSize: %d\n", ret);
    printf("resCode: %d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) return 1;

    //  copy ssid
    memcpy(GPacket.ssid, GPacket.msg, 4);
    int ssid = *(int *)GPacket.ssid;
    swapEndian(&ssid, INT);
    printf("ssid: %d\n", ssid);

    //  for version 28
    int token;
    ret = read(Sockfd, &token, 4);
    
    //  copy num-of-clusters
    short nCluster;
    ret = read(Sockfd, &nCluster, sizeof(short));
    swapEndian(&nCluster, SHORT);
    printf("num-of-clusters: %hd\n", nCluster);

    for (i=0; i<nCluster; i++) {
        char name[128];
        char type[32];
        short id, dataSeg;
        
        //  read info classes
        read(Sockfd, &size, sizeof(int));
        swapEndian(&size, INT);
        read(Sockfd, name, size);
        name[size] = '\0';
        read(Sockfd, &id, sizeof(short));
        swapEndian(&id, SHORT);
        
//        read(Sockfd, &size, sizeof(int));
//        swapEndian(&size, INT);
//        read(Sockfd, type, size);
//        type[size] = '\0';
//        read(Sockfd, &dataSeg, sizeof(short));
//        swapEndian(&dataSeg, SHORT);
//        printf("name:'%s'\t id:%hd\t type:%s\t dataSeg:%hd\n", name, id, type, dataSeg);
        
        //  for version 28
        printf("name:'%s'\t id:%hd\n", name, id);
    }
    
    read(Sockfd, &size, sizeof(int));
    swapEndian(&size, INT);
    if (size > 0) {
        char clusterConfig[size];
        read(Sockfd, clusterConfig, size);
    }
    
    read(Sockfd, &size, sizeof(int));
    swapEndian(&size, INT);
    if (size > 0) {
        char orientRelease[size+1];
        read(Sockfd, orientRelease, size);
        orientRelease[size] = '\0';
        printf("%s\n", orientRelease);
    }
    return 0;
}

int createDatabase(char *dbName) {
    int ret, size;
    GPacket.opType = REQUEST_DB_CREATE;
    size = reqDBCreateMsg(GPacket.msg, dbName, "graph", "plocal");
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+0);
    if (ret<0 || GPacket.opType!=0) return 1;
    
    printf("CREATE DATABASE '%s' plocal: OK!\n", dbName);
    return 0;
}

int loadRecord(short clusterId, long clusterPos) {
    int ret, size;
    GPacket.opType = REQUEST_RECORD_LOAD;
    size = reqRecLoadMsg(GPacket.msg, clusterId, clusterPos);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5);
    //printf("ret:%d\n", ret);
    printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) return 1;
    
    char payloadStatus;
    read(Sockfd, &payloadStatus, sizeof(char));
    printf("status:%hhu\n", payloadStatus);
    
    char recType;
    read(Sockfd, &recType, sizeof(char));
    printf("record-type: %c\n", recType);
    
    int recVersion;
    read(Sockfd, &recVersion, sizeof(int));
    swapEndian(&recVersion, INT);
    printf("record-version: %d\n", recVersion);
    
    //  read content
    read(Sockfd, &size, sizeof(int));
    swapEndian(&size, INT);
    char content[size+1];
    read(Sockfd, content, size);
    content[size] = '\0';
    printf("content:'%s'\n", content);
    
    return 0;
}

int queryRecord(char *query) {
    int i, ret, size, total;
    GPacket.opType = REQUEST_COMMAND;
    size = reqQueryMsg(GPacket.msg, "q", query);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5);
    //printf("ret:%d\n", ret);
    printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) return 1;
    
    read(Sockfd, &GPacket.msg, 1);
    read(Sockfd, &total, 4);
    swapEndian(&total, INT);
    printf("total: %d\n",total);    // total record
    read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    //read(Sockfd, &GPacket.msg, 1+4+2+1+2+8+4);
    
    for(i=0;i<total;i++){
        read(Sockfd, &size, 4);
        swapEndian(&size, INT);
        printf("[%d]--------------------------------------------------\n",i);
        printf("size: %d\t",size);
        
        //  read content
        read(Sockfd, GPacket.msg, size);
        GPacket.msg[size]='\0';
        printf("msg: %.*s\n",size,GPacket.msg);
        //printf("len: %d\n", strlen(GPacket.msg));
        read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    }
    
//    read(Sockfd, &GPacket.msg, 2+1+2+8+4);
//    read(Sockfd, &size, 4);
//    swapEndian(&size, INT);
//    printf("size: %d\n",size);
//    //while (size>0) {
//        read(Sockfd, GPacket.msg, size);
//        printf("msg: %s\n",GPacket.msg);
//       //size -= NAME_SIZE;
//    //}
    
    return 0;
}

int createClass(char *class, char *extend) {
    int ret;
    char command[256];
    sprintf(command, "CREATE CLASS %s EXTENDS %s", class, extend);
    ret = sendCommand(command);
    if (ret!=0) return 1;
    
    printf("%s: OK!\n", command);
    return 0;
}

int createVertex(char *command){
    int ret, size;
    GPacket.opType = REQUEST_COMMAND;
    size = reqCommandMsg(GPacket.msg, "c", command);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+1+2+1);
    if (ret<0 || GPacket.opType!=0){
        //printf("failed\n");
        return 1;
    }
    else{
        ret = read(Sockfd, &addr.cltid, 2);
        if (ret<0) return 1;
        swapEndian(&addr.cltid, SHORT);
        
        ret = read(Sockfd, &addr.rid, 8);
        if (ret<0) return 1;
        swapEndian(&addr.rid, LONG);
        
        printf("@rid #%d:%lu\n\n",addr.cltid,addr.rid);
        ret = read(Sockfd,&GPacket.msg,256);
        if (ret<0) return 1;
        
        return 0;
    }
}

//int testCreateVertex(char *vertex, char *file, int nRecord) {
//    int ret, i;
//    int size = 0;
//    char *name;
//    char buffer[NAME_SIZE];
//    FILE *fd;
//    int start, stop;
//    double sum=0, allSum=0;
//    
//    GPacket.opType = REQUEST_COMMAND;
//    size = addByte('s', GPacket.msg);
//    size += addInt(5+4+25+strlen(vertex)+NAME_SIZE+1+1+1, GPacket.msg+size);
//    size += addString("c", GPacket.msg+size);
//    size += addInt(32+NAME_SIZE+1, GPacket.msg+size);
//    sprintf(GPacket.msg+size, "CREATE VERTEX %s SET name='", vertex);
//    size += 25 + strlen(vertex);
//    name = GPacket.msg+size;
//    size += NAME_SIZE;
//    size += addByte('\'', GPacket.msg+size);
//    size += addByte(0, GPacket.msg+size);
//    size += addByte(0, GPacket.msg+size);
//    
//    fd = fopen(file, "r");
//    for (i=0; i<nRecord; i++) {
//        fread(name, NAME_SIZE, 1, fd);
//        fread(buffer, 1, 1, fd);
//        start = clock();
//        write(Sockfd, &GPacket, 5+size);
//        read(Sockfd, buffer, 1+4+1+2+1+2+8+4);
//        read(Sockfd, &ret, 4);
//        swapEndian(&ret, INT);
//        while (ret>0) {
//            read(Sockfd, buffer, NAME_SIZE);
//            ret -= NAME_SIZE;
//        }
//        stop = clock();
//        sum += stop-start;
//        if (i%SAMPLING_TIME == 0) {
//            printf("i:%d, arv:%f ms\n", i, sum/CLOCKS_PER_SEC/SAMPLING_TIME*1000);
//            allSum += sum;
//            sum = 0;
//        }
//    }
//    fclose(fd);
//    printf("allsum: %f sec\n", allSum/CLOCKS_PER_SEC);
//    printf("allarv: %f ms\n", allSum/CLOCKS_PER_SEC/N_RECORD*1000);
//    return 0;
//}
//int testCreateEdge() {
//    int size = 0;
//    char *from, *to;
//    
//    GPacket.opType = REQUEST_COMMAND;
//    size = addByte('s', GPacket.msg);
//    size += addInt(5+4+27+7+8+7+1+1, GPacket.msg+size);
//    size += addString("c", GPacket.msg+size);
//    size += addInt(27+7+8+7, GPacket.msg+size);
//    memcpy(GPacket.msg+size, "CREATE EDGE Teach from #11:", 27);
//    size += 27;
//    from = GPacket.msg+size;
//    size += 7;
//    memcpy(GPacket.msg+size, " to #12:", 8);
//    size += 8;
//    to = GPacket.msg+size;
//    size += 7;
//    size += addByte(0, GPacket.msg+size);
//    size += addByte(0, GPacket.msg+size);
//    
//    int i;
//    char buffer[NAME_SIZE];
//    int tmp;
//    FILE *fd;
//    int start, stop;
//    double sum=0, allSum=0;
//    fd = fopen(EDGE_FILE, "r");
//    for (i=0;i<N_RECORD; i++) {
//        fread(from, 7, 1, fd);
//        fread(buffer, 1, 1, fd);
//        fread(to, 7, 1, fd);
//        fread(buffer, 1, 1, fd);
//        start = clock();
//        write(Sockfd, &GPacket, 5+size);
//        read(Sockfd, buffer, 1+4+1+4+2+1+2+8+4);
//        if (buffer[0] == 0) {
//            read(Sockfd, &tmp, 4);
//            tmp = (tmp<<8 & 0xff00ff00) | (tmp>>8 & 0x00ff00ff);
//            tmp = (tmp<<16 & 0xffff0000) | (tmp>>16 & 0x0000ffff);
//            while (tmp>0) {
//                read(Sockfd, buffer, NAME_SIZE);
//                tmp -= NAME_SIZE;
//            }
//        }
//        stop = clock();
//        sum += stop-start;
//        //if (ret<0 || buffer[0]!=0) return 1;
//        if (i%SAMPLING_TIME == 0) {
//            printf("i:%d, arv:%f ms\n", i, sum/CLOCKS_PER_SEC/SAMPLING_TIME*1000);
//            allSum += sum;
//            sum = 0;
//        }
//    }
//    printf("allsum: %f sec\n", allSum/CLOCKS_PER_SEC);
//    printf("allarv: %f ms\n", allSum/CLOCKS_PER_SEC/N_RECORD*1000);
//    printf("CREATE Edge OK\n");
//    fclose(fd);
//    
//    return 0;
//}
