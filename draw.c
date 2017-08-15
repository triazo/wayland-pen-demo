#include <gtk/gtk.h>

/*
 *  Source initially from
 *  https://developer.gnome.org/gtk3/stable/ch01s05.html
 */

static cairo_surface_t* surface = NULL;

static void clear_surface(void) {
cairo_t* cr;

    // Create a layer linked to the surface.
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1,1,1);
    // Set surface to cr
    cairo_paint(cr);

    cairo_destroy(cr);

}

static gboolean configure_event_cb(GtkWidget* widget, GdkEventConfigure* event, gpointer data) {
    if (surface)
        cairo_surface_destroy(surface);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
                                                gtk_widget_get_allocated_width(widget),
                                                gtk_widget_get_allocated_height(widget));

    clear_surface();

    return TRUE;
}


// TODO: investigate if this is even used
static gboolean draw_cb (GtkWidget* widget, cairo_t* cr, gpointer data) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return FALSE;
}

static void draw_brush(GtkWidget* widget, gdouble x, gdouble y) {
    cairo_t *cr;

    cr = cairo_create(surface);
    cairo_rectangle(cr, x-3, y-3, 6, 6);
    cairo_fill(cr);
    cairo_destroy(cr);

    gtk_widget_queue_draw_area(widget, x-3, y-3, 6, 6);
}


static gboolean button_press_event_cb(GtkWidget* widget, GdkEventButton *event, gpointer data) {
    if (surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY) {
        draw_brush(widget, event->x, event->y);
    }
    else if (event->button == GDK_BUTTON_SECONDARY) {
        clear_surface();
        gtk_widget_queue_draw(widget);
    }

    return TRUE;
}

// Runs on every mouse move or something
static gboolean motion_notify_event_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    if (surface == NULL)
        return FALSE;

    // Test if the primary mouse button is held down
    if (event->state & GDK_BUTTON1_MASK)
        draw_brush(widget, event->x, event->y);

    return TRUE;
}

static void close_window(void) {
    if (surface)
        cairo_surface_destroy(surface);
}

static void activate (GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window), 8);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(window), frame);

    drawing_area = gtk_drawing_area_new();

    gtk_widget_set_size_request(drawing_area, 1920, 1080);

    gtk_container_add(GTK_CONTAINER(frame), drawing_area);

    // SIGNALS for backing surface
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_cb), NULL);
    g_signal_connect(drawing_area, "configure-event", G_CALLBACK(configure_event_cb), NULL);

    // Signals for events
    g_signal_connect(drawing_area, "motion-notify-event", G_CALLBACK(motion_notify_event_cb), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(button_press_event_cb), NULL);

    gtk_widget_set_events(drawing_area,
                          gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);

    gtk_widget_show_all(window);
}



int main (int argc, char** argv) {
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.triazo.pendemo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
