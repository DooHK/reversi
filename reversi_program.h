#pragma once

void board_init();
void print_board(WINDOW* win);

char* get_currentPlayer();
void makeMove(int row, int col);
void changePlayer();
int isValidMove(int row, int col);
int isBoardFull() ;
int isValidMoveAvailable();
int count_white();
int count_black();
void print_winner();
void count_stone();

