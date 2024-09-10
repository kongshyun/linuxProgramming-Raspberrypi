/*
 * 24.09.09 monday
 * page.466
 * make " French flag!!! " using drawfacemmap()
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h> /*FAST!!*/ 


#define FBDEVICE    "/dev/fb0"
//#define COL 640

typedef unsigned char ubyte;

struct fb_var_screeninfo vinfo;

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b){
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}


static void drawpoint(int fd, int x, int y, ubyte r, ubyte g, ubyte b)
{
    ubyte a= 0xFF;

    int offset = (x+y*vinfo.xres) * vinfo.bits_per_pixel/8.;
    lseek(fd,offset,SEEK_SET);
    unsigned short pixel;
    pixel= makepixel(r,g,b);

    write(fd,&pixel,2);
    
}

static void drawline(int fd, int start_x, int end_x, int y, ubyte r, ubyte g, ubyte b)
{
    ubyte a=0xFF;


    for(int x=start_x; x<end_x;x++){
        int offset = (x+y*vinfo.xres)*vinfo.bits_per_pixel/8.;
        unsigned short pixel;
        pixel=makepixel(r,g,b);
        write(fd,&pixel,2);
        lseek(fd,offset,SEEK_SET);
    }
}



static void drawcircle(int fd, int center_x, int center_y, int radius, ubyte r, ubyte g, ubyte b)
{
    int x=radius, y=0;
    int radiusError =1-x;

    unsigned short pixel;
    pixel=makepixel(r,g,b);
    write(fd,&pixel,2);

    while(x>=y) {
        drawpoint(fd, x+center_x, y+center_y, r,g,b);
        drawpoint(fd, y+center_x, x+center_y, r,g,b);
        drawpoint(fd, -x+center_x, y+center_y, r,g,b);
        drawpoint(fd, -y+center_x, x+center_y, r,g,b);
        drawpoint(fd, -x+center_x, -y+center_y, r,g,b);
        drawpoint(fd, -y+center_x, -x+center_y, r,g,b);
        drawpoint(fd, x+center_x, -y+center_y, r,g,b);
        drawpoint(fd, y+center_x, -x+center_y, r,g,b);

        y++;
        if (radiusError<0) {
            radiusError += 2*y +1;

        }else {
            x--;
            radiusError +=2 *(y-x+1);
        }
    }
}

static void drawface(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b)
{
    ubyte a=0xFF;
    if(end_x ==0) end_x = vinfo.xres;
    if(end_y ==0) end_y = vinfo.yres;

    for(int x=start_x; x<end_x; x++){
        for( int y=start_y ; y<end_y;y++ ){
            int offset = (x+y*vinfo.xres)*vinfo.bits_per_pixel/8.;
            lseek(fd,offset,SEEK_SET);
            unsigned short pixel;
            pixel = makepixel(r,g,b);

            write(fd,&pixel,2); // 2byte
        
        
    }
}

static void drawfacemmap(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b)
{
    ubyte a=0xFF;
    unsigned short *pfb;

    int color = vinfo.bits_per_pixel/8.;

    if(end_x ==0) end_x = vinfo.xres;
    if(end_y ==0) end_y = vinfo.yres;

    pfb=(unsigned short *)mmap(0,vinfo.xres * vinfo.yres *color,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

    //for(int x=start_x; x<end_x*color; x+=color){
    for(int x=start_x; x<end_x; x++){
        for( int y=start_y ; y<end_y;y++ ){
            unsigned short pixel;
            pixel = makepixel(r,g,b);
            int xline = vinfo.xres * y;
            *((pfb+x)+xline) =pixel;
//            *(pfb+y*vinfo)           
            //*(pfb+ (x+0) + y*vinfo.xres*color) =b;
            //*(pfb+ (x+1) + y*vinfo.xres*color) =g;
            //*(pfb+ (x+2) + y*vinfo.xres*color) =r;
            //*(pfb+ (x+3) + y*vinfo.xres*color) =a;
        
        }
    }
    munmap(pfb, vinfo.xres * vinfo.yres * color); 
   /*
    unsigned short *pfb;

    pfb=(unsigned short *)mmap(0,640*480*2, PROT_READ | PROT_WRITE, \MAP_SHARED,fd,0);
    for(int x=0;x<100;x++){
        *(pfb +x*640 +100) = pixel;
    }*/
    
}

/*    
static void drawFrance(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b)
{
    int color
    
    int YPOS1=start_y,YPOS2=end_y;
    int XPOS1=start_x,XPOS2=end_x;
    for(int t=YPOS1;t<YPOS2;t++){
        for(int tt=XPOS1;tt<XPOS2;tt++){
            offset=(t*XPOS2 +tt)*2;
            if(lseek(fd, offset, SEEK_SET)<0){
                printf("ERROR fbdev lseek\n");
                exit(1);
            }
            //write(fd,&pixel,2);
            *(fpb + (x+0) + y*vinfo.xres*color) =b;
            
        }
    }
    munmap(pfb, vinfo.xres * vinfo.yres *color);
}
*/

int main(int argc, char **argv)
{
    int fbfd,status, offset;

    fbfd = open(FBDEVICE,O_RDWR);
    if(fbfd<0){
        perror("Error");
 		return -1;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO,&vinfo)<0){
        perror("Error reading variable information");
        return -1;
    }

//    drawface(fbfd, 0,0,0,0,255,192,203);
  /* 
    drawfacemmap(fbfd,0,0,0,0,255,255,0);
    drawpoint(fbfd,50,50,255,0,0);
    drawpoint(fbfd,500,500,255,0,0);
    drawpoint(fbfd,150,150,0,0,255);
    drawline(fbfd, 0,100,200,255,255,255);
    drawline(fbfd, 0,200,100,255,0,0);
   
    drawcircle(fbfd, 200,200,100,0,0,20);
    drawcircle(fbfd, 300,300,200,0,0,20);
*/

    int xresW=vinfo.xres/3;


//    drawfacemmap(fbfd,0,0,vinfo.xres,vinfo.yres,0,0,255);
    drawfacemmap(fbfd,  xresW*0,0,    xresW*1,0      ,0,0,255);
    drawfacemmap(fbfd,  xresW*1,0,    xresW*2,0,      255,255,255);
    drawfacemmap(fbfd,  xresW*2,0,    xresW*3,0,      255,0,0);
   
    //munmap(pfb, vinfo.xres * vinfo.yres *color);
    close(fbfd);
    return 0;
}

