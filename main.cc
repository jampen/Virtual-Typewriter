#include "ui.h"

int main() {
    UI::begin();
    UI::Line_reader reader;

    UI::Key_event_loop loop;
    loop.add_listener(&reader);
    loop.listen();

    UI::end();
}