#include <conio.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <winsock2.h>

#include "ulities.c"

#define PORT 8080

// server's local IPv4 address

#define ADDR "192.168.43.62"

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

    gchar XMLBuffer[200] = "";

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
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(mainWindow));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
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
int loadEnd = 1;
int deletedUser = 0;
int user_firstLoad = 1;

GtkStyleContext *context;
GtkWidget *slot;

int adm_getUserThread() {
    if (loadEnd == 1) {
        loadEnd = 0;
        userPosition++;
        initSocket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in address;

        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        address.sin_addr.s_addr = inet_addr(ADDR);

        if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
            return "connect_failed";
        }

        send(initSocket, glbChar, strlen(glbChar), 0);

        gchar XMLBuffer[200] = "";

        recv(initSocket, &XMLBuffer, 2000, 0);

        closesocket(initSocket);

        printf("%s", XMLBuffer);

        int top = userPosition + 1;

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
        } else if (user_firstLoad == 1) {
            listLength++;
        }

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
            printf("user pos %d", userPosition);
            printf("pos: %s\n", request);
            gdk_threads_add_timeout(10, adm_getUserThread, "");
            loadEnd = 1;
        }

        gtk_widget_show_all(adm_user);
    }
    return FALSE;
}

int getproducts_end = 1;
int productPosition = 0;
int productListLength = 15;

GObject *adm_product_grid;

char *decToHexa(int n) {
    static char hexaDeciNum[100] = "";

    for (int i = 0; i < sizeof(hexaDeciNum) / sizeof(hexaDeciNum[0]); i++) {
        hexaDeciNum[i] = NULL;
    }

    int i = 1;
    while (n != 0) {
        int temp = 0;

        temp = n % 16;

        strcpy(hexaDeciNum, "00");

        if (temp < 10 || i > 0) {
            hexaDeciNum[i] = temp + 48;
            i--;
        } else {
            hexaDeciNum[i] = temp + 55;
            i--;
        }

        n = n / 16;
    }

    return hexaDeciNum;
}

int updateProduct(GtkWidget *w, int top) {
    printf("%d\n", top);

    char clstr[1000];
    GObject *obj;

    strcpy(clstr, "<name>");
    obj = gtk_grid_get_child_at(adm_product_grid, 1, top);
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</name>");

    obj = gtk_grid_get_child_at(adm_product_grid, 2, top);
    strcat(clstr, "<price>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</price>");

    obj = gtk_grid_get_child_at(adm_product_grid, 3, top);
    strcat(clstr, "<sale>");
    strcat(clstr, gtk_entry_get_text(obj));
    strcat(clstr, "</sale>");

    obj = gtk_grid_get_child_at(adm_product_grid, 0, top);
    strcat(clstr, "<index>");
    strcat(clstr, gtk_label_get_text(obj));
    strcat(clstr, "</index>");

    strcat(clstr, "<api>updateProduct</api>");

    fetch(clstr);

    printf("%s\n", clstr);

    return FALSE;
}

void delProduct(GtkWidget *w, int top) {
    char clstr[1000];
    GObject *obj;
    for (int i = 0; i <= 8; i++) {
        obj = gtk_grid_get_child_at(adm_product_grid, i, top);
        gtk_widget_hide(obj);
    }
    obj = gtk_grid_get_child_at(adm_product_grid, 0, top);
    strcat(clstr, "<index>");
    strcat(clstr, gtk_label_get_text(obj));
    strcat(clstr, "</index>");
    strcat(clstr, "<api>delProduct</api>");

    fetch(clstr);
}

int adm_getProductList() {
    if (getproducts_end == 1) {
        getproducts_end = 0;

        char request[300] = "<api>getProduct</api><index>";

        char sindex[30];
        itoa(productPosition, sindex, 10);

        strcat(request, sindex);
        strcat(request, "</index>");

        char *XMLBuffer = fetch(request);

        printf("\n ------------------ %s\n", XMLBuffer);

        if (strcmp(XMLBuffer, "none") != 0 && strlen(xmlp("name", XMLBuffer)) > 0) {
            adm_product_grid = gtk_builder_get_object(admin_users_manage_window, "adm_product_grid");
            int top = productPosition + 1;

            slot = gtk_label_new(xmlp("index", XMLBuffer));
            gtk_grid_attach(adm_product_grid, slot, 0, top, 1, 1);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-index");
            gtk_widget_set_hexpand(slot, TRUE);
            gtk_widget_set_vexpand(slot, TRUE);

            slot = gtk_entry_new();
            gtk_entry_set_text(slot, xmlp("name", XMLBuffer));
            gtk_grid_attach(adm_product_grid, slot, 1, top, 1, 1);
            gtk_entry_set_alignment(slot, 0.5);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-entry");

            slot = gtk_entry_new();
            gtk_entry_set_text(slot, xmlp("price", XMLBuffer));
            gtk_grid_attach(adm_product_grid, slot, 2, top, 1, 1);
            gtk_entry_set_alignment(slot, 0.5);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-entry");

            slot = gtk_entry_new();
            gtk_entry_set_text(slot, xmlp("sale", XMLBuffer));
            gtk_grid_attach(adm_product_grid, slot, 3, top, 1, 1);
            gtk_entry_set_alignment(slot, 0.5);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-entry");

            slot = gtk_button_new_with_label("delete");
            gtk_grid_attach(adm_product_grid, slot, 5, top, 1, 1);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-del");
            g_signal_connect(slot, "clicked", G_CALLBACK(delProduct), top);

            slot = gtk_button_new_with_label("save");
            gtk_grid_attach(adm_product_grid, slot, 4, top, 1, 1);
            context = gtk_widget_get_style_context(slot);
            gtk_style_context_add_class(context, "adm_user_slot-save");
            g_signal_connect(slot, "clicked", G_CALLBACK(updateProduct), top);

            GdkRGBA *color2;
            gdk_rgba_parse(&color2, xmlp("color1", XMLBuffer));
            GtkWidget *colorBtn1;
            colorBtn1 = gtk_color_button_new_with_rgba(&color2);
            gtk_grid_attach(adm_product_grid, colorBtn1, 6, top, 1, 1);
            context = gtk_widget_get_style_context(colorBtn1);
            gtk_style_context_add_class(context, "adm_user_slot-col");
            gtk_widget_set_hexpand(colorBtn1, TRUE);
            gtk_widget_set_vexpand(colorBtn1, TRUE);

            gdk_rgba_parse(&color2, xmlp("color2", XMLBuffer));
            GtkWidget *colorBtn2;
            colorBtn2 = gtk_color_button_new_with_rgba(&color2);
            gtk_grid_attach(adm_product_grid, colorBtn2, 7, top, 1, 1);
            context = gtk_widget_get_style_context(colorBtn2);
            gtk_style_context_add_class(context, "adm_user_slot-col");
            gtk_widget_set_hexpand(colorBtn2, TRUE);
            gtk_widget_set_vexpand(colorBtn2, TRUE);

            gdk_rgba_parse(&color2, xmlp("color3", XMLBuffer));
            GtkWidget *colorBtn3;
            colorBtn3 = gtk_color_button_new_with_rgba(&color2);
            gtk_grid_attach(adm_product_grid, colorBtn3, 8, top, 1, 1);
            context = gtk_widget_get_style_context(colorBtn3);
            gtk_style_context_add_class(context, "adm_user_slot-col");
            gtk_widget_set_hexpand(colorBtn3, TRUE);
            gtk_widget_set_vexpand(colorBtn3, TRUE);
        }

        if (productPosition < productListLength) {
            productPosition++;
            gdk_threads_add_timeout(100, adm_getProductList, "");
            getproducts_end = 1;
        }
    }
    gtk_widget_show_all(adm_product_grid);
    return FALSE;
}

int scollable = 1;

void enableScroll() {
    scollable = 1;
    return FALSE;
}

void admUserEdgeReach(GtkScrolledWindow *scrolled_window, GtkPositionType pos, gpointer user_data) {
    if (scollable == 1 && pos == GTK_POS_BOTTOM) {
        user_firstLoad = 0;
        loadEnd = 1;
        scollable = 0;
        listLength += 15;
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

    strcat(clstr, "<color1>#");
    obj = gtk_builder_get_object(admin_users_manage_window, "color1");
    gtk_color_chooser_get_rgba(obj, color1);
    strcat(clstr, decToHexa((int)(color1->red * 255)));
    strcat(clstr, decToHexa((int)(color1->green * 255)));
    strcat(clstr, decToHexa((int)(color1->blue * 255)));
    strcat(clstr, "</color1>");

    strcat(clstr, "<color2>#");
    obj = gtk_builder_get_object(admin_users_manage_window, "color2");
    gtk_color_chooser_get_rgba(obj, color1);
    strcat(clstr, decToHexa((int)(color1->red * 255)));
    strcat(clstr, decToHexa((int)(color1->green * 255)));
    strcat(clstr, decToHexa((int)(color1->blue * 255)));
    strcat(clstr, "</color2>");

    strcat(clstr, "<color3>#");
    obj = gtk_builder_get_object(admin_users_manage_window, "color3");
    gtk_color_chooser_get_rgba(obj, color1);
    strcat(clstr, decToHexa((int)(color1->red * 255)));
    strcat(clstr, decToHexa((int)(color1->green * 255)));
    strcat(clstr, decToHexa((int)(color1->blue * 255)));
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
    char *text = "";
    text = gtk_entry_get_text(obj);
    strcat(clstr, text);
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
            int isSp = g_ascii_isspace(content[i]);
            if (isDg == 0 && isAl == 0 && isSp == 0) {
                isValid++;
                obj = gtk_builder_get_object(admin_users_manage_window, "tagsDes");
                gtk_label_set_text(obj, "Tags (using for user to search) (not include special characters)");
                break;
            } else {
                obj = gtk_builder_get_object(admin_users_manage_window, "tagsDes");
                gtk_label_set_text(obj, "Tags (using for user to search)");
            }
        }

        obj = gtk_builder_get_object(admin_users_manage_window, "productDescript");
        content = gtk_entry_get_text(obj);

        for (int i = 0; i < strlen(content); i++) {
            int isDg = g_ascii_isdigit(content[i]);
            int isAl = g_ascii_isalpha(content[i]);
            int isSp = g_ascii_isspace(content[i]);
            if (isDg == 0 && isAl == 0 && isSp == 0) {
                isValid++;
                obj = gtk_builder_get_object(admin_users_manage_window, "desDes");
                gtk_label_set_text(obj, "Description for your item (not include special characters)");
                break;
            } else {
                obj = gtk_builder_get_object(admin_users_manage_window, "desDes");
                gtk_label_set_text(obj, "Description for your item");
            }
        }

        printf("isvalid: %d", isValid);

        if (isValid == 0) {
            gdk_threads_add_timeout(100, addItemThread, "");
        }
    }
}

void admin_users_manage() {
    loadEnd = 1;
    userPosition = 0;
    productPosition = 0;
    getproducts_end = 1;
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
    if (productPosition <= 1) {
        gdk_threads_add_timeout(500, adm_getProductList, "");
    }
}

GtkBuilder *user_cart;

int cartClickable = 1;

void resetCartClick() {
    cartClickable = 1;
    return FALSE;
}

GtkWidget *consumePrice;

deleteCartItem(GtkWidget *w, gpointer name1) {
    int consume = 0;
    char filePath[100] = "appcache\\cart\\";
    strcat(filePath, name1);
    printf("-- filepath: %s\n", filePath);
    remove(filePath);
    GtkWidget *wparent = gtk_widget_get_parent(w);

    GObject *price = gtk_grid_get_child_at(wparent, 1, 0);
    GObject *quantity = gtk_grid_get_child_at(wparent, 2, 0);

    consume = atoi(gtk_label_get_text(consumePrice)) - atoi(gtk_label_get_text(price)) * atoi(gtk_label_get_text(quantity));

    char consumeStr[100] = "";
    itoa(consume, consumeStr, 10);

    gtk_label_set_text(consumePrice, consumeStr);

    gtk_widget_destroy(wparent);
}

int addProductsThread();

void search() {
    button = gtk_builder_get_object(builder, "searchEntry");
    char value[1000];

    strcpy(value, gtk_entry_get_text(button));

    int isValid = 0;

    if (strlen(value) > 0) {
        for (int i = 0; i < strlen(value); i++) {
            int isDg = g_ascii_isdigit(value[i]);
            int isAl = g_ascii_isalpha(value[i]);
            int isSp = g_ascii_isspace(value[i]);
            if (isDg == 0 && isAl == 0 && isSp == 0) {
                isValid = 0;
                break;
            } else {
                isValid = 1;
            }
        }
    }

    if (isValid == 1) {
        char request[1000] = "<api>search</api><keys>";
        strcat(request, value);
        strcat(request, "</keys>");

        initSocket = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        address.sin_addr.s_addr = inet_addr(ADDR);
        if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
            return "connect_failed";
        }
        send(initSocket, request, strlen(request), 0);
        gchar XMLBuffer[200] = "";
        recv(initSocket, &XMLBuffer, 200, 0);
        closesocket(initSocket);

        printf("%s\n", XMLBuffer);

        char *token;
        char *rest = XMLBuffer;

        GList *children, *iter;
        children = gtk_container_get_children(GTK_CONTAINER(gtk_builder_get_object(builder, "productsShowCase")));
        for (iter = children; iter != NULL; iter = g_list_next(iter))
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        g_list_free(children);

        while ((token = strtok_r(rest, ";", &rest))) {
            gdk_threads_add_idle(addProductsThread, GINT_TO_POINTER(atoi(token)));
            printf("%s\n", token);
        }
    }
}

void confirmOrder(GtkWidget *w, GtkWidget *user_cart_grid) {
    DIR *d;
    struct dirent *dir;
    d = opendir("appcache\\cart");
    int count = -2;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            count++;
            if (count > 0) {
                char filePath[1000] = "appcache\\cart\\";
                char request[1000] = "<api>submitOrder</api><index>";
                strcat(request, dir->d_name);
                strcat(request, "</index><quantity>");

                char itemQuantity[10] = "0";
                strcat(filePath, dir->d_name);
                readFile(filePath, &itemQuantity);

                strcat(request, itemQuantity);
                strcat(request, "</quantity><token>");
                strcat(request, accountToken);
                strcat(request, "</token>");

                if (strcmp(itemQuantity, "0") != 0) {
                    initSocket = socket(AF_INET, SOCK_STREAM, 0);
                    struct sockaddr_in address;
                    address.sin_family = AF_INET;
                    address.sin_port = htons(PORT);
                    address.sin_addr.s_addr = inet_addr(ADDR);
                    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
                        return "connect_failed";
                    }
                    send(initSocket, request, strlen(request), 0);
                    gchar XMLBuffer[200] = "";
                    recv(initSocket, &XMLBuffer, 200, 0);
                    closesocket(initSocket);
                }
            }
        }
        closedir(d);
    }
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(user_cart_grid));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
}

void user_cart_event() {
    user_cart = gtk_builder_new();
    user_cart = gtk_builder_new_from_file("UI\\cart.xml");
    GObject *user_cart_window = gtk_builder_get_object(user_cart, "user_cartWindow");
    GtkWidget *user_cart_grid = gtk_builder_get_object(user_cart, "user_cart");

    GtkWidget *cart_confirm_btn = gtk_builder_get_object(user_cart, "cart_confirm_btn");
    g_signal_connect(cart_confirm_btn, "clicked", G_CALLBACK(confirmOrder), user_cart_grid);

    gtk_window_set_modal(user_cart_window, TRUE);
    gtk_widget_show_all(user_cart_window);

    int consume = 0;

    DIR *d;
    struct dirent *dir;
    d = opendir("appcache\\cart");
    int count = -2;
    int index = 0;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            count++;
            if (count > 0) {
                char filePath[1000] = "appcache\\cart\\";
                char request[1000] = "<api>getProduct</api><index>";
                strcat(request, dir->d_name);
                strcat(request, "</index>");

                char *response = fetch(request);

                printf("%s\n", response);

                char itemQuantity[10] = "0";
                strcat(filePath, dir->d_name);
                readFile(filePath, &itemQuantity);

                if (strcmp(itemQuantity, "0") != 0) {
                    GtkWidget *itemGrid = gtk_grid_new();
                    GObject *label;

                    label = gtk_label_new(xmlp("name", response));
                    gtk_grid_attach(itemGrid, label, 0, 0, 1, 1);

                    label = gtk_label_new(xmlp("price", response));
                    gtk_grid_attach(itemGrid, label, 1, 0, 1, 1);

                    label = gtk_label_new(itemQuantity);
                    gtk_grid_attach(itemGrid, label, 2, 0, 1, 1);

                    consume += atoi(xmlp("price", response)) * atoi(itemQuantity);

                    GtkStyleContext *context;
                    label = gtk_button_new_with_label("Delete");
                    gtk_grid_attach(itemGrid, label, 3, 0, 1, 1);

                    gtk_grid_attach(user_cart_grid, itemGrid, 0, index, 1, 1);
                    context = gtk_widget_get_style_context(label);
                    gtk_style_context_add_class(context, "cart_del_btn");
                    g_signal_connect(label, "clicked", G_CALLBACK(deleteCartItem), xmlp("index", response));

                    gtk_grid_set_row_homogeneous(itemGrid, TRUE);
                    gtk_grid_set_column_homogeneous(itemGrid, TRUE);
                    gtk_widget_set_hexpand(itemGrid, TRUE);

                    index++;
                }
            }
        }
        closedir(d);
    }

    char consumeStr[100] = "";
    itoa(consume, consumeStr, 10);
    consumePrice = gtk_builder_get_object(user_cart, "cart_consume");
    gtk_label_set_text(consumePrice, consumeStr);
    gtk_widget_show_all(user_cart_grid);
    gtk_window_set_position(GTK_WINDOW(user_cart_window), GTK_WIN_POS_CENTER_ALWAYS);
}

void addItemToCartThread(gpointer top) {
    cartClickable = 0;

    int index = GPOINTER_TO_INT(top);

    char filePath[1000] = "appcache\\cart\\";

    char sindex[10] = "";

    itoa(index, sindex, 10);

    strcat(filePath, sindex);

    printf("%s\n", filePath);

    if (checkFile(filePath) == 0) {
        writeFile(filePath, "1");
    } else {
        char quantity[100] = "";
        readFile(filePath, &quantity);
        char value = atoi(quantity);
        value++;
        itoa(value, sindex, 10);
        writeFile(filePath, sindex);
    }

    return FALSE;
}

void addItemToCartEvent(GtkWidget *w, gpointer top) {
    int index = GPOINTER_TO_INT(top);
    printf("--- add to cart  1! %d \n", index);
    if (cartClickable == 1) {
        gdk_threads_add_timeout(50, addItemToCartThread, GINT_TO_POINTER(index));
        gdk_threads_add_timeout(500, resetCartClick, GINT_TO_POINTER(index));
    }
}

void colorPicker(GtkWidget *item, gpointer color) {
    printf("%s\n", color);
}

int addProductsThread(gpointer top) {
    int index = GPOINTER_TO_INT(top);

    char request[300] = "<api>getProduct</api><index>";

    char sindex[30];
    itoa(index, sindex, 10);

    strcat(request, sindex);
    strcat(request, "</index>");

    initSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }

    send(initSocket, request, strlen(request), 0);

    gchar response[200] = "";

    recv(initSocket, &response, 200, 0);

    closesocket(initSocket);

    printf("%s\n", response);

    if (strcmp(response, "none") != 0 && strlen(xmlp("name", response)) > 0) {
        GObject *productContainer;
        GObject *productForm;
        GObject *obj;
        GtkBuilder *product;

        GObject *grid = gtk_builder_get_object(builder, "productsShowCase");

        product = gtk_builder_new_from_file("UI\\product.xml");

        productForm = gtk_builder_get_object(product, "productForm");

        gtk_grid_attach(grid, productForm, 0, index, 1, 1);

        obj = gtk_builder_get_object(product, "product_name");
        gtk_label_set_text(obj, xmlp("name", response));

        obj = gtk_builder_get_object(product, "product_des");
        gtk_label_set_text(obj, xmlp("des", response));

        GObject *cartBtn = gtk_builder_get_object(product, "addToCartBtn");
        g_signal_connect(cartBtn, "clicked", G_CALLBACK(addItemToCartEvent), GINT_TO_POINTER(index));

        char price[10];
        strcpy(price, "$");
        strcat(price, xmlp("price", response));
        obj = gtk_builder_get_object(product, "product_price");

        gtk_label_set_text(obj, price);

        GdkColor color;

        obj = gtk_builder_get_object(product, "product_form_color1");
        gdk_color_parse(xmlp("color1", response), &color);
        gtk_widget_modify_bg(GTK_WIDGET(obj), GTK_STATE_NORMAL, &color);
        g_signal_connect(obj, "clicked", G_CALLBACK(colorPicker), xmlp("color1", response));

        obj = gtk_builder_get_object(product, "product_form_color2");
        gdk_color_parse(xmlp("color2", response), &color);
        gtk_widget_modify_bg(GTK_WIDGET(obj), GTK_STATE_NORMAL, &color);
        g_signal_connect(obj, "clicked", G_CALLBACK(colorPicker), xmlp("color2", response));

        obj = gtk_builder_get_object(product, "product_form_color3");
        gdk_color_parse(xmlp("color3", response), &color);
        gtk_widget_modify_bg(GTK_WIDGET(obj), GTK_STATE_NORMAL, &color);
        g_signal_connect(obj, "clicked", G_CALLBACK(colorPicker), xmlp("color3", response));

        gtk_widget_show_all(grid);

        printf("%s\n", response);
        printf("%s\n", request);
    }

    return FALSE;
}

GtkBuilder *adm_order_builder;
GObject *adm_order_window;
GObject *adm_order_grid;

int currentOrderPos = 0;
int maxOrderPos = 15;

void resolveOrder(GtkWidget *self, char orderToken[1000]) {
    GtkWidget *parent = gtk_widget_get_parent(self);
    gtk_widget_hide(parent);

    char request[1000] = "<api>resolveOrder</api><token>";
    strcat(request, accountToken);
    strcat(request, "</token><orderToken>");
    strcat(request, orderToken);
    strcat(request, "</orderToken>");

    fetch(request);
}

int ad_order_loadThread(gpointer pos) {
    int position = GPOINTER_TO_INT(pos);

    char posStr[100] = "";
    itoa(position, posStr, 10);

    char request[1000] = "<api>adm_getOrder</api><token>";
    strcat(request, accountToken);
    strcat(request, "</token><pos>");
    strcat(request, posStr);
    strcat(request, "</pos>");

    initSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDR);
    if (connect(initSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return "connect_failed";
    }
    send(initSocket, request, strlen(request), 0);
    gchar XMLBuffer[2000] = "";
    recv(initSocket, &XMLBuffer, 200, 0);
    closesocket(initSocket);

    printf("%s\n", XMLBuffer);

    if (strlen(xmlp("username", XMLBuffer)) > 0) {
        char *token;
        char *rest = xmlp("items", XMLBuffer);
        GtkStyleContext *context;

        GtkWidget *container = gtk_grid_new();
        gtk_grid_set_row_homogeneous(container, TRUE);
        gtk_grid_set_column_homogeneous(container, TRUE);

        GtkWidget *dataGrid = gtk_grid_new();
        gtk_grid_attach(container, dataGrid, 0, 1, 1, 1);
        gtk_grid_set_column_homogeneous(dataGrid, TRUE);
        gtk_widget_set_hexpand(dataGrid, TRUE);

        int top = 0;
        int sum = 0;

        while ((token = strtok_r(rest, ";", &rest))) {
            int left = 1;
            char *childTok;

            int quant = 0;
            int cons = 0;

            while ((childTok = strtok_r(token, ",", &token))) {
                GtkWidget *label = gtk_label_new(childTok);
                gtk_label_set_xalign(label, 0);
                gtk_grid_attach(dataGrid, label, left, top, 1, 1);
                context = gtk_widget_get_style_context(label);
                if (top == 0) {
                    gtk_style_context_add_class(context, "order_label-header");
                } else {
                    gtk_style_context_add_class(context, "order_label");
                }
                if (left == 2) {
                    quant = atoi(childTok);
                }
                if (left == 3) {
                    cons = atoi(childTok);
                }
                left++;
            }
            sum += quant * cons;
            top++;
        }

        char money[100] = "$";
        char sumStr[100] = "";
        itoa(sum, sumStr, 10);
        strcat(money, sumStr);

        GtkWidget *usernameLabel = gtk_label_new(xmlp("username", XMLBuffer));
        context = gtk_widget_get_style_context(usernameLabel);
        gtk_style_context_add_class(context, "order_label-header");
        gtk_grid_attach(dataGrid, usernameLabel, 0, 0, 1, top - 3);

        GtkWidget *consumeLabel = gtk_label_new(money);
        context = gtk_widget_get_style_context(consumeLabel);
        gtk_style_context_add_class(context, "order_label-sum");
        gtk_grid_attach(dataGrid, consumeLabel, 0, top - 3, 1, 2);

        GtkWidget *resolveBtn = gtk_button_new_with_label("resolve now");
        context = gtk_widget_get_style_context(resolveBtn);
        gtk_style_context_add_class(context, "order_label-resolve");
        gtk_grid_attach(dataGrid, resolveBtn, 0, top - 1, 1, 1);
        g_signal_connect(resolveBtn, "clicked", G_CALLBACK(resolveOrder), xmlp("token", XMLBuffer));

        gtk_grid_attach(adm_order_grid, container, 0, position, 1, 1);
        gtk_widget_show_all(adm_order_grid);
    }

    if (currentOrderPos < maxOrderPos) {
        currentOrderPos++;
        printf("%d\n", currentOrderPos);
        gdk_threads_add_timeout(100, ad_order_loadThread, GINT_TO_POINTER(currentOrderPos));
    }

    return FALSE;
}

void ad_order_display() {
    currentOrderPos = 0;
    adm_order_builder = gtk_builder_new();
    adm_order_builder = gtk_builder_new_from_file("UI\\orders.xml");
    adm_order_window = gtk_builder_get_object(adm_order_builder, "mainWindow");
    adm_order_grid = gtk_builder_get_object(adm_order_builder, "mainGrid");
    gtk_window_set_modal(adm_order_window, TRUE);
    gtk_widget_show_all(adm_order_window);
    gdk_threads_add_timeout(100, ad_order_loadThread, GINT_TO_POINTER(currentOrderPos));
}

void user_cart_event();

int currentProduct = 0;
int productLength = 3;
int landingScrollEnable = 1;

void enableLandingScroll() {
    landingScrollEnable = 1;
}

void landingScroll() {
    gdk_threads_add_timeout(500, enableLandingScroll, "");

    if (landingScrollEnable == 1) {
        landingScrollEnable = 0;
        productLength += 3;
        for (currentProduct = currentProduct; currentProduct < productLength; currentProduct++) {
            gdk_threads_add_idle(addProductsThread, GINT_TO_POINTER(currentProduct));
        }
    }
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
                g_signal_connect(button, "clicked", G_CALLBACK(ad_order_display), NULL);
                gtk_widget_show(button);
            }
            printf("token accepted\n");
        }
    }

    for (currentProduct = 0; currentProduct < productLength; currentProduct++) {
        gdk_threads_add_idle(addProductsThread, GINT_TO_POINTER(currentProduct));
    }

    button = gtk_builder_get_object(builder, "cartBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(user_cart_event), NULL);

    button = gtk_builder_get_object(builder, "homeBtn");
    g_signal_connect(button, "clicked", G_CALLBACK(landingScreenDisplay), NULL);

    button = gtk_builder_get_object(builder, "landingScroll");
    g_signal_connect(button, "edge-reached", G_CALLBACK(landingScroll), NULL);

    GObject *searchBtn = gtk_builder_get_object(builder, "searchBtn");
    g_signal_connect(searchBtn, "clicked", G_CALLBACK(search), NULL);
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