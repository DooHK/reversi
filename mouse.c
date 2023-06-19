#include <ncurses.h>
int main() {
    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);

    // 마우스 클릭 이벤트 확인
    MEVENT event;
    while (1) {
        int ch = getch();
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_CLICKED) {
                    clear();  // 화면 지우기
                    mvprintw(0, 0, "mouse click : x=%d, y=%d", event.x, event.y);
                    refresh();  // 변경 사항 표시
                }
            }
        }
    }

    // ncurses 종료
    endwin();

    return 0;
}