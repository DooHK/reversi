#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ncurses.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <linux/socket.h>
#include "reversi_program.h"
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#define SIZE 9
struct buf{
    int x, y;
}msg;
sem_t sem;

int connect_ipaddr_port (const char * ip, int port)
{
	int sock_fd ;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ;
	bzero(&address, sizeof(address)) ;
	address.sin_family = AF_INET ; 
	address.sin_port = htons(port) ; 
	if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	return sock_fd ;
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
            msg.x = inputy;
            msg.y = inputx;
            
            
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
    
    pthread_t tid;
    sem_init(&sem,0,0);
    pthread_create(&tid,NULL,pick_from_mouse,NULL);

    board_init();
    print_board(stdscr);
    while(1){
       
        /*if (isBoardFull()) {
            // 게임 보드가 가득 찬 경우
            mvwprintw(stdscr,15,0,"Board is full game over.");
            msg= -1;
            send(conn_fd, &msgizeof(msg0) ;
            break;
        }
        if (!isValidMoveAvailable()) {
            // 현재 플레이어와 상대방 모두 돌을 놓을 수 없는 경우
            mvwprintw(stdscr,15,0, "there is no place to put the rock");
            msg.x = -1;
            send(conn_fd, &msg ,sizeof(msg),0) ;
            break;
        }*/
        sem_wait(&sem);
        send(conn_fd, (char*)&msg, sizeof(msg), 0) ;
        makeMove(msg.y,msg.x);
        changePlayer();
        print_board(stdscr);
        wrefresh(stdscr);
        int ret = recv(conn_fd,(char*)&msg,sizeof(msg),0);
        

        /*if(msg=-1){
            mvwprintw(stdscr,15,0,"Game end");
            break;
        }*/
        mvwprintw(stdscr,17,0,"msg.x = %d msg.y = %d",msg.x,msg.y);
        wrefresh(stdscr);
        makeMove(msg.y,msg.x);
        changePlayer();
        print_board(stdscr);
        wrefresh(stdscr);

    }
    pthread_join(tid,NULL);
   
    
  

    endwin(); // ncurses 모드 종료
	
}

int main (int argc, char const ** argv)
{ 
	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0}; 
	char * data ;

	if (argc != 3) {
		fprintf(stderr, "Wrong number of arguments\n") ;
		fprintf(stderr, "Usage: ./chat-sendfirst [IP addr] [Port num]\n") ;
		exit(EXIT_FAILURE) ;
	}

	int conn_fd = connect_ipaddr_port(argv[1], atoi(argv[2])) ;
	
	chat(conn_fd) ;

	shutdown(conn_fd, SHUT_RDWR) ;

	return EXIT_SUCCESS ;
} 