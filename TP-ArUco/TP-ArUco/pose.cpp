

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <aruco.hpp>
#include <iostream>
#include <cstdlib>


namespace {
    const char* about = "Pose estimation of ArUco marker images";
    const char* keys =
        "{d        |16    | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, "
        "DICT_4X4_250=2, DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, "
        "DICT_5X5_250=6, DICT_5X5_1000=7, DICT_6X6_50=8, DICT_6X6_100=9, "
        "DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12, DICT_7X7_100=13, "
        "DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{h        |false | Print help }"
        "{l        |      | Actual marker length in meter }"
        "{v        |<none>| Custom video source, otherwise '0' }"
        "{h        |false | Print help }"
        "{l        |      | Actual marker length in meter }"
        "{v        |<none>| Custom video source, otherwise '0' }"
        ;
}

void pose(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (parser.get<bool>("h")) {
        parser.printMessage();
        return;
    }

    int dictionaryId = parser.get<int>("d");
    float marker_length_m = 0.055;
    int wait_time = 1;

    if (marker_length_m <= 0) {
        std::cerr << "marker length must be a positive value in meter"
            << std::endl;
        return ;
    }

    cv::String videoInput = "0";
    cv::VideoCapture in_video;
    
    if (parser.has("v")) {
        videoInput = parser.get<cv::String>("v");
        if (videoInput.empty()) {
            parser.printMessage();
            return ;
        }
        char* end = nullptr;
        int source = static_cast<int>(std::strtol(videoInput.c_str(), &end, \
            10));
        if (!end || end == videoInput.c_str()) {
            in_video.open(videoInput); // url
        }
        else {
            in_video.open(source); // id
        }
    }
    else {
        in_video.open(0);
    }

    if (!parser.check()) {
        parser.printErrors();
        return ;
    }

    if (!in_video.isOpened()) {
        std::cerr << "failed to open video input: " << videoInput << std::endl;
        return ;
    }

    cv::Mat image, image_copy;
    cv::Mat camera_matrix, dist_coeffs;
    std::ostringstream vector_to_marker;

    cv::Ptr<cv::aruco::Dictionary> dictionary =
        cv::aruco::getPredefinedDictionary(\
            cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    cv::FileStorage fs("calibration_params.yml", cv::FileStorage::READ);

    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficients"] >> dist_coeffs;

    std::cout << "camera_matrix\n" << camera_matrix << std::endl;
    std::cout << "\ndist coeffs\n" << dist_coeffs << std::endl;

    while (in_video.grab())
    {
        in_video.retrieve(image);
        image.copyTo(image_copy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        // if at least one marker detected
        if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(image_copy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, marker_length_m,
                camera_matrix, dist_coeffs, rvecs, tvecs);

            // Draw axis for each marker
            for (int i = 0; i < ids.size(); i++)
            {
                cv::aruco::drawAxis(image_copy, camera_matrix, dist_coeffs,
                    rvecs[i], tvecs[i], 0.1);

                // This section is going to print the data for all the detected
                // markers. If you have more than a single marker, it is
                // recommended to change the below section so that either you
                // only print the data for a specific marker, or you print the
                // data for each marker separately.
                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                    << "x: " << std::setw(8) << tvecs[0](0);
                cv::putText(image_copy, vector_to_marker.str(),
                    cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0, 252, 124), 1, cv::LINE_AA);
                
                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                    << "y: " << std::setw(8) << tvecs[0](1);
                cv::putText(image_copy, vector_to_marker.str(),
                    cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0, 252, 124), 1, cv::LINE_AA);

                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                    << "z: " << std::setw(8) << tvecs[0](2);
                cv::putText(image_copy, vector_to_marker.str(),
                    cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0, 252, 124), 1, cv::LINE_AA);
            }
        }

        imshow("Pose estimation", image_copy);
        char key = (char)cv::waitKey(wait_time);
        if (key == 27)
            break;
    }

    in_video.release();

    return ;
}