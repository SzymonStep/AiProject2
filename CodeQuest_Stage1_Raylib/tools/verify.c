/* =============================================================================
 *  tools/verify.c  --  Ground-truth generator for the question bank.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  Why this file exists
 *  --------------------
 *  The Section A questions in the app ask "what is the output?". Hard-coding
 *  those expected outputs by hand is exactly the sort of thing that silently
 *  goes wrong, so instead every lab function is implemented here to the lab
 *  sheet specification and this program prints the real output. The printed
 *  text is what gets baked into src/content.c.
 *
 *  This is a build-time tool, not part of the raylib app.
 *  Build: gcc tools/verify.c -o build/verify.exe && ./build/verify.exe
 * ========================================================================== */
#include <stdio.h>
#include <stdbool.h>

/* ============================ LAB 1 ======================================= */

int isLeapYear(int y)
{
    if (y % 4 != 0)                  return 0;
    if (y % 100 == 0 && y % 400 != 0) return 0;
    return 1;
}

int isPalindromeNumber(int n)
{
    int orig = n, rev = 0;
    while (n > 0) { rev = rev * 10 + n % 10; n /= 10; }
    return rev == orig;
}

int isPrime(int n)
{
    if (n < 2) return 0;
    for (int d = 2; d * d <= n; d++) if (n % d == 0) return 0;
    return 1;
}

int binaryToDecimal(int b)
{
    int val = 0, pow2 = 1;
    while (b > 0) {
        int bit = b % 10;
        val  += bit * pow2;
        pow2 *= 2;
        b    /= 10;
    }
    return val;
}

int parseDigitsToInt(char *s)
{
    int v = 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9') return 0;
        v = v * 10 + (*s - '0');
    }
    return v;
}

void printTriangleA(int n)
{
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) putchar('A');
        putchar('\n');
    }
}

void printUpDownA(int n)
{
    /* 2 for loops, at most one if/else: the row length walks up then back
     * down, so the outer loop runs 2n-1 times and picks the length. */
    for (int row = 1; row <= 2 * n - 1; row++) {
        int count = (row <= n) ? row : 2 * n - row;
        for (int j = 0; j < count; j++) putchar('A');
        putchar('\n');
    }
}

int find(int size, int arr[], int toFind)
{
    for (int i = 0; i < size; i++) if (arr[i] == toFind) return i;
    return -1;
}

int find2ndLargest(int size, int arr[])
{
    int largest = -1, second = -1;
    for (int i = 0; i < size; i++) {
        if (arr[i] > largest)                        { second = largest; largest = arr[i]; }
        else if (arr[i] < largest && arr[i] > second) { second = arr[i]; }
    }
    return second;
}

void copyArraytoArray(int size, int arr1[], int arr2[])
{
    for (int i = 0; i < size; i++) arr2[i] = arr1[i];
}

bool insertElement(int *size, int *count, int arr[], int x, int idx)
{
    if (*count >= *size || idx < 0 || idx > *count) return false;
    for (int i = *count; i > idx; --i) arr[i] = arr[i-1];
    arr[idx] = x;
    (*count)++;
    return true;
}

bool deleteElement(int *size, int *count, int arr[], int idx)
{
    (void)size;
    if (*count == 0 || idx < 0 || idx >= *count) return false;
    for (int i = idx; i < *count - 1; ++i) arr[i] = arr[i+1];
    *count -= 1;
    return true;
}

int frequencyCount(int size, int arr[], int value)
{
    int c = 0;
    for (int i = 0; i < size; i++) if (arr[i] == value) c++;
    return c;
}

int countDuplicates(int n, int a[])
{
    int dup = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i] == a[j]) { dup++; break; }
    return dup;
}

void reverseArr(int n, int a[])
{
    for (int L = 0, R = n - 1; L < R; L++, R--) {
        int t = a[L]; a[L] = a[R]; a[R] = t;
    }
}

int rotateLeft(int n, int a[])
{
    int first = a[0];
    for (int i = 0; i < n - 1; i++) a[i] = a[i+1];
    a[n-1] = first;
    return 1;
}

bool twoMovies(int L, int m[], int n)
{
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (m[i] + m[j] == L) return true;
    return false;
}

int wordCounter(int n, char s[])
{
    int words = 0, in = 0;
    for (int i = 0; i < n; i++) {
        char c = s[i];
        int isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        if (isLetter && !in)            { in = 1; words++; }
        else if (!isLetter && c == ' ')   in = 0;
    }
    return words;
}

/* ============================ LAB 2 ======================================= */

static void PrintArray(int a[4][6])
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 6; c++) printf("%d%s", a[r][c], c == 5 ? "" : " ");
        printf("\n");
    }
}

int ReplaceLargestValue(int a[4][6])
{
    int maxv = a[0][0];
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)
        if (a[r][c] > maxv) maxv = a[r][c];
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)
        if (a[r][c] == maxv) a[r][c] = -1;
    return maxv;
}

int CountAndReplaceZeros(int a[4][6])
{
    int count = 0;
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)
        if (a[r][c] == 0) { a[r][c] = -1; count++; }
    return count;
}

int ReplaceRowPattern(int a[4][6], int pattern[3])
{
    int matches = 0;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c <= 6 - 3; c++) {
            if (a[r][c]   == pattern[0] &&
                a[r][c+1] == pattern[1] &&
                a[r][c+2] == pattern[2]) {
                a[r][c] = a[r][c+1] = a[r][c+2] = -1;
                matches++;
                c += 2;                 /* do not re-match the cells we blanked */
            }
        }
    }
    return matches;
}

int FindColumnSum(int a[4][6], int col)
{
    int sum = 0;
    for (int r = 0; r < 4; r++) if (a[r][col] != -1) sum += a[r][col];
    return sum;
}

int ReplaceBorder(int a[4][6])
{
    int n = 0;
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)
        if (r == 0 || r == 3 || c == 0 || c == 5) { a[r][c] = -1; n++; }
    return n;
}

int CountUniqueValues(int a[4][6])
{
    int seen[21] = { 0 }, cnt = 0;
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++) {
        int v = a[r][c];
        if (v <= 0) continue;
        if (seen[v] == 0) { seen[v] = 1; cnt++; }
    }
    return cnt;
}

void ReverseArrayRows(int a[4][6])
{
    for (int r = 0; r < 4; r++)
        for (int L = 0, R = 5; L < R; L++, R--) {
            int t = a[r][L]; a[r][L] = a[r][R]; a[r][R] = t;
        }
}

int FindMostFrequent(int a[4][6])
{
    int freq[21] = { 0 };
    for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++) {
        int v = a[r][c];
        if (v <= 0) continue;
        freq[v] = freq[v] + 1;
    }
    int best = 1;
    for (int v = 2; v <= 20; v++) if (freq[v] > freq[best]) best = v;
    return best;
}

void ShiftArrayRight(int a[4][6])
{
    for (int r = 0; r < 4; r++) {
        int last = a[r][5];
        for (int c = 5; c > 0; c--) a[r][c] = a[r][c-1];
        a[r][0] = last;
    }
}

void SwapRows(int a[4][6], int r1, int r2)
{
    for (int c = 0; c < 6; c++) {
        int tmp = a[r1][c]; a[r1][c] = a[r2][c]; a[r2][c] = tmp;
    }
}

/* Gravity: in each column every value that is not -1 falls to the bottom,
 * keeping its relative order. Cells left empty at the top become -2. */
void FallDownAndReplace(int a[4][6])
{
    for (int c = 0; c < 6; c++) {
        int write = 3;
        for (int r = 3; r >= 0; r--)
            if (a[r][c] != -1) a[write--][c] = a[r][c];
        while (write >= 0) a[write--][c] = -2;
    }
}

/* ============================ LAB 3 ======================================= */

void swap_ints(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int find_max(int *arr, int size)
{
    int best = *arr;
    for (int i = 1; i < size; i++) { int v = *(arr + i); if (v > best) best = v; }
    return best;
}

void reverse_array(int *arr, int size)
{
    int *L = arr, *R = arr + size - 1;
    while (L < R) { int t = *L; *L = *R; *R = t; L = L + 1; R = R - 1; }
}

int strlen_ptr(char *s)
{
    char *p = s;
    while (*p != '\0') p++;
    return (int)(p - s);
}

int count_vowels(char *s)
{
    int n = 0;
    for (char *p = s; *p; p++) {
        char c = *p;
        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||
            c=='A'||c=='E'||c=='I'||c=='O'||c=='U') n++;
    }
    return n;
}

int sum_array(int *arr, int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++) sum += *(arr + i);
    return sum;
}

int compare_strings(char *a, char *b)
{
    while (*a && (*a == *b)) { a++; b++; }
    return (int)((unsigned char)*a) - (int)((unsigned char)*b);
}

/* ============================ DRIVER ====================================== */

static void hdr(const char *id) { printf("\n===== %s =====\n", id); }

int main(void)
{
    /* ---------------- LAB 1 ---------------- */
    hdr("L1-A1");
    printf("%d\n%d\n%d\n%d\n", isLeapYear(2000), isLeapYear(1900),
                               isLeapYear(2024), isLeapYear(2025));
    hdr("L1-A2");
    printf("%d\n%d\n", isPalindromeNumber(12321), isPalindromeNumber(12021));
    hdr("L1-A3");
    printf("%d\n%d\n%d\n", isPrime(29), isPrime(1), isPrime(21));
    hdr("L1-A4");
    printf("%d\n%d\n", binaryToDecimal(101101), binaryToDecimal(100000));
    hdr("L1-A5");
    { char s1[] = { '1','4','5','\0' }, s2[] = { '5','x','2','\0' };
      printf("%d\n%d\n", parseDigitsToInt(s1), parseDigitsToInt(s2)); }
    hdr("L1-A6"); printTriangleA(4);
    hdr("L1-A7"); printUpDownA(4);
    hdr("L1-A8");
    { int a[6] = {4,9,2,9,7,5};
      printf("%d\n%d\n", find(6,a,9), find(6,a,3)); }
    hdr("L1-A9");
    { int b[6] = {5,12,9,12,7,1}; printf("%d\n", find2ndLargest(6,b)); }
    hdr("L1-A10");
    { int s[4]={3,1,4,1}, d[4]={0,0,0,0};
      copyArraytoArray(4,s,d);
      for (int i=0;i<4;i++) printf("%d ", d[i]); printf("\n"); }
    hdr("L1-A11");
    { int size=6, count=3, arr[6]={2,4,6,0,0,0};
      printf("%d\n", insertElement(&size,&count,arr,5,1));
      for (int i=0;i<count;i++) printf("%d ", arr[i]); printf("\n"); }
    hdr("L1-A12");
    { int size=6, count=5, arr[6]={2,5,4,6,9,0};
      printf("%d\n", deleteElement(&size,&count,arr,2));
      for (int i=0;i<count;i++) printf("%d ", arr[i]); printf("\n"); }
    hdr("L1-A13");
    { int c[8]={1,2,2,3,2,4,2,5}; printf("%d\n", frequencyCount(8,c,2)); }
    hdr("L1-A14");
    { int d[8]={1,1,2,3,3,3,4,5}; printf("%d\n", countDuplicates(8,d)); }
    hdr("L1-A15");
    { int r[5]={1,2,3,4,5}; reverseArr(5,r);
      for (int i=0;i<5;i++) printf("%d ", r[i]); printf("\n"); }
    hdr("L1-A16");
    { int rot[5]={7,8,9,10,11}; rotateLeft(5,rot);
      for (int i=0;i<5;i++) printf("%d ", rot[i]); printf("\n"); }
    hdr("L1-A17");
    { int m[6]={90,85,75,60,120,150}; printf("%d\n", twoMovies(180,m,6)); }
    hdr("L1-A18");
    { char txt[] = "Hello,  world 42 is big";
      printf("%d\n", wordCounter(sizeof(txt)-1, txt)); }

    /* ---------------- LAB 2 ---------------- */
    hdr("L2-A1");
    { int A[4][6]={{1,2,4,1,3,4},{2,4,2,3,1,2},{1,3,4,4,2,1},{2,1,3,2,4,4}};
      int maxv = ReplaceLargestValue(A); PrintArray(A);
      printf("maxv=%d\n", maxv); }
    hdr("L2-A2");
    { int B[4][6]={{0,1,1,0,2,3},{2,0,3,1,0,0},{1,2,0,3,1,2},{0,0,1,2,3,0}};
      int c = CountAndReplaceZeros(B); PrintArray(B);
      printf("count=%d\n", c); }
    hdr("L2-A3");
    { int P[4][6]={{3,3,3,1,2,3},{1,3,3,3,2,1},{2,2,3,3,3,2},{3,2,1,3,3,3}};
      int pat[3]={3,3,3};
      int m = ReplaceRowPattern(P,pat); PrintArray(P);
      printf("matches=%d\n", m); }
    hdr("L2-A4");
    { int C[4][6]={{9,8,7,6,5,4},{1,2,3,4,5,6},{6,5,4,3,2,1},{4,5,6,7,8,9}};
      int r = ReplaceBorder(C); PrintArray(C);
      printf("replaced=%d\n", r); }
    hdr("L2-A5");
    { int D[4][6]={{1,-1,3,4,5,6},{-1,2,-1,1,0,2},{3,-1,1,-1,2,1},{4,2,-1,0,-1,3}};
      printf("col2sum=%d\n", FindColumnSum(D,2)); }
    hdr("L2-A6");
    { int E[4][6]={{1,2,-1,3,4,3},{1,-1,-1,3,4,-1},{1,2,-1,3,-1,2},{-1,-1,-1,2,3,3}};
      FallDownAndReplace(E); PrintArray(E); }
    hdr("L2-A7");
    { int G[4][6]={{1,2,2,0,3,-1},{2,2,3,1,-1,4},{4,0,1,2,3,3},{1,1,2,2,4,0}};
      printf("unique=%d\n", CountUniqueValues(G));
      printf("mostFrequent=%d\n", FindMostFrequent(G)); }
    hdr("L2-A8");
    { int H[4][6]={{1,2,3,4,5,6},{7,8,9,10,11,12},
                   {13,14,15,16,17,18},{19,20,21,22,23,24}};
      ShiftArrayRight(H); PrintArray(H); }
    hdr("L2-A9");
    { int I[4][6]={{1,2,3,4,5,6},{7,8,9,10,11,12},
                   {13,14,15,16,17,18},{19,20,21,22,23,24}};
      ReverseArrayRows(I); SwapRows(I,0,3); PrintArray(I); }

    /* ---------------- LAB 3 ---------------- */
    hdr("L3-A1");
    { int x=7,y=11; swap_ints(&x,&y); printf("x=%d y=%d\n", x, y); }
    hdr("L3-A2");
    { int a[6]={1,2,3,4,5,6}; reverse_array(a,6);
      for (int i=0;i<6;i++) printf("%d ", a[i]); printf("\n"); }
    hdr("L3-A3");
    { char s[]="Hello, world!"; printf("len=%d\n", strlen_ptr(s)); }
    hdr("L3-A4");
    { char t[]="OpenAI rocks"; printf("vowels=%d\n", count_vowels(t)); }
    hdr("L3-A5");
    { int z[5]={10,-3,7,0,6}; printf("sum=%d\n", sum_array(z,5)); }
    hdr("L3-A6");
    { char p[]="apple", q[]="apples";
      int cmp = compare_strings(p,q);
      printf("cmp=%d\n", cmp==0 ? 0 : (cmp<0 ? -1 : 1)); }
    hdr("L3-A7");
    { int arr[5]={3,9,2,8,5}; printf("max=%d\n", find_max(arr,5)); }

    return 0;
}
