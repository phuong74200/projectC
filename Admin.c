#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item
{
	char name[1000];
	int id;
	unsigned int price;
	unsigned int discount;
	unsigned int currentQuantity;
	unsigned int soldQuantity;
	unsigned int profit;
} item[1000];

int index = 0;

void writeXMLFile(struct item item[], int index)
{
	FILE *xmlFilePrt;
	char xmlInput[1000] = "<name>";
	char addressFile[1000] = "database\\products\\";
	char idstring[10];
	itoa(item[index].id, idstring, 10);
	strcat(strcat(xmlInput, idstring), "</name>");
	xmlFilePrt = fopen(strcat(strcat(addressFile, idstring), ".xml"), "w");
	fprintf(xmlFilePrt, "%s", xmlInput);
	fclose(xmlFilePrt);
}

void inputName(struct item item[], int index)
{
	char idstring[10];
	char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	printf("Nhap ten mat hang: ");
	fflush(stdin);
	fgets(item[index].name, sizeof(item[index].name), stdin);
	itoa(item[index].id, idstring, 10);
	FILE *itemFilePtr;
	itemFilePtr = fopen(strcat(strcat(addressFile, idstring), ".txt"), "a");
	fprintf(itemFilePtr, "Ten san pham: %s", item[index].name);
	fclose(itemFilePtr);
}

void inputID(struct item item[], int index)
{
	char validation;
	int checker;
	char idstring[10];
	char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		do
		{
			printf("Nhap so ID cua mat hang: ");
			fflush(stdin);
			scanf("%u", &item[index].id);
			scanf("%c", &validation);
			if (validation != 10)
				printf("Nhap loi. Xin nhap lai.\n");
		}
		while (validation != 10);
		for (checker = 0; checker < index; checker++)
		{
			if (item[checker].id == item[index].id)
			{
				printf("Da co mat hang co ID %d \n", item[index].id);
				break;
			}
		}
	}
	while (checker != index);
	itoa(item[index].id, idstring, 10);
	FILE *itemFilePtr;
	itemFilePtr = fopen(strcat(strcat(addressFile, idstring), ".txt"), "w");
	fprintf(itemFilePtr, "ID: %d \n", item[index].id);
	fclose(itemFilePtr);
}

void inputPrice(struct item item[], int index)
{
	char validation;
	char idstring[10];
	char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap don gia: ");
		fflush(stdin);
		scanf("%u", &item[index].price);
		scanf("%c", &validation);
		if (validation != 10)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10);
	itoa(item[index].id, idstring, 10);
	FILE *itemFilePtr;
	itemFilePtr = fopen(strcat(strcat(addressFile, idstring), ".txt"), "a");
	fprintf(itemFilePtr, "Don gia: %d\n", item[index].price);
	fclose(itemFilePtr);
}

void inputDiscount(struct item item[], int index)
{
	char validation;
	char idstring[10];
	char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap muc giam gia (theo phan tram): ");
		fflush(stdin);
		scanf("%u", &item[index].discount);
		scanf("%c", &validation);
		if (validation != 10 || item[index}.discount < 0 || item[index}.discount >= 100)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10 || item[index}.discount < 0 || || item[index}.discount >= 100);
	item[index].price = item[index].price - item[index].price * item[index].discount / 100;
	itoa(item[index].id, idstring, 10);
	FILE *itemFilePtr;
	itemFilePtr = fopen(strcat(strcat(addressFile, idstring), ".txt"), "a");
	fprintf(itemFilePtr, "Muc giam gia: %d%%\n", item[index].discount);
	fclose(itemFilePtr);
}

void inputCurrentQuantity(struct item item[], int index)
{
	char validation;
	char idstring[10];
	char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap so luong: ");
		fflush(stdin);
		scanf("%u", &item[index].currentQuantity);
		scanf("%c", &validation);
		if (validation != 10)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10);
	itoa(item[index].id, idstring, 10);
	FILE *itemFilePtr;
	itemFilePtr = fopen(strcat(strcat(addressFile, idstring), ".txt"), "a");
	fprintf(itemFilePtr, "So luong: %d\n", item[index].currentQuantity);
	fclose(itemFilePtr);
}

void inputNewItem(struct item item[], int *indexPtr)
{
	char confirmation;
	do
	{
		inputID(item, *indexPtr);
		inputName(item, *indexPtr);
		inputPrice(item, *indexPtr);
		inputDiscount(item, *indexPtr);
		inputCurrentQuantity(item, *indexPtr);
		(*indexPtr)++;
		printf("Ban co muon tiep tuc nhap hang khong? (Y/N)");
		fflush(stdin);
		scanf("%c", &confirmation);
		while (confirmation != 'Y' && confirmation != 'y' && confirmation != 'N' && confirmation != 'n')
		{
			printf("Nhap loi. Xin nhap lai.\n");
			fflush(stdin);
			scanf("%c", &confirmation);
		}
	}
	while (confirmation == 'Y' || confirmation == 'y');
}

void deleteItem(struct item item[], int *indexPtr)
{
	int id, checker, arraydeleter;
	char validation, confirmation;
	if (index == 0)
	{
		printf("Khong co hang nao de xoa.\n");
	}
	else
	{
		do
		{
			printf("Hay chon ID cua mon hang can xoa: ");
			scanf("%d", &id);
			scanf("%c", &validation);
			while (validation != 10)
			{
				printf("Nhap loi. Xin nhap lai: ");
				fflush(stdin);
				scanf("%d", &id);
				scanf("%c", &validation);
				printf("\n");
			}
			for (checker = 0; checker <= index; checker++)
			{
				if (item[checker].id == id)
				{
					char idstring[10];
					char addressFile[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
					itoa(item[checker].id, idstring, 10);
					remove(strcat(strcat(addressFile, idstring), ".txt"));
					for (arraydeleter = checker; arraydeleter < index; arraydeleter++)
					{
						item[arraydeleter].id = item[arraydeleter + 1].id;
						strcpy(item[arraydeleter + 1].name, item[arraydeleter].name);
						item[arraydeleter].price = item[arraydeleter + 1].price;
						item[arraydeleter].discount = item[arraydeleter + 1].discount;
						item[arraydeleter].currentQuantity = item[arraydeleter + 1].currentQuantity;
						item[arraydeleter].soldQuantity = item[arraydeleter + 1].soldQuantity;
						item[arraydeleter].profit = item[arraydeleter + 1].profit;
					}
					break;
				}
			}
			if (checker != index + 1)
				(*indexPtr)--;
			else
				printf("Khong co mat hang nao co ID %d \n", id);
			printf("Ban co muon tiep tuc xoa hang khong? (Y/N)");
			fflush(stdin);
			scanf("%c", &confirmation);
			while (confirmation != 'Y' && confirmation != 'y' && confirmation != 'N' && confirmation != 'n')
			{
				printf("Nhap loi. Xin nhap lai: ");
				fflush(stdin);
				scanf("%c", &confirmation);
				printf("\n");
			}
			if (index == 0 && (confirmation == 'Y' || confirmation == 'y'))
			{
				printf("Khong co hang nao de xoa.\n");
			}
		}
		while ((confirmation == 'Y' || confirmation == 'y') && index > 0);
	}
}


int main()
{
	int key;
	printf("Xin chao, ban can gi?\n");
	do
	{
		printf("\nNhap 1 de them mon hang moi.\n");
		printf("Nhap 2 de xoa 1 mon hang.\n");
		printf("Nhap 8 de dang xuat.\n\n");
		scanf("%d", &key);
		switch (key)
		{
			case 1:
			{
				inputNewItem(item, &index);
				break;
			}
			case 2:
			{
				deleteItem(item, &index);
				break;
			}
			case 8:
			{
				break;
			}
			default:
			{
				printf("Nhap loi. Xin nhap lai!\n");
			}
		}
	}
	while (key != 8);
	return 0;
}
 
