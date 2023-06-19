#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <ncurses.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <linux/socket.h>
#include "reversi_program.h"
#include <pthread.h>
#include <semaphore.h>
#define SIZE 9
struct buf{
    int x, y;
}msg[2];
sem_t sem;
int listen_at_port (int portnum) 
{
	int sock_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (sock_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ; 
	bzero(&address, sizeof(address)) ; 	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* localhost */ ; 
	address.sin_port = htons(portnum); 

	if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed: "); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(sock_fd, 16 /* the size of waiting queue*/) < 0) { 
		perror("listen failed : "); 
		exit(EXIT_FAILURE); 
	} 

	int addrlen = sizeof(address); 
	int conn_fd = accept(sock_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
	if (conn_fd < 0) {
		perror("accept failed: "); 
		exit(EXIT_FAILURE); 
	}
	return conn_fd ;
}
void* pick_from_mouse(){
    
    int inputx,inputy;
    int x=10, y=10;
    
    int canput;
    MEVENT event;
    while (1) {
        int press = 0;
        int ch = getch();
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_CLICKED) {
                    press=1;
                }
            }
        }
        if(press){
            inputy= event.x/2;
            inputx = event.y;
            msg[0].x = inputy;
            msg[0].y = inputx;
            
            
            mvwprintw(stdscr,11,0,"x = %d y = %d",inputx,inputy);
            canput = isValidMove(inputx,inputy);
            if(canput == 0){//돌을 못놓는곳
                mvwprintw(stdscr,12,0,"can't put there");
                refresh();
                continue;
            }
            else{//놓을 수 있는 곳
                mvwprintw(stdscr,12,0,"can put there");
                wrefresh(stdscr);
                sem_post(&sem);
                //makeMove(inputx,inputy);
                //changePlayer();
                //print_board(stdscr);
                //wrefresh(stdscr);
            }
            /*if (isBoardFull()) {
                // 게임 보드가 가득 찬 경우
                mvwprintw(stdscr,15,0,"Board is full game over.");
                break;
            }
            
            if (!isValidMoveAvailable()) {
                // 현재 플레이어와 상대방 모두 돌을 놓을 수 없는 경우
                mvwprintw(stdscr,15,0, "there is no place to put the rock");
                break;
            }*/
        }
    }
}
void chat (int conn_fd) 
{
	initscr();  // ncurses 모드 초기화
    cbreak();   // 입력 버퍼를 사용하지 않음 (즉시 입력)
    echo();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    
    memset(&msg[0], 0, sizeof(struct buf));
    memset(&msg[1], 0, sizeof(struct buf));
    pthread_t tid;
    pthread_create(&tid,NULL,pick_from_mouse,NULL);
    sem_init(&sem,0,0);

    board_init();
    print_board(stdscr);
    while(1){
        recv(conn_fd,(char*)&msg[1],sizeof(msg[1]),0);
        /*if(msg.x==-1){
            mvwprintw(stdscr,15,0,"Game end");
            break;
        }*/
        makeMove(msg[1].y,msg[1].x);
        changePlayer();
        print_board(stdscr);
        wrefresh(stdscr);

        
        /*if (isBoardFull()) {
            // 게임 보드가 가득 찬 경우
            mvwprintw(stdscr,15,0,"Board is full game over.");
            send(conn_fd, "quit", sizeof(msg), 0) ;
            break;
        }
        if (!isValidMoveAvailable()) {
            // 현재 플레이어와 상대방 모두 돌을 놓을 수 없는 경우
            mvwprintw(stdscr,15,0, "there is no place to put the rock");
            send(conn_fd, "quit", sizeof(msg), 0) ;
            break;
        }*/
        sem_wait(&sem);
        send(conn_fd, (char*)&msg[0], sizeof(msg[0]), 0) ;
        makeMove(msg[0].y,msg[0].x);
        changePlayer();
        print_board(stdscr);
        wrefresh(stdscr);
        
    }
    
    pthread_join(tidm,NULL);
    

    endwin(); // ncurses 모드 종료
}

int main (int argc, char const **argv) 
{	
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments") ;
		exit(EXIT_FAILURE) ;
	}

	int conn_fd = listen_at_port(atoi(argv[1])) ;

	chat(conn_fd) ;

	shutdown(conn_fd, SHUT_RDWR) ;
} 
