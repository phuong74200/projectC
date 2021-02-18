#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    GtkWidget *window;

    /* Initialise GTK+ passing to it all command line arguments  */
    gtk_init(&argc, &argv);

    /* create a new window, set values */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hello GTK+ World");
    /* set the size of the window */
    gtk_widget_set_size_request(GTK_WIDGET(window), 400, 400);
    /* connect the windows "destroy" event */
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    /* set the window as visible */
    gtk_widget_show(window);

    /* run the GTK+ main loop */
    gtk_main();
    return 0;
}