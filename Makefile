EXEC = doom.out

CC = gcc
CFLAGS = -I${PONG_INCLUDES} -lSDL2

PONG_INCLUDES = src/include/
SRCS = ${wildcard ./src/*.c}
OBJS = ${SRCS:.c=.o} 

all: ${OBJS}
	${CC} ${OBJS} ${CFLAGS} -o ${EXEC}

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${EXEC}

re: fclean all
