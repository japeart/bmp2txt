/*
 * Utility for converting BMP font sheets to ASCII tiles for use with 
 * assembly language games or anywhere else you want to define gfx in 
 * binary (like Z80 progrmming for the speccy)
 */

#include "SDL2/SDL.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* forward decalarations */
SDL_Surface* loadImage(const char* path);
Uint32 getPixel(SDL_Surface* surface, int x, int y);
void printColourValue(SDL_Color colour, int i, int j);
void printTile(int x, int y);

/* variables */
const int debug = 1;
int imageWidth;
int imageHeight;
SDL_Surface* surface;
SDL_PixelFormat* format;
int bitDepth;
const int outputToConsole = 1;
int tileWidth;
int tileHeight;
FILE *file;
char* inputFilename;
char* outputFilename;

int main(int argc, char* argv[])
{
    /* print parameters */
    if (debug) {
        for (int i = 0; i < argc; ++i)
        {
            printf("argv[%d]: %s\n", i, argv[i]);
        }
    }
    
    if(argc == 1) {
        printf("%s\n", "Usage: input_file output_filename tile_width tile_height");
        return -1;
    }
    
    int opt;
    while((opt = getopt(argc, argv, "i:f:w:h:")) != -1) {
        switch (opt) {
            case 'i':
                inputFilename = optarg;
                break;
                
            case 'f':
                outputFilename = optarg;
                strcat(outputFilename, ".txt");
                break;
                
            case 'w':
                tileWidth = atoi(optarg) == 0 ? 8 : atoi(optarg);
                break;
                
            case 'h':
                tileHeight = atoi(optarg) == 0 ? 8 : atoi(optarg);
                break;
                
                default:
                printf("Unknown parameter supplied!");
                return -1;
        }
    }
    
    /* input validtation */
    if(!(surface = loadImage(inputFilename))) {
        printf("Unable to load image, exiting program\n");
        return -1;
    } else if (tileWidth > imageWidth || tileWidth <= 0) {
        printf("Invalid tile width supplied, exiting program\n");
        return -1;
    } else if (tileHeight > imageHeight || tileHeight <= 0) {
        printf("Invalid tile height supplied, exiting program\n");
        return -1;
    }
    
    /* okay, now write out to file */
    file = fopen(outputFilename, "w");
    int tileNumber = 0;
    
    /* outer loop */
    for(int i = 0; i < imageHeight; i += tileHeight) {
        for(int j = 0; j < imageWidth; j += tileWidth) {
            fprintf(file, "; Tile # %d ", tileNumber);
            fprintf(file, "at: %d, %d\n\n", j, i);
            printTile(j, i);
            tileNumber++;
        }
    }
    
    /* close file and exit */
    if(fclose(file) != 0) {
        printf("Unable to write file");
        return -1;
    }
}

/*
 * Print a single tile to output file/console
 * @param x - the leftmost starting coordinate
 * @param y - the uppermost starting coordinate
 */
void printTile(const int x, const int y)
{
    SDL_Color colour;
    Uint32 data;
    char outputString[tileWidth * 2 + 1];
    
    /* loop through tile and produce output */
    for(int j = y; j < y + tileHeight; j++) {
        /* remember to allow for the termination character */
        int tilePlace = 0;
        for(int i = x; i < x + tileWidth; i++) {
            data = getPixel(surface, i, j);
            SDL_GetRGB(data, surface->format, &colour.r, &colour.g, &colour.b);
            outputString[tilePlace] = colour.r + colour.g + colour.b == 0 ? '0' : '1';
            outputString[tilePlace + 1] = ',';
            tilePlace += 2;
        }
        fprintf(file, "%s\n", outputString);
    }
    fprintf(file, "\n");
}

/*
 * Load a BMP, set up the image variables.
 * @return a pointer to an SDL_Surface
 */
SDL_Surface* loadImage(const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    if(!surface) {
        printf("%s\n", SDL_GetError());
        return NULL;
    }
    else {
        printf("Image Loaded...\n");
        /* print some stats about the image */
        imageWidth = surface->w;
        imageHeight = surface->h;
        format = surface->format;
        bitDepth = (int) format->BitsPerPixel;
        printf("Image Bit Depth: %d\n", bitDepth);
        printf("Image Width: %d\n", surface->w);
        printf("Image Height: %d\n", surface->h);
    }
    return surface;
}

/*
 * Code is referenced from:
 * https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
 */
Uint32 getPixel(SDL_Surface *surface, const int x, const int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch (bpp)
    {
        case 1:
            return *p;
            break;
            
        case 2:
            return *(Uint16 *)p;
            break;
            
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
            
        case 4:
            return *(Uint32 *)p;
            break;
            
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}
