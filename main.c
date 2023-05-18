#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "swapNumbers.h"
#include <math.h>
#include <string.h>

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

void savePixels(const char* filename, char* pixel_data, int pixels_offset, int pixels_size)
{
    FILE* file = fopen(filename, "r+b");
    fseek(file, pixels_offset, 1);
    fwrite(pixel_data, pixels_size*3, 1, file);
    fclose(file);
}

char* transformPixels(char* pixels, char* pixels_copy, int width, int height)
{
    int RADIUS = 50;
    float ANGLE = 0.7853; // pi/4 - 45 degrees

    int total_pixels = width*height;
    int total_pixels_size = total_pixels*3;

    int center_x = width/2;
    int center_y = height/2;

    for (int px = 0; px < total_pixels; px++)
    {
        int x = px % height;
        int y = px / height;

        float dx = x-center_x;
        float dy = y-center_y;

        int distance_from_center2 = (dx * dx) + (dy * dy);

        if(distance_from_center2 > RADIUS*RADIUS)
        {
            continue;
        }
        else
        {
            int distance_from_center = sqrt(distance_from_center2);
            float ratio = (float)(RADIUS-distance_from_center)/(float)RADIUS;
            float rot_angle = ratio * ANGLE;
            // float rot_angle = ANGLE;

            int out_x = cos(rot_angle)*(x-center_x) - sin(rot_angle)*(y-center_y) + center_x;
            int out_y = sin(rot_angle)*(x-center_x) + cos(rot_angle)*(y-center_y) + center_y;

            int offset = (out_y*width + out_x)*3;
            uint8_t red = pixels[offset];
            uint8_t green = pixels[offset+1];
            uint8_t blue = pixels[offset+2];

            pixels_copy[px*3] = red;
            pixels_copy[px*3+1] = green;
            pixels_copy[px*3+2] = blue;
        }
    }

    return pixels_copy;
}

int main()
{
    const char* filename = "ein24.bmp";
    BMPHeader header;
    BMPInfoHeader info_header;
    char* pixel_data_copy = (char*)loadBMP(filename, &header, &info_header);
    char* pixel_data = (char*)loadBMP(filename, &header, &info_header);

    int img_size = info_header.width * info_header.height;

    char* transformed_pixels = transformPixels(pixel_data, pixel_data_copy, info_header.width, info_header.height);

    // savePixels(filename, pixel_data, header.pixel_data_offset, img_size);
    savePixels(filename, transformed_pixels, header.pixel_data_offset, img_size);

    free(pixel_data);
    return 0;
}
