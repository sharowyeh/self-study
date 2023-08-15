#include "facial_landmarks.h"

// following code was built from CharGPT which ask it give me sample using Dlib for facial landmark detection

#include <iostream>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/face.hpp>

// re-built opencv 4.6.0 with vs2022
#pragma comment(lib, "opencv_face460d.lib")

using namespace std;
using namespace dlib;
using namespace cv;

int facial_landmarks_dlib() {
    // Load the Dlib's face detector and the shape predictor
    frontal_face_detector face_detector = get_frontal_face_detector();
    shape_predictor shape_predictor;

    // NOTE: this predictor for dlib(debug) default cost 5~6s per image via my laptop
    // VSC++ default working directory is project root instead of output folder
    deserialize("..\\x64\\Debug\\shape_predictor_68_face_landmarks.dat") >> shape_predictor;
    // Can download dat file from http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2

    if (shape_predictor.num_parts() == 0 && shape_predictor.num_features() == 0) {
        std::cout << "Predictor loading failed" << std::endl;
        return 0;
    }
    else {
        std::cout << "Predictor loaded" << std::endl;
    }

    VideoCapture cap;
    // Load the input image
    //Mat img = imread("..\\x64\\Debug\\robot-anime-girl.png");
    Mat img = imread("../Lenna_test_image.png");
    if (img.empty()) {
        std::cout << "Image not found, using camera retrieve frame" << std::endl;
        if (cap.isOpened() == false)
            cap.open(0);
        while (cap.read(img) == false)
            Sleep(100);
        if (img.empty() == false)
            std::cout << "Got frame from camera" << std::endl;
    }

    // Detect faces in the image
    //dlib::array2d<unsigned char> dlib_img;

    do {
        // Convert the image to grayscale
        //cvtColor(img, img, COLOR_BGR2GRAY);

        //dlib::assign_image(dlib_img, dlib::cv_image<unsigned char>(img));
        // simply wraps Mat object, won't copy anything
        dlib::cv_image<bgr_pixel> dlib_img(img);
        std::vector<dlib::rectangle> faces = face_detector(dlib_img);

        std::cout << "Found " << faces.size() << " faces" << std::endl;

        // Loop over the detected faces
        for (int i = 0; i < faces.size(); i++) {
            // Find the facial landmarks for each face
            dlib::full_object_detection shape = shape_predictor(dlib_img, faces[i]);

            // Loop over the 68 facial landmarks and draw them on the image
            for (int j = 0; j < shape.num_parts(); j++) {
                int x = shape.part(j).x();
                int y = shape.part(j).y();
                circle(img, Point(x, y), 3, Scalar(255, 255, 255), -1);
            }
        }

        // Show the image with the facial landmarks
        imshow("Facial Landmarks", img);

        // press c to quit while-loop
        if (waitKey(20) == 'c')
            break;

    } while (cap.isOpened() && cap.read(img));
    
    waitKey(0);

    return 0;
}

int facial_landmarks_opencv() {
    // following code was generated from chatgpt...
    Mat image = imread("image.png");
    
    // load pre-trained facial landmark detector
    // lbfmodel.yaml is here? https://github.com/kurnianggoro/GSOC2017/tree/master/data
    Ptr<face::Facemark> facemark = face::FacemarkLBF::create();
    facemark->loadModel("lbfmodel.yaml");

    // face regions from classifier
    // classification is here? C:\opencv\build\install\etc\haarcascades
    std::vector<Rect> faces;
    CascadeClassifier faceDetector("haarcascade_frontalface_default.xml");
    faceDetector.detectMultiScale(image, faces);

    // detect the facial landmarks
    std::vector<std::vector<Point2f>> landmarks;
    facemark->fit(image, faces, landmarks);

    // draw the facial landmarks on the image
    Mat draw;
    cvtColor(image, draw, COLOR_BGR2RGB);
    face::drawFacemarks(draw, landmarks[0], Scalar(0, 0, 255));

    // display
    imshow("face landmarks", draw);
    waitKey(0);

    return 0;
}