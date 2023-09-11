#include "ui.h"
#include "control.h"
#include <fstream>

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        puts("Missing a required argument: filepath");
        puts("Quitting");
        return 1;
    }
     
    const std::string path = argv[1];

    UI::begin();
    UI::Paper paper;

    { // If the user supplies a path to a file that already exists,
      // then open it.
        std::ifstream file (path);
        file >> paper; // Reads the file into paper, does nothing if the file is not open
    }

    UI::Line_reader reader(&paper);
    Ctrl::Quitter quitter;
    Ctrl::Saver saver(path, &paper);
    reader.reset_cursor();

    UI::Key_event_loop loop;
    loop.add_listener(&reader);
    loop.add_listener(&saver);
    loop.add_listener(&quitter);
    loop.listen();

    UI::end();
}