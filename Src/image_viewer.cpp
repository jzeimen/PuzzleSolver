#include "image_viewer.h"


#if OPENCV_VERSION_MAJOR == 2
// Mimic cv::rotate(src,dest,rotation_code) which is available starting in OpenCV 3.x 
void cv_rotate(const cv::Mat& image, cv::Mat& dest, int rotation_code)
{
  switch (rotation_code) {
  case ROTATE_90_CLOCKWISE:
    cv::flip(image.t(), dest, 1);
    break;
  case ROTATE_180:
    cv::flip(image, dest, -1);
    break;
  case ROTATE_90_COUNTERCLOCKWISE:
    cv::flip(image.t(), dest, 0);
    break;
  default:
    dest = image.clone();
    break;
  }
}
#endif

// Initial width, the window is resizable
#define INITIAL_IMAGE_WINDOW_WIDTH 768

// Show one or more images in a GUI window
class image_viewer {
public:
    std::string window_name;
    std::vector<cv::Mat> images;
    cv::Mat rendered;
    int rotation_code;
    int img_index;

    
    image_viewer(std::string window_name, std::vector<cv::Mat> images) {
        this->window_name = window_name;
        this->images = images;
        this->rotation_code = 3; // 0 = 90, 1 = 180, 2 = 270, 3 = 0
        this->img_index = 0;
    }
    
    void render() {
        if (rotation_code == 3) {
            rendered = images[img_index].clone();
        } else {
            compat_cv_rotate(images[img_index], rendered, rotation_code);
        }
        cv::imshow(window_name, rendered);        
    }
    
    void view() {

        float aspect = (float)images[0].size().width / images[0].size().height;
    
        int height = INITIAL_IMAGE_WINDOW_WIDTH / aspect;
    
        cv::namedWindow(window_name, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name, INITIAL_IMAGE_WINDOW_WIDTH, height);
    
        render();
        
        bool done = false;
        do {
            int c = cv::waitKey(0);
            // std::cout << c << std::endl;
            switch (c) {
                case -1:  // User probably clicked on the "x" in the window title bar
                case 13:  // Return key
                case 141: // Enter key
                case 'n': // "next"
                case 'q': // "quit"
                    done = true;
                    break;
                case 'r': // "rotate"
                    rotation_code = (rotation_code + 1) % 4;
                    render();
                    break;
                case 'R': // "rotate"
                    rotation_code = (rotation_code + 3) % 4;
                    render();
                    break;                    
                case 't': // "toggle"
                    img_index = (img_index + 1) % images.size();
                    render();
                    break;
                case 'T': // "toggle"
                    img_index = (img_index + images.size() - 1) % images.size();
                    render();
                default:
                    break;
            }
        }
        while (!done);
        try {
            cv::destroyWindow(window_name);
        }
        catch (cv::Exception x) {
            // Ignore
        }
        

    }
   
}; 

void show_image(std::string window_name, cv::Mat& image1) {
    
    std::vector<cv::Mat> images;
    images.push_back(image1);
    image_viewer viewer(window_name, images);
    viewer.view();
}

void show_images(std::string window_name, cv::Mat& image1, cv::Mat& image2) {
    std::vector<cv::Mat> images;
    images.push_back(image1);
    images.push_back(image2);
    image_viewer viewer(window_name, images);
    viewer.view();    
}




