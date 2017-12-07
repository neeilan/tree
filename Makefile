FLAGS = -Wall -std=gnu99 -g
DEPENDENCIES = ftree.h

all: tree

tree: tree.o ftree.o
	gcc ${FLAGS} -o $@ $^

%.o: %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean: 
	rm *.o tree
	
