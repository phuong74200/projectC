#include <conio.h>
#include <dirent.h>
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

char accountToken[100] = "";

char glbChar[1000] = "";

void gc(char data[1000]) {
    memset(glbChar, 0, strlen(glbChar));
    strcpy(glbChar, data);
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

    static char response[40000] = "";

    for (int i = 0; i < sizeof(response) / sizeof(response[0]); i++) {
        response[i] = NULL;
    }

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

int loginThread(gpointer data) {
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
    printf("%s\n", XMLBuffer);

    if (strcmp(stat, "login_success") == 0) {
        printf("login sucess\n");
        writeFile("appcache\\token.ctf", token);
        landingScreenDisplay();
    } else {
        button = gtk_builder_get_object(builder, "loginLog");
        gtk_label_set_text(button, "Wrong username or password");
        gtk_widget_set_visible(button, TRUE);
    }

    return FALSE;
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

    gchar request[1000] = "";
    memset(request, 0, strlen(request));

    strcpy(request, "<api>login</api><username>");
    strcat(request, username);
    strcat(request, "</username><password>");
    strcat(request, password);
    strcat(request, "</password>");

    int validUsername = 0;
    int validPassword = 0;

    for (int i = 0; i < strlen(username) - 1; i++) {
        int isAl = g_ascii_isalpha(username[i]);
        int isDg = g_ascii_isdigit(username[i]);
        if ((isAl == 0 && isDg == 0) || strlen(username) < 6) {
            validUsername = 0;
            break;
        } else {
            validUsername = 1;
        }
    }

    for (int i = 0; i < strlen(password) - 1; i++) {
        int isAl = g_ascii_isalpha(password[i]);
        int isDg = g_ascii_isdigit(password[i]);
        if ((isAl == 0 && isDg == 0) || strlen(password) < 6) {
            validPassword = 0;
            break;
        } else {
            validPassword = 1;
        }
    }

    if (validPassword == 1 && validUsername == 1) {
        gdk_threads_add_idle(loginThread, request);
    } else {
        printf("not valid\n");
        button = gtk_builder_get_object(builder, "loginLog");
        gtk_label_set_text(button, "Wrong username or password");
        gtk_widget_set_visible(button, TRUE);
    }
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

GtkBuilder *admin_users_manage_window;
int userPosition = 0;

GObject *adm_user;

void changeRole(GtkButton *btn, gpointer data) {
    char value[100] = "";
    strcpy(value, gtk_button_get_label(btn));

    GtkWidget *target = gtk_grid_get_child_at(adm_user, 1, GPOINTER_TO_INT(data));
    printf("%s", gtk_label_get_text(target));

    char username[100] = "";

    strcpy(username, gtk_label_get_text(target));

    char request[1000] = "<api>switchRole</api><token>";
    strcat(request, accountToken);
    strcat(request, "</token><username>");
    strcat(request, username);
    strcat(request, "</username>");

    if (!strcmp(username, "(deleted)") == 0) {
        if (strcmp(value, "admin") == 0) {
            gtk_button_set_label(btn, "user");
        } else {
            gtk_button_set_label(btn, "admin");
        }
        printf("change it\n");
        fetch(request);
    }
}

void none() {
    return 0;
}

void deleteUser(GtkButton *btn, gpointer data) {
    GtkWidget *target = gtk_grid_get_child_at(adm_user, 1, GPOINTER_TO_INT(data));
    printf("%s", gtk_label_get_text(target));

    char username[100] = "";

    strcpy(username, gtk_label_get_text(target));

    char request[1000] = "<api>deleteUser</api><token>";
    strcat(request, accountToken);
    strcat(request, "</token><username>");
    strcat(request, username);
    strcat(request, "</username>");

    if (!strcmp(username, "(deleted)") == 0) {
        printf("delete it\n");
        fetch(request);
    }

    GtkWidget *deltarget;

    deltarget = gtk_grid_get_child_at(adm_user, 0, GPOINTER_TO_INT(data));
    gtk_label_set_text(deltarget, "(deleted)");

    deltarget = gtk_grid_get_child_at(adm_user, 1, GPOINTER_TO_INT(data));
    gtk_label_set_text(deltarget, "(deleted)");

    deltarget = gtk_grid_get_child_at(adm_user, 2, GPOINTER_TO_INT(data));
    gtk_label_set_text(deltarget, "(deleted)");

    deltarget = gtk_grid_get_child_at(adm_user, 3, GPOINTER_TO_INT(data));
    gtk_label_set_text(deltarget, "(deleted)");

    deltarget = gtk_grid_get_child_at(adm_user, 4, GPOINTER_TO_INT(data));
    gtk_button_set_label(deltarget, "(deleted)");

    deltarget = gtk_grid_get_child_at(adm_user, 5, GPOINTER_TO_INT(data));
    gtk_button_set_label(deltarget, "unable");
}

int listLength = 15;
int currentListLen = 0;
int loadEnd = 1;
int deletedUser = 0;

GtkStyleContext *context;
GtkWidget *slot;

int adm_getUserThread() {
    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }

    send(initSocket, glbChar, strlen(glbChar), 0);

    gchar XMLBuffer[200];

    recv(initSocket, &XMLBuffer, 2000, 0);

    closesocket(initSocket);

    printf("%s", XMLBuffer);

    int top = atoi(xmlp("index", glbChar)) + 1;

    adm_user = gtk_builder_get_object(admin_users_manage_window, "adm_user_grid");

    if (strcmp(xmlp("username", XMLBuffer), "(none)") != 0 && strlen(XMLBuffer) > 0) {
        slot = gtk_label_new(xmlp("index", glbChar));
        gtk_grid_attach(adm_user, slot, 0, top, 1, 1);
        context = gtk_widget_get_style_context(slot);
        gtk_style_context_add_class(context, "adm_user_slot");
        gtk_widget_set_hexpand(slot, TRUE);

        slot = gtk_label_new(xmlp("username", XMLBuffer));
        gtk_grid_attach(adm_user, slot, 1, top, 1, 1);
        context = gtk_widget_get_style_context(slot);
        gtk_style_context_add_class(context, "adm_user_slot");
        gtk_widget_set_hexpand(slot, TRUE);

        slot = gtk_label_new(xmlp("token", XMLBuffer));
        gtk_grid_attach(adm_user, slot, 2, top, 1, 1);
        context = gtk_widget_get_style_context(slot);
        gtk_style_context_add_class(context, "adm_user_slot");
        gtk_widget_set_hexpand(slot, TRUE);

        slot = gtk_label_new(xmlp("password", XMLBuffer));
        gtk_grid_attach(adm_user, slot, 3, top, 1, 1);
        context = gtk_widget_get_style_context(slot);
        gtk_style_context_add_class(context, "adm_user_slot");
        gtk_widget_set_hexpand(slot, TRUE);

        slot = gtk_button_new_with_label(xmlp("role", XMLBuffer));
        gtk_grid_attach(adm_user, slot, 4, top, 1, 1);
        g_signal_connect(slot, "clicked", G_CALLBACK(changeRole), GINT_TO_POINTER(top));
        context = gtk_widget_get_style_context(slot);
        gtk_style_context_add_class(context, "adm_user_slot_btn");
        gtk_widget_set_hexpand(slot, TRUE);

        if (strcmp(xmlp("role", XMLBuffer), "user") == 0) {
            slot = gtk_button_new_with_label("delete");
            gtk_grid_attach(adm_user, slot, 5, top, 1, 1);
            g_signal_connect(slot, "clicked", G_CALLBACK(deleteUser), GINT_TO_POINTER(top));
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_del_btn");
            gtk_widget_set_hexpand(slot, TRUE);
        } else {
            slot = gtk_button_new_with_label("unable");
            gtk_grid_attach(adm_user, slot, 5, top, 1, 1);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_del_btn");
            gtk_widget_set_hexpand(slot, TRUE);
        }
    } else {
        listLength++;
    }

    printf("buffer: %s\n", XMLBuffer);

    if (userPosition < listLength) {
        printf("pos: %d\n", userPosition);
        gchar request[1000] = "<api>getUserList</api><index>";
        char index[4] = "";
        itoa(userPosition, index, 10);
        strcat(request, index);
        strcat(request, "</index><token>");
        strcat(request, accountToken);
        strcat(request, "</token>");
        gc(request);
        userPosition++;
        printf("pos: %s\n", request);
        gdk_threads_add_timeout(10 + listLength, adm_getUserThread, "");
    }

    gtk_widget_show_all(adm_user);

    return FALSE;
}

int scollable = 1;

void enableScroll() {
    scollable = 1;
    return FALSE;
}

void admUserEdgeReach(GtkScrolledWindow *scrolled_window, GtkPositionType pos, gpointer user_data) {
    if (scollable == 1 && pos == GTK_POS_BOTTOM) {
        scollable = 0;
        listLength += 5;
        gdk_threads_add_timeout(100, adm_getUserThread, "");
        gdk_threads_add_timeout(500, enableScroll, "");
    }
}

int addItemThread(gpointer data) {
    char clstr[1000];
    char cstr[100];
    GObject *obj;

    strcpy(clstr, "<name>");
    obj = gtk_builder_get_object(admin_users_manage_window, "productNameEntry");
    char name[100];
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</name>");

    GdkRGBA *color1;

    strcat(clstr, "<color1>");
    obj = gtk_builder_get_object(admin_users_manage_window, "color1");
    gtk_color_chooser_get_rgba(obj, color1);
    itoa((int)(color1->red * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->green * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->blue * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, "</color1>");

    strcat(clstr, "<color2>");
    obj = gtk_builder_get_object(admin_users_manage_window, "color2");
    gtk_color_chooser_get_rgba(obj, color1);
    itoa((int)(color1->red * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->green * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->blue * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, "</color2>");

    strcat(clstr, "<color3>");
    obj = gtk_builder_get_object(admin_users_manage_window, "color3");
    gtk_color_chooser_get_rgba(obj, color1);
    itoa((int)(color1->red * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->green * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, ",");
    itoa((int)(color1->blue * 255), cstr, 10);
    strcat(clstr, cstr);
    strcat(clstr, "</color3>");

    obj = gtk_builder_get_object(admin_users_manage_window, "productPriceEntry");
    strcat(clstr, "<price>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</price>");

    obj = gtk_builder_get_object(admin_users_manage_window, "productSaleEntry");
    strcat(clstr, "<sale>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</sale>");

    obj = gtk_builder_get_object(admin_users_manage_window, "productDescript");
    strcat(clstr, "<des>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</des>");

    obj = gtk_builder_get_object(admin_users_manage_window, "productTags");
    strcat(clstr, "<tag>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</tag>");

    strcat(clstr, "<api>addProduct</api>");

    fetch(clstr);

    printf("%s", clstr);
    return FALSE;
}

int addItemDelay = 1;

int resetItemDelay() {
    addItemDelay = 1;
    GObject *obj = gtk_builder_get_object(admin_users_manage_window, "addItemBtn");
    gtk_button_set_label(obj, "Add item to shop");
    return FALSE;
}

void addItemEvent(gpointer data) {
    GObject *obj;

    if (addItemDelay == 1) {
        printf("addItem!");
        addItemDelay = 0;
        gdk_threads_add_timeout(2500, resetItemDelay, "");
        obj = gtk_builder_get_object(admin_users_manage_window, "addItemBtn");
        gtk_button_set_label(obj, "Please wait 2.5s to click again");

        int isValid = 0;
        char *content = "";
        obj = gtk_builder_get_object(admin_users_manage_window, "productPriceEntry");
        content = gtk_entry_get_text(obj);
        if (strlen(content) > 0) {
            for (int i = 0; i < strlen(content); i++) {
                int isDg = g_ascii_isdigit(content[i]);
                if (isDg == 0) {
                    isValid++;
                    obj = gtk_builder_get_object(admin_users_manage_window, "saleoutDes");
                    gtk_label_set_text(obj, "Set price (must be an integer!)");
                    break;
                } else {
                    obj = gtk_builder_get_object(admin_users_manage_window, "saleoutDes");
                    gtk_label_set_text(obj, "Set price");
                }
            }
        } else {
            isValid++;
            obj = gtk_builder_get_object(admin_users_manage_window, "saleoutDes");
            gtk_label_set_text(obj, "Set price (can't leave blank)");
        }

        obj = gtk_builder_get_object(admin_users_manage_window, "productSaleEntry");
        content = gtk_entry_get_text(obj);
        if (strlen(content) > 0) {
            for (int i = 0; i < strlen(content); i++) {
                int isDg = g_ascii_isdigit(content[i]);
                if (isDg == 0) {
                    isValid++;
                    obj = gtk_builder_get_object(admin_users_manage_window, "productSaleEntry");
                    gtk_label_set_text(obj, "Set sale value (must be an integer!)");
                    break;
                } else {
                    obj = gtk_builder_get_object(admin_users_manage_window, "saleoutDes1");
                    gtk_label_set_text(obj, "Set sale value");
                }
            }
        } else {
            isValid++;
            obj = gtk_builder_get_object(admin_users_manage_window, "saleoutDes1");
            gtk_label_set_text(obj, "Set sale value (can't leave blank)");
        }

        obj = gtk_builder_get_object(admin_users_manage_window, "productNameEntry");
        content = gtk_entry_get_text(obj);
        if (strlen(content) > 0) {
            for (int i = 0; i < strlen(content); i++) {
                int isDg = g_ascii_isdigit(content[i]);
                int isAl = g_ascii_isalpha(content[i]);
                int isSp = g_ascii_isspace(content[i]);
                if (isDg == 0 && isAl == 0 && isSp == 0) {
                    isValid++;
                    obj = gtk_builder_get_object(admin_users_manage_window, "nameDes");
                    gtk_label_set_text(obj, "Name for item (Can't leave blank and not include special characters)");
                    break;
                } else {
                    obj = gtk_builder_get_object(admin_users_manage_window, "nameDes");
                    gtk_label_set_text(obj, "Name for item");
                }
            }
        } else {
            isValid++;
            obj = gtk_builder_get_object(admin_users_manage_window, "nameDes");
            gtk_label_set_text(obj, "Name for item (can't leave blank)");
        }

        obj = gtk_builder_get_object(admin_users_manage_window, "productTags");
        content = gtk_entry_get_text(obj);

        for (int i = 0; i < strlen(content); i++) {
            int isDg = g_ascii_isdigit(content[i]);
            int isAl = g_ascii_isalpha(content[i]);
            if (isDg == 0 && isAl == 0) {
                isValid++;
                obj = gtk_builder_get_object(admin_users_manage_window, "tagsDes");
                gtk_label_set_text(obj, "Tags (using for user to search) (not include special characters)");
                break;
            } else {
                obj = gtk_builder_get_object(admin_users_manage_window, "tagsDes");
                gtk_label_set_text(obj, "Tags (using for user to search)");
            }
        }

        printf("isvalid: %d", isValid);

        if (isValid == 0) {
            gdk_threads_add_timeout(100, addItemThread, "");
        }
    }
}

void admin_users_manage() {
    userPosition = 0;
    admin_users_manage_window = gtk_builder_new();
    admin_users_manage_window = gtk_builder_new_from_file("UI\\user_manage.xml");
    GObject *adminWindow = gtk_builder_get_object(admin_users_manage_window, "mainWindow");

    GObject *scollWindow = gtk_builder_get_object(admin_users_manage_window, "adm_scoll_window");
    g_signal_connect(scollWindow, "edge-reached", G_CALLBACK(admUserEdgeReach), "");

    GObject *addItemBtn = gtk_builder_get_object(admin_users_manage_window, "addItemBtn");
    g_signal_connect(addItemBtn, "clicked", G_CALLBACK(addItemEvent), "");

    gtk_window_set_modal(adminWindow, TRUE);
    gtk_widget_show_all(adminWindow);
    gchar request[1000] = "<api>getUserList</api><index>0</index><token>";
    strcat(request, accountToken);
    strcat(request, "</token>");
    gc(request);
    if (userPosition <= 1) {
        gdk_threads_add_timeout(250, adm_getUserThread, "");
    }
}

int addProductsThread(gpointer top) {
    int index = GPOINTER_TO_INT(top);

    char request[300] = "<api>getProduct</api><index>";

    char sindex[30];
    itoa(index, sindex, 10);

    strcat(request, sindex);
    strcat(request, "</index>");

    char *response = "";

    response = fetch(request);

    GObject *productContainer;
    GObject *productForm;
    GObject *obj;

    GObject *grid = gtk_builder_get_object(builder, "productsGrid");

    productContainer = gtk_builder_new_from_file("UI\\product.xml");

    productForm = gtk_builder_get_object(productContainer, "productForm");

    gtk_grid_attach(grid, productForm, 0, index, 1, 1);

    gtk_widget_show_all(grid);

    obj = gtk_builder_get_object(builder, "product_name");
    gtk_label_set_text(obj, xmlp("name", response));

    printf("%s\n", response);
    printf("%s\n", request);

    return FALSE;
}

void landingScreenDisplay() {
    setForm("UI\\landing.xml", "landingPage");
    gtk_window_set_title(GTK_WINDOW(mainWindow), "Shoply");
    gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER_ALWAYS);

    button = gtk_builder_get_object(builder, "landingLoginBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(loginScreenDisplay), NULL);
    if (checkFile("appcache\\token.ctf") == 1) {
        char userToken[100] = "";
        readFile("appcache\\token.ctf", &userToken);

        strcpy(accountToken, userToken);

        char request[200] = "<api>certificate</api><token>";
        strcat(request, userToken);
        strcat(request, "</token>");

        char *response = "";

        response = fetch(request);

        printf("result of token checking: %s\n", response);

        if (strcmp(response, "not_exist") == 0) {
            gtk_button_set_label(button, "log in");
            printf("token rejected\n");
        } else {
            gtk_button_set_label(button, "log out");
            if (strcmp(response, "admin") == 0) {
                button = gtk_builder_get_object(builder, "ad_users");
                gtk_widget_show(button);
                g_signal_connect(button, "clicked", G_CALLBACK(admin_users_manage), NULL);

                button = gtk_builder_get_object(builder, "ad_products");
                gtk_widget_show(button);
            }
            printf("token accepted\n");
        }
    }
    gdk_threads_add_timeout(250, addProductsThread, GINT_TO_POINTER(1));
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

    int validUsername = 0;
    int validPassword = 0;

    printf("len: %d\n", strlen(password));
    printf("len: %d\n", strlen(username));

    for (int i = 0; i < strlen(password) - 1; i++) {
        int isAl = g_ascii_isalpha(password[i]);
        int isDg = g_ascii_isdigit(password[i]);
        if ((isAl == 0 && isDg == 0) || strlen(password) < 6) {
            validPassword = 0;
            break;
        } else {
            validUsername = 1;
        }
    }

    for (int i = 0; i < strlen(username) - 1; i++) {
        int isAl = g_ascii_isalpha(username[i]);
        int isDg = g_ascii_isdigit(username[i]);
        if ((isAl == 0 && isDg == 0) || strlen(username) < 6) {
            validUsername = 0;
            break;
        } else {
            validPassword = 1;
        }
    }

    if (strcmp(password, repassword) == 0 && gtk_entry_get_text_length(repasswordEntry) > 0 && gtk_entry_get_text_length(rerepasswordEntry) > 0 && validUsername == 1 && validPassword == 1) {
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
    } else if (strlen(username) < 6 || strlen(password) < 6) {
        log = gtk_builder_get_object(builder, "registerLog");
        gtk_label_set_text(log, "Username or password length smaller than 6");
        gtk_widget_set_visible(log, TRUE);
        log = gtk_builder_get_object(builder, "registerLog1");
        gtk_widget_set_visible(log, FALSE);
        log = gtk_builder_get_object(builder, "registerLog2");
        gtk_widget_set_visible(log, FALSE);
    } else if (validPassword == 0 || validUsername == 0) {
        log = gtk_builder_get_object(builder, "registerLog");
        gtk_label_set_text(log, "Username or password is not valid");
        gtk_widget_set_visible(log, TRUE);
        log = gtk_builder_get_object(builder, "registerLog1");
        gtk_widget_set_visible(log, FALSE);
        log = gtk_builder_get_object(builder, "registerLog2");
        gtk_widget_set_visible(log, FALSE);
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

void entrychecking1(GtkEntry *entry, gpointer user_data) {
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

    g_signal_connect(usernameEntry, "delete-text", G_CALLBACK(entrychecking1), NULL);
    g_signal_connect(usernameEntry, "insert-text", G_CALLBACK(entrychecking1), NULL);
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