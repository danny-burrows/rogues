#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    unsigned char ch;
    unsigned char r;
    unsigned char g;
    unsigned char b;
} AnsiChar;

typedef struct {
    size_t width;
    size_t height;
    AnsiChar * data;
} Image;

char * scale = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
int numScale;

unsigned char luminanceFromRGB(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned char) (0.2126 * r + 0.7152 * g + 0.0722 * b);
}

Image * loadImage(char * location) {
    FILE * f = fopen(location, "rb");

    if (f == NULL) {
        perror("[ERR] Couldn't open image:");
        return NULL;
    }

    // Get bmp file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Checking against SIZE_MAX (Might want to change) and 54 (Min bmp size).
    if(size > SIZE_MAX) {
        fprintf(stderr, "[ERR] Image size is too big!");
        fclose(f);	
        return NULL;
    } else if (size < 54) {
        fprintf(stderr, "[ERR] Image size too small: May be corrupt or invalid file type.");
        return NULL;
    }

    unsigned char * bmp_file_data;
    bmp_file_data = (unsigned char *) malloc((size_t) size); // Not sure on the cast to size_t here? :/

    if (size != fread(bmp_file_data, sizeof(unsigned char), (size_t) size, f)) {
        perror("[ERR] Failed to read bmp file:");
        free(bmp_file_data);
        fclose(f);
        return NULL;
    }

    fclose(f);

    // Magic section...

    if (bmp_file_data[0] != 'B' || bmp_file_data[1] != 'M') {
        fprintf(stderr, "[ERR] Can't find BM in file header. File type may not be BMP");
        free(bmp_file_data);
        return NULL;
    }

    size_t specifiedSize = bmp_file_data[2] | bmp_file_data[3] << 8 | bmp_file_data[4] << 16 | bmp_file_data[5] << 24;

    if (specifiedSize != size) {
        fprintf(stderr, "[ERR] Size in header doesn't match file size!");
        free(bmp_file_data);
        return NULL;
    }

    size_t pdOffset = bmp_file_data[10] | bmp_file_data[11] << 8 | bmp_file_data[12] << 16   | bmp_file_data[13] << 24;

    unsigned long width = bmp_file_data[18] | bmp_file_data[19] << 8 | bmp_file_data[20] << 16   | bmp_file_data[21] << 24;
    unsigned long height = bmp_file_data[22] | bmp_file_data[23] << 8 | bmp_file_data[24] << 16 | bmp_file_data[25] << 24;

    unsigned long bpp = bmp_file_data[28] | bmp_file_data[29] << 8;
    int noCompression = bmp_file_data[30] == 0 && bmp_file_data[31] == 0 && bmp_file_data[32] == 0   && bmp_file_data[33] == 0;

    if (bpp != 24 || !noCompression || width < 1 || height < 1 || width > 64000 || height > 64000) {
        fprintf(stderr, "[ERR] Unsupported BMP format, only 24 bits per pixel is supported!");
        free(bmp_file_data);
        return NULL;
    }

    int bytesPerPixel = (int) (bpp / 8);

    size_t rowBytes = (width * bytesPerPixel + 3) / 4 * 4;

    printf("Bytes per row: %lu\n", rowBytes);

    size_t usedRowBytes = width * bytesPerPixel;
    size_t imageBytes = rowBytes * height;

    if (pdOffset > size || pdOffset + imageBytes > size) {
        fprintf(stderr, "[ERR] Invalid offset specified!");
        free(bmp_file_data);
        return NULL;
    }

    Image * img = malloc(sizeof(Image));
    img->height = height;
    img->width = width;

    size_t imgSize = width * height;

    img->data = (AnsiChar *) malloc(imgSize * sizeof(AnsiChar));

    printf("Offset: %lu\n", pdOffset);

    AnsiChar * ptr = img->data;
    unsigned char * srcPtr = &bmp_file_data[pdOffset];

    for (size_t i = 0; i < imgSize; ++i) {
        unsigned char r = *srcPtr;
        unsigned char g = *(srcPtr + 1);
        unsigned char b = *(srcPtr + 2);

        ptr->r = r;
        ptr->g = g;
        ptr->b = b;
        ptr->ch = luminanceFromRGB(r, g, b);

        ptr++;
        srcPtr += bytesPerPixel;

        if (i % width == 0) {
            srcPtr += rowBytes - usedRowBytes;
        }
    }

    free(bmp_file_data);

    return img;
}

void convert_to_ascii(Image * img) {
    for (size_t y = img->height - 1; y > 0; --y)
    {
        for (size_t x = 0; x < img->width; ++x)
        {
            AnsiChar * c = img->data + x + img->width * y;
            int rescaled = c->ch * numScale / 256;
            printf("\033[38;2;%u;%u;%um%c", c->b, c->g, c->r, scale[numScale - rescaled]);
        }

        printf("\n");
    }
}

void release(Image * img){
    if (img){
        if (img->data) free(img->data);
        free(img);
    }
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERR] Argument needed: filepath to BMP image.");
        return EXIT_FAILURE;
    }

    puts(argv[1]);

    // setbuf(stdout, 0);

    numScale = strlen(scale) - 1;
    printf("ASCII Brightness Scale: %d\n", numScale);

    Image * img = loadImage(argv[1]);

    if (img != NULL) {
        printf("Image dimensions: %lux%lu\n", img->width, img->height);

        convert_to_ascii(img);

        release(img);
    }

    return EXIT_SUCCESS;
}
