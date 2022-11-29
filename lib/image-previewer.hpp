#ifndef IMAGE_PREVIEWER
#define IMAGE_PREVIEWER
#include <iostream>
#include <gtkmm-3.0/gtkmm.h>
#include "image.hpp"
#include "image-cropper.hpp"

// Image previewer class
class ImagePreviewer : public Gtk::ScrolledWindow {
    private:
    // Attributes
    Gtk::Grid grid;
    std::vector<Gtk::DrawingArea> drawingAreas;
    std::vector<Gtk::Button> deleteButtons;
    std::vector<Gtk::Button> rotateLeftButtons;
    std::vector<Gtk::Button> rotateRightButtons;
    std::vector<Gtk::Button> moveLeftButtons;
    std::vector<Gtk::Button> moveRightButtons;
    std::vector<Gtk::Button> cropButtons;
    std::vector<Image> images;

    public:
    // Constructors
    ImagePreviewer();

    ImagePreviewer(std::vector<Image> images);

    // Getter and setter

    std::vector<Image> getImages();

    void setImages(std::vector<Image> images);
};
#endif
