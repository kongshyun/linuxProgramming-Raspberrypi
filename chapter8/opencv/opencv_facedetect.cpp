#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <opencv2/core/core.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#define FBDEV 			  "/dev/fb0"
#define CAMERA_COUNT 	100 // 카메라 지속 시간
#define CAM_WIDTH 		640
#define CAM_HEIGHT 		480

using namespace cv;

const static char* cascade_name =
"/usr/share/opencv4/haarcascades//haarcascade_frontalface_alt.xml";
typedef unsigned char ubyte;
int main(int argc, char **argv)
{
    int fbfd;

    /* 프레임 버퍼 정보 처리를 위한 구조체 */
    struct fb_var_screeninfo vinfo;
    unsigned char *buffer;
    unsigned short *pfbmap;//unsigned short 로 바꿔줌.
    unsigned int x, y, i, j, screensize;
    VideoCapture vc(0); /* 카메라를 위한 변수 */
    CascadeClassifier cascade;
    Mat frame(CAM_WIDTH, CAM_HEIGHT, CV_8UC3, Scalar(255));
    Point pt1, pt2;

    if(!cascade.load(cascade_name)){
        perror("load()");
        return EXIT_FAILURE;
    }

    vc.set(CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
    vc.set(CAP_PROP_FRAME_WIDTH, CAM_HEIGHT);
    fbfd = open(FBDEV, O_RDWR);
        if(fbfd == -1) {
            perror("open() : framebuffer device");
            return EXIT_FAILURE;
        }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information.");
        return EXIT_FAILURE;
    }

    screensize = vinfo.xres* vinfo.yres *vinfo.bits_per_pixel/8.;
    pfbmap= (unsigned short *) mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd,0); // unsigned short로 바꿔줌!!!
    if(pfbmap==(unsigned short*)-1) { // unsigned short로 바꿔줌

        perror("mmap() : framebuffer device to memory");
        return EXIT_FAILURE;
    }

    memset(pfbmap, 0,screensize);
    for(i = 0; i < CAMERA_COUNT; i++) {
        int colors = vinfo.bits_per_pixel/8;
        long location = 0;
        int istride = frame.cols*colors;

        /* 이미지의 폭을 넘어가면 다음 라인으로 내려가도록 설정한다. */
        vc >> frame; /* 카메라로부터 한 프레임의 영상을 가져온다. */
        Mat image(CAM_WIDTH, CAM_HEIGHT, CV_8UC1, Scalar(255));
        cvtColor(frame, image,COLOR_BGR2GRAY);

        std::vector<Rect> faces;
        cascade.detectMultiScale(image, faces, 1.1 ,2,0 |CASCADE_SCALE_IMAGE, Size(30,30));

        for(j=0;j<faces.size();j++){
            pt1.x = faces[j].x; pt2.x = (faces[j].x + faces[j].width);
            pt1.y = faces[j].y; pt2.y = (faces[j].y + faces[j].height);

            rectangle(frame, pt1, pt2, Scalar(255,0,0),3,8);
        }
        buffer = (uchar*)frame.data;

        for(y = 0, location = 0; y < frame.rows; y++) {
            for(x = 0; x < vinfo.xres; x++) {
                /* 화면에서 이미지를 넘어서는 빈 공간을 처리한다. */
                if(x >= frame.cols) {
                    location++; // location++로 바꿔줌
                    continue;
                }
                ubyte b = *(buffer+(y*image.cols+x)*3+0);
                ubyte g = *(buffer+(y*image.cols+x)*3+1);
                ubyte r = *(buffer+(y*image.cols+x)*3+2);

                pfbmap[location++] = ((r>>3)<<11)|((g>>2)<<5)|(b>>3); 
           }
        }
    }
    /*사용이 끝난 자원과 메모리를 해제한다.*/
    munmap(pfbmap, screensize);
    close(fbfd);
    return 0;



}
