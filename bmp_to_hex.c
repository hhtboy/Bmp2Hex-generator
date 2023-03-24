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
    BLACK_4BIT = 0x000000,
    BLUE_4BIT = 0x0000FF,
    GREEN_4BIT = 0x008000,
    TURQUOISE_4BIT = 0x40E0D0,
    RED_4BIT = 0xFF0000,
    PURPLE_4BIT = 0x800080,
    SADDLEBROWN_4BIT = 0x8B4513,
    LIGHT_TREY_4BIT = 0xD3D3D3,
    GRAY_4BIT = 0x808080,
    LIGHT_BLUE_4BIT = 0xADD8E6,
    LIME_4BIT = 0x00FF00,
    SKY_BLUE_4BIT = 0x87CEFA,
    TOMATO_4BIT = 0xFF6347,
    FUCHSIA_4BIT = 0xFF00FF,
    YELLOW_4BIT = 0xFFFF00,
    WHITE_4BIT = 0xFFFFFF
};


// 색상의 순서 변경
unsigned int palette_4bit_order[] = {
    BLACK_4BIT,
    BLUE_4BIT,
    GREEN_4BIT ,
    TURQUOISE_4BIT,
    RED_4BIT,
    PURPLE_4BIT ,
    SADDLEBROWN_4BIT,
    LIGHT_TREY_4BIT,
    GRAY_4BIT,
    LIGHT_BLUE_4BIT,
    LIME_4BIT,
    SKY_BLUE_4BIT,
    TOMATO_4BIT,
    FUCHSIA_4BIT,
    YELLOW_4BIT,
    WHITE_4BIT
};

enum PALETTE_3BIT {
    BLACK_3BIT = 0x000000,
    BLUE_3BIT = 0x0000FF,
    LIME_3BIT = 0x00FF00,
    CYAN_3BIT = 0x00FFFF,
    RED_3BIT = 0xFF0000,
    MAGENTA_3BIT = 0xFF00FF,
    YELLOW_3BIT = 0xFFFF00,
    WHITE_3BIT = 0xFFFFFF
};

unsigned int palette_3bit_order[] = {
    BLACK_3BIT,
    BLUE_3BIT,
    LIME_3BIT,
    CYAN_3BIT,
    RED_3BIT,
    MAGENTA_3BIT,
    YELLOW_3BIT,
    WHITE_3BIT
};

enum PALETTE_2BIT {
    BLACK_2BIT = 0x000000,
    BLUE_2BIT = 0x0000FF,
    RED_2BIT = 0xFF0000,
    WHITE_2BIT = 0xFFFFFF
};

unsigned int palette_2bit_order[] = {
    BLACK_2BIT,
    BLUE_2BIT,
    RED_2BIT,
    WHITE_2BIT
};

enum PALETTE_1BIT {
    BLACK_1BIT = 0x000000,
    WHITE_1BIT = 0xFFFFFF
};

unsigned int palette_1bit_order[] = {
    BLACK_2BIT,
    WHITE_2BIT
};

enum COLOR_BIT_SIZE {
    COLOR_4 = 4,
    COLOR_2 = 2
};

static const int HEADER_SIZE = 54;
static const int lcd_width = 160;
static const int lcd_height = 240;

void open_bmp(char* file_name, unsigned char color_bit_size);
void compact_color(COLOR* colors, unsigned int* image_buf, unsigned char* result, unsigned int buf_size, enum COLOR_BIT_SIZE color_bit_size);
unsigned int search_color(COLOR* colors, unsigned int color, unsigned int color_bit_size); // find the closest color in color template, and return 24bit color
unsigned int rgb_square_mean(unsigned int color1, unsigned int color2); // returns the mean of squared values
void define_colors(COLOR* colors, unsigned int color_bit_size);
void check_file_availability(BITMAPHEADER* header);
void define_header_name (char* file_name, char* header_name);

int main() 
{
    printf("<Usage> \n");
    printf("1. Enter bmp file name.\n");
    printf("2. Enter bit size of colors : 1~4 \n");
    printf("File format must be 24bit bmp.\nImage size must be smaller than 160 X 240. \nLength of bmp file name must be shorter than 30.\n");

    char file_name[30];
    unsigned short bit_size;
    scanf("%s", file_name);
    fflush(stdin);

    scanf("%hd", &bit_size);
    fflush(stdin);

    open_bmp(file_name, bit_size);

    if(bit_size != 4 && bit_size != 3 && bit_size != 2 && bit_size != 1){
        printf("ERROR : unexpected input value. Bit size of colors must be 1 ~ 4");
        exit(1);
    }
}

void open_bmp(char* file_name, unsigned char color_bit_size) {
    FILE* pFile = fopen(file_name, "rb");
    
    if(pFile == NULL) {
        printf("cannot open file");
        return;
    }

    char* left_string = NULL;
    strtok_r(file_name, ".", &left_string);


    char* prefix = strtok(file_name, ".");

    // *.bmp -> *.h
    int name_length = strlen(file_name);
    char* new_file_name = (char *)malloc(sizeof(char) * (name_length + 2));
    char* color_bit_size_str = (char *)malloc(sizeof(char) * 2);
    char* header_name = (char *)malloc(sizeof(char) * (name_length + 2));

    // failed to allocate memory
    if(new_file_name == NULL) {
        printf("malloc ERROR !! \n");
    }

    //copy file name
    strcpy(new_file_name, prefix);
    sprintf(color_bit_size_str, "_%d", color_bit_size);
    strcat(new_file_name, color_bit_size_str);
    strcat(new_file_name, "bit.h");
    printf("file generated : %s\n", new_file_name);

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

    fprintf(pNewFile, "#ifndef %s\n", header_name);
    fprintf(pNewFile, "#define %s\n", header_name);
    fprintf(pNewFile, "static const IMAGE_DATA %s_%d%s = ", prefix, color_bit_size, "bit_image");

    fprintf(pNewFile, "{%d, %d, %d, { \n",width, height, color_bit_size);

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
    unsigned char color_size = pow(2, color_bit_size);
    COLOR* colors = (COLOR *)malloc(sizeof(COLOR) * color_size);
    define_colors(colors, color_bit_size);

    compact_color(colors, image_buf, result, width * height, color_bit_size);

    // create txt file

    if(color_bit_size == 3) {
        int total_num = (int)(width * height * 3.0 / 24.0); //내림 
        int idx = 0;
        for(int i = 0; i < total_num; i++) {
            unsigned int temp = 0;
            for(int k = 7; k >= 0; k--) {
                temp |= result[idx++] << k * 3;
            }
            if(i % 3 == 0){
                fprintf(pNewFile, "\n");
            }
            fprintf(pNewFile, "0x%x, ", temp >> 16);
            fprintf(pNewFile, "0x%x, ", temp >> 8 & 0xFF);
            fprintf(pNewFile, "0x%x, ", temp & 0xFF);
            
        }
        int remain_num = (width * height * 3) % 24;
        unsigned int temp = 0;
        for(int i = 0, k = 7; i < (int)(remain_num / 3); i++, k--){
            temp |= result[idx++] << k * 3;
        }
        int quotient = (int)(remain_num / 8);
        for(int i = 0; i < quotient; i++) {
            fprintf(pNewFile, "0x%x ", (temp >> 8 * (2 - i)) & 0xFF );
        }
        if (remain_num % 8 != 0){
            fprintf(pNewFile, ",0x%x ", (temp >> 8 * (2 - quotient)) & 0xFF );
        }
    }
    else {
        switch (color_bit_size)
        {
        case 4:
            /* code */
            for(int i = 0 ; i < width * height - 2 ; ) {
                fprintf(pNewFile, "0x%x", result[i]);
                fprintf(pNewFile, "%x, ", result[i + 1]);
                i += 2;

                if(i % 16 == 0) {
                    fprintf(pNewFile, "\n");
                }
            }
            // fprintf(pNewFile, "0x%x", result[width * height - 2]);
            // fprintf(pNewFile, "%x", result[width * height - 1]);
            break;

        case 2:
            for(int i = 0 ; i < width * height - 4 ; ) {
                unsigned char byte = result[i] << 6;
                byte = byte | (result[i+1] << 4);
                byte = byte | (result[i+2] << 2);
                byte = byte | (result[i+3]);

                fprintf(pNewFile, "0x%x, ", byte);
                i += 4;

                if(i % 32 == 0) 
                    fprintf(pNewFile, "\n");
            }
            break;
        
        case 1:
            for(int i = 0 ; i < width * height - 8 ; ) {
                unsigned char byte = 0;
                for(int j = 0; j < 8; j++) {
                    byte = byte | (result[i+j] << (7 - j));
                }
                fprintf(pNewFile, "0x%x, ", byte);
                i += 8;
                if(i % 64 == 0) {
                    fprintf(pNewFile, "\n");
                }
            }
            break;

        default:
            break;
        }

        // 공통 로직
        unsigned char byte = 0;
        int j;
        int i;
        int noc = 8 / color_bit_size; // number of colors in 1Byte
        if(width * height % noc == 0) {
            i = 8 / color_bit_size; // 8bit / 2bit = 4
        }
        else {
            i =(width * height) % noc ;
        }
        for (j = i - 1; i > 0; i--, j--) {
            byte = byte | (result[width * height - i] << j * color_bit_size); // 2bit
        }

        fprintf(pNewFile, "0x%x", byte);
    }


    fprintf(pNewFile, "}};\n");
    fprintf(pNewFile, "#endif");
    // close file
    fclose(pFile);
    fclose(pNewFile);

    //메모리 해제
    free(new_file_name);
    free(color_bit_size_str);
    free(header_name);
    free(colors);

}

void define_colors(COLOR* colors, unsigned int color_bit_size) {
    //index 값 채우기
    for(int i = 0; i < pow(2, color_bit_size); i++) {
        colors[i].index = i;
    }

    switch (color_bit_size)
    {
    case 4:
        for(int i = 0; i < pow(2, color_bit_size); i++) {
            colors[i].data = palette_4bit_order[i];
        } 
        break;
    
    case 3:
        for(int i = 0; i < pow(2, color_bit_size); i++) {
            colors[i].data = palette_3bit_order[i];
        }
        break;

    case 2:
        for(int i = 0; i < pow(2, color_bit_size); i++) {
            colors[i].data = palette_2bit_order[i];
        }
        break;
    
    case 1:
        for(int i = 0; i < pow(2, color_bit_size); i++) {
            colors[i].data = palette_1bit_order[i];
        }
        break;
    
    default:
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