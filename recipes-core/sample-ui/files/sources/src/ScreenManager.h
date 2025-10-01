#pragma once

#include "lvgl.h"
#include "IComponent.h"

class ScreenManager {
    private:
        lv_display_t *disp;
        lv_indev_t *touch;
        static constexpr const char* DefaultFbDev = "/dev/fb0";
        static constexpr const char* DefaultTouchDev = "/dev/input/touchscreen0";

    public:
        ScreenManager() : ScreenManager(DefaultFbDev, DefaultTouchDev) {}

        ScreenManager(const char* fbdev, const char* touchdev) : disp(nullptr), touch(nullptr) {
            Initialize(fbdev, touchdev);
        }

        ~ScreenManager() {
            if (disp) {
                lv_display_delete(disp);
            }
            if (touch) {
                lv_indev_delete(touch);
            }
        }

        void Initialize(const char *fbdev, const char *touchdev);

        void Start(IComponent& view);

        lv_display_t* getDisplay() const { return disp; }
        lv_indev_t* getTouch() const { return touch; }
};
