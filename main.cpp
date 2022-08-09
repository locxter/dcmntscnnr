#include <iostream>
#include <opencv2/opencv.hpp>
#include "lib/image-previewer.hpp"

// Main function
int main(int argc, char** argv) {
    // UI components
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("com.github.locxter.dcmntscnnr");
    Gtk::Window window;
    Gtk::Grid grid;
    Gtk::Button openButton("Open");
    Gtk::Button convertButton("Convert");
    Gtk::Button saveButton("Save");
    ImagePreviewer imagePreviewer;
    Gtk::Label aboutLabel("2022 locxter");
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs;
    // Add functions to the buttons
    openButton.signal_clicked().connect([&]() {
        Glib::RefPtr<Gtk::FileFilter> fileFilter = Gtk::FileFilter::create();
        Gtk::FileChooserDialog dialog(window, "Open");
        int response;
        // Configure file filter and dialog
        fileFilter->set_name("Images");
        fileFilter->add_pattern("*.png");
        fileFilter->add_pattern("*.jpg");
        fileFilter->add_pattern("*.jpeg");
        dialog.add_filter(fileFilter);
        dialog.set_select_multiple(true);
        dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("Open", Gtk::RESPONSE_OK);
        // Show the dialog and wait for a user response
        response = dialog.run();
        // Handle the response
        if (response == Gtk::RESPONSE_OK) {
            std::vector<std::string> filenames = dialog.get_filenames();
            pixbufs = imagePreviewer.getPixbufs();
            // Load files
            for (std::string filename : filenames) {
                pixbufs.push_back(Gdk::Pixbuf::create_from_file(filename));
            }
            imagePreviewer.setPixbufs(pixbufs);
        }
    });
    convertButton.signal_clicked().connect([&]() {
        const std::string FILENAME = ".dcmntscnnr-tmp.png";
        pixbufs = imagePreviewer.getPixbufs();
        for (int i = 0; i < pixbufs.size(); i++) {
            pixbufs[i]->save(FILENAME, "png");
            // Image containers
            cv::Mat rawImage = cv::imread(FILENAME);
            cv::Mat preprocessedImage;
            cv::Mat cannyImage;
            cv::Mat processedImage;
            // Image processing variables
            std::vector<std::vector<cv::Point>> contours;
            std::vector<std::vector<cv::Point>> approximations;
            std::vector<cv::Point> approximation(4);
            std::vector<cv::Point> sortedApproximation(4);
            float topWidth;
            float bottomWidth;
            float width;
            float leftHeight;
            float rightHeight;
            float height;
            cv::Point2f sourceCoordinates[4];
            cv::Point2f destinationCoordinates[4];
            // Preprocess for edge detection
            cv::cvtColor(rawImage, preprocessedImage, cv::COLOR_BGR2GRAY);
            cv::medianBlur(preprocessedImage, preprocessedImage, 25);
            cv::adaptiveThreshold(preprocessedImage, preprocessedImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 75, 0);
            cv::medianBlur(preprocessedImage, preprocessedImage, 25);
            // Perform canny edge detection
            cv::Canny(preprocessedImage, cannyImage, 0, 0);
            // Find contours and their approximations in order to select the largest one 
            cv::findContours(cannyImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            approximations.resize(contours.size());
            for (int j = 0; j < contours.size(); j++) {
                double epsilon = 0.05 * cv::arcLength(contours[j], true);
                cv::approxPolyDP(contours[j], approximations[j], epsilon, true);
                double area = cv::contourArea(approximations[j]);
                static double largestArea = 0;
                if (j == 0) {
                    largestArea = 0;
                }
                if (area > largestArea && cv::isContourConvex(approximations[j]) && approximations[j].size() == 4) {
                    approximation = approximations[j];
                    largestArea = area;
                }
            }
            // Order the points for perspective transform
            for (int j = 0; j < 4; j++) {
                static float xAverage = (approximation[0].x + approximation[1].x + approximation[2].x + approximation[3].x) / 4.0;
                static float yAverage = (approximation[0].y + approximation[1].y + approximation[2].y + approximation[3].y) / 4.0;
                if (j == 0) {
                    xAverage = (approximation[0].x + approximation[1].x + approximation[2].x + approximation[3].x) / 4.0;
                    yAverage = (approximation[0].y + approximation[1].y + approximation[2].y + approximation[3].y) / 4.0;
                }
                if (approximation[j].x < xAverage && approximation[j].y < yAverage) {
                    // Top left
                    sortedApproximation[0] = approximation[j];
                } else if (approximation[j].x > xAverage && approximation[j].y < yAverage) {
                    // Top right
                    sortedApproximation[1] = approximation[j];
                } else if (approximation[j].x > xAverage && approximation[j].y > yAverage) {
                    // Bottom right
                    sortedApproximation[2] = approximation[j];
                } else {
                    //Bottom left
                    sortedApproximation[3] = approximation[j];
                }
            }
            // Calculate utility values for perspective transform
            topWidth = std::sqrt(std::pow(sortedApproximation[1].x - sortedApproximation[0].x, 2) + std::pow(sortedApproximation[1].y - sortedApproximation[0].y, 2));
            bottomWidth = std::sqrt(std::pow(sortedApproximation[2].x - sortedApproximation[3].x, 2) + std::pow(sortedApproximation[2].y - sortedApproximation[3].y, 2));
            width = std::max(topWidth, bottomWidth);
            leftHeight = std::sqrt(std::pow(sortedApproximation[3].x - sortedApproximation[0].x, 2) + std::pow(sortedApproximation[3].y - sortedApproximation[0].y, 2));
            rightHeight = std::sqrt(std::pow(sortedApproximation[2].x - sortedApproximation[1].x, 2) + std::pow(sortedApproximation[2].y - sortedApproximation[1].y, 2));
            height = std::max(leftHeight, rightHeight);
            sourceCoordinates[0] = cv::Point2f(sortedApproximation[0].x, sortedApproximation[0].y);
            sourceCoordinates[1] = cv::Point2f(sortedApproximation[1].x, sortedApproximation[1].y);
            sourceCoordinates[2] = cv::Point2f(sortedApproximation[2].x, sortedApproximation[2].y);
            sourceCoordinates[3] = cv::Point2f(sortedApproximation[3].x, sortedApproximation[3].y);
            destinationCoordinates[0] = cv::Point2f(0, 0);
            destinationCoordinates[1] = cv::Point2f(width - 1, 0);
            destinationCoordinates[2] = cv::Point2f(width - 1, height - 1);
            destinationCoordinates[3] = cv::Point2f(0, height - 1);
            // Perform perspective transform
            cv::Mat transformationMatrix = cv::getPerspectiveTransform(sourceCoordinates, destinationCoordinates);
            cv::warpPerspective(rawImage, processedImage, transformationMatrix, cv::Size(width, height));
            // Clean the result
            cv::cvtColor(processedImage, processedImage, cv::COLOR_BGR2GRAY);
            cv::adaptiveThreshold(processedImage, processedImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 125, 25);
            // Update the image sorter
            cv::imwrite(FILENAME, processedImage);
            pixbufs[i] = Gdk::Pixbuf::create_from_file(FILENAME);
            imagePreviewer.setPixbufs(pixbufs);
            while (Gtk::Main::events_pending()) {
                Gtk::Main::iteration();
            }
        }
        std::remove(FILENAME.c_str());
    });
    saveButton.signal_clicked().connect([&]() {
        if (pixbufs.size() > 0) {
            Glib::RefPtr<Gtk::FileFilter> fileFilter = Gtk::FileFilter::create();
            Gtk::FileChooserDialog dialog(window, "Save", Gtk::FILE_CHOOSER_ACTION_SAVE);
            int response;
            // Configure file filter and dialog
            fileFilter->set_name("PDF documents");
            fileFilter->add_pattern("*.pdf");
            dialog.add_filter(fileFilter);
            dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
            dialog.add_button("Save", Gtk::RESPONSE_OK);
            pixbufs = imagePreviewer.getPixbufs();
            // Show the dialog and wait for a user response
            response = dialog.run();
            // Handle the response
            if (response == Gtk::RESPONSE_OK) {
                const int PAGE_WIDTH = 210;
                const int PAGE_HEIGHT = 297;
                const float MM_TO_PT = 72 / 25.4;
                std::string filename = dialog.get_filename();
                Cairo::RefPtr<Cairo::PdfSurface> surface = Cairo::PdfSurface::create(filename, 0, 0);
                Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
                // Write file
                for (Glib::RefPtr<Gdk::Pixbuf> pixbuf : pixbufs) {
                    int pageWidth;
                    int pageHeight;
                    // Check for landscape document
                    if ((float) pixbuf->get_width() / pixbuf->get_height() < (((float) PAGE_WIDTH / PAGE_HEIGHT) + (PAGE_HEIGHT / PAGE_WIDTH)) / 2) {
                        pageWidth = PAGE_WIDTH;
                        pageHeight = PAGE_HEIGHT;
                    } else {
                        pageWidth = PAGE_HEIGHT;
                        pageHeight = PAGE_WIDTH;
                    }
                    surface->set_size(std::round(pageWidth * MM_TO_PT), std::round(pageHeight * MM_TO_PT));
                    context->scale((pageWidth * MM_TO_PT) / pixbuf->get_width(), (pageHeight * MM_TO_PT) / pixbuf->get_height());
                    Gdk::Cairo::set_source_pixbuf(context, pixbuf);
                    context->paint();
                    surface->show_page();
                    context->scale(pixbuf->get_width() / (pageWidth * MM_TO_PT), pixbuf->get_height() / (pageHeight * MM_TO_PT));
                }
            }
        }
    });
    // Create the main grid
    grid.set_column_spacing(10);
    grid.set_row_spacing(10);
    openButton.set_hexpand(true);
    grid.attach(openButton, 0, 0);
    convertButton.set_hexpand(true);
    grid.attach(convertButton, 1, 0);
    saveButton.set_hexpand(true);
    grid.attach(saveButton, 2, 0);
    imagePreviewer.set_hexpand(true);
    imagePreviewer.set_vexpand(true);
    grid.attach(imagePreviewer, 0, 1, 3);
    aboutLabel.set_hexpand(true);
    grid.attach(aboutLabel, 0, 2, 3);
    // Create the main window
    window.set_title("dcmntscnnr");
    window.set_border_width(10);
    window.set_default_size(640, 640);
    window.set_size_request(480, 480);
    window.add(grid);
    window.show_all();
    return app->run(window);
}
