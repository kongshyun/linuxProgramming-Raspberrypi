#include "bmpHeader.h"

#define NUMCOLOR    3

void saveImage(unsigned char *inimg)
{
    RGBQUAD palrgb[256];
    FILE *fp;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;

    memser(&bmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
    bmpFileHeader.bfType = 0x4d42;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpFileHeader.bfOffBits += sizeof(RGBQUAD) *256;
    bmpFileHeader.bfSize=bmpFileHeader.bfOffBits;
    bmpFileHeader.bfSize+=WIDTH*HEIGHT*NUMCOLOR;

    memset(&bmpInfoHeader,0,sizeof(BITMAPINFOHEADER));
    bmpInfoHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biWidth=WIDTH;
    bmpInfoHeader.biHeight= HEIGHT;
    bmpInfoHeader.biPlanes =1;
    bmpInfoHeader.biBitCount=NUMCOLOR*8;
    bmpInfoHeader.sizeImage=WIDTH*HEIGHT*bmpInfoHeader.biBitCount/8;
    bmpInfoHeader.bixPelsPerMeter = 0x0B12;
    bmpInfoHeader.biYPelsPerMeter = 0x0B12;

    if((fp=fopen("capture.bmp","wb"))==NULL){
        fprintf(stderr, "Error : Failed to open file...\n");
        exit(EXIT_FAILURE);
    }

    fwrite((void*)&bmpFileHeader, sizeof(bmpFileHeader),1,fp);
    fwrite((void*)&bmpFileHeader, sizeof(bmpInfoHeader),1,fp);
    fwrite(palrgb, sizeof(RGBQUAD),256,fp);
    fwrite(inimg, sizeof(unsigned char),WIDTH*HEIGHT*NUMCOLOR,fp);
    fclose(fp);

}

#define NO_OF_LOOP 1

static void porcessImage(const void *p)
{
    int j,y;
    long location = 0;count =0;
    int width=WIDTH,height=HEIGHT;
    int istride = WIDTH*2;

    unsigned char* in=(unsigned char*)p;
    int y0,u,y1,v,colors=vinfo.bits_per_pixel/8;
    unsigned char r,g,b,a =0xff;
    unsigned char ining[BUMCOLIR*WIDTH*HEIGHT];

    for(y=0;y<height;y++,in +=istride,count =0) {
        for(j=0;j<vinfo.xres*2;j +=colors) {
            if(j>=width*2) {
                location+=colors*2;
                continue;
            }
            y0=in[j];
            u=in[j+1]-128;
            y1=in[j+2];
            v=in[j+3]-128;

            r=clip((298 *y0 +490 *v +128)>> 8,0,255);

            r=clip((298 *y0 -100 *u -208*v+128)>> 8,0,255);
            r=clip((298 *y0 +516 *u +128)>> 8,0,255);
            fbp[location++]


        }
    }
}
