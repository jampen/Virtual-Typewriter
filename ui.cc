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

// Impl of Paper

UI::Paper::Paper() {
    int max_y;
    int max_x;
    getmaxyx(stdscr, max_y, max_x);
    size.num_columns = max_x;
    size.num_rows = max_y - PAPER_BEGIN_OFFSET - Line_reader::LINE_HEIGHT;
    window = Window(newwin(size.num_rows, size.num_columns, 0, 0));
}


void UI::Paper::on_line_entered(const std::string& line) {
    rows.push_back(line);

    if (row_end < rows.size()) {
        ++row_end;
    }

    const size_t view_row_diff = row_end - row_begin;

    if (view_row_diff == size.num_rows) {
        ++row_begin;
    }

    // Display latest contents    
    draw();
}

void UI::Paper::resize(const Size new_size) {
    size.num_columns = new_size.num_columns;
    size.num_rows = new_size.num_rows - PAPER_BEGIN_OFFSET;
    wresize(get_window(), size.num_rows, size.num_columns);
    wmove(get_window(), 0, 0); // Reset the cursor
    draw(); // Needed to re-adjust the screen
}

void UI::Paper::remove_row(const std::size_t row_index) {
    if (row_index >= rows.size()) return;
    --row_end;
    rows.erase(rows.begin() + row_index);
    draw();
}

void UI::Paper::draw() {
    // draw paper
    wclear(get_window());

    long cursor_begin_row = (getmaxy(get_window()) - rows.size());
    if (cursor_begin_row < 0) cursor_begin_row = 0;

    wmove(get_window(), cursor_begin_row, 0); 
    wrefresh(get_window()); // refresh cursor pos
    
    for (std::size_t row = row_begin; row < row_end; ++row) {
        const auto& rowstr = rows.at(row);
        wprintw(get_window(), "%s\n", rowstr.c_str());
    }

    wrefresh(get_window()); // display to screen
}

std::istream& UI::operator >> (std::istream& is, Paper& paper) {
    for (std::string line; std::getline(is, line, '\n'); ) {
        paper.on_line_entered(line);
    }
    return is;
}

std::ostream& UI::operator << (std::ostream& os, const Paper& paper) {
    const auto lines = paper.get_rows();
    for (const auto& line : lines) {
        os << line << '\n';
    }
    return os;
}


// Impl of Line_reader

UI::Line_reader::Line_reader(Paper* paper, const int num_columns)
: 
    paper(paper),
    window(newwin(LINE_HEIGHT, num_columns, 0, 0))
{
    int maxx;
    int maxy;
    getmaxyx(stdscr, maxy, maxx);
    resize(Size{maxy, maxx});
}

// remove trailing spaces from a string
// trim from right
std::string& rstrip(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string UI::Line_reader::read() {
    std::string line;
    for (int col = 0; col < num_columns; ++col) {
        line.push_back(static_cast<char>(mvwinch(get_window(), 0, col) & A_CHARTEXT));
    }
    return rstrip(line);
}

void UI::Line_reader::clear() {
    // stub
    wclear(get_window());
}

// Impl of abstract methods
void UI::Line_reader::resize(const UI::Size new_size) {
    num_columns = new_size.num_columns;
    wresize(get_window(), 1, new_size.num_columns);
    mvwin(get_window(), new_size.num_rows-LINE_HEIGHT, 0);
    wrefresh(get_window());
    paper->resize(new_size);
}

UI::Key_input_response UI::Line_reader::on_key_pressed(const chtype key) {
    if (key == '\n') {
        // submit..
        paper->on_line_entered(read());
        wclear(get_window()); // Clear contents
        // Move cursor back to the start of the line input
        wmove(get_window(), 0, 0);
        wrefresh(get_window()); // wmove only takes effect upon a refresh
        return Key_input_response::Submit;
    }

    // handle special cases, such as backspace
    const bool printable = isprint(key);

    if(printable) {
        // Handle inputtable characters
        waddch(get_window(), key);
    }

    bool handled = false;
    handled |= printable;

    switch (key) {
        case KEY_RESIZE:
            resize({getmaxy(stdscr), getmaxx(stdscr)});
            return Key_input_response::Resized;

        // Handle special characters
        case KEY_BACKSPACE:
            waddch(get_window(), '\b');
            handled |= true;
            break;
    }

    // Only refresh if we made a change
    if (handled) {
        wrefresh(get_window());
        return Key_input_response::Accepted;
    }
    
    // We were unable to handle this key, so move it on to someone else
    return Key_input_response::Refused;
}

void UI::Line_reader::reset_cursor() {
    wmove(get_window(), 0, 0);
    wrefresh(get_window());
}