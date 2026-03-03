#pragma once
#include <Arduino.h>
#include <constants.h>

class SwitchButton {
private:
    static void (*callback)(bool active);
    static bool hasBeenPressed;
    static bool active;
    static void buttonOnPress();
    static void buttonOffPress();

public:
    static void setup();
    static bool isPressed();
    /**
     * Registers a callback function to be called when the button is active.
     * This function use an interrupt to detect the button press, so the callback
     * will be called in an interrupt context. Make sure to keep the callback
     * function short and avoid using any blocking functions inside it.
     *
     * Note: This function integrates a debounce mechanism to prevent multiple
     * triggers from a single button press. The debounce time is defined by the
     * `DEBOUNCE_DELAY` constant in constants.h.
     *
     * Warning: Calling this function multiple times will overwrite the previous
     * callback and may lead to unexpected behavior. Make sure to call this
     * function only once per button instance.
     */
    static void onPress(void (*callback)(bool active));
    static void loop();
};