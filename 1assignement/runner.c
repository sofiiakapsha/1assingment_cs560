#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>

typedef struct Node {
    char* data;
    int capacity;
    struct Node* next;
}Node;

Node* currentNode;
Node* head;
char* past;

typedef struct Cursor {
    Node* line;
    int index;
}Cursor;

Cursor cursor;


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

void CreateUndoNode(Node* originNode, bool newNode) {

    if (originNode == NULL) {
        printf("Error: originNode is NULL\n");
        return;
    }

    if (headUndoNode != curUndoNode) {
        UndoNode* toDelete = headUndoNode;
        while (toDelete != NULL && toDelete != curUndoNode) {
            UndoNode* temp = toDelete;
            toDelete = toDelete->nextUndo;
            free(temp->data);
            free(temp);
        }

        headUndoNode = curUndoNode;
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


    char* tester = (char*)malloc(originNode->capacity * sizeof(char));

    if (tester == NULL) {
        printf("Memory allocation error.\n");
        free(newNode);
        return;
    }
    newUndo->data = tester;
    if (originNode->data != NULL) {
        strcpy(newUndo->data, originNode->data);
    }
    else {
        newUndo->data[0] = '\0';
    }

    newUndo->nextUndo = curUndoNode;
    curUndoNode = newUndo;
    headUndoNode = curUndoNode;
}


void newLine(int cap, bool isPrinting) {

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

        CreateUndoNode(currentNode, true);

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

            CreateUndoNode(currentNode, true);

            currentNode->data[lengths] = '\n';
            currentNode->data[lengths + 1] = '\0';
            currentNode = newNode;
            if (isPrinting) printf("New line is started\n");
        }
    }
}

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

    if (len > 0 && currentNode->data[len - 1] == '\n') {
        newLine(currentNode->capacity, false);
    }

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

    char fileName[100];

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
        cursor.line = head;
        cursor.index = 0;
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

void InsertPasteReplace(int choice) {

    Node* line;
    int index;
    char* text;
    int capacity = 20;
    Node* countingNode = head;

    printf("Choose line and index:\n");
    ReadingConsole();

    line = cursor.line;
    index = cursor.index;

    if (choice == 1 || choice == 3) {
        text = (char*)malloc(capacity * sizeof(char));

        if (choice == 1) printf("Enter text to insert:\n");
        else printf("Enter text to replace:\n");

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
    }
    else if (choice == 2) {
        if (past != NULL && strlen(past) > 0) {
            text = past;
        }
        else {
            printf("Buffer is empty.\n");
            return;
        }
    }
    else {
        printf("Invalid command.\n");
        return;
    }

    while (countingNode != NULL && countingNode != line) {
        countingNode = countingNode->next;
    }

    if (countingNode == NULL) {
        printf("Error: this line does not exist.\n");
        if (choice == 1 || choice == 3) {
            free(text);
        }
        return;
    }

    size_t len = strlen(text);

    if (countingNode->data != NULL) {

        size_t dataLen = strlen(countingNode->data);

        if (index < 0 || dataLen < index) {
            printf("This index does not exist.\n");
            if (choice == 1 || choice == 3) {
                free(text);
            }
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

        if (choice == 1 || choice == 2) {

            if (dataLen > 0) {

                for (int i = (int)dataLen; i >= index; i--) {
                    countingNode->data[i + len] = countingNode->data[i];
                }
            }
            else {
                printf("Indexation error.\n");
                free(text);
                return;
            }
        }

 
        for (int i = index; i < len + index; i++) {
            countingNode->data[i] = text[i - index];
        }

        if (choice == 3 && index + len > dataLen) {
            countingNode->data[index + len] = '\0';
        }
    }
    else {

        if (index != 0) {
            printf("This index does not exist. Line is empty.\n");
            if (choice == 1 || choice == 3) {
                free(text);
            }            
            return;
        }

        CreateUndoNode(countingNode, false);

        countingNode->capacity = len + 1;

        countingNode->data = (char*)malloc(countingNode->capacity * sizeof(char));

        strncpy(countingNode->data, text, len);

        countingNode->data[len] = '\0';
    }

    if (choice == 1 || choice == 3) {
        free(text);
    }
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

    Node* line = NULL;
    int indexStart = 0;

    int indexEnd = 0;

    Node* deletingNode = head;

    printf("Choose line, index for start and then for end (only in one line):\n");

    ReadingConsole();
    line = cursor.line;
    indexStart = cursor.index;

    ReadingConsole();
    indexEnd = cursor.index;

    if (line != cursor.line) {
        printf("Only in one line!\n");
        return;
    }

    int number = indexEnd - indexStart;


    while (deletingNode != NULL && deletingNode != line) {
        deletingNode = deletingNode->next;
    }

    if (deletingNode == NULL) {
        printf("Error: this line does not exist.\n");
        return;
    }

    if (deletingNode->data != NULL) {

        size_t dataLen = strlen(deletingNode->data);


        if (indexStart < 0 || indexEnd >dataLen || number <= 0) {
            printf("Invalid selection.\n");
            return;
        }

        CreateUndoNode(deletingNode, false);

        if (cut && number > 0) {

            free(past);
            past = (char*)malloc((number + 1) * sizeof(char));

            for (int i = 0; i < number; i++) {

                past[i] = deletingNode->data[indexStart + i];
            }

            past[number] = '\0';
        }

        for (int i = indexStart; i < dataLen - number; i++) {
            deletingNode->data[i] = deletingNode->data[i + number];
        }

        deletingNode->data[dataLen - number] = '\0';

        printf("Information deleted.\n");
    }
    else printf("No data in this line.\n");
}

void Undo() {
    if (curUndoNode->nextUndo == NULL) {
        printf("No changes to undo.\n");
        return;
    }

    UndoNode* prevUndo = curUndoNode->nextUndo;
    Node* tempNode = curUndoNode->prev;


    if (tempNode == NULL) {
        printf("Error: Invalid undo state.\n");
        return;
    }

    int tempCap = tempNode->capacity;
    Node* tempNext = tempNode->next;

    char* tempData = (char*)malloc(tempNode->capacity * sizeof(char));

    if (tempNode->data != NULL) {
        strcpy(tempData, tempNode->data);
    }
    else {
        tempData[0] = '\0';
    }

    if (tempNode->data != NULL) {
        free(tempNode->data);
    }

    tempNode->capacity = curUndoNode->capacity;
    tempNode->data = (char*)malloc(tempNode->capacity * sizeof(char));

    if (curUndoNode->data != NULL) {
        strcpy(tempNode->data, curUndoNode->data);
    }
    else {
        tempNode->data[0] = '\0';
    }

    if (tempNode->data == NULL) {
        printf("Memory allocation error.\n");
        free(tempData);
        return;
    }

    free(curUndoNode->data);

    curUndoNode->data = tempData;
    curUndoNode->capacity = tempCap;
    curUndoNode->next = tempNext;

    if (curUndoNode->newLine == true) 
    {
        if (tempNode->next != NULL) {
            if (cursor.line == tempNode->next) {
                cursor.line = currentNode;
                cursor.index = 0;
            }

            if (tempNode->next->data != NULL) {
                free(tempNode->next->data);
            }
            free(tempNode->next);
            tempNode->next = NULL;
            currentNode = tempNode;
        }
    }
    
    cursor.index = 0;

    curUndoNode = prevUndo;
    printf("Undo completed.\n");
}

void Redo() {
    if (headUndoNode == curUndoNode) {
        printf("No available info to redo.\n");
        return;
    }

    UndoNode* RedoNode = headUndoNode;

    while (RedoNode != NULL && RedoNode->nextUndo != curUndoNode) {
        RedoNode = RedoNode->nextUndo;
    }

    if (RedoNode == NULL) {
        printf("Error: Invalid redo state.\n");
        return;
    }

    if(RedoNode->prev == NULL) {
        printf("Error: Invalid redo state.\n");
        return;
    }

    Node* targetNode = RedoNode->prev;

    int tempCap = targetNode->capacity;
    Node* tempNext = targetNode->next;


    if (RedoNode->newLine == true) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->capacity = 20;
        newNode->next = NULL;

        newNode->data = (char*)malloc(newNode->capacity * sizeof(char));
        newNode->data[0] = '\0';

        targetNode->next = newNode;

        currentNode = newNode;

        RedoNode->capacity = tempCap;
        RedoNode->next = newNode;
    }
    else {

        char* temp = (char*)malloc(tempCap * sizeof(char));
        if (targetNode->data != NULL) {
            strncpy(temp, targetNode->data, tempCap);
        }
        else {
            temp[0] = '\0';
        }
        targetNode->capacity = RedoNode->capacity;
        targetNode->next = RedoNode->next;

        free(targetNode->data);
        targetNode->data = (char*)malloc(RedoNode->capacity * sizeof(char));
        strncpy(targetNode->data, RedoNode->data, RedoNode->capacity - 1);
        targetNode->data[RedoNode->capacity - 1] = '\0';

        free(RedoNode->data);
        RedoNode->data = temp;
        RedoNode->capacity = tempCap;
        RedoNode->next = tempNext;
    }

    curUndoNode = RedoNode;
    printf("Redo completed.\n");
}

void Copy() {

    Node* copyNode = head;

    Node* line = NULL;
    int indexStart = 0;

    int indexEnd = 0;

    Node* deletingNode = head;

    printf("Choose line, index for start and then for end (only in one line):\n");

    ReadingConsole();
    line = cursor.line;
    indexStart = cursor.index;

    ReadingConsole();
    indexEnd = cursor.index;

    if (line != cursor.line) {
        printf("Only in one line!\n");
        return;
    }


    while (copyNode != NULL && copyNode != line) {
        copyNode = copyNode->next;
    }

    if (copyNode == NULL) {
        printf("Error: this line does not exist.\n");
        return;
    }


    if (copyNode->data != NULL) {

        size_t dataLen = strlen(copyNode->data);

        int number = indexEnd - indexStart;

        if (indexStart < 0 || indexEnd >dataLen || number <= 0) {
            printf("Invalid selection.\n");
            return;
        }

        if (number > 0) {

            free(past);
            past = (char*)malloc((number + 1) * sizeof(char));

            for (int i = 0; i < number; i++) {

                past[i] = copyNode->data[indexStart + i];
            }

            past[number] = '\0';
        }
        else {
            printf("No data to copy.\n");
            return;
        }

    }
    else printf("No data in this line.\n");
}

void RightAndLeft(bool side) {
    if (side) {
        if (cursor.line->data != NULL && strlen(cursor.line->data) > cursor.index) {
            cursor.index++;
        }
    }
    else if (cursor.line->data != NULL && cursor.index - 1 >= 0) {
        cursor.index--;
    }
}

void UpAndDown(bool side){
    if (side) {
        Node* curNode = head;
        while (curNode != NULL && curNode->next != cursor.line) {
            curNode = curNode->next;
        }
        if (curNode != NULL && curNode->data != NULL) {
            cursor.line = curNode;
            cursor.index = 0;
        }
    }
    else if (cursor.line->next != NULL && cursor.line->next->data != NULL) {
        cursor.line = cursor.line->next;
        cursor.index = 0;
    }
}

void PrintWithCursor() {
    if (head->data == NULL) {
        printf("No current text.\n");
    }
    else {
        Node* printingNode = head;
        while (printingNode != NULL) {

            if (printingNode->data != NULL) {

                if (cursor.line != printingNode) {
                    printf("%s", printingNode->data);
                }

                else {
 
                    size_t len = strlen(printingNode->data);
                    for (size_t i = 0; i < len; i++) {

                        if (i == cursor.index) {
                            printf("|");
                        }
                        printf("%c", printingNode->data[i]);
                    }

                    if (cursor.index == len) {
                        printf("|");
                    }
                    printf("\n"); 
                }
            }
            printingNode = printingNode->next;
        }
    }
}

void ReadingConsole() {

    int ch1, ch2;

    while (1) {
        ch1 = _getch();

        if (ch1 == 0xE0 || ch1 == 0) {
            ch2 = _getch();

            switch (ch2) {
            case 72: UpAndDown(true); break;
            case 80: UpAndDown(false);; break;
            case 75: RightAndLeft(false); break;
            case 77: RightAndLeft(true); break;
            }

            system("cls");
            PrintWithCursor();
        }

        else if (ch1 == 27) break;
    }
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

    cursor.line = head;
    cursor.index = 0;

    past = NULL;

    int answer;

    while (1) {
        printf("\nChoose the command:\n");
        printf("1 - Append, 2 - newLine, 3 - Save, 4 - Load\n");
        printf("5 - Print,  6 - Insert,  7 - Search, 8 - Delete\n");
        printf("9 - Undo,  10 - Redo,  11 - Cut, 12 - Paste\n");
        printf("13 - Copy,  14 - Replace,  15 - Move cursor, 16 - Exit\n");

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
            InsertPasteReplace(1);
            break;
        case 7:
            Search();
            break;
        case 8:
            DeleteAndCut(false);
            break;
        case 9:
            Undo();
            break;
        case 10:
            Redo();
            break;
        case 11:
            DeleteAndCut(true);
            break;
        case 12:
            InsertPasteReplace(2);
            break;
        case 13:
            Copy();
            break;
        case 14:
            InsertPasteReplace(3);
            break;
        case 15:
            ReadingConsole();
            break;

        case 16:

        {
            Node* deletingNode = head;

            while (deletingNode != NULL) {

                currentNode = deletingNode->next;
                free(deletingNode->data);
                free(deletingNode);
                deletingNode = currentNode;
            }

            UndoNode* undoDelete = headUndoNode;
            while (undoDelete != NULL) {
                UndoNode* temp = undoDelete;
                undoDelete = undoDelete->nextUndo;
                if (temp->data != NULL) {
                    free(temp->data);
                }
                free(temp);
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