#pragma once
#include <memory>
#include <set>
#include <curses.h>

namespace UI {
    void begin();
    void end();

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

    class Key_inputtable {
    public:
        // This function is called when a key is pressed and the subclass is targeted
        virtual bool on_key_pressed(const chtype input) = 0;
    };

    class Key_event_loop {
    public:
        // This function blocks, and sends messages to the Key_inputtables on a key pressed
        // if the Key_inputtable returns TRUE then the key is considered to be 'consumed'
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

    class Line_reader : 
        public Resizeable,
        public Key_inputtable {
    public:
        static constexpr int LINE_HEIGHT = 1;

        Line_reader(const int num_columns = 80);
        inline WINDOW* get_window() { return window.get(); } 
        std::string read() const;   // Returns the line as a string, without any colors.
        void clear();               // Clear the line reader to blank
        void resize(const Size new_size) override;
        bool on_key_pressed(const chtype key) override;
    private:
        Window window;
        int num_columns;
    };
}

