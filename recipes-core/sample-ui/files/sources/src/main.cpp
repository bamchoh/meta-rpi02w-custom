#include "lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

void event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        fprintf(stderr, "Shutdown!\n");
        kill(getpid(), SIGINT);
    }
}

static lv_obj_t * active_ta = NULL;  // 現在フォーカス中のテキストエリア

static void textarea_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    LV_LOG_USER("Enter was pressed. The current text is: %s", lv_textarea_get_text(ta));
}

static void textarea_focus_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_FOCUSED) {
        active_ta = lv_event_get_target(e);   // フォーカスされた方を記録
    }
}

static void btnm_event_handler(lv_event_t * e)
{
    if(active_ta == NULL) return;  // まだ何もフォーカスされてない場合は無視

    lv_obj_t * obj = lv_event_get_target(e);
    const char * txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));

    if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) lv_textarea_del_char(active_ta);
    else if(strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) lv_event_send(active_ta, LV_EVENT_READY, NULL);
    else lv_textarea_add_text(active_ta, txt);
}

void lv_example_textarea_1(void)
{
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(ta, true);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_add_event_cb(ta, textarea_event_handler, LV_EVENT_READY, ta);
    lv_obj_add_event_cb(ta, textarea_focus_event, LV_EVENT_FOCUSED, NULL);

    lv_obj_t * ta1 = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(ta1, true);
    lv_obj_align(ta1, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_add_event_cb(ta1, textarea_event_handler, LV_EVENT_READY, ta1);
    lv_obj_add_event_cb(ta1, textarea_focus_event, LV_EVENT_FOCUSED, NULL);

    static const char * btnm_map[] = {"1", "2", "3", "\n",
                                      "4", "5", "6", "\n",
                                      "7", "8", "9", "\n",
                                      LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_NEW_LINE, ""
                                     };

    lv_obj_t * btnm = lv_btnmatrix_create(lv_scr_act());
    lv_obj_set_size(btnm, 200, 150);
    lv_obj_align(btnm, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btnm, btnm_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE); /*To keep the text area focused on button clicks*/
    lv_btnmatrix_set_map(btnm, btnm_map);
}

int main(void)
{
    uint32_t disp_width;
    uint32_t disp_height;
    uint32_t disp_dpi;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t *disp_buf;
    static lv_disp_drv_t disp_drv;
    lv_disp_t *disp;
    lv_indev_drv_t indev_drv;
    lv_indev_t *indev;
    uint64_t lvgl_tick_prev = -1;

    lv_init();

    fbdev_init();
    fbdev_get_sizes(&disp_width, &disp_height, &disp_dpi);
    printf("Display: %ux%u dpi=%u\n", disp_width, disp_height, disp_dpi);
    disp_buf = (lv_color_t *) malloc(sizeof(lv_color_t) * disp_width * disp_height);
    if(disp_buf == nullptr) {
        fprintf(stderr, "Failed to alocate display buffer\n");
        return -1;
    }
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, disp_width * disp_height);


    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res = disp_width;
    disp_drv.ver_res = disp_height;
    disp_drv.dpi = disp_dpi;
    disp = lv_disp_drv_register(&disp_drv);


    evdev_init();
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    indev = lv_indev_drv_register(&indev_drv);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);    

    // header container
    /*
    lv_obj_t * container_header = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container_header, disp_width, LV_SIZE_CONTENT);
    lv_obj_align(container_header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_flow(container_header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container_header, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_color(container_header, lv_color_white(), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(container_header); // 現在アクティブなスクリーンに配置
    lv_label_set_text(label, "Hello, LVGL v9!");

    lv_obj_t *label_clock = lv_label_create(container_header); // 現在アクティブなスクリーンに配置
    lv_label_set_text(label_clock, "(clock here)");

    lv_obj_t *shutdown_btn = lv_btn_create(container_header);
    lv_obj_add_event_cb(shutdown_btn, event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *shutdown_label = lv_label_create(shutdown_btn);
    lv_label_set_text(shutdown_label, "Shutdown");
    lv_obj_center(shutdown_label);
    */

    lv_example_textarea_1();

    while(1) {
        /*
        struct timespec ts;
        if(clock_gettime(CLOCK_REALTIME, &ts) < 0) {
            fputs("Failed to get time\n", stderr);
            return -1;
        }
        struct tm gt;
        if(gmtime_r(&ts.tv_sec, &gt) == nullptr) {
            fputs("Failed to convert time\n", stderr);
            return -1;
        }
        char clock[64];
        snprintf(clock, 64, "%02u-%02u-%04u %02u:%02u:%02u (UTC)",
            gt.tm_mon + 1,
            gt.tm_mday,
            gt.tm_year + 1900,
            gt.tm_hour,
            gt.tm_min,
            gt.tm_sec);
        lv_label_set_text(label_clock, clock);
        */

        struct timespec ts4tick;
        clock_gettime(CLOCK_MONOTONIC, &ts4tick);
        uint64_t curr = (uint64_t) ts4tick.tv_sec * 1000000000 + (uint64_t) ts4tick.tv_nsec;
        if(lvgl_tick_prev < 0) {
            lvgl_tick_prev = curr;
        } else {
            uint32_t actual_tick_period = (curr - lvgl_tick_prev) / 1000000;
            lv_tick_inc(actual_tick_period);
            lvgl_tick_prev = curr;
        }

        lv_timer_handler();  // タイマー処理（描画更新など）

        usleep(5000);        // 5ms スリープ
    }

    return 0;
}
