#include "ui.h"

int main() {
    UI::begin();

    UI::Line_reader reader;
    reader.refresh();

    int i;
    while (i = getch()) {
        if(i == KEY_F(1))
            break;
    }

    UI::end();
}