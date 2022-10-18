#include "image-previewer.hpp"

// Constructor
ImagePreviewer::ImagePreviewer() : Gtk::ScrolledWindow() {
    grid.set_column_spacing(10);
    grid.set_row_spacing(10);
    grid.set_halign(Gtk::ALIGN_CENTER);
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    add(grid);
}

// Getter and setter
std::vector<Glib::RefPtr<Gdk::Pixbuf>> ImagePreviewer::getPixbufs() {
    return pixbufs;
}

void ImagePreviewer::setPixbufs(std::vector<Glib::RefPtr<Gdk::Pixbuf>> pixbufs) {
    // Clear the grid
    for (int i = 0; i < pixbufs.size() * 2; i++) {
        grid.remove_column(i);
    }
    drawingAreas.clear();
    deleteButtons.clear();
    rotateLeftButtons.clear();
    rotateRightButtons.clear();
    moveLeftButtons.clear();
    moveRightButtons.clear();
    // Create the grid
    for (int i = 0; i < pixbufs.size(); i++) {
        drawingAreas.push_back(Gtk::DrawingArea());
        deleteButtons.push_back(Gtk::Button());
        rotateLeftButtons.push_back(Gtk::Button());
        rotateRightButtons.push_back(Gtk::Button());
        moveLeftButtons.push_back(Gtk::Button());
        moveRightButtons.push_back(Gtk::Button());
        Gtk::Allocation allocation;
        // Fill the available space
        drawingAreas[i].signal_draw().connect([=](const Cairo::RefPtr<Cairo::Context>& context) -> bool {
            int height = this->drawingAreas[i].get_height();
            float aspectRatio = (float) this->pixbufs[i]->get_width() / this->pixbufs[i]->get_height();
            Gdk::Cairo::set_source_pixbuf(context, this->pixbufs[i]->scale_simple(std::round(aspectRatio * height), height, Gdk::INTERP_BILINEAR));
            context->paint();
            this->drawingAreas[i].set_size_request(std::round(aspectRatio * height), -1);
            return true;
        });
        drawingAreas[i].set_vexpand(true);
        grid.attach(drawingAreas[i], i * 2, 0, 2);
        // Delete image when clicked
        deleteButtons[i].signal_clicked().connect([=]() {
            std::vector<Glib::RefPtr<Gdk::Pixbuf>> buffer = this->pixbufs;
            buffer.erase(buffer.begin() + i);
            this->setPixbufs(buffer);
        });
        deleteButtons[i].set_image_from_icon_name("user-trash-symbolic");
        grid.attach(deleteButtons[i], i * 2, 1, 2);
        // Rotate the image left when clicked
        rotateLeftButtons[i].signal_clicked().connect([=]() {
            this->pixbufs[i] = this->pixbufs[i]->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE);
            this->grid.queue_draw();
        });
        rotateLeftButtons[i].set_image_from_icon_name("object-rotate-left-symbolic");
        grid.attach(rotateLeftButtons[i], i * 2, 2);
        // Rotate the image right when clicked
        rotateRightButtons[i].signal_clicked().connect([=]() {
            this->pixbufs[i] = this->pixbufs[i]->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE);
            this->grid.queue_draw();
        });
        rotateRightButtons[i].set_image_from_icon_name("object-rotate-right-symbolic");
        grid.attach(rotateRightButtons[i], i * 2 + 1, 2);
        if (i > 0) {
            // Move the image left when clicked
            moveLeftButtons[i].signal_clicked().connect([=]() {
                Glib::RefPtr<Gdk::Pixbuf> buffer = this->pixbufs[i];
                this->pixbufs.erase(this->pixbufs.begin() + i);
                this->pixbufs.insert(this->pixbufs.begin() + (i - 1), buffer);
                this->grid.queue_draw();
            });
            moveLeftButtons[i].set_image_from_icon_name("go-previous-symbolic");
            if (i == pixbufs.size() - 1) {
                grid.attach(moveLeftButtons[i], i * 2, 3, 2);
            } else {
                grid.attach(moveLeftButtons[i], i * 2, 3);
            }
        }
        if (i < pixbufs.size() - 1) {
            // Move the image right when clicked
            moveRightButtons[i].signal_clicked().connect([=]() {
                Glib::RefPtr<Gdk::Pixbuf> buffer = this->pixbufs[i];
                this->pixbufs.erase(this->pixbufs.begin() + i);
                this->pixbufs.insert(this->pixbufs.begin() + (i + 1), buffer);
                this->grid.queue_draw();
            });
            moveRightButtons[i].set_image_from_icon_name("go-next-symbolic");
            if (i == 0) {
                grid.attach(moveRightButtons[i], i * 2, 3, 2);
            } else {
                grid.attach(moveRightButtons[i], i * 2 + 1, 3);
            }
        }
    }
    grid.show_all();
    this->pixbufs = pixbufs;
}
