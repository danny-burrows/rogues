#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} PixelData;

typedef struct {
    size_t width;
    size_t height;
    PixelData * data;
} Image;

char * scale = "$@&B%8WM#ZO0QoahkbdpqwmLCJUYXIjft/\\|()1{}[]l?zcvunxr!<>i;:*-+~_,\"^`'. ";
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
        fprintf(stderr, "[ERR] Image size is too big!\n");
        fclose(f);	
        return NULL;
    } else if (size < 54) {
        fprintf(stderr, "[ERR] Image size too small: May be corrupt or invalid file type.\n");
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
        fprintf(stderr, "[ERR] Can't find BM in file header. File type may not be BMP\n");
        free(bmp_file_data);
        return NULL;
    }

    size_t specifiedSize = bmp_file_data[2] | bmp_file_data[3] << 8 | bmp_file_data[4] << 16 | bmp_file_data[5] << 24;

    if (specifiedSize != size) {
        fprintf(stderr, "[ERR] Size in header doesn't match file size!\n");
        free(bmp_file_data);
        return NULL;
    }

    size_t pdOffset = bmp_file_data[10] | bmp_file_data[11] << 8 | bmp_file_data[12] << 16   | bmp_file_data[13] << 24;

    unsigned long width = bmp_file_data[18] | bmp_file_data[19] << 8 | bmp_file_data[20] << 16   | bmp_file_data[21] << 24;
    unsigned long height = bmp_file_data[22] | bmp_file_data[23] << 8 | bmp_file_data[24] << 16 | bmp_file_data[25] << 24;

    unsigned long bpp = bmp_file_data[28] | bmp_file_data[29] << 8;
    int noCompression = bmp_file_data[30] == 0 && bmp_file_data[31] == 0 && bmp_file_data[32] == 0   && bmp_file_data[33] == 0;

    if (bpp != 24 || !noCompression || width < 1 || height < 1 || width > 64000 || height > 64000) {
        fprintf(stderr, "[ERR] Unsupported BMP format, only 24 bits per pixel is supported!\n");
        free(bmp_file_data);
        return NULL;
    }

    int bytesPerPixel = (int) (bpp / 8);

    size_t rowBytes = (width * bytesPerPixel + 3) / 4 * 4;

    printf("Bytes per row: %lu\n", rowBytes);

    size_t usedRowBytes = width * bytesPerPixel;
    size_t imageBytes = rowBytes * height;

    if (pdOffset > size || pdOffset + imageBytes > size) {
        fprintf(stderr, "[ERR] Invalid offset specified!\n");
        free(bmp_file_data);
        return NULL;
    }

    Image * img = malloc(sizeof(Image));
    img->height = height;
    img->width = width;

    size_t imgSize = width * height;

    img->data = (PixelData *) malloc(imgSize * sizeof(PixelData));

    printf("Offset: %lu\n", pdOffset);

    PixelData * ptr = img->data;
    unsigned char * srcPtr = &bmp_file_data[pdOffset];

    for (size_t i = 0; i < imgSize; ++i) {
        unsigned char r = *srcPtr;
        unsigned char g = *(srcPtr + 1);
        unsigned char b = *(srcPtr + 2);

        ptr->r = r;
        ptr->g = g;
        ptr->b = b;

        ptr++;
        srcPtr += bytesPerPixel;

        if (i % width == 0) {
            srcPtr += rowBytes - usedRowBytes;
        }
    }

    free(bmp_file_data);

    return img;
}

unsigned char uchar_avg(unsigned char char1, unsigned char char2) {
    return (char1 + char2) / 2;
}

void convert_to_ascii(Image * img) {
    for (size_t y = img->height - 1; y > 1; y -= 2)
    {
        for (size_t x = 0; x < img->width; ++x)
        {
            // Take the average of this row and next to squash image vertially.
            PixelData * c = img->data + x + img->width * y;
            PixelData * c2 = img->data + x + img->width * (y - 1);
            
            c->r = uchar_avg(c->r, c2->r);
            c->b = uchar_avg(c->b, c2->b);
            c->g = uchar_avg(c->g, c2->g);

            // Calc luminace and use to find Ascii char.
            unsigned char ch = luminanceFromRGB(c->r, c->g, c->b);
            int rescaled = ch * numScale / 256;

            // Left padding numbers with 0's to 3 digits.
            printf("\033[38;2;%03u;%03u;%03um%c", c->b, c->g, c->r, scale[numScale - rescaled]);
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
        fprintf(stderr, "[ERR] Argument needed: filepath to BMP image.\n");
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
