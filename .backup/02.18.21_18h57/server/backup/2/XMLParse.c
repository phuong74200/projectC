#include <stdio.h>
#include <string.h>

int main() {
    char FirstTag[5] = "fuong";
    strcpy(FirstTag, "<");
    strcat(FirstTag, "UserName");
    strcat(FirstTag, ">");
    printf("%s", FirstTag);
}