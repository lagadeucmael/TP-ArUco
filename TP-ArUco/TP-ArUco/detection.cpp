
#include <opencv2/opencv.hpp>
#include <aruco.hpp>
#include <iostream>
#include <cstdlib>


namespace {
    const char* about = "Detect ArUco marker images";
    const char* keys =
        "{d        |16    | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, "
        "DICT_4X4_250=2, DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, "
        "DICT_5X5_250=6, DICT_5X5_1000=7, DICT_6X6_50=8, DICT_6X6_100=9, "
        "DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12, DICT_7X7_100=13, "
        "DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{h        |false | Print help }"
        "{v        |<none>| Custom video source, otherwise '0' }"
        ;
}

void detection(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (parser.get<bool>("h")) {
        parser.printMessage();
        return ;
    }

    int dictionaryId = parser.get<int>("d");
    int wait_time = 10;
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
        return;
    }

    cv::Ptr<cv::aruco::Dictionary> dictionary =
        cv::aruco::getPredefinedDictionary(\
            cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    while (in_video.grab()) {
        cv::Mat image, image_copy;
        in_video.retrieve(image);
        image.copyTo(image_copy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        // If at least one marker detected
        if (ids.size() > 0)
            cv::aruco::drawDetectedMarkers(image_copy, corners, ids);

        imshow("Detected markers", image_copy);
        char key = (char)cv::waitKey(wait_time);
        if (key == 27)
            break;
    }

    in_video.release();

    return ;
}
