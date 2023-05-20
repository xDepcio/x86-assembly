#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "transformf.h"
#include <math.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_memfile.h>

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

void* loadBMP(const char* filename, BMPHeader* to_header, BMPInfoHeader* to_info_header)
{
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        printf("Failed to open the file: %s\n", filename);
        return NULL;
    }

    // Read the header
    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);

    // Read the info header
    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file);

    int image_size = info_header.width * info_header.height;
    int bytes_pixels_size = image_size * 3;

    // Allocate memory for the pixel data
    unsigned char* pixel_data = (unsigned char*)malloc(bytes_pixels_size);
    if (!pixel_data) {
        printf("Failed to allocate memory for pixel data\n");
        fclose(file);
        return NULL;
    }

    // Read the pixel data
    fseek(file, header.pixel_data_offset, SEEK_SET);
    fread(pixel_data, bytes_pixels_size, 1, file);

    fclose(file);

    *to_header = header;
    *to_info_header = info_header;

    return pixel_data;
}

// char* loadBMPRaw(const char* filename)
// {
//     FILE* file = fopen(filename, "r+b");
//     if (!file) {
//         printf("Failed to open the file: %s\n", filename);
//         return NULL;
//     }

//     // Read the header
//     BMPHeader header;
//     fread(&header, sizeof(BMPHeader), 1, file);

//     char* raw_file = (char*)malloc(header.file_size);
//     fseek(file, 0, 0);
//     fread(raw_file, header.file_size, 1, file);
//     fclose(file);
//     return raw_file;
// }

void savePixels(const char* filename, char* pixel_data, int pixels_offset, int pixels_size)
{
    FILE* file = fopen(filename, "r+b");
    fseek(file, pixels_offset, 1);
    fwrite(pixel_data, pixels_size*3, 1, file);
    fclose(file);
}

// char* transformPixels(char* pixels, char* pixels_copy, int origin_x, int origin_y, int radius, int angle_deg, int width, int height)
// {
//     int total_pixels = width*height;
//     int total_pixels_size = total_pixels*3;

//     float ANGLE = (float)angle_deg/(float)180 * (float)3.14159265;

//     int origin_y = height - origin_y;

//     for (int px = 0; px < total_pixels; px++)
//     {
//         int x = px % height;
//         int y = px / height;

//         float dx = x-origin_x;
//         float dy = y-origin_y;

//         int distance_from_center2 = (dx * dx) + (dy * dy);

//         if(distance_from_center2 > radius*radius)
//         {
//             continue;
//         }
//         else
//         {
//             int distance_from_center = sqrt(distance_from_center2);
//             float ratio = (float)(radius-distance_from_center)/(float)radius;
//             float rot_angle = ratio * ANGLE;

//             int out_x = cos(rot_angle)*(x-origin_x) - sin(rot_angle)*(y-origin_y) + origin_x;
//             int out_y = sin(rot_angle)*(x-origin_x) + cos(rot_angle)*(y-origin_y) + origin_y;

//             int offset = (out_y*width + out_x)*3;
//             uint8_t red = pixels[offset];
//             uint8_t green = pixels[offset+1];
//             uint8_t blue = pixels[offset+2];

//             pixels_copy[px*3] = red;
//             pixels_copy[px*3+1] = green;
//             pixels_copy[px*3+2] = blue;
//         }
//     }
//     return pixels_copy;
// }

ALLEGRO_BITMAP *load_bitmap_from_memory(char* data, size_t size)
{
    ALLEGRO_FILE *memfile = al_open_memfile(data, size, "r");
    if (!memfile)
    {
        fprintf(stderr, "Failed to open memory file!\n");
        return NULL;
    }

    // ALLEGRO_BITMAP *bitmap = al_load_bitmap_f(memfile, ".bmp");
    ALLEGRO_BITMAP *bitmap = al_load_bitmap("ein24.bmp");
    al_fclose(memfile);

    return bitmap;
}

int main()
{
    ALLEGRO_DISPLAY *display;
    ALLEGRO_BITMAP *membitmap, *bitmap;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    int zoom = 1;

    // Initialize Allegro
    if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro!\n");
        return -1;
    }
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();

    const char* filename = "ein24.bmp";
    BMPHeader header;
    BMPInfoHeader info_header;
    // char* pixel_data_copy = (char*)loadBMP(filename, &header, &info_header);
    char* pixel_data = (char*)loadBMP(filename, &header, &info_header);
    // char* bmp_raw = loadBMPRaw(filename);

    int img_size = info_header.width * info_header.height;

    char* bmp_raw = (char*)malloc(header.file_size);
    memcpy(bmp_raw, &header, 14);
    memcpy(bmp_raw+14, &info_header, 40);
    memcpy(bmp_raw+54, pixel_data, header.file_size-54);



    // Create a display window
    display = al_create_display(info_header.width, info_header.height); // Replace width and height with your desired dimensions

    membitmap = load_bitmap_from_memory(bmp_raw, header.file_size); // Replace bmpData and bmpSize with your actual data
    if (!membitmap) {
        fprintf(stderr, "Failed to load bitmap!\n");
        return -1;
    }

    al_draw_bitmap(membitmap, 0, 0, 0); // Draw the bitmap at coordinates (0, 0)
    al_flip_display(); // Update the display

    // char* transformed_pixels = transformPixels(pixel_data, pixel_data_copy, info_header.width, info_header.height);

    // savePixels(filename, pixel_data, header.pixel_data_offset, img_size);
    // savePixels(filename, transformed_pixels, header.pixel_data_offset, img_size);

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
        if (event.type == ALLEGRO_EVENT_DISPLAY_ORIENTATION) {
            int o = event.display.orientation;
            if (o == ALLEGRO_DISPLAY_ORIENTATION_0_DEGREES) {
                printf("0 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_90_DEGREES) {
                printf("90 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_180_DEGREES) {
                printf("180 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_270_DEGREES) {
                printf("270 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_FACE_UP) {
                printf("Face up\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_FACE_DOWN) {
                printf("Face down\n");
            }
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            fprintf(stderr, "dasdas!\n");
            fprintf(stderr, "mouse.x: %d", event.mouse.x);
            fprintf(stderr, "mouse.y: %d", event.mouse.y);
            char* pixels_copy = (char*)malloc(header.file_size-54);
            memcpy(pixels_copy, bmp_raw+54, header.file_size-54);
            transformf(bmp_raw+54, pixels_copy, event.mouse.x, event.mouse.y, 50, 45, info_header.width, info_header.height);
            memcpy(bmp_raw+54, pixels_copy, header.file_size-54);
            free(pixels_copy);
            savePixels("ein24.bmp", bmp_raw+54, 54, header.file_size-54);
        }
        if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            fprintf(stderr, "dasdas!\n");
        }
        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;
            if (event.keyboard.unichar == '1')
                zoom = 1;
            if (event.keyboard.unichar == '+')
                zoom *= 1.1;
            if (event.keyboard.unichar == '-')
                zoom /= 1.1;
            if (event.keyboard.unichar == 'f')
                zoom = (double)al_get_display_width(display) /
                    al_get_bitmap_width(bitmap);
        }
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb_f(0, 0, 0));
            ALLEGRO_BITMAP *new_membitmap = load_bitmap_from_memory(bmp_raw, header.file_size); // Replace bmpData and bmpSize with your actual data
            if (!new_membitmap) {
                fprintf(stderr, "Failed to load bitmap!\n");
                return -1;
            }
            al_draw_bitmap(new_membitmap, 0, 0, 0);
            if (zoom == 1)
                al_draw_bitmap(new_membitmap, 0, 0, 0);
            else
                al_draw_scaled_rotated_bitmap(
                    new_membitmap, 0, 0, 0, 0, zoom, zoom, 0, 0);
            al_flip_display();
        }
    }

    free(pixel_data);
    free(bmp_raw);
    al_destroy_display(display);
    return 0;
}
