// CircleMoon's colors for the Cyan Glass Gothic theme: deep midnight-navy glass surfaces,
// luminous arcane cyan accents, and crisp ice-cyan highlights.
#include "app/theme.h"

namespace circlemoon::theme {

namespace {
ImVec4 rgb(int r, int g, int b, float a = 1.0f) { return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a); }
}  // namespace

const Palette& palette() {
    static const Palette p{
        rgb(6, 16, 27),             // surface (midnight abyss)
        rgb(9, 24, 40, 0.93f),      // card (translucent midnight glass)
        rgb(14, 36, 58, 0.94f),     // raised (navy glass)
        rgb(22, 54, 84),            // high (illuminated slate)
        rgb(56, 189, 248, 0.24f),   // line (cyan glass rim)
        rgb(240, 248, 255),         // ink (ice white)
        rgb(148, 184, 212),         // ink_muted (cool silver-cyan)
        rgb(93, 128, 158),          // ink_faint (muted slate-blue)
        rgb(34, 211, 238),          // accent (arcane cyan)
        rgb(77, 228, 250),          // accent_hover (bright ice cyan)
        rgb(8, 145, 178),           // accent_active (deep teal cyan)
        rgb(34, 211, 238, 0.16f),   // accent_soft (cyan glow)
        rgb(4, 19, 30),             // on_accent (deep midnight text on bright cyan)
        rgb(125, 243, 255),         // highlight (luminous focus ring)
        rgb(52, 211, 153),          // good (emerald verified)
        rgb(248, 113, 113),         // bad (crimson error)
        rgb(14, 165, 233),          // glow (cerulean moonlight)
    };
    return p;
}

}  // namespace circlemoon::theme
