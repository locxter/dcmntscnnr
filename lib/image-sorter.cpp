#include "image-sorter.hpp"

// Constructor
ImageSorter::ImageSorter() : Gtk::ScrolledWindow() {
    grid.set_column_spacing(10);
    grid.set_row_spacing(10);
    grid.set_halign(Gtk::ALIGN_CENTER);
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    add(grid);
}

// Getter and setter
std::vector<Glib::RefPtr<Gdk::Pixbuf>> ImageSorter::getPixbufs() {
    return pixbufs;
}

void ImageSorter::setPixbufs(std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs) {
    // Create the grid
    for (int i = 0; i < pixbufs.size(); i++) {
        Gtk::DrawingArea* drawingArea = new Gtk::DrawingArea;
        Gtk::Button* rotateLeftButton = new Gtk::Button;
        Gtk::Button* rotateRightButton = new Gtk::Button;
        // Fill the available space
        drawingArea->signal_draw().connect([=](const Cairo::RefPtr<Cairo::Context>& context) -> bool {
            int height = drawingArea->get_height();
            float ratio = this->pixbufs[i]->get_width() / (float) this->pixbufs[i]->get_height();
            Gdk::Cairo::set_source_pixbuf(context, this->pixbufs[i]->scale_simple(ratio * height, height, Gdk::INTERP_BILINEAR));
            context->paint();
            drawingArea->set_size_request(ratio * height, 0);
            return true;
        });
        drawingArea->set_vexpand(true);
        grid.attach(*drawingArea, i * 2, 0, 2);
        // Rotate the image left when clicked
        rotateLeftButton->signal_clicked().connect([=]() {
            this->pixbufs[i] = this->pixbufs[i]->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE);
            this->grid.queue_draw();
        });
        rotateLeftButton->set_image_from_icon_name("object-rotate-left-symbolic");
        grid.attach(*rotateLeftButton, i * 2, 1);
        // Rotate the image right when clicked
        rotateRightButton->signal_clicked().connect([=]() {
            this->pixbufs[i] = this->pixbufs[i]->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE);
            this->grid.queue_draw();
        });
        rotateRightButton->set_image_from_icon_name("object-rotate-right-symbolic");
        grid.attach(*rotateRightButton, i * 2 + 1, 1);
        if (i > 0) {
            Gtk::Button* leftButton = new Gtk::Button;
            // Move the image left when clicked
            leftButton->signal_clicked().connect([=]() {
                Glib::RefPtr<Gdk::Pixbuf> buffer = this->pixbufs[i];
                this->pixbufs.erase(this->pixbufs.begin() + i);
                this->pixbufs.insert(this->pixbufs.begin() + (i - 1), buffer);
                this->grid.queue_draw();
            });
            leftButton->set_image_from_icon_name("go-previous-symbolic");
            if (i == pixbufs.size() - 1) {
                grid.attach(*leftButton, i * 2, 2, 2);
            } else {
                grid.attach(*leftButton, i * 2, 2);
            }
        }
        if (i < pixbufs.size() - 1) {
            Gtk::Button* rightButton = new Gtk::Button;
            // Move the image right when clicked
            rightButton->signal_clicked().connect([=]() {
                Glib::RefPtr<Gdk::Pixbuf> buffer = this->pixbufs[i];
                this->pixbufs.erase(this->pixbufs.begin() + i);
                this->pixbufs.insert(this->pixbufs.begin() + (i + 1), buffer);
                this->grid.queue_draw();
            });
            rightButton->set_image_from_icon_name("go-next-symbolic");
            if (i == 0) {
                grid.attach(*rightButton, i * 2, 2, 2);
            } else {
                grid.attach(*rightButton, i * 2 + 1, 2);
            }
        }

    }
    grid.show_all();
    this->pixbufs = pixbufs;
}