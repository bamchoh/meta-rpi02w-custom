#include "Theme.h"

lv_style_t Theme::ParentFlexObj;
lv_style_t Theme::TextMuted;
lv_style_t Theme::Title;
lv_style_t Theme::Icon;
lv_style_t Theme::Bullet;
lv_style_t Theme::TextBlock;

void Theme::Init() {
    lv_style_init(&TextMuted);
    lv_style_set_text_opa(&TextMuted, LV_OPA_50);

    lv_style_init(&Title);
    lv_style_set_text_font(&Title, LV_FONT_DEFAULT);

    lv_style_init(&Icon);
    lv_style_set_text_color(&Icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&Icon, LV_FONT_DEFAULT);

    lv_style_init(&Bullet);
    lv_style_set_border_width(&Bullet, 0);
    lv_style_set_radius(&Bullet, LV_RADIUS_CIRCLE);

    lv_style_init(&ParentFlexObj);
    lv_style_set_size(&ParentFlexObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_style_set_border_width(&ParentFlexObj, 0);
    lv_style_set_bg_opa(&ParentFlexObj, LV_OPA_0);
    lv_style_set_pad_all(&ParentFlexObj, 0);

    lv_style_init(&TextBlock);
    lv_style_set_width(&TextBlock, LV_PCT(100));
}