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

    class Quitter
    : public UI::Key_inputtable
    {
    public:
        static constexpr auto KEY = KEY_F(2);
        UI::Key_input_response on_key_pressed(const chtype key) override;
    };
}