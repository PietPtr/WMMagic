#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace cv;

std::string exec(char* cmd);

enum Side { LEFT, RIGHT, NONE, BOTH};

int main(int argc, char* argv[])
{
    VideoCapture cap(-1);

    int i = 0;

    float averageRL = 0;
    float averageGL = 0;
    float averageBL = 0;

    int diffRL = 0;
    int diffGL = 0;
    int diffBL = 0;

    float averageRR = 0;
    float averageGR = 0;
    float averageBR = 0;

    int diffRR = 0;
    int diffGR = 0;
    int diffBR = 0;

    Side currentSide = NONE;
    Side lastSide = NONE;
    int numberOfNones = 0;

    int lastMove = 0;

    int callibration = 30;

    while( cap.isOpened() )
    {
        Mat frame;
        cap.read(frame);


        if (i < callibration)
        {
            std::cout << "Callibrating...";
            std::cout << (int)frame.at<Vec3b>(240, 0).val[2] << " " << (int)frame.at<Vec3b>(240, 0).val[1] << " " << (int)frame.at<Vec3b>(240, 0).val[1] << "\n";

            averageRL += (int)frame.at<Vec3b>(240, 0).val[2];
            averageGL += (int)frame.at<Vec3b>(240, 0).val[1];
            averageBL += (int)frame.at<Vec3b>(240, 0).val[0];

            averageRR += (int)frame.at<Vec3b>(240, 639).val[2];
            averageGR += (int)frame.at<Vec3b>(240, 639).val[1];
            averageBR += (int)frame.at<Vec3b>(240, 639).val[0];
        }
        else if (i == callibration + 1)
        {
            averageRL /= (float)callibration;
            averageGL /= (float)callibration;
            averageBL /= (float)callibration;

            averageRR /= (float)callibration;
            averageGR /= (float)callibration;
            averageBR /= (float)callibration;
            std::cout << averageRL << " " << averageGL << " " << averageBL << "\n";
            std::cout << averageRR << " " << averageGR << " " << averageBR << "\n\n";
        }
        else if (i > callibration + 1)
        {
            //std::cout << (int)averageR - (int)frame.at<Vec3b>(240, 0).val[2] << ", ";
            //std::cout << (int)averageG - (int)frame.at<Vec3b>(240, 0).val[1] << ", ";
            //std::cout << (int)averageB - (int)frame.at<Vec3b>(240, 0).val[0] << "\n";

            diffRL = (int)averageRL - (int)frame.at<Vec3b>(240, 0).val[2];
            diffGL = (int)averageGL - (int)frame.at<Vec3b>(240, 0).val[1];
            diffBL = (int)averageBL - (int)frame.at<Vec3b>(240, 0).val[0];

            diffRR = (int)averageRR - (int)frame.at<Vec3b>(240, 639).val[2];
            diffGR = (int)averageGR - (int)frame.at<Vec3b>(240, 639).val[1];
            diffBR = (int)averageBR - (int)frame.at<Vec3b>(240, 639).val[0];

            int sensitivity = 100;
            currentSide = NONE;
            if (std::abs(diffRL + diffGL + diffBL) > sensitivity)
            {
                currentSide = LEFT;
                std::cout << "left\n";
            }
            if (std::abs(diffRR + diffGR + diffBR) > sensitivity)
            {
                currentSide = RIGHT;
                std::cout << "right\n";
            }
            if ((std::abs(diffRL + diffGL + diffBL) > sensitivity) && (std::abs(diffRR + diffGR + diffBR) > sensitivity))
                currentSide = BOTH;

            if (currentSide == NONE)
                numberOfNones++;
            else
                numberOfNones = 0;

            //std::cout << currentSide << " " << lastSide << "\n";

            int x = 0;

            if (lastSide == RIGHT && currentSide == LEFT && numberOfNones < 4 && i - lastMove > 16)
            {
                //x = system("xwininfo -id $(xdotool getwindowfocus) | grep \"Absolute upper-left X\" | sed 's/[^0-9]*//g'");

                std::string output = exec("xwininfo -id $(xdotool getwindowfocus) | grep \"Absolute upper-left X\" | sed 's/[^0-9]*//g'");
                output.pop_back();
                x = std::stoi(output, nullptr);

                std::cout << "RIGHT LEFT" << x << "\n";

                if (x > 1920 && x < 3840)
                {
                    system("xdotool key alt+shift+e");
                    lastMove = i;
                    std::cout << "moved 3\n";
                }
                else if (x > 3840)
                {
                    system("xdotool key alt+shift+w");
                    lastMove = i;
                    std::cout << "moved >1\n";
                }
            }
            else if (lastSide == LEFT && currentSide == RIGHT && numberOfNones < 4 && i - lastMove > 16)
            {
                //x = system("xwininfo -id $(xdotool getwindowfocus) | grep \"Absolute upper-left X\" | sed 's/[^0-9]*//g'");

                std::string output = exec("xwininfo -id $(xdotool getwindowfocus) | grep \"Absolute upper-left X\" | sed 's/[^0-9]*//g'");
                output.pop_back();
                x = std::stoi(output, nullptr);

                std::cout << "LEFT RIGHT" << x << "\n";

                if (x > 1920 && x < 3840)
                {
                    system("xdotool key alt+shift+r");
                    lastMove = i;
                    std::cout << "moved 2\n";
                }
                else if (x < 1920)
                {
                    system("xdotool key alt+shift+w");
                    lastMove = i;
                    std::cout << "moved <1\n";
                }
            }

            if (currentSide != NONE)
                lastSide = currentSide;

            //std::cout << currentSide << " " << lastSide << " " << std::abs(diffRR + diffGR + diffBR) << "\n";
            if (i - lastMove > 16)
                std::cout << "Ready for gesture\n";

        }

        imshow("Float",frame);

        waitKey(1);
        i++;
    }
    return 0;
}

std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}
