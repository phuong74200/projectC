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

int isValidStr(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if ((str[i] >= 65 && str[i] <= 90) || (str[i] >= 97 && str[i] <= 122) | (str[i] >= 48 && str[i] <= 57)) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

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

    static char response[40000];

    recv(initSocket, &response, 40000, 0);

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

void loginScreenDisplay();
void registerScreenDisplay();
void landingScreenDisplay();

static void print_hello(GtkWidget *widget, gpointer data) {
    char *response = fetch("<api>fuong</api>");
    printf("%s\n", response);
}

const char *xmlp(char tagName[100], char xmlstr[1000]) {
    char openTag[100] = "";
    char closeTag[100] = "";

    g_strlcpy(openTag, "<", 100);
    g_strlcat(openTag, tagName, 100);
    g_strlcat(openTag, ">", 100);

    g_strlcpy(closeTag, "</", 100);
    g_strlcat(closeTag, tagName, 100);
    g_strlcat(closeTag, ">", 100);

    int xmllen = strlen(xmlstr);

    int oplen = strlen(openTag);
    int cllen = strlen(closeTag);

    int oppos = -1;
    int clpos = -1;

    for (int i = 0; i < xmllen; i++) {
        int cmp = memcmp(openTag, xmlstr + i, strlen(openTag));
        if (cmp == 0) {
            oppos = i;
            break;
        }
    }

    for (int i = 0; i < xmllen; i++) {
        int cmp = memcmp(closeTag, xmlstr + i, strlen(closeTag));
        if (cmp == 0) {
            clpos = i;
            break;
        }
    }

    char *ret = g_malloc(200);

    memset(ret, 0, sizeof(ret));

    int spoint = oppos + strlen(openTag);
    int epoint = clpos + strlen(closeTag) - cllen;

    memset(ret, 0, sizeof(ret));

    int pos = 0;
    for (int i = spoint; i < epoint; i++) {
        ret[pos] = xmlstr[i];
        pos++;
    }

    ret[pos] = '\0';

    return ret;
}

const char *myName() {
    return "Flavio";
}

void loginThread(gpointer data) {
    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }

    send(initSocket, data, strlen(data), 0);

    gchar XMLBuffer[200];

    recv(initSocket, &XMLBuffer, 200, 0);

    closesocket(initSocket);

    gchar *stat = "";
    stat = xmlp("status", XMLBuffer);

    gchar *token = "";
    token = xmlp("token", XMLBuffer);

    printf("%s\n", stat);
    printf("%s\n", token);

    if (strcmp(stat, "login_success") == 0) {
        printf("login sucess\n");
        writeFile("appcache\\token.ctf", token);
        landingScreenDisplay();
    }

    g_thread_exit(0);
}

void loginEvent(GtkWidget *widget, gpointer data) {
    GObject *usernameEntry;
    GObject *passwordEntry;
    usernameEntry = gtk_builder_get_object(builder, "usernameEntry");
    passwordEntry = gtk_builder_get_object(builder, "passwordEntry");

    const gchar *password = gtk_entry_get_text(passwordEntry);
    const gchar *username = gtk_entry_get_text(usernameEntry);

    printf("%s\n", password);
    printf("%s\n", username);

    gchar request[1000] = "<api>login</api><username>";

    strcat(request, username);
    strcat(request, "</username><password>");
    strcat(request, password);
    strcat(request, "</password>");

    g_thread_new("avail", loginThread, request);
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

void landingScreenDisplay() {
    setForm("UI\\landing.xml", "landingPage");
    gtk_window_set_title(GTK_WINDOW(mainWindow), "Shoply (Guest)");
    gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER_ALWAYS);

    button = gtk_builder_get_object(builder, "landingLoginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(loginScreenDisplay), NULL);
    if (checkFile("appcache\\token.ctf") == 1) {
        gtk_button_set_label(button, "log out");

        char userToken[100] = "";
        readFile("appcache\\token.ctf", &userToken);

        char request[200] = "<api>certificate</api><token>";
        strcat(request, userToken);
        strcat(request, "</token>");

        char response[200] = "\0";
        printf("result of token checking: %s\n", response);
        strcpy(response, fetch(request));

        printf("result of token checking: %s\n", response);

        char title[100] = "";
        printf("%s\n", title);
        memset(title, 0, strlen(title));
        printf("%s\n", title);
        strcpy(title, "Shoply (");
        printf("%s\n", title);
        strcat(title, response);
        printf("%s\n", title);
        strcat(title, ")");
        printf("%s\n", title);

        gtk_window_set_title(GTK_WINDOW(mainWindow), title);
    }
}

void registerEvent() {
    GObject *repasswordEntry;
    GObject *rerepasswordEntry;
    GObject *reusernameEntry;
    GObject *log;

    repasswordEntry = gtk_builder_get_object(builder, "repasswordEntry");
    rerepasswordEntry = gtk_builder_get_object(builder, "rerepasswordEntry");
    reusernameEntry = gtk_builder_get_object(builder, "reusernameEntry");

    char *username = gtk_entry_get_text(reusernameEntry);
    char *password = gtk_entry_get_text(repasswordEntry);
    char *repassword = gtk_entry_get_text(rerepasswordEntry);

    printf("%s\n", password);
    printf("%s\n", repassword);

    GtkStyleContext *context;

    if (strcmp(password, repassword) == 0 && gtk_entry_get_text_length(repasswordEntry) > 0 && gtk_entry_get_text_length(rerepasswordEntry) > 0) {
        char request[1000] = "<api>register</api><username>";

        strcat(request, username);
        strcat(request, "</username><password>");
        strcat(request, password);
        strcat(request, "</password>");

        char *response;
        response = fetch(request);

        if (strcmp(response, "success") == 0) {
            log = gtk_builder_get_object(builder, "registerLog1");
            gtk_widget_set_visible(log, FALSE);
            log = gtk_builder_get_object(builder, "registerLog2");
            gtk_widget_set_visible(log, FALSE);
            log = gtk_builder_get_object(builder, "registerLog");
            gtk_label_set_text(log, "Register success");
            gtk_widget_set_visible(log, TRUE);
        } else if (strcmp(response, "existed") == 0) {
            log = gtk_builder_get_object(builder, "registerLog");
            gtk_label_set_text(log, "Username existed");
            gtk_widget_set_visible(log, TRUE);
            log = gtk_builder_get_object(builder, "registerLog1");
            gtk_widget_set_visible(log, FALSE);
            log = gtk_builder_get_object(builder, "registerLog2");
            gtk_widget_set_visible(log, FALSE);
        } else {
            log = gtk_builder_get_object(builder, "registerLog");
            gtk_label_set_text(log, "An error has occured");
            gtk_widget_set_visible(log, TRUE);
            log = gtk_builder_get_object(builder, "registerLog1");
            gtk_widget_set_visible(log, FALSE);
            log = gtk_builder_get_object(builder, "registerLog2");
            gtk_widget_set_visible(log, FALSE);
        }

        printf("%s\n", response);
    } else {
        log = gtk_builder_get_object(builder, "registerLog1");
        gtk_label_set_text(log, "Password not match");
        gtk_widget_set_visible(log, TRUE);

        log = gtk_builder_get_object(builder, "registerLog2");
        gtk_label_set_text(log, "Password not match");
        gtk_widget_set_visible(log, TRUE);
    }
}

void registerScreenDisplay() {
    setForm("UI\\register.xml", "registerForm");

    gtk_window_set_title(GTK_WINDOW(mainWindow), "Register");

    button = gtk_builder_get_object(builder, "rebackToLogin");
    g_signal_connect(button, "clicked", G_CALLBACK(loginScreenDisplay), NULL);

    button = gtk_builder_get_object(builder, "rereguestLoginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(landingScreenDisplay), NULL);

    button = gtk_builder_get_object(builder, "reregisterBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(registerEvent), NULL);
}

void entrychecking() {
    GObject *usernameEntry;
    GObject *passwordEntry;
    usernameEntry = gtk_builder_get_object(builder, "usernameEntry");
    passwordEntry = gtk_builder_get_object(builder, "passwordEntry");

    if (isValidStr(password) == 0) {
        gtk_entry_sett_text(passwordEntry, "");
    }
    if (isValidStr(username) == 0) {
        gtk_entry_sett_text(usernameEntry, "");
    }
}

void loginScreenDisplay() {
    setForm("UI\\login.xml", "loginForm");

    gtk_window_set_title(GTK_WINDOW(mainWindow), "Login");

    button = gtk_builder_get_object(builder, "guestLoginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(landingScreenDisplay), NULL);

    button = gtk_builder_get_object(builder, "loginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(loginEvent), NULL);

    button = gtk_builder_get_object(builder, "registerBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(registerScreenDisplay), NULL);

    GObject *usernameEntry;
    GObject *passwordEntry;

    usernameEntry = gtk_builder_get_object(builder, "usernameEntry");
    passwordEntry = gtk_builder_get_object(builder, "passwordEntry");

    g_signal_connect(usernameEntry, "delete-text", G_CALLBACK(entrychecking), NULL);
    g_signal_connect(passwordEntry, "delete-text", G_CALLBACK(entrychecking), NULL);
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

void quitapp() {
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

    closesocket(initSocket);

    gtk_main_quit();
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

    g_signal_connect(mainWindow, "destroy", G_CALLBACK(quitapp), NULL);

    landingScreenDisplay();

    gtk_main();

    return 0;
}