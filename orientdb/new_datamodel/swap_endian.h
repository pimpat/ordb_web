#ifdef __cplusplus
extern "C" {
#endif

enum { CHAR, 
    SHORT, 
    INT, 
    LONG, 
};

void swapEndian(void *num, int type);

#ifdef __cplusplus
}
#endif
