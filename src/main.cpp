#include <iostream>
#include <format>
#include <memory>
#include <vector>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>


auto find_pixel_size(const double radius, const double expected) -> double {
    auto pixel_size = 1;

    auto measured_radius = radius / pixel_size;

    while (measured_radius > expected) {
        measured_radius = radius / pixel_size;
        pixel_size += 1;
    }
    pixel_size -= 1;
    while (measured_radius > expected) {
        measured_radius = radius / pixel_size;
        pixel_size += 0.001;
    }

    return pixel_size;
}


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

    if (!outer_circle.empty()) {
        cv::HoughCircles(
            blur,
            inner_circle,
            cv::HoughModes::HOUGH_GRADIENT,
            1,
            blur.rows / 16,
            40, 50,
            10, outer_circle[0][2] * 0.8);
    }
    else {
        std::cout << "Failed to find outer circle" << "\n";
        return;
    }


    const auto blue = cv::Scalar(255, 0, 0, 255);
    const auto red = cv::Scalar(0, 0, 255, 255);

    constexpr auto expected_radius = 3.5;
    const auto pixel_size = find_pixel_size(outer_circle[0][2], expected_radius);

    for (auto circle : outer_circle) {
        const auto center = cv::Point(circle[0], circle[1]);

        cv::circle(*image, center, 2, blue, 2, cv::LINE_AA);

        const auto radius = circle[2];

        cv::circle(*image, center, radius, blue, 2, cv::LINE_AA);

        auto text = std::format("Center: {} {} | Dim: {:.3f}", circle[0], circle[1], 2 * (circle[2] / pixel_size));
        cv::putText(*image, text, cv::Point(30, 60), cv::FONT_HERSHEY_PLAIN, 3, blue, 1, cv::LINE_AA);
    }

    for (auto circle : inner_circle) {
        const auto center = cv::Point(circle[0], circle[1]);

        cv::circle(*image, center, 2, red, 2, cv::LINE_AA);

        const auto radius = circle[2];

        cv::circle(*image, center, radius, red, 2, cv::LINE_AA);
        auto text = std::format("Center: {} {} | Dim: {:.3f}", circle[0], circle[1], 2 * (circle[2] / pixel_size));
        cv::putText(*image, text, cv::Point(30, 200), cv::FONT_HERSHEY_PLAIN, 3, red, 1, cv::LINE_AA);
    }
}


int main(const int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Please pass in input and output paths" << "\n";
    }

    const auto input = std::string(argv[1]);
    const auto output = std::string(argv[2]);

    std::cout << "Input: " << input << "\n";
    std::cout << "Output: " << output << "\n";


    auto capture = cv::VideoCapture();
    auto sink = cv::VideoWriter();

    const auto fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');


    capture.open(input);
    // "appsrc ! videoconvert ! x264enc ! mpegtsmux ! udpsink host=localhost port=5000"
    sink.open(output, 0, 30.0,
              cv::Size(capture.get(cv::CAP_PROP_FRAME_WIDTH), capture.get(cv::CAP_PROP_FRAME_HEIGHT)));


    if (!capture.isOpened()) {
        std::cout << "Failed to open video" << "\n";
        return 1;
    }

    const auto frame = std::make_shared<cv::Mat>();
    auto frame_count = 0;

    const auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << std::format("Time started: {}", start_time) << "\n";
    while (capture.read(*frame)) {
        if (frame->empty()) {
            std::cout << "Failed to get frame" << "\n";
            break;
        }
        image_loop(frame);
        sink.write(*frame);

        frame_count++;
    }
    std::cout << '\n';
    std::cout << std::format("Frames written: {}", frame_count) << "\n";
    const auto end_time = std::chrono::high_resolution_clock::now();

    auto time_taken = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << std::format("Time taken: {}", time_taken) << "\n";

    sink.release();
    capture.release();

    std::cout << "Done." << "\n";

    return 0;
}
