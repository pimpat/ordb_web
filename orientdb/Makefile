MAKE=make
CC=cc

#################### OUTPUT ####################
OUTPUT_O=orient-binary.o
################################################

#################### SOURCE ####################
SRC_C=main.c reqmsg.c swap_endian.c
SRC_H=reqmsg.h swap_endian.h
################################################

all: $(OUTPUT_O)

$(OUTPUT_O): $(SRC_C) $(SRC_H)
	$(CC) -o $(OUTPUT_O) $(SRC_C)

clean:
	rm -rf $(OUTPUT_O)

