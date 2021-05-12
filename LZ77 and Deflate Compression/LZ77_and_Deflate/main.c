#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define OFFSETBITS 12
#define LENGTHBITS (16 - OFFSETBITS)

#define OFFSETMAX ((1 << (OFFSETBITS)) - 1)
#define LENGTHMAX ((1 << (LENGTHBITS)) - 1) // 1000

#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

struct token
{
    uint8_t offset_length;
    char c;
} token;

int coupling(char *search, char *lookahead, int limit) // Compares strings
{
    int length = 0;

    while (*search++ == *lookahead++ && length < limit)
        length++;

    return length; // length of equal parts between Search buffer and look ahead buffer
}

struct token *encode(char *text, int limit, int *numTokens)
{
    int cap = 1 << 3; //1000 = 8 bit

    int number_of_tokens = 0;

    char *Look_ahead_buffer, *Search_buffer;

    struct token *encoded = malloc(cap * sizeof(struct token));

    for (Look_ahead_buffer = text; Look_ahead_buffer < text + limit; Look_ahead_buffer++)
    {
        Search_buffer = Look_ahead_buffer - OFFSETMAX;

        if (Search_buffer < text)
            Search_buffer = text;

        int max_length = 0;

        char *max_match = Look_ahead_buffer;
        //printf("%s\n\n", max_match);

        for (; Search_buffer < Look_ahead_buffer; Search_buffer++) //searching in search buffer
        {
            int length = coupling(Search_buffer, Look_ahead_buffer, LENGTHMAX);

            if (length > max_length)
            {
                max_length = length;
                max_match = Search_buffer;
            }
        }

        if (Look_ahead_buffer + max_length >= text + limit)
        {
            max_length = text + limit - Look_ahead_buffer - 1;
        }

        //printf("%d  ", Look_ahead_buffer - max_match);
        //printf("%d  ", max_length);

        token.offset_length = OFFSETLENGTH(Look_ahead_buffer - max_match, max_length);
        Look_ahead_buffer += max_length;
        token.c = *Look_ahead_buffer;

        //printf("%d ", number_of_tokens);

        if (number_of_tokens + 1 > cap)
        {
            cap = cap << 1;
            encoded = realloc(encoded, cap * sizeof(struct token)); // changing memory size with realloc
        }

        encoded[number_of_tokens++] = token;
    }

    if (numTokens)
        *numTokens = number_of_tokens;

    return encoded;
}

char *reading_file(FILE *file, int *size)
{
    char *content;
    fseek(file, 0, SEEK_END);
    *size = ftell(file); // ftell() returns the current file position
    content = malloc(*size);
    fseek(file, 0, SEEK_SET);
    fread(content, 1, *size, file); //fread() returns the text of the specified size
    return content;
}

void frequency_of_letter(FILE *file)
{
    char ch;
    int size = 0, c = 0, j = 0;

    file = fopen("Sample.txt", "r");

    if (file == NULL)
    {
        printf("File can not open.\n");
        exit(0);
    }

    //char arr[95] = {};
    int frequency[128] = {};
    char *arr;

    while(!feof(file))
    {
        fscanf(file, "%c", &ch);
        frequency[ch]++;
    }

    fclose(file);

    for(int i = 32; i <= 126; i++)
    {
        if (frequency[i] != 0)
        {
            //printf("%d    %c     %d\n", i, i, frequency[i]);
            arr = (char*)malloc(sizeof(char));
            *arr = i;
            size++;
        }
    }

    //size = sizeof(arr) / sizeof(arr[0]);
    //printf("%d", size);

    HuffmanCodes(&arr, frequency, size);
}

struct MinHeapNode
{
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap
{
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* newNode(char data, unsigned freq)
{
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc
                               (sizeof(struct MinHeapNode));

    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity)
{
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode*)malloc(minHeap->capacity * sizeof(struct MinHeapNode));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]-> freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->
            freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx)
    {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(struct MinHeap* minHeap)
{
    return (minHeap->size == 1);
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];

    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
    {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap* minHeap)

{
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

void printArr(FILE *file, int arr[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        fprintf(file, "%d", arr[i]);
        //printf("%d", arr[i]);
        //printf("%d\n", n);
    }
    //printf("\n");
}

int isLeaf(struct MinHeapNode* root)
{
    return !(root->left) && !(root->right);
}

struct MinHeap* Create_min_stack(char *data, int freq[], int size)
{
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

struct MinHeapNode* building_huffman(char *data, int freq[], int size)
{
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = Create_min_stack(&data, freq, size);
    while (!isSizeOne(minHeap))
    {

        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

void printCodes(struct MinHeapNode* root, int arr[], int top)
{
    FILE *output;
    output = fopen("HuffmanOutput.txt", "a+");


    if (root->left)
    {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right)
    {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (isLeaf(root))
    {

        fprintf(output, "", root->data);
        //printf("%c : ", root->data);
        printArr(output, arr, top);
    }
}

void HuffmanCodes(char *data, int freq[], int size)
{
    struct MinHeapNode* root = building_huffman(&data, freq, size);
    int arr[100], top = 0;
    printCodes(root, arr, top);
}

int main(void)
{
    FILE *file;
    int text_size = 8, number_of_token;
    char *source_text = "aaaaaaaa";
    struct token *encoded_text;


    if (file = fopen("Sample.txt", "rb"))
    {
        source_text = reading_file(file, &text_size); // all text = source text
        fclose(file);
    }
    //printf("%s %d ", source_text, text_size);

    encoded_text = encode(source_text, text_size, &number_of_token);

    if (file = fopen("encodedLZ77.txt", "wb"))
    {
        fwrite(encoded_text, sizeof(struct token), number_of_token, file);
        fclose(file);
    }

    printf("Original Size: %d, Encode Size: %d  (LZ77) \n", text_size, number_of_token * sizeof(struct token));

    frequency_of_letter(file);

    if (file = fopen("HuffmanOutput.txt", "rb"))
    {
        source_text = reading_file(file, &text_size); // all text = source text
        fclose(file);
    }
    //printf("%s %d ", source_text, text_size);

    encoded_text = encode(source_text, text_size, &number_of_token);

    if (file = fopen("encodedDEFLATE.txt", "wb"))
    {
        fwrite(encoded_text, sizeof(struct token), number_of_token, file);
        fclose(file);
    }

    printf("Original Size: %d, Encode Size: %d  (DEFLATE)", text_size, number_of_token * sizeof(struct token));

}
