
#include <opencv2/highgui.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <aruco.hpp>

using namespace cv;

namespace {
    const char* about = "Create an ArUco grid board image";
    const char* keys =
        "{@outfile |<none> | Output image }"
        "{w        |       | Number of markers in X direction }"
        "{h        |       | Number of markers in Y direction }"
        "{l        |       | Marker side length (in pixels) }"
        "{s        |       | Separation between two consecutive markers in the grid (in pixels)}"
        "{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{m        |       | Margins size (in pixels). Default is marker separation (-s) }"
        "{bb       | 1     | Number of bits in marker borders }"
        "{si       | false | show generated image }";
}

void create_board(int bb, int m, int w, int h, int l, int s, int d, bool si) {


    int markersX = w;
    int markersY = h;
    int markerLength = l;
    int markerSeparation = s;
    int dictionaryId = d;
    int margins = m;

    int borderBits = bb;
    bool showImage = si;

    String out ="board.jpg";

    Size imageSize;
    imageSize.width = markersX * (markerLength + markerSeparation) - markerSeparation + 2 * margins;
    imageSize.height =
        markersY * (markerLength + markerSeparation) - markerSeparation + 2 * margins;

    Ptr<aruco::Dictionary> dictionary =
        aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    Ptr<aruco::GridBoard> board = aruco::GridBoard::create(markersX, markersY, float(markerLength),
        float(markerSeparation), dictionary);

    // show created board
    Mat boardImage;
    board->draw(imageSize, boardImage, margins, borderBits);

    if (showImage) {
        imshow("board", boardImage);
        imwrite("boardImage.jpg", boardImage);
        waitKey(0);
    }

    imwrite(out, boardImage);
}