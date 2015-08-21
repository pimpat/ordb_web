#ifdef __cplusplus
extern "C" {
#endif

int addByte(char byte, char *buffer);
int addBytes(char *bytes, char len, char *buffer);
int addShort(short num, char *buffer);
int addInt(int num, char *buffer);
int addLong(long num ,char *buffer);
int addString(char *str, char *buffer);
int reqConnectMsg(char *message, char *driverName, char *driverVersion, short protocolVersion, char *clientId, char *username, char *password);
int reqDBOpenMsg(char *message, char *driverName, char *driverVersion, short protocolVersion, char *clientId, char *dbName, char *dbType, char *username, char *password);
int reqDBCreateMsg(char *message, char *dbName, char *dbType, char *storageType);
int reqDBDopMsg(char *message, char *dbName, char *storageType);
int reqDBCloseMsg(char *message);
//int reqClusterAdd(char *message, char *name, short id);
int reqRecLoadMsg(char *message, short clusterId, long clusterPos);
int reqRecCreateMsg(char *message, short clusterId, char *recordContect);
int reqCommandMsg(char *message, char *className, char *command);
int reqQueryMsg(char *message, char *className, char *query);
int reqQueryMsg_Binary(char *message, char *className, char *query);
int reqTX_Commit(char *message, int tx, char* tx_entry);

#define REQUEST_SHUTDOWN    1
#define REQUEST_CONNECT     2
#define REQUEST_DB_OPEN     3
#define REQUEST_DB_CREATE   4
#define REQUEST_DB_CLOSE    5
#define REQUEST_DB_EXIST    6
#define REQUEST_DB_DROP     7
#define REQUEST_DB_SIZE     8
#define REQUEST_DB_COUNTRECORDS     9
//#define REQUEST_DATACLUSTER_ADD     10
//#define REQUEST_DATACLUSTER_DROP    11
//#define REQUEST_DATACLUSTER_COUNT   12
//#define REQUEST_DATACLUSTER_DATARANGE   13
#define REQUEST_RECORD_LOAD     30
#define REQUEST_RECORD_CREATE   31
#define REQUEST_RECORD_UPDATE   32
#define REQUEST_RECORD_DELETE   33
#define REQUEST_COMMAND     41
#define REQUEST_TX_COMMIT   60

#ifdef __cplusplus
}
#endif
