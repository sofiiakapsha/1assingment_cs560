#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    char* data;
    int capacity;
    struct Node* next;
}Node;

Node* currentNode;
Node* head;

void Append() {
    printf("Enter text to append:\n");

    if (currentNode->data == NULL) {
        currentNode->data = (char*)malloc(currentNode->capacity * sizeof(char));
        if (currentNode->data == NULL) {
            printf("Memory allocation error.\n");
            return;
        }
        currentNode->data[0] = '\0';
    }

    size_t len = strlen(currentNode->data);

    if (currentNode->capacity - len <= 1) {
        currentNode->capacity += 20;
        char* result = realloc(currentNode->data, currentNode->capacity);
        if (result == NULL) {
            printf("Memory reallocation error.\n");
            return;
        }
        currentNode->data = result;
    }

    if (fgets(currentNode->data + len, currentNode->capacity - len, stdin) == NULL) {
        return;
    }

    len = strlen(currentNode->data);
    while (len > 0 && currentNode->data[len - 1] != '\n') {
        currentNode->capacity += 20;

        char* result = realloc(currentNode->data, currentNode->capacity);
        if (result == NULL) {
            printf("Memory reallocation error.\n");
            return;
        }
        currentNode->data = result;

        if (fgets(currentNode->data + len, currentNode->capacity - len, stdin) == NULL) {
            break;
        }
        len = strlen(currentNode->data);
    }

    if (len > 0 && currentNode->data[len - 1] == '\n') {
        currentNode->data[len - 1] = '\0';
    }
}

void newLine(int cap) {

    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->next = NULL;
    newNode->capacity = cap;
    newNode->data = NULL;

    currentNode->next = newNode;

    size_t lengths = 0;

    if (currentNode->data == NULL) {

        currentNode->data = (char*)malloc(currentNode->capacity * sizeof(char));

        if (currentNode->data == NULL) {
            printf("Memory allocation error.\n");
            free(newNode);
            currentNode->next = NULL;
            return;
        }

        currentNode->data[lengths] = '\n';
        currentNode->data[lengths + 1] = '\0';
        currentNode = newNode;
        printf("New line is started\n");
    }
    else {

        lengths = strlen(currentNode->data);

        if (lengths > 0 && currentNode->data[lengths - 1] == '\n') {
            currentNode = newNode;
            printf("New line is started\n");
        }
        else {

            if (lengths + 2 > currentNode->capacity) {
                currentNode->capacity += 20;
                char* result = realloc(currentNode->data, currentNode->capacity);

                if (result == NULL) {
                    printf("Memory allocation error.\n");
                    currentNode->next = NULL;
                    free(newNode);
                    return;
                }
                currentNode->data = result;
            }

            currentNode->data[lengths] = '\n';
            currentNode->data[lengths + 1] = '\0';
            currentNode = newNode;
            printf("New line is started\n");
        }
    }
}


void Save() {

    FILE* file;

    char fileName[50];

    printf("Enter file name:\n");
    scanf("%s", fileName);

    while (getchar() != '\n');

    file = fopen(fileName, "w");

    if (file == NULL) {
        printf("Error opening file");
    }

    else {
        Node* savingNode = head;

        while (savingNode != NULL) {
            if (savingNode->data != NULL) {
                fprintf(file, "%s", savingNode->data);
            }
            savingNode = savingNode->next;
        }

        fclose(file);
        printf("Text has been saved successfully.\n");
    }
}

void Load() {
    FILE* file;

    int capacityLoader = 20;

    char fileName[50];

    printf("Enter file name:\n");
    scanf("%s", fileName);

    while (getchar() != '\n');

    file = fopen(fileName, "r");

    if (file == NULL) {
        printf("Error opening file");
    }

    else {

        if (head->data != NULL) {

            printf("All unsaved information will be deleted.");
            Node* deletingNode = head;

            while (deletingNode != NULL) {

                currentNode = deletingNode->next;
                free(deletingNode->data);
                free(deletingNode);
                deletingNode = currentNode;
            }

            currentNode = (Node*)malloc(sizeof(Node));
            currentNode->next = NULL;
            currentNode->data = NULL;
            currentNode->capacity = 20;
            head = currentNode;
        }


        currentNode = head;

        char buffer[50];

        while (fgets(buffer, 50, file) != NULL) {

            if (currentNode->data == NULL) {
                currentNode->data = (char*)malloc(currentNode->capacity * sizeof(char));
                currentNode->data[0] = '\0';
            }

            if (currentNode->data != NULL && strlen(buffer) + strlen(currentNode->data) + 1 > currentNode->capacity) {

                currentNode->capacity += 50;
                currentNode->data = realloc(currentNode->data, currentNode->capacity);

            }

            strcat(currentNode->data, buffer);

            if (currentNode->data[strlen(currentNode->data) - 1] == '\n') {
                newLine(currentNode->capacity);
            }
        }

        fclose(file);
        printf("Text has been loaded successfully.\n");
    }
}

void Print() {
    if (head->data == NULL) {
        printf("No current text.\n");
    }
    else {
        Node* printingNode = head;
        while (printingNode != NULL) {
            if (printingNode->data != NULL) {
                printf("%s", printingNode->data);
            }
            printingNode = printingNode->next;
        }
    }
}

void Insert() {

    int line, index;
    char* text;
    int capacity = 20;
    Node* countingNode = head;
    int counter = 0;

    text = (char*)malloc(capacity * sizeof(char));

    printf("Choose line and index:\n");
    scanf("%d %d", &line, &index);

    while (getchar() != '\n');

    printf("Enter text to insert:\n");
    fgets(text, capacity, stdin);

    while (text[strlen(text) - 1] != '\n') {
        capacity += 20;
        text = realloc(text, capacity);
        size_t lengths = strlen(text);
        fgets(text + lengths, capacity - lengths, stdin);
    }

    text[strlen(text) - 1] = '\0';

    while (countingNode != NULL && counter != line) {
        countingNode = countingNode->next;
        counter++;
    }

    if (countingNode == NULL) {
        printf("Error: this line does not exist.\n");
        free(text);
        return;
    }

    size_t len = strlen(text);

    if (countingNode->data != NULL) {

        size_t dataLen = strlen(countingNode->data);

        if (index < 0 || dataLen < index) {
            printf("This index does not exist.\n");
            free(text);
            return;
        }

        if (dataLen + len + 1 > countingNode->capacity) {
            countingNode->capacity += len + 1;
            countingNode->data = realloc(countingNode->data, countingNode->capacity);
        }

        for (int i = dataLen; i >= index; i--) {
            countingNode->data[i + len] = countingNode->data[i];
        }

        for (int i = index; i < len + index; i++) {
            countingNode->data[i] = text[i - index];
        }
    }
    else {

        if (index != 0) {
            printf("This index does not exist. Line is empty.\n");
            free(text);
            return;
        }

        countingNode->capacity = len + 1;

        countingNode->data = (char*)malloc(countingNode->capacity * sizeof(char));

        strncpy(countingNode->data, text, len);

        countingNode->data[len] = '\0';
    }

    free(text);
}

void Search() {

    char target[50];
    char* check;
    Node* searchingNode = head;
    int line = 0;
    bool isFound = false;

    printf("Enter the text to search:\n");
    check = fgets(target, 50, stdin);

    if (check == NULL) {
        printf("Error: searching text is too long.\n");
        return;
    }
    else {
        size_t len = strlen(target);
        if (len > 0 && target[len - 1] == '\n') {
            target[len - 1] = '\0';
        }
    }

    while (searchingNode != NULL) {
        if (searchingNode->data != NULL) {
            char* finder = strstr(searchingNode->data, target);
            if (finder != NULL) {
                printf("Text in position: %d %d\n", (int)line, (int)(finder - searchingNode->data));
                isFound = true;
            }
        }
        searchingNode = searchingNode->next;
        line++;
    }
    if (isFound == false) {
        printf("Not found.\n");
    }
}


int main() {
    currentNode = (Node*)malloc(sizeof(Node));
    currentNode->next = NULL;
    currentNode->data = NULL;
    currentNode->capacity = 20;
    head = currentNode;

    int answer;

    while (1) {
        printf("\nChoose the command:\n");
        printf("1 - Append, 2 - newLine, 3 - Save, 4 - Load\n");
        printf("5 - Print,  6 - Insert,  7 - Search, 8 - Exit\n");

        scanf("%d", &answer);

        while (getchar() != '\n');

        switch (answer) {
        case 1:
            Append();
            break;
        case 2:
            newLine(20);
            break;
        case 3:
            Save();
            break;
        case 4:
            Load();
            break;
        case 5:
            Print();
            break;
        case 6:
            Insert();
            break;
        case 7:
            Search();
            break;
        case 8:

        {
            Node* deletingNode = head;

            while (deletingNode != NULL) {

                currentNode = deletingNode->next;
                free(deletingNode->data);
                free(deletingNode);
                deletingNode = currentNode;
            }
            return 0;
        }

        default:
            printf("Invalid command! Please try again.\n");
            break;
        }
    }

    return 0;
}