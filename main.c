#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "transformf.h"
#include <math.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_font.h>
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_color.h"
#include <allegro5/allegro_primitives.h>

// gcc -o main main.c -lm -lallegro_image -lallegro_memfile -lallegro -lallegro_font -lallegro_main -lallegro_ttf -lallegro_color -lallegro_primitives

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t pixel_data_offset;
} BMPHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t total_colors;
    uint32_t important_colors;
} BMPInfoHeader;
#pragma pack(pop)

char* loadBMP2(const char* filename, BMPHeader* to_header, BMPInfoHeader* to_info_header)
{
    FILE* file = fopen(filename, "r+b");

    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);
    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file);
    *to_header = header;
    *to_info_header = info_header;

    char* raw_data = malloc(header.file_size);
    fseek(file, 0, SEEK_SET);
    fread(raw_data, header.file_size, 1, file);
    fclose(file);

    return raw_data;
}

void savePixels(const char* filename, char* pixel_data, int pixels_offset, int pixels_size)
{
    FILE* file = fopen(filename, "r+b");
    fseek(file, pixels_offset, 1);
    // fwrite(pixel_data, pixels_size*3, 1, file); // here changed
    fwrite(pixel_data, pixels_size, 1, file);
    fclose(file);
}

char* transformPixels(char* pixels, char* pixels_copy, int origin_x, int origin_y, int radius, int angle_deg, int width, int height)
{
    int total_pixels = width*height;
    int total_pixels_size = total_pixels*3;

    float ANGLE = angle_deg/(float)180 * (float)3.14159265;

    origin_y = height - origin_y;

    int padding_size = width % 4;

    for (int px = 0; px < total_pixels; px++)
    {
        // int x = px % height; // here
        // int y = px / height; // here
        int x = px % width;
        int y = px / width;

        int dx = x-origin_x;
        int dy = y-origin_y;

        int distance_from_center2 = (dx * dx) + (dy * dy);

        if(distance_from_center2 > radius*radius)
        {
            continue;
        }
        else
        {
            int distance_from_center = sqrt(distance_from_center2);
            float ratio = (float)(radius-distance_from_center)/(float)radius;
            float rot_angle = ratio * ANGLE;

            int out_x = cos(rot_angle)*(x-origin_x) - sin(rot_angle)*(y-origin_y) + origin_x;
            int out_y = sin(rot_angle)*(x-origin_x) + cos(rot_angle)*(y-origin_y) + origin_y;

            out_x = fmin(width-1, fmax(0, out_x));
            out_y = fmin(height-1, fmax(0, out_y));

            int padding = out_y * padding_size;
            int offset = (out_y*width + out_x)*3 + padding;
            uint8_t red = pixels[offset+2];
            uint8_t green = pixels[offset+1];
            uint8_t blue = pixels[offset];

            int res_padding = y * padding_size;
            pixels_copy[px*3+2+res_padding] = red;
            pixels_copy[px*3+1+res_padding] = green;
            pixels_copy[px*3+res_padding] = blue;
        }
    }
    return pixels_copy;
}

ALLEGRO_BITMAP *load_bitmap_from_memory(char* data, size_t size)
{
    ALLEGRO_FILE *memfile = al_open_memfile(data, size, "r");
    ALLEGRO_BITMAP *bitmap = al_load_bitmap_f(memfile, ".bmp");
    al_fclose(memfile);

    return bitmap;
}

int main(int argc, char *argv[])
{
    al_init();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_DISPLAY *display;
    ALLEGRO_BITMAP *membitmap;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    int radius = 50;
    int angle = 45;
    int TOP_PADDING = 55;
    // const char* filename = argv[1];
    const char* filename = "land_big.bmp";

    BMPHeader header;
    BMPInfoHeader info_header;
    char* bmp_raw = loadBMP2(filename, &header, &info_header);

    display = al_create_display(info_header.width, info_header.height + TOP_PADDING);

    membitmap = load_bitmap_from_memory(bmp_raw, header.file_size);

    timer = al_create_timer(1.0 / 30);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue , al_get_mouse_event_source());
    al_start_timer(timer);

    while (1) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            char* pixels_copy = (char*)malloc(header.file_size-54);
            memcpy(pixels_copy, bmp_raw+54, header.file_size-54);
            transformPixels(bmp_raw+54, pixels_copy, event.mouse.x, event.mouse.y-TOP_PADDING, radius, angle, info_header.width, info_header.height);
            memcpy(bmp_raw+54, pixels_copy, header.file_size-54);
            free(pixels_copy);
            savePixels(filename, bmp_raw+54, 54, header.file_size-54);
        }
        if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;
            if (event.keyboard.unichar == 'w')
                radius += 10;
            if (event.keyboard.unichar == 's')
                radius -= 10;
            if (event.keyboard.unichar == 'e')
                angle += 5;
            if (event.keyboard.unichar == 'd')
                angle -= 5;
        }
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        if (redraw && al_is_event_queue_empty(queue))
        {
            redraw = false;
            al_clear_to_color(al_map_rgb_f(0, 0, 0));
            ALLEGRO_BITMAP *new_membitmap = load_bitmap_from_memory(bmp_raw, header.file_size);
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(new_membitmap, 0, 55, 0);
            char formattedString[100];
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 5, 0, "Change radius with <w, s>");
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 20, 0, "Change angle with <e, d>");
            sprintf(formattedString, "Radius: %d Angle: %d", radius, angle);
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 35, 0, formattedString);
            al_flip_display();
        }
    }

    free(bmp_raw);
    al_destroy_display(display);
    al_destroy_font(font);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    return 0;
}
