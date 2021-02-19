#include <conio.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <winsock2.h>

#include "ulities.c"

#define PORT 8080

// server's local IPv4 address

#define ADDR "192.168.1.14"

void slog(char *content) {
    printf("[client]: %s.\n", content);
}

WSADATA wsaData;

SOCKET initSocket;

int initResult;

char *fetch(char message[200]) {
    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (initSocket == INVALID_SOCKET) {
        return "init_socket_failed";
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }

    send(initSocket, message, strlen(message), 0);

    static char response[200];

    recv(initSocket, &response, 200, 0);

    closesocket(initSocket);

    return response;
}

static void
print_hello(GtkWidget *widget, gpointer data) {
    char *response = fetch("<api>fuong</api>");
    printf("%s\n", response);
}

int main(int argc, char *argv[]) {
    // init socket

    initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // XML loader

    GtkBuilder *builder;
    GObject *window;
    GObject *button;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "UI\\builder.xml", &error);

    // gtk_widget_set_size_request(window, 300, 500);

    // CSS loader

    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *css_relpath = "css\\style.css";
    GError *error1 = NULL;
    GFile *css_file = g_file_new_for_path(css_relpath);

    gtk_css_provider_load_from_file(provider, css_file, &error1);

    // Code goes following

    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // button = gtk_builder_get_object(builder, "button1");
    // g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

    // button = gtk_builder_get_object(builder, "button2");
    // g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

    button = gtk_builder_get_object(builder, "quit");
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}