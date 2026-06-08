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
char* past;


typedef struct UndoNode {
    char* data;
    int capacity;
    struct UndoNode* nextUndo;
    Node* next;
    Node* prev;
    bool newLine;
}UndoNode;

UndoNode* curUndoNode;
UndoNode* headUndoNode;


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
    CreateUndoNode(currentNode, false);

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

void newLine(int cap, bool isPrinting) {

    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->next = NULL;
    newNode->capacity = cap;
    newNode->data = NULL;

    currentNode->next = newNode;

    size_t lengths = 0;

    CreateUndoNode(currentNode, true);

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
        if (isPrinting) printf("New line is started\n");
    }
    else {

        lengths = strlen(currentNode->data);

        if (lengths > 0 && currentNode->data[lengths - 1] == '\n') {
            currentNode = newNode;
            if (isPrinting) printf("New line is started\n");
        }
        else {

            if (lengths + 2 > currentNode->capacity) {
                currentNode->capacity += 20;
                char* result = realloc(currentNode->data, currentNode->capacity);

                if (result == NULL) {
                    if (isPrinting) printf("New line is started\n");
                    currentNode->next = NULL;
                    free(newNode);
                    return;
                }
                currentNode->data = result;
            }

            currentNode->data[lengths] = '\n';
            currentNode->data[lengths + 1] = '\0';
            currentNode = newNode;
            if (isPrinting) printf("New line is started\n");
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
                char* test = realloc(currentNode->data, currentNode->capacity);

                if (test == NULL) {
                    printf("Memory allocation error.\n");
                    return;
                }

                currentNode->data = test;

            }

            strcat(currentNode->data, buffer);

            if (currentNode->data[strlen(currentNode->data) - 1] == '\n') {
                newLine(currentNode->capacity, false);
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
        char* test_text = realloc(text, capacity);

        if (test_text == NULL) {
            printf("Memory allocation error.\n");
            free(text);
            return;
        }

        text = test_text;
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

        CreateUndoNode(countingNode, false);

        if (dataLen + len + 1 > countingNode->capacity) {
            countingNode->capacity += len + 1;

            char* test = realloc(countingNode->data, countingNode->capacity);
            if (test == NULL) {
                printf("Memory allocation error.\n");
                free(text);
                countingNode->capacity -= len + 1;
                return;
            }
            countingNode->data = test;

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


void DeleteAndCut(bool cut) {

    int line = 0;
    int index = 0;
    int number = 0;

    Node* deletingNode = head;
    int counter = 0;

    printf("Choose line, index and number of symbols:\n");
    scanf("%d %d %d", &line, &index, &number);

    while (getchar() != '\n');
    

    while (deletingNode != NULL && counter != line) {
        deletingNode = deletingNode->next;
        counter++;
    }

    if (deletingNode == NULL) {
        printf("Error: this line does not exist.\n");
        return;
    }


    if (deletingNode->data != NULL) {

        size_t dataLen = strlen(deletingNode->data);

        if (index < 0 || dataLen < index) {
            printf("This index does not exist.\n");
            return;
        }

        CreateUndoNode(deletingNode, false);

        if (cut && number > 0) {

            free(past);
            past = (char*)malloc((number + 1) * sizeof(char));

            for (int i = 0; i < number; i++) {

                past[i] = deletingNode->data[index + i];
            }

            past[number] = '\0';
        }

        for (int i = index; i < dataLen - number; i++) {
            deletingNode->data[i] = deletingNode->data[i + number];
        }

        deletingNode->data[dataLen - number] = '\0';

        printf("Information deleted.\n");
    }
    else printf("No data in this line.\n");
}


void CreateUndoNode(Node* originNode, bool newNode) {

    if (curUndoNode->nextUndo != NULL) {
        UndoNode* toDelete = curUndoNode->nextUndo;
        while (toDelete != NULL) {
            UndoNode* temp = toDelete;
            toDelete = toDelete->nextUndo;
            free(temp->data);
            free(temp);
        }
        curUndoNode->nextUndo = NULL;
    }

    UndoNode* newUndo = (UndoNode*)malloc(sizeof(UndoNode));
    newUndo->nextUndo = NULL;
    newUndo->data = NULL;
    newUndo->prev = NULL;
    newUndo->next = NULL;
    newUndo->newLine = newNode;

    newUndo->prev = originNode;
    newUndo->capacity = originNode->capacity;
    newUndo->next = originNode->next;

    newUndo->data = (char*)malloc(originNode->capacity * sizeof(char));
    if (originNode->data != NULL) {
        strcpy(newUndo->data, originNode->data);
    }
    else {
        newUndo->data[0] = '\0';
    }

    newUndo->nextUndo = curUndoNode;
    curUndoNode = newUndo;
}

void Undo() {
    if (curUndoNode->nextUndo == NULL) {
        printf("No changes to undo.\n");
        return;
    }

    UndoNode* prevUndo = curUndoNode->nextUndo;
    Node* tempNode = curUndoNode->prev;

    free(tempNode->data);
    tempNode->data = (char*)malloc(curUndoNode->capacity * sizeof(char));
    strcpy(tempNode->data, curUndoNode->data);
    tempNode->capacity = curUndoNode->capacity;
    tempNode->next = curUndoNode->next;

    if (curUndoNode->newLine == true && tempNode->next != NULL) {
        free(tempNode->next->data);
        free(tempNode->next);
        tempNode->next = NULL;
    }

    curUndoNode = prevUndo;
    printf("Undo completed.\n");
}


int main() {
    currentNode = (Node*)malloc(sizeof(Node));
    currentNode->next = NULL;
    currentNode->data = NULL;
    currentNode->capacity = 20;
    head = currentNode;

    curUndoNode = (UndoNode*)malloc(sizeof(UndoNode));
    curUndoNode->next = NULL;
    curUndoNode->data = NULL;
    curUndoNode->prev = NULL;
    headUndoNode = curUndoNode;

    past = (char*)malloc(20 * sizeof(char));

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
            newLine(20, true);
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
            DeleteAndCut(false);
            break;
        case 9:

        {
            Node* deletingNode = head;

            while (deletingNode != NULL) {

                currentNode = deletingNode->next;
                free(deletingNode->data);
                free(deletingNode);
                deletingNode = currentNode;
            }

            free(past);
            return 0;
        }

        default:
            printf("Invalid command! Please try again.\n");
            break;
        }
    }

    return 0;
}