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
#define MAX_SCHEMA (1000)
#define MAX_SQL_SIZE (2000)
#define MAX_DIFF_SIZE (1000)
#define MAX_NODE_REF_SIZE (1000)
#define MAX_CONTENT_SIZE (1000)
#define MAX_NEW_TAG (20)
#define FULL_CONTENT (4)
#define VER_KEEPED (3)

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
    DataContent* lastestCommon;
    DataContent* head;
    int versionKeeped;
    int versionCount;
    int index_DevRef;
    DataContent* current;
    DeviceRef* userDevicePtr[10];
} DataHolder;

typedef struct Data{
    const char* dataID;
    char dataName[30];
    DataHolder* content;
    const char* chatRoom;
} Data;

extern Data* mydata;
Data* mydata = NULL;

enum TAGNAME {
    book_id,
    author,
    title,
    genre,
    price,
    publish_date,
    description
};

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

ObjectBinary* getDataContentCommonVersion(Data* data);
ObjectBinary* getDataContent(Data* data);
ObjectBinary* getDataContentCommonVersionByID(char* dataID);
ObjectBinary* getDataContentByID(char* dataID);

Text getDiffDataAtlastestCommon(Data* data);
Text getDiffDataAthead(Data* data);
Text getDiffDataAtlastestCommonByID(char* dataID);
Text getDiffDataAtheadByID(char* dataID);

ObjectBinary* getContentNextVer(Data* data);
ObjectBinary* getContentPreVer(Data* data);
ObjectBinary* getContentNextVerByID(char* dataID);
ObjectBinary* getContentPreVerByID(char* dataID);

Text getDiffDataNextVer(Data* data);
Text getDiffDataPreVer(Data* data);
Text getDiffDataNextVerByID(char* dataID);
Text getDiffDataPreVerByID(char* dataID);

int getCurLevelFromCommon(Data* data);
int getCurLevelFromHead(Data* data);

Text getDataContentWithTag(Data* data, char* tagName);
Text getDataContentWithTagByID(char* dataID, char* tagName);

int setNewDataContent(Data* data, char* diffContent);
int setNewDataDiffWithTag(Data* data, char* tagName, char* diff);

Text getTagContent(ObjectBinary* fullContent, char* tagName);
Text getTagContent(ObjectBinary* fullContent, int tagNameEnum);

int setTagContent(ObjectBinary* fullContent, Text newTagContent, char* tagName);

const char* createOrg(Text orgName);
const char* createUser(Text userName);
const char* createCategory(Text categoryName);
const char* createData(Text dataName);

Text getDataContent(char* schema, char* dataID, char* keyName);

int addData2Category(char* categoryID, Data* data);
int addData2CategoryTest(char* categoryID, Data* data);
int addCategory2User(char* userID, char* categoryID);
int addUser2Org(char* orgID, char* userID);

Data* queryDataByID(char* dataID);
Category** queryCategoryByID(char* dataID);
User** queryUserByID(char* dataID);
DeviceRef** getDeviceRefList(Data* data);
DeviceRef** getDeviceRefListByID(char* dataID);

string getDiff(char* old_str, char* new_str);           // pim
string getPatch(char* str, char* str_patch);            // pim
char* getDataContentFirst(Data* data);                  // pim
char* getDataContentLast(Data* data);                   // pim
char* getDataContentVer(Data* data,int ver);            // pim

/* Test Pim */
//int testUUID(uuid_t* uuid);
//void testDMP();
//void testUserCategory(int x);
void testTag(Data* data);
void testsetNewDataContent(char* categoryID);
//void testCountByte();
//void testArrayRid();
void testqueryCategoryByID(char* myID);
void testqueryUserByID(char* myID);
void testDraft2();
void testDraft2_ByID();
void testqueryDataByID();

int main() {
    int ret;
    int size;

    prepareDB();  // include connectServer()
    
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
    //getRid("select in from (select expand(out_toDataContent) from #19:43) where name='lastestCommon')");
    //testDraft2();
    //getContent("SELECT xmlSchema,data,byteCount from #22:414");

//  Test Draft II --------------------------------------------------------------
    testqueryCategoryByID("A19E592D09344701B6B4504CB1D55DCB");
//    testqueryUserByID("A19E592D09344701B6B4504CB1D55DCB");
//    testqueryDataByID();
//    testDraft2_ByID();
//------------------------------------------------------------------------------
    
//  Test Draft I ---------------------------------------------------------------
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
//------------------------------------------------------------------------------
    
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
    ret = sendCommand("CREATE PROPERTY Data.dataName STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Data.dataName STRING: OK!\n");
    ret = sendCommand("CREATE PROPERTY Data.chatRoom STRING");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY Data.chatRoom STRING: OK!\n");
    
    /* Edge toDataHolder */
    ret = createClass("toDataHolder","E");
    if (ret!=0) return 1;
    
    /* DataHolder */
    ret = createClass("DataHolder","V");
    if (ret!=0) return 1;
    ret = sendCommand("CREATE PROPERTY DataHolder.versionCount INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataHolder.versionCount INTEGER: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataHolder.versionKeeped INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataHolder.versionKeeped INTEGER: OK!\n");
    ret = sendCommand("CREATE PROPERTY DataHolder.index_DevRef INTEGER");
    if (ret!=0) return 1;
        //printf("CREATE PROPERTY DataHolder.index_DevRef INTEGER: OK!\n");
    
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
    
    if(total==0){
        printf("FAILED >> data not found\n");
        return NULL;
    }
    
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
    dh->versionCount = 0;
    dh->versionKeeped = VER_KEEPED;
    dh->index_DevRef = 0;
    return dh;
}

ObjectBinary* getDataContentCommonVersion(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    obj_ct->xmlSchema = strdup(data->content->head->objContent->xmlSchema);
    obj_ct->data = strdup(data->content->lastestCommon->objContent->data);
    obj_ct->byteCount = strlen(obj_ct->data);
    return obj_ct;
}

ObjectBinary* getDataContent(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    DataContent* mydc;
    int i, j;
    int count = (data->content->versionCount-1)/FULL_CONTENT;
    int offset = (data->content->versionCount-1)%FULL_CONTENT;
    
    mydc=data->content->lastestCommon;
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
    obj_ct->xmlSchema = strdup(data->content->head->objContent->xmlSchema);
    obj_ct->data = init_str;
    obj_ct->byteCount = strlen(init_str);
    
    return obj_ct;
}

ObjectBinary* getDataContentCommonVersionByID(char* dataID){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_dc;
    char *result1, *result2;
    char* token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid lastestCommon]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'lastestCommon')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_dc = getRid(sql);

    printf("--------------------------------------------------[get objContent]\n");
    sprintf(sql,"SELECT byteCount,data from #%s",rid_dc);
    printf("SQL: %s\n",sql);
    result1 = getContent(sql);
    
    
    token = strtok(result1,":");
    token = strtok(NULL,",");
    //printf("byteCount: %s\n", token);
    obj_ct->byteCount = atoi(token);
    printf("\n\nbyteCount: %d\n", obj_ct->byteCount);
    
    token = strtok(NULL,"\"");
    token = strtok(NULL,"\"");
    //printf("\ndata: %s\n",token);
    char* result_data = strdup(token);
    //printf("len: %d\n",strlen(result_data));
    result_data[strlen(result_data)-1] = '\0';
    //printf("\ndata: %s\n", result_data);
    //printf("len: %d\n",strlen(result_data));
    obj_ct->data = result_data;
    printf("data: %s\n", obj_ct->data);

    printf("--------------------------------------------------[get objContent_Schema]\n");
    sprintf(sql,"SELECT xmlSchema from #%s",rid_dc);
    printf("SQL: %s\n",sql);
    result2 = getContent(sql);
    printf("result2: %s\n",result2);
    char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
    memcpy(temp,result2+11,strlen(result2)-11);
    temp[strlen(temp)-1] = '\0';
    printf("\ntemp: %s\n",temp);
    obj_ct->xmlSchema = temp;
    //printf("xmlSchema: %s\n", obj_ct->xmlSchema);

    free(result1);
    free(result2);
    free(rid_dc);
    free(rid_dh);
    return obj_ct;
}

ObjectBinary* getDataContentByID(char* dataID){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    char sql[MAX_SQL_SIZE];
    char *rid_dh;
    char* token;
    int i=0;
    int count=0;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    /*
    printf("--------------------------------------------------[get versionCount]\n");
    sprintf(sql,"SELECT versionCount from #%s",rid_dh);
    printf("SQL: %s\n",sql);
    result1 = getContent(sql);
    
    token = strtok(result1,":");
    token = strtok(NULL,"\n");
    count = atoi(token);
    printf("versionCount: %d\n", count);
    */
    
    char* rid_dc[FULL_CONTENT];
    char* result_dc[FULL_CONTENT];
    int full_con=0;
    char* result_data;
    char* result_schema;
    
    printf("--------------------------------------------------[get @rid head]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'head'",rid_dh);
    printf("SQL: %s\n",sql);
    rid_dc[0] = getRid(sql);
    printf("rid_head: %s\n",rid_dc[0]);
    
    for(i=0;i<FULL_CONTENT;i++){
        printf("--------------------------------------------------[get data]\n");
        sprintf(sql,"SELECT isDiff,data from #%s",rid_dc[i]);
        result_data = getContent(sql);
        
        token = strtok(result_data, ":");
        token = strtok(NULL, ",");
        if(strcmp(token,"false")==0){
            full_con=1;
        }
        token = strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        
        // หั่น data รอไว้
        result_dc[i] = strdup(token);
        printf("result_dc[%d]: %s\n",i,result_dc[i]);

        // เป็น full ตั้งแต่ head
        if(full_con==1 && i==0){
            // retturn เลย
            printf("--------------------------------------------------[get objContent_Schema]\n");
            sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
            printf("SQL: %s\n",sql);
            result_schema = getContent(sql);
            printf("result_schema: %s\n",result_schema);
            char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
            memcpy(temp,result_schema+11,strlen(result_schema)-11);
            temp[strlen(temp)-1] = '\0';
            printf("\ntemp: %s\n",temp);
            obj_ct->xmlSchema = temp;
            obj_ct->byteCount = strlen(obj_ct->xmlSchema);
            obj_ct->data = result_dc[i];
            
            free(result_data);
            free(result_schema);
            free(rid_dc[i]);
            //free(result_dc[i]);
            free(rid_dh);
            return obj_ct;
        }
        // เจอ full ที่ตน.อื่น
        else if(full_con==1 && i!=0){
            printf("--------------------------------------------------[get objContent_Schema]\n");
            sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
            printf("SQL: %s\n",sql);
            result_schema = getContent(sql);
            printf("result_schema: %s\n",result_schema);
            char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
            memcpy(temp,result_schema+11,strlen(result_schema)-11);
            temp[strlen(temp)-1] = '\0';
            printf("\ntemp: %s\n",temp);
            obj_ct->xmlSchema = temp;
            // ออกจากลูป แล้วเริ่ม patch ที่ตน. i->0 (byteCount/data)
            free(result_data);
            free(result_schema);
            break;
        }
        else{
            sprintf(sql,"select in from (select expand(out_toDiffContent) from #%s) where status='pre'",rid_dc[i]);
            printf("SQL: %s\n",sql);
            rid_dc[i+1] = getRid(sql);
            printf("rid[%d]: %s\n",i+1,rid_dc[i+1]);
            free(result_data);
        }
    }
    printf("\ni: %d\n\n",i);
    
    //Patch
    string s;
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    sprintf(init_str,"%s",result_dc[i]);
    int j;
    for(j=i;j>0;j--){
        s = getPatch(init_str, result_dc[j-1]);
        sprintf(init_str,"%s",s.c_str());
    }
    //printf("\n\n\nMY DATA: %s\n",init_str);
    obj_ct->data = strdup(init_str);
    obj_ct->byteCount = strlen(obj_ct->data);
    free(init_str);
    
    for(j=i;j>=0;j--){
        free(rid_dc[j]);
        free(result_dc[j]);
    }
    free(rid_dh);
    return obj_ct;
}

Text getDiffDataAtlastestCommon(Data* data){
    return data->content->lastestCommon->objContent->data;
}

Text getDiffDataAthead(Data* data){
    return data->content->head->objContent->data;
}

Text getDiffDataAtlastestCommonByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_dc;
    char *result, *str_data;
    char* token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid lastestCommon]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'lastestCommon')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_dc = getRid(sql);
    
    printf("--------------------------------------------------[get objContent]\n");
    sprintf(sql,"SELECT data from #%s",rid_dc);
    printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    token = strtok(result,"\"");
    token = strtok(NULL,"\"");
    printf("data: %s\n", token);
    str_data = strdup(token);
    
    free(rid_dh);
    free(rid_dc);
    free(result);
    return str_data;
}

Text getDiffDataAtheadByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_dc;
    char *result, *str_data;
    char* token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid head]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'head')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_dc = getRid(sql);
    
    printf("--------------------------------------------------[get objContent]\n");
    sprintf(sql,"SELECT data from #%s",rid_dc);
    printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    token = strtok(result,"\"");
    token = strtok(NULL,"\"");
    printf("data: %s\n", token);
    str_data = strdup(token);
    
    free(rid_dh);
    free(rid_dc);
    free(result);
    return str_data;
}

ObjectBinary* getContentNextVer(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    if(data->content->current->nextVersion==NULL){
        free(obj_ct);
        printf("\n*** Now current is pointing to head\n");
        return NULL;
    }
    else if(data->content->current->nextVersion->isDiff==0){
        obj_ct->xmlSchema = strdup(data->content->current->nextVersion->objContent->xmlSchema);
        obj_ct->data = strdup(data->content->current->nextVersion->objContent->data);
        obj_ct->byteCount = data->content->current->nextVersion->objContent->byteCount;
        
        DataContent* tmp;
        tmp = data->content->current->nextVersion;
        data->content->current = tmp;
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
        //printf("count: %d\n",count);
        string s;

        for(i=0;i<count+1;i++){
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            sprintf(init_str,"%s",s.c_str());
        }
        obj_ct->xmlSchema = strdup(mydc->objContent->xmlSchema);
        obj_ct->data = init_str;
        obj_ct->byteCount = strlen(obj_ct->data);
        
        DataContent* tmp;
        tmp = data->content->current->nextVersion;
        data->content->current = tmp;
        return obj_ct;
    }
}

ObjectBinary* getContentPreVer(Data* data){
    ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    if(data->content->current->preVersion==NULL){
        free(obj_ct);
        printf("\n*** Now current is pointing to lastestCommon\n");
        return NULL;
    }
    else if(data->content->current->preVersion->isDiff==0){
        obj_ct->xmlSchema = strdup(data->content->current->preVersion->objContent->xmlSchema);
        obj_ct->data = strdup(data->content->current->preVersion->objContent->data);
        obj_ct->byteCount = data->content->current->preVersion->objContent->byteCount;
        
        DataContent* tmp;
        tmp = data->content->current->preVersion;
        data->content->current = tmp;
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
        //printf("count: %d\n",count);
        string s;
        
        for(i=0;i<count-1;i++){
            mydc=mydc->nextVersion;
            s = getPatch(init_str, mydc->objContent->data);
            sprintf(init_str,"%s",s.c_str());
        }
        obj_ct->xmlSchema = strdup(mydc->objContent->xmlSchema);
        obj_ct->data = init_str;
        obj_ct->byteCount = strlen(obj_ct->data);
        
        DataContent* tmp;
        tmp = data->content->current->preVersion;
        data->content->current = tmp;
        return obj_ct;
    }
}

ObjectBinary* getContentNextVerByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_cur;
    char* token;
    int i;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'current')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_cur = getRid(sql);
    
    char* rid_dc[FULL_CONTENT];
    char* result_dc[FULL_CONTENT];
    int full_con=0;
    char* result_data;
    char* result_schema;

    printf("--------------------------------------------------[get @rid next_current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status = 'next')",rid_cur);
    printf("SQL: %s\n",sql);
    rid_dc[0] = getRid(sql);
    
    if(rid_dc[0]==NULL){
        printf("\n*** Now current is pointing to head\n");
        free(rid_dh);
        free(rid_cur);
        return NULL;
    }
    else{
        printf("rid_nextCurrrent: %s\n",rid_dc[0]);
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        
        for(i=0;i<FULL_CONTENT;i++){
            printf("--------------------------------------------------[get data]\n");
            sprintf(sql,"SELECT isDiff,data from #%s",rid_dc[i]);
            printf("SQL: %s\n",sql);
            result_data = getContent(sql);
            
            token = strtok(result_data, ":");
            token = strtok(NULL, ",");
            if(strcmp(token,"false")==0){
                full_con=1;
            }
            token = strtok(NULL,"\"");
            token = strtok(NULL,"\"");
            
            // หั่น data รอไว้
            result_dc[i] = strdup(token);
            printf("result_dc[%d]: %s\n",i,result_dc[i]);
            
            // เป็น full ตั้งแต่ head
            if(full_con==1 && i==0){
                // retturn เลย
                printf("--------------------------------------------------[get objContent_Schema]\n");
                sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
                printf("SQL: %s\n",sql);
                result_schema = getContent(sql);
                //printf("result_schema: %s\n",result_schema);
                char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
                memcpy(temp,result_schema+11,strlen(result_schema)-11);
                temp[strlen(temp)-1] = '\0';
                printf("\ntemp: %s\n",temp);
                obj_ct->xmlSchema = temp;
                obj_ct->byteCount = strlen(obj_ct->xmlSchema);
                obj_ct->data = result_dc[i];
                
                free(result_data);
                free(result_schema);
                free(rid_dc[i]);
                //free(result_dc[i]);
                free(rid_dh);
                return obj_ct;
            }
            // เจอ full ที่ตน.อื่น
            else if(full_con==1 && i!=0){
                printf("--------------------------------------------------[get objContent_Schema]\n");
                sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
                printf("SQL: %s\n",sql);
                result_schema = getContent(sql);
                printf("result_schema: %s\n",result_schema);
                char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
                memcpy(temp,result_schema+11,strlen(result_schema)-11);
                temp[strlen(temp)-1] = '\0';
                printf("\ntemp: %s\n",temp);
                obj_ct->xmlSchema = temp;
                // ออกจากลูป แล้วเริ่ม patch ที่ตน. i->0 (byteCount/data)
                free(result_data);
                free(result_schema);
                break;
            }
            else{
                sprintf(sql,"select in from (select expand(out_toDiffContent) from #%s) where status='pre'",rid_dc[i]);
                printf("SQL: %s\n",sql);
                rid_dc[i+1] = getRid(sql);
                printf("rid[%d]: %s\n",i+1,rid_dc[i+1]);
                free(result_data);
            }
        }
        
        //Patch
        string s;
        char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
        sprintf(init_str,"%s",result_dc[i]);
        int j;
        for(j=i;j>0;j--){
            s = getPatch(init_str, result_dc[j-1]);
            sprintf(init_str,"%s",s.c_str());
        }
        //printf("\n\n\nMY DATA: %s\n",init_str);
        obj_ct->data = strdup(init_str);
        obj_ct->byteCount = strlen(obj_ct->data);
        free(init_str);
        
        for(j=i;j>=0;j--){
            free(rid_dc[j]);
            free(result_dc[j]);
        }
        free(rid_dh);
        free(rid_cur);
        return obj_ct;
    }
}

ObjectBinary* getContentPreVerByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_cur;
    char* token;
    int i;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'current')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_cur = getRid(sql);
    
    char* rid_dc[FULL_CONTENT];
    char* result_dc[FULL_CONTENT];
    int full_con=0;
    char* result_data;
    char* result_schema;
    
    printf("--------------------------------------------------[get @rid pre_current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status = 'pre')",rid_cur);
    printf("SQL: %s\n",sql);
    rid_dc[0] = getRid(sql);
    
    if(rid_dc[0]==NULL){
        printf("\n*** Now current is pointing to lastestCommon\n");
        free(rid_dh);
        free(rid_cur);
        return NULL;
    }
    else{
        printf("rid_nextCurrrent: %s\n",rid_dc[0]);
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        
        for(i=0;i<FULL_CONTENT;i++){
            printf("--------------------------------------------------[get data]\n");
            sprintf(sql,"SELECT isDiff,data from #%s",rid_dc[i]);
            printf("SQL: %s\n",sql);
            result_data = getContent(sql);
            
            token = strtok(result_data, ":");
            token = strtok(NULL, ",");
            if(strcmp(token,"false")==0){
                full_con=1;
            }
            token = strtok(NULL,"\"");
            token = strtok(NULL,"\"");
            
            // หั่น data รอไว้
            result_dc[i] = strdup(token);
            printf("result_dc[%d]: %s\n",i,result_dc[i]);
            
            // เป็น full ตั้งแต่ head
            if(full_con==1 && i==0){
                // retturn เลย
                printf("--------------------------------------------------[get objContent_Schema]\n");
                sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
                printf("SQL: %s\n",sql);
                result_schema = getContent(sql);
                //printf("result_schema: %s\n",result_schema);
                char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
                memcpy(temp,result_schema+11,strlen(result_schema)-11);
                temp[strlen(temp)-1] = '\0';
                printf("\ntemp: %s\n",temp);
                obj_ct->xmlSchema = temp;
                obj_ct->byteCount = strlen(obj_ct->xmlSchema);
                obj_ct->data = result_dc[i];
                
                free(result_data);
                free(result_schema);
                free(rid_dc[i]);
                //free(result_dc[i]);
                free(rid_dh);
                return obj_ct;
            }
            // เจอ full ที่ตน.อื่น
            else if(full_con==1 && i!=0){
                printf("--------------------------------------------------[get objContent_Schema]\n");
                sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
                printf("SQL: %s\n",sql);
                result_schema = getContent(sql);
                printf("result_schema: %s\n",result_schema);
                char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
                memcpy(temp,result_schema+11,strlen(result_schema)-11);
                temp[strlen(temp)-1] = '\0';
                printf("\ntemp: %s\n",temp);
                obj_ct->xmlSchema = temp;
                // ออกจากลูป แล้วเริ่ม patch ที่ตน. i->0 (byteCount/data)
                free(result_data);
                free(result_schema);
                break;
            }
            else{
                sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status='pre'",rid_dc[i]);
                printf("SQL: %s\n",sql);
                rid_dc[i+1] = getRid(sql);
                printf("rid[%d]: %s\n",i+1,rid_dc[i+1]);
                free(result_data);
            }
        }
        
        //Patch
        string s;
        char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
        sprintf(init_str,"%s",result_dc[i]);
        int j;
        for(j=i;j>0;j--){
            s = getPatch(init_str, result_dc[j-1]);
            sprintf(init_str,"%s",s.c_str());
        }
        //printf("\n\n\nMY DATA: %s\n",init_str);
        obj_ct->data = strdup(init_str);
        obj_ct->byteCount = strlen(obj_ct->data);
        free(init_str);
        
        for(j=i;j>=0;j--){
            free(rid_dc[j]);
            free(result_dc[j]);
        }
        free(rid_dh);
        free(rid_cur);
        return obj_ct;
    }
}

Text getDiffDataNextVer(Data* data){
    if(data->content->current->nextVersion==NULL){
        printf("\n*** Now current is pointing to head\n");
        return NULL;
    }
    else{
        DataContent* tmp;
        tmp = data->content->current->nextVersion;
        data->content->current = tmp;
        return data->content->current->objContent->data;
    }
}

Text getDiffDataPreVer(Data* data){
    if(data->content->current->preVersion==NULL){
        printf("\n*** Now current is pointing to lastestCommon\n");
        return NULL;
    }
    else{
        DataContent* tmp;
        tmp = data->content->current->preVersion;
        data->content->current = tmp;
        return data->content->current->objContent->data;
    }
}

Text getDiffDataNextVerByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_cur, *rid_next_cur;
    char *result, *str_data;
    char* token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'current')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_cur = getRid(sql);
    
    printf("--------------------------------------------------[get @rid next_current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status = 'next')",rid_cur);
    printf("SQL: %s\n",sql);
    rid_next_cur = getRid(sql);
    
    if(rid_next_cur==NULL){
        printf("\n*** Now current is pointing to head\n");
        free(rid_dh);
        free(rid_cur);
        return NULL;
    }
    else{
        printf("--------------------------------------------------[get objContent]\n");
        sprintf(sql,"SELECT data from #%s",rid_next_cur);
        printf("SQL: %s\n",sql);
        result = getContent(sql);
    
        token = strtok(result,"\"");
        token = strtok(NULL,"\"");
        printf("data: %s\n", token);
        str_data = strdup(token);
    
        free(rid_dh);
        free(rid_cur);
        free(rid_next_cur);
        free(result);
        return str_data;
    }
}

Text getDiffDataPreVerByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *rid_cur, *rid_pre_cur;
    char *result, *str_data;
    char* token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'current')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_cur = getRid(sql);
    
    printf("--------------------------------------------------[get @rid pre_current]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status = 'pre')",rid_cur);
    printf("SQL: %s\n",sql);
    rid_pre_cur = getRid(sql);
    
    if(rid_pre_cur==NULL){
        printf("\n*** Now current is pointing to lastestCommon\n");
        free(rid_dh);
        free(rid_cur);
        return NULL;
    }
    else{
        printf("--------------------------------------------------[get objContent]\n");
        sprintf(sql,"SELECT data from #%s",rid_pre_cur);
        printf("SQL: %s\n",sql);
        result = getContent(sql);
        
        token = strtok(result,"\"");
        token = strtok(NULL,"\"");
        printf("data: %s\n", token);
        str_data = strdup(token);
        
        free(rid_dh);
        free(rid_cur);
        free(rid_pre_cur);
        free(result);
        return str_data;
    }
}

int getCurLevelFromCommon(Data* data){
//    data->content->current = data->content->lastestCommon;
//    data->content->current = NULL;
    char addr_curr[20],addr_point[20];
    printf("@current: %p\n",data->content->current);
    sprintf(addr_curr,"%p",data->content->current);
    
    DataContent *mydc, *next_mydc;
    int i=0;
    for(mydc=data->content->lastestCommon;mydc!=NULL;mydc=next_mydc){
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

int getCurLevelFromHead(Data* data){
//    data->content->current = data->content->lastestCommon;
    char addr_curr[20],addr_point[20];
    printf("@current: %p\n",data->content->current);
    sprintf(addr_curr,"%p",data->content->current);
    
    DataContent *mydc, *pre_mydc;
    int i=0;
    for(mydc=data->content->head;mydc!=NULL;mydc=pre_mydc){
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

Text getDataContentWithTag(Data* data, char* tagName){
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
        free(partial_result);
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

Text getDataContentWithTagByID(char* dataID, char* tagName){
    ObjectBinary* myobj;
    myobj = getDataContentByID(dataID);
    
    if(myobj==NULL)
        return NULL;
    
    char* result;
    char* partial_result = (char*)malloc(sizeof(char)*(MAX_DIFF_SIZE/2));
    char* mystr = (char*)malloc(sizeof(char)*(MAX_DIFF_SIZE+20));
    result = myobj->data;
    sprintf(mystr,"<root>%s</root>", result);
    //free(result);
    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    partial_data = ezxml_get(root,tagName,-1);
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        free(myobj->data);
        free(myobj->xmlSchema);
        free(myobj);
        free(mystr);
        free(partial_result);
        ezxml_free(root);
        return NULL;
    }
    //printf("partial_data: %s\n",partial_data->txt);
    sprintf(partial_result,"%s",partial_data->txt);
    //printf("partial_result: %s\n\n",partial_result);
    free(mystr);
    ezxml_free(root);
    free(myobj->data);
    free(myobj->xmlSchema);
    free(myobj);
    return partial_result;
}

int setNewDataContent(Data* data, char* diffContent){
    
    if(data->content->versionCount==0){
        printf("\n\n[ Data #0 ]\n");
        data->content->versionCount=1;
        DataContent *dc = (DataContent*)malloc(sizeof(DataContent));
        data->content->lastestCommon = dc;
        data->content->head = dc;
        data->content->current = dc;    // current cursor
        
        dc->preVersion = NULL;
        dc->nextVersion = NULL;
        dc->isDiff = 0;
        
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc->objContent = obj_ct;
        //char tmp[] = "<book_id><author><title><genre><price><publish_date><description>";
        //obj_ct->xmlSchema = strdup(tmp);
        //char schema[10000];
        ezxml_t tmp = ezxml_parse_file("/Users/pimpat/Documents/schema.xml");
        obj_ct->xmlSchema = ezxml_toxml(tmp);
        ezxml_free(tmp);
        
        obj_ct->data = strdup(diffContent);
        obj_ct->byteCount = strlen(obj_ct->data);
        
        return obj_ct->byteCount;
    }
    
    else {
        string s;
        if((data->content->versionCount)%FULL_CONTENT!=0){
            char* result;
            result = getDataContentLast(data);
            printf("\nresult_before_diff: %s\n",result);
            s = getDiff(result,diffContent);
            free(result);
        }
        
        printf("\n\n[ Data #%d ]\n",data->content->versionCount);
        data->content->versionCount++;
        DataContent *dc = (DataContent*)malloc(sizeof(DataContent));
        
        // change ptr new dc preVersion point to old dc
        dc->preVersion = data->content->head;
        
        // change ptr old dc nextVersion point to new dc
        data->content->head->nextVersion = dc;
        
        // change ptr head point to new dc
        data->content->head = dc;
        
        // ptr new dc nextVersion is NULL
        dc->nextVersion = NULL;
        
        data->content->current = dc;    // current cursor
        
        if((data->content->versionCount-1)%FULL_CONTENT!=0){
            dc->isDiff = 1;
        }
        else {
            dc->isDiff = 0;
        }
        
        ObjectBinary* obj_ct = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc->objContent = obj_ct;
        //char tmp[] = "<book_id><author><title><genre><price><publish_date><description>";
        //obj_ct->xmlSchema = strdup(tmp);
        ezxml_t tmp = ezxml_parse_file("/Users/pimpat/Documents/schema.xml");
        obj_ct->xmlSchema = ezxml_toxml(tmp);
        ezxml_free(tmp);
        
        if((data->content->versionCount-1)%FULL_CONTENT!=0){
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

Text getTagContent(ObjectBinary* fullContent, int tagNameEnum){
    char* mystr = (char*)malloc(sizeof(char)*fullContent->byteCount+20);
    char* partial_result;
    sprintf(mystr,"<root>%s</root>", fullContent->data);
    
    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    switch(tagNameEnum){
        case book_id:
            partial_data = ezxml_get(root,"book_id",-1);
            break;
        case author:
            partial_data = ezxml_get(root,"author",-1);
            break;
        case title:
            partial_data = ezxml_get(root,"title",-1);
            break;
        case genre:
            partial_data = ezxml_get(root,"genre",-1);
            break;
        case price:
            partial_data = ezxml_get(root,"price",-1);
            break;
        case publish_date:
            partial_data = ezxml_get(root,"publish_date",-1);
            break;
        case description:
            partial_data = ezxml_get(root,"description",-1);
            break;
        default:
            printf("FAILED >> incorrect tag\n");
            free(mystr);
            ezxml_free(root);
            return NULL;
            break;
    }
    
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

const char* createData(Text dataName){
    int ret;
    const char* uuidstr;
    char sql[MAX_SQL_SIZE];
    
    uuidstr = createNewData();
    printf("uuid: %s\n",uuidstr);
    sprintf(sql,"CREATE VERTEX Data set dataID='%s', dataName='%s', chatRoom='%s'",uuidstr,dataName,mydata->chatRoom);
    printf("sql: %s\n",sql);
    ret = sendCommand(sql);
    
    if (ret!=0)
        printf("createData..FAILED(vertex)\n");
    else
        printf("createData..PASS(vertex)\n");
    
    return uuidstr;
    
}

Text getDataContent(char* schema, char* dataID, char* keyName){
    ezxml_t root = ezxml_parse_file("/Users/pimpat/Documents/schema.xml");
    ezxml_t element, complex, sequence, tagName;
    char* result;
    
    element = ezxml_child(root,"xs:element");
    //result = ezxml_attr(element,"name");
    //printf("element: %s\n",result);
    complex = ezxml_child(element,"xs:complexType");
    sequence = ezxml_child(complex,"xs:sequence");
    
    int check=0;  //  1 = found
    int i=0;
    for(tagName = ezxml_child(sequence,"xsd:element");tagName;tagName=tagName->next){
        result = (char*)ezxml_attr(tagName,"name");
        printf("tagName[%d]: %s\n",i,result);
        if(strcmp(result,keyName)==0)
            check=1;
        i++;
    }
    ezxml_free(root);
    if(check==1){
        //  query
        char* partial_result;
        partial_result = getDataContentWithTagByID(dataID,keyName);
        return partial_result;
    }
    else{
        printf("not found keyName in xmlSchema\n");
        return NULL;
    }
}

int addData2Category(char* categoryID, Data* data){
    char* rid_cat;
    char sql[MAX_SQL_SIZE];
    int ret;
    
    printf("--------------------------------------------------[get @rid Category]\n");
    sprintf(sql,"SELECT @rid from Category where categoryID ='%s'",categoryID);
    rid_cat = getRid(sql);
    
    printf("--------------------------------------------------[create_vertex_Data]\n");
    sprintf(sql,"CREATE VERTEX Data set dataID='%s', dataName='mydata', chatRoom='%s'",data->dataID,data->chatRoom);
    ret = createVertex(sql);
    if (ret!=0)
        printf("addData2Category..FAILED(vertex_Data)\n");
    
    short cltid_d, cltid_dh;
    long rid_d, rid_dh;
    cltid_d = addr.cltid;
    rid_d = addr.rid;
    
    printf("--------------------------------------------------[create_edge_toData]\n");
    sprintf(sql,"CREATE EDGE toData from #%s to #%d:%lu",rid_cat,cltid_d,rid_d);
    ret = sendCommand(sql);
    if (ret!=0)
        printf("addData2Category..FAILED(edge_toData)\n");
    
    printf("--------------------------------------------------[create_vertex_DataHolder]\n");
    sprintf(sql,"CREATE VERTEX DataHolder set versionKeeped=%d, versionCount=%d, index_DevRef=%d",data->content->versionKeeped,data->content->versionCount,data->content->index_DevRef);
    ret = createVertex(sql);
    if (ret!=0)
        printf("addData2Category..FAILED(vertex_DataHolder)\n");
    
    cltid_dh = addr.cltid;
    rid_dh = addr.rid;
    
    printf("--------------------------------------------------[create_edge_toDataHolder]\n");
    sprintf(sql,"CREATE EDGE toDataHolder from #%d:%lu to #%d:%lu",cltid_d,rid_d,cltid_dh,rid_dh);
    ret = sendCommand(sql);
    if (ret!=0)
        printf("addData2Category..FAILED(edge_toDataHolder)\n");
    
    //  DataContent
    DataContent *dc, *next_dc;
    short cltid_lastestCommon, cltid_last, cltid_tmp;
    long rid_lastestCommon, rid_last, rid_tmp;
    int i=0;
    
    for(dc=data->content->lastestCommon;dc!=NULL;dc=next_dc){
        printf("--------------------------------------------------[create_vertex_DataContent]\n");
        sprintf(sql,"CREATE VERTEX Datacontent set isDiff=%d, xmlSchema='%s', byteCount=%d, data=\"%s\"",dc->isDiff,dc->objContent->xmlSchema,dc->objContent->byteCount,dc->objContent->data);
        //printf("SQL: %s\n",sql);
        ret = createVertex(sql);
        if (ret!=0){
            printf("addData2Category..FAILED(vertex_DataContent)\n");
            return -1;
        }
        if(i==0){
            cltid_lastestCommon = addr.cltid;
            rid_lastestCommon = addr.rid;
            cltid_tmp = cltid_lastestCommon;
            rid_tmp = rid_lastestCommon;
            printf("--------------------------------------------------[create_edge_toDataContent_lastestCommon]\n");
            sprintf(sql,"CREATE EDGE toDataContent from #%d:%lu to #%d:%lu set name='lastestCommon'",cltid_dh,rid_dh,cltid_lastestCommon,rid_lastestCommon);
            //printf("SQL: %s\n",sql);
            ret = sendCommand(sql);
            if (ret!=0){
                printf("addData2Category..FAILED(edge_toDataContent_lastestCommon)\n");
                return -1;
            }
        }
        else{
        //else if(i < data->content->versionCount-1){
            
            //  create edge preVersion
            cltid_last = addr.cltid;
            rid_last = addr.rid;
            printf("--------------------------------------------------[create_edge_toDiffContent_Pre]\n");
            sprintf(sql,"CREATE EDGE toDiffContent from #%d:%lu to #%d:%lu set status='pre'",cltid_last,rid_last,cltid_tmp,rid_tmp);
            //printf("SQL: %s\n",sql);
            ret = sendCommand(sql);
            if (ret!=0){
                printf("addData2Category..FAILED(edge_toDiffContent_Pre)\n");
                return -1;
            }
            
            //  create edge nextVersion
            printf("--------------------------------------------------[create_edge_toDiffContent_Next]\n");
            sprintf(sql,"CREATE EDGE toDiffContent from #%d:%lu to #%d:%lu set status='next'",cltid_tmp,rid_tmp,cltid_last,rid_last);
            //printf("SQL: %s\n",sql);
            ret = sendCommand(sql);
            if (ret!=0){
                printf("addData2Category..FAILED(edge_toDiffContent_Next)\n");
                return -1;
            }
            cltid_tmp = cltid_last;
            rid_tmp = rid_last;
        }
        next_dc = dc->nextVersion;
        i++;
    }
    
    //  after for loop: set head
    if(i==1){
        printf("--------------------------------------------------[create_edge_toDataContent_head]\n");
        sprintf(sql,"CREATE EDGE toDataContent from #%d:%lu to #%d:%lu set name='head'",cltid_dh,rid_dh,cltid_lastestCommon,rid_lastestCommon);
        //printf("SQL: %s\n",sql);
        ret = sendCommand(sql);
        if (ret!=0){
            printf("addData2Category..FAILED(edge_toDataContent_head)\n");
            return -1;
        }
    }
    else{
        printf("--------------------------------------------------[create_edge_toDataContent_head]\n");
        sprintf(sql,"CREATE EDGE toDataContent from #%d:%lu to #%d:%lu set name='head'",cltid_dh,rid_dh,cltid_last,rid_last);
        //printf("SQL: %s\n",sql);
        ret = sendCommand(sql);
        if (ret!=0){
            printf("addData2Category..FAILED(edge_toDataContent_head)\n");
            return -1;
        }
    }
    
    /* for test DeviceRef */
    
    // เอา userID
    char* result;
    char* uuidstr = (char*)malloc(sizeof(char)*33);
    printf("--------------------------------------------------[get userID]\n");
    sprintf(sql,"SELECT userID from (SELECT expand(in('toCategory')) from Category where categoryID='%s')",categoryID);
    result = getContent(sql);
    //printf("result: %s\n", result);
    memcpy(uuidstr,result+8,strlen(result)-8-1);
    uuidstr[32] = '\0';
    
    // หา nodeRef
    char* rid_dc;
    printf("--------------------------------------------------[get @rid head]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%d:%lu) where name ='head'",cltid_dh,rid_dh);
    rid_dc = getRid(sql);
    
    // เอาข้อมูลลง struct DeviceRef
    data->content->userDevicePtr[data->content->index_DevRef] = (DeviceRef*)malloc(sizeof(DeviceRef));
    data->content->userDevicePtr[data->content->index_DevRef]->userID = uuidstr;
    data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID = stringUUID();
    sprintf(data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent,"%s",rid_dc);
    printf("userID: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->userID);
    printf("deviceTransportID: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID);
    printf("nodeRef: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent);

    // สร้าง vertex DeviceRef
    printf("--------------------------------------------------[create_vertex_DeviceRef]\n");
    sprintf(sql,"CREATE VERTEX DeviceRef set userID='%s', deviceTransportID='%s', nodeRefToDataContent='#%s'",data->content->userDevicePtr[data->content->index_DevRef]->userID,data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID,data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent);
    //printf("SQL: %s\n",sql);
    ret = createVertex(sql);
    if (ret!=0){
        printf("addData2Category..FAILED(vertex_DeviceRef)\n");
        return 1;
    }
    data->content->index_DevRef++;
    
    printf("--------------------------------------------------[update_index_DevRef]\n");
    sprintf(sql,"UPDATE #%d:%lu set index_DevRef='%d'",cltid_dh,rid_dh,data->content->index_DevRef);
    ret = sendCommand(sql);
    if (ret!=0){
        printf("addData2Category..FAILED(update_index_DevRef)\n");
        return 1;
    }
    
    short cltid_dr;
    long rid_dr;
    cltid_dr = addr.cltid;
    rid_dr = addr.rid;
    
    // สร้าง edge จาก DeviceRef -> DataHolder
    printf("--------------------------------------------------[create_edge_fromDeviceRef]\n");
    sprintf(sql,"CREATE EDGE fromDeviceRef from #%d:%lu to #%d:%lu",cltid_dr,rid_dr,cltid_dh,rid_dh);
    printf("SQL: %s\n",sql);
    ret = sendCommand(sql);
    if (ret!=0){
        printf("addData2Category..FAILED(edge_fromDeviceRef)\n");
        return 1;
    }

    free(rid_cat);
    free(result);
    free(rid_dc);
    return 0;
}

int addData2CategoryTest(char* categoryID, Data* data){
    char* rid_cat;
    char* rid_d;
    char* rid_dh;
    char sql[MAX_SQL_SIZE];
    int ret;
    
    printf("--------------------------------------------------[get @rid Category]\n");
    sprintf(sql,"SELECT @rid from Category where categoryID ='%s'",categoryID);
    rid_cat = getRid(sql);
    
    printf("--------------------------------------------------[get @rid Data]\n");
    sprintf(sql,"SELECT @rid from Data where dataID ='%s'",data->dataID);
    rid_d = getRid(sql);
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from #%s)",rid_d);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[create_edge_toData]\n");
    sprintf(sql,"CREATE EDGE toData from #%s to #%s",rid_cat,rid_d);
    printf("SQL: %s\n",sql);
    ret = sendCommand(sql);
    if (ret!=0)
        printf("addData2CategoryTest..FAILED(edge_toData)\n");
    
    /* for test DeviceRef */
    
    // เอา userID
    char* result;
    char* uuidstr = (char*)malloc(sizeof(char)*33);
    printf("--------------------------------------------------[get userID]\n");
    sprintf(sql,"SELECT userID from (SELECT expand(in('toCategory')) from Category where categoryID='%s')",categoryID);
    result = getContent(sql);
    //printf("result: %s\n", result);
    memcpy(uuidstr,result+8,strlen(result)-8-1);
    uuidstr[32] = '\0';
    
    // หา nodeRef
    char* rid_dc;
    printf("--------------------------------------------------[get @rid head]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name='head'",rid_dh);
    rid_dc = getRid(sql);
    
    // เอาข้อมูลลง struct DeviceRef
    data->content->userDevicePtr[data->content->index_DevRef] = (DeviceRef*)malloc(sizeof(DeviceRef));
    data->content->userDevicePtr[data->content->index_DevRef]->userID = uuidstr;
    data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID = stringUUID();
    sprintf(data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent,"%s",rid_dc);
    printf("userID: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->userID);
    printf("deviceTransportID: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID);
    printf("nodeRef: %s\n",data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent);
    
    // สร้าง vertex DeviceRef
    printf("--------------------------------------------------[create_vertex_DeviceRef]\n");
    sprintf(sql,"CREATE VERTEX DeviceRef set userID='%s', deviceTransportID='%s', nodeRefToDataContent='#%s'",data->content->userDevicePtr[data->content->index_DevRef]->userID,data->content->userDevicePtr[data->content->index_DevRef]->deviceTransportID,data->content->userDevicePtr[data->content->index_DevRef]->nodeRefToDataContent);
    //printf("SQL: %s\n",sql);
    ret = createVertex(sql);
    if (ret!=0){
        printf("addData2CategoryTest..FAILED(vertex_DeviceRef)\n");
        return 1;
    }
    data->content->index_DevRef++;
    
    printf("--------------------------------------------------[update_index_DevRef]\n");
    sprintf(sql,"UPDATE #%s set index_DevRef='%d'",rid_dh,data->content->index_DevRef);
    ret = sendCommand(sql);
    if (ret!=0){
        printf("addData2CategoryTest..FAILED(update_index_DevRef)\n");
        return 1;
    }
    
    short cltid_dr;
    long rid_dr;
    cltid_dr = addr.cltid;
    rid_dr = addr.rid;
    
    // สร้าง edge จาก DeviceRef -> DataHolder
    printf("--------------------------------------------------[create_edge_fromDeviceRef]\n");
    sprintf(sql,"CREATE EDGE fromDeviceRef from #%d:%lu to #%s",cltid_dr,rid_dr,rid_dh);
    printf("SQL: %s\n",sql);
    ret = sendCommand(sql);
    if (ret!=0){
        printf("addData2CategoryTest..FAILED(edge_fromDeviceRef)\n");
        return 1;
    }

    free(rid_cat);
    free(rid_d);
    free(rid_dh);
    free(rid_dc);
    free(result);
    return 0;

}

int addCategory2User(char* userID, char* categoryID){
    int ret;
    char* rid_user;
    char* rid_cat;
    char sql[MAX_SQL_SIZE];
    
    printf("--------------------------------------------------[get @rid User]\n");
    sprintf(sql,"SELECT @rid from User where userID ='%s'",userID);
    rid_user = getRid(sql);
    
    printf("--------------------------------------------------[get @rid Category]\n");
    sprintf(sql,"SELECT @rid from Category where categoryID ='%s'",categoryID);
    rid_cat = getRid(sql);
    
    printf("--------------------------------------------------[create_edge_toCategory]\n");
    sprintf(sql,"CREATE EDGE toCategory from #%s to #%s",rid_user,rid_cat);
    ret = sendCommand(sql);
    
    free(rid_user);
    free(rid_cat);
    return ret; //  1 = failed, 0 = success
}

int addUser2Org(char* orgID, char* userID){
    int ret;
    char* rid_org;
    char* rid_user;
    char sql[MAX_SQL_SIZE];
    
    printf("--------------------------------------------------[get @rid Org]\n");
    sprintf(sql,"SELECT @rid from Org where orgID ='%s'",orgID);
    rid_org = getRid(sql);
    
    printf("--------------------------------------------------[get @rid User]\n");
    sprintf(sql,"SELECT @rid from User where userID ='%s'",userID);
    rid_user = getRid(sql);
    
    printf("--------------------------------------------------[create_edge_toUser]\n");
    sprintf(sql,"CREATE EDGE toUser from #%s to #%s",rid_org,rid_user);
    ret = sendCommand(sql);
    
    free(rid_org);
    free(rid_user);
    return ret; //  1 = failed, 0 = success
}

Data* queryDataByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh;
    char *result, *result_schema;
    char *token;
    
    printf("--------------------------------------------------[get dataName,chatRoom]\n");
    sprintf(sql,"SELECT dataName,chatRoom from Data where dataID ='%s'",dataID);
    printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    if(result==NULL){
        printf("FAILED >> incorrect dataID\n");
        return NULL;
    }
    
    Data* d = (Data*)malloc(sizeof(Data));
    d->dataID = strdup(dataID);
    
    token = strtok(result,"\"");
    token = strtok(NULL,"\"");
    printf("dataName: %s\n",token);
    memcpy(d->dataName,token,strlen(token));
    
    token = strtok(NULL,"\"");
    token = strtok(NULL,"\"");
    printf("chatRoom: %s\n",token);
    d->chatRoom = strdup(token);
    free(result);
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    if(rid_dh == NULL){
        printf("no DataHolder in this dataID\n");
        free(rid_dh);
        return d;
    }
    
    printf("--------------------------------------------------[get versionKeeped,versionCount,index_DevRef]\n");
    sprintf(sql,"SELECT versionKeeped,versionCount,index_DevRef from #%s",rid_dh);
    result = getContent(sql);
    
    d->content = createNewDataHolder();
    token = strtok(result,":");
    token = strtok(NULL,",");
    printf("ver_keeped: %s\n",token);
    d->content->versionKeeped = atoi(token);
    
    token = strtok(NULL,":");
    token = strtok(NULL,",");
    printf("ver_count: %s\n",token);
    d->content->versionCount = atoi(token);
    
    token = strtok(NULL,":");
    token = strtok(NULL,"\n");
    printf("index: %s\n",token);
    d->content->index_DevRef = atoi(token);
    free(result);
    
    int i;
    int count = d->content->versionCount;
    char* rid_dc[count];
    //char* result_dc[count];
    DataContent *dc[count];
    for(i=0;i<count;i++){
        dc[i] = (DataContent*)malloc(sizeof(DataContent));
        dc[i]->objContent = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    }
    
    printf("--------------------------------------------------[get @rid lastestCommon]\n");
    sprintf(sql,"SELECT in from (select expand(out_toDataContent) from #%s) where name = 'lastestCommon')",rid_dh);
    printf("SQL: %s\n",sql);
    rid_dc[0] = getRid(sql);
    printf("rid[0]: %s\n",rid_dc[0]);
    
    for(i=0;i<count;i++){
        printf("--------------------------------------------------[get isDiff,byteCount,data]\n");
        sprintf(sql,"SELECT isDiff,byteCount,data from #%s",rid_dc[i]);
        printf("SQL: %s\n",sql);
        result = getContent(sql);
        
        token = strtok(result, ":");
        token = strtok(NULL, ",");
        
        if(strcmp(token,"true")==0)
            dc[i]->isDiff = 1;
        else
            dc[i]->isDiff = 0;
        printf("\nisDiff: %d\n",dc[i]->isDiff);
        
        token = strtok(NULL,":");
        token = strtok(NULL,",");
        dc[i]->objContent->byteCount = atoi(token);
        printf("\nbyteCount: %d\n",dc[i]->objContent->byteCount);
        
        token = strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        dc[i]->objContent->data = strdup(token);
        printf("\ndata: %s\n",dc[i]->objContent->data);
        free(result);
        
        printf("--------------------------------------------------[get xmlSchema]\n");
        sprintf(sql,"SELECT xmlSchema from #%s",rid_dc[i]);
        printf("SQL: %s\n",sql);
        result_schema = getContent(sql);
        //printf("result_schema: %s\n",result_schema);
        char* temp = (char*)malloc(sizeof(char)*MAX_SCHEMA);
        memcpy(temp,result_schema+11,strlen(result_schema)-11);
        temp[strlen(temp)-1] = '\0';
        dc[i]->objContent->xmlSchema = temp;
        printf("\nxmlSchema: %s\n",dc[i]->objContent->xmlSchema);
        free(result_schema);

        if(i==0){
            dc[i]->preVersion = NULL;
            dc[i]->nextVersion = dc[i+1];
        }
        else if(i+1==count){
            dc[i]->preVersion = dc[i-1];
            dc[i]->nextVersion = NULL;
        }
        else {
            dc[i]->preVersion = dc[i-1];
            dc[i]->nextVersion = dc[i+1];
        }
        
        if(i+1!=count){
            printf("--------------------------------------------------[get @rid DataContent]\n");
            sprintf(sql,"SELECT in from (select expand(out_toDiffContent) from #%s) where status='next'",rid_dc[i]);
            //printf("SQL: %s\n",sql);
            rid_dc[i+1] = getRid(sql);
            printf("rid[%d]: %s\n",i+1,rid_dc[i+1]);
        }
    }
    
    // ออกจากลูป แล้วค่อยกำหนด head-last
    d->content->lastestCommon = dc[0];
    d->content->head = dc[count-1];
    
    // DeviceRef
    int index = d->content->index_DevRef;
    for(i=0;i<index;i++){
        d->content->userDevicePtr[i] = (DeviceRef*)malloc(sizeof(DeviceRef));
    }
    
    char **rid_devRef;
    sprintf(sql,"SELECT out from fromDeviceRef where in = #%s",rid_dh);
    rid_devRef = getArrayRid(sql);
    
    for(i=0;i<index;i++){
        sprintf(sql,"SELECT userID,deviceTransportID,nodeRefToDataContent from %s",rid_devRef[i]);
        printf("SQL: %s\n",sql);
        result = getContent(sql);
        printf("result[%d]: %s\n",i,result);
        
        token = strtok(result, "\"");
        token = strtok(NULL, "\"");

        d->content->userDevicePtr[i]->userID = strdup(token);
        
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        //printf("token_inner: %s\n", token);
        d->content->userDevicePtr[i]->deviceTransportID = strdup(token);
        
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        //printf("token_inner: %s\n", token);
        
        sprintf(d->content->userDevicePtr[i]->nodeRefToDataContent,"%s",token);
        free(result);
    }
    
    for(i=0;i<index;i++)
        free(rid_devRef[i]);
    free(rid_devRef);
    
    for(i=0;i<count;i++){
        free(rid_dc[i]);
    }
    free(rid_dh);
    
    return d;
}

Category** queryCategoryByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char* result;
    char* token;
    int i, count;
    
    sprintf(sql,"SELECT count(*) from (select expand(in('toData')) from Data where dataID='%s')",dataID);
    //printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    if(result==NULL){
        printf("FAILED >> incorrect dataID\n");
        return NULL;
    }
    
    token = strtok(result, ":");
    token = strtok(NULL, "l");
    count = atoi(token);
    printf("count: %d\n",count);
    
    Category** catPtr = (Category**)malloc(sizeof(Category*)*count+1);
    for(i=0;i<count;i++)
        catPtr[i] = (Category*)malloc(sizeof(Category));
    
    char* result_data[count];
    char** result_rid;
    sprintf(sql,"SELECT @rid from (select expand(in('toData')) from Data where dataID='%s')",dataID);
    result_rid = getArrayRid(sql);
    
    for(i=0;i<count;i++){
        sprintf(sql,"SELECT categoryID,categoryName from %s",result_rid[i]);
        printf("SQL: %s\n",sql);
        result_data[i] = getContent(sql);
        token = strtok(result_data[i], "\"");
        token = strtok(NULL, "\"");
        //printf("catID: %s\n",token);
        catPtr[i]->categoryID = strdup(token);
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        //printf("catName: %s\n",token);
        sprintf(catPtr[i]->categoryName,"%s",token);
        
        printf("\ncatID: %s\n",catPtr[i]->categoryID);
        printf("catName: %s\n",catPtr[i]->categoryName);
    }
    catPtr[i] = NULL;
    
    for(i=0;result_rid[i]!=NULL;i++){
        free(result_rid[i]);
    }
    free(result_rid);
    
    for(i=0;i<count;i++){
        free(result_data[i]);
    }
    
    free(result);
    return catPtr;
}

User** queryUserByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char* result;
    char* token;
    int i, count;
    
    sprintf(sql,"SELECT count(*) from (select expand(in('toData').in('toCategory')) from Data where dataID='%s')",dataID);
    //printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    if(result==NULL){
        printf("FAILED >> incorrect dataID\n");
        return NULL;
    }
    
    token = strtok(result, ":");
    token = strtok(NULL, "l");
    count = atoi(token);
    printf("count: %d\n",count);
    
    User** usrPtr = (User**)malloc(sizeof(User*)*count+1);
    for(i=0;i<count;i++)
        usrPtr[i] = (User*)malloc(sizeof(User));
    
    char* result_data[count];
    char** result_rid;
    sprintf(sql,"SELECT @rid from (select expand(in('toData').in('toCategory')) from Data where dataID='%s')",dataID);
    result_rid = getArrayRid(sql);
    
    for(i=0;i<count;i++){
        sprintf(sql,"SELECT userID,userName from %s",result_rid[i]);
        printf("SQL: %s\n",sql);
        result_data[i] = getContent(sql);
        token = strtok(result_data[i], "\"");
        token = strtok(NULL, "\"");
        usrPtr[i]->userID = strdup(token);
        
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        sprintf(usrPtr[i]->userName,"%s",token);
        
        printf("\nuserID: %s\n",usrPtr[i]->userID);
        printf("userName: %s\n",usrPtr[i]->userName);
    }
    usrPtr[i] = NULL;
    
    for(i=0;result_rid[i]!=NULL;i++){
        free(result_rid[i]);
    }
    free(result_rid);
    
    for(i=0;i<count;i++){
        free(result_data[i]);
    }
    
    free(result);
    return usrPtr;
}

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

char* getDataContentFirst(Data *data){
    return data->content->lastestCommon->objContent->data;
}

char* getDataContentLast(Data *data){
    DataContent* mydc;
    char* init_str = (char*)malloc(sizeof(char)*MAX_DIFF_SIZE);
    int i, j;
    int count = (data->content->versionCount-1)/FULL_CONTENT;
    int offset = (data->content->versionCount-1)%FULL_CONTENT;
    //printf("count: %d\n",count);
    //printf("offset: %d\n",offset);
    
//    printf("\n***********************************************\n");
    mydc=data->content->lastestCommon;
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
    if(ver>=data->content->versionCount){
        printf("FAILED >> incorrect version\n");
        free(init_str);
        return NULL;
    }
    mydc=data->content->lastestCommon;
    
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

DeviceRef** getDeviceRefList(Data* data){
    DeviceRef** devRef = (DeviceRef**)malloc(sizeof(DeviceRef*)*data->content->index_DevRef);
    
    int i;
    for(i=0;i<data->content->index_DevRef;i++){
        devRef[i] = (DeviceRef*)malloc(sizeof(DeviceRef));
        devRef[i]->userID = strdup(data->content->userDevicePtr[i]->userID);
        devRef[i]->deviceTransportID = strdup(data->content->userDevicePtr[i]->deviceTransportID);
        sprintf(devRef[i]->nodeRefToDataContent,"%s",data->content->userDevicePtr[i]->nodeRefToDataContent);
    }
    return devRef;
}

DeviceRef** getDeviceRefListByID(char* dataID){
    char sql[MAX_SQL_SIZE];
    char *rid_dh, *result;
    char *token;
    
    printf("--------------------------------------------------[get @rid DataHolder]\n");
    sprintf(sql,"SELECT @rid from (select expand(out('toDataHolder')) from Data where dataID ='%s')",dataID);
    printf("SQL: %s\n",sql);
    rid_dh = getRid(sql);
    
    printf("--------------------------------------------------[get @rid lastestCommon]\n");
    sprintf(sql,"SELECT index_DevRef from #%s",rid_dh);
    printf("SQL: %s\n",sql);
    result = getContent(sql);
    
    token = strtok(result, ":");
    token = strtok(NULL, "\n");
    
    int index_DevRef = atoi(token);
    printf("index: %d\n",index_DevRef);
    
    if(index_DevRef==0){
        free(rid_dh);
        free(result);
        return NULL;
    }
    
    char* result_data[index_DevRef];
    char** result_rid;
    sprintf(sql,"SELECT out from fromDeviceRef where in = #%s",rid_dh);
    result_rid = getArrayRid(sql);
    DeviceRef** devRef = (DeviceRef**)malloc(sizeof(DeviceRef*)*index_DevRef+1);
    
    int i;
    for(i=0;i<index_DevRef;i++){
        sprintf(sql,"SELECT userID,deviceTransportID,nodeRefToDataContent from %s",result_rid[i]);
        printf("SQL: %s\n",sql);
        result_data[i] = getContent(sql);
        printf("result_data[%d]: %s\n",i,result_data[i]);
        
        devRef[i] = (DeviceRef*)malloc(sizeof(DeviceRef));
        
        token = strtok(result_data[i], "\"");
        token = strtok(NULL, "\"");
        //printf("token_inner: %s\n", token);
        devRef[i]->userID = strdup(token);
        
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        //printf("token_inner: %s\n", token);
        devRef[i]->deviceTransportID = strdup(token);
        
        token = strtok(NULL, "\"");
        token = strtok(NULL, "\"");
        //printf("token_inner: %s\n", token);
        
        sprintf(devRef[i]->nodeRefToDataContent,"%s",token);
        
//        printf("\nuserID: %s\n",devRef[i]->userID);
//        printf("deviceTransportID: %s\n",devRef[i]->deviceTransportID);
//        printf("nodeRef: %s\n",devRef[i]->nodeRefToDataContent);
    }
    devRef[i] = NULL;
    
    for(i=0;i<index_DevRef;i++){
        free(result_rid[i]);
        free(result_data[i]);
    }
    free(result_rid);
    
    free(rid_dh);
    free(result);
    
    return devRef;
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
void testTag(Data* data){
    int ret;
    char mystr[1000];
    char content0[MAX_DIFF_SIZE] = "<book_id>bk100</book_id><author>Gambardella, Matthew</author><title>XML Developer's Guide</title><genre>Computer</genre><price>44.95</price><publish_date>2000-10-01</publish_date><description>An in-depth look at creating applications with XML.</description>";
    
    char content7[MAX_DIFF_SIZE] = "<book_id>bk107</book_id><author>Knorr, Stefan</author><title>Creepy Crawlies</title><genre>Horror</genre><price>4.95</price><publish_date>2000-12-06</publish_date><description>An anthology of horror stories about roaches, centipedes, scorpions and other insects.</description>";
    
    sprintf(mystr,"<root>%s</root>",content7);
    
    char content1[100] = "<author>Pimpat Teopanichkit</author>";
    char* partial_result;
    char diff_result[1000];

    ezxml_t root = ezxml_parse_str(mystr,(unsigned long)strlen(mystr)), partial_data;
    //printf("xml_raw: %s\n",ezxml_toxml(root));
    
    partial_data = ezxml_get(root,"author",-1);
    if(partial_data==NULL) {
        printf("FAILED >> incorrect tag\n");
        ezxml_free(root);
    }
    partial_result = ezxml_toxml(partial_data);
    printf("\npartial_result: %s\n", partial_result);
    sprintf(diff_result,"%s",getDiff(partial_result,content1).c_str());
    printf("\ndiff_result: %s\n", diff_result);

    ret = setNewDataDiffWithTag(data, "author", diff_result);
    printf(">>>number of byte(8)=\t%d\n",ret);
    
    free(partial_result);
    ezxml_free(root);
    
//    clock_t start, end, sum1, sum2;
//    
//    start = clock();
//    partial_data = ezxml_child(root,"author");
//    end = clock();
//    sum1 = end-start;
//    //printf("\npartial_data: %s\n\n",partial_data->txt);
//    
////    ezxml_t cut = ezxml_cut(partial_data);
////    printf("xml_partial: %s\n",ezxml_toxml(partial_data));
////    printf("xml_cut: %s\n",ezxml_toxml(cut));
//    
//    start = clock();
//    partial_data = ezxml_get(root,"author",-1);
//    end = clock();
//    sum2 = end-start;
//    //printf("\npartial_data: %s\n\n",partial_data->txt);
//    
//    printf("time1: %f\n",(double)sum1/CLOCKS_PER_SEC);
//    printf("time2: %f\n",(double)sum2/CLOCKS_PER_SEC);
//    ezxml_free(root);
    
}

void testsetNewDataContent(char* categoryID){
//    const char* uuid_usr;
//    const char* uuid_cat;
    const char* uuid_data;
//    uuid_usr = createUser((char*)"Pimpat");
//    uuid_cat = createCategory((char*)"Docs",(char*)uuid_usr);
    
    uuid_data = createNewData();
    printf("my_uuid: %s\n",uuid_data);
    printf("ver_keeped: %d\n",mydata->content->versionCount);
    
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
    printf("result[lastestCommon]: %s\n\n",result[8]);
    
    result[9] = getDataContentLast(mydata);
    printf("result[last]: %s\n\n",result[9]);
  
    char* partial_result;
    partial_result = getDataContentWithTag(mydata,"genre");
    printf("partial_result(from tag): %s\n\n",partial_result);
    
    testTag(mydata);

    result[8] = getDataContentVer(mydata,8);
    printf("result[8]: %s\n\n",result[8]);
    
    addData2Category(categoryID,mydata);
    
    const char* uuid_usr2;
    const char* uuid_cat2;
    uuid_usr2 = createUser("Pasit");
    uuid_cat2 = createCategory("Games");

    addCategory2User((char*)uuid_usr2,(char*)uuid_cat2);
    addData2CategoryTest((char*)uuid_cat2,mydata);
    
//  getDeviceRefList -----------------------------------------------------------
    DeviceRef** devRef;
    devRef = getDeviceRefList(mydata);
    
    printf("===== DeviceRefList =====\n");
    int j;
    for(j=0;j<mydata->content->index_DevRef;j++){
        printf("userID: %s\n",devRef[j]->userID);
        printf("deviceTransportID: %s\n",devRef[j]->deviceTransportID);
        printf("nodeRef: %s\n",devRef[j]->nodeRefToDataContent);
    }
    
    //  free devRef!!
    for(j=0;j<mydata->content->index_DevRef;j++){
        free((char*)mydata->content->userDevicePtr[j]->deviceTransportID);
        free((char*)mydata->content->userDevicePtr[j]->userID);
        free((char*)mydata->content->userDevicePtr[j]);
        
        free((char*)devRef[j]->deviceTransportID);
        free((char*)devRef[j]->userID);
        free((char*)devRef[j]);
    }
    free(devRef);
//------------------------------------------------------------------------------

    printf("xmlSchema: %s\n\n",mydata->content->lastestCommon->objContent->xmlSchema);
    
//  getCurLevelFromCommon/Common ----------------------------------------
//    int check;
//    check = getCurLevelFromCommon(mydata);
//    printf("lastestCommon: %d\n\n",check);
//    check = getCurLevelFromHead(mydata);
//    printf("common: %d\n",check);
//------------------------------------------------------------------------------

//  getDiffDataAtlastestCommon/head + getDiffDataPre/NextVer -------------------
//    char* temp;
//    temp = getDiffDataAtlastestCommon(mydata);
//    printf("lastestCommon: %s\n",temp);
//    temp = getDiffDataAthead(mydata);
//    printf("last: %s\n",temp);
//    
//    int j;
//    for(j=0;j<9;j++){
//        temp = getDiffDataPreVer(mydata);
//        if(temp!=NULL)
//            printf("data(pre): %s\n",temp);
//    }
//    for(j=0;j<9;j++){
//        temp = getDiffDataNextVer(mydata);
//        if(temp!=NULL)
//            printf("data(next): %s\n",temp);
//    }
//    
////    mydata->content->current = mydata->content->current->preVersion;
////    temp = getDiffDataNextVer(mydata);
////    printf("next: %s\n",temp);
////    temp = getDiffDataPreVer(mydata);
////    printf("pre: %s\n",temp);
//------------------------------------------------------------------------------
    
//  getContentPre/NextVer ------------------------------------------------------
//    ObjectBinary* myobj[20];
//    int used=0;
//    int j;
//    for(j=0;j<9;j++){
//        myobj[used] = getContentPreVer(mydata);
//        if(myobj[used]!=NULL){
//            printf("data(pre): %s\n",myobj[used]->data);
//            used++;
//        }
//    }
//    for(j=0;j<9;j++){
//        myobj[used] = getContentNextVer(mydata);
//        if(myobj[used]!=NULL){
//            printf("data(next): %s\n",myobj[used]->data);
//            used++;
//        }
//    }
//    printf("used: %d\n",used);
//    
//    for(j=0;j<used;j++){
//        free(myobj[j]->data);
//        free(myobj[j]->xmlSchema);
//        free(myobj[j]);
//    }
//------------------------------------------------------------------------------

//  getDataContent/Commonversion -----------------------------------------------
//    ObjectBinary* obj_temp;
//    ObjectBinary* obj_temp2;
//    obj_temp = getDataContent(mydata);
//    printf("xmlSchema: %s\n",obj_temp->xmlSchema);
//    printf("byteCount: %d\n",obj_temp->byteCount);
//    printf("data: %s\n",obj_temp->data);
//    
//    printf("\ntest_current: %s\n",mydata->content->current->objContent->data);
//    printf("test_current(byte): %d\n",mydata->content->current->objContent->byteCount);
//    
//    free(obj_temp->xmlSchema);
//    free(obj_temp->data);
//    free(obj_temp);
//    
//    obj_temp2 = getDataContentCommonVersion(mydata);
//    printf("xmlSchema: %s\n",obj_temp2->xmlSchema);
//    printf("byteCount: %d\n",obj_temp2->byteCount);
//    printf("data: %s\n",obj_temp2->data);
//    
//    free(obj_temp2->xmlSchema);
//    free(obj_temp2->data);
//    free(obj_temp2);
//------------------------------------------------------------------------------

    int i;
    for(i=0;i<10;i++){
        //if(i!=8)
        free(result[i]);
    }
    free(result);

    free(partial_result);
    
    free((char*)uuid_cat2);
    free((char*)uuid_usr2);
    
    printf("\n\n");
    DataContent *mydc, *next_mydc;
    for(mydc=mydata->content->lastestCommon;mydc!=NULL;mydc=next_mydc){
        next_mydc = mydc->nextVersion;
        printf("byteCount: %d\n",mydc->objContent->byteCount);
        free(mydc->objContent->xmlSchema);
        free(mydc->objContent->data);
        free(mydc->objContent);
        free(mydc);
    }
    
    free(mydata->content);
    free((char*)mydata->dataID);
    free((char*)mydata->chatRoom);
    free(mydata);
}

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

void testqueryCategoryByID(char* myID){
    Category** mycat;
    mycat = queryCategoryByID(myID);
    printf("\n[test]\n");
    int i;
    for(i=0;mycat[i]!=NULL;i++){
        printf("catID: %s\n",mycat[i]->categoryID);
        printf("catName: %s\n",mycat[i]->categoryName);
        free((char*)mycat[i]->categoryID);
        free(mycat[i]);
    }
    free(mycat);
    
}

void testqueryUserByID(char* myID){
    User** myusr;
    myusr = queryUserByID(myID);
    printf("\n[test]\n");
    int i;
    for(i=0;myusr[i]!=NULL;i++){
        printf("userID: %s\n",myusr[i]->userID);
        printf("userName: %s\n",myusr[i]->userName);
        free((char*)myusr[i]->userID);
        free(myusr[i]);
    }
    free(myusr);
}

void testDraft2(){
    const char* uuid_org;
    const char* uuid_usr;
    const char* uuid_cat;
    
    uuid_org = createOrg("Throughwave");
    uuid_usr = createUser("Pimpat");
    uuid_cat = createCategory("Documents");
    
    addUser2Org((char*)uuid_org,(char*)uuid_usr);
    addCategory2User((char*)uuid_usr,(char*)uuid_cat);
    testsetNewDataContent((char*)uuid_cat);
    
    free((char*)uuid_org);
    free((char*)uuid_usr);
    free((char*)uuid_cat);

//  getTagContent(2) + setTagContent -------------------------------------------
//    char content7[MAX_DIFF_SIZE] = "<book_id>bk107</book_id><author>Knorr, Stefan</author><title>Creepy Crawlies</title><genre>Horror</genre><price>4.95</price><publish_date>2000-12-06</publish_date><description>An anthology of horror stories about roaches, centipedes, scorpions and other insects.</description>";
//    
//    ObjectBinary* myobj = (ObjectBinary*)malloc(sizeof(ObjectBinary));
//    myobj->data = strdup(content7);
//    myobj->byteCount = strlen(myobj->data);
//    Text result;
//    //result = getTagContent(myobj,"description");
//    result = getTagContent(myobj,6);
//    printf("output1: %s\n\n",result);
//    
//    setTagContent(myobj,"1234567890","book_id");
//    printf("output2: %s\n", myobj->data);
//    
//    free(result);
//    free(myobj->data);
//    free(myobj);
//------------------------------------------------------------------------------
}

void testDraft2_ByID(){
    
//  getDataContentCommonVersionByID --------------------------------------------
//     ObjectBinary* myobj;
//     myobj = getDataContentCommonVersionByID("9C7279CFD0FA400E9F41391FE843A075");
//     
//     printf("\n\nxml: %s\n",myobj->xmlSchema);
//     printf("data: %s\n",myobj->data);
//     printf("byteCount: %d\n",myobj->byteCount);
//     
//     free(myobj->xmlSchema);
//     free(myobj->data);
//     free(myobj);
//------------------------------------------------------------------------------

//  getDataContentByID ---------------------------------------------------------
//     ObjectBinary* myobj;
//     myobj = getDataContentByID("A19E592D09344701B6B4504CB1D55DCB");
//     printf("\n\nxml: %s\n",myobj->xmlSchema);
//     printf("data: %s\n",myobj->data);
//     printf("byteCount: %d\n",myobj->byteCount);
//     
//     free(myobj->xmlSchema);
//     free(myobj->data);
//     free(myobj);
//------------------------------------------------------------------------------
    
//  getContentNext/PreVerByID --------------------------------------------------
//    ObjectBinary *myobj_next, *myobj_pre;
//    myobj_next = getContentNextVerByID("A19E592D09344701B6B4504CB1D55DCB");
//    printf("----NEXT----\n");
//    if(myobj_next!=NULL){
//        printf("\n\nxml: %s\n",myobj_next->xmlSchema);
//        printf("data: %s\n",myobj_next->data);
//        printf("byteCount: %d\n",myobj_next->byteCount);
//    
//        free(myobj_next->xmlSchema);
//        free(myobj_next->data);
//        free(myobj_next);
//    }
//
//    printf("----PRE----\n");
//    myobj_pre = getContentPreVerByID("A19E592D09344701B6B4504CB1D55DCB");
//    if(myobj_pre!=NULL){
//        printf("\n\nxml: %s\n",myobj_pre->xmlSchema);
//        printf("data: %s\n",myobj_pre->data);
//        printf("byteCount: %d\n",myobj_pre->byteCount);
//    
//        free(myobj_pre->xmlSchema);
//        free(myobj_pre->data);
//        free(myobj_pre);
//    }
//------------------------------------------------------------------------------
    
//  getDiffDataNext/PreVerByID -------------------------------------------------
//    char *next, *pre;
//    next = getDiffDataNextVerByID("A19E592D09344701B6B4504CB1D55DCB");
//    pre = getDiffDataPreVerByID("A19E592D09344701B6B4504CB1D55DCB");
//    printf("\n\nnext: %s\n",next);
//    printf("\n\npre: %s\n",pre);
//    free(next);
//    free(pre);
//------------------------------------------------------------------------------
    
//  getDataContentWithTagByID --------------------------------------------------
//    char* partial;
//    partial = getDataContentWithTagByID("A19E592D09344701B6B4504CB1D55DCB","author");
//    printf("\npartial: %s\n", partial);
//    if(partial!=NULL)
//        free(partial);
//------------------------------------------------------------------------------
    
//  getDeviceRefListByID -------------------------------------------------------
//    DeviceRef** devRef;
//    devRef = getDeviceRefListByID("A19E592D09344701B6B4504CB1D55DCB");
//    if(devRef!=NULL){
//        int i;
//        for(i=0;devRef[i]!=NULL;i++){
//            printf("\nuserID: %s\n",devRef[i]->userID);
//            printf("deviceTransportID: %s\n",devRef[i]->deviceTransportID);
//            printf("nodeRef: %s\n",devRef[i]->nodeRefToDataContent);
//        
//            free((char*)devRef[i]->userID);
//            free((char*)devRef[i]->deviceTransportID);
//            free(devRef[i]);
//        }
//        free(devRef);
//    }
//------------------------------------------------------------------------------
    
//  getDataContent -------------------------------------------------------------
//    char* partial;
//    partial = getDataContent(NULL,"A19E592D09344701B6B4504CB1D55DCB","author");
//    printf("\npartial: %s\n", partial);
//        if(partial!=NULL)
//            free(partial);
//------------------------------------------------------------------------------
    
//  getDiffDataAtlastestCommon/headByID ----------------------------------------
//    char *head, *last;
//    last = getDiffDataAtlastestCommonByID("A19E592D09344701B6B4504CB1D55DCB");
//    head = getDiffDataAtheadByID("A19E592D09344701B6B4504CB1D55DCB");
//    printf("\n\nlast: %s\n",last);
//    printf("\n\nhead: %s\n",head);
//    free(head);
//    free(last);
//------------------------------------------------------------------------------
    
}

void testqueryDataByID(){
    Data* mydata;
    mydata = queryDataByID("A19E592D09344701B6B4504CB1D55DCB");
    
    /* Print for check all data */
    printf("\n-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-\n");
    
    //  Data
    printf("dataID: %s\n",mydata->dataID);
    printf("dataName: %s\n",mydata->dataName);
    printf("chatRoom: %s\n",mydata->chatRoom);
    
    //  DataHolder
    printf("ver_keeped: %d\n",mydata->content->versionKeeped);
    printf("ver_count: %d\n",mydata->content->versionCount);
    printf("index: %d\n",mydata->content->index_DevRef);
    
    //  DataContent
    DataContent *mydc, *next_mydc;
    for(mydc=mydata->content->lastestCommon;mydc!=NULL;mydc=next_mydc){
        next_mydc = mydc->nextVersion;
        printf("isDiff: %d\n",mydc->isDiff);
        printf("byteCount: %d\n",mydc->objContent->byteCount);
        printf("xml: %s\n",mydc->objContent->xmlSchema);
        printf("data: %s\n",mydc->objContent->data);
    }
    
    //  DeviceRef
    int j;
    for(j=0;j<mydata->content->index_DevRef;j++){
        printf("userID: %s\n",mydata->content->userDevicePtr[j]->userID);
        printf("deviceTransportID: %s\n",mydata->content->userDevicePtr[j]->deviceTransportID);
        printf("nodeRef: %s\n",mydata->content->userDevicePtr[j]->nodeRefToDataContent);
    }
    
    printf("\n-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-..-\n");
    
    //  free all data
    for(j=0;j<mydata->content->index_DevRef;j++){
        free((char*)mydata->content->userDevicePtr[j]->deviceTransportID);
        free((char*)mydata->content->userDevicePtr[j]->userID);
        free((char*)mydata->content->userDevicePtr[j]);
    }
    
    for(mydc=mydata->content->lastestCommon;mydc!=NULL;mydc=next_mydc){
        next_mydc = mydc->nextVersion;
        free(mydc->objContent->xmlSchema);
        free(mydc->objContent->data);
        free(mydc->objContent);
        free(mydc);
    }
    
    free(mydata->content);
    free((char*)mydata->dataID);
    free((char*)mydata->chatRoom);
    free(mydata);
}

