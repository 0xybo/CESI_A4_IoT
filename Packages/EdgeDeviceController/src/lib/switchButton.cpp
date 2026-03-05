#include <lib/switchButton.h>

void (*SwitchButton::callback)(bool active) = nullptr;
bool SwitchButton::hasBeenPressed = false;
bool SwitchButton::active = true;

void SwitchButton::buttonOnPress() {
    if (SwitchButton::active || SwitchButton::hasBeenPressed) return; // Debounce: Ignore if already active
    SwitchButton::active = true;
    SwitchButton::hasBeenPressed = true;
}

void SwitchButton::buttonOffPress() {
    if (!SwitchButton::active || SwitchButton::hasBeenPressed) return; // Debounce: Ignore if already released
    SwitchButton::active = false;
    SwitchButton::hasBeenPressed = true;
}

void SwitchButton::setup() {
    pinMode(BUTTON_OFF_PIN, INPUT_PULLUP);
    pinMode(BUTTON_ON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_OFF_PIN), SwitchButton::buttonOffPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_ON_PIN), SwitchButton::buttonOnPress, FALLING);
}

bool SwitchButton::isPressed() {
    return SwitchButton::active;
}

/**
 * Registers a callback function to be called when the button is active.
 * This function use an interrupt to detect the button press, so the callback
 * will be called in an interrupt context. Make sure to keep the callback
 * function short and avoid using any blocking functions inside it.
 */
void SwitchButton::onPress(void (*callback)(bool active)) {
    SwitchButton::callback = callback;
}

void SwitchButton::loop(int index) {
    if (SwitchButton::hasBeenPressed && SwitchButton::callback != nullptr) {
        SwitchButton::callback(SwitchButton::active);
        SwitchButton::hasBeenPressed = false;
    }
}