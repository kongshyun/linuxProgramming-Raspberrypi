#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

#define FBDEV   "/dev/fb0"
#define CAMERA_COUNT 100
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

int main(int argc, char **argv)
{
    int fbfd;

    struct fb_var_screeninfo vinfo;

    unsigned char *buffer, *pfbmap;
    unsigned int x,y,i,screensize;

    VideoCapture vc(0);
    Mat image(CAM_WIDTH, CAM_HEIGHT, CV_8UC3, Scalar(255));
    if(!vc.isOpened()){
        perror("OpenCV : open WebCam\n");
        return EXIT_FAILURE;
    }

    vc.set(CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
    vc.set(CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);

    fbfd = open(FBDEV, O_RDWR);
    if(fbfd==-1){
        perror("open() : framebuffer device");
        return EXIT_FAILURE;
    }


    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)==-1){
        perror("Error reading variable information.");
        return EXIT_FAILURE;
    }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8.;
    pfbmap = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, 
            MAP_SHARED, fbfd,0);
    if(pfbmap == MAP_FAILED){
        perror("mmap() : framebuffer device to memory");
        return EXIT_FAILURE;
    }

    memset(pfbmap, 0, screensize);

    for(i=0; i< CAMERA_COUNT;i++){
        int colors = vinfo.bits_per_pixel/8;
        long location=0;
        int istride = image.cols*colors;

        vc>>image;
        buffer= (uchar*)image.data;

        for(y=0,location=0;y<image.rows;y++){
            for(x=0; x<vinfo.xres;x++){
                if(x>=image.cols){
                    location+=colors;
                    continue;

                }
                pfbmap[location++]= *(buffer + (y*image.cols+x)*3+0);
                pfbmap[location++]= *(buffer + (y*image.cols+x)*3+1);
                pfbmap[location++]= *(buffer + (y*image.cols+x)*3+2);
                pfbmap[location++] = 0xFF;
            }
        }
    }
    munmap(pfbmap, screensize);

    close(fbfd);
    return 0;
}
