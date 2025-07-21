#include <iostream>
#include <format>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

auto image_loop(const std::shared_ptr<cv::Mat>& image) {
    auto grayscale_image = cv::Mat();

    cv::cvtColor(*image, grayscale_image, cv::COLOR_BGR2GRAY);

    auto blur = cv::Mat();

    cv::medianBlur(grayscale_image, blur, 25);

    auto outer_circle = std::vector<cv::Vec3f>();
    auto inner_circle = std::vector<cv::Vec3f>();

    cv::HoughCircles(
        blur,
        outer_circle,
        cv::HoughModes::HOUGH_GRADIENT,
        1,
        blur.rows / 16,
        40, 50,
        10, 3000);

    cv::HoughCircles(
        blur,
        inner_circle,
        cv::HoughModes::HOUGH_GRADIENT,
        1,
        blur.rows / 16,
        40, 50,
        10, outer_circle[0][2] * 0.8);

    const auto blue = cv::Scalar(255, 0, 0, 255);
    const auto black = cv::Scalar(0, 0, 0, 255);
    const auto red = cv::Scalar(0, 0, 255, 255);

    for (auto circle : outer_circle) {
        const auto center = cv::Point(circle[0], circle[1]);

        cv::circle(*image, center, 2, blue, 2, cv::LINE_AA);

        const auto radius = circle[2];

        cv::circle(*image, center, radius, blue, 2, cv::LINE_AA);
        auto text = std::format("Center: {} {} | Radius: {}", circle[0], circle[1], circle[2]);
        cv::putText(*image, text, cv::Point(30, 30), cv::FONT_HERSHEY_PLAIN, 3, black, CPU_SSE4_1, cv::LINE_AA);
        std::cout << text << "\n";
    }

    for (auto circle : inner_circle) {
        const auto center = cv::Point(circle[0], circle[1]);

        cv::circle(*image, center, 2, red, 2, cv::LINE_AA);

        const auto radius = circle[2];

        cv::circle(*image, center, radius, red, 2, cv::LINE_AA);
        auto text = std::format("Center: {} {} | Radius: {}", circle[0], circle[1], circle[2]);
        cv::putText(*image, text, cv::Point(30, 90), cv::FONT_HERSHEY_PLAIN, 3, black, CPU_SSE4_1, cv::LINE_AA);
        std::cout << text << "\n";
    }
}


int main() {
    //const auto image = cv::imread(R"(C:\Users\stewart\Downloads\250630115015707.jpg)");

    auto capture = cv::VideoCapture();

    capture.open(R"(C:\Users\stewart\Downloads\2025-07-21 12-00-06.mkv)", cv::CAP_FFMPEG);

    if (!capture.isOpened()) {
        std::cout << "Failed to open video" << "\n";
        return 1;
    }

    auto frame = std::make_shared<cv::Mat>();

    while (true) {
        capture.read(*frame);
        if (frame->empty()) {
            std::cout << "Failed to get frame" << "\n";
            break;
        }
        image_loop(frame);
        cv::imshow("image", *frame);
        if (cv::waitKey(5) >= 0) {
            break;
        }
    }


    return 0;
}
