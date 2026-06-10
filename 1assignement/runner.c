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

typedef enum { modify, newline } OpType;

typedef struct UndoNode {
    OpType op;
    int lineIndex;   
    char* oldData;
    int oldCapacity;
    char* newData;
    int newCapacity;
    struct UndoNode* prev;
    struct UndoNode* next;
} UndoNode;

UndoNode* undoTop = NULL;
UndoNode* redoTop = NULL;

static char* CopyText(const char* s, int cap) {
    if (s == NULL) return NULL;
    size_t len = strlen(s);
    char* copy = (char*)malloc((cap + 1) * sizeof(char));
    if (copy) strcpy(copy, s);
    return copy;
}

Node* FindNodeByIndex(int lineIndex) {
    Node* temp = head;
    int index = 0;
    while (temp != NULL && index < lineIndex) {
        temp = temp->next;
        index++;
    }
    return temp;
}

static int NodeIndex(Node* node) {
    Node* temp = head;
    int index = 0;
    while (temp != NULL && temp != node) {
        temp = temp->next;
        index++;
    }
    if (temp == node) return index;
    else return -1;
}

static void ClearRedoStack() {
    while (redoTop != NULL) {
        UndoNode* delete = redoTop;
        redoTop = redoTop->next;
        free(delete->oldData);
        free(delete->newData);
        free(delete);
    }
}


static UndoNode* BeginModify(Node* node) {
    ClearRedoStack();

    UndoNode* nodeUndo = (UndoNode*)malloc(sizeof(UndoNode));
    if (!nodeUndo) return NULL;

    nodeUndo->op = modify;
    nodeUndo->lineIndex = NodeIndex(node);
    nodeUndo->oldCapacity = node->capacity;
    nodeUndo->oldData = CopyText(node->data, nodeUndo->oldCapacity);

    nodeUndo->newData = NULL;
    nodeUndo->newCapacity = 0;
    nodeUndo->prev = NULL;
    nodeUndo->next = NULL;

    return nodeUndo;
}

static void EndModify(UndoNode* nodeUndo, Node* node) {
    if (!nodeUndo) return;

    nodeUndo->newCapacity = node->capacity;
    nodeUndo->newData = CopyText(node->data, nodeUndo->newCapacity);

    nodeUndo->prev = undoTop;
    nodeUndo->next = NULL;
    if (undoTop) undoTop->next = nodeUndo;
    undoTop = nodeUndo;
}

static void RecordNewLine(int lineIndex) {
    ClearRedoStack();

    UndoNode* nodeUndo = (UndoNode*)malloc(sizeof(UndoNode));
    if (!nodeUndo) return;

    nodeUndo->op = newline;
    nodeUndo->lineIndex = lineIndex;
    nodeUndo->oldData = NULL;
    nodeUndo->newData = NULL;

    nodeUndo->prev = undoTop;
    nodeUndo->next = NULL;
    if (undoTop) undoTop->next = nodeUndo;
    undoTop = nodeUndo;
}


void Undo() {
    if (undoTop == NULL) {
        printf("No changes to undo.\n");
        return;
    }

    UndoNode* nodeUndo = undoTop;
    undoTop = nodeUndo->prev;
    if (undoTop) undoTop->next = NULL;

    if (nodeUndo->op == modify) {
        Node* node = FindNodeByIndex(nodeUndo->lineIndex);
        if (!node) { 
            printf("Undo error: line not found.\n"); 
            return; 
        }

        free(node->data);
        node->data = CopyText(nodeUndo->oldData, nodeUndo->oldCapacity);
        node->capacity = nodeUndo->oldCapacity;

    }
    else {
        if (nodeUndo->lineIndex == 0) {
            
            printf("Undo error: cannot remove head.\n");
            
        }
        else {
            Node* prev = FindNodeByIndex(nodeUndo->lineIndex - 1);
            if (!prev || !prev->next) {
                printf("Undo error: line not found.\n");
                return;
            }
            Node* toDelete = prev->next;

            if (cursor.line == toDelete) {
                cursor.line = prev;
                cursor.index = 0;
            }
            if (currentNode == toDelete) currentNode = prev;

            prev->next = toDelete->next;
            free(toDelete->data);
            free(toDelete);

            if (prev->data != NULL) {
                int len = strlen(prev->data);
                if (len > 0 && prev->data[len - 1] == '\n') prev->data[len - 1] = '\0';
            }
        }
    }

    nodeUndo->prev = NULL;
    nodeUndo->next = redoTop;
    if (redoTop) redoTop->prev = nodeUndo;
    redoTop = nodeUndo;

    Node* last = head;
    while (last->next != NULL) last = last->next;
    currentNode = last;

    cursor.index = 0;
    printf("Undo completed.\n");
}

void Redo() {
    if (redoTop == NULL) {
        printf("No available info to redo.\n");
        return;
    }

    UndoNode* nodeUndo = redoTop;
    redoTop = nodeUndo->next;
    if (redoTop) redoTop->prev = NULL;

    if (nodeUndo->op == modify) {
        Node* node = FindNodeByIndex(nodeUndo->lineIndex);
        if (!node) { 
            printf("Redo error: line not found.\n"); 
            return; 
        }

        free(node->data);
        node->data = CopyText(nodeUndo->newData, nodeUndo->newCapacity);
        node->capacity = nodeUndo->newCapacity;

    }
    else {
        Node* prev = FindNodeByIndex(nodeUndo->lineIndex - 1);
        if (!prev) { 
            printf("Redo error: line not found.\n"); 
            return; 
        }

        if (prev->capacity < strlen(prev->data) + 2) {
            char* test = realloc(prev->data, strlen(prev->data) + 2);

            if (!test) {
                printf("Memory allocation error.\n");
                return;
            }

            prev->capacity = strlen(prev->data) + 2;
            prev->data = test;
        }

        size_t len = strlen(prev->data);

        prev->data[len] = '\n';
        prev->data[len + 1] = '\0';

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (!newNode) return;
        newNode->data = NULL;
        newNode->capacity = 20;

        char* test = (char*)malloc(newNode->capacity * sizeof(char));
        
        if (!test) {
            printf("Memory allocation error.\n");
            return;
        }
        newNode->data = test;
        newNode->data[0] = '\0';
        newNode->next = prev->next;
        prev->next = newNode;
        currentNode = newNode;
    }

    nodeUndo->next = NULL;
    nodeUndo->prev = undoTop;
    if (undoTop) undoTop->next = nodeUndo;
    undoTop = nodeUndo;

    Node* last = head;
    while (last->next != NULL) last = last->next;
    currentNode = last;

    cursor.line = head;
    cursor.index = 0;
    printf("Redo completed.\n");
}

void RightAndLeft(bool side) {
    if (side) {
        if (cursor.line->data != NULL && strlen(cursor.line->data) > cursor.index)
            cursor.index++;
    }
    else if (cursor.line->data != NULL && cursor.index - 1 >= 0) {
        cursor.index--;
    }
}

void UpAndDown(bool side) {
    if (side) {
        Node* curNode = head;
        while (curNode != NULL && curNode->next != cursor.line)
            curNode = curNode->next;
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

bool PrintWithCursor() {
    if (head->data == NULL) {
        printf("No current text.\n");
        return false;
    }
    else {
        printf("\n");
        Node* printingNode = head;
        while (printingNode != NULL) {
            if (printingNode->data != NULL) {
                if (cursor.line != printingNode) {
                    printf("%s", printingNode->data);
                }
                else {
                    size_t len = strlen(printingNode->data);
                    for (size_t i = 0; i < len; i++) {
                        if (i == (size_t)cursor.index) printf("|");
                        printf("%c", printingNode->data[i]);
                    }
                    if (cursor.index == (int)len) printf("|");
                }
            }
            printingNode = printingNode->next;
        }
    }
    printf("\n");
    return true;
}

void ReadingConsole() {
    int ch1, ch2;

    printf("\n");

    if (!PrintWithCursor()) return;

    while (1) {

        ch1 = _getch();

        if (ch1 == 0xE0 || ch1 == 0) {

            ch2 = _getch();

            switch (ch2) {
            case 72: UpAndDown(true); break;
            case 80: UpAndDown(false); break;
            case 75: RightAndLeft(false); break;
            case 77: RightAndLeft(true); break;
            }
            PrintWithCursor();
        }
        else if (ch1 == 27) break;
    }
}

void newLine(int cap, bool isPrinting) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = NULL;
    newNode->capacity = cap;
    newNode->data = NULL;

    int prevIndex = NodeIndex(currentNode);

    size_t lengths = 0;

    if (currentNode->data == NULL) {
        currentNode->data = (char*)malloc(currentNode->capacity * sizeof(char));
        if (currentNode->data == NULL) {
            printf("Memory allocation error.\n");
            free(newNode);
            return;
        }
        currentNode->data[0] = '\n';
        currentNode->data[1] = '\0';
        currentNode->next = newNode;
        currentNode = newNode;
        RecordNewLine(prevIndex + 1);
        if (isPrinting) printf("New line is started\n");
    }
    else {
        lengths = strlen(currentNode->data);
        if (lengths > 0 && currentNode->data[lengths - 1] == '\n') {
            currentNode->next = newNode;
            currentNode = newNode;
            RecordNewLine(prevIndex + 1);
            if (isPrinting) printf("New line is started\n");
        }
        else {
            if (lengths + 2 > (size_t)currentNode->capacity) {
                currentNode->capacity += 20;
                char* result = realloc(currentNode->data, currentNode->capacity);
                if (result == NULL) {
                    if (isPrinting) printf("New line is started\n");
                    free(newNode);
                    return;
                }
                currentNode->data = result;
            }
            currentNode->data[lengths] = '\n';
            currentNode->data[lengths + 1] = '\0';
            currentNode->next = newNode;
            currentNode = newNode;
            RecordNewLine(prevIndex + 1);
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

    UndoNode* nodeUndo = BeginModify(currentNode);

    if ((size_t)currentNode->capacity - len <= 1) {
        currentNode->capacity += 20;
        char* result = realloc(currentNode->data, currentNode->capacity);
        if (result == NULL) { 
            printf("Memory reallocation error.\n"); 
            return; 
        }
        currentNode->data = result;
    }

    if (fgets(currentNode->data + len, currentNode->capacity - (int)len, stdin) == NULL) {
        EndModify(nodeUndo, currentNode);
        return;
    }

    len = strlen(currentNode->data);

    while (len > 0 && currentNode->data[len - 1] != '\n') {

        currentNode->capacity += 20;

        char* result = realloc(currentNode->data, currentNode->capacity);

        if (result == NULL) { 
            printf("Memory reallocation error.\n"); 
            break; 
        }
        currentNode->data = result;
        if (fgets(currentNode->data + len, currentNode->capacity - (int)len, stdin) == NULL) break;
        len = strlen(currentNode->data);
    }

    len = strlen(currentNode->data);
    if (len > 0 && currentNode->data[len - 1] == '\n')
        currentNode->data[len - 1] = '\0';

    EndModify(nodeUndo, currentNode);
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
        return; 
    }

    Node* savingNode = head;

    while (savingNode != NULL) {
        if (savingNode->data != NULL) fprintf(file, "%s", savingNode->data);
        savingNode = savingNode->next;
    }

    fclose(file);
    printf("Text has been saved successfully.\n");
}

void Load() {

    FILE* file;

    char fileName[100];

    printf("Enter file name:\n");
    scanf("%s", fileName);

    while (getchar() != '\n');

    file = fopen(fileName, "r");

    if (file == NULL) { 
        printf("Error opening file"); 
        return; 
    }

    if (head->data != NULL) {

        printf("All unsaved information will be deleted.\n");

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

    ClearRedoStack();

    while (undoTop != NULL) {
        UndoNode* clear = undoTop;
        undoTop = clear->prev;
        free(clear->oldData);
        free(clear->newData);
        free(clear);
    }

    currentNode = head;

    char buffer[50];

    while (fgets(buffer, 50, file) != NULL) {

        if (currentNode->data == NULL) {
            currentNode->data = (char*)malloc(currentNode->capacity * sizeof(char));
            currentNode->data[0] = '\0';
        }

        if (strlen(buffer) + strlen(currentNode->data) + 1 > (size_t)currentNode->capacity) {

            currentNode->capacity += 50;

            char* test = realloc(currentNode->data, currentNode->capacity);

            if (test == NULL) { 
                printf("Memory allocation error.\n"); 
                fclose(file); 
                return; 
            }

            currentNode->data = test;
        }

        strcat(currentNode->data, buffer);
        if (currentNode->data[strlen(currentNode->data) - 1] == '\n') {
            newLine(currentNode->capacity, false);
        }
    }

     while (undoTop != NULL) {
        UndoNode* clear = undoTop;
        undoTop = clear->prev;
        free(clear->oldData);
        free(clear->newData);
        free(clear);
    }

    fclose(file);
    cursor.line = head;
    cursor.index = 0;
    printf("Text has been loaded successfully.\n");
}

void Print() {
    if (head->data == NULL) { 
        printf("No current text.\n");
        return; 
    }

    Node* printingNode = head;
    while (printingNode != NULL) {
        if (printingNode->data != NULL) printf("%s", printingNode->data);
        printingNode = printingNode->next;
    }
}

void InsertPasteReplace(int choice) {

    Node* line;
    int index;
    char* text;
    int capacity = 20;

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
            fgets(text + lengths, capacity - (int)lengths, stdin);
        }

        text[strlen(text) - 1] = '\0';
    }
    else if (choice == 2) {
        if (past != NULL && strlen(past) > 0) text = past;
        else { 
            printf("Buffer is empty.\n"); 
            return; 
        }
    }
    else { 
        printf("Invalid command.\n"); 
        return; 
    }

    Node* countingNode = head;
    while (countingNode != NULL && countingNode != line)
        countingNode = countingNode->next;

    if (countingNode == NULL) {
        printf("Error: this line does not exist.\n");
        if (choice == 1 || choice == 3) free(text);
        return;
    }

    size_t len = strlen(text);

    if (choice == 3 && countingNode->data == NULL) {
        printf("No data in this line.\n");
        free(text);
        return;
    }

    if (countingNode->data != NULL) {

        size_t dataLen = strlen(countingNode->data);

        if (index < 0 || (int)dataLen < index) {
            printf("This index does not exist.\n");
            if (choice == 1 || choice == 3) free(text);
            return;
        }

        UndoNode* nodeUndo = BeginModify(countingNode);

        if (dataLen + len + 1 > (size_t)countingNode->capacity) {

            countingNode->capacity += (int)len + 1;

            char* test = realloc(countingNode->data, countingNode->capacity);

            if (test == NULL) {
                printf("Memory allocation error.\n");

                if (choice == 1 || choice == 3) free(text);

                countingNode->capacity -= (int)len + 1;
                free(nodeUndo->oldData); 
                free(nodeUndo);
                return;
            }
            countingNode->data = test;

        }

        if (choice == 1 || choice == 2) {

            if (dataLen > 0) {

                for (int i = (int)dataLen; i >= index; i--)
                    countingNode->data[i + len] = countingNode->data[i];
            }
            else {
                printf("Indexation error.\n");
                if (choice == 1 || choice == 3) free(text);
                free(nodeUndo->oldData);
                free(nodeUndo);
                return;
            }
        }

        for (int i = index; i < (int)len + index; i++)
            countingNode->data[i] = text[i - index];

        if (choice == 3 && index + (int)len > (int)dataLen)
            countingNode->data[index + len] = '\0';

        EndModify(nodeUndo, countingNode);
    }
    else {

        if (index != 0) {
            printf("This index does not exist. Line is empty.\n");
            if (choice == 1 || choice == 3) free(text);
            return;
        }

        UndoNode* nodeUndo = BeginModify(countingNode);

        countingNode->capacity = (int)len + 1;
        countingNode->data = (char*)malloc(countingNode->capacity * sizeof(char));

        strncpy(countingNode->data, text, len);

        countingNode->data[len] = '\0';

        EndModify(nodeUndo, countingNode);
    }

    if (choice == 1 || choice == 3) free(text);
}

void Search() {

    char target[50];
    Node* searchingNode = head;
    int line = 0;
    bool isFound = false;

    printf("Enter the text to search:\n");

    char* check = fgets(target, 50, stdin);

    if (check == NULL) {
        printf("Error: searching text is too long.\n");
        return;
    }

    size_t len = strlen(target);

    if (len > 0 && target[len - 1] == '\n') target[len - 1] = '\0';

    while (searchingNode != NULL) {
        if (searchingNode->data != NULL) {
            char* finder = strstr(searchingNode->data, target);
            if (finder != NULL) {
                printf("Text in position: %d %d\n", line, (int)(finder - searchingNode->data));
                isFound = true;
            }
        }
        searchingNode = searchingNode->next;
        line++;
    }
    if (!isFound) printf("Not found.\n");
}


void DeleteAndCut(bool cut) {

    Node* line = NULL;
    int indexStart = 0, indexEnd = 0;

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

    Node* deletingNode = head;

    while (deletingNode != NULL && deletingNode != line)
        deletingNode = deletingNode->next;

    if (deletingNode == NULL) {
        printf("Error: this line does not exist.\n");
        return;
    }

    if (deletingNode->data != NULL) {

        size_t dataLen = strlen(deletingNode->data);

        if (indexStart < 0 || indexStart >(int)dataLen || indexEnd < 0 || indexEnd >(int)dataLen || number <= 0) {
            printf("Invalid selection.\n");
            return;
        }

        UndoNode* nodeUndo = BeginModify(deletingNode);

        if (cut && number > 0) {

            free(past);
            past = (char*)malloc((number + 1) * sizeof(char));

            for (int i = 0; i < number; i++) {

                past[i] = deletingNode->data[indexStart + i];
            }

            past[number] = '\0';
        }

        for (int i = indexStart; i < (int)dataLen - number; i++) {
            deletingNode->data[i] = deletingNode->data[i + number];
        }

        deletingNode->data[dataLen - number] = '\0';

        EndModify(nodeUndo, deletingNode);
        printf("Information deleted.\n");
    }
    else printf("No data in this line.\n");
}

void Copy() {
    Node* line = NULL;
    int indexStart = 0, indexEnd = 0;

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

    Node* copyNode = head;

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

        if (indexStart < 0 || indexEnd >(int)dataLen || number <= 0) {
            printf("Invalid selection.\n");
            return;
        }

        free(past);
        past = (char*)malloc((number + 1) * sizeof(char));

        if (past != NULL) {
            for (int i = 0; i < number; i++) {
                past[i] = copyNode->data[indexStart + i];
            }

            past[number] = '\0';
        }
        else printf("Memory allocation error.\n");
    }
    else printf("No data in this line.\n");
}

int main() {
    currentNode = (Node*)malloc(sizeof(Node));
    currentNode->next = NULL;
    currentNode->data = NULL;
    currentNode->capacity = 20;
    head = currentNode;

    cursor.line = head;
    cursor.index = 0;

    past = NULL;
    undoTop = NULL;
    redoTop = NULL;

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
            Node* delete = head;
            while (delete != NULL) {
                Node* deleteNext = delete->next;
                free(delete->data);
                free(delete);
                delete = deleteNext;
            }
            ClearRedoStack();
            while (undoTop != NULL) {
                UndoNode* delete = undoTop;
                undoTop = delete->prev;
                free(delete->oldData);
                free(delete->newData);
                free(delete);
            }
            if (past) free(past);
            return 0;
        }

        default:
            printf("Invalid command! Please try again.\n");
            break;
        }
    }
    return 0;
}