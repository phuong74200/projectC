#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct product {
    char name[1000];
    int id;
    unsigned int price;
    unsigned int discount;
    unsigned int quantity;
    unsigned int soldQuantity;
    unsigned int profit;
} product[1000];

int index = 0;

void inputName(struct product product[], int index) {
    char idString[10];
    char filePath[1000] = "database\\products\\";
    itoa(product[index].id, idString, 10);
    FILE *productFilePtr;
    productFilePtr = fopen(strcat(strcat(filePath, idString), ".xml"), "a");
    strcat(product[index].name, "</name>\n");
    fprintf(productFilePtr, "<name>");
    fprintf(productFilePtr, "%s", product[index].name);
    fclose(productFilePtr);
}

void inputID(struct product product[], int index) {
    char idString[10];
    char filePath[1000] = "database\\products\\";
    itoa(product[index].id, idString, 10);
    FILE *productFilePtr;
    productFilePtr = fopen(strcat(strcat(filePath, idString), ".xml"), "w");
    fprintf(productFilePtr, "<id>");
    fprintf(productFilePtr, "%d</id>\n", product[index].id);
    fclose(productFilePtr);
}

void inputPrice(struct product product[], int index) {
    char idString[10];
    char filePath[1000] = "database\\products\\";
    itoa(product[index].id, idString, 10);
    FILE *productFilePtr;
    productFilePtr = fopen(strcat(strcat(filePath, idString), ".xml"), "a");
    fprintf(productFilePtr, "<price>");
    fprintf(productFilePtr, "%d</price>\n", product[index].price);
    fclose(productFilePtr);
}

void inputDiscount(struct product product[], int index) {
    char idString[10];
    char filePath[1000] = "database\\products\\";
    product[index].price = product[index].price - product[index].price * product[index].discount / 100;
    itoa(product[index].id, idString, 10);
    FILE *productFilePtr;
    productFilePtr = fopen(strcat(strcat(filePath, idString), ".xml"), "a");
    fprintf(productFilePtr, "<discount>");
    fprintf(productFilePtr, "%d</discount>\n", product[index].discount);
    fclose(productFilePtr);
}

void inputCurrentQuantity(struct product product[], int index) {
    char idString[10];
    char filePath[1000] = "database\\products\\";
    itoa(product[index].id, idString, 10);
    FILE *productFilePtr;
    productFilePtr = fopen(strcat(strcat(filePath, idString), ".xml"), "a");
    fprintf(productFilePtr, "<quantity>");
    fprintf(productFilePtr, "%d</quantity>", product[index].quantity);
    fclose(productFilePtr);
}