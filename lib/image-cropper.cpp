#include "image-cropper.hpp"

// Method to crop the image on response
void ImageCropper::on_response(int response) {
    if (response == Gtk::RESPONSE_OK) {
        int width = image.getPixbuf()->get_width();
        int height = image.getPixbuf()->get_height();
        Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
        Gdk::Cairo::set_source_pixbuf(context, image.getPixbuf());
        context->move_to(width * corners[0][0], height - (corners[0][1] * height));
        context->line_to(width * corners[1][0], height - (corners[1][1] * height));
        context->line_to(width * corners[2][0], height - (corners[2][1] * height));
        context->line_to(width * corners[3][0], height - (corners[3][1] * height));
        context->close_path();
        context->clip();
        context->paint();
        setImage(Gdk::Pixbuf::create(surface, 0, 0, width, height));
    }
}

// Constructors
ImageCropper::ImageCropper(Gtk::Window& parent) : Gtk::Dialog("Image cropper", parent, true) {
    drawingArea.set_vexpand(true);
    drawingArea.add_events(Gdk::BUTTON_PRESS_MASK);
    // Update the corner position on mouse click
    drawingArea.signal_button_press_event().connect([&](GdkEventButton* event) -> bool {
        if (event->type == GDK_BUTTON_PRESS) {
            int width = drawingArea.get_width();
            int height = drawingArea.get_height();
            double clickX = event->x;
            double clickY = height - event->y;
            int corner = 0;
            if (clickX > width / 2.0 && clickY <= height / 2.0) {
                // Right bottom
                corner = 1;
            } else if (clickX <= width / 2.0 && clickY <= height / 2.0) {
                // Left bottom
                corner = 2;
            } else if (clickX <= width / 2.0 && clickY > height / 2.0) {
                // Left top
                corner = 3;
            }
            corners[corner][0] = clickX / width;
            corners[corner][1] = clickY / height;
            get_content_area()->queue_draw();
        }
        return true;
    });
    // Fill the available space and draw the image with an overlay
    drawingArea.signal_draw().connect([&](const Cairo::RefPtr<Cairo::Context>& context) -> bool {
        int height = drawingArea.get_height();
        float aspectRatio = (float) image.getPixbuf()->get_width() / image.getPixbuf()->get_height();
        Gdk::Cairo::set_source_pixbuf(context, image.getPixbuf()->scale_simple(std::round(aspectRatio * height), height, Gdk::INTERP_BILINEAR));
        context->paint();
        // Draw the selected area
        context->set_source_rgba(0, 255, 0, 0.2);
        context->move_to(aspectRatio * height * corners[0][0], height - (corners[0][1] * height));
        context->line_to(aspectRatio * height * corners[1][0], height - (corners[1][1] * height));
        context->line_to(aspectRatio * height * corners[2][0], height - (corners[2][1] * height));
        context->line_to(aspectRatio * height * corners[3][0], height - (corners[3][1] * height));
        context->close_path();
        context->fill();
        // Draw the corners
        for (int i = 0; i < 4; i++) {
            context->set_source_rgb(0, 255, 0);
            context->arc(aspectRatio * height * corners[i][0], height - (corners[i][1] * height), std::max(height * 0.005, 4.0), 0, 2 * M_PI);
            context->fill();
        }
        drawingArea.set_size_request(std::round(aspectRatio * height), -1);
        return true;
    });
    get_content_area()->set_halign(Gtk::ALIGN_CENTER);
    get_content_area()->add(drawingArea);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Crop", Gtk::RESPONSE_OK);
    set_border_width(10);
    set_default_size(640, 640);
    set_size_request(480, 480);
    show_all();
}

ImageCropper::ImageCropper(Gtk::Window& parent, Image image) : ImageCropper(parent) {
    setImage(image);
}

// Getter and setter
Image ImageCropper::getImage() {
    return image;
}

void ImageCropper::setImage(Image image) {
    this->image = image;
    get_content_area()->queue_draw();
}
