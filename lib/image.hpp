#ifndef IMAGE
#define IMAGE
#include "image.hpp"
#include <gtkmm-3.0/gtkmm.h>
#include <iostream>

// Image previewer class
class Image {
    private:
    // Attributes
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    bool isScanned = false;

    public:
    // Constructors
    Image();

    Image(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

    // Getter and setter
    Glib::RefPtr<Gdk::Pixbuf> getPixbuf();

    bool getIsScanned();

    void setPixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

    void setIsScanned(bool isScanned);
};
#endif
