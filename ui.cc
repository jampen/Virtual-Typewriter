#include "ui.h"

void UI::begin() {
    initscr();              // Initialize ncurses
    raw();                  // Characters are passed immediately to our our program
    noecho();               // Don't display the inputted characters as they are typed (we handle it)
    keypad(stdscr, TRUE);   // We want to receive function keys like F1 and F2
    refresh();
}

void UI::end() {
    endwin();
}

void UI::Window_deleter::operator()(WINDOW* window) {
    delwin(window);
}

// Impl of Key_event_loop

void UI::Key_event_loop::listen() {
    chtype ch;
    bool quit = false;

    while (!quit) {
        ch = getch();
        if (!ch) {
            break;
        }
        quit = false;

        // Send the messages to each receving object
        for (Key_inputtable* listener : listeners) {
            const auto response = listener->on_key_pressed(ch);

            if (response == Key_input_response::Refused) {
                continue;
            }

            else if (response == Key_input_response::Accepted) {
                break;
            }

            else if (response == Key_input_response::Submit) {
                // TODO: impl submit actions
            } else if (response == Key_input_response::Quit) {
                quit = true;
                break;
            }
        }
        //??? on_finish();
    }  
}


// Impl of line_reader

UI::Line_reader::Line_reader(const int num_columns)
    : window(newwin(LINE_HEIGHT, num_columns, 0, 0))
{
    resize(Size{1, num_columns});
}

std::string UI::Line_reader::read() const {
    return "Hello World";
}

void UI::Line_reader::clear() {
    // stub
    wclear(get_window());
}

// Impl of abstract methods
void UI::Line_reader::resize(const UI::Size new_size) {
    wresize(get_window(), new_size.num_rows, new_size.num_columns);
    wrefresh(get_window());
}

UI::Key_input_response UI::Line_reader::on_key_pressed(const chtype key) {
    if (key == '\n') {
        // submit..
        return Key_input_response::Submit;
    }

    // handle special cases, such as backspace
    bool printable = isprint(key);

    if(printable) {
        // Handle inputtable characters
        waddch(get_window(), key);
    }

    bool handled = false;
    handled |= printable;

    switch (key) {
        // Handle special characters
        case KEY_BACKSPACE:
            waddch(get_window(), '\b');
            handled |= true;
            break;

        case KEY_F(1):
            return Key_input_response::Quit;
    }

    // Only refresh if we made a change
    if (handled) {
        wrefresh(get_window());
        return Key_input_response::Accepted;
    }
    
    // We were unable to handle this key, so move it on to someone else
    return Key_input_response::Refused;
}