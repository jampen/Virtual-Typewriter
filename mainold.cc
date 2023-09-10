#include <ncurses.h>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <regex>

static void debug_key_input() {
    // EVENT LOOP:
    for(int ch; ch=getch();) {
        if (ch == KEY_F(1)) break;
        if (ch == '\n') break;
        printw("%c=%d\n",ch,ch);
        refresh();
    }
}


// remove trailing spaces from a string
// trim from right
std::string& rstrip(std::string& s, const char* t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

class Typewriter {
public:
    Typewriter() {
        const int num_rows = getmaxy(stdscr);
        num_cols = getmaxx(stdscr);
        linefeed = Linefeed(newwin(LINEFEED_HEIGHT, num_cols, 0, 0));
        paper = Paper(newwin(num_rows-PAPER_HEIGHT_OFFSET, num_cols, 0, 0));
        on_terminal_resize();
    }

    void reset_cursor() {
        wmove(linefeed.get(), 0, 0);
    }

    void submit_line() {
        std::string line;
        for (int col = 0; col < num_cols; ++col) {
            line.push_back(
                static_cast<char>(mvwinch(linefeed.get(), 0, col) & A_CHARTEXT)
            );
        }

        rstrip(line);

        lines.push_back(line);
        reset_cursor();
        wclear(linefeed.get());
        draw_paper();

        // TODO: use views to provide a sliding-up view of the page
        
    }

    bool submit_char(const int ch) {
        if (iscntrl(ch)) return false;
        if(!isprint(ch)) return false;
        waddch(linefeed.get(), ch);
        return true;
    }

    void backspace() {
        waddch(linefeed.get(), '\b');
    }

    void deletekey() {
        wdelch(linefeed.get());
    }

    void draw_linefeed() {
        wrefresh(linefeed.get());
    }

    void draw_paper() {
        const int insert_y = getmaxy(stdscr) - LINEFEED_HEIGHT - lines.size() - PAPER_HEIGHT_OFFSET;
        wclear(paper.get());
        wmove(paper.get(),insert_y, 0);


        wprintw(paper.get(), "[lines %d to %d]\n", view_line_begin, view_line_end);

        for (int line_index = view_line_begin; line_index != view_line_end; ++line_index) {
            const std::string& line = lines[line_index];
            wprintw(paper.get(), "%d -- %s\n", line_index, line.c_str());
        }
        wrefresh(paper.get());
    }

    void on_terminal_resize() {
        num_cols = getmaxx(stdscr);
        int num_rows = getmaxy(stdscr);
        int linefeed_ypos = num_rows - 1;
        wresize(paper.get(), num_rows-1, num_cols);
        mvwin(linefeed.get(), linefeed_ypos, 0);
        wrefresh(linefeed.get());
        wrefresh(paper.get());
    }

    void write_to_disk(const std::string& filename) {
        std::ofstream file (filename);
        for (const auto& line : lines) {
            file << line << '\n';
        }
    }

private:
    struct WindowDeleter {
        void operator()(WINDOW* window) {
            delwin(window);
        }
    };

    using Linefeed = std::unique_ptr<WINDOW, WindowDeleter>;
    using Paper = std::unique_ptr<WINDOW, WindowDeleter>;

    std::vector<std::string> lines;
    int num_cols;
    Linefeed linefeed;
    Paper paper;

    size_t view_line_begin{};
    size_t view_line_end{};

     // Constants for window sizes and positions
    static constexpr int LINEFEED_HEIGHT = 1;
    static constexpr int PAPER_HEIGHT_OFFSET = 1;
};


int main() {
    // SET UP CURSES
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    refresh();

    Typewriter typewriter;
    typewriter.draw_paper();

    for (int ch; ch=getch();) {
        if (ch == KEY_F(1)) break;
        
        if (ch == KEY_RESIZE) {
            typewriter.on_terminal_resize();
        }

        switch (ch) {
            case '\n': typewriter.submit_line(); break;
            case KEY_BACKSPACE: typewriter.backspace(); break;
            default: typewriter.submit_char(ch);
        }

        typewriter.draw_linefeed();
    }


    typewriter.write_to_disk("output.txt");

    // EXIT CURSES
    endwin();
    return 0;
}