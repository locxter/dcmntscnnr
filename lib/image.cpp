#include "image.hpp"

// Constructors
Image::Image() {
}

Image::Image(Glib::RefPtr<Gdk::Pixbuf> pixbuf) : Image() {
    setPixbuf(pixbuf);
}

// Getter and setter
Glib::RefPtr<Gdk::Pixbuf> Image::getPixbuf() {
    return pixbuf;
}

bool Image::getIsScanned() {
    return isScanned;
}

void Image::setPixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf) {
    this->pixbuf = pixbuf;
}

void Image::setIsScanned(bool isScanned) {
    this->isScanned = isScanned;
}
