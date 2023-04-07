#ifndef IMAGE_PREVIEWER
#define IMAGE_PREVIEWER
#include "image-cropper.hpp"
#include "image.hpp"
#include <gtkmm-3.0/gtkmm.h>
#include <iostream>

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
