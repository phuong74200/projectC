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

GObject *window;
GObject *button;
GObject *mainWindow;

GObject *loginForm;
GObject *registerForm;

GError *error = NULL;

GtkBuilder *builder;

static void print_hello(GtkWidget *widget, gpointer data) {
    char *response = fetch("<api>fuong</api>");
    printf("%s\n", response);
}

void loginEvent(GtkWidget *widget, gpointer data) {
    GObject *usernameEntry;
    GObject *passwordEntry;
    usernameEntry = gtk_builder_get_object(builder, "usernameEntry");
    passwordEntry = gtk_builder_get_object(builder, "passwordEntry");

    char *password = gtk_entry_get_text(passwordEntry);
    char *username = gtk_entry_get_text(usernameEntry);

    printf("%s\n", password);
    printf("%s\n", username);

    char request[1000] = "<api>login</api><username>";

    strcat(request, username);
    strcat(request, "</username><password>");
    strcat(request, password);
    strcat(request, "</password>");

    char *response;
    response = fetch(request);

    printf("%s\n", response);
}

void clearAllForms() {
    GObject *form;
    form = gtk_builder_get_object(builder, "loginForm");
    gtk_widget_destroy(form);
    form = gtk_builder_get_object(builder, "registerForm");
    gtk_widget_destroy(form);
    form = gtk_builder_get_object(builder, "landingPage");
    gtk_widget_destroy(form);
}

void setForm(char *path, char *id) {
    clearAllForms();
    gtk_builder_add_from_file(builder, path, &error);
    GObject *form = gtk_builder_get_object(builder, id);
    gtk_container_add(GTK_CONTAINER(mainWindow), form);
}

void guestLogin() {
    setForm("UI\\landing.xml", "landingPage");
}

void loginScreenDisplay() {
    gtk_builder_add_from_file(builder, "UI\\login.xml", &error);

    GObject *login = gtk_builder_get_object(builder, "loginForm");

    gtk_container_add(GTK_CONTAINER(mainWindow), login);

    button = gtk_builder_get_object(builder, "guestLoginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(guestLogin), NULL);
}

void addProducts(int left, int top) {
    GObject *productContainer;
    GObject *productForm;

    GObject *grid = gtk_builder_get_object(builder, "productsGrid");

    productContainer = gtk_builder_new_from_file("UI\\product.xml");

    productForm = gtk_builder_get_object(productContainer, "productForm");

    gtk_grid_attach(grid, productForm, left, top, 1, 1);

    gtk_widget_show_all(grid);

    printf(grid);
    printf(productForm);
}

int main(int argc, char *argv[]) {
    // init socket
    initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    gtk_builder_add_from_file(builder, "UI\\builder.xml", &error);
    gtk_builder_add_from_file(builder, "UI\\login.xml", &error);
    gtk_builder_add_from_file(builder, "UI\\register.xml", &error);
    gtk_builder_add_from_file(builder, "UI\\landing.xml", &error);

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

    mainWindow = gtk_builder_get_object(builder, "mainWindow");

    g_signal_connect(mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    loginScreenDisplay();

    gtk_main();

    return 0;
}