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

    // Controls
    Ctrl::Quitter quitter;
    Ctrl::Clear_line line_clearer(&reader);
    Ctrl::Line_remover line_remover(&paper);
    Ctrl::Saver saver(path, &paper);

    reader.reset_cursor();

    UI::Key_event_loop loop;
    loop.add_listener(&reader);
    loop.add_listener(&saver);
    loop.add_listener(&line_clearer);
    loop.add_listener(&line_remover);
    loop.add_listener(&quitter);
    loop.listen();

    UI::end();
}