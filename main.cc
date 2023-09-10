#include "ui.h"

int main() {
    UI::begin();
    UI::Line_reader reader;

    //UI::KeyEventLoop ev;
    //ev.add_targets();
    
    int i;
    while (i = getch()) {
        if(i == KEY_F(1))
            break;
    }

    UI::end();
}