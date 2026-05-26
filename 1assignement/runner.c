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