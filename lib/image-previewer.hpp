#ifndef IMAGE_PREVIEWER
#define IMAGE_PREVIEWER
#include <iostream>
#include <gtkmm-3.0/gtkmm.h>

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
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs;

    public:
    // Constructor
    ImagePreviewer();

    // Getter and setter
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> getPixbufs();

    void setPixbufs(std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs);
};
#endif
