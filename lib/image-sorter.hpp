#ifndef LIBSERIAL_HELPERS
#define LIBSERIAL_HELPERS
#include <iostream>
#include <gtkmm-3.0/gtkmm.h>

// Image sorter class
class ImageSorter : public Gtk::ScrolledWindow {
    private:
    // Attributes
    Gtk::Grid grid;
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs;

    public:
    // Constructor
    ImageSorter();

    // Getter and setter
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> getPixbufs();

    void setPixbufs(std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs);
};
#endif