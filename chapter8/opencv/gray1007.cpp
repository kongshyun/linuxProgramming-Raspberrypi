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
}
