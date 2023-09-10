#include "ui.h"

int main() {
    UI::begin();

    UI::Paper paper;
    UI::Line_reader reader(&paper);

    UI::Key_event_loop loop;
    loop.add_listener(&reader);
    loop.listen();

    UI::end();
}