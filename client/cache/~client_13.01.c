/* Compile me with:
 *  gcc -o sample4 sample4.c $(pkg-config --cflags --libs gtk+-2.0 gthread-2.0)
 */
#include <gtk/gtk.h>

/* Progress variable and it's associated mutex */
gint progress = 0;
G_LOCK_DEFINE_STATIC(progress);

static gpointer thread_func(gpointer data) {
    gint i;

    for (i = 0; i < 1000; i++) {
        G_LOCK(progress);
        progress = i;
        G_UNLOCK(progress);
    }
}

static gboolean
cb_timeout(gpointer data) {
    gchar *label;

    G_LOCK(progress);
    label = g_strdup_printf("Finished %d of 999", progress);
    G_UNLOCK(progress);

    gtk_button_set_label(GTK_BUTTON(data), label);
    g_free(label);

    return (TRUE);
}

int main(int argc,
         char **argv) {
    GtkWidget *window;
    GtkWidget *button;
    GThread *thread;
    GError *error = NULL;

    /* Secure glib */
    if (!g_thread_supported())
        g_thread_init(NULL);

    /* Do stuff as usual */
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    button = gtk_button_new_with_label("Initial value");
    gtk_container_add(GTK_CONTAINER(window), button);

    gdk_threads_add_timeout(100, cb_timeout, (gpointer)button);

    /* Create new thread */
    thread = g_thread_create(thread_func, (gpointer)button,
                             FALSE, &error);
    if (!thread) {
        g_print("Error: %s\n", error->message);
        return (-1);
    }

    gtk_widget_show_all(window);
    gtk_main();

    return (0);
}