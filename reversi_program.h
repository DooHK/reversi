#pragma once

void board_init();
void print_board(WINDOW* win);


void makeMove(int row, int col);
void changePlayer();
int isValidMove(int row, int col);
int isBoardFull() ;
int isValidMoveAvailable();




