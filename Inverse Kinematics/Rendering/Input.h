//
// Created by Jake Meegan on 03/03/2020.
//

#ifndef NATURAL_PHENOMENA_INPUT_H
#define NATURAL_PHENOMENA_INPUT_H

#include <QFlags>
#include <Qt>

struct Mouse {

};

struct Keyboard {
    bool KEY_W = false;
    bool KEY_A = false;
    bool KEY_S = false;
    bool KEY_D = false;
};

struct Input {
    Mouse mouse;
    Keyboard keyboard;
};

#endif //NATURAL_PHENOMENA_INPUT_H
