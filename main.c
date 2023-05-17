#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "swapNumbers.h"
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

void savePixels(const char* filename, const unsigned char* pixel_data, int pixels_offset, int pixels_size)
{
    FILE* file = fopen(filename, "r+b");
    fseek(file, pixels_offset, 1);
    fwrite(pixel_data, pixels_size*3, 1, file);
    fclose(file);
}

int main()
{
    const char* filename = "ein24.bmp";
    BMPHeader header;
    BMPInfoHeader info_header;
    unsigned char* pixel_data = (unsigned char*)loadBMP(filename, &header, &info_header);

    // Use the pixel_data pointer to access the pixel values
    int img_size = info_header.width * info_header.height;

    for (int i = 0; i < img_size*3; i += 3) {
        // Extract the RGB components
        unsigned char blue = pixel_data[i];
        unsigned char green = pixel_data[i + 1];
        unsigned char red = pixel_data[i + 2];

        // Calculate the average value
        unsigned char gray = (red + green + blue) / 3;

        // Set the grayscale value for all components
        pixel_data[i] = gray;
        pixel_data[i + 1] = gray;
        pixel_data[i + 2] = gray;
    }

    savePixels(filename, pixel_data, header.pixel_data_offset, img_size);

    free(pixel_data);
    return 0;
}

// int main(int argc, char *argv[])
// {
//     if(argc < 2)
//     {
//         printf("Arg. missing\n");
//         return;
//     }
//     char* res = swapNumbers(argv[1]);
//     printf(res);
//     printf("\n");
//     return 0;
// }
