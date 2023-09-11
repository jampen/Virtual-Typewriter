#pragma once
#include "ui.h"

namespace Ctrl {
    class Saver
    : public UI::Key_inputtable
    {
    public:
        static constexpr auto KEY = KEY_F(1);
        Saver(const std::string& filename, UI::Paper* paper);
        UI::Key_input_response on_key_pressed(const chtype key) override;
    private:
        std::string filename;
        UI::Paper* paper; 
    };

    class Clear_line
    : public UI::Key_inputtable
    {
    public:
        static constexpr auto KEY = KEY_F(2);
        Clear_line(UI::Line_reader* reader);
        UI::Key_input_response on_key_pressed(const chtype key) override;
    private:
        UI::Line_reader* reader;        
    };

    class Quitter
    : public UI::Key_inputtable
    {
    public:
        static constexpr auto KEY = KEY_F(9);
        UI::Key_input_response on_key_pressed(const chtype key) override;
    };
}