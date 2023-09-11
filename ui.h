#pragma once
#include <memory>
#include <set>
#include <vector>
#include <istream> // for input streams
#include <curses.h>

namespace UI {
    // These functions must be called at the beginning & end of the program
    // if not, the program will not work.
    void begin(); // Begin curses mode
    void end();   // End curses mode

    struct Window_deleter {
        void operator()(WINDOW* window);
    };

    using Window = std::unique_ptr<WINDOW, Window_deleter>; 

    struct Size {
        int num_rows;
        int num_columns;
    };

    class Resizeable {
    public:
        virtual void resize(const Size new_size) = 0;
    };

    enum class Key_input_response {
        Accepted, // The key was accepted and made a change
        Submit,   // The key caused a submit action.
        Refused,  // The key was refused, and someone else should handle it
        Quit,     // The user requests that the program should quit because of this key 
        Resized,  // The key caused a resize of the user
    };

    class Key_inputtable {
    public:
        // This function is called when a key is pressed and the subclass is targeted
        virtual Key_input_response on_key_pressed(const chtype input) = 0;
    };

    class Key_event_loop {
    public:
        // This function blocks, and sends messages to the Key_inputtables on a key pressed.
        void listen();

        inline void add_listener(Key_inputtable* inputtable) { 
            listeners.emplace(inputtable);
        }
    
        inline void remove_listener(Key_inputtable* inputtable) {
            // find and erase the listener
            auto iter = listeners.find(inputtable);
            if (iter != listeners.end()) {
                listeners.erase(iter);
            }
        }

        inline void clear_listeners() {
            listeners.clear();
        }
    private:
        std::set<Key_inputtable*> listeners;
    };

    class Line_reader;

    class Paper : public Resizeable {
    public:
        static constexpr int PAPER_BEGIN_OFFSET = 1;
        Paper();
        void on_line_entered(const std::string& line);
        void resize(const Size new_size) override;
        inline WINDOW* get_window() { return window.get(); }
        inline const std::vector<std::string>& get_rows() const { return rows;}
        void remove_row(const std::size_t row_index);
    private:
        void draw();

        Window window;
        std::vector<std::string> rows;
        Size size;
        size_t row_begin{};
        size_t row_end{};
    };

    // Stream into the paper, for reading lines.
    std::istream& operator >> (std::istream& is, Paper& paper);
    // Stream into the file, for writing lines
    std::ostream& operator << (std::ostream& os, const Paper& paper);

    class Line_reader : 
        public Resizeable,
        public Key_inputtable {
    public:
        static constexpr int LINE_HEIGHT = 1;
        Line_reader(Paper* paper, const int num_columns = 80);
        inline WINDOW* get_window() { return window.get(); } 
        std::string read();         // Returns the line as a string, without any colors.
        void clear();               // Clear the line reader to blank
        void resize(const Size new_size) override;
        Key_input_response on_key_pressed(const chtype key) override;
        void reset_cursor();
    private:

        Window window;
        int num_columns;
        Paper* paper;
    };
}

