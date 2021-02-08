#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct product
{
	char name[1000];
	int id;
	unsigned int price;
	unsigned int discount;
	unsigned int currentQuantity;
	unsigned int soldQuantity;
	unsigned int profit;
} product[1000];

int index = 0;

void writeXMLFile(struct product product[], int index)
{
	FILE *xmlFilePrt;
	char xmlInput[1000] = "<name>";
	char filePath[1000] = "database\\products\\";
	char idString[10];
	itoa(product[index].id, idString, 10);
	strcat(strcat(xmlInput, idString), "</name>");
	xmlFilePrt = fopen(strcat(strcat(filePath, idString), ".xml"), "w");
	fprintf(xmlFilePrt, "%s", xmlInput);
	fclose(xmlFilePrt);
}

void inputName(struct product product[], int index)
{
	char idString[10];
	char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	printf("Nhap ten mat hang: ");
	fflush(stdin);
	fgets(product[index].name, sizeof(product[index].name), stdin);
	itoa(product[index].id, idString, 10);
	FILE *productFilePtr;
	productFilePtr = fopen(strcat(strcat(filePath, idString), ".txt"), "a");
	fprintf(productFilePtr, "Ten san pham: %s", product[index].name);
	fclose(productFilePtr);
}

void inputID(struct product product[], int index)
{
	char validation;
	int checker;
	char idString[10];
	char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		do
		{
			printf("\nNhap so ID cua mat hang: ");
			fflush(stdin);
			scanf("%u", &product[index].id);
			scanf("%c", &validation);
			if (validation != 10 || product[index].id <= 0)
				printf("Nhap loi. Xin nhap lai.\n");
		}
		while (validation != 10 || product[index].id <= 0);
		for (checker = 0; checker < index; checker++)
		{
			if (product[checker].id == product[index].id)
			{
				printf("Da co mat hang co ID %d \n", product[index].id);
				break;
			}
		}
	}
	while (checker != index);
	itoa(product[index].id, idString, 10);
	FILE *productFilePtr;
	productFilePtr = fopen(strcat(strcat(filePath, idString), ".txt"), "w");
	fprintf(productFilePtr, "ID: %d \n", product[index].id);
	fclose(productFilePtr);
}

void inputPrice(struct product product[], int index)
{
	char validation;
	char idString[10];
	char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap don gia: ");
		fflush(stdin);
		scanf("%u", &product[index].price);
		scanf("%c", &validation);
		if (validation != 10 || product[index].price < 0)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10 || product[index].price < 0);
	itoa(product[index].id, idString, 10);
	FILE *productFilePtr;
	productFilePtr = fopen(strcat(strcat(filePath, idString), ".txt"), "a");
	fprintf(productFilePtr, "Don gia: %d\n", product[index].price);
	fclose(productFilePtr);
}

void inputDiscount(struct product product[], int index)
{
	char validation;
	char idString[10];
	char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap muc giam gia (theo phan tram): ");
		fflush(stdin);
		scanf("%u", &product[index].discount);
		scanf("%c", &validation);
		if (validation != 10 || product[index].discount < 0 || product[index].discount >= 100)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10 || product[index].discount < 0 || product[index].discount >= 100);
	product[index].price = product[index].price - product[index].price * product[index].discount / 100;
	itoa(product[index].id, idString, 10);
	FILE *productFilePtr;
	productFilePtr = fopen(strcat(strcat(filePath, idString), ".txt"), "a");
	fprintf(productFilePtr, "Muc giam gia: %d%%\n", product[index].discount);
	fclose(productFilePtr);
}

void inputCurrentQuantity(struct product product[], int index)
{
	char validation;
	char idString[10];
	char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
	do
	{
		printf("Nhap so luong: ");
		fflush(stdin);
		scanf("%u", &product[index].currentQuantity);
		scanf("%c", &validation);
		if (validation != 10 || product[index].currentQuantity <= 0)
			printf("Nhap loi. Xin nhap lai.\n");
	}
	while (validation != 10 || product[index].currentQuantity <= 0);
	itoa(product[index].id, idString, 10);
	FILE *productFilePtr;
	productFilePtr = fopen(strcat(strcat(filePath, idString), ".txt"), "a");
	fprintf(productFilePtr, "So luong: %d\n", product[index].currentQuantity);
	fclose(productFilePtr);
}

void inputNewProduct(struct product product[], int *indexPtr)
{
	char confirmation;
	do
	{
		inputID(product, *indexPtr);
		inputName(product, *indexPtr);
		inputPrice(product, *indexPtr);
		inputDiscount(product, *indexPtr);
		inputCurrentQuantity(product, *indexPtr);
		(*indexPtr)++;
		printf("Ban co muon tiep tuc nhap hang khong? (Y/N): ");
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

void deleteProduct(struct product product[], int *indexPtr)
{
	int id, checker, arrayDeleter;
	char validation, confirmation;
	if (index == 0)
	{
		printf("\nKhong co hang nao de xoa.\n");
	}
	else
	{
		do
		{
			printf("\nHay chon ID cua mon hang can xoa: ");
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
				if (product[checker].id == id)
				{
					char idString[10];
					char filePath[1000] = "E:\\Mine\\Study\\C\\Projects\\Tet\\ItemFiles\\";
					itoa(product[checker].id, idString, 10);
					remove(strcat(strcat(filePath, idString), ".txt"));
					for (arrayDeleter = checker; arrayDeleter < index; arrayDeleter++)
					{
						product[arrayDeleter].id = product[arrayDeleter + 1].id;
						strcpy(product[arrayDeleter + 1].name, product[arrayDeleter].name);
						product[arrayDeleter].price = product[arrayDeleter + 1].price;
						product[arrayDeleter].discount = product[arrayDeleter + 1].discount;
						product[arrayDeleter].currentQuantity = product[arrayDeleter + 1].currentQuantity;
						product[arrayDeleter].soldQuantity = product[arrayDeleter + 1].soldQuantity;
						product[arrayDeleter].profit = product[arrayDeleter + 1].profit;
					}
					break;
				}
			}
			if (checker != index + 1)
			{
				(*indexPtr)--;
				printf("Hang da xoa thanh cong.\n\n");
			}
			else
				printf("Khong co mat hang nao co ID %d \n\n", id);
			printf("Ban co muon tiep tuc xoa hang khong? (Y/N): ");
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

void searchProduct(struct product product[])
{
	char productSearch[1000];
	char confirmation;
	int checker;
	int countProduct;
	do
	{
		printf("\nNhap ten san pham can tim: ");
		fflush(stdin);
		scanf("%[^\n]", productSearch);
		countProduct = 0;
		for (checker = 0; checker <= index; checker++)
		{
			if (strstr(product[checker].name, productSearch) != NULL)
			{
				printf("\nID: %d\n", product[checker].id);
				printf("Ten san pham: %s", product[checker].name);
				printf("Don gia: %d\n", product[checker].price);
				printf("Muc giam gia: %d\n", product[checker].discount);
				printf("So luong: %d\n", product[checker].currentQuantity);
				countProduct++;
			}
		}
		if (countProduct == 0)
			printf("Khong co san pham \"%s\".\n\n", productSearch);
		printf("Ban co muon tiep tuc tim hang khong? (Y/N): ");
		fflush(stdin);
		scanf("%c", &confirmation);
		while (confirmation != 'Y' && confirmation != 'y' && confirmation != 'N' && confirmation != 'n')
		{
			printf("Nhap loi. Xin nhap lai: ");
			fflush(stdin);
			scanf("%c", &confirmation);
			printf("\n");
		}
	}
	while (confirmation == 'Y' || confirmation == 'y');
}

int main()
{
	int key;
	printf("Xin chao, ban can gi?\n");
	do
	{
		printf("\nNhap 1 de them mon hang moi.\n");
		printf("Nhap 2 de xoa 1 mon hang.\n");
		printf("Nhap 3 de tim 1 mon hang theo ten.\n");
		printf("Nhap 8 de dang xuat.\n\n");
		printf("Hanh dong ban chon: ");
		scanf("%d", &key);
		switch (key)
		{
			case 1:
			{
				inputNewProduct(product, &index);
				break;
			}
			case 2:
			{
				deleteProduct(product, &index);
				break;
			}
			case 3:
			{
				searchProduct(product);
				break;
			}
			case 8:
			{
				printf("Bye bye ~~");
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
