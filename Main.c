// Enzo Tres mediano 2211731
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LENGTH 256
#define MAX_SIZE 100

// structs------------------------------------------------------------------------------------------
typedef struct node Node;

struct node {
    char c;
    int freq;
    Node* left;
    Node* right;
};
// Prototipos---------------------------------------------------------------------------------------
void count_letters(const char* input_string, long numBytes, int letter_counts[][2]);
Node* createNode(char c, int val);
Node* constructHuffmanTree(int stringList[][2], int size);
int compareFunc(const void* a, const void* b);
void freeHufftree(Node* root);
void assignHuffCodes(Node* root, char* code, int depth, char** huffCodes);
void encodeText(const char* text, int numBytes, char** huffCodes, FILE* output);
char* createArray(char* fileName, long* numbytes);
void printHuffCodes(Node* root, char* spaces);
void decode(Node* root, int* numBytes);

// Main---------------------------------------------------------------------------------------------

int main() {
    char* input_string;
    int letter_counts[MAX_LENGTH][2] = { 0 };
    int diffLetters = 0;
    int i = 0;
    long numBytes;

    /*printf("Enter a string: ");
    fgets(input_string, MAX_LENGTH, stdin);*/

    input_string = createArray("input.txt", &numBytes);

    count_letters(input_string, numBytes, letter_counts);

    printf("Tamanho do arquivo antes: %d\n", (int)numBytes);

    int bigChar = 0;

    printf("Letter Counts:\n");
    for (int i = 0; i < MAX_LENGTH; i++) {
        if (letter_counts[i][0] == 0) {
            break;
        }
        if (letter_counts[i][0] > bigChar) {
            bigChar = letter_counts[i][0];
        }
        printf("%c: %d\n", letter_counts[i][0], letter_counts[i][1]);
    }

    for (i = 0; i < MAX_LENGTH; i++) {
        if ((int)letter_counts[i][1] == 0) {
            break;
        }
        diffLetters++;
    }

    //Building Huffman Tree and assigning Codes
    //
    //
    Node* root = constructHuffmanTree(letter_counts, diffLetters);

    printf("Visual Representation of the Huffman Tree\n\n");
    printHuffCodes(root, "");

    char** huffmanCodes = (char**)malloc(sizeof(char*) * (bigChar + 1));

    char code[60];

    printf("\nHuffmanCode:\n");
    assignHuffCodes(root, code, 0, huffmanCodes);
    
    //Encoding the Text to a compressed File
    //
    //
    FILE* compressFile = fopen("compressed.bin", "wb");

    encodeText(input_string, (int)numBytes, huffmanCodes, compressFile);

    fclose(compressFile);
    
    //DECODING
    //
    //
    printf("\nResult of decoding:\n");

    decode(root, &(int)numBytes);
    printf("\n");

    free(huffmanCodes);
    freeHufftree(root);
    free(input_string);
    return 0;
}
// Functions ---------------------------------------------------------------------------------------

void count_letters(const char* input_string, long numBytes, int letter_counts[][2]) {
    int i, j;
    

    for (i = 0; i < numBytes; i++) {
        char c = input_string[i];

        for (j = 0; j < MAX_LENGTH; j++) {
            if (letter_counts[j][0] == c) {
                letter_counts[j][1]++;
                break;
            }

            if (letter_counts[j][0] == 0) {
                letter_counts[j][0] = c;
                letter_counts[j][1] = 1;
                break;
            }
        }
    }
}

Node* createNode(char c, int freq) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error allocating space!");
        exit(2);
    }
    newNode->freq = freq;
    newNode->c = (char)c;
    newNode->right = NULL;
    newNode->left = NULL;
    return newNode;
}

Node* constructHuffmanTree(int stringList[][2], int size) {
    Node** nodes = (Node**)malloc(sizeof(Node*) * size);
    if (nodes == NULL) {
        printf("Error allocating space!");
        exit(2);
    }
    
    for (int i = 0; i < size; i++) {
        nodes[i] = createNode((char)(stringList[i][0]), stringList[i][1]);
        //printf("Letter: %c \t Number: %d\n", nodes[i]->c, nodes[i]->freq);
    }
    printf("\n");

    while (size > 1) {
        
        qsort(nodes, size, sizeof(Node*), compareFunc);
        for (int i = 0; i < size; i++) {
            //printf("Char-Freq: %c %d ",nodes[i]->c, nodes[i]->freq);
        }
        //printf("\n");
        Node* newNode = createNode('+', (nodes[0]->freq + nodes[1]->freq));

        newNode->left = nodes[0];
        newNode->right = nodes[1];

        nodes[0] = newNode;
        nodes[1] = NULL;

        size--;

        for (int i = 1; i < size; i++) {
            nodes[i] = nodes[i + 1];
        }
    }

    Node* root = nodes[0];
    free(nodes);
    return root;
}

void freeHufftree(Node* root) {
    if (root == NULL) {
        return;
    }

    freeHufftree(root->left);
    freeHufftree(root->right);
}

int compareFunc(const void* a, const void* b) {
    
    Node** nodeA = (Node**)a;
    Node** nodeB = (Node**)b;
    //printf("NodeA Char: %c Freq: %d \t NodeB Char: %c Freq: %d\n", (*nodeA)->c, (*nodeA)->freq, (*nodeB)->c, (*nodeB)->freq);
    return (*nodeA)->freq - (*nodeB)->freq;
}

void assignHuffCodes(Node* root, char* code, int depth, char** huffCodes) {

    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        printf("Letter: %c\t Code: %s\n", root->c, code);
        huffCodes[(unsigned char)root->c] = strdup(code);
        return;
    }

    code[depth] = '0';
    assignHuffCodes(root->left, code, depth + 1, huffCodes);

    code[depth] = '1';
    assignHuffCodes(root->right, code, depth + 1, huffCodes);

    //All codes are stored in an array called huffCodes
}

void printHuffCodes(Node* root, char* spaces) {

    if (root == NULL) {
        return;
    }

    
    printf("%s Letter: %c\t Freq: %d\n", spaces, root->c, root->freq);
   
    char* newSpaces = (char*)malloc(sizeof(char) * (strlen(spaces)+strlen("  ") + 1));
    if (newSpaces == NULL) {
        printf("Error allocating space!");
        exit(2);
    }
    strcpy(newSpaces, spaces);
    strcat(newSpaces, "  ");

    printHuffCodes(root->left, newSpaces);

    printHuffCodes(root->right, newSpaces);

    free(newSpaces);
    //All codes are stored in an array called huffCodes
}

void encodeText(const char* text, int numBytes, char** huffCodes, FILE* output) {

    fwrite(&numBytes, sizeof(unsigned int), 1, output);

    unsigned char buffer = 0;
    int bufferPos = 0;

    for (unsigned int i = 0; i < numBytes; i++) {
        char* code = huffCodes[text[i]];
        unsigned int codeLen = strlen(code);

        for (unsigned int j = 0; j < codeLen; j++) {
            buffer <<= 1;
            buffer |= code[j] - '0';
            bufferPos++;

            if (bufferPos == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, output);
                buffer = 0;
                bufferPos = 0;
            }
        }
    }

    //When we dont get a full buffer (aka the last thing to be put into the arquivo)
    if (bufferPos > 0) {
        buffer <<= (8 - bufferPos);
        fwrite(&buffer, sizeof(unsigned char), 1, output);
    }
}

char* createArray(char* fileName, long* numBytes) {
    FILE* textfile;
    char* text;

    textfile = fopen(fileName, "r");
    if (textfile == NULL)
        return 1;

    fseek(textfile, 0L, SEEK_END);
    *numBytes = ftell(textfile);
    fseek(textfile, 0L, SEEK_SET);

    text = (char*)malloc(*numBytes * sizeof(char));
    if (text == NULL)
        return 1;

    fread(text, sizeof(char), *numBytes, textfile);
    fclose(textfile);

    return text;
}

void decode(Node* root, int* numBytes) {
    
    Node* Aux = root;
    int countChar = 0;
    unsigned char myByte;
    unsigned char myBit;

    FILE* binFile = fopen("compressed.bin", "rb");
    if (binFile == NULL)
        return 1;
    
    fseek(binFile, 0L, SEEK_END);
    *numBytes = (int)ftell(binFile);
    fseek(binFile, 0L, SEEK_SET);

    int numChars;
    fread(&numChars, sizeof(int), 1, binFile);

    *numBytes -= sizeof(int);

    printf("Tamanho do arquivo depois: %d\n", (int)*numBytes);

    char* vBytes = (char*)malloc(*numBytes * sizeof(char));
    if (vBytes == NULL)
        return 1;

    fread(vBytes, sizeof(char), numBytes, binFile);

    fclose(binFile);

    FILE* textfile = fopen("output.txt", "w");
    for (int i = 0; i < numBytes; i++) {
        myByte = vBytes[i];

        for (int j = 0; j < 8; j++) {
            myBit = myByte & 0x80;
            myByte <<= 1;
            if (myBit == 0) {
                Aux = Aux->left;
            }
            else {
                Aux = Aux->right;
            }
            if (Aux->right == NULL && Aux->left == NULL) {
                printf("%c", Aux->c);
                fprintf(textfile, "%c", Aux->c);
                Aux = root;
                countChar++;
                if (countChar == numChars) {
                    return;
                }
            }
        }
    }
    free(vBytes);
}