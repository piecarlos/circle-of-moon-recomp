#pragma once
// Multi-touch on-screen gamepad overlay for touchscreen play (disabled by default).

#include <SDL3/SDL.h>

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace circlemoon::input {

class TouchOverlay {
public:
    void reset();

    // Processes SDL_EVENT_FINGER_* events in normalized [0..1] window coordinates.
    // Returns true when the event was handled by the overlay.
    bool handle_event(const SDL_Event& e, float display_w, float display_h,
                      const std::function<void()>& on_menu);

    // Current GBA KEYINPUT bitmask (1 = pressed, matching input::Button bit order).
    std::uint16_t gba_keys() const { return keys_; }

    // Draws the translucent Cyan Glass controls on ImGui's foreground draw list.
    void draw(float display_w, float display_h, float opacity) const;

private:
    struct FingerState {
        float x = 0.0f;
        float y = 0.0f;
    };

    void recompute(float display_w, float display_h);

    std::unordered_map<SDL_FingerID, FingerState> fingers_;
    std::uint16_t keys_ = 0;
};

}  // namespace circlemoon::input
