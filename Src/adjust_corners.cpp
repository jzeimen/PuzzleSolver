#include "opencv2/opencv.hpp"

#include "adjust_corners.h"

#define DEFAULT_SCALE_FACTOR 3.0f
#define UNSCALED_CIRCLE_SIZE 4

void ce_mouse_callback(int event, int x, int y, int flags, void* userdata);

// GUI for manually editing the corners of a puzzle piece. Implemented using OpenCV highgui.
class corner_editor {
public:
    std::string& window_name;
    float scale_factor;
    std::vector<cv::Point2f>& original_corners;
    std::vector<cv::Point2f>& edited_corners;
    bool verbose;
    
    cv::Mat& image;
    cv::Mat resized;
    cv::Mat rendered;
    std::vector<cv::Point2f> scaled_corners;
    int circle_size;
    int corner_index;
    bool edited;

    
    corner_editor(std::string& window_name, cv::Mat& image, float scale_factor, std::vector<cv::Point2f>& original_corners, std::vector<cv::Point2f>& edited_corners, bool verbose) :
        image(image), window_name(window_name), scale_factor(scale_factor), original_corners(original_corners), edited_corners(edited_corners), verbose(verbose) 
    {
        corner_index = -1;
        edited = false;
       
        init_scaled_corners(original_corners);
    }
    

    void init_scaled_corners(std::vector<cv::Point2f>& corners) {
        circle_size = image.size().width * scale_factor / 50;

        cv::resize(image, resized, cv::Size(), scale_factor, scale_factor, cv::INTER_CUBIC);
        rendered = resized.clone();
        
        scaled_corners.clear();
        for (uint i = 0; i < std::min((size_t)4, corners.size()); i++) {
            scaled_corners.push_back( corners[i] * scale_factor);
        }
        
        // If original_corners was short, add new ones
        if (scaled_corners.size() < 4) {
            float newx = resized.size().width / 2.0f;
            float newy = resized.size().height / 2.0f;
            
            for (uint i = scaled_corners.size(); i < 4; i++) {
                scaled_corners.push_back( cv::Point2f(newx, newy));
                newx = newx + circle_size * 4.0f;
            }
        }        
    }

    void adjust_scale( float adjustment) {
        float new_scale =  scale_factor + adjustment;
        if (new_scale < 0.25) {
            new_scale = 0.25;
        }
        if (new_scale == scale_factor) {
            return;
        }
        
        std::vector<cv::Point2f> corners;
        for (uint i = 0; i < scaled_corners.size(); i++) {
            cv::Point2f c = scaled_corners[i];
            corners.push_back(cv::Point2f(c.x / scale_factor, c.y / scale_factor));
        }
        scale_factor = new_scale;
        init_scaled_corners(corners);
        render_circles();   
        std::cout << "Scale factor is now " << scale_factor << std::endl;
    }
    
    bool edit() {

        render_circles();
        
        cv::namedWindow(window_name);    
        cv::setMouseCallback(window_name, ce_mouse_callback, this);
        cv::imshow(window_name, rendered);
        
        bool done = false;
        do {
            int c = cv::waitKey(0);
            // std::cout << c << std::endl;
            switch (c) {
                case -1:  // User probably clicked on the "x" in the window title bar
                case 13:  // Return key
                case 141: // Enter key
                case 'n': // "next"
                    done = true;
                    break;
                case 'r': // "reset"
                    if (corner_index == -1) {
                        reset_circles();
                    }
                    break;
                case '-':
                    adjust_scale(-0.25);
                    break;
                case '+':
                case '=':
                    adjust_scale(0.25);
                    break;                    
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
        
        if (edited) {
            edited_corners.clear();
            for (uint i = 0; i < 4; i++) {
                cv::Point2f c = scaled_corners[i];
                edited_corners.push_back(cv::Point2f(c.x / scale_factor, c.y / scale_factor));
            }
        }
        return edited;
    }
    
    void reset_circles() {
        edited = false;
        init_scaled_corners(original_corners);
        render_circles();
    }
    
    void mouse_down(int x, int y) {
        for (uint i = 0; i < scaled_corners.size(); i++) {
            int cx = scaled_corners[i].x;
            int cy = scaled_corners[i].y;
            if (cx - circle_size < x &&  x < cx + circle_size && cy - circle_size < y && y < cy + circle_size) {
               corner_index = i;
               if (verbose) {
                   std::cout << "Click in circle #" << i << std::endl;
               }
            }
        }
    }
    
    void mouse_move(int x, int y) {
        if (corner_index == -1) {
            return;
        }
        scaled_corners[corner_index].x = x;
        scaled_corners[corner_index].y = y; 
        render_circles();
    }
    
    void mouse_up(int x, int y) {
        if (corner_index == -1) {
            return;
        }
        
        scaled_corners[corner_index].x = x;
        scaled_corners[corner_index].y = y;        
        render_circles();
        edited = true;
        corner_index = -1;
    }    
    
private:
    void render_circles() {
        resized.copyTo(rendered);
        for(uint i = 0; i < scaled_corners.size(); i++ ) {
            circle( rendered, scaled_corners[i], circle_size, cv::Scalar(0,0,255), 2, 8, 0 );
        }       
        cv::imshow(window_name, rendered);
    }    
}; 

void ce_mouse_callback(int event, int x, int y, int flags, void* userdata) {
    corner_editor* data = (corner_editor*)userdata;
    
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        if (data->verbose) {
            std::cout << "Left mouse down - position (" << x << ", " << y << ")" << std::endl;
        }
        data->mouse_down(x, y);
    }
    else if  ( event == cv::EVENT_LBUTTONUP )
    {
        if (data->verbose) {
            std::cout << "Left mouse up - position (" << x << ", " << y << ")" << std::endl;
        }
        data->mouse_up(x, y);
    }    
    else if ( event == cv::EVENT_MOUSEMOVE )
    {
        data->mouse_move(x, y);
        // std::cout << "Mouse move - position (" << x << ", " << y << ")" << std::endl;
    }    
}


bool adjust_corners(std::string& window_name, cv::Mat& image, float scale_factor, std::vector<cv::Point2f>& original_corners, std::vector<cv::Point2f>& edited_corners, bool verbose)
{
    corner_editor editor(window_name, image, scale_factor, original_corners, edited_corners, verbose);
    return editor.edit();
}

/*
int main(int argc, char * argv[])
{
      
    std::vector<cv::Point2f> corners;
    corners.push_back(cv::Point2f(180.021484,163.588364));
    corners.push_back(cv::Point2f(14.8587542,61.6872482));
    corners.push_back(cv::Point2f(22.5145721,173.08255));
//    corners.push_back(cv::Point2f(132.522491,54.6961975));
    
    std::string filename = "color-003-002-0106.png";
    cv::Mat image = cv::imread(filename);
    if (!image.data) {
        return -1;
    }
    
    std::vector<cv::Point2f> edited_corners;
    
    
    bool edited = edit_corners(filename, image, 3.0f, corners, edited_corners, false);
    if (edited) {
        // re-invoke the editor to check that edited_corners contains the correct data
        // i.e., this window should show the corners in the edited (not original) locations.
        std::vector<cv::Point2f> edited_corners2;
        edit_corners(filename, image, 3.0f, edited_corners, edited_corners2, false);
    }
    
    return 0;
}
*/