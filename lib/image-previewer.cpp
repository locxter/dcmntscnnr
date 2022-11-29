#include "image-previewer.hpp"

// Constructors
ImagePreviewer::ImagePreviewer() : Gtk::ScrolledWindow() {
    grid.set_column_spacing(10);
    grid.set_row_spacing(10);
    grid.set_halign(Gtk::ALIGN_CENTER);
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    add(grid);
}

ImagePreviewer::ImagePreviewer(std::vector<Image> images) : ImagePreviewer() {
    setImages(images);
}

// Getter and setter
std::vector<Image> ImagePreviewer::getImages() {
    return images;
}

void ImagePreviewer::setImages(std::vector<Image> images) {
    // Clear the grid
    for (int i = 0; i < images.size() * 2; i++) {
        grid.remove_column(i);
    }
    drawingAreas.clear();
    cropButtons.clear();
    deleteButtons.clear();
    rotateLeftButtons.clear();
    rotateRightButtons.clear();
    moveLeftButtons.clear();
    moveRightButtons.clear();
    // Create the grid
    for (int i = 0; i < images.size(); i++) {
        drawingAreas.push_back(Gtk::DrawingArea());
        cropButtons.push_back(Gtk::Button());
        deleteButtons.push_back(Gtk::Button());
        rotateLeftButtons.push_back(Gtk::Button());
        rotateRightButtons.push_back(Gtk::Button());
        moveLeftButtons.push_back(Gtk::Button());
        moveRightButtons.push_back(Gtk::Button());
        Gtk::Allocation allocation;
        // Fill the available space
        drawingAreas[i].signal_draw().connect([=](const Cairo::RefPtr<Cairo::Context>& context) -> bool {
            int height = this->drawingAreas[i].get_height();
            float aspectRatio = (float) this->images[i].getPixbuf()->get_width() / this->images[i].getPixbuf()->get_height();
            Gdk::Cairo::set_source_pixbuf(context, this->images[i].getPixbuf()->scale_simple(std::round(aspectRatio * height), height, Gdk::INTERP_BILINEAR));
            context->paint();
            this->drawingAreas[i].set_size_request(std::round(aspectRatio * height), -1);
            return true;
        });
        drawingAreas[i].set_vexpand(true);
        grid.attach(drawingAreas[i], i * 2, 0, 2);
        // Delete image when clicked
        deleteButtons[i].signal_clicked().connect([=]() {
            std::vector<Image> buffer = this->images;
            buffer.erase(buffer.begin() + i);
            this->setImages(buffer);
        });
        deleteButtons[i].set_image_from_icon_name("user-trash-symbolic");
        grid.attach(deleteButtons[i], i * 2, 1, 2);
        // Rotate the image left when clicked
        rotateLeftButtons[i].signal_clicked().connect([=]() {
            this->images[i].setPixbuf(this->images[i].getPixbuf()->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE));
            this->grid.queue_draw();
        });
        rotateLeftButtons[i].set_image_from_icon_name("object-rotate-left-symbolic");
        grid.attach(rotateLeftButtons[i], i * 2, 2);
        // Rotate the image right when clicked
        rotateRightButtons[i].signal_clicked().connect([=]() {
            this->images[i].setPixbuf(this->images[i].getPixbuf()->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE));
            this->grid.queue_draw();
        });
        rotateRightButtons[i].set_image_from_icon_name("object-rotate-right-symbolic");
        grid.attach(rotateRightButtons[i], i * 2 + 1, 2);
        if (i > 0) {
            // Move the image left when clicked
            moveLeftButtons[i].signal_clicked().connect([=]() {
                Image buffer = this->images[i];
                this->images.erase(this->images.begin() + i);
                this->images.insert(this->images.begin() + (i - 1), buffer);
                this->grid.queue_draw();
            });
            moveLeftButtons[i].set_image_from_icon_name("go-previous-symbolic");
            if (i == images.size() - 1) {
                grid.attach(moveLeftButtons[i], i * 2, 3, 2);
            } else {
                grid.attach(moveLeftButtons[i], i * 2, 3);
            }
        }
        if (i < images.size() - 1) {
            // Move the image right when clicked
            moveRightButtons[i].signal_clicked().connect([=]() {
                Image buffer = this->images[i];
                this->images.erase(this->images.begin() + i);
                this->images.insert(this->images.begin() + (i + 1), buffer);
                this->grid.queue_draw();
            });
            moveRightButtons[i].set_image_from_icon_name("go-next-symbolic");
            if (i == 0) {
                grid.attach(moveRightButtons[i], i * 2, 3, 2);
            } else {
                grid.attach(moveRightButtons[i], i * 2 + 1, 3);
            }
        }
        // Crop the image manually when clicked
        cropButtons[i].signal_clicked().connect([=]() {
            Gtk::Window* parent = (Gtk::Window*) get_toplevel();
            ImageCropper imageCropper(*parent, this->images[i]);
            int response = imageCropper.run();
            // Handle the response
            if (response == Gtk::RESPONSE_OK) {
                this->images[i] = imageCropper.getImage();
                this->grid.queue_draw();
            }
        });
        cropButtons[i].set_image_from_icon_name("image-crop");
        grid.attach(cropButtons[i], i * 2, 4, 2);
    }
    grid.show_all();
    this->images = images;
}
