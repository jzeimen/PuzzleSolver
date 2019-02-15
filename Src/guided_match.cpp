#include "opencv2/opencv.hpp"

#include <vector>
#include "compat_opencv.h"
#include "guided_match.h"
#include "utils.h"

void gm_mouse_callback(int event, int x, int y, int flags, void* userdata);

std::vector<cv::Point2f> rotate(cv::Point2f center, std::vector<cv::Point> cont, double angle) {
    
    std::vector<cv::Point2f> ret_contour;
    angle = angle * M_PI / 180.0;
    
    for(std::vector<cv::Point>::iterator i = cont.begin(); i != cont.end(); i++ ) {
        
        cv::Point2f t(i->x - center.x, i->y - center.y);
        double cosa = std::cos(angle);
        double sina = std::sin(angle);
        double new_x = cosa * t.x - sina * t.y;
        double new_y = sina * t.x + cosa * t.y;
        ret_contour.push_back(cv::Point2f((float)(new_x + center.x), (float)(new_y + center.y)));
    }
    return ret_contour;
}
    
class guided_matcher {
public:
    piece& p1;
    piece& p2;
    int e1;
    int e2;
    bool debug;

    bool pieces;
    bool edges;    
    bool color;
    int margin;
    
    int p2xoff = 0;
    int p2yoff = 0;
    
    
    params& user_params;
    
    float scale_factor;
    cv::Mat rendered;
    std::string window_name;

    
    guided_matcher(piece& p1, piece& p2, int e1, int e2, params& user_params)
        : p1(p1), p2(p2), e1(e1), e2(e2), user_params(user_params)
    {
        debug = false;
        color = true;
        edges = false;
        pieces = true;
        margin = 25;
        
        scale_factor = user_params.getGuiScale();
        
        std::stringstream wns;
        wns << p1.get_id() << "-" << e1 << " __ " << p2.get_id() << "-" << e2;
        window_name = wns.str();
        
        
    }
    
    cv::Mat bw_to_color(cv::Mat bw) {
        std::vector<cv::Mat> channels(3);
        channels.at(0) = bw;
        channels.at(1) = bw;
        channels.at(2) = bw;
        
        cv::Mat color;
        cv::merge(channels,color);
        return color;
    }
    
    template<class T>
    cv::Point_<T> get_top(std::vector<cv::Point_<T>>& contour, cv::Point_<T>* bottom = NULL) {
        cv::Point f = contour.front();
        cv::Point b = contour.back();
        if (f.y < b.y) {
            if (bottom != NULL) {
                bottom->x = b.x;
                bottom->y = b.y;
            }
            return f;
        } 
        else {
            if (bottom != NULL) {
                bottom->x = f.x;
                bottom->y = f.y;
            }
            return b;
        }
    }
    
    template<class T>
    double compute_angle(std::vector<cv::Point_<T>> contour) {
        cv::Point_<T> b;
        cv::Point_<T> a = get_top(contour, &b);
        cv::Point_<T> c = cv::Point_<T>(b.x, a.y);
        return utils::compute_rotation_angle(a, b, c);        
    }
    

    std::vector<cv::Point> render_piece(piece& p, int edge, int extra_rotation, cv::Mat& dst, int xoffset, int yoffset, int padded_dim, cv::Point* align_to) {  
        cv::Mat orig;
        cv::Mat padded;
        cv::Mat aligned;
        cv::Mat resized;
        
        int alignx = 0;
        int aligny = 0;
        
        orig = color ? p.full_color : bw_to_color(p.bw);

        
        // Add alpha channel and initialize it using the bw image
        cv::Mat channels[4];
        cv::split(orig, channels);
        p.bw.convertTo(channels[3], channels[0].type());
        cv::merge(channels, 4, orig);
        
        
        // Calculate the translation into the padded area
        cv::Point tx((padded_dim - orig.size().width) / 2, (padded_dim - orig.size().height) / 2);
        
        // Calculate the center of the original
        cv::Point center(orig.cols/2.0, orig.rows/2.0);


        // Compute the rotation required to align the pieces
        
        // Angle to straighten edge 0
        double angle = compute_angle(p.edges[0].get_contour());

        // Update the angle to get the selected edge facing the correct direction
        // extra_rotation = 0 is used for the piece on the left, extra_rotation=2 is used on the right
        angle -= 90.0 * ((edge + extra_rotation) % 4 - 2);
        
        // Rotate the selected edge contour
        std::vector<cv::Point2f> edge_contour = rotate(center, p.edges[edge].get_contour(), -angle);
        
        // Fine tune the angle 
        angle += compute_angle(edge_contour);
        
        
        // Rotate and translate the selected edge
        edge_contour = rotate(center, p.edges[edge].get_contour(), -angle);
        std::vector<cv::Point> edge_contour_tx;
        utils::translate_contour(edge_contour, edge_contour_tx, tx.x, tx.y, 1.0);        


        // Determine the translation required for alignment
        cv::Point top = get_top(edge_contour_tx);
        if (align_to != NULL) {
            alignx = align_to->x - top.x;
            aligny = align_to->y - top.y;
        }
        
        alignx += xoffset;
        aligny += yoffset;
        
        if (pieces) {
            // Copy the original into the padded image
            padded = cv::Mat::zeros( padded_dim, padded_dim, CV_8UC4);        
            orig.copyTo(padded(cv::Rect(tx.x, tx.y, orig.size().width, orig.size().height)));            
            
            // Rotate the padded image
            cv::Mat r = cv::getRotationMatrix2D(cv::Point2f(padded.cols/2.0, padded.rows/2.0), angle, 1.0);
            cv::warpAffine(padded, aligned, r, padded.size());               
            
            if (aligny != 0) {
                cv::Mat out = cv::Mat::zeros(aligned.size(), aligned.type());
                
                if (aligny > 0) {
                    aligned(cv::Rect(0,0, aligned.cols,aligned.rows-aligny))
                            .copyTo(out(cv::Rect(0,aligny,aligned.cols,aligned.rows-aligny)));                 
                } else {
                    aligned(cv::Rect(0,-aligny, aligned.cols,aligned.rows+aligny))
                            .copyTo(out(cv::Rect(0,0,aligned.cols,aligned.rows+aligny)));               
                }
                aligned = out;
            }
            
            // Resize
            cv::resize(aligned, resized, cv::Size(), scale_factor, scale_factor, cv::INTER_CUBIC);
            
            // Split to gain access to the padded, rotated, and resized alpha channel
            cv::split(resized, channels);
            
            // Copy using the alpha channel as the mask
            cv::Rect crect(alignx*scale_factor, 0, std::min(rendered.size().width, resized.size().width), std::min(rendered.size().height, resized.size().height));
            resized.copyTo(rendered(cv::Rect(crect)), channels[3]);  
        }
        
        // Render edge contour in red
        if (edges) {
            std::vector<std::vector<cv::Point>> edge_contours;
            edge_contours.push_back(utils::translate_contour(edge_contour_tx, alignx, aligny, scale_factor));
            cv::polylines(rendered, edge_contours, false, cv::Scalar(0, 0, 255), 2);        
        }
        
        return edge_contour_tx;
    }
    

    void render() {
     
        int maxdim = std::max(std::max(std::max(
                p1.full_color.size().width, 
                p1.full_color.size().height), 
                p2.full_color.size().width), 
                p2.full_color.size().height);        
        
        int width = maxdim * 2 + margin * 4;

        int height = maxdim + margin * 2;

        int padded_dim = maxdim + margin * 2;
        
        width *= scale_factor;
        width += (width % 2) + 2;
        height *= scale_factor;
        height += (height % 2) + 2;
        
        rendered = cv::Mat::zeros(height, width, CV_8UC4 );

        std::vector<cv::Point> txc = render_piece(p1, e1, 0, rendered, 0, 0, padded_dim, NULL);

        cv::Point top = get_top(txc);
        render_piece(p2, e2, 2, rendered, p2xoff, p2yoff, padded_dim, &top);

        std::stringstream title_stream;
        title_stream << "Does piece " << p1.get_number()
            << " fit to " << p2.get_number()
            << " ? ";
        cv::putText(rendered, title_stream.str(), cv::Point(25,25),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 255, 255), 1, COMPAT_CV_LINE_AA); 
        
        
        cv::imshow(window_name, rendered);
    }

    void adjust_scale( float adjustment) {
        float new_scale =  scale_factor + adjustment;
        if (new_scale < 0.25) {
            new_scale = 0.25;
        }
        if (new_scale == scale_factor) {
            return;
        }
        scale_factor = new_scale;
        render();
//        std::cout << "Scale factor is now " << scale_factor << std::endl;
    }
    
    std::string edit() {

        
        cv::namedWindow(window_name);    
        cv::setMouseCallback(window_name, gm_mouse_callback, this);
        
        render();

        std::string command;
        
        bool done = false;
        do {
            int c = cv::waitKey(0);
            // std::cout << c << std::endl;
            switch (c) {
                case -1:  // User probably clicked on the "x" in the window title bar
                    done = true;
                    command = GM_COMMAND_X_CLOSE;
                    break;
                case 'n':
                    done = true;
                    command = GM_COMMAND_NO;
                    break;
                case 'y':
                    done = true;
                    command = GM_COMMAND_YES;
                    break;
                case 's':
                    done = true;
                    command = GM_COMMAND_SHOW_SET;
                    break;
                case 'r':
                    done = true;
                    command = GM_COMMAND_SHOW_ROTATION;
                    break;  
                case 'w':
                    done = true;
                    command = GM_COMMAND_WORK_ON_SET;
                    break;
                case 'b':
                    done = true;
                    command = GM_COMMAND_MARK_BOUNDARY;
                    break;
                case 'c':
                    color = !color;
                    render();
                    break;
                case 'e':
                case 'p':
                    pieces = !pieces;
                    edges = !pieces;
                    render();
                    break;                          
                case 'x':
                    p2xoff += 1;
                    render();
                    break;
                case 'X':
                    p2xoff -= 1;
                    render();
                    break;       
                case 'z':
                    p2yoff -= 1;
                    render();
                    break;
                case 'Z':
                    p2yoff += 1;
                    render();
                    break;                      
                case '-':
                    adjust_scale(-0.25);
                    break;
                case '+':
                case '=':
                    adjust_scale(0.25);
                    break; 
                case 'h':
                case '?':
                    std::cout << "Keyboard commands:" << std::endl
                            << "KEY   MEANING" << std::endl
                            << "n     no, this is not a valid match" << std::endl
                            << "y     yes, this is a valid match" << std::endl
                            << "c     toggle between color and black and white" << std::endl
                            << "e     toggle between showing the pieces and the matched edge outlines" << std::endl
                            << "p     same as pressing key 'e'" << std::endl                            
                            << "-/=   decrease/increase the rendering scale of the pieces" << std::endl
                            << "x/X   adjust the x-offset between the matched pieces" << std::endl
                            << "z/Z   adjust the y-offset between the matched pieces" << std::endl                            
                            << "s     show the pieces in the current collection set" << std::endl
                            << "r     show the piece rotations of the current collection set" << std::endl                            
                            << "w     work on a different collection set" << std::endl                                                        
                            << "b     mark the right edge of the left-hand piece as a boundary" << std::endl
                            ;
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
        
        return command;
    }
    
    
    void mouse_down(int x, int y) {
    }
    
    void mouse_move(int x, int y) {
    }
    
    void mouse_up(int x, int y) {

    }    
    
private:

}; 

void gm_mouse_callback(int event, int x, int y, int flags, void* userdata) {
    guided_matcher* data = (guided_matcher*)userdata;
    
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        if (data->debug) {
            std::cout << "Left mouse down - position (" << x << ", " << y << ")" << std::endl;
        }
        data->mouse_down(x, y);
    }
    else if  ( event == cv::EVENT_LBUTTONUP )
    {
        if (data->debug) {
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


std::string guided_match(piece& p1, piece& p2, int e1, int e2, params& user_params) {
    guided_matcher editor(p1, p2, e1, e2, user_params);
    return editor.edit();
}

