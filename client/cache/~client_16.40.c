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

int guestLogin() {
    landingPageDisplay();
}

void backToLogin() {
    GObject *productContainer;
    GObject *productForm;

    GObject *grid = gtk_builder_get_object(builder, "registerForm");

    gtk_widget_destroy(grid);

    productContainer = gtk_builder_new_from_file("UI\\login.xml");

    productForm = gtk_builder_get_object(productContainer, "loginPage");

    window = gtk_builder_get_object(builder, "mainWindow");

    gtk_container_add(window, productForm);

    gtk_window_set_title(GTK_WINDOW(window), "Login");

    gtk_widget_show_all(window);

    window = gtk_builder_get_object(productContainer, "backToLogin");
    g_signal_connect(window, "clicked", G_CALLBACK(backToLogin), NULL);

    printf(grid);
    printf(productForm);
};

void registerNow(GtkWidget *widget, gpointer data) {
    GObject *productContainer;
    GObject *productForm;

    GObject *grid = gtk_builder_get_object(builder, "loginPage");

    gtk_widget_destroy(grid);

    productContainer = gtk_builder_new_from_file("UI\\register.xml");

    productForm = gtk_builder_get_object(productContainer, "registerForm");

    window = gtk_builder_get_object(builder, "mainWindow");

    gtk_container_add(window, productForm);

    gtk_window_set_title(GTK_WINDOW(window), "Register");

    gtk_widget_show_all(window);

    window = gtk_builder_get_object(productContainer, "backToLogin");
    g_signal_connect(window, "clicked", G_CALLBACK(backToLogin), NULL);

    printf(grid);
    printf(productForm);
}

void loginScreenDisplay() {
    printf("hello?");

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "UI\\builder.xml", &error);

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

    gtk_window_set_title(GTK_WINDOW(gtk_builder_get_object(builder, "mainWindow")), "Login");

    window = gtk_builder_get_object(builder, "mainWindow");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    window = gtk_builder_get_object(builder, "guestLoginBtn");
    g_signal_connect(window, "clicked", G_CALLBACK(guestLogin), NULL);

    button = gtk_builder_get_object(builder, "loginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(loginEvent), NULL);

    window = gtk_builder_get_object(builder, "registerBtn");
    g_signal_connect(window, "clicked", G_CALLBACK(registerNow), NULL);
}

GObject *container;

void landingPageDisplay() {
    window = gtk_builder_get_object(builder, "loginPage");
    gtk_widget_destroy(window);
    window = gtk_builder_get_object(builder, "landingPage");

    gtk_container_add(container, window);

    gtk_window_set_position(GTK_WINDOW(container), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_title(GTK_WINDOW(container), "Shoply (Guest)");

    gtk_window_set_icon_from_file(GTK_WINDOW(container), "assets\\icon.png", NULL);

    for (int left = 0; left < 3; left++) {
        for (int top = 0; top < 3; top++) {
            addProducts(left, top);
        }
    }
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

    loginScreenDisplay();
    container = gtk_builder_get_object(builder, "mainWindow");

    gtk_main();

    return 0;
}