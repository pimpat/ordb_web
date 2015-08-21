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
#include <jansson.h>
#include "reqmsg.h"
#include "swap_endian.h"
#include "diff_match_patch.h"

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 2424

#define DB_NAME "test-jansson"
#define MAX_SQL_SIZE (2000)
#define MAX_DIFF_SIZE (1000)
#define MAX_CONTENT_SIZE (1000)
#define VER_KEEPED (10)

#define TRUE 1
#define FALSE 0

using namespace std;

int Sockfd;
typedef struct Packet {
    char opType;
    char ssid[4];
    char msg[20000];
} Packet;
Packet GPacket;

typedef struct Addr{
    short cltid;
    long rid;
} Addr;
Addr addr;

//----------------------------------

typedef struct ChatRef{
    const char* chatRefURL;
} ChatRef;

typedef struct CWTimeStamp{
    char* timeStampCode;
} CWTimeStamp;

typedef char* Text;
typedef char* Schema;

typedef struct ObjectBinary {
    int schemaCode;
    int byteCount;
    char* data;
} ObjectBinary;

typedef struct DataHolder DataHolder;
typedef int t_bool;
typedef struct DataContent{
    DataHolder* dataHd;
    DataContent* nextVersion;
    DataContent* preVersion;
    char* SHA256hashCode;
    CWTimeStamp* timeStamp;
    ObjectBinary* objContent;
    t_bool isDiff;
} DataContent;

struct DataHolder{
    DataContent* lastestCommon;
    DataContent* head;
    DataContent* current;
    int versionKeeped;
};

typedef struct Data{
    const char* dataID;
    int dataType;
    DataHolder* content;
    Text* attachmentFTOLinks;   //  NULL
    ChatRef* chatRoom;          //  NULL
} Data;

typedef Data Org;
typedef Data User;
typedef Data Category;
typedef Data State;
typedef Data Task;
typedef Data SubTask;

typedef int ReturnErr;

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

/* datamodel function */
const char* stringUUID();
const char* createNewData();
DataHolder* createNewDataHolder();

ObjectBinary* getDataContent(Data* data);
ObjectBinary* getDataContentLastestCommon(Data* data);
ObjectBinary* getDataContentByID(char* dataID);
ObjectBinary* getDataContentLastestCommonByID(char* dataID);

Text getDiffDataAtHead(Data* data);
Text getDiffDataAtLastestCommon(Data* data);
Text getDiffDataAtHeadByID(char* dataID);
Text getDiffDataAtLastestCommonByID(char* dataID);

ObjectBinary* getContentNextVer(Data* data);
ObjectBinary* getContentPreVer(Data* data);
ObjectBinary* getContentNextVerByID(char* dataID);
ObjectBinary* getContentPreVerByID(char* dataID);

Text getDiffDataNextVer(Data* data);
Text getDiffDataPreVer(Data* data);
Text getDiffDataNextVerByID(char* dataID);
Text getDiffDataPreVerByID(char* dataID);

Text getDataContentWithTag(Data* data, char* tagName);
Text getDataContentWithTagByID(char* dataID, char* tagName);

ReturnErr setNewDataContent(Data* data, ObjectBinary* content);
ReturnErr setNewDataDiffWithTag(Data* data, char* tagName, Text diff);

Text getTagContentWithName(ObjectBinary* fullContent, char* tagName);
Text getTagContent(ObjectBinary* fullContent, int tagNameEnum);
ReturnErr setTagContent(ObjectBinary* fullContent, Text newTagContent, char* tagName);

const char* createOrg(Text orgName, Schema* orgSchema);
const char* createUser(Text userName, Schema* userSchema);
const char* createCategory(Text categoryName, Schema* categorySchema);
const char* createState(Text stateName, Schema* stateSchema);
const char* createTask(Text taskName, Schema* );
const char* createSubTask(Text subTaskName);
const char* createData(Text dataName, Schema* dataSchema);

Text getDataContentForKey(Schema* schema, uuid_t objID, char* keyName);

ReturnErr addData2Category(char* categoryID, Data* data);
ReturnErr addTask2Category(char* categoryID, Task *task);
ReturnErr addSubTask2Task(char* taskID, SubTask *subTask);
ReturnErr addState2Category(char* categoryID, State *state);
ReturnErr addCategory2User(char* userID, char* categoryID);
ReturnErr addUser2Org(char* orgID, char* userID);
//  addTaskToState ???

ReturnErr removeDataFromCategory(char* categoryID, char* dataID);
ReturnErr removeTaskFromCategory(char* categoryID, char* taskID);
ReturnErr removeSubTaskFromTask(char* taskID, char* subTaskID);
ReturnErr removeStateFromCategory(char* categoryID, char* stateID);
ReturnErr removeCategoryFromUser(char* userID, char* categoryID);
ReturnErr removeUserFromOrg(char* orgID, char* userID);
//  removeTaskFromState ???

Data* queryDataByID(char* dataID);
State** queryAllStatesFromData(char* dataID);
Category** queryAllCategoriesFromData(char* dataID);
User** queryAllUsersFromData(char* dataID);
//  queryOrg/Task/SubTask ???

bool isObjectUnderState(char* stateID, char* objID);
bool isObjectUnderCategory(char* categoryID, char* objID);
bool isObjectOwnedByUser(char* userID, char* objID);

ReturnErr deleteObj(char* userID, char* objID);
ReturnErr flushTrash(char* userID);
//  delete flag / delete edge

/* diff-patch function */
string getDiff(char* old_str, char* new_str);
string getPatch(char* str, char* str_patch);
char* getDataContentFirst(Data* data);
char* getDataContentLast(Data* data);
char* getDataContentVer(Data* data,int ver);

ObjectBinary* createNewObjBinary(char* data);
int countDataContent(Data* data);
void testDiffPatchJson();

int main(){
    testDiffPatchJson();
    
    return 0;
}

string getDiff(char* old_str, char* new_str){
    diff_match_patch<wstring> dmp;
    string s;
    wstring strPatch;
    wstring str1 = wstring(new_str,new_str+strlen(new_str));
//    wcout << str1 << endl;
    wstring str0 = wstring(old_str,old_str+strlen(old_str));
//    wcout << str0 << endl;
    strPatch = dmp.patch_toText(dmp.patch_make(str0, str1));
    s.assign(strPatch.begin(),strPatch.end());
//    wcout << strPatch << endl;
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
    mydata->dataType = 1;   // fix
    mydata->content = createNewDataHolder();
    mydata->attachmentFTOLinks = NULL;
    mydata->chatRoom = NULL;
    return mydata->dataID;
}

DataHolder* createNewDataHolder(){
    DataHolder* dh = (DataHolder*)malloc(sizeof(DataHolder));
    dh->versionKeeped = VER_KEEPED;
    dh->head = NULL;
    dh->lastestCommon = NULL;
    dh->current = NULL;
    return dh;
}

ObjectBinary* createNewObjBinary(char* data){
    ObjectBinary* obj = (ObjectBinary*)malloc(sizeof(ObjectBinary));
    obj->schemaCode = 0;    // fix
    obj->byteCount = strlen(data);
    obj->data = strdup(data);
    return obj;
}

int countDataContent(Data* data){
    DataContent* dc = data->content->lastestCommon;
    int i=0;
    while(dc!=NULL){
        i++;
        dc = dc->nextVersion;
    }
    return i-1;
}

int setNewDataContent(Data* data, ObjectBinary* content){
    if(data->content->head==NULL && data->content->lastestCommon==NULL){
        printf("\n[ DATA #1 ]\n");
        
        DataContent* dc0 = (DataContent*)malloc(sizeof(DataContent));
        DataContent* dc1 = (DataContent*)malloc(sizeof(DataContent));
        DataContent* dc1_1 = (DataContent*)malloc(sizeof(DataContent));
        
        DataHolder* dh = data->content;
        dh->lastestCommon = dc0;
        dh->head = dc1;
        dh->current = dc1;
        
        dc0->preVersion = NULL;
        dc0->nextVersion = dc1;
        dc1->preVersion = dc0;
        dc1->nextVersion = dc1_1;
        dc1_1->preVersion = NULL;
        dc1_1->nextVersion = NULL;
        
        //  dc0
        dc0->dataHd = dh;
        dc0->SHA256hashCode = strdup("default");
        dc0->timeStamp = NULL;
        dc0->isDiff = TRUE;
        
        ObjectBinary* obj0 = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc0->objContent = obj0;
        obj0->schemaCode = content->schemaCode;
        obj0->data = strdup("{}");
        obj0->byteCount = strlen(obj0->data);
        
        //  dc1
        dc1->dataHd = dh;
        dc1->SHA256hashCode = strdup("default");
        dc1->timeStamp = NULL;
        dc1->isDiff = FALSE;
        
        ObjectBinary* obj1 = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc1->objContent = obj1;
        obj1->schemaCode = content->schemaCode;
        obj1->data = strdup(content->data);
        obj1->byteCount = content->byteCount;
        
        //  dc1_1
        dc1_1->dataHd = NULL;
        dc1_1->SHA256hashCode = strdup("default");
        dc1_1->timeStamp = NULL;
        dc1_1->isDiff = TRUE;
        
        ObjectBinary* obj1_1 = (ObjectBinary*)malloc(sizeof(ObjectBinary));
        dc1_1->objContent = obj1_1;
        obj1_1->schemaCode = content->schemaCode;
        
        string s = getDiff(obj0->data,obj1->data);
        obj1_1->data = strdup(s.c_str());
        obj1_1->byteCount = strlen(obj1_1->data);
        
        printf("init: %s\n",obj0->data);
        printf("new: %s\n",obj1->data);
        printf("diff: %s\n",obj1_1->data);
        
        return obj1->byteCount;
        
    }
    else{
        int total = countDataContent(data);
        printf("\n[ DATA #%d ]\n",total);
        
        DataContent* dc1 = data->content->head;
        DataContent* dc1_1 = data->content->head->nextVersion;
        DataContent* dc_new = (DataContent*)malloc(sizeof(DataContent));
        
        DataHolder* dh = data->content;
        dh->head = dc1;
        dh->current = dc1;
        
        //  ค่าเก็บใน dc_new แทน
        printf("old: %s\n",dc1->objContent->data);
        printf("new: %s\n",content->data);
        string s = getDiff(content->data,dc1->objContent->data);

        DataContent* dc0 = dc1->preVersion;
        dc0->nextVersion = dc1_1;
        dc1_1->preVersion = dc0;
        dc1_1->nextVersion = dc1;
        dc1->preVersion = dc1_1;
        dc1->nextVersion = dc_new;
        dc_new->preVersion = NULL;
        dc_new->nextVersion = NULL;
        
        //  dc1
        dc1->dataHd = dh;
        dc1->SHA256hashCode = strdup("default");
        dc1->timeStamp = NULL;
        dc1->isDiff = FALSE;
        
        dc1->objContent->schemaCode = content->schemaCode;
        free(dc1->objContent->data);
        dc1->objContent->data = strdup(content->data);
        dc1->objContent->byteCount = content->byteCount;
        
//        printf("pass\n");
        //  dc_new
        dc_new->dataHd = NULL;
        dc_new->SHA256hashCode = strdup("default");
        dc_new->timeStamp = NULL;
        dc_new->isDiff = TRUE;
        printf("pass\n");
        dc_new->objContent->schemaCode = content->schemaCode;
        printf("pass\n");
        dc_new->objContent->data = strdup(s.c_str());
        printf("pass\n");
        dc_new->objContent->byteCount = strlen(dc_new->objContent->data);
        printf("pass\n");
        return dc1->objContent->byteCount;
    }
    
}

void testDiffPatchJson(){
    const char* uuid_data;
    uuid_data = createNewData();
    
    printf("my_uuid: %s\n",uuid_data);
    printf("ver_keeped: %d\n\n",mydata->content->versionKeeped);
    
    const char* nickname[] = {"pim","poom","pon"};
    const char* name[] = {"pimpat","tanapoom","tanapon"};
    int age[] = {24,18,13};
    
    char* ret_strings = NULL;
    json_t* root = json_object();
    ret_strings = json_dumps(root,JSON_INDENT(2));
//    printf("%s\n",ret_strings);
    json_object_set_new( root, "nickname", json_string(nickname[0]));
    json_object_set_new( root, "name", json_string(name[0]));
    json_object_set_new( root, "age", json_integer(age[0]));
    
    ret_strings = json_dumps(root,JSON_INDENT(2));
//    printf("%s\n",ret_strings);
    ObjectBinary* obj1 = createNewObjBinary(ret_strings);
    
    json_object_set(root,"name", json_string(name[1]));
    ret_strings = json_dumps(root,JSON_INDENT(2));
//    printf("%s\n",ret_strings);
    ObjectBinary* obj2 = createNewObjBinary(ret_strings);
    
    int total = countDataContent(mydata);
    printf("total(0): %d\n",total);
    setNewDataContent(mydata,obj1);
    setNewDataContent(mydata,obj2);
    
    printf("okay\n");
    
}

