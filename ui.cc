#include "ui.h"

void UI::begin() {
    initscr();              // Initialize ncurses
    raw();                  // Characters are passed immediately to our our program
    noecho();               // Don't display the inputted characters as they are typed (we handle it)
    keypad(stdscr, TRUE);   // We want to receive function keys like F1 and F2
}

void UI::end() {
    endwin();
}

void UI::Window_deleter::operator()(WINDOW* window) {
    delwin(window);
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
}

// Impl of abstract methods

void UI::Line_reader::draw() {
    wrefresh(get_window());
}

void UI::Line_reader::resize(const UI::Size new_size) {
    wresize(get_window(), new_size.num_rows, new_size.num_columns);
    wrefresh(get_window());
}

bool UI::Line_reader::on_key_pressed(const chtype key) {
    return true;
}