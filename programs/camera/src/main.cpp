#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/objdetect/aruco_detector.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat inputFrame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID, apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    for (;;) {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(inputFrame);
        // check if we succeeded
        if (inputFrame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
        cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        cv::aruco::ArucoDetector detector(dictionary, detectorParams);
        detector.detectMarkers(inputFrame, markerCorners, markerIds, rejectedCandidates);

        cv::Mat outputFrame = inputFrame.clone();
        cv::aruco::drawDetectedMarkers(outputFrame, markerCorners, markerIds);

        // show live and wait for a key with timeout long enough to show images
        imshow("Live", outputFrame);
        if (waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}