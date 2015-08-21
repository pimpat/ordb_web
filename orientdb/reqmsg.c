#include <string.h>
#include <stdio.h>

#include "reqmsg.h"

int addBytes(char *bytes, char len, char *buffer) {
    if (len >= 0) {
        addInt(len, buffer);
        memcpy(buffer+4, bytes, len);
        return sizeof(int)+len;
    } else {
        addInt(-1, buffer);
        return sizeof(int);
    }
}

int addByte(char byte, char *buffer) {
    buffer[0] = byte;
    return sizeof(char);
}
int addShort(short num, char *buffer) {
    buffer[0] = (num>>8) & 0xff;
    buffer[1] = (num) & 0xff;
    return sizeof(short);
}
int addInt(int num, char *buffer) {
    buffer[0] = (num>>24) & 0xff;
    buffer[1] = (num>>16) & 0xff;
    buffer[2] = (num>>8) & 0xff;
    buffer[3] = (num) & 0xff;
    return sizeof(int);
}
int addLong(long num ,char *buffer) {
    buffer[0] = (num>>56) & 0xff;
    buffer[1] = (num>>48) & 0xff;
    buffer[2] = (num>>40) & 0xff;
    buffer[3] = (num>>32) & 0xff;
    buffer[4] = (num>>24) & 0xff;
    buffer[5] = (num>>16) & 0xff;
    buffer[6] = (num>>8) & 0xff;
    buffer[7] = (num) & 0xff;
    return sizeof(long);
}
int addString(char *str, char *buffer) {
    if (str != NULL) {
        int len = (int)strlen(str);
        addInt(len, buffer);
        memcpy(buffer+4, str, len);
        return sizeof(int)+len;
    } else {
        addInt(-1, buffer);
        return sizeof(int);
    }
}

int reqConnectMsg(char *message, char *driverName, char *driverVersion, short protocolVersion, char *clientId, char *username, char *password) {
    int size = 0;
    size += addString(driverName, message+size);
    size += addString(driverVersion, message+size);
    size += addShort(protocolVersion, message+size);
    size += addString(clientId, message+size);
    
    //  for version 28
    size += addString("ORecordDocument2csv", message+size);
    size += addByte(0, message+size);
    
    size += addString(username, message+size);
    size += addString(password, message+size);
    return size;
}

int reqDBOpenMsg(char *message, char *driverName, char *driverVersion, short protocolVersion, char *clientId, char *dbName, char *dbType, char *username, char *password) {
    int size = 0;
    size += addString(driverName, message+size);
    size += addString(driverVersion, message+size);
    size += addShort(protocolVersion, message+size);
    size += addString(clientId, message+size);
    
    //  for version 28
    size += addString("ORecordDocument2csv", message+size);
    size += addByte(0, message+size);
    
    size += addString(dbName, message+size);
    size += addString(dbType, message+size);
    size += addString(username, message+size);
    size += addString(password, message+size);
    return size;
}

int reqDBCreateMsg(char *message, char *dbName, char *dbType, char *storageType) {
    int size = 0;
    size += addString(dbName, message+size);
    size += addString(dbType, message+size);
    size += addString(storageType, message+size);
    return size;
}

int reqDBDopMsg(char *message, char *dbName, char *storageType) {
    int size = 0;
    size += addString(dbName, message+size);
    size += addString(storageType, message+size);
    return size;
}

int reqDBCloseMsg(char *message) {
    return 0;
}

//int reqClusterAdd(char *message, char *name, short id) {
//    int size = 0;
//    size += addShort(id, message);
//    size += addString(name, message);
//    return size;
//}

int reqRecLoadMsg(char *message, short clusterId, long clusterPos) {
    int size = 0;
    size += addShort(clusterId, message+size);
    size += addLong(clusterPos, message+size);
    size += addInt(-1, message+size);
    size += addByte(1, message+size);
    size += addByte(0, message+size);
    return size;
}

int reqTX_Commit(char *message, int tx, char* tx_entry) {
    int size = 0;
    size += addInt(tx, message+size);   // tx-id
    size += addByte(1, message+size);   // using tx-log
    size += addByte(3, message+size);   // operation-type
//    size += addInt((int)strlen(tx_entry)+sizeof(short)+sizeof(long)+2*sizeof(char), message+size);
    

//    size += addInt(3,message+size);
//        size += addShort(11, message+size);  // cluster-id
//        size += addLong(79, message+size);   // cluster-pos
        size += addByte('d', message+size); // record-type
    
    
    size += addString(tx_entry, message+size);  // entry-content
    size += addByte(0, message+size);
//    printf("size: %d\n",size-(2+sizeof(int)));
    //size += addInt(size-(2+sizeof(int)+4), message+(2+sizeof(int)));
    
//    size += addByte('d', message+size); // record-type
//    size += addByte(0, message+size);   // end-of-records
    return size;
}

//  this func
int reqRecCreateMsg(char *message, short clusterId, char *recordContent) {
    int size = 0;
    //size += addInt(-1, message+size);
    size += addShort(clusterId, message+size);
    size += addString(recordContent, message+size);
    size += addByte('d', message+size);
    size += addByte(0, message+size);
    return size;
}

int reqCommandMsg(char *message, char *className, char *command) {
    int size = 0;
    size += addByte('s', message+size);
    size += addString(className, message+size+sizeof(int));
    size += addString(command, message+size+sizeof(int));
    size += addByte(0, message+size+sizeof(int));
    size += addByte(0, message+size+sizeof(int));
    size += addInt(size-sizeof(char), message+sizeof(char));
    return size;
}

int reqQueryMsg(char *message, char *className, char *query) {
    int size = 0;
    size += addByte('s', message+size);
    size += addString(className, message+size+sizeof(int));
    size += addString(query, message+size+sizeof(int));
    size += addInt(-1, message+size+sizeof(int));
    size += addInt(0, message+size+sizeof(int));
    size += addInt(0, message+size+sizeof(int));
    size += addInt(size-sizeof(char), message+sizeof(char));
    return size;
}

int reqQueryMsg_Binary(char *message, char *className, char *query) {
    int size = 0;
    size += addByte('s', message+size);
    size += addString(className, message+size+sizeof(int));
    printf("len: %d\n",(int)strlen(query));
    size += addBytes(query, strlen(query), message+size+sizeof(int));
    //size += addString(query, message+size+sizeof(int));
    size += addInt(-1, message+size+sizeof(int));
    size += addInt(0, message+size+sizeof(int));
    size += addInt(0, message+size+sizeof(int));
    size += addInt(size-sizeof(char), message+sizeof(char));
    return size;
}

