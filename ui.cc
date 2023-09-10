#include "ui.h"

void UI::setup_curses() {
    initscr();              // Initialize ncurses
    raw();                  // Characters are passed immediately to our our program
    noecho();               // Don't display the inputted characters as they are typed (we handle it)
    keypad(stdscr, TRUE);   // We want to receive function keys like F1 and F2
}

void UI::Window_deleter(WINDOW* window) {
    delwin(window);
}


void UI::Line_reader::Line_reader(const int num_columns) {
    resize(Size{1, num_columns});
} 