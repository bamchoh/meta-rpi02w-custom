#include <stdexcept>

#include "ScreenManager.h"

void ScreenManager::Initialize(const char* fbdev, const char* touchdev) {
    lv_init();
    disp = lv_linux_fbdev_create();
    if (!disp) {
        throw std::runtime_error("Failed to create framebuffer display");
    }
    lv_linux_fbdev_set_file(disp, fbdev);

    touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, touchdev);
    if (!touch) {
        throw std::runtime_error("Failed to create touchscreen input device");
    }
    lv_indev_set_display(touch, disp);    
    lv_evdev_set_calibration(touch, 200, 3900, 3900, 200);
    lv_evdev_set_swap_axes(touch, true);
}

void ScreenManager::Start(IComponent& view) {
    while (1)
    {
        uint32_t time_till_next = lv_timer_handler();
        view.Draw();
        lv_delay_ms(time_till_next);
    }
}