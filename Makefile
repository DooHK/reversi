all: reversi_cilent reversi_server

reversi_cilent: reversi_program.o reversi_cilent.o
	gcc -o reversi_cilent reversi_program.o reversi_cilent.o -lncurses

reversi_server: reversi_server.o reversi_program.o
	gcc -o reversi_server reversi_program.o reversi_server.o -lncurses

reversi_program: reversi_program.o
	gcc -o reversi_program reversi_program.o -lncurses

reversi_cilent.o: reversi_cilent.c
	gcc -c -o reversi_cilent.o reversi_cilent.c -lncurses

reversi_server.o: reversi_server.c
	gcc -c -o reversi_server.o reversi_server.c -lncurses

reversi_program.o : reversi_program.c
	gcc -c -o reversi_program.o reversi_program.c -lncurses

clean:
	rm -rf reversi reversi_program.o reversi_cilent reversi_server reversi_server.o reversi_cilent.o
