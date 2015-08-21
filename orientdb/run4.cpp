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

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 2424
#define NAME_SIZE (19999)

#define DB_NAME "test-draft2"
#define MAX_SQL_SIZE (1000)
#define MAX_DIFF_SIZE (1000)
#define MAX_NODE_REF_SIZE (1000)
#define MAX_CONTENT_SIZE (1000)
#define MAX_NEW_TAG (20)
#define FULL_CONTENT (3)
using namespace std;

int Sockfd;
typedef struct Packet {
    char opType;
    char ssid[4];
    char msg[NAME_SIZE*2];
} Packet;
Packet GPacket;

typedef struct Addr{
    short cltid;
    long rid;
} Addr;
Addr addr;

typedef struct Rid{
    Rid* next;
    char rid[20];
} Rid;

//  edge.h
enum VALUETYPE {
    intType,
    longType,
    floatType,
    doubleType,
    stringType
};

enum EDGETYPE {
    Org2User,
    User2Cat,
    Cat2Task,
    Task2SubTask,
    Cat2State,
    State2Task,
    Share,
    Delete,
    Data2DataHolder,
    DataHolder2Content,
    Content2DataHolder,
    ContentVersion,
    DevRef2Content
};

typedef struct EdgeKeyValue {
    char* key;
    char* value;
    int type;
} EdgeKeyValue;

typedef struct Edge {
    int edgeType;
    const char* edgeID;
    char edgeName[30];
    EdgeKeyValue** keyValueList;
} Edge;

const char* createEdge(const char* fromObj, const char* toObj, EdgeKeyValue *keyValue);
int deleteEdge(const char* edgeID);
Edge* getEdge(const char* edgeID);

int


typedef char* Text;
typedef struct ObjectBinary {
    Text xmlSchema;
    int byteCount;
    char* data;
} ObjectBinary;

typedef struct User{
    const char* userID;
    char userName[30];
} User;

typedef struct Category{
    const char* categoryID;
    char categoryName[30];
} Category;

typedef struct DeviceRef {
    const char* userID;
    const char* deviceTransportID;
    char nodeRefToDataContent[MAX_NODE_REF_SIZE];
} DeviceRef;

typedef struct DataContent{
    DataContent* nextVersion;
    DataContent* preVersion;
    ObjectBinary* objContent;
    int isDiff;
} DataContent;

typedef struct DataHolder{
    DataContent* head;
    DataContent* lastestCommon;
    int versionKeeped;
    int userCount;
    DataContent* current;
    DeviceRef* userDevicePtr[10];
} DataHolder;

typedef struct Data{
    const char* dataID;
    DataHolder* content;
    const char* chatRoom;
} Data;

extern Data* mydata;
Data* mydata = NULL;

/* binary protocol */
int connectSocket();
int connectServer();
int disconnectServer();
int createDatabase(char *dbName);
int createClass(char *myclass, char *extend);
int prepareDB();
int openDatabase(char *dbName);
int sendCommand(char *command);
int queryRecord(char *query);
int queryRecord_Binary(char *query);
int loadRecord(short clusterId, long clusterPos);
int createVertex(char *command);

char* getRid(char *query);
char* getContent(char *query);
char** getArrayRid(char* query);

/* P'Part function */
const char* stringUUID();
const char* createNewData();
DataHolder* createNewDataHolder();
const char* createOrg(Text orgName);
const char* createUser(Text userName);
const char* createCategory(Text categoryName);
//const char* createData(Text dataName);
ObjectBinary* getDataContent(Data* data);
//ObjectBinary getDataContentByID(char* dataID);
ObjectBinary* getDataContentCommonVersion(Data* data);
//ObjectBinary getDataContentCommonVersionByID(char* dataID);
Text getDiffDataAtHead(Data* data);
//Text getDiffDataAtHeadByID(char* dataID);
Text getDiffDataAtLastestCommon(Data* data);
//Text getDiffDataAtLastestCommonByID(char* dataID);
ObjectBinary* getContentNextVer(Data* data);
//ObjectBinary getContentNextVerByID(char* dataID);
ObjectBinary* getContentPreVer(Data* data);
//ObjectBinary getContentPreVerByID(char* dataID);
Text getDiffDataNextVer(Data* data);
//Text getDiffDataNextVerByID(char* dataID);
Text getDiffDataPreVer(Data* data);
//Text getDiffDataPreVerByID(char* dataID);

int getCurLevelFromHead(Data* data);
int getCurLevelFromCommon(Data* data);

Text getTagContent(ObjectBinary* fullContent, char* tagName);
int setTagContent(ObjectBinary* fullContent, Text newTagContent, char* tagName);

//char* getDataContentWithTag(Data* data, char* tagName);
int setNewDataContent(Data* data, char* diffContent);
//int setNewDataDiffWithTag(Data* data, char* tagName, char* diff);
//int addDataToCategory(char* categoryID, Data *data);
//Data* queryDataByID(char* dataID);
//Category** queryCategoryByID(char* dataID);
//User** queryUserByID(char* dataID);

string getDiff(char* old_str, char* new_str);           // pim
string getPatch(char* str, char* str_patch);            // pim
char* getDataContentFirst(Data* data);                  // pim
char* getDataContentLast(Data* data);                    // pim
char* getDataContentVer(Data* data,int ver);           // pim


/* Test Pim */
int testUUID(uuid_t* uuid);
void testDMP();
void testUserCategory(int x);
void testTag(Data* data);
void testsetNewDataContent();
void testCountByte();
void testqueryDataByID(char* myID, char* mycatID);
void testArrayRid();
void testqueryCategoryByID(char* myID);
void testqueryUserByID(char* myID);
void testDraft2();

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
    
    //testUUID(uuid);
    //testDMP();
    //testUserCategory();
    //testCountByte();
    //testTag();
    //testArrayRid();
    
    //queryRecord("select shortestPath(#20:2615,#20:2622,'BOTH')");
    //queryRecord("find references #22:18");

    /* A set for test */
    testsetNewDataContent();
    //testDraft2();
//    testUserCategory(1);
//    testUserCategory(2);
//    
//    char* myID;
//    char _myID[35];
//    myID = getContent("select dataID from Data");
//    memcpy(_myID, myID+8,32);
//    _myID[32]='\0';
//    printf("myID: %s\n",_myID);
//    
//    char* mycatID;
//    char _mycatID[35];
//    mycatID = getContent("select categoryID from Category where categoryName='Games'");
//    memcpy(_mycatID, mycatID+12,32);
//    _mycatID[32]='\0';
//    printf("catID: %s\n",_mycatID);
//
//    testqueryDataByID(_myID,_mycatID);
//    testqueryCategoryByID(_myID);
//    testqueryUserByID(_myID);
//
//    free(myID);
//    free(mycatID);
    /*----------------*/
    
    //queryRecord("select out from fromDeviceRef where in = #17:16");
    //queryRecord("select in from toDataContent where out = #17:16");
    //queryRecord("select count(*) in from toDataContent where out = #17:16");
    
    //getRid("select @rid from User");
    //queryRecord("select classes[name='Testtest'].defaultClusterId from 0:1");
    //queryRecord("select expand(in_Link) from Diff limit -1");
    //sendCommand("delete vertex #11:67");
    //sendCommand("update Diff set key='555', value='666' where key='100'");
    
    disconnectServer();
    close(Sockfd);

    return 0;
}

//----------------------------------------------------------------------------------------------------------

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

int createClass(char *myclass, char *extend) {
    int ret;
    char command[256];
    sprintf(command, "CREATE CLASS %s EXTENDS %s", myclass, extend);
    ret = sendCommand(command);
    if (ret!=0) return 1;
    
    printf("%s: OK!\n", command);
    return 0;
}

int prepareDB() {
    int ret;

    Sockfd = connectSocket();
    if (Sockfd < 0) return Sockfd;
    printf("connectSocket..PASS\n");
    ret = connectServer();
    if (ret!=0) return 1;
    printf("connectServer...PASS\n");
    
    ret = createDatabase(DB_NAME);
    if (ret!=0) return 1;
    printf("createDB...PASS\n");
    
    /* Org */
    ret = createClass("Org", "V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY Org.orgID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Org.orgID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY Org.orgName STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Org.orgName STRING: OK!\n");
    
    /* User */
    ret = createClass("User", "V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY User.userID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY User.userID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY User.userName STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY User.userName STRING: OK!\n");
    
    /* Category */
    ret = createClass("Category","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY Category.categoryID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Category.categoryID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY Category.categoryName STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Category.categoryName STRING: OK!\n");
    
    /* Edge toUser */
    ret = createClass("toUser","E");
    if (ret!=0) return 1;
    
    /* Edge toCategory */
    ret = createClass("toCategory","E");
    if (ret!=0) return 1;
    
    /* Edge toData */
    ret = createClass("toData","E");
    if (ret!=0) return 1;
    
    /* Data */
    ret = createClass("Data","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY Data.dataID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Data.dataID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY Data.chatRoom STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Data.chatRoom STRING: OK!\n");
    
    /* Edge toDataHolder */
    ret = createClass("toDataHolder","E");
    if (ret!=0) return 1;
    
    /* DataHolder */
    ret = createClass("DataHolder","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY DataHolder.versionKeeped INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataHolder.versionKeeped INTEGER: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataHolder.userCount INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataHolder.userCount INTEGER: OK!\n");
    
    /* Edge toDataContent */
    ret = createClass("toDataContent","E");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY toDataContent.name STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY toDataContent.name STRING: OK!\n");
    
    /* Edge fromDeviceRef */
    ret = createClass("fromDeviceRef","E");
    if (ret!=0) return 1;
    
    /* DataContent */
    ret = createClass("DataContent","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY DataContent.xmlSchema STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataContent.xmlSchema STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataContent.byteCount INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataContent.byteCount INTEGER: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataContent.data STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataContent.data STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataContent.isDiff BOOLEAN");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataContent.isDiff BOOLEAN: OK!\n");
    
    /* Edge toDiffContent */
    ret = createClass("toDiffContent","E");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY toDiffContent.status STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY toDiffContent.status STRING: OK!\n");
    
    /* DeviceRef */
    ret = createClass("DeviceRef","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY DeviceRef.userID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DeviceRef.userID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY DeviceRef.deviceTransportID STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DeviceRef.deviceTransportID STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY DeviceRef.nodeRefToDataContent STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DeviceRef.nodeRefToDataContent STRING: OK!\n");
    
    //  alter class
    ret = sendCommand("ALTER DATABASE CUSTOM useLightweightEdges=false");
    if (ret!=0) return 1;
    printf("ALTER DATABASE CUSTOM useLightweightEdges=false: OK!\n");
    
    disconnectServer();
    close(Sockfd);
    
    printf("prepareDB...PASS\n");
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
        //char type[32];
        short id;//, dataSeg;
        
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
        //clusterConfig[size]='\0';
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

int sendCommand(char *command) {
    int ret, size;
    GPacket.opType = REQUEST_COMMAND;
    size = reqCommandMsg(GPacket.msg, "c", command);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+256);
    //printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) 
        return 1;
    else 
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
    
    return 0;
}

int queryRecord_Binary(char *query) {
    int i, ret, size, total;
    GPacket.opType = REQUEST_COMMAND;
    size = reqQueryMsg_Binary(GPacket.msg, "q", query);
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
    printf("size: %d\n",size);
    
    char content[size+1];
    read(Sockfd, content, size);
    content[size] = '\0';
    printf("content:'%s'\n", content);

    return 0;
}

int createVertex(char *command){
    int ret, size;
    int in_size;
    GPacket.opType = REQUEST_COMMAND;
    size = reqCommandMsg(GPacket.msg, "c", command);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5+1+2+1);
    if (ret<0 || GPacket.opType!=0){
        //printf("createVertex..FAILED\n");
        return 1;
    }
    else{
        ret = read(Sockfd, &addr.cltid, 2);
        if (ret<0) return 1;
        swapEndian(&addr.cltid, SHORT);
        ret = read(Sockfd, &addr.rid, 8);
        if (ret<0) return 1;
        swapEndian(&addr.rid, LONG);
        printf("@rid #%d:%lu\n",addr.cltid,addr.rid);
        
        read(Sockfd, &in_size, 4);
        read(Sockfd, &in_size, 4);
        swapEndian(&in_size, INT);
        //printf("in_size: %d\n",in_size);
        
        ret = read(Sockfd,&GPacket.msg,in_size+1);
        if (ret<0) return 1;
        return 0;
    }
}

char* getRid(char *query) {
    int i, ret, size, total;
    GPacket.opType = REQUEST_COMMAND;
    size = reqQueryMsg(GPacket.msg, "q", query);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5);
    //printf("ret:%d\n", ret);
    printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0){
        printf("getRid error\n");
        return NULL;
    }
    
    read(Sockfd, &GPacket.msg, 1);
    read(Sockfd, &total, 4);
    swapEndian(&total, INT);
    printf("total: %d\n",total);    // total record
    read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    //read(Sockfd, &GPacket.msg, 1+4+2+1+2+8+4);
    
    char myResult[15];
    char* str;
    char* token;
    char delim[2] = "#";
    //char* str = (char*)malloc(sizeof(char)*10);   (fix)
    
    
    for(i=0;i<total;i++){
        read(Sockfd, &size, 4);
        swapEndian(&size, INT);
        //printf("[%d]--------------------------------------------------\n",i);
        //printf("size: %d\t",size);
        
        //  read content
        read(Sockfd, GPacket.msg, size);
        GPacket.msg[size]='\0';
        sprintf(myResult,"%s",GPacket.msg);
        printf("size: %lu\t",strlen(myResult));
        printf("msg: %s\n", myResult);
        token = strtok(myResult, delim);
        token = strtok(NULL, delim);
        //printf("result_token: %s\n", token);
        str = strdup(token);
        //sprintf(str,"%s",token);  (fix)
        read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    }
    printf("@rid #%s\n",str);
    return str;
}

char* getContent(char *query) {
    int i, ret, size, total;
    GPacket.opType = REQUEST_COMMAND;
    size = reqQueryMsg(GPacket.msg, "q", query);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5);
    printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) {
        printf("getContent error\n");
        return NULL;
    }
    
    read(Sockfd, &GPacket.msg, 1);
    read(Sockfd, &total, 4);
    swapEndian(&total, INT);
    printf("total: %d\n",total);    // total record
    read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    
    if(total==0)
        return NULL;
    
    char* str = (char*)malloc(sizeof(char)*MAX_CONTENT_SIZE);
    
    //for(i=0;i<total;i++){
        read(Sockfd, &size, 4);
        swapEndian(&size, INT);
        printf("size: %d\n",size);
        read(Sockfd, str, size);
        str[size]='\0';
        printf("msg: %s\n",str);
        read(Sockfd, &GPacket.msg, 2+1+2+8+4);
        printf("[result from getContent]---------------\n",i);
    //}
    //printf("result(in): %s\n",str);
    return str;
}

char** getArrayRid(char* query){
    int i, ret, size, total;
    GPacket.opType = REQUEST_COMMAND;
    size = reqQueryMsg(GPacket.msg, "q", query);
    ret = write(Sockfd, &GPacket, 5+size);
    
    ret = read(Sockfd, &GPacket, 5);
    //printf("ret:%d\n", ret);
    printf("opType:%d\n", GPacket.opType);
    if (ret<0 || GPacket.opType!=0) {
        printf("FAILED >> getArrayRid\n");
        return NULL;
    }
    
    read(Sockfd, &GPacket.msg, 1);
    read(Sockfd, &total, 4);
    swapEndian(&total, INT);
    printf("total: %d\n",total);    // total record
    read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    
    char **result_rid = (char**)malloc(sizeof(char*)*total+1);
    
    for(i=0;i<total;i++){
        read(Sockfd, &size, 4);
        swapEndian(&size, INT);
        printf("[%d]--------------------------------------------------\n",i);
        printf("size: %d\t",size);
        result_rid[i] = (char*)malloc(sizeof(char)*size-3);
        
        //  read content
        read(Sockfd, &GPacket.msg, 4);
        read(Sockfd, &GPacket.msg, size-4);
        GPacket.msg[size-4]='\0';
        printf("msg: %s\n", GPacket.msg);
        sprintf(result_rid[i],"%s",GPacket.msg);
        //printf("len: %d\n",strlen(result_rid[i]));
        printf("myrid: %s\n",result_rid[i]);
        read(Sockfd, &GPacket.msg, 2+1+2+8+4);
    }
    result_rid[i] = NULL;
    
//    for(i=0;result_rid[i]!=NULL;i++) {
//        printf("TEST_rid: %s\n",result_rid[i]);
//    }
    
    return result_rid;
}

//----------------------------------------------------------------------------------------------------------

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
    
    uuidstr[32]='\0';
    return uuidstr;
}

const char* createNewData(){
    mydata = (Data*)malloc(sizeof(Data));
    mydata->dataID = stringUUID();
    printf("dataID: %s\n",mydata->dataID);
    mydata->chatRoom = stringUUID();
    printf("chatRoom: %s\n",mydata->chatRoom);
    mydata->content = createNewDataHolder();
    
    return mydata->dataID;
}

DataHolder* createNewDataHolder(){
    DataHolder *dh = (DataHolder*)malloc(sizeof(DataHolder));
    dh->versionKeeped = 0;
    dh->userCount = 0;
    return dh;
}

const char* createOrg(Text orgName){
    int ret;
    const char* uuidstr;
    char sql[MAX_SQL_SIZE];
    
    uuidstr = stringUUID();
    printf("uuid: %s\n",uuidstr);
    sprintf(sql,"CREATE VERTEX Org set orgID='%s', orgName='%s'",uuidstr,orgName);
    printf("sql: %s\n",sql);
    ret = sendCommand(sql);
    
    if (ret!=0)
        printf("createOrg..FAILED\n");
    else
        printf("createOrg..PASS\n");
    
    return uuidstr;
}

const char* createUser(Text userName){
    int ret;
    //uuid_t* uuid = (uuid_t*)malloc (sizeof(uuid_t));
    const char* uuidstr;
    char sql[MAX_SQL_SIZE];
    
    //uuid_generate(*uuid);
    //uuidstr = stringUUID(*uuid);
    uuidstr = stringUUID();
    printf("uuid: %s\n",uuidstr);
    sprintf(sql,"CREATE VERTEX User set userID='%s', userName='%s'",uuidstr,userName);
    printf("sql: %s\n",sql);
    ret = sendCommand(sql);
    
    if (ret!=0)
        printf("createUser..FAILED\n");
    else
        printf("createUser..PASS\n");
    
    return uuidstr;
}

const char* createCategory(Text categoryName){
    int ret;
    const char* uuidstr;
    char sql[MAX_SQL_SIZE];

    uuidstr = stringUUID();
    printf("uuid: %s\n",uuidstr);
    sprintf(sql,"CREATE VERTEX Category set categoryID='%s', categoryName='%s'",uuidstr,categoryName);
    printf("sql: %s\n",sql);
    ret = sendCommand(sql);
    
    if (ret!=0)
        printf("createCategory..FAILED(vertex)\n");
    else
        printf("createCategory..PASS(vertex)\n");
    
    return uuidstr;
}

ObjectBinary* getDataContent(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    obj_ct->xmlSchema = strdup(data->content->lastestCommon->objContent->xmlSchema);
    obj_ct->data = strdup(data->content->head->objContent->data);
    obj_ct->byteCount = strlen(obj_ct->data);
    return obj_ct;
}

ObjectBinary* getDataContentCommonVersion(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    DataContent* mydc;
    int i, j;
    int count = (data->content->versionKeeped-1)/FULL_CONTENT;
    int offset = (data->content->versionKeeped-1)%FULL_CONTENT;
    
    mydc=data->content->head;
    for(i=0;i<FULL_CONTENT*count;i++){
        mydc=mydc->nextVersion;
    }
    sprintf(init_str,"%s",mydc->objContent->data);
    string s;
    
    if(mydc->nextVersion!=NULL && offset!=0){
        for(j=0;j<offset;j++){
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            sprintf(init_str,"%s",s.c_str());
        }
    }
    obj_ct->xmlSchema = strdup(data->content->lastestCommon->objContent->xmlSchema);
    obj_ct->data = init_str;
    obj_ct->byteCount = strlen(init_str);

    return obj_ct;
}

ObjectBinary* getContentNextVer(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    if(data->content->current->nextVersion==NULL){
        free(obj_ct);
        return NULL;
    }
    else if(data->content->current->nextVersion->isDiff==0){
        obj_ct->xmlSchema = strdup(data->content->current->nextVersion->objContent->xmlSchema);
        obj_ct->data = strdup(data->content->current->nextVersion->objContent->data);
        obj_ct->byteCount = data->content->current->nextVersion->objContent->byteCount;
        return obj_ct;
    }
    else{
        char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
        DataContent *mydc, *pre_mydc;
        int count = 0;
        int i;
        for(mydc=data->content->current;mydc!=NULL;mydc=pre_mydc){
            if(mydc->isDiff==false) break;
            pre_mydc = mydc->preVersion;
            count++;
        }
        sprintf(init_str,"%s",mydc->objContent->data);
        //printf("init_str: %s\n",init_str);
        printf("count: %d\n",count);
        string s;

        for(i=0;i<count+1;i++){
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            sprintf(init_str,"%s",s.c_str());
        }
        obj_ct->xmlSchema = strdup(mydc->objContent->xmlSchema);
        obj_ct->data = init_str;
        obj_ct->byteCount = strlen(obj_ct->data);
        return obj_ct;
    }
}

ObjectBinary* getContentPreVer(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    if(data->content->current->preVersion==NULL){
        free(obj_ct);
        return NULL;
    }
    else if(data->content->current->preVersion->isDiff==0){
        obj_ct->xmlSchema = strdup(data->content->current->preVersion->objContent->xmlSchema);
        obj_ct->data = strdup(data->content->current->preVersion->objContent->data);
        obj_ct->byteCount = data->content->current->preVersion->objContent->byteCount;
        return obj_ct;
    }
    else{
        char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
        DataContent *mydc, *pre_mydc;
        int count = 0;
        int i;
        for(mydc=data->content->current;mydc!=NULL;mydc=pre_mydc){
            if(mydc->isDiff==false && count!=0) break;
            pre_mydc = mydc->preVersion;
            count++;
        }
        sprintf(init_str,"%s",mydc->objContent->data);
        //printf("init_str: %s\n",init_str);
        printf("count: %d\n",count);
        string s;
        
        for(i=0;i<count-1;i++){
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            sprintf(init_str,"%s",s.c_str());
        }
        obj_ct->xmlSchema = strdup(mydc->objContent->xmlSchema);
        obj_ct->data = init_str;
        obj_ct->byteCount = strlen(obj_ct->data);
        return obj_ct;
    }
}

Text getDiffDataNextVer(Data* data){
    return data->content->current->nextVersion->objContent->data;
}

Text getDiffDataPreVer(Data* data){
    return data->content->current->preVersion->objContent->data;
}

Text getDiffDataAtHead(Data* data){
    return data->content->head->objContent->data;
}

Text getDiffDataAtLastestCommon(Data* data){
    return data->content->lastestCommon->objContent->data;
}

int getCurLevelFromHead(Data* data){
//    data->content->current = data->content->head;
//    data->content->current = NULL;
    char addr_curr[20],addr_point[20];
    printf("@current: %p\n",data->content->current);
    sprintf(addr_curr,"%p",data->content->current);
    
    DataContent *mydc, *next_mydc;
    int i=0;
    for(mydc=data->content->head;mydc!=NULL;mydc=next_mydc){
        printf("@mydc #%d: %p\n",i,mydc);
        sprintf(addr_point,"%p",mydc);
        if(strcmp(addr_curr,addr_point)==0){
            printf("\n*** FOUND CURRENT ***\n");
            return i;   // i = level found
        }
        next_mydc = mydc->nextVersion;
        i++;
    }
    printf("\n!!! NOT FOUND CURRENT !!!\n");
    return -1;   // -1 = not found
}

int getCurLevelFromCommon(Data* data){
//    data->content->current = data->content->head;
    char addr_curr[20],addr_point[20];
    printf("@current: %p\n",data->content->current);
    sprintf(addr_curr,"%p",data->content->current);
    
    DataContent *mydc, *pre_mydc;
    int i=0;
    for(mydc=data->content->lastestCommon;mydc!=NULL;mydc=pre_mydc){
        printf("@mydc #%d: %p\n",i,mydc);
        sprintf(addr_point,"%p",mydc);
        if(strcmp(addr_curr,addr_point)==0){
            printf("\n*** FOUND CURRENT ***\n");
            return i;   // i = level found
        }
        pre_mydc = mydc->preVersion;
        i++;
    }
    printf("\n!!! NOT FOUND CURRENT !!!\n");
    return -1;   // -1 = not found
}

char* getDataContentFirst(Data *data){
    return data->content->head->objContent->data;
}

Text getTagContent(ObjectBinary* fullContent, char* tagName){
    char* mystr = (char*)malloc(sizeof(char)*fullContent->byteCount+20);
    char* partial_result;
    sprintf(mystr,"<root>%s</root>", fullContent->data);
    
    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    partial_data = ezxml_get(root,tagName,-1);
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        free(mystr);
        ezxml_free(root);
        return NULL;
    }
    //printf("partial_result: %s\n",partial_data->txt);
    partial_result = strdup(partial_data->txt);

    free(mystr);
    ezxml_free(root);
    return partial_result;
}

int setTagContent(ObjectBinary* fullContent, Text newTagContent, char* tagName){
    char* mystr = (char*)malloc(sizeof(char)*fullContent->byteCount+20);
    char* temp_result;
    
    sprintf(mystr,"<root>%s</root>", fullContent->data);
    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    partial_data = ezxml_get(root,tagName,-1);
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        free(mystr);
        ezxml_free(root);
        return -1;
    }
    
    ezxml_set_txt(partial_data,newTagContent);
    temp_result = ezxml_toxml(root);
    //printf("xml: %s\n\n",temp_result);
    
    char* final_result = (char*)malloc(sizeof(char)*strlen(temp_result));
    memcpy(final_result,temp_result+6,strlen(temp_result)-8-6+1);
    //printf("final_result: %s\n\n",final_result);
    
    free(fullContent->data);
    fullContent->data = final_result;
    
    free(temp_result);
    free(mystr);
    ezxml_free(root);
    return 0;
}

char* getDataContentLast(Data *data){
    DataContent* mydc;
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    int i, j;
    int count = (data->content->versionKeeped-1)/FULL_CONTENT;
    int offset = (data->content->versionKeeped-1)%FULL_CONTENT;
    //printf("count: %d\n",count);
    //printf("offset: %d\n",offset);
    
//    printf("\n***********************************************\n");
    mydc=data->content->head;
    for(i=0;i<FULL_CONTENT*count;i++){
        mydc=mydc->nextVersion;
    }
    sprintf(init_str,"%s",mydc->objContent->data);
    string s;
    
    if(mydc->nextVersion!=NULL && offset!=0){
        for(j=0;j<offset;j++){
            //printf("init: %s\n",init_str);
            //printf("mydc: %s\n",mydc->diffContent);
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            //printf("result_in: %s\n",s.c_str());
            sprintf(init_str,"%s",s.c_str());
        }
    }
//    printf("return_content: %s\n",init_str);
//    printf("***********************************************\n");
    return init_str;
}

char* getDataContentVer(Data* data,int ver){
    DataContent* mydc;
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    int i, j;
    int count = ver/FULL_CONTENT;
    int offset = ver%FULL_CONTENT;
    
//    printf("\n*****************************************\n");
    if(ver>=data->content->versionKeeped){
        printf("FAILED >> incorrect version\n");
        free(init_str);
        return NULL;
    }
    mydc=data->content->head;
    
    for(i=0;i<FULL_CONTENT*count;i++){
        mydc=mydc->nextVersion;
    }
    sprintf(init_str,"%s",mydc->objContent->data);
    //printf("init_str: %s\n",init_str);
    string s;
    
    if(mydc->nextVersion!=NULL && offset!=0){
        for(j=0;j<offset;j++){
            //printf("init: %s\n",init_str);
            //printf("mydc: %s\n",mydc->diffContent);
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            //printf("result_in: %s\n",s.c_str());
            sprintf(init_str,"%s",s.c_str());
        }
    }
    return init_str;
}

char* getDataContentWithTag(Data* data, char* tagName){
    char* result;
    char* partial_result = (char*)malloc(sizeof(char)*(MAX_DIFF_SIZE/2));
    char* mystr = (char*)malloc(sizeof(char)*(MAX_DIFF_SIZE+20));
    result = getDataContentLast(data);
    sprintf(mystr,"<root>%s</root>", result);
    free(result);
    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    partial_data = ezxml_get(root,tagName,-1);
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        free(mystr);
        ezxml_free(root);
        return NULL;
    }
    //printf("partial_data: %s\n",partial_data->txt);
    sprintf(partial_result,"%s",partial_data->txt);
    //printf("partial_result: %s\n\n",partial_result);
    free(mystr);
    ezxml_free(root);
    return partial_result;
}

int setNewDataContent(Data* data, char* diffContent){
    
    if(data->content->versionKeeped==0){
        printf("\n\n[ Data #0 ]\n");
        data->content->versionKeeped=1;
        DataContent *dc = (DataContent*)malloc(sizeof(DataContent));
        data->content->head = dc;
        data->content->lastestCommon = dc;
        data->content->current = dc;    // current cursor
        
        dc->preVersion = NULL;
        dc->nextVersion = NULL;
        dc->isDiff = 0;
        
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc->objContent = obj_ct;
        char tmp[] = "<book_id><author><title><genre><price><publish_date><description>";
        obj_ct->xmlSchema = strdup(tmp);
        obj_ct->data = strdup(diffContent);
        obj_ct->byteCount = strlen(obj_ct->data);
        
        return obj_ct->byteCount;
    }
    
    else {
        string s;
        if((data->content->versionKeeped)%FULL_CONTENT!=0){
            char* result;
            result = getDataContentLast(data);
            printf("\nresult_before_diff: %s\n",result);
            s = getDiff(result,diffContent);
            free(result);
        }
        
        printf("\n\n[ Data #%d ]\n",data->content->versionKeeped);
        data->content->versionKeeped++;
        DataContent *dc = (DataContent*)malloc(sizeof(DataContent));
        
        // change ptr new dc preVersion point to old dc
        dc->preVersion = data->content->lastestCommon;
        
        // change ptr old dc nextVersion point to new dc
        data->content->lastestCommon->nextVersion = dc;
        
        // change ptr lastestCommon point to new dc
        data->content->lastestCommon = dc;
        
        // ptr new dc nextVersion is NULL
        dc->nextVersion = NULL;
        
        data->content->current = dc;    // current cursor
        
        if((data->content->versionKeeped-1)%FULL_CONTENT!=0){
            dc->isDiff = 1;
        }
        else {
            dc->isDiff = 0;
        }
        
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc->objContent = obj_ct;
        char tmp[] = "<book_id><author><title><genre><price><publish_date><description>";
        obj_ct->xmlSchema = strdup(tmp);

        if((data->content->versionKeeped-1)%FULL_CONTENT!=0){
            obj_ct->data = strdup(s.c_str());
            printf("result_after_diff: %s\n",obj_ct->data);
        }
        else {
            obj_ct->data = strdup(diffContent);
            printf("result: %s\n",obj_ct->data);
        }
        obj_ct->byteCount = strlen(obj_ct->data);
        
        return obj_ct->byteCount;
    }
}

int setNewDataDiffWithTag(Data* data, char* tagName, char* diff){
    int ret;
    char* last_dc;                      // string last DataContent
    char root_xml[MAX_DIFF_SIZE+20];    // xml last dc with <root>
    char* tag_before;                   // tag line (before)
    char tag_after[MAX_DIFF_SIZE/2];    // tag line (after)
    char* temp_result;                  // xml modified last dc with <root>
    char final_result[MAX_DIFF_SIZE];   // result of function!!
    
    last_dc = getDataContentLast(data);
    sprintf(root_xml,"<root>%s</root>", last_dc);
    free(last_dc);
    
    ezxml_t root = ezxml_parse_str(root_xml,(unsigned long)strlen(root_xml));
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    ezxml_t partial_data = ezxml_get(root,tagName,-1);
    
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        ezxml_free(root);
        return -1;
    }
    
    tag_before = ezxml_toxml(partial_data);
//    printf("\n\ntag_before: %s\n\n", tag_before);
    
    string s = getPatch(tag_before,diff);
    sprintf(tag_after,"%s",s.c_str());
//    printf("tag_after: %s\n\n", tag_after);
    ezxml_t temp = ezxml_parse_str(tag_after,(unsigned long)strlen(tag_after));
    
    // เอาไปแทรก data เดิม
    ezxml_set_txt(partial_data,temp->txt);
    temp_result = ezxml_toxml(root);
//    printf("\n\ntemp_result: %s\n\n", temp_result);

    // เอา tag <root></root> ออก
    memcpy(final_result,temp_result+6,strlen(temp_result)-8-6+1);
//    printf("final_result: %s\n\n",final_result);
    
    // เพิ่มเป็น data ใหม่
    printf("dataID: %s\n",data->dataID);
    ret = setNewDataContent(data, final_result);
    
    free(tag_before);
    free(temp_result);
    ezxml_free(temp);
    ezxml_free(root);
    return ret;
}

//int addDataToCategory(char* categoryID, Data *data){
//    int ret;
//    char sql[MAX_SQL_SIZE];
//    char* rid_cat;
//    char* rid_data;
//    char* rid_dc;
//    char* rid_dh;
//
//    printf("--------------------------------------------------[get @rid Category]\n");
//    sprintf(sql,"SELECT @rid from Category where categoryID ='%s'",categoryID);
//    rid_cat = getRid(sql);
//    
//    printf("--------------------------------------------------[get @rid Data]\n");
//    sprintf(sql,"SELECT @rid from Data where dataID ='%s'",data->dataID);
//    rid_data = getRid(sql);
//    
//    printf("--------------------------------------------------[get @rid DataHolder]\n");
//    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from #%s)",rid_data);
//    rid_dh = getRid(sql);
//    
//    printf("--------------------------------------------------[create_edge_toData]\n");
//    sprintf(sql,"CREATE EDGE toData from #%s to #%s",rid_cat,rid_data);
//    ret = sendCommand(sql);
//
//    // เอา userID
//    char* result;
//    char* uuidstr = (char*)malloc(sizeof(char)*33);
//    printf("--------------------------------------------------[get userID]\n");
//    sprintf(sql,"SELECT userID from (SELECT expand(in('toCategory')) from Category where categoryID='%s')",categoryID);
//    result = getContent(sql);
//    //printf("result: %s\n", result);
//    memcpy(uuidstr,result+8,strlen(result)-8-1);
//    uuidstr[32] = '\0';
//    
//    // หา nodeRef
//    printf("--------------------------------------------------[get @rid Last_DataContent]\n");
//    sprintf(sql,"SELECT max(@rid) from (select expand(out('toDataContent')) from #%s)",rid_dh);
//    rid_dc = getRid(sql);
//    
//    // เอาข้อมูลลง struct DeviceRef
//    data->content->userDevicePtr[data->content->userCount] = (DeviceRef*)malloc(sizeof(DeviceRef));
//    data->content->userDevicePtr[data->content->userCount]->userID = uuidstr;
//    data->content->userDevicePtr[data->content->userCount]->deviceTransportID = stringUUID();
//    sprintf(data->content->userDevicePtr[data->content->userCount]->nodeRefToDataContent,"%s",rid_dc);
//    //printf("len userID: %d\n",strlen(data->content->userDevicePtr[data->content->userCount]->userID));
//    printf("userID: %s\n",data->content->userDevicePtr[data->content->userCount]->userID);
//    //printf("len deviceTransportID: %d\n",strlen(data->content->userDevicePtr[data->content->userCount]->deviceTransportID));
//    printf("deviceTransportID: %s\n",data->content->userDevicePtr[data->content->userCount]->deviceTransportID);
//    printf("nodeRef: %s\n",data->content->userDevicePtr[data->content->userCount]->nodeRefToDataContent);
//
//    // สร้าง vertex DeviceRef
//    printf("--------------------------------------------------[create_vertex_DeviceRef]\n");
//    sprintf(sql,"CREATE VERTEX DeviceRef set userID='%s', deviceTransportID='%s', nodeRefToDataContent='#%s'",data->content->userDevicePtr[data->content->userCount]->userID,data->content->userDevicePtr[data->content->userCount]->deviceTransportID,data->content->userDevicePtr[data->content->userCount]->nodeRefToDataContent);
//    //printf("SQL: %s\n",sql);
//    ret = createVertex(sql);
//    if (ret!=0){
//        printf("addDataToCategory..FAILED(vertex_DeviceRef)\n");
//        return 1;
//    }
//    data->content->userCount++;
//    printf("--------------------------------------------------[update_userCount]\n");
//    sprintf(sql,"UPDATE #%s set userCount='%d'",rid_dh,data->content->userCount);
//    ret = sendCommand(sql);
//    if (ret!=0){
//        printf("addDataToCategory..FAILED(update_userCount)\n");
//        return 1;
//    }
//    
//    short cltid;
//    long rid;
//    cltid = addr.cltid;
//    rid = addr.rid;
//    
//    // สร้าง edge จาก DeviceRef -> DataHolder
//    printf("--------------------------------------------------[create_edge_fromDeviceRef]\n");
//    sprintf(sql,"CREATE EDGE fromDeviceRef from #%d:%lu to #%s",cltid,rid,rid_dh);
//    //printf("SQL: %s\n",sql);
//    ret = sendCommand(sql);
//    
//    free(result);
//    free(rid_cat);
//    free(rid_data);
//    free(rid_dc);
//    free(rid_dh);
//    
//    if (ret!=0){
//        printf("addDataToCategory..FAILED(edge_fromDeviceRef)\n");
//        return 1;
//    }
//    else
//        return 0;
//}

//Data* queryDataByID(char* dataID){
//    Data* d = (Data*)malloc(sizeof(Data));
//    char sql[MAX_SQL_SIZE];
//    char *result;
//    sprintf(sql,"select chatRoom from Data where dataID = '%s'",dataID);
//    result = getContent(sql);
//    
//    if(result==NULL){
//        printf("FAILED >> incorrect dataID\n");
//        free(result);
//        free(d);
//        return NULL;
//    }
//    printf("\n\n");
//    //printf("result: %s\n",result);
//    
//    char *uuidstr = (char*)malloc(sizeof(char)*33);
//    memcpy(uuidstr,result+10,strlen(result)-10-1);
//    uuidstr[32] = '\0';
//    d->dataID = dataID;
//    d->chatRoom = uuidstr;
//    printf("dataID: %s\n",d->dataID);
//    printf("chatRoom: %s\n",d->chatRoom);
//
//    char* rid_dh;
//    char* result2;
//    
//    printf("--------------------------------------------------[get @rid DataHolder]\n");
//    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
//    rid_dh = getRid(sql);
//    
//    if(rid_dh == NULL){
//        printf("queryDataByID..FAILED(get @rid DataHolder)\n");
//        free(result);
//        free(result2);
//        free(rid_dh);
//        free(d);
//        return NULL;
//    }
//    
//    printf("--------------------------------------------------[get ver,usrCount]\n");
//    sprintf(sql,"SELECT versionKeeped,userCount from #%s)",rid_dh);
//    
//    result2 = getContent(sql);
//    
//    if(result2 == NULL){
//        printf("queryDataByID..FAILED(get ver,usrCount)\n");
//        free(result);
//        free(result2);
//        free(rid_dh);
//        free(d);
//        return NULL;
//    }
//    
//    // หั่น data เอามาใส่ struct
//    char delim1[2] = ":";
//    char delim2[2] = ",";
//    char delim3[2] = "\"";
//    char* token;
//    char str[10];
//    
//    d->content = createNewDataHolder();
//    
//    token = strtok(result2, delim1);
//    token = strtok(NULL, delim2);
//    //printf("token1: %s\n", token);
//    d->content->versionKeeped = atoi(token);
//    
//    token = strtok(NULL, delim1);
//    token = strtok(NULL, delim1);
//    //printf("token2: %s\n", token);
//    d->content->userCount = atoi(token);
//    printf("ver: %d\nusrCount: %d\n",d->content->versionKeeped,d->content->userCount);
//
//    int i;
//    int count = d->content->versionKeeped;
//    printf("count: %d\n",count);
//    
//    DataContent** dcPtr = (DataContent**)malloc(sizeof(DataContent*)*count);
//    for(i=0;i<count;i++)
//        dcPtr[i] = (DataContent*)malloc(sizeof(DataContent));
//    
//    char* rid_dc[count];
//    printf("--------------------------------------------------[get @rid 1st DataContent]\n");
//    sprintf(sql,"SELECT min(@rid) from (select expand(out('toDataContent')) from #%s)",rid_dh);
//    rid_dc[0] = getRid(sql);
//    printf("rid[0]: %s\n",rid_dc[0]);
//    
//    char* result_dc[count];
//    
//    for(i=0;i<count;i++){
//        printf("--------------------------------------------------[get isDiff,diff DataContent]\n");
//        sprintf(sql,"SELECT isDiff,diffContent from #%s",rid_dc[i]);
//        result_dc[i] = getContent(sql);
//        //printf("result_dc[i]: %s\n---------------------\n",result_dc[i]);
//        
//        token = strtok(result_dc[i], delim1);
//        token = strtok(NULL, delim2);
//        
//        if(strcmp(token,"true")==0)
//            dcPtr[i]->isDiff = 1;
//        else
//            dcPtr[i]->isDiff = 0;
//        
//        printf("isDiff: %d\n",dcPtr[i]->isDiff);
//        token = strtok(NULL, "\"");
//        token = strtok(NULL, "\"");
//        sprintf(dcPtr[i]->diffContent,"%s",token);
//        printf("diffContent: %s\n",dcPtr[i]->diffContent);
//        
//        if(i==0){
//            dcPtr[i]->preVersion = NULL;
//            dcPtr[i]->nextVersion = dcPtr[i+1];
//        }
//        else if(i+1==count){
//            dcPtr[i]->preVersion = dcPtr[i-1];
//            dcPtr[i]->nextVersion = NULL;
//        }
//        else {
//            dcPtr[i]->preVersion = dcPtr[i-1];
//            dcPtr[i]->nextVersion = dcPtr[i+1];
//        }
//
//        if(i+1!=count){
//            printf("--------------------------------------------------[get @rid DataContent]\n");
//            sprintf(sql,"SELECT max(@rid) from (select expand(out('toDiffContent')) from #%s)",rid_dc[i]);
//            //printf("SQL: %s\n",sql);
//            rid_dc[i+1] = getRid(sql);
//            printf("rid[%d]: %s\n",i+1,rid_dc[i+1]);
//        }
//    }
//    
//    // ออกจากลูป แล้วค่อยกำหนด head-last
//    d->content->head = dcPtr[0];
//    d->content->lastestCommon = dcPtr[count-1];
//    
//    char* result_data[d->content->userCount];
//    char** result_rid;
//    
//    sprintf(sql,"SELECT out from fromDeviceRef where in = #%s",rid_dh);
//    result_rid = getArrayRid(sql);
//    
//    // DeviceRef
//    for(i=0;i<d->content->userCount;i++){
//        sprintf(sql,"SELECT userID,deviceTransportID,nodeRefToDataContent from %s",result_rid[i]);
//        printf("SQL: %s\n",sql);
//        result_data[i] = getContent(sql);
//        printf("result_data[%d]: %s\n",i,result_data[i]);
//        
//        d->content->userDevicePtr[i] = (DeviceRef*)malloc(sizeof(DeviceRef));
//        
//        token = strtok(result_data[i], delim3);
//        token = strtok(NULL, delim3);
//        printf("token_inner: %s\n", token);
//        d->content->userDevicePtr[i]->userID = strdup(token);
//        //d->content->userDevicePtr[i]->userID = token; (fix)
//        
//        token = strtok(NULL, delim3);
//        token = strtok(NULL, delim3);
//        printf("token_inner: %s\n", token);
//        d->content->userDevicePtr[i]->deviceTransportID = strdup(token);
//        //d->content->userDevicePtr[i]->deviceTransportID = token;  (fix)
//        
//        token = strtok(NULL, delim3);
//        token = strtok(NULL, delim3);
//        printf("token_inner: %s\n", token);
//        
//        sprintf(d->content->userDevicePtr[i]->nodeRefToDataContent,"%s",token);
//        
//        printf("\nuserID: %s\n",d->content->userDevicePtr[i]->userID);
//        printf("deviceTransportID: %s\n",d->content->userDevicePtr[i]->deviceTransportID);
//        printf("nodeRef: %s\n",d->content->userDevicePtr[i]->nodeRefToDataContent);
//    }
//    
//    for(i=0;result_rid[i]!=NULL;i++){
//        free(result_rid[i]);
//        free(result_data[i]);
//    }
//    free(result_rid);
//
//    for(i=0;i<count;i++){
//        //free(dcPtr[i]);
//        free(result_dc[i]);
//        free(rid_dc[i]);
//    }
//    
//    free(dcPtr);
//    free(result);
//    free(result2);
//    free(rid_dh);
//    
//    return d;
//}

//Category** queryCategoryByID(char* dataID){
//    char sql[MAX_SQL_SIZE];
//    char* count;
//    char* token;
//    char delim1[2] = ":";
//    char delim2[2] = "\"";
//    int i, num;
//    
//    sprintf(sql,"SELECT count(*) from (select expand(in('toData')) from Data where dataID='%s')",dataID);
//    //printf("SQL: %s\n",sql);
//    count = getContent(sql);
//    token = strtok(count, delim1);
//    //printf("token1: %s\n", token);
//    token = strtok(NULL, "l");
//    //printf("token2: %s\n", token);
//    num = atoi(token);
//    printf("num: %d\n",num);
//    
//    Category** catPtr = (Category**)malloc(sizeof(Category*)*num+1);
//    for(i=0;i<num;i++)
//        catPtr[i] = (Category*)malloc(sizeof(Category));
//    
//    char* result_data[num];
//    char** result_rid;
//    sprintf(sql,"SELECT @rid from (select expand(in('toData')) from Data where dataID='%s')",dataID);
//    result_rid = getArrayRid(sql);
//    
//    for(i=0;i<num;i++){
//        sprintf(sql,"SELECT categoryID,categoryName from %s",result_rid[i]);
//        printf("SQL: %s\n",sql);
//        result_data[i] = getContent(sql);
//        token = strtok(result_data[i], delim2);
//        token = strtok(NULL, delim2);
//        //printf("catID: %s\n",token);
//        catPtr[i]->categoryID = strdup(token);
//        //catPtr[i]->categoryID = token;    (fix)
//        token = strtok(NULL, delim2);
//        token = strtok(NULL, delim2);
//        //printf("catName: %s\n",token);
//        sprintf(catPtr[i]->categoryName,"%s",token);
//        
//        printf("\ncatID: %s\n",catPtr[i]->categoryID);
//        printf("catName: %s\n",catPtr[i]->categoryName);
//    }
//    catPtr[i] = NULL;
//    
//    for(i=0;result_rid[i]!=NULL;i++){
//        free(result_rid[i]);
//    }
//    free(result_rid);
//    
//    for(i=0;i<num;i++){
//        free(result_data[i]);
//    }
//    
//    free(count);
//    
//    return catPtr;
//}

//User** queryUserByID(char* dataID){
//    char sql[MAX_SQL_SIZE];
//    char* count;
//    char* token;
//    char delim1[2] = ":";
//    char delim2[2] = "\"";
//    int i, num;
//    
//    sprintf(sql,"SELECT count(*) from (select expand(in('toData').in('toCategory')) from Data where dataID='%s')",dataID);
//    //printf("SQL: %s\n",sql);
//    count = getContent(sql);
//    token = strtok(count, delim1);
//    //printf("token1: %s\n", token);
//    token = strtok(NULL, "l");
//    //printf("token2: %s\n", token);
//    num = atoi(token);
//    printf("num: %d\n",num);
//    
//    User** usrPtr = (User**)malloc(sizeof(User*)*num+1);
//    for(i=0;i<num;i++)
//        usrPtr[i] = (User*)malloc(sizeof(User));
//    
//    char* result_data[num];
//    char** result_rid;
//    sprintf(sql,"SELECT @rid from (select expand(in('toData').in('toCategory')) from Data where dataID='%s')",dataID);
//    result_rid = getArrayRid(sql);
//    
//    for(i=0;i<num;i++){
//        sprintf(sql,"SELECT userID,userName from %s",result_rid[i]);
//        printf("SQL: %s\n",sql);
//        result_data[i] = getContent(sql);
//        token = strtok(result_data[i], delim2);
//        token = strtok(NULL, delim2);
//        usrPtr[i]->userID = strdup(token);
//        //usrPtr[i]->userID = token;
//        token = strtok(NULL, delim2);
//        token = strtok(NULL, delim2);
//        sprintf(usrPtr[i]->userName,"%s",token);
////        printf("\nuserID: %s\n",usrPtr[i]->userID);
////        printf("userName: %s\n",usrPtr[i]->userName);
//    }
//    usrPtr[i] = NULL;
//    
//    for(i=0;result_rid[i]!=NULL;i++){
//        free(result_rid[i]);
//    }
//    free(result_rid);
//    
//    for(i=0;i<num;i++){
//        free(result_data[i]);
//    }
//    free(count);
//    
//    return usrPtr;
//}

string getDiff(char* old_str, char* new_str){
//    printf("len old: %lu\n",strlen(old_str));
//    printf("%s\n",old_str);
//    printf("len new: %lu\n",strlen(new_str));
//    printf("%s\n",new_str);
    diff_match_patch<wstring> dmp;
    string s;
    wstring strPatch;
    wstring str1 = wstring(new_str,new_str+strlen(new_str));
    //wcout << str1 << endl;
    wstring str0 = wstring(old_str,old_str+strlen(old_str));
    //wcout << str0 << endl;
    strPatch = dmp.patch_toText(dmp.patch_make(str0, str1));
    s.assign(strPatch.begin(),strPatch.end());
    //wcout << strPatch << endl;
    return s;
}

string getPatch(char* str, char* str_patch){
    diff_match_patch<wstring> dmp;
    string s;
    wstring strResult;
    strResult = wstring(str,str+strlen(str));
    pair<wstring, vector<bool> > out
    = dmp.patch_apply(dmp.patch_fromText(wstring(str_patch,str_patch+strlen(str_patch))), strResult);
    strResult = out.first;
    return s.assign(strResult.begin(),strResult.end());
}

//----------------------------------------------------------------------------------------------------------

//int testUUID(uuid_t* uuid) {
//    
//    //    const char* uuidstr;
//    //    uuidstr = stringUUID(*uuid);
//    //    printf("uuid: %s\n",uuidstr);
//    
//    printf("size: %lu\n", sizeof(uuid_t));
//    
//    int ret, size=0;
//    GPacket.opType = REQUEST_COMMAND;
//    size += addByte('s', GPacket.msg);
//    size += addInt(5+4+39+sizeof(uuid_t)+22+3, GPacket.msg+size);
//    size += addString("c", GPacket.msg+size);
//    size += addInt(56+22, GPacket.msg+size);
//    //    size += addInt(5+4+71+22+3, GPacket.msg+size);
//    //    size += addString("c", GPacket.msg+size);
//    //    size += addInt(71+22+1, GPacket.msg+size);
//    //    memcpy(GPacket.msg+size, "CREATE VERTEX Category set categoryID='C94C90973EE0469485337D18DE5F8B95", 71);
//    //    size += 71;
//    memcpy(GPacket.msg+size, "CREATE VERTEX Category set categoryID='", 39);
//    size += 39;
//    memcpy(GPacket.msg+size,uuid,sizeof(uuid_t));
//    size += sizeof(uuid_t);
//    
//    memcpy(GPacket.msg+size, "', categoryName='Image", 22);
//    size += 22;
//    
//    size += addByte('\'', GPacket.msg+size);
//    size += addByte(0, GPacket.msg+size);
//    size += addByte(0, GPacket.msg+size);
//    ret = write(Sockfd, &GPacket, 5+size);
//    
//    ret = read(Sockfd, &GPacket, 5+256);
//    if (ret<0 || GPacket.opType!=0){
//        printf("Failed\n");
//        return 1;
//    }
//    else
//        return 0;
//}
//
//void testDMP(){
//    diff_match_patch<wstring> dmp;
//    wstring str1 = L"<book_id>bk101</book_id><author>Gambardella, Matthew</author><title>XML Developer's Guide</title><genre>Computer</genre><price>44.95</price><publish_date>2000-10-01</publish_date><description>An in-depth look at creating applications with XML.</description>";
//    wstring str2 = L"<book_id>bk101</book_id><author>Ralls, Kim</author><title>Midnight Rain</title><genre>Fantasy</genre><price>5.95</price><publish_date>2000-12-16</publish_date><description>A former architect battles corporate zombies, an evil sorceress, and her own childhood to become queen of the world.</description>";
//    
//	wcout << "\nstr1: " << str1 << "\n\n";
//	wcout << "str2: " << str2 << "\n\n";
//    string s;
//    wstring strPatch = dmp.patch_toText(dmp.patch_make(str1, str2));
//	wcout << "patch: " << strPatch << "\n";
//    
//    char str1_c[MAX_DIFF_SIZE], str_patch[MAX_DIFF_SIZE], result[MAX_DIFF_SIZE];
//    sprintf(str1_c,"%ls",str1.c_str());
//    sprintf(str_patch,"%ls",strPatch.c_str());
//    
//    printf("str1_c: %s\n",str1_c);
//    printf("str_patch: %s\n",str_patch);
//    
//    sprintf(result,"%s",getPatch(str1_c,str_patch).c_str());
//    printf("result: %s\n",result);
//}
//
//void testUserCategory(int x){
//    const char* uuid_usr;
//    const char* uuid_cat;
//    
//    if(x==1){
//        uuid_usr = createUser((char*)"Tanapon");
//        uuid_cat = createCategory((char*)"Games",(char*)uuid_usr);
//    }
//    
//    else if (x==2){
//        uuid_usr = createUser((char*)"Tanapoom");
//        uuid_cat = createCategory((char*)"Musics",(char*)uuid_usr);
//    }
//    free((char*)uuid_usr);
//    free((char*)uuid_cat);
//}
//
//void testTag(Data* data){
//    int ret;
//    char mystr[1000];
//    char content0[MAX_DIFF_SIZE] = "<book_id>bk100</book_id><author>Gambardella, Matthew</author><title>XML Developer's Guide</title><genre>Computer</genre><price>44.95</price><publish_date>2000-10-01</publish_date><description>An in-depth look at creating applications with XML.</description>";
//    
//    char content7[MAX_DIFF_SIZE] = "<book_id>bk107</book_id><author>Knorr, Stefan</author><title>Creepy Crawlies</title><genre>Horror</genre><price>4.95</price><publish_date>2000-12-06</publish_date><description>An anthology of horror stories about roaches, centipedes, scorpions and other insects.</description>";
//    
//    sprintf(mystr,"<root>%s</root>",content7);
//    
//    char content1[100] = "<author>Pimpat, Stefan</author>";
//    char* partial_result;
//    char diff_result[1000];
//
//    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
//    //printf("xml_raw: %s\n",ezxml_toxml(root));
//    
//    partial_data = ezxml_get(root,"author",-1);
//    if(partial_data==NULL) {
//        printf("FAILED >> incorrect tag\n");
//        ezxml_free(root);
//    }
//    partial_result = ezxml_toxml(partial_data);
//    printf("\npartial_result: %s\n", partial_result);
//    sprintf(diff_result,"%s",getDiff(partial_result,content1).c_str());
//    printf("\ndiff_result: %s\n", diff_result);
//
//    ret = setNewDataDiffWithTag(data, "author", diff_result);
//    printf(">>>number of byte(8)=\t%d\n",ret);
//    
//    free(partial_result);
//    ezxml_free(root);
//    
////    clock_t start, end, sum1, sum2;
////    
////    start = clock();
////    partial_data = ezxml_child(root,"author");
////    end = clock();
////    sum1 = end-start;
////    //printf("\npartial_data: %s\n\n",partial_data->txt);
////    
//////    ezxml_t cut = ezxml_cut(partial_data);
//////    printf("xml_partial: %s\n",ezxml_toxml(partial_data));
//////    printf("xml_cut: %s\n",ezxml_toxml(cut));
////    
////    start = clock();
////    partial_data = ezxml_get(root,"author",-1);
////    end = clock();
////    sum2 = end-start;
////    //printf("\npartial_data: %s\n\n",partial_data->txt);
////    
////    printf("time1: %f\n",(double)sum1/CLOCKS_PER_SEC);
////    printf("time2: %f\n",(double)sum2/CLOCKS_PER_SEC);
////    ezxml_free(root);
//    
//}
//
void testsetNewDataContent(){
//    const char* uuid_usr;
//    const char* uuid_cat;
    const char* uuid_data;
//    uuid_usr = createUser((char*)"Pimpat");
//    uuid_cat = createCategory((char*)"Docs",(char*)uuid_usr);
    
    uuid_data = createNewData();
    printf("my_uuid: %s\n",uuid_data);
    printf("ver_keeped: %d\n",mydata->content->versionKeeped);
    
    char content0[MAX_DIFF_SIZE] = "<book_id>bk100</book_id><author>Gambardella, Matthew</author><title>XML Developer's Guide</title><genre>Computer</genre><price>44.95</price><publish_date>2000-10-01</publish_date><description>An in-depth look at creating applications with XML.</description>";
    
    char content1[MAX_DIFF_SIZE] = "<book_id>bk101</book_id><author>Ralls, Kim</author><title>Midnight Rain</title><genre>Fantasy</genre><price>5.95</price><publish_date>2000-12-16</publish_date><description>A former architect battles corporate zombies, an evil sorceress, and her own childhood to become queen of the world.</description>";
    
    char content2[MAX_DIFF_SIZE] = "<book_id>bk102</book_id><author>Corets, Eva</author><title>Maeve Ascendant</title><genre>Fantasy</genre><price>5.95</price><publish_date>2000-11-17</publish_date><description>After the collapse of a nanotechnology society in England, the young survivors lay the foundation for a new society.</description>";
    
    char content3[MAX_DIFF_SIZE] = "<book_id>bk103</book_id><author>Corets, Eva</author><title>Oberon's Legacy</title><genre>Fantasy</genre><price>5.95</price><publish_date>2001-03-10</publish_date><description>In post-apocalypse England, the mysterious agent known only as Oberon helps to create a new life for the inhabitants of London. Sequel to Maeve Ascendant.</description>";
    
    char content4[MAX_DIFF_SIZE] = "<book_id>bk104</book_id><author>Corets, Eva</author><title>The Sundered Grail</title><genre>Fantasy</genre><price>5.95</price><publish_date>2001-09-10</publish_date><description>The two daughters of Maeve, half-sisters, battle one another for control of England. Sequel to Oberon's Legacy.</description>";
    
    char content5[MAX_DIFF_SIZE] = "<book_id>bk105</book_id><author>Randall, Cynthia</author><title>Lover Birds</title><genre>Romance</genre><price>4.95</price><publish_date>2000-09-02</publish_date><description>When Carla meets Paul at an ornithology conference, tempers fly as feathers get ruffled.</description>";
    
    char content6[MAX_DIFF_SIZE] = "<book_id>bk106</book_id><author>Thurman, Paula</author><title>Splish Splash</title><genre>Romance</genre><price>4.95</price><publish_date>2000-11-02</publish_date><description>A deep sea diver finds true love twenty thousand leagues beneath the sea.</description>";
    
    char content7[MAX_DIFF_SIZE] = "<book_id>bk107</book_id><author>Knorr, Stefan</author><title>Creepy Crawlies</title><genre>Horror</genre><price>4.95</price><publish_date>2000-12-06</publish_date><description>An anthology of horror stories about roaches, centipedes, scorpions and other insects.</description>";
    
    int ret;
    ret = setNewDataContent(mydata,content0);
    printf(">>>number of byte(0)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content1);
    printf(">>>number of byte(1)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content2);
    printf(">>>number of byte(2)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content3);
    printf(">>>number of byte(3)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content4);
    printf(">>>number of byte(4)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content5);
    printf(">>>number of byte(5)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content6);
    printf(">>>number of byte(6)=\t%d\n",ret);
    ret = setNewDataContent(mydata,content7);
    printf(">>>number of byte(7)=\t%d\n",ret);
    
    char** result = (char**)malloc(sizeof(char*)*10);
    
    result[0] = getDataContentVer(mydata,0);
    printf("result[0]: %s\n\n",result[0]);
    
    result[1] = getDataContentVer(mydata,1);
    printf("result[1]: %s\n\n",result[1]);
    
    result[2] = getDataContentVer(mydata,2);
    printf("result[2]: %s\n\n",result[2]);
    
    result[3] = getDataContentVer(mydata,3);
    printf("result[3]: %s\n\n",result[3]);
    
    result[4] = getDataContentVer(mydata,4);
    printf("result[4]: %s\n\n",result[4]);
    
    result[5] = getDataContentVer(mydata,5);
    printf("result[5]: %s\n\n",result[5]);
    
    result[6] = getDataContentVer(mydata,6);
    printf("result[6]: %s\n\n",result[6]);
    
    result[7] = getDataContentVer(mydata,7);
    printf("result[7]: %s\n\n",result[7]);
    
    result[8] = getDataContentVer(mydata,8);
    printf("result[NULL]: %s\n\n",result[8]);

    printf("\n\n");
    result[8] = getDataContentFirst(mydata);
    printf("result[head]: %s\n\n",result[8]);
    
    result[9] = getDataContentLast(mydata);
    printf("result[last]: %s\n\n",result[9]);
    
//    addDataToCategory((char*)uuid_cat,mydata);
//    
//    char* partial_result;
//    partial_result = getDataContentWithTag(mydata,"price");
//    printf("partial_result(from tag): %s\n\n",partial_result);
//    
//    testTag(mydata);
//    
//    result[8] = getDataContentVer(mydata,8);
//    printf("result[8]: %s\n\n",result[8]);
//    
//    free((char*)mydata->content->userDevicePtr[mydata->content->userCount-1]->deviceTransportID);
//    free((char*)mydata->content->userDevicePtr[mydata->content->userCount-1]->userID);
//    free((char*)mydata->content->userDevicePtr[mydata->content->userCount-1]);
    
//    int j;
//    for(j=0;j<10;j++){
//        mydata->content->userDevicePtr[j] = (DeviceRef*)malloc(sizeof(DeviceRef));
//        mydata->content->userDevicePtr[j]->test_num = j;
//        printf("test_num[%d]: %d\n",j,mydata->content->userDevicePtr[j]->test_num);
//        
//    }

    int check;
    check = getCurLevelFromHead(mydata);
    printf("head: %d\n\n",check);
    check = getCurLevelFromCommon(mydata);
    printf("common: %d\n",check);
    
    //  Test getDiff Head/Last/Next/Pre
/*
    char* temp;
    temp = getDiffDataAtHead(mydata);
    printf("head: %s\n",temp);
    temp = getDiffDataAtLastestCommon(mydata);
    printf("last: %s\n",temp);
    
    mydata->content->current = mydata->content->current->preVersion;
    temp = getDiffDataNextVer(mydata);
    printf("next: %s\n",temp);
    temp = getDiffDataPreVer(mydata);
    printf("pre: %s\n",temp);
*/
    
    //  Test getContentPre/NextVer
/*
    ObjectBinary* myobj;
    mydata->content->current = mydata->content->head;
    myobj = getContentPreVer(mydata);
    if(myobj==NULL){
        printf("\n\nthis is 1st data\n");
    }
    mydata->content->current = mydata->content->lastestCommon->preVersion->preVersion->preVersion->preVersion->preVersion->preVersion->preVersion;
    myobj = getContentPreVer(mydata);
    
    if(myobj!=NULL){
        printf("data: %s\n",myobj->data);
        free(myobj);
    }
    else
        printf("this is 1st data\n");
*/
    
    //  Test getDataContent/Commonversion
/*
    ObjectBinary* obj_temp;
    ObjectBinary* obj_temp2;
    obj_temp = getDataContentCommonVersion(mydata);
    printf("xmlSchema: %s\n",obj_temp->xmlSchema);
    printf("byteCount: %d\n",obj_temp->byteCount);
    printf("data: %s\n",obj_temp->data);
    
    printf("\ntest_current: %s\n",mydata->content->current->objContent->data);
    printf("test_current(byte): %d\n",mydata->content->current->objContent->byteCount);
    
    free(obj_temp->xmlSchema);
    free(obj_temp->data);
    free(obj_temp);
    
    obj_temp2 = getDataContent(mydata);
    printf("xmlSchema: %s\n",obj_temp2->xmlSchema);
    printf("byteCount: %d\n",obj_temp2->byteCount);
    printf("data: %s\n",obj_temp2->data);
    
    free(obj_temp2->xmlSchema);
    free(obj_temp2->data);
    free(obj_temp2);
*/

    int i;
    for(i=0;i<10;i++){
        if(i!=8)
        free(result[i]);
        //free(mydata->content->userDevicePtr[i]);
    }
    free(result);

//    free(partial_result);
//    free((char*)uuid_cat);
//    free((char*)uuid_usr);
    
    printf("\n\n");
    DataContent *mydc, *next_mydc;
    for(mydc=mydata->content->head;mydc!=NULL;mydc=next_mydc){
        next_mydc = mydc->nextVersion;
        printf("byteCount: %d\n",mydc->objContent->byteCount);
        free(mydc->objContent->xmlSchema);
        free(mydc->objContent->data);
        free(mydc->objContent);
        free(mydc);
    }
    
    //free(next_mydc);
    //free(mydc);
    free(mydata->content);
    free((char*)mydata->dataID);
    free((char*)mydata->chatRoom);
    free(mydata);
    
}
//
//void testCountByte(){
//    char content0[MAX_DIFF_SIZE] = "<book_id>bk100</book_id><author>Gambardella, Matthew</author><title>XML Developer's Guide</title><genre>Computer</genre><price>44.95</price><publish_date>2000-10-01</publish_date><description>An in-depth look at creating applications with XML.</description>";
//    
//    char content1[MAX_DIFF_SIZE] = "<book_id>bk101</book_id><author>Ralls, Kim</author><title>Midnight Rain</title><genre>Fantasy</genre><price>5.95</price><publish_date>2000-12-16</publish_date><description>A former architect battles corporate zombies, an evil sorceress, and her own childhood to become queen of the world.</description>";
//    
//    printf("\n\nsize0: %lu\n",sizeof(content0));
//    printf("len0: %lu\n",strlen(content0));
//    printf("size1: %lu\n",sizeof(content1));
//    printf("len1: %lu\n",strlen(content1));
//    
//}
//
//void testqueryDataByID(char* myID, char* mycatID){
//    Data* data = queryDataByID(myID);
//    addDataToCategory(mycatID,data);
//
//    int i, count;
//    count = data->content->userCount;
//    printf("count: %d\n",count);
//    
//    for(i=0;i<count;i++){
//        printf("\nuserID: %s\n",data->content->userDevicePtr[i]->userID);
//        printf("deviceTransportID: %s\n",data->content->userDevicePtr[i]->deviceTransportID);
//        printf("nodeRef: %s\n",data->content->userDevicePtr[i]->nodeRefToDataContent);
//        free((char*)data->content->userDevicePtr[i]->userID);
//        free((char*)data->content->userDevicePtr[i]->deviceTransportID);
//        free(data->content->userDevicePtr[i]);
//    }
//
//    if(data!=NULL){
//        DataContent *mydc, *next_mydc;
//        for(mydc=data->content->head;mydc!=NULL;mydc=next_mydc){
//            next_mydc = mydc->nextVersion;
//            free(mydc);
//        }
//
//        //free((char*)data->dataID);
//        free(data->content);
//        free((char*)data->chatRoom);
//        free(data);
//    }
//}
//
//void testArrayRid(){
//    char** result_rid;
//    result_rid = getArrayRid("select in from toDataContent where out = #17:16");
//    int i;
//    printf("\n\n");
//    for(i=0;result_rid[i]!=NULL;i++){
//        printf("record[%d]: %s\n",i,result_rid[i]);
//    }
//    for(i=0;result_rid[i]!=NULL;i++){
//        free(result_rid[i]);
//    }
//    free(result_rid);
//}
//
//void testqueryCategoryByID(char* myID){
//    Category** mycat;
//    mycat = queryCategoryByID(myID);
//    printf("\n[test]\n");
//    int i;
//    for(i=0;mycat[i]!=NULL;i++){
//        printf("catID: %s\n",mycat[i]->categoryID);
//        printf("catName: %s\n",mycat[i]->categoryName);
//        free((char*)mycat[i]->categoryID);
//        free(mycat[i]);
//    }
//    free(mycat);
//    
//}
//
//void testqueryUserByID(char* myID){
//    User** myusr;
//    myusr = queryUserByID(myID);
//    printf("\n[test]\n");
//    int i;
//    for(i=0;myusr[i]!=NULL;i++){
//        printf("userID: %s\n",myusr[i]->userID);
//        printf("userName: %s\n",myusr[i]->userName);
//        free((char*)myusr[i]->userID);
//        free(myusr[i]);
//    }
//    free(myusr);
//}

void testDraft2(){
//    const char* uuid_org;
//    const char* uuid_usr;
//    const char* uuid_cat;
//    
//    uuid_org = createOrg("Throughwave");
//    uuid_usr = createUser("Pimpat");
//    uuid_cat = createCategory("Documents");
//    
//    printf("\n\nuuid_org: %s\n",uuid_org);
//    printf("uuid_usr: %s\n",uuid_usr);
//    printf("uuid_cat: %s\n",uuid_cat);
    
    char content7[MAX_DIFF_SIZE] = "<book_id>bk107</book_id><author>Knorr, Stefan</author><title>Creepy Crawlies</title><genre>Horror</genre><price>4.95</price><publish_date>2000-12-06</publish_date><description>An anthology of horror stories about roaches, centipedes, scorpions and other insects.</description>";
    
    ObjectBinary* myobj = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    myobj->data = strdup(content7);
    myobj->byteCount = strlen(myobj->data);
    Text result;
    result = getTagContent(myobj,"description");
    printf("output1: %s\n\n",result);
    
    setTagContent(myobj,"1234567890","book_id");
    printf("output2: %s\n", myobj->data);
    
    free(result);
    free(myobj->data);
    free(myobj);
    
//    free((char*)uuid_org);
//    free((char*)uuid_usr);
//    free((char*)uuid_cat);
}

