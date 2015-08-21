#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#include <uuid/uuid.h>
#include "reqmsg.h"
#include "swap_endian.h"
#include "diff_match_patch.h"
#include "ezxml.h"

#define MAX_STRING_SIZE (1024)      // xml
#define MAX_NEW_TAG (20)            // xml
#define FULL_CONTENT (5)            // xml
using namespace std;                // xml

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 2424

//#define N_RECORD (10)
#define NAME_SIZE (19999)
#define SAMPLING_TIME (1000)

#define DB_NAME "test-new"
#define DIFF_FILE "/Users/pimpat/Desktop/result_diff.txt"

typedef struct Packet Packet;
struct Packet {
    char opType;
    char ssid[4];
    char msg[NAME_SIZE*2];
} PacketDesc;

Packet GPacket;
int Sockfd;

typedef struct Addr Addr;
struct Addr{
    short cltid;
    long rid;
};

Addr addr;

/* binary protocol */
int prepareDB();
int connectSocket();
int sendCommand(char *command);
int connectServer();
int disconnectServer();
int openDatabase(char *dbName);
int createDatabase(char *dbName);
int createClass(char *myclass, char *extend);
int loadRecord(short clusterId, long clusterPos);
int queryRecord(char *query);
int createNewInfo(char *name, char *str);
int createVertex(char *command);

/* xml parser */
int isNumber(char* str);
string get_strLast(ezxml_t root);
string get_strVer(ezxml_t root, char* vtarget);
string update_strVer(ezxml_t root, char* vnow , char* vtarget, char* str);
void add_diff_list_full( ezxml_t root, char* new_str, char *num, char* new_strPatch, char* last_num);
const char *stringUUID();

void append(char* s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

int main() {
    int ret;
    int size;

    //prepareDB();  // include connectServer()
    
    Sockfd = connectSocket();
    if (Sockfd < 0) return Sockfd;

    ret = openDatabase(DB_NAME);
    if (ret!=0) {
        printf ("error openDatabase\n");
        return 1;
    }
    
    clock_t start, end, sum1, sum2;
    int i;
    
    start = clock();
    for(i=0;i<100;i++){
    queryRecord("find references #11:43");
    }
    end = clock();
    sum1 = end-start;
    
    start = clock();
    for(i=0;i<100;i++){
    queryRecord("select @rid from Link where out =#11:43 or in=#11:43");
    }
    end = clock();
    sum2 = end-start;
    
    printf("\n\n");
    printf("time(find): %f\n",(double)sum1/CLOCKS_PER_SEC/100);
    printf("time(select): %f\n",(double)sum2/CLOCKS_PER_SEC/100);
    
    //queryRecord("select @rid from (select expand(both()) from #11:43)");
//    const char *uuid;
//    uuid = stringUUID();
//    printf("uuid: %s\n",uuid);
//    char sql[200];
//    sprintf(sql,"create vertex TestUUID set uuid='%s'",uuid);
//    //sprintf(sql,"create vertex TestUUID set str='%s'",uuid);
//    printf("sql: %s\n",sql);
//    sendCommand(sql);
//    //queryRecord("select from TestUUID");
    
//    ret = createNewInfo("mytext1", "pimpat1");
//    if (ret!=0) printf ("error createNewInfo\n");
//    
//    ret = createNewInfo("mytext2", "pimpat2");
//    if (ret!=0) printf ("error createNewInfo\n");
    
//    printf("--------------------------------------------------\n");
//    const char *uuid;
//    char ch;
//    char name[12];
//    char sql[100];
//    int i,j;
//    srand(time(NULL));
//    for(i=0;i<10;i++){
//        uuid = stringUUID();
//        printf("uuid: %s\n",uuid);
//            for(j=0;j<10;j++){
//                ch = rand()%26 + 97;
//                append(name,ch);
//            }
//        printf("name: %s\n",name);
//        sprintf(sql,"CREATE VERTEX Info set uuid='%s', name='%s'",uuid,name);
//        ret = sendCommand(sql);
//        if (ret!=0) printf ("error sendCommand\n");
//        name[0]='\0';
//    }
//    printf("--------------------------------------------------\n");
    
    //ret = queryRecord("traverse * from #11:42");
    //ret = queryRecord("select classes[name='Testtest'].defaultClusterId from 0:1");
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

//----------------------------------------------------------------------------------------------------------
int prepareDB() {
    int ret, i;

    Sockfd = connectSocket();
    if (Sockfd < 0) return Sockfd;
    
    ret = connectServer();
    if (ret!=0) return 1;
    
    ret = createDatabase(DB_NAME);
    if (ret!=0) return 1;
    
    //  create
    
    //  create class Info
    ret = createClass("Info", "V");
    if (ret!=0) return 1;
    
    ret = sendCommand("CREATE PROPERTY Info.uuid STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY Info.uuid STRING: OK!\n");
    
    ret = sendCommand("CREATE PROPERTY Info.name STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY Info.name STRING: OK!\n");
    
    //  create class Data
    ret = createClass("Data","V");
    if (ret!=0) return 1;
    
    ret = sendCommand("CREATE PROPERTY Data.content STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY Data.content STRING: OK!\n");
    
    //  create class Diff_A
    ret = createClass("Diff_A","V");
    if (ret!=0) return 1;
    
    ret = sendCommand("CREATE PROPERTY Diff_A.content STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY Diff_A.content STRING: OK!\n");

    //  create class Diff_B
    ret = createClass("Diff_B","V");
    if (ret!=0) return 1;
    
    ret = sendCommand("CREATE PROPERTY Diff_B.content STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY Diff_B.content STRING: OK!\n");
    
    //  create edge LinkData
    ret = createClass("LinkData","E");
    if (ret!=0) return 1;
    
    //  create edge LinkDiff
    ret = createClass("LinkDiff","E");
    if (ret!=0) return 1;
    
    ret = sendCommand("CREATE PROPERTY LinkDiff.ptr STRING");
    if (ret!=0) return 1;
    printf("CREATE PROPERTY LinkDiff.ptr STRING: OK!\n");
    
    //  alter class
    ret = sendCommand("ALTER DATABASE CUSTOM useLightweightEdges=false");
    if (ret!=0) return 1;
    printf("ALTER DATABASE CUSTOM useLightweightEdges=false: OK!\n");
    
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

int createNewInfo(char *name, char* str){
    int ret;
    short cltid_f, cltid_t;
    long rid_f, rid_t;
    const char *uuid;
    char sql[200];
    uuid = stringUUID();
    printf("uuid: %s\n",uuid);
    
    sprintf(sql,"CREATE VERTEX Info set uuid='%s', name='%s'",uuid,name);
    //ret = sendCommand(sql);
    ret = createVertex(sql);
    if (ret!=0) return 1;
    cltid_f = addr.cltid;
    rid_f = addr.rid;
    
    sprintf(sql,"CREATE VERTEX Data set content='%s'",str);
    ret = createVertex(sql);
    if (ret!=0) return 1;
    cltid_t = addr.cltid;
    rid_t = addr.rid;
    
    sprintf(sql,"CREATE EDGE LinkData from #%d:%lu to #%d:%lu",cltid_f,rid_f,cltid_t,rid_t);
    ret = sendCommand(sql);
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

int connectServer() {
    int ret, size;
    GPacket.opType = REQUEST_CONNECT;
    *(int *)GPacket.ssid = -1;
    size = reqConnectMsg(GPacket.msg, NULL, NULL, 28, NULL, "root", "pimpat");
    ret = write(Sockfd, &GPacket, 5+size);
    ret = read(Sockfd, &GPacket, 5+sizeof(int));
    if (ret<0 || GPacket.opType!=0) return 1;

    // copy ssid
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
    size = reqDBOpenMsg(GPacket.msg, NULL, NULL, 28, NULL, dbName, "graph", "root", "pimpat");
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+sizeof(int));
    printf("resSize: %d\n", ret);
    printf("resCode: %d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) return 1;

    // copy ssid
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
        //printf("[%d]--------------------------------------------------\n",i);
        printf("size: %d\t",size);
        
        //  read content
        read(Sockfd, GPacket.msg, size);
        GPacket.msg[size]='\0';
        printf("msg: %.*s\n",size,GPacket.msg);
        //printf("len: %d\n", strlen(GPacket.msg));
        read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    }
    
    return 0;
}

int createClass(char *myclass, char *extend) {
    int ret;
    char command[256];
    sprintf(command, "CREATE CLASS %s EXTENDS %s", myclass, extend);
    ret = sendCommand(command);
    if (ret!=0) return 1;
    
    printf("%s: OK!\n", command);
    return 0;
}

//----------------------------------------------------------------------------------------------------------
int isNumber(char* str){
    int isDigit=0;
    int i=0;
	while(i<strlen(str)){
  		isDigit = isdigit(str[i]);
  		if (isDigit == 0) {
			printf("this string is not a number.\n");
			break;
		}
        i++;
	}
    return isDigit;
}

string get_strLast(ezxml_t root){
    char last[5];
    sprintf(last,"%s",ezxml_child(root,"max")->txt);
    //printf("last1: %s\n",last);
    int num_last = atoi(last)-1;
    //sprintf(last,"%d",num_last);
    //printf("last2: %s\n",last);
    
    int count = num_last/FULL_CONTENT;
    int offset = num_last%FULL_CONTENT;
    int i;
    
    diff_match_patch<wstring> dmp;
    string s;
    wstring strResult;
    ezxml_t desc;
    
    //--------------------------------------------------------------
    ezxml_t ver, myver;
    ver = ezxml_child(root,"ver");
    desc = ezxml_get(root,"ver",FULL_CONTENT*count,"desc",-1);
    strResult = wstring(desc->txt,desc->txt+strlen(desc->txt));
    myver = ezxml_idx(ver,FULL_CONTENT*count);
    //const char *verid;
    //verid = ezxml_attr(myver, "id");
    //printf("verid: %s\n\n", verid);
    if(offset != 0){
        for(i=1;i<=offset;i++){
            myver = ezxml_idx(myver,1);
            desc = ezxml_child(myver,"desc");
            pair<wstring, vector<bool> > out
            = dmp.patch_apply(dmp.patch_fromText(wstring(desc->txt,desc->txt+strlen(desc->txt))), strResult);
            strResult = out.first;
            //printf("loop: %s\n",s.assign(strResult.begin(),strResult.end()).c_str());
        }
    }
    return s.assign(strResult.begin(),strResult.end());
}

string get_strVer(ezxml_t root, char* vtarget){
    //    int isDigit=0;
    //    isDigit = isNumber(vtarget);
    //    if(isDigit == 0) return "NULL";
    
    int target = atoi(vtarget);
    int count = target/FULL_CONTENT;
    int offset = target%FULL_CONTENT;
    int i, m;
    char num[5];
    sprintf(num,"%s",ezxml_child(root,"max")->txt);
    //printf("num: %s\n",num);
    m = atoi(num)-1;
    
    if((target<0)||(target>m))
        return "no data in list!!";
    
    diff_match_patch<wstring> dmp;
    string s;
    wstring strResult;
    ezxml_t desc;
    
    desc = ezxml_get(root,"ver",FULL_CONTENT*count,"desc",-1);
    strResult = wstring(desc->txt,desc->txt+strlen(desc->txt));
    
    if(offset != 0){
        for(i=1;i<=offset;i++){
            desc = ezxml_get(root,"ver",FULL_CONTENT*count+i,"desc",-1);
            pair<wstring, vector<bool> > out
            = dmp.patch_apply(dmp.patch_fromText(wstring(desc->txt,desc->txt+strlen(desc->txt))), strResult);
            strResult = out.first;
            //printf("loop: %s\n",s.assign(strResult.begin(),strResult.end()).c_str());
        }
    }
    //printf("%s\n",ezxml_get(root,"ver",i,"desc",-1)->txt);
    //return ezxml_get(root,"ver",i,"desc",-1)->txt;
    return s.assign(strResult.begin(),strResult.end());
}

string update_strVer(ezxml_t root, char* vnow , char* vtarget, char* str){
    int now = atoi(vnow);
    int target = atoi(vtarget);
    int i, m;
    char num[5];
    sprintf(num,"%s",ezxml_child(root,"max")->txt);
    m = atoi(num)-1;
    
    if((now<0)||(now>m)||(target<0)||(target>m))
        return "no data in list!!";
    if(now>target)
        return "incorrect version";
    
    int count_now = now/FULL_CONTENT;
    int count_target = target/FULL_CONTENT;
    int offset_now = now%FULL_CONTENT;
    int offset_target = target%FULL_CONTENT;
    int diff = offset_target-offset_now;
    
    printf(">> updating string from ver[%s] to ver[%s]...\n",vnow,vtarget);
    
    diff_match_patch<wstring> dmp;
    string s;
    wstring strResult;
    ezxml_t desc;
    
    if(count_now == count_target){
        desc = ezxml_get(root,"ver",now,"desc",-1);
        strResult = wstring(str,str+strlen(str));
        for(i=1;i<=diff;i++){
            desc = ezxml_get(root,"ver",now+i,"desc",-1);
            pair<wstring, vector<bool> > out
            = dmp.patch_apply(dmp.patch_fromText(wstring(desc->txt,desc->txt+strlen(desc->txt))), strResult);
            strResult = out.first;
            //printf("loop1: %s\n",s.assign(strResult.begin(),strResult.end()).c_str());
        }
    }
    else{
        //  count_now < count_target
        desc = ezxml_get(root,"ver",FULL_CONTENT*count_target,"desc",-1);
        strResult = wstring(desc->txt,desc->txt+strlen(desc->txt));
        for(i=1;i<=offset_target;i++){
            desc = ezxml_get(root,"ver",FULL_CONTENT*count_target+i,"desc",-1);
            pair<wstring, vector<bool> > out
            = dmp.patch_apply(dmp.patch_fromText(wstring(desc->txt,desc->txt+strlen(desc->txt))), strResult);
            strResult = out.first;
            //printf("loop2: %s\n",s.assign(strResult.begin(),strResult.end()).c_str());
        }
    }
    return s.assign(strResult.begin(),strResult.end());
}

void add_diff_list_full( ezxml_t root, char* new_str, char *num, char* new_strPatch, char* last_num){
    int m;
    sprintf(num,"%s",ezxml_child(root,"max")->txt);
    ezxml_t ver, desc, max;
    ver = ezxml_add_child(root,"ver",0);
    ezxml_set_attr(ver,"id",num);
    desc = ezxml_add_child(ver,"desc",0);
    
    // get strPatch last and new
    m = atoi(num);
    if((m%FULL_CONTENT)==0){
        sprintf(new_strPatch,"%s",new_str);
        ezxml_set_txt(desc,new_strPatch);
    }
    else{
        diff_match_patch<wstring> dmp;
        string s;
        wstring strPatch;
        char curr_str[MAX_STRING_SIZE];
        sprintf(curr_str,"%s",get_strLast(root).c_str());
        wstring str1 = wstring(new_str,new_str+strlen(new_str));
        //wcout << str1 << endl;
        wstring str0 = wstring(curr_str,curr_str+strlen(curr_str));
        //wcout << str0 << endl;
        strPatch = dmp.patch_toText(dmp.patch_make(str0, str1));
        s.assign(strPatch.begin(),strPatch.end());
        //wcout << strPatch << endl;
        sprintf(new_strPatch,"%s",s.c_str());
        //printf("new_strPatch: %s\n",new_strPatch);
        ezxml_set_txt(desc,new_strPatch);
    }
    
    max = ezxml_child(root,"max");
    m++;
    sprintf(last_num,"%d",m);
    ezxml_set_txt(max,last_num);
}

const char *stringUUID(){
    char hex_char [] = "0123456789ABCDEF";
    char *uuidstr = (char*) malloc (sizeof (uuid_t) * 2 + 1);
    uuid_t uuid;
    uuid_generate (uuid);
    
//    int i;
//    for (i = 0; i < sizeof uuid; i ++) {
//        printf("%02x ", uuid[i]);
//    }
//    printf("\n\n");
    
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < sizeof (uuid_t); byte_nbr++) {
        uuidstr [byte_nbr * 2 + 0] = hex_char [uuid [byte_nbr] >> 4];
        uuidstr [byte_nbr * 2 + 1] = hex_char [uuid [byte_nbr] & 15];
    }
    return uuidstr;
}
