g++ -w -c -g -I./datamodel_lib -I./ -I/usr/local/include -lzmq -L/usr/local/lib main.cpp && g++ -L/usr/local/lib -lzmq -o main main.o reqmsg.o swap_endian.o dmp.o ezxml.o && ./main