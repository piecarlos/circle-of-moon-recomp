#include "options/touch_overlay.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>

#include "app/theme.h"
#include "options/input.h"

namespace circlemoon::input {

namespace {

struct Rect {
    ImVec2 min;
    ImVec2 max;

    bool contains(float x, float y, float pad = 0.0f) const {
        return x >= min.x - pad && x <= max.x + pad && y >= min.y - pad && y <= max.y + pad;
    }

    ImVec2 center() const { return ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f); }
};

struct Layout {
    float unit = 1.0f;
    ImVec2 dpad_center;
    float dpad_r = 76.0f;
    ImVec2 btn_a;
    ImVec2 btn_b;
    float btn_r = 34.0f;
    Rect rect_l;
    Rect rect_r;
    Rect rect_select;
    Rect rect_start;
    Rect rect_menu;
};

Layout compute_layout(float w, float h) {
    Layout l;
    const float short_side = std::max(1.0f, std::min(w, h));
    l.unit = std::clamp(short_side / 480.0f, 0.72f, 2.2f);
    const float u = l.unit;

    const float mx = std::max(24.0f * u, w * 0.035f);
    const float my = std::max(22.0f * u, h * 0.045f);

    l.dpad_r = 74.0f * u;
    l.dpad_center = ImVec2(mx + l.dpad_r, h - my - l.dpad_r);

    l.btn_r = 34.0f * u;
    const float right_edge = w - mx;
    l.btn_a = ImVec2(right_edge - l.btn_r - 4.0f * u, h - my - l.btn_r * 2.15f);
    l.btn_b = ImVec2(right_edge - l.btn_r * 3.10f, h - my - l.btn_r * 1.10f);

    const float sh_w = 88.0f * u;
    const float sh_h = 36.0f * u;
    const float sh_y = std::max(14.0f * u, std::min(l.dpad_center.y - l.dpad_r - 24.0f * u - sh_h,
                                                    l.btn_a.y - l.btn_r - 24.0f * u - sh_h));
    l.rect_l = {{mx, sh_y}, {mx + sh_w, sh_y + sh_h}};
    l.rect_r = {{right_edge - sh_w, sh_y}, {right_edge, sh_y + sh_h}};

    const float pill_w = 64.0f * u;
    const float pill_h = 26.0f * u;
    const float gap = 14.0f * u;
    const float mid_x = w * 0.5f;
    const float bot_y = h - std::max(14.0f * u, my * 0.45f) - pill_h;
    l.rect_select = {{mid_x - gap * 0.5f - pill_w, bot_y}, {mid_x - gap * 0.5f, bot_y + pill_h}};
    l.rect_start = {{mid_x + gap * 0.5f, bot_y}, {mid_x + gap * 0.5f + pill_w, bot_y + pill_h}};

    const float menu_w = 72.0f * u;
    const float menu_h = 28.0f * u;
    const float top_y = std::max(10.0f * u, h * 0.02f);
    l.rect_menu = {{mid_x - menu_w * 0.5f, top_y}, {mid_x + menu_w * 0.5f, top_y + menu_h}};

    return l;
}

constexpr std::uint16_t bit(Button b) { return std::uint16_t(1u << static_cast<int>(b)); }

bool in_circle(float x, float y, ImVec2 c, float r) {
    const float dx = x - c.x;
    const float dy = y - c.y;
    return (dx * dx + dy * dy) <= (r * r);
}

void draw_centered_label(ImDrawList* dl, ImFont* font, float font_size, ImVec2 center, ImU32 col, const char* text) {
    const ImVec2 sz = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text);
    dl->AddText(font, font_size, ImVec2(center.x - sz.x * 0.5f, center.y - sz.y * 0.5f), col, text);
}

void draw_pill_button(ImDrawList* dl, const Rect& r, float rounding, bool pressed, float opacity,
                      const theme::Palette& c, ImFont* font, float font_size, const char* label) {
    const ImU32 fill = pressed ? theme::u32(c.accent, std::min(1.0f, opacity * 0.85f))
                               : theme::u32(c.card, opacity * 0.70f);
    const ImU32 border = pressed ? theme::u32(c.highlight, std::min(1.0f, opacity + 0.25f))
                                 : theme::u32(c.accent, opacity * 0.65f);
    const ImU32 ink = pressed ? theme::u32(c.on_accent, std::min(1.0f, opacity + 0.35f))
                              : theme::u32(c.ink, std::min(1.0f, opacity + 0.25f));
    dl->AddRectFilled(r.min, r.max, fill, rounding);
    dl->AddRect(r.min, r.max, border, rounding, 0, pressed ? 2.0f : 1.25f);
    draw_centered_label(dl, font, font_size, r.center(), ink, label);
}

void draw_circle_button(ImDrawList* dl, ImVec2 center, float radius, bool pressed, float opacity,
                        const theme::Palette& c, ImFont* font, float font_size, const char* label) {
    const ImU32 fill = pressed ? theme::u32(c.accent, std::min(1.0f, opacity * 0.90f))
                               : theme::u32(c.card, opacity * 0.72f);
    const ImU32 border = pressed ? theme::u32(c.highlight, std::min(1.0f, opacity + 0.25f))
                                 : theme::u32(c.accent, opacity * 0.70f);
    const ImU32 ink = pressed ? theme::u32(c.on_accent, std::min(1.0f, opacity + 0.35f))
                              : theme::u32(c.ink, std::min(1.0f, opacity + 0.28f));
    dl->AddCircleFilled(center, radius, fill, 36);
    dl->AddCircle(center, radius, border, 36, pressed ? 2.2f : 1.4f);
    draw_centered_label(dl, font, font_size, center, ink, label);
}

}  // namespace

void TouchOverlay::reset() {
    fingers_.clear();
    keys_ = 0;
}

bool TouchOverlay::handle_event(const SDL_Event& e, float display_w, float display_h,
                                const std::function<void()>& on_menu) {
    if (e.type != SDL_EVENT_FINGER_DOWN && e.type != SDL_EVENT_FINGER_MOTION &&
        e.type != SDL_EVENT_FINGER_UP && e.type != SDL_EVENT_FINGER_CANCELED) {
        return false;
    }
    if (display_w <= 0.0f || display_h <= 0.0f) return true;

    const float px = e.tfinger.x * display_w;
    const float py = e.tfinger.y * display_h;
    const Layout l = compute_layout(display_w, display_h);

    if (e.type == SDL_EVENT_FINGER_DOWN) {
        if (l.rect_menu.contains(px, py, 8.0f * l.unit)) {
            reset();
            if (on_menu) on_menu();
            return true;
        }
        fingers_[e.tfinger.fingerID] = {px, py};
    } else if (e.type == SDL_EVENT_FINGER_MOTION) {
        if (fingers_.find(e.tfinger.fingerID) != fingers_.end()) {
            fingers_[e.tfinger.fingerID] = {px, py};
        }
    } else {
        fingers_.erase(e.tfinger.fingerID);
    }

    recompute(display_w, display_h);
    return true;
}

void TouchOverlay::recompute(float display_w, float display_h) {
    keys_ = 0;
    if (fingers_.empty()) return;

    const Layout l = compute_layout(display_w, display_h);
    const float dead = l.dpad_r * 0.18f;
    const float dpad_hit_r = l.dpad_r * 1.25f;
    const float btn_hit_r = l.btn_r * 1.25f;
    const float pad = 10.0f * l.unit;

    for (const auto& [id, f] : fingers_) {
        (void)id;
        if (in_circle(f.x, f.y, l.dpad_center, dpad_hit_r)) {
            const float dx = f.x - l.dpad_center.x;
            const float dy = f.y - l.dpad_center.y;
            const float dist = std::sqrt(dx * dx + dy * dy);
            if (dist >= dead) {
                if (dx > dist * 0.38f) keys_ |= bit(Button::Right);
                if (dx < -dist * 0.38f) keys_ |= bit(Button::Left);
                if (dy < -dist * 0.38f) keys_ |= bit(Button::Up);
                if (dy > dist * 0.38f) keys_ |= bit(Button::Down);
            }
        }
        if (in_circle(f.x, f.y, l.btn_a, btn_hit_r)) keys_ |= bit(Button::A);
        if (in_circle(f.x, f.y, l.btn_b, btn_hit_r)) keys_ |= bit(Button::B);
        if (l.rect_l.contains(f.x, f.y, pad)) keys_ |= bit(Button::L);
        if (l.rect_r.contains(f.x, f.y, pad)) keys_ |= bit(Button::R);
        if (l.rect_select.contains(f.x, f.y, pad * 0.7f)) keys_ |= bit(Button::Select);
        if (l.rect_start.contains(f.x, f.y, pad * 0.7f)) keys_ |= bit(Button::Start);
    }
}

void TouchOverlay::draw(float display_w, float display_h, float opacity) const {
    if (display_w <= 0.0f || display_h <= 0.0f) return;
    opacity = std::clamp(opacity, 0.15f, 1.0f);

    const Layout l = compute_layout(display_w, display_h);
    const auto& c = theme::palette();
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImFont* font = theme::font_button() ? theme::font_button() : ImGui::GetFont();
    const float u = l.unit;

    // D-Pad outer ring + 4 directional segments
    dl->AddCircleFilled(l.dpad_center, l.dpad_r, theme::u32(c.card, opacity * 0.55f), 40);
    dl->AddCircle(l.dpad_center, l.dpad_r, theme::u32(c.accent, opacity * 0.55f), 40, 1.3f);

    const float arm_w = 22.0f * u;
    const float arm_len = l.dpad_r * 0.84f;
    const float inner = 16.0f * u;
    struct DirBox {
        Button b;
        ImVec2 min;
        ImVec2 max;
        const char* glyph;
    };
    const DirBox dirs[] = {
        {Button::Up, {l.dpad_center.x - arm_w, l.dpad_center.y - arm_len}, {l.dpad_center.x + arm_w, l.dpad_center.y - inner}, "^"},
        {Button::Down, {l.dpad_center.x - arm_w, l.dpad_center.y + inner}, {l.dpad_center.x + arm_w, l.dpad_center.y + arm_len}, "v"},
        {Button::Left, {l.dpad_center.x - arm_len, l.dpad_center.y - arm_w}, {l.dpad_center.x - inner, l.dpad_center.y + arm_w}, "<"},
        {Button::Right, {l.dpad_center.x + inner, l.dpad_center.y - arm_w}, {l.dpad_center.x + arm_len, l.dpad_center.y + arm_w}, ">"},
    };
    for (const auto& d : dirs) {
        const bool down = (keys_ & bit(d.b)) != 0;
        const ImU32 fill = down ? theme::u32(c.accent, std::min(1.0f, opacity * 0.88f))
                                : theme::u32(c.raised, opacity * 0.72f);
        const ImU32 border = down ? theme::u32(c.highlight, std::min(1.0f, opacity + 0.25f))
                                  : theme::u32(c.accent, opacity * 0.45f);
        const ImU32 ink = down ? theme::u32(c.on_accent, std::min(1.0f, opacity + 0.35f))
                               : theme::u32(c.ink_muted, std::min(1.0f, opacity + 0.2f));
        dl->AddRectFilled(d.min, d.max, fill, 7.0f * u);
        dl->AddRect(d.min, d.max, border, 7.0f * u, 0, down ? 1.8f : 1.0f);
        draw_centered_label(dl, font, 16.0f * u,
                            ImVec2((d.min.x + d.max.x) * 0.5f, (d.min.y + d.max.y) * 0.5f), ink, d.glyph);
    }

    // Face buttons (A / B)
    draw_circle_button(dl, l.btn_a, l.btn_r, (keys_ & bit(Button::A)) != 0, opacity, c, font, 20.0f * u, "A");
    draw_circle_button(dl, l.btn_b, l.btn_r, (keys_ & bit(Button::B)) != 0, opacity, c, font, 20.0f * u, "B");

    // Shoulder buttons (L / R)
    draw_pill_button(dl, l.rect_l, 12.0f * u, (keys_ & bit(Button::L)) != 0, opacity, c, font, 16.0f * u, "L");
    draw_pill_button(dl, l.rect_r, 12.0f * u, (keys_ & bit(Button::R)) != 0, opacity, c, font, 16.0f * u, "R");

    // Select / Start / Menu
    draw_pill_button(dl, l.rect_select, 12.0f * u, (keys_ & bit(Button::Select)) != 0, opacity, c, font, 11.5f * u, "SELECT");
    draw_pill_button(dl, l.rect_start, 12.0f * u, (keys_ & bit(Button::Start)) != 0, opacity, c, font, 11.5f * u, "START");
    draw_pill_button(dl, l.rect_menu, 12.0f * u, false, opacity * 0.88f, c, font, 11.5f * u, "MENU");
}

}  // namespace circlemoon::input
