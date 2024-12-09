#include "modes/Rivals2.hpp"

#define ANALOG_STICK_MIN 0 
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 255 

bool input_persist; //for angled tilts
int timer = 0; //for angled tilts

Rivals2::Rivals2(socd::SocdType socd_type) {
    _socd_pair_count = 5;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{&InputState::left,    &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up2,     socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };
}

void Rivals2::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.a;
    outputs.b = inputs.b;
    outputs.x = inputs.x;
    outputs.y = inputs.y;
    outputs.buttonR = inputs.z;
    if (inputs.nunchuk_connected) {
        // Lightshield with C button.
        if (inputs.nunchuk_c) {
            outputs.triggerLAnalog = 49;
        }
        outputs.triggerLDigital = inputs.nunchuk_z;
    } else {
        outputs.triggerLDigital = inputs.l;
    }
    outputs.triggerRDigital = inputs.r;
    outputs.start = inputs.start;
    outputs.select = inputs.select;
    outputs.home = inputs.home;
    outputs.leftStickClick = inputs.lightshield;
    outputs.buttonL = inputs.midshield;

    // Activate D-Pad layer by holding Mod X + Mod Y.
    if (inputs.mod_x && inputs.mod_y) {
        outputs.dpadUp = inputs.c_up;
        outputs.dpadDown = inputs.c_down;
        outputs.dpadLeft = inputs.c_left;
        outputs.dpadRight = inputs.c_right;
    }
}

void Rivals2::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.left,
        inputs.right,
        inputs.down,
        inputs.up || inputs.up2,
        inputs.c_left,
        inputs.c_right,
        inputs.c_down,
        inputs.c_up,
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

   bool shield_button_pressed = inputs.r || inputs.l;


    if (directions.diagonal && !shield_button_pressed) { //added this conditional to give joystick accurate diagonals rather than (+/- 1.2, 1.2) should be (0.87~, 0.87~)
        outputs.leftStickX = 128 + (directions.x * 92); // 92 (0.78 in-game), reduced below 0.8 to allow crouch tilts/crouch turn-around tilts
        outputs.leftStickY = 128 + (directions.y * 96);//Y value 0.83. >0.8 allows fast fall
    }

    if (directions.diagonal && shield_button_pressed) {
        outputs.leftStickX = 128 + (directions.x * 92); // (0.77~, 0.77~) to prevent spot dodging when pressing diagonal on the ground
        outputs.leftStickY = 128 + (directions.y * 92);  
    }

    // MX Angled Tilts
    //(x, y), (69, 53), (~0.506, ~0.31) [coords, code_values, in-game values]
    if (input_persist) { //input_persist becomes true if ModX + diagonal + A
        timer++;
        outputs.leftStickX = 128 + (directions.x * 69);
        outputs.leftStickY = 128 + (directions.y * 53);
    }

    if (timer == 150) { // 150 has a 90% success rate on pico
        timer = 0;
        input_persist = false;
    }


    if (inputs.mod_x) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 76); //76 gives 0.58~ in-game for a medium speed walk. will also do tilts 
        }
        
        if(directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 53); // 48 (0.31~ in-game), 0.3 allows tilts and shield drop
        }

        if (directions.diagonal && shield_button_pressed) { //for max-length diagonal wavedash while holding ModX
            outputs.leftStickX = 128 + (directions.x * 76);
            outputs.leftStickY = 128 + (directions.y * 42); 
        }

        if (directions.diagonal && !shield_button_pressed) {
            /* 100% Magnitude UpB when holding B */
            if (inputs.b && !inputs.z) {
                // (x, y), (123, 51), (1.14~, 0.29~) [coords, code_values, in-game values]
                outputs.leftStickX = 128 + (directions.x * 123);
                outputs.leftStickY = 128 + (directions.y * 51);
                // (x, y), (120, 61), (1.1~, 0.41~) [coords, code_values, in-game values]
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 120);
                    outputs.leftStickY = 128 + (directions.y * 61);
                }
                // (x, y), (115, 69), (1.04~, 0.51~) [coords, code_values, in-game values]
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 115);
                    outputs.leftStickY = 128 + (directions.y * 69);
                }
                // (x, y), (110, 78), (0.98~, 0.61~) [coords, code_values, in-game values]
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 110);
                    outputs.leftStickY = 128 + (directions.y * 78);
                }
                // (x, y), (103, 87), (0.9~, 0.71~) [coords, code_values, in-game values]
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 103);
                    outputs.leftStickY = 128 + (directions.y * 87);
                }
            }
            /* 60% Magnitude UpB when not holding B nor Z*/
            if (!inputs.z && !inputs.b && !input_persist) {
                // (x, y), (68, 42), (~0.49, ~0.188) [coords, code_values, in-game values]
                outputs.leftStickX = 128 + (directions.x * 68);
                outputs.leftStickY = 128 + (directions.y * 42);
                // (x, y), (71, 47), (~0.52, ~0.24) [coords, code_values, in-game values]
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 71);
                    outputs.leftStickY = 128 + (directions.y * 47);
                }
                // (x, y), (71, 51), (~0.52, 0.29~) [coords, code_values, in-game values]
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 71);
                    outputs.leftStickY = 128 + (directions.y * 51);
                }
                // (x, y), (69, 55), (~0.51, ~0.34) [coords, code_values, in-game values]
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 69);
                    outputs.leftStickY = 128 + (directions.y * 55);
                }
                // (x, y), (64, 60), (, ~0.38) [coords, code_values, in-game values]
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 64);
                    outputs.leftStickY = 128 + (directions.y * 60);
                }
            }
            /* Shortest UpB when holding Z*/
            if (inputs.z) { 
                // (x, y), (53, 68), (~0.31, ~0.188) [coords, code_values, in-game values] 
                outputs.leftStickX = 128 + (directions.x * 53);
                outputs.leftStickY = 128 + (directions.y * 42);
            }
            /*ModX Angled Tilts*/
            if (inputs.a) {
                input_persist = true;
                timer = 0;
                outputs.leftStickX = 128 + (directions.x * 69);
                outputs.leftStickY = 128 + (directions.y * 53);
            }
        }
    }

    if (inputs.mod_y) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 53); //53 equates to 0.318~ in-game. 0.3 is min to achieve a walk
        }

        if(directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 90); // 0.75~ in-game. will shield drop and tap jump; will not fast fall
        }

        if (directions.diagonal && !shield_button_pressed) {
            /* 100% Magnitude UpB when holding B*/
            if (inputs.b && !inputs.z) {
                // (x, y), (51, 123), (~0.29, ~1.14) [coords, code_values, in-game values]
                outputs.leftStickX = 128 + (directions.x * 51); 
                outputs.leftStickY = 128 + (directions.y * 123);
                // (x, y), (61, 120), (~0.41, ~1.1) [coords, code_values, in-game values]
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 61);
                    outputs.leftStickY = 128 + (directions.y * 120);
                }
                // (x, y), (69, 115), (~0.51, 1.04~) [coords, code_values, in-game values]
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 69);
                    outputs.leftStickY = 128 + (directions.y * 115);
                }
                // (x, y), (78, 110), (~0.61, 0.98~) [coords, code_values, in-game values]
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 78);
                    outputs.leftStickY = 128 + (directions.y * 110);
                }
                // (x, y), (87, 103), (~0.71, 0.9~) [coords, code_values, in-game values]
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 87);
                    outputs.leftStickY = 128 + (directions.y * 103);
                }
            }
            /* 60% Magnitude UpB when not holding B nor Z*/
            if (!inputs.z && !inputs.b) {
                // (x, y), (42, 68), (~0.188, ~0.49) [coords, code_values, in-game values] 
                outputs.leftStickX = 128 + (directions.x * 42);
                outputs.leftStickY = 128 + (directions.y * 68);
                // (x, y), (47, 71), (~0.24, ~0.52) [coords, code_values, in-game values]
                if (inputs.c_down) {
                    outputs.leftStickX = 128 + (directions.x * 47);
                    outputs.leftStickY = 128 + (directions.y * 71);
                }
                // (x, y), (51, 71), (~0.29, ~0.52) [coords, code_values, in-game values]
                if (inputs.c_left) {
                    outputs.leftStickX = 128 + (directions.x * 51);
                    outputs.leftStickY = 128 + (directions.y * 71);
                }
                // (x, y), (55, 69), (~0.34, ~0.51) [coords, code_values, in-game values]
                if (inputs.c_up) {
                    outputs.leftStickX = 128 + (directions.x * 55);
                    outputs.leftStickY = 128 + (directions.y * 69);
                }
                // (x, y), (60, 64), (~0.38, ~0.) [coords, code_values, in-game values]
                if (inputs.c_right) {
                    outputs.leftStickX = 128 + (directions.x * 60);
                    outputs.leftStickY = 128 + (directions.y * 64);
                }
            }
            /* Shortest UpB when holding Z*/
            if (inputs.z) {
                // (x, y), (42, 53), (~0.188, ~0.31) [coords, code_values, in-game values] 
                outputs.leftStickX = 128 + (directions.x * 42);
                outputs.leftStickY = 128 + (directions.y * 53);
            }
        }
    }

    // Shut off C-stick when using D-Pad layer.
    if (inputs.mod_x && inputs.mod_y) {
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}
