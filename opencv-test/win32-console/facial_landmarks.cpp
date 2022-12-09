#include "facial_landmarks.h"

// following code was built from CharGPT which ask it give me sample using Dlib for facial landmark detection

#include <iostream>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace dlib;
using namespace cv;

int main() {
    // Load the Dlib's face detector and the shape predictor
    frontal_face_detector face_detector = get_frontal_face_detector();
    shape_predictor shape_predictor;
    deserialize("shape_predictor_68_face_landmarks.dat") >> shape_predictor;

    // Load the input image
    Mat img = imread("input.jpg");

    // Convert the image to grayscale
    cvtColor(img, img, COLOR_BGR2GRAY);

    // Detect faces in the image
    dlib::array2d<unsigned char> dlib_img;
    dlib::assign_image(dlib_img, dlib::cv_image<unsigned char>(img));
    std::vector<dlib::rectangle> faces = face_detector(dlib_img);

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
    waitKey(0);

    return 0;
}
