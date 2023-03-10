#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 100
#pragma pack(1)
typedef struct {
    unsigned char  fileMarker1; /* 'B' */
    unsigned char  fileMarker2; /* 'M' */
    unsigned int   bfSize; /* File's size */
    unsigned short unused1; /* Aplication specific */
    unsigned short unused2; /* Aplication specific */
    unsigned int   imageDataOffset; /* Offset to the start of image data */
} bmp_fileheader;

typedef struct {
    unsigned int   biSize; /* Size of the info header - 40 bytes */
    signed int     width; /* Width of the image */
    signed int     height; /* Height of the image */
    unsigned short planes;
    unsigned short bitPix; /* Number of bits per pixel = 3 * 8 (for each channel R, G, B we need 8 bits */
    unsigned int   biCompression; /* Type of compression */
    unsigned int   biSizeImage; /* Size of the image data */
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} bmp_infoheader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} RGB;

void edit(RGB*** img, bmp_fileheader* header, bmp_infoheader* info, int* pad, char *path) {
    FILE *f1 = NULL;
    int i = 0, j = 0;
    f1 = fopen(path, "rb");
    fread(&(*header), sizeof(bmp_fileheader), 1, f1);
    fread(&(*info), sizeof(bmp_infoheader), 1, f1);
    (*img) = calloc((*info).height, sizeof(RGB*));
    for (i = 0; i < (*info).height; i++)
        (*img)[i] = calloc((*info).width, sizeof(RGB));
    if (((*info).width *3) % 4 != 0) {
        *pad = 4 - (3 * (*info).width) % 4;
    }
    for (i = 0; i < (*info).height; i++) {
        for (j = 0; j < (*info).width; j++) {
            fread(&(*img)[i][j].blue, 1, 1, f1);
            fread(&(*img)[i][j].green, 1, 1, f1);
            fread(&(*img)[i][j].red, 1, 1, f1);
        }
        fseek(f1, ftell(f1) + (*pad), SEEK_SET);
    }
    fclose(f1);
}
void save(RGB** img, bmp_fileheader header, bmp_infoheader info, int pad, char *path1) {
    FILE *f2 = NULL;
    int i = 0, j = 0;
    f2 = fopen(path1, "wb");
    fwrite(&header, sizeof(bmp_fileheader), 1, f2);
    fwrite(&info, sizeof(bmp_infoheader), 1, f2);
    for (i = 0; i < info.height; i++) {
        for (j = 0; j < info.width; j++) {
            fwrite(&img[i][j].blue, 1, 1, f2);
            fwrite(&img[i][j].green, 1, 1, f2);
            fwrite(&img[i][j].red, 1, 1, f2);
        }
        for (int k = 0; k < pad; k++) {
        fputc(0x00, f2);
        }
    }
    fclose(f2);
}

void quit(RGB ***img, int pr) {
    for (int i = 0; i < pr; i++) {
        free((*img)[i]);
    }
    free((*img));
}

void insert(RGB*** img, RGB** img1, bmp_infoheader info, unsigned int y, unsigned int x, char *path2) {
    bmp_fileheader header1;
    bmp_infoheader info1;
    int pad1 = 0, ysave = 0;
    int i = 0, j = 0;
    if (x >= 0 && x < info.height && y >= 0 && y < info.width) {
        ysave = (int)y;
        edit(&img1, &header1, &info1, &pad1, path2);
        for (i = 0; i < info1.height ; i++) {
            if (x >= info.height || y >= info.width)
                break;
            for (j = 0; j < info1.width; j++) {
                if (y >= info.width || x >= info.height)
                    break;
                (*img)[x][y] = img1[i][j];
                y++;
            }
            x++;
            y = ysave;
        }
        quit(&img1, info1.height);
    }
}
#pragma pack()
void fill(RGB*** img, int height, int width, int Y, int X, RGB initc, RGB finc) {
    if (X >= 0 && X < height && Y >= 0 && Y < width) {
       if ((*img)[X][Y].red == initc.red && (*img)[X][Y].green == initc.green && (*img)[X][Y].blue == initc.blue) {
           (*img)[X][Y].red = finc.red;
           (*img)[X][Y].green = finc.green;
           (*img)[X][Y].blue = finc.blue;
           fill(img, height, width, Y, X+1, initc, finc);
           fill(img, height, width, Y, X-1, initc, finc);
           fill(img, height, width, Y+1, X, initc, finc);
           fill(img, height, width, Y-1, X, initc, finc);
       }
    }
}
void draw_line(int y1, int x1, int y2, int x2, int w, int height, int width, RGB*** img, int R, int G, int B) {
    int xmax = 0, xmin = 0, ymax = 0, ymin = 0, i = 0, j = 0, a = 0, b = 0, contor = 0, X = 0, Y = 0, k = 0;
    if (x1 > x2) {
        xmax = x1;
        xmin = x2;
    } else {
        xmax = x2;
        xmin = x1;
    }
    if (y1 > y2) {
        ymax = y1;
        ymin = y2;
    } else {
        ymax = y2;
        ymin = y1;
    }
    if (xmax - xmin < ymax - ymin) {
        a = ymin;
        b = ymax;
        contor = 0;
    } else {
        a = xmin;
        b = xmax;
        contor = 1;
    }
    if (x1 == x2) {
        for (i = ymin; i <= ymax; i++) {
            for (j = -w/2; j <= w/2; j++) {
                if (j + x1 >= 0 && j + x1 < height) {
                    for (k = -w/2; k <= w/2; k++) {
                        if (k + i >= 0 && k + i < width) {
                            (*img)[x1+j][i+k].red = R;
                            (*img)[x1+j][i+k].green = G;
                            (*img)[x1+j][i+k].blue = B;
                        }
                    }
                }
            }
        }
    } else if (y1 == y2) {
        for (i = xmin; i <= xmax; i++) {
            for (j = -w/2; j <= w/2; j++) {
                if (j + y1 >= 0 && j + y1 < width) {
                    for (k = -w/2; k <= w/2; k++) {
                        if (k + i >= 0 && k + i < height) {
                            (*img)[i+k][y1+j].red = R;
                            (*img)[i+k][y1+j].green = G;
                            (*img)[i+k][y1+j].blue = B;
                        }
                    }
                }
            }
        }
    } else if (contor == 0) {
        for (i = a; i <= b; i++) {
            X = ((i-y1)*(x2-x1)+x1*y2-x1*y1) / (y2-y1);
            for (j = -w/2; j <= w/2; j++) {
                if (j + i >= 0 && j + i < width) {
                    for (k = -w/2; k <= w/2; k++) {
                        if (k + X >= 0 && k + X < height) {
                            (*img)[X+k][i+j].red = R;
                            (*img)[X+k][i+j].green = G;
                            (*img)[X+k][i+j].blue = B;
                        }
                    }
                }
            }
        }
    } else if (contor == 1) {
        for (i = a; i <= b; i++) {
            Y = ((i-x1)*(y2-y1)+y1*x2-y1*x1) / (x2-x1);
            for (j = -w/2; j <= w/2; j++) {
                if (j + i >= 0 && j + i < height) {
                    for (k = -w/2; k <= w/2; k++) {
                        if (k + Y >= 0 && k + Y < width) {
                            (*img)[i+j][Y+k].red = R;
                            (*img)[i+j][Y+k].green = G;
                            (*img)[i+j][Y+k].blue = B;
                        }
                    }
                }
            }
        }
    }
}
void draw_rectangle(int y1, int x1, int width, int height, RGB ***img, int w, int R, int G, int B, int heig, int wid) {
    draw_line(y1, x1, y1+wid, x1, w, height, width, img, R, G, B);
    draw_line(y1+wid, x1, y1+wid, x1+heig, w, height, width, img, R, G, B);
    draw_line(y1+wid, x1+heig, y1, x1+heig, w, height, width, img, R, G, B);
    draw_line(y1, x1+heig, y1, x1, w, height, width, img, R, G, B);
}
void draw_triangle(int y1, int x1, int y2, int x2, int y3, int x3, int w, int height, int width,
 RGB*** img, int R, int G, int B) {
    draw_line(y1, x1, y2, x2, w, height, width, img, R, G, B);
    draw_line(y2, x2, y3, x3, w, height, width, img, R, G, B);
    draw_line(y3, x3, y1, x1, w, height, width, img, R, G, B);
}
int main() {
    bmp_fileheader header;
    bmp_infoheader info;
    RGB** img = NULL;
    RGB** img1 = NULL;
    RGB initc, finc;
    int height = 0, width = 0, heig = 0, wid = 0;
    unsigned int x = 0, y = 0;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
    int pad = 0, R = 0, G = 0, B = 0, w = 1, X = 0, Y = 0;
    char *path2 = NULL, *path = NULL, *path1 = NULL;
    path2 = calloc(max, sizeof(char));
    path1 = calloc(max, sizeof(char));
    path = calloc(max, sizeof(char));
    char *str = NULL, *str1 = NULL;
    str1 = calloc(max, sizeof(char));
    str = calloc(max, sizeof(char));
    while (scanf("%s", str) == 1) {
        if (strncmp(str, "edit", 4) == 0) {
            scanf("%s", path);
            edit(&img, &header, &info, &pad, path);
        }
        if (strncmp(str, "save", 4) == 0) {
            scanf("%s", path1);
            save(img, header, info, pad, path1);
        }
        if (strncmp(str, "quit", 4) == 0) {
            quit(&img, info.height);
        }
        if (strncmp(str, "insert", strlen("insert")) == 0) {
            scanf("%s", path2);
            scanf("%d %d", &y, &x);
            insert(&img, img1, info, y, x, path2);
        }
        if (strncmp(str, "set", 3) == 0) {
            scanf("%s", str1);
            if (strncmp(str1, "draw_color", strlen("draw_color")) == 0) {
                scanf("%d", &R);
                scanf("%d", &G);
                scanf("%d", &B);
            }
            if (strncmp(str1, "line_width", strlen("line_width")) == 0) {
                scanf("%d", &w);
            }
        }
        if (strncmp(str, "fill", 4) == 0) {
            scanf("%d %d", &Y, &X);
            finc.red = R;
            finc.green = G;
            finc.blue = B;
            initc.red = img[X][Y].red;
            initc.green = img[X][Y].green;
            initc.blue = img[X][Y].blue;
            fill(&img, info.height, info.width, Y, X, initc, finc);
        }
        if (strncmp(str, "draw", 4) == 0) {
            scanf("%s", str1);
            if (strncmp(str1, "line", 4) == 0) {
                scanf("%d %d %d %d", &y1, &x1, &y2, &x2);
                height = info.height;
                width = info.width;
                draw_line(y1, x1, y2, x2, w, height, width, &img, R, G, B);
            }
            if (strncmp(str1, "rectangle", strlen("rectangle")) == 0) {
                scanf("%d %d %d %d", &y1, &x1, &wid, &heig);
                height = info.height;
                width = info.width;
                draw_rectangle(y1, x1, width, height, &img, w, R, G, B, heig, wid);
            }
            if (strncmp(str1, "triangle", strlen("triangle")) == 0) {
                scanf("%d %d %d %d %d %d", &y1, &x1, &y2, &x2, &y3, &x3);
                height = info.height;
                width = info.width;
                draw_triangle(y1, x1, y2, x2, y3, x3, w, height, width, &img, R, G, B);
            }
        }
    }
    return 0;
}
