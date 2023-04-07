#ifndef IMAGE_CROPPER
#define IMAGE_CROPPER
#include "image.hpp"
#include <gtkmm-3.0/gtkmm.h>
#include <iostream>

// Image cropper class
class ImageCropper : public Gtk::Dialog {
    private:
    // Attributes
    Gtk::DrawingArea drawingArea;
    Image image;
    double corners[4][2] = {{1, 1}, {1, 0}, {0, 0}, {0, 1}};

    protected:
    // Method to crop the image on response
    void on_response(int response) override;

    public:
    // Constructors
    ImageCropper(Gtk::Window& parent);

    ImageCropper(Gtk::Window& parent, Image image);

    // Getter and setter
    Image getImage();

    void setImage(Image image);
};
#endif
