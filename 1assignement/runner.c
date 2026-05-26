#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

            currentNode->data[lengths] = '\n';
            currentNode->data[lengths + 1] = '\0';

            currentNode = newNode;
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
                        printf("Memory allocation error.");
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


int main() {
    currentNode = (Node*)malloc(sizeof(Node));
    currentNode->next = NULL;
    currentNode->data = NULL;
    currentNode->capacity = 20;
    head = currentNode;
}