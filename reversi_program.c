#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reversi_program.h"
#include "reversi_data.h"

#define rows 9
#define cols 9

struct buf{
    int x, y;
};



void board_init(){

    //행렬 배열 선언
    char temp[2];
    board[0][0]="";
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            board[i][j]= (char*)malloc(sizeof(char)*2);
        }
    }
    for(int i=1; i<rows; i++){
        for(int j=1; j<cols; j++){
            
            board[i][j] = ".";
        }
    }
    for (int i=1; i<9; i++){
        sprintf(temp,"%d",i);
        
        strcpy(board[0][i],temp);
        
        strcpy(board[i][0],temp);
 
    }    
    
    board[4][4]="B";
    board[4][5]="W";
    board[5][4]="W";
    board[5][5]="B";


    

}
void print_board(WINDOW* win){
    // 윈도우에 원하는 형태 출력
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mvwprintw(win,i, (j)*2, "%s", board[i][j]);
            wrefresh(win);
        }
    }
    if(strcmp(currentPlayer,"B")==0)
        mvwprintw(win,13,0,"black turn");
    else
        mvwprintw(win,13,0,"white turn");

}


int isBoardFull() {
    int i, j;
    
    for (i = 1; i <= SIZE - 1; i++) {
        for (j = 1; j <= SIZE - 1; j++) {
            if (strcmp(board[i][j], "-") == 0) {
                // 보드의 빈 공간이 존재하면 가득 차지 않은 것으로 판단
                return 0;
            }
        }
    }
    
    // 보드의 모든 위치가 돌로 채워진 경우
    return 1;
}
int isValidMoveAvailable() {
    int i, j;

    // 보드를 순회하며 각 위치마다 유효한 돌을 놓을 수 있는지 확인
    for (i = 1; i <= SIZE - 1; i++) {
        for (j = 1; j <= SIZE - 1; j++) {
            // 해당 위치에 돌을 놓을 수 있는지 검사
            if (strcmp(board[i][j], "-") == 0 && isValidMove(i, j)) {
                return 1; // 유효한 돌을 놓을 수 있는 위치가 존재
            }
        }
    }

    return 0; // 유효한 돌을 놓을 수 있는 위치가 없음
}
void makeMove(int row, int col) {
    int dr, dc;
    board[row][col]= (char*)malloc(sizeof(char)*2);
    strcpy(board[row][col],currentPlayer);
    mvwprintw(stdscr,15,0,"%s",currentPlayer);
    wrefresh(stdscr);
    // 인접한 8개의 방향에 대해 확인
    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            // 자기 자신 방향으로는 확인하지 않음
            if (dr == 0 && dc == 0) {
                continue;
            }
            
            int r = row + dr;
            int c = col + dc;
            int count = 0;
            int isValidDirection = 0;
            
            // 현재 플레이어와 다른 돌이 나올 때까지 확인
            while (r > 0 && r < SIZE && c > 0 && c < SIZE && (strcmp(board[r][c],".")!=0) && (strcmp(board[r][c] ,currentPlayer)!=0)) {
                r += dr;
                c += dc;
                count++;
            }
            
            // 다른 돌을 만난 경우
            if (r > 0 && r < SIZE && c > 0 && c < SIZE && (strcmp(board[r][c],currentPlayer)==0) && count > 0) {
                isValidDirection = 1;
            }
            
            // 돌을 뒤집음
            if (isValidDirection) {
                r = row + dr;
                c = col + dc;
                while (strcmp(board[r][c], currentPlayer)!=0) {
                    board[r][c] = (char*)malloc(sizeof(char)*2);
                    strcpy(board[r][c],currentPlayer);
                    r += dr;
                    c += dc;
                }
            }
        }
    }
}
void changePlayer() {
    if (strcmp(currentPlayer,"B")==0) {
        strcpy(currentPlayer,"W");
    } else {
        strcpy(currentPlayer,"B");

    }
}
int isValidMove(int row, int col) {
    if (strcmp(board[row][col],".")!=0) {
        return 0; // 이미 돌이 있는 위치는 유효하지 않음
    }
    
    int dr, dc;
    int oppositeCount = 0;
    
    // 인접한 8개의 방향에 대해 확인
    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            // 자기 자신 방향으로는 확인하지 않음
            if (dr == 0 && dc == 0) {
                continue;
            }
            
            int r = row + dr;
            int c = col + dc;
            int count = 0;
            
            // 현재 플레이어와 다른 돌이 나올 때까지 확인
            while (r > 0 && r < SIZE && c > 0 && c < SIZE && (strcmp(board[r][c],".")!=0) && (strcmp(board[r][c] ,currentPlayer)!=0)) {
                r += dr;
                c += dc;
                count++;
            }
            
            // 다른 돌을 만난 경우
            if (r > 0 && r < SIZE && c > 0 && c < SIZE && (strcmp(board[r][c],currentPlayer)==0) && count > 0) {
                oppositeCount += count;
            }
        }
    }
    
    return oppositeCount > 0;
}

/*int main() {
    initscr();  // ncurses 모드 초기화
    cbreak();   // 입력 버퍼를 사용하지 않음 (즉시 입력)
    echo();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    struct buf msg;

    
    board_init();
    print_board(stdscr);
    while(1){
        msg = pick_from_mouse();
        makeMove(msg.y,msg.x);
        changePlayer();
        print_board(stdscr);
        wrefresh(stdscr);
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            free(board[i][j]);
        }
    }

    endwin(); // ncurses 모드 종료

    return 0;
}*/