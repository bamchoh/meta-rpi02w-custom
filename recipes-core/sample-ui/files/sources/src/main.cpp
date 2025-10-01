#include "lvgl.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include "lvgl/src/core/lv_global.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

#include <ScreenManager.h>
#include <IComponent.h>

class Model : public IComponent {
private:
    lv_subject_t time_subject;
    char current_time[64];

public:
    Model() {
        lv_subject_init_string(&time_subject, current_time, NULL, sizeof(current_time), "");
    };

    lv_subject_t* GetSubject() { return &time_subject; }

    ~Model() {
    };

    void Draw() override {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        struct tm *tm = std::localtime(&t);

        char buf[64];
        snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d (%s)",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            (tm->tm_wday == 0) ? "Sun" :
            (tm->tm_wday == 1) ? "Mon" :
            (tm->tm_wday == 2) ? "Tue" :
            (tm->tm_wday == 3) ? "Wed" :
            (tm->tm_wday == 4) ? "Thu" :
            (tm->tm_wday == 5) ? "Fri" : "Sat");

        lv_subject_copy_string(&time_subject, buf);
        lv_subject_notify(&time_subject);

    }
};

class SystemDateTimeWidget {
public:
    static void MySubjectObserverCb(lv_observer_t *observer, lv_subject_t *subject)
    {
        const char* buf = lv_subject_get_string(subject);
        lv_obj_t *lbl = (lv_obj_t *)lv_observer_get_target(observer);
        if (lbl) {
            lv_label_set_text(lbl, buf);
        }
    }

    void SetupWidget(lv_obj_t* parent, lv_subject_t* my_subject);
};

void SystemDateTimeWidget::SetupWidget(lv_obj_t* parent, lv_subject_t* my_subject)
{
    lv_obj_t* current_datetime = lv_obj_create(parent);
    lv_obj_set_size(current_datetime, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t* current_datetime_label = lv_label_create(current_datetime);
    lv_label_set_text(current_datetime_label, "----/--/-- --:--:-- (---)");
    lv_subject_add_observer_obj(my_subject, MySubjectObserverCb, current_datetime_label, NULL);
}

class BaseView : public IComponent {
private:
    lv_style_t style_text_muted;
    lv_style_t style_title;
    lv_style_t style_icon;
    lv_style_t style_bullet;
    lv_style_t style_parent_flex_obj;
    lv_style_t style_textblock;

    const lv_font_t* font_large;
    const lv_font_t* font_normal;

    void HeaderCreate(lv_obj_t* parent);
    void ProfileCreate(lv_obj_t* parent, Model& model);

    std::vector<IComponent*> components;

public:
    BaseView() {
        font_large = LV_FONT_DEFAULT;
        font_normal = LV_FONT_DEFAULT;

        int32_t tab_h;
        tab_h = 45;

        lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
            font_normal);

        lv_style_init(&style_text_muted);
        lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

        lv_style_init(&style_title);
        lv_style_set_text_font(&style_title, font_large);

        lv_style_init(&style_icon);
        lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
        lv_style_set_text_font(&style_icon, font_large);

        lv_style_init(&style_bullet);
        lv_style_set_border_width(&style_bullet, 0);
        lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

        lv_style_init(&style_parent_flex_obj);
        lv_style_set_size(&style_parent_flex_obj, LV_PCT(100), LV_SIZE_CONTENT);
        lv_style_set_border_width(&style_parent_flex_obj, 0);
        lv_style_set_bg_opa(&style_parent_flex_obj, LV_OPA_0);
        lv_style_set_pad_all(&style_parent_flex_obj, 0);

        lv_style_init(&style_textblock);
        lv_style_set_width(&style_textblock, LV_PCT(100));

        lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);
    }

    void Show(Model& model);

    void Draw() override {
        for (const auto& component : components) {
            component->Draw();
        }
    }
};


void BaseView::Show(Model& model)
{
    HeaderCreate(lv_layer_top());

    ProfileCreate(lv_screen_active(), model);
}



void BaseView::HeaderCreate(lv_obj_t* parent)
{
    lv_obj_t* header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(100), 40);               // 画面幅いっぱい、高さ40px
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x111184), 0); // 背景色
    lv_obj_set_style_pad_all(header, 8, 0);                 // 内側余白
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_opa(header, LV_OPA_80, 0);
    /* Label を作る */
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Hardware Settings");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_center(title);  // 中央配置
}

void BaseView::ProfileCreate(lv_obj_t* parent, Model& model)
{
    lv_obj_t* panel2 = lv_obj_create(parent);
    lv_obj_set_height(panel2, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(panel2, 50, 0);

    lv_obj_t* panel2_title = lv_label_create(panel2);
    lv_label_set_text_static(panel2_title, "Diagnostics");
    lv_obj_add_style(panel2_title, &style_title, 0);

    lv_obj_t* flex_obj_root = lv_obj_create(panel2);
    lv_obj_set_flex_flow(flex_obj_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(flex_obj_root, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(flex_obj_root, 0, 0);
    // lv_obj_add_style(flex_obj_root, &style_parent_flex_obj, 0);
    lv_obj_set_style_pad_all(flex_obj_root, 0, 0);
    lv_obj_set_style_pad_row(flex_obj_root, 30, 0);



    lv_obj_t* flex_obj1 = lv_obj_create(flex_obj_root);
    lv_obj_set_flex_flow(flex_obj1, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(flex_obj1, &style_parent_flex_obj, 0);

    lv_obj_t* date_time_label = lv_label_create(flex_obj1);
    lv_label_set_text_static(date_time_label, "Date/Time");
    lv_obj_add_style(date_time_label, &style_text_muted, 0);

    SystemDateTimeWidget dt_widget;
    dt_widget.SetupWidget(flex_obj1, model.GetSubject());
    components.push_back(&model);

    lv_obj_t* flex_obj2 = lv_obj_create(flex_obj_root);
    lv_obj_set_flex_flow(flex_obj2, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(flex_obj2, &style_parent_flex_obj, 0);

    lv_obj_t* ip_addr1_label = lv_label_create(flex_obj2);
    lv_label_set_text_static(ip_addr1_label, "Ethernet 1");
    lv_obj_add_style(ip_addr1_label, &style_text_muted, 0);

    lv_obj_t* current_ip_addr1 = lv_obj_create(flex_obj2);
    lv_obj_set_size(current_ip_addr1, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t* current_ip_addr1_label = lv_label_create(current_ip_addr1);
    lv_label_set_text(current_ip_addr1_label, "192.168.1.1");



    lv_obj_t* flex_obj3 = lv_obj_create(flex_obj_root);
    lv_obj_set_flex_flow(flex_obj3, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(flex_obj3, &style_parent_flex_obj, 0);

    lv_obj_t* ip_addr2_label = lv_label_create(flex_obj3);
    lv_label_set_text_static(ip_addr2_label, "Ethernet 2");
    lv_obj_add_style(ip_addr2_label, &style_text_muted, 0);

    lv_obj_t* current_ip_addr2 = lv_obj_create(flex_obj3);
    lv_obj_set_size(current_ip_addr2, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t* current_ip_addr2_label = lv_label_create(current_ip_addr2);
    lv_label_set_text(current_ip_addr2_label, "192.168.1.2");



    lv_obj_t* flex_obj4 = lv_obj_create(flex_obj_root);
    lv_obj_set_flex_flow(flex_obj4, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(flex_obj4, &style_parent_flex_obj, 0);

    lv_obj_t* version_title = lv_label_create(flex_obj4);
    lv_label_set_text_static(version_title, "Versions");
    lv_obj_add_style(version_title, &style_text_muted, 0);

    lv_obj_t* version_comp1 = lv_obj_create(flex_obj4);
    lv_obj_set_flex_flow(version_comp1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_border_width(version_comp1, 0, 0);
    lv_obj_set_size(version_comp1, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t* version1_label = lv_label_create(version_comp1);
    lv_label_set_text(version1_label, "Version1");
    lv_obj_set_size(version1_label, LV_PCT(45), LV_SIZE_CONTENT);

    lv_obj_t* version1_value = lv_label_create(version_comp1);
    lv_label_set_text(version1_value, "1.0.0.0");
    lv_obj_set_size(version1_value, LV_PCT(45), LV_SIZE_CONTENT);

    lv_obj_t* version_comp2 = lv_obj_create(flex_obj4);
    lv_obj_set_flex_flow(version_comp2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_border_width(version_comp2, 0, 0);
    lv_obj_set_size(version_comp2, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t* version2_label = lv_label_create(version_comp2);
    lv_label_set_text(version2_label, "Version1");
    lv_obj_set_size(version2_label, LV_PCT(45), LV_SIZE_CONTENT);

    lv_obj_t* version2_value = lv_label_create(version_comp2);
    lv_label_set_text(version2_value, "12.23.34.45");
    lv_obj_set_size(version2_value, LV_PCT(45), LV_SIZE_CONTENT);


    static int32_t grid_main_col_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t grid_main_row_dsc[] = { LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };

    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    static int32_t grid_2_col_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t grid_2_row_dsc[] = {
        LV_GRID_CONTENT,      /*Title*/
        5,                    /*Separator*/
        LV_GRID_CONTENT,      /*Box title*/
        LV_GRID_TEMPLATE_LAST /*Box*/
    };

    lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
    // lv_obj_set_grid_dsc_array(panel3, grid_main_col_dsc, grid_2_row_dsc);

    lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(flex_obj_root, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 2, 1);
}



int main(void)
{
    auto sm = std::make_unique<ScreenManager>();

    auto view = std::make_unique<BaseView>();

    Model model;
    view->Show(model);

    sm->Start(*view);

    return 0;
}
