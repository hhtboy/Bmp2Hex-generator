#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// bmp file struct
typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned int      bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned int      bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
        unsigned int      biSize;
        unsigned int       biWidth;
        unsigned int       biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned int      biCompression;
        unsigned int      biSizeImage;
        unsigned int       biXPelsPerMeter;
        unsigned int       biYPelsPerMeter;
        unsigned int      biClrUsed;
        unsigned int      biClrImportant;
} BITMAPINFOHEADER;

//useless meta data
typedef struct tagRGBQUAD {
        unsigned char    rgbBlue;
        unsigned char    rgbGreen;
        unsigned char    rgbRed;
        unsigned char    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPHEADER {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    RGBQUAD hRGB[256];
}BITMAPHEADER;

typedef struct color {
    unsigned int data;
    unsigned char index;
}COLOR;

enum PALETTE_4BIT {
    BLACK = 0x000000,
    BLUE = 0x0000FF,
    GREEN = 0x008000,
    TURQUOISE = 0x40E0D0,
    RED = 0xFF0000,
    PURPLE = 0x800080,
    SADDLEBROWN = 0x8B4513,
    LIGHT_TREY = 0xD3D3D3,
    GRAY = 0x808080,
    LIGHT_BLUE = 0xADD8E6,
    LIME = 0x00FF00,
    SKY_BLUE = 0x87CEFA,
    TOMATO = 0xFF6347,
    FUCHSIA = 0xFF00FF,
    YELLOW = 0xFFFF00,
    WHITE = 0xFFFFFF
};

enum PALETTE_2BIT {
    BLACK_2BIT = 0x000000,
    BLUE_2BIT = 0x0000FF,
    RED_2BIT = 0xFF0000,
    WHITE_2BIT = 0xFFFFFF
};

enum COLOR_BIT_SIZE {
    COLOR_4 = 4,
    COLOR_2 = 2
};

static const int HEADER_SIZE = 54;
static const int lcd_width = 160;
static const int lcd_height = 240;

void open_bmp(char* file_name);
void open_bmp_2bit(char* file_name);
void compact_color(COLOR* colors, unsigned int* image_buf, unsigned char* result, unsigned int buf_size, enum COLOR_BIT_SIZE color_bit_size);
unsigned int search_color(COLOR* colors, unsigned int color, enum COLOR_BIT_SIZE color_bit_size); // find the closest color in color template, and return 24bit color
unsigned int rgb_square_mean(unsigned int color1, unsigned int color2); // returns the mean of squared values
void define_colors(COLOR* colors, enum COLOR_BIT_SIZE color_bit_size);
void check_file_availability(BITMAPHEADER* header);
void define_header_name (char* file_name, char* header_name);

int main() 
{
    printf("<Usage> \n");
    printf("1. Enter bmp file name.\n");
    printf("2. Enter bit size of colors : 4 or 2\n");
    printf("File format must be 24bit bmp.\nImage size must be smaller than 160 X 240. \nLength of bmp file name must be shorter than 30.\n");

    char file_name[30];
    char bit_size[4];
    scanf("%s", file_name);
    fflush(stdin);
    strcat()

    scanf("%s", bit_size);
    fflush(stdin);

    if(!strcmp(bit_size,"4")) {
        open_bmp(file_name);
    }
    else if(!strcmp(bit_size, "2")) {
        open_bmp_2bit(file_name);
    }
    else {
        printf("ERROR : unexpected input value. Bit size must be 4 or 2\n");
        exit(1);
    }
}

void open_bmp(char* file_name) 
{
    FILE* pFile = fopen(file_name, "rb");
    
    if(pFile == NULL) {
        printf("cannot open file");
        return;
    }

    char* left_string = NULL;
    strtok_r(file_name, ".", &left_string);

    // check file format
    if(strcmp(left_string, "bmp")) {
        printf("Wrong file format : %s File format should be BMP", left_string);
        return;
    }

    char* prefix = strtok(file_name, ".");

    // *.bmp -> *.h
    int name_length = strlen(file_name);
    char* new_file_name = (char *)malloc(sizeof(char) * (name_length + 2));
    char* header_name = (char *)malloc(sizeof(char) * (name_length + 2));

    // failed to allocate memory
    if(new_file_name == NULL) 
    {
        printf("malloc ERROR !! \n");
    }

    //copy file name
    strcpy(new_file_name, prefix);
    strcat(new_file_name, "_4bit.h");

    define_header_name(new_file_name, header_name);

    FILE* pNewFile = fopen(new_file_name, "wb");

    // initialize header.bf
    BITMAPHEADER header;
    fread(&header.bf.bfType, 1, 2, pFile);
    fread(&header.bf.bfSize, 4, 1, pFile);
    fread(&header.bf.bfReserved1, 2, 1, pFile);
    fread(&header.bf.bfReserved2, 2, 1, pFile);
    fread(&header.bf.bfOffBits, 4, 1, pFile);

    int file_size = header.bf.bfSize;
    
    // initialize header.bi
    fread(&header.bi, 40, 1, pFile);

    check_file_availability(&header);

    int width = header.bi.biWidth;
    int height = header.bi.biHeight;
    int pixel_size = width * height * 3;

    fread(header.hRGB, header.bf.bfOffBits - HEADER_SIZE ,1,pFile);


    // offset , image size, bit depth, 
    u_int8_t buf[width * height * 3];
    u_int8_t dummy[4];
    unsigned char space = width % 4;
    // fread(buf, sizeof(u_int8_t), sizeof(buf), pFile);
    for(int i = 0; i < height; i++) {
        fread(buf + 3 * i * width, sizeof(u_int8_t), 3 * width, pFile);
        fread(dummy, sizeof(u_int8_t), space, pFile);
    }

    // getting pixel start offset from Bmp header
    unsigned int offset = header.bf.bfOffBits;

    unsigned int image_buf[width * height];

    //define header
    fprintf(pNewFile, "#ifndef %s\n", header_name);
    fprintf(pNewFile, "#define %s\n", header_name);
    fprintf(pNewFile, "static const IMAGE_DATA %s = ", strcat(prefix, "_4bit_image"));

    fprintf(pNewFile, "{%d, %d, %d, { \n",width, height, COLOR_4);

    int k = 1;
    int j = 1;
    for( int i = 0; i < width * height; i++) { 
        
        // rgb data is stored backwards
        u_int8_t r = buf[pixel_size - (3 * i + 1)];
        u_int8_t g = buf[pixel_size - (3 * i + 2)];
        u_int8_t b = buf[pixel_size - (3 * i + 3)];

        unsigned int rgb24 = r << 16 | g << 8 | b;
        
        // save data Symmetric
        image_buf[width *  k - j] = rgb24;
        
        if(j > width - 1) {
            j = 1;
            k ++;
        }
        else
            j = j + 1;
    }

    unsigned char result[width * height];

    //define colors 
    COLOR colors[16]; 
    define_colors(colors, COLOR_4);

    compact_color(colors, image_buf, result, width * height, COLOR_4);

    // create txt file

    for(int i = 0 ; i < width * height - 2 ; ) {
        fprintf(pNewFile, "0x%x", result[i]);
        fprintf(pNewFile, "%x, ", result[i + 1]);
        i += 2;

        if(i % 16 == 0) 
            fprintf(pNewFile, "\n");
    }
    fprintf(pNewFile, "0x%x", result[width * height - 2]);
    fprintf(pNewFile, "%x", result[width * height - 1]);

    fprintf(pNewFile, "}};\n");
    fprintf(pNewFile, "#endif");
    // close file
    fclose(pFile);
    fclose(pNewFile);
}

void open_bmp_2bit(char* file_name) 
{
    FILE* pFile = fopen(file_name, "rb");
    
    if(pFile == NULL) {
        printf("cannot open file");
        return;
    }

    char* left_string = NULL;
    strtok_r(file_name, ".", &left_string);

    // check file format
    if(strcmp(left_string, "bmp")) {
        printf("Wrong file format : %s File format should be BMP", left_string);
        return;
    }

    char* prefix = strtok(file_name, ".");

    // *.bmp -> *.h
    int name_length = strlen(file_name);
    char* new_file_name = (char *)malloc(sizeof(char) * (name_length + 2));
    char* header_name = (char *)malloc(sizeof(char) * (name_length + 2));

    // failed to allocate memory
    if(new_file_name == NULL) 
    {
        printf("malloc ERROR !! \n");
    }

    //copy file name
    strcpy(new_file_name, prefix);
    strcat(new_file_name, "_2bit.h");

    define_header_name(new_file_name, header_name);


    // initialize header.bf
    BITMAPHEADER header;
    fread(&header.bf.bfType, 1, 2, pFile);
    fread(&header.bf.bfSize, 4, 1, pFile);
    fread(&header.bf.bfReserved1, 2, 1, pFile);
    fread(&header.bf.bfReserved2, 2, 1, pFile);
    fread(&header.bf.bfOffBits, 4, 1, pFile);

    int file_size = header.bf.bfSize;
    
    // initialize header.bi
    fread(&header.bi, 40, 1, pFile);
    int width = header.bi.biWidth;
    int height = header.bi.biHeight;
    int pixel_size = width * height * 3;

    fread(header.hRGB, header.bf.bfOffBits - HEADER_SIZE ,1,pFile);

    check_file_availability(&header);

    // offset , image size, bit depth, 
    u_int8_t buf[width * height * 3];
    u_int8_t dummy[4];
    unsigned char space = width % 4;
    // fread(buf, sizeof(u_int8_t), sizeof(buf), pFile);
    for(int i = 0; i < height; i++) {
        fread(buf + 3 * i * width, sizeof(u_int8_t), 3 * width, pFile);
        fread(dummy, sizeof(u_int8_t), space, pFile);
    }

    // getting pixel start offset from Bmp header
    unsigned int offset = header.bf.bfOffBits;

    unsigned int image_buf[width * height];

    FILE* pNewFile = fopen(new_file_name, "wb");

    fprintf(pNewFile, "#ifndef %s\n", header_name);
    fprintf(pNewFile, "#define %s\n", header_name);
    fprintf(pNewFile, "static const IMAGE_DATA %s = ", strcat(prefix, "_2bit_image"));

    fprintf(pNewFile, "{%d, %d, %d, { \n",width, height, COLOR_2);

    int k = 1;
    int j = 1;

    for( int i = 0; i < width * height; i++) { 
        
        // rgb data is stored backwards
        u_int8_t r = buf[pixel_size - (3 * i + 1)];
        u_int8_t g = buf[pixel_size - (3 * i + 2)];
        u_int8_t b = buf[pixel_size - (3 * i + 3)];

        unsigned int rgb24 = r << 16 | g << 8 | b;
        
        // save data Symmetric
        image_buf[width *  k - j] = rgb24;
        
        if(j > width - 1) {
            j = 1;
            k ++;
        }
        else
            j = j + 1;
    }

    unsigned char result[width * height];
    COLOR colors[4]; 
    define_colors(colors, COLOR_2); 
    compact_color(colors, image_buf, result, width * height, COLOR_2);

        unsigned char byte = result[i] << 6;
        byte = byte | (result[i+1] << 4);
        byte = byte | (result[i+2] << 2);
        byte = byte | (result[i+3]);

        fprintf(pNewFile, "0x%x, ", byte);
        i += 4;

        if(i % 32 == 0) 
            fprintf(pNewFile, "\n");
    }

    unsigned char byte = 0;
    for (int i = ((width * height) % 4 + 4) % 5, int j = 6; i > 0; i--, j -= 2) {
        byte = byte | (result[width * height - i] << j;
    }

    fprintf(pNewFile, "0x%x", byte);

    fprintf(pNewFile, "}};\n");
    fprintf(pNewFile, "#endif");
    // close file
    fclose(pFile);
    fclose(pNewFile);
}

void define_colors(COLOR* colors, enum COLOR_BIT_SIZE color_bit_size) {
    for(int i = 0; i < pow(2, color_bit_size); i++) {
        colors[i].index = i;
    }
    if(color_bit_size == COLOR_4){
        colors[0].data = BLACK;
        colors[1].data = BLUE; 
        colors[2].data = GREEN;
        colors[3].data = TURQUOISE;
        colors[4].data = RED; 
        colors[5].data = PURPLE; 
        colors[6].data = SADDLEBROWN; 
        colors[7].data = LIGHT_TREY;
        colors[8].data = GRAY;
        colors[9].data = LIGHT_BLUE; 
        colors[10].data = LIME; 
        colors[11].data = SKY_BLUE;
        colors[12].data = TOMATO; 
        colors[13].data = FUCHSIA; 
        colors[14].data = YELLOW; 
        colors[15].data = WHITE; 

    }
    else if(color_bit_size == COLOR_2) {
        colors[0].data = BLACK_2BIT;
        colors[1].data = BLUE_2BIT;
        colors[2].data = RED_2BIT;
        colors[3].data = WHITE_2BIT;
    }

    else {
        printf("wrong color bit size\n");
        exit(1);
    }
}


void compact_color(COLOR* colors, unsigned int* image_buf, unsigned char* result, unsigned int buf_size, enum COLOR_BIT_SIZE color_bit_size)
{
    unsigned int closest_color;
    unsigned char compressed_color;
    for(int i = 0; i < buf_size; i++) {
        closest_color = search_color(colors, image_buf[i], color_bit_size);
        for(int j = 0; j < pow(2, color_bit_size); j ++) {
            if(closest_color == colors[j].data){
                compressed_color = colors[j].index;
                break;
            }
        }
        result[i] = compressed_color;
    }
}

// return the shortest color
unsigned int search_color(COLOR* colors, unsigned int color, enum COLOR_BIT_SIZE color_bit_size)
{
    // unsigned int result = binary_search(colors, color, 16, 0);
    unsigned int result_idx = 0;
    unsigned int dist = rgb_square_mean(colors[0].data, color);

    for(int i = 1; i < pow(2, color_bit_size); i++) {
        unsigned int _dist = rgb_square_mean(colors[i].data, color);
        if(_dist < dist) {
            dist = _dist;
            result_idx = i;
        }
    }

    return colors[result_idx].data;
}

unsigned int rgb_square_mean(unsigned int color1, unsigned int color2) 
{
    unsigned int result = 0;
    unsigned int temp1;
    unsigned int temp2;
    for (int i = 0; i < 3; i++) {
        temp1 = color1 >> (8 * i) & 0xFF;
        temp2 = color2 >> (8 * i) & 0xFF;
        result += ((temp1 - temp2) * (temp1 - temp2));
    }
    return (unsigned int)(result / 3.0);
}

void check_file_availability(BITMAPHEADER* header)
 {
    if(header->bf.bfType != 0x4d42) {
        printf("bmp file format exception: file format is not bmp\n");
        exit(1);
    }

    if(header->bi.biBitCount != 24) {
        printf("bmp file format exception: bit count should be 24bit\n");
        exit(1);
    }

    if(header->bi.biHeight > 240) {
        printf("bmp file format exception: file height exceeded. Should be lower than 240 \n");
        exit(1);
    }

    if(header->bi.biWidth > 160) {
        printf("bmp file format exception: file width exceeded. Should be lower than 160 \n");
        exit(1);
    }
}

void define_header_name(char* file_name, char* result)
{
    for(int i = 0 ; i < strlen(file_name); i ++) {
        char it = file_name[i];
        if(it == '.') {
            result[i] = '_';
        }
        else if((it >= 'a') && (it <= 'z')) {
            result[i] = it - 32;
        }
        else {
            result[i] = it;
        }
    }
}