#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main()
{
    Mat image1 = imread("mandrill.jpg", IMREAD_COLOR);
    Mat grayscale_image;

    cvtColor(image1, grayscale_image, COLOR_BGR2GRAY);
    imshow("image1_original", image1);
    imshow("grayscale_image", grayscale_image);
    
    waitKey(0);
    return 0;


    for(i=0;i<height;i++){
        index = (height-i-1)*size;
        for(j=0;j<width;j++){
            int value;
            r=(float)inimg[index+3*j +2];
            g=(float)inimg[index+3*j +1];
            b=(float)inimg[index+3*j +0];
            gray=(r*0.3F) + (g*0.59F) + (b*0.11F);

            outimg[index+3*j]= outimg[index+3*j+1]=outimg[index+3*j+2]=gray;
        }
    }
}
