/*
 * Hardware: Arduino Giga R1 WiFi + Giga Display Shield
 * Target: LVGL 9.4.x (Landscape)
 * Fix: Uses GigaDisplay.h to ensure draw functions are recognized.
 */

#include "Arduino_GigaDisplay.h" // 🟢 Use this instead of H7_Video for direct drawing
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

// 🟢 Initialize Hardware
GigaDisplay Display; 
Arduino_GigaDisplayTouch Touch;

// 🟢 Buffers
#define DRAW_BUF_SIZE (480 * 800 / 10 * sizeof(lv_color16_t))
static uint8_t buf1[DRAW_BUF_SIZE];

// ==========================================
// 1. FINAL FIXED FLUSH CALLBACK
// ==========================================
void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    // GigaDisplay library uses these exact names for drawing rectangles of pixels
    Display.beginDraw(area->x1, area->y1, w, h);
    Display.drawPixels((uint16_t*)px_map, w * h);
    Display.endDraw();

    lv_display_flush_ready(disp);
}

// ==========================================
// 2. TOUCH CALLBACK (Landscape Mapping)
// ==========================================
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    uint8_t contacts = Touch.getTouchPoints(NULL);

    if (contacts > 0) {
        GDTpoint_t points[5];
        Touch.getTouchPoints(points);
        data->state = LV_INDEV_STATE_PRESSED;

        // 🔄 Landscape 90° Mapping: Physical Y -> Logical X, 480-Physical X -> Logical Y
        data->point.x = points[0].y;
        data->point.y = 480 - points[0].x;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void setup() {
    Serial.begin(115200);

    // 1. Hardware Init
    Display.begin();
    Touch.begin();

    // 2. LVGL Init
    lv_init();

    // 3. Create Display (Native resolution)
    lv_display_t * disp = lv_display_create(480, 800); 
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf1, NULL, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    // 🟢 SOFTWARE ROTATION TO LANDSCAPE
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

    // 4. Input Device Setup
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    // 5. Minimal UI Test
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t * btn = lv_button_create(screen);
    lv_obj_set_size(btn, 200, 60);
    lv_obj_center(btn);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA00), LV_PART_MAIN);
    
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "SUCCESS 🟢");
    lv_obj_center(label);
}

void loop() {
    lv_timer_handler(); 
    delay(5);
}