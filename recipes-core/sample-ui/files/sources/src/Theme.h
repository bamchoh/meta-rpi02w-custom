#pragma once

#include "lvgl.h"

class Theme {
public:
    static void Init();
    static lv_style_t TextMuted;
    static lv_style_t Title;
    static lv_style_t Icon;
    static lv_style_t Bullet;
    static lv_style_t ParentFlexObj;
    static lv_style_t TextBlock;
};