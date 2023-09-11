#include "control.h"
#include <fstream>

// Impl for Saver

Ctrl::Saver::Saver(const std::string& filename, UI::Paper* paper)
:   filename(filename), paper(paper) {
    if (paper == nullptr) throw std::invalid_argument("paper is nullptr");
}

UI::Key_input_response Ctrl::Saver::on_key_pressed(const chtype key) {
    if (key == KEY) {
        // save:
        std::ofstream file (filename);
        file << *paper;
        return UI::Key_input_response::Accepted;
    } else {
        return UI::Key_input_response::Refused;
    }
}

// Impl for Clear_line

Ctrl::Clear_line::Clear_line(UI::Line_reader* reader)
:    reader(reader)
{
    if (reader == nullptr) throw std::invalid_argument("reader is nullptr");
}

UI::Key_input_response Ctrl::Clear_line::on_key_pressed(const chtype key) {
    if (key == KEY) {
        reader->clear();
        reader->reset_cursor();
        return UI::Key_input_response::Accepted;
    } else {
        return UI::Key_input_response::Refused;
    }
}


// Impl for Quitter

UI::Key_input_response Ctrl::Quitter::on_key_pressed(const chtype key) {
    if (key == KEY) {
        // todo: special behavior..
        return UI::Key_input_response::Quit;
    } else {
        return UI::Key_input_response::Refused;
    }
}
