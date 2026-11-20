/* =============================================================================
 *  lessons.c  --  Lesson notes, examples and quizzes.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Tables are in the same order as the lessons in paths.c. A lesson is one
 *  table entry: change the words here and the lesson screen follows.
 * ========================================================================== */
#include "lessons.h"
#include "content.h"
#include "save.h"

#include <stddef.h>

/* Shorthand for an own-notes lesson with no exam questions. */
#define OWN  -1, -1
#define NOQ  { NULL }, -1

/* ===========================================================================
 *  C Fundamentals
 * ========================================================================= */
static const LessonContent kCFund[] = {
/* 1 */ {
    "Every C program starts running at main. The #include line pulls in stdio.h, "
    "which declares printf, the function that prints text to the console.\n\n"
    "C is compiled: gcc turns your .c file into an .exe, and only then can it run. "
    "If the compiler finds an error, no program is produced.",
    "#include <stdio.h>\n\nint main(void)\n{\n    printf(\"Hello, CodeQuest!\\n\");\n    return 0;\n}",
    "main is where execution begins; return 0 means success.", OWN,
    { { "Where does a C program start running?",
        { "At the first line of the file", "At main", "At #include", "At the last function" }, 1,
        "Execution always begins at main, wherever it sits in the file." },
      { "What does \\n do inside printf?",
        { "Prints a backslash and n", "Ends the program", "Starts a new line", "Adds a tab" }, 2,
        "\\n is the newline escape: the next output starts on a new line." } }, 2, NOQ },
/* 2 */ {
    "A variable is a named box for a value. Its type decides what it can hold: "
    "int for whole numbers, float or double for decimals, char for one character.\n\n"
    "Always give a variable a value before you read it. An uninitialised local "
    "variable holds whatever happened to be in memory.",
    "int age = 19;\ndouble price = 4.99;\nchar grade = 'A';\n\nprintf(\"%d %.2f %c\\n\", age, price, grade);",
    "Pick the type from the kind of value, and initialise before use.", OWN,
    { { "Which type fits the value 3.75 best?",
        { "int", "char", "double", "bool" }, 2,
        "3.75 has a fractional part, so it needs a floating-point type like double." },
      { "What does an uninitialised local int contain?",
        { "0", "Whatever was in that memory", "-1", "It will not compile" }, 1,
        "Locals are not zeroed in C; reading one before assigning it is a bug." } }, 2, NOQ },
/* 3 */ {
    "Arithmetic works as you expect, with one trap: dividing two ints throws away "
    "the remainder. 7 / 2 is 3, and 7 % 2 gives that remainder, 1.\n\n"
    "Precedence follows maths: * / % before + -. Use brackets whenever you are "
    "not sure; they cost nothing.",
    "int a = 7, b = 2;\nprintf(\"%d\\n\", a / b);      // 3\nprintf(\"%d\\n\", a % b);      // 1\nprintf(\"%.1f\\n\", a / 2.0);  // 3.5",
    "int / int is integer division; make one side a double to keep decimals.", OWN,
    { { "What is 17 / 5 when both are ints?",
        { "3.4", "3", "4", "2" }, 1,
        "Integer division drops the fraction: 17 / 5 is 3." },
      { "What is 17 % 5?",
        { "3", "2", "0", "5" }, 1,
        "% gives the remainder: 17 = 3 * 5 + 2." } }, 2, NOQ },
/* 4 */ {
    "scanf reads input using the same format codes as printf. It needs the address "
    "of the variable to fill, which is why you write &x.\n\n"
    "scanf returns how many values it read. Checking that number is the simplest "
    "way to catch bad input.",
    "int n;\nprintf(\"Enter a number: \");\nif (scanf(\"%d\", &n) == 1)\n    printf(\"You typed %d\\n\", n);",
    "Pass &variable to scanf, and check its return value.", OWN,
    { { "Why does scanf(\"%d\", &n) need the &?",
        { "It is optional style", "scanf needs the address to store into", "It converts to int", "It clears the buffer" }, 1,
        "scanf writes into your variable, so it needs where that variable lives." },
      { "scanf(\"%d\", &n) returns 0. What happened?",
        { "The user typed 0", "Nothing could be read as an int", "It worked", "End of program" }, 1,
        "The return value counts successful conversions; 0 means none." } }, 2, NOQ },
/* 5 */ {
    "if runs a block only when its condition is true; else covers every other case. "
    "Conditions use == != < > <= >= and combine with && (and) and || (or).\n\n"
    "A classic slip is writing = (assignment) where you meant == (comparison).",
    "if (mark >= 70)\n    printf(\"First\\n\");\nelse if (mark >= 40)\n    printf(\"Pass\\n\");\nelse\n    printf(\"Fail\\n\");",
    "== compares, = assigns. Order else-if checks from most to least specific.", OWN,
    { { "What prints when mark is 55?",
        { "First", "Pass", "Fail", "Nothing" }, 1,
        "55 is not >= 70, but it is >= 40, so the else-if branch runs." },
      { "Which is true when x is 5?",
        { "x > 5 && x < 10", "x == 5 || x == 6", "x != 5", "x < 0" }, 1,
        "x == 5 is true, and || only needs one side true." } }, 2, NOQ },
/* 6 */ {
    "switch picks a branch by comparing one integer value against case labels. "
    "Each case usually ends with break; without it, execution falls through into "
    "the next case.\n\n"
    "default runs when no case matches, like a final else.",
    "switch (choice) {\ncase 1:  printf(\"Start\\n\"); break;\ncase 2:  printf(\"Help\\n\");  break;\ndefault: printf(\"Unknown\\n\");\n}",
    "Forgetting break makes cases fall through.", OWN,
    { { "What happens if a case has no break?",
        { "Compile error", "The switch ends", "It falls through to the next case", "default runs" }, 2,
        "Without break, the following case's statements run too." },
      { "Which can a switch case label be?",
        { "A string", "An integer constant", "A float", "A variable" }, 1,
        "case labels must be integer constants (chars count, they are small ints)." } }, 2, NOQ },
/* 7 */ {
    "A while loop checks its condition first and repeats while it stays true. "
    "do-while runs the body once before checking, so it always runs at least once.\n\n"
    "Something in the body must change the condition, or the loop never ends.",
    "int n = 3;\nwhile (n > 0) {\n    printf(\"%d \", n);\n    n--;\n}\n// prints: 3 2 1",
    "while may run zero times; do-while runs at least once.", OWN,
    { { "How many times does while (0) { ... } run its body?",
        { "Once", "Never", "Forever", "Twice" }, 1,
        "The condition is false at the first check, so the body never runs." },
      { "Which loop always runs its body at least once?",
        { "while", "for", "do-while", "none" }, 2,
        "do-while checks the condition after the body." } }, 2, NOQ },
/* 8 */ {
    "for packs the three parts of a counting loop into one line: start, condition, "
    "step. Put a loop inside another and the inner one runs completely for every "
    "pass of the outer one.\n\n"
    "Nested loops are how you print patterns and walk 2D grids.",
    "for (int row = 1; row <= 3; row++) {\n    for (int col = 0; col < row; col++)\n        printf(\"*\");\n    printf(\"\\n\");\n}",
    "Inner loop = columns, outer loop = rows.", OWN,
    { { "How many stars does the example print in total?",
        { "3", "6", "9", "4" }, 1,
        "Rows print 1, 2 and 3 stars: 1 + 2 + 3 = 6." },
      { "for (i = 0; i < 5; i++) runs how many times?",
        { "4", "5", "6", "Forever" }, 1,
        "i takes the values 0, 1, 2, 3 and 4: five passes." } }, 2, NOQ },
/* 9 */ {
    "A function packages a job under a name. Parameters carry values in; return "
    "sends one value back. C passes arguments by value: the function gets copies.\n\n"
    "That is why a function cannot change the caller's int unless you pass its "
    "address (you will see that with pointers).",
    "int square(int x)\n{\n    return x * x;\n}\n\nint main(void)\n{\n    printf(\"%d\\n\", square(4));  // 16\n    return 0;\n}",
    "Arguments are copied; return gives one value back.", OWN,
    { { "void f(int x) { x = 10; } leaves the caller's variable as...",
        { "10", "Unchanged", "0", "Undefined" }, 1,
        "f changed its own copy of x, not the caller's variable." },
      { "What does a void function return?",
        { "0", "Nothing", "An int", "NULL" }, 1,
        "void means the function gives no value back." } }, 2, NOQ },
/* 10 */ {
    "A variable declared inside a block only exists inside it: that is its scope. "
    "Local variables live on the stack and vanish when the function returns.\n\n"
    "static locals keep their value between calls, and globals live for the whole "
    "program, but use both sparingly.",
    "void counter(void)\n{\n    static int calls = 0;\n    calls++;\n    printf(\"called %d times\\n\", calls);\n}",
    "Locals die at the closing brace; static locals survive.", OWN,
    { { "When is a normal local variable destroyed?",
        { "When the program ends", "When its block ends", "Never", "When it is printed" }, 1,
        "Automatic locals live until the end of the block that declares them." },
      { "Calling counter() three times prints which last?",
        { "called 1 times", "called 3 times", "called 0 times", "called 2 times" }, 1,
        "The static int keeps counting across calls: 1, 2, 3." } }, 2, NOQ },
/* 11 */ {
    "An array stores many values of one type side by side. Indexes start at 0, so "
    "an array of 5 has valid indexes 0 to 4.\n\n"
    "C does not check bounds. Reading a[5] in a 5-element array is undefined "
    "behaviour, not an error message.",
    "int marks[5] = { 61, 74, 58, 90, 47 };\nint total = 0;\nfor (int i = 0; i < 5; i++)\n    total += marks[i];\nprintf(\"%d\\n\", total);",
    "Valid indexes are 0 .. size - 1, and nobody checks for you.", OWN,
    { { "What is the last valid index of int a[8]?",
        { "8", "7", "9", "0" }, 1,
        "Indexes run from 0 to size - 1." },
      { "What does C do if you read a[10] in a 5-element array?",
        { "Throws an error", "Returns 0", "Undefined behaviour", "Resizes the array" }, 2,
        "There is no bounds check; anything can happen." } }, 2, NOQ },
/* 12 */ {
    "Exam arrays often have a fixed capacity (size) and a count of slots actually "
    "in use. Inserting increases count; deleting decreases it; the capacity never "
    "changes.\n\n"
    "Loops that process the data must stop at count, not size.",
    "int data[10] = { 4, 8, 15 };\nint count = 3;\n\ndata[count] = 16;   // append\ncount++;\n\nfor (int i = 0; i < count; i++)\n    printf(\"%d \", data[i]);",
    "Loop to count, check count < size before inserting.", OWN,
    { { "Before appending, what must be true?",
        { "count > 0", "count < size", "count == size", "size < count" }, 1,
        "There must be a free slot left." },
      { "After deleting one element, count becomes...",
        { "count + 1", "count - 1", "size - 1", "0" }, 1,
        "One fewer slot is in use." } }, 2, NOQ },
/* 13 */ {
    "Linear search checks each element in turn and stops when it finds the target. "
    "Finding a maximum means keeping the best value seen so far.\n\n"
    "Bubble sort repeatedly swaps neighbours that are in the wrong order; after "
    "each pass the largest remaining value has bubbled to the end.",
    "int best = a[0];\nfor (int i = 1; i < n; i++)\n    if (a[i] > best)\n        best = a[i];",
    "Start the maximum at a[0], not 0, so negative data still works.", OWN,
    { { "Why start best at a[0] instead of 0?",
        { "It is faster", "All values might be negative", "0 is not an int", "Style only" }, 1,
        "If every value is negative, starting at 0 would give the wrong answer." },
      { "After one bubble sort pass, what is guaranteed?",
        { "The array is sorted", "The largest value is last", "The smallest is first", "Nothing" }, 1,
        "Each pass carries the largest unsorted value to the end." } }, 2, NOQ },
/* 14 */ {
    "A 2D array is an array of rows. grid[r][c] picks row r, then column c. "
    "Walk it with nested loops: rows outside, columns inside.\n\n"
    "Neighbour checks (up, down, left, right) must stay inside the grid, so test "
    "the bounds before reading.",
    "int grid[4][6] = { 0 };\nfor (int r = 0; r < 4; r++) {\n    for (int c = 0; c < 6; c++)\n        printf(\"%d \", grid[r][c]);\n    printf(\"\\n\");\n}",
    "grid[row][col]; check bounds before looking at neighbours.", OWN,
    { { "In int g[4][6], how many columns are there?",
        { "4", "6", "24", "10" }, 1,
        "The second size is the number of columns in each row." },
      { "Which index is out of bounds for g[4][6]?",
        { "g[3][5]", "g[0][0]", "g[4][0]", "g[2][3]" }, 2,
        "Rows run 0 to 3, so row 4 does not exist." } }, 2, NOQ },
/* 15 */ {
    "A pointer holds a memory address. &x gives the address of x, and *p follows "
    "the pointer to the value it points at.\n\n"
    "Passing a pointer lets a function change the caller's variable, which is how "
    "swap works.",
    "void swap(int *a, int *b)\n{\n    int t = *a;\n    *a = *b;\n    *b = t;\n}\n\nswap(&x, &y);",
    "& takes an address, * follows one.", OWN,
    { { "If p = &x, what is *p?",
        { "The address of x", "The value of x", "The address of p", "Always 0" }, 1,
        "*p follows the pointer to x's value." },
      { "Why does swap take int * parameters?",
        { "Speed", "So it can change the caller's variables", "ints cannot be copied", "Style" }, 1,
        "With addresses, swap writes into the original variables." } }, 2, NOQ },
/* 16 */ {
    "Adding 1 to a pointer moves it one element forward, not one byte: for an "
    "int * that is sizeof(int) bytes. An array name used in an expression becomes "
    "a pointer to its first element.\n\n"
    "So a[i] and *(a + i) mean exactly the same thing.",
    "int a[4] = { 10, 20, 30, 40 };\nint *p = a;\nprintf(\"%d\\n\", *(p + 2));   // 30\np++;\nprintf(\"%d\\n\", *p);         // 20",
    "p + 1 moves one element.", OWN,
    { { "With p = a (int a[] = {5,6,7}), what is *(p + 1)?",
        { "5", "6", "7", "An address" }, 1,
        "p + 1 points at the second element." },
      { "a[i] is the same as...",
        { "*a + i", "*(a + i)", "&a + i", "a * i" }, 1,
        "Indexing is defined as pointer arithmetic then dereference." } }, 2, NOQ },
/* 17 */ {
    "A C string is a char array that ends with the null character '\\0'. Every "
    "string function walks until it reaches that terminator.\n\n"
    "So a string of 5 letters needs an array of at least 6 chars.",
    "char name[6] = \"Szymo\";   // S z y m o \\0\nint len = 0;\nwhile (name[len] != '\\0')\n    len++;\nprintf(\"%d\\n\", len);   // 5",
    "Always leave room for the '\\0'.", OWN,
    { { "How many chars does \"cat\" occupy in memory?",
        { "3", "4", "2", "5" }, 1,
        "Three letters plus the terminating '\\0'." },
      { "What marks the end of a C string?",
        { "A space", "'\\n'", "'\\0'", "The array size" }, 2,
        "The null character '\\0' ends every C string." } }, 2, NOQ },
/* 18 */ {
    "A struct groups related values under one type, like a record. Access members "
    "with a dot on a variable, or with -> through a pointer.\n\n"
    "p->mark is just shorthand for (*p).mark.",
    "typedef struct Student {\n    char name[32];\n    int  mark;\n} Student;\n\nStudent s = { \"Ana\", 72 };\nStudent *p = &s;\np->mark += 5;\nprintf(\"%s %d\\n\", s.name, s.mark);",
    "Dot on a struct, arrow on a pointer to a struct.", OWN,
    { { "p is a Student *. Which reads the mark?",
        { "p.mark", "p->mark", "*p.mark", "&p.mark" }, 1,
        "-> follows the pointer then picks the member." },
      { "What does the example print?",
        { "Ana 72", "Ana 77", "Ana 5", "Nothing" }, 1,
        "p points at s, so adding 5 through p changes s.mark to 77." } }, 2, NOQ },
/* 19 */ {
    "malloc asks for memory at run time and returns a pointer to it, or NULL if "
    "it fails. Memory you malloc stays until you free it.\n\n"
    "Forgetting free leaks memory; using memory after free is a dangling pointer bug.",
    "int n = 5;\nint *a = malloc(n * sizeof *a);\nif (a == NULL) return 1;\nfor (int i = 0; i < n; i++)\n    a[i] = i * i;\nfree(a);\na = NULL;",
    "Check for NULL, free exactly once, then forget the pointer.", OWN,
    { { "What does malloc return when it fails?",
        { "0 bytes", "NULL", "-1", "It exits the program" }, 1,
        "A failed allocation returns NULL, so always check." },
      { "What is a memory leak?",
        { "Freeing twice", "Memory never freed", "Reading past an array", "A NULL pointer" }, 1,
        "Allocated memory that is never freed is leaked." } }, 2, NOQ },
/* 20 */ {
    "fopen opens a file and gives you a FILE *. Read with fscanf or fgets, write "
    "with fprintf, and always fclose when done.\n\n"
    "fopen returns NULL if the file cannot be opened, for example if it does not exist.",
    "FILE *f = fopen(\"scores.txt\", \"w\");\nif (f == NULL) return 1;\nfprintf(f, \"%d\\n\", 42);\nfclose(f);",
    "\"r\" reads, \"w\" overwrites, \"a\" appends. Check for NULL.", OWN,
    { { "Which mode adds to the end of an existing file?",
        { "\"r\"", "\"w\"", "\"a\"", "\"x\"" }, 2,
        "\"a\" opens for appending; \"w\" would wipe the file." },
      { "What should you do after fopen?",
        { "Nothing", "Check the result is not NULL", "Call free", "Call malloc" }, 1,
        "If fopen failed, using the NULL FILE * crashes." } }, 2, NOQ },
};

/* ===========================================================================
 *  Coding Exam 1 - notes and questions from CExam Trainer, Labs 1 and 2
 *  Note sections: 0 = Lab 1, 1 = Lab 2, 2 = Lab 3, 3 = exam technique
 * ========================================================================= */
#define EXAM_SET(sec, idx, ...) { NULL, NULL, NULL, sec, idx, { { 0 } }, 0, { __VA_ARGS__, NULL }, -1 }

static const LessonContent kExam1[] = {
    EXAM_SET(0, 0, "L1-A1", "L1-A3", "L1-B1", "L1-B3"),
    EXAM_SET(0, 3, "L1-A2", "L1-A4", "L1-A5", "L1-B2", "L1-B4"),
    EXAM_SET(0, 4, "L1-A6", "L1-A7", "L1-A18", "L1-B12"),
    EXAM_SET(0, 5, "L1-A8", "L1-A9", "L1-A13", "L1-A17", "L1-B11"),
    EXAM_SET(0, 1, "L1-A10", "L1-A11", "L1-A12", "L1-B6", "L1-B7"),
    EXAM_SET(0, 2, "L1-A14", "L1-A15", "L1-A16", "L1-B8", "L1-B9", "L1-B10"),
    EXAM_SET(1, 0, "L2-A1", "L2-A2", "L2-A3", "L2-A5"),
    EXAM_SET(1, 2, "L2-A4", "L2-A9", "L2-B4", "L2-B5"),
    EXAM_SET(1, 4, "L2-A6", "L2-A7", "L2-A8", "L2-B1", "L2-B2", "L2-B3"),
    { NULL, NULL, NULL, 3, 1, { { 0 } }, 0, { NULL }, MOCK_EXAM1 },
};

/* ===========================================================================
 *  Coding Exam 2 - Lab 3 notes and questions, plus structs and memory
 * ========================================================================= */
static const LessonContent kExam2[] = {
    EXAM_SET(2, 0, "L3-A1", "L3-B7"),
    EXAM_SET(2, 1, "L3-A5", "L3-A7", "L3-B6"),
    EXAM_SET(2, 1, "L3-A2", "L3-B1", "L3-B2"),
    EXAM_SET(2, 2, "L3-A3", "L3-B3"),
    EXAM_SET(2, 3, "L3-B4", "L3-B5"),
    EXAM_SET(2, 4, "L3-A6"),
    EXAM_SET(2, 5, "L3-A4"),
    {
    "Exam snippets often pass a struct by pointer so the function can change it. "
    "Inside the function, every member access goes through ->.\n\n"
    "An array of structs is walked like any array; s[i].mark and (s + i)->mark "
    "are the same thing.",
    "typedef struct Item { int id; int qty; } Item;\n\nvoid restock(Item *it, int n)\n{\n    it->qty += n;\n}\n\nItem stock[3] = { {1, 2}, {2, 0}, {3, 5} };\nrestock(&stock[1], 4);\nprintf(\"%d\\n\", stock[1].qty);",
    "Through a pointer, always ->.", OWN,
    { { "What does the example print?",
        { "0", "4", "2", "6" }, 1,
        "stock[1].qty starts at 0 and restock adds 4." },
      { "(s + 2)->id is the same as...",
        { "s->id + 2", "s[2].id", "s.id[2]", "*s.id" }, 1,
        "s + 2 points at element 2; -> then picks its id." },
      { "Why pass Item * rather than Item?",
        { "Structs cannot be copied", "So the function changes the original", "It is required", "To save typing" }, 1,
        "Passing by value would only change a copy." } }, 3, NOQ },
    {
    "Section B often asks you to finish a function that mallocs an array. The "
    "marks are in the details: the size passed to malloc, the NULL check, and the "
    "free at the end.\n\n"
    "Free each block exactly once, and set the pointer to NULL afterwards so a "
    "second free does nothing.",
    "int *copy_array(const int *src, int n)\n{\n    int *dst = malloc(n * sizeof *dst);\n    if (dst == NULL) return NULL;\n    for (int i = 0; i < n; i++)\n        dst[i] = src[i];\n    return dst;   // caller must free\n}",
    "malloc(n * sizeof *p), check NULL, free once.", OWN,
    { { "What size should malloc get for 10 ints?",
        { "10", "10 * sizeof(int)", "sizeof(int)", "40 always" }, 1,
        "malloc counts bytes, so multiply by the element size." },
      { "Who frees the array copy_array returns?",
        { "copy_array", "The caller", "Nobody", "The compiler" }, 1,
        "Ownership passes to the caller, who must free it." },
      { "What happens if you free the same pointer twice?",
        { "Nothing", "Undefined behaviour", "It frees more memory", "Compile error" }, 1,
        "A double free is undefined; set the pointer to NULL after freeing." } }, 3, NOQ },
    { NULL, NULL, NULL, 3, 3, { { 0 } }, 0, { NULL }, MOCK_EXAM2 },
};

/* ===========================================================================
 *  Advanced C++
 * ========================================================================= */
static const LessonContent kCpp[] = {
/* 1 */ {
    "C++ keeps almost all of C, then adds classes, references, overloading, "
    "templates and a large standard library. Input and output use streams: "
    "std::cout << to print and std::cin >> to read.\n\n"
    "You compile with g++ instead of gcc, and files end in .cpp.",
    "#include <iostream>\n\nint main()\n{\n    int n = 3;\n    std::cout << \"n is \" << n << '\\n';\n    return 0;\n}",
    "Same core language, plus classes, references and the STL.", OWN,
    { { "Which prints to the console in C++?",
        { "std::cin >>", "std::cout <<", "print()", "echo" }, 1,
        "std::cout with << writes to standard output." },
      { "Which compiler driver builds C++?",
        { "gcc", "g++", "javac", "make" }, 1,
        "g++ compiles C++ and links the C++ standard library." } }, 2, NOQ },
/* 2 */ {
    "A reference is another name for an existing variable. Unlike a pointer it "
    "cannot be NULL and cannot be reseated to refer to something else.\n\n"
    "Pass by reference to change the caller's variable, or by const reference to "
    "avoid copying something large.",
    "void addBonus(int &mark)\n{\n    mark += 5;\n}\n\nint m = 60;\naddBonus(m);   // m is now 65",
    "int & changes the original; const T & reads without copying.", OWN,
    { { "After addBonus(m) with m = 60, m is...",
        { "60", "65", "5", "Undefined" }, 1,
        "mark is a reference to m, so m itself changes." },
      { "Which is true of references?",
        { "They can be NULL", "They must be bound when created", "They can be reseated", "They need *" }, 1,
        "A reference must refer to something from the start." } }, 2, NOQ },
/* 3 */ {
    "A class bundles data (members) with the functions that work on it (methods). "
    "An object is one instance of a class.\n\n"
    "Methods are called with a dot, just like reading a struct member.",
    "class Counter {\npublic:\n    int value = 0;\n    void tick() { value++; }\n};\n\nCounter c;\nc.tick();\nc.tick();\nstd::cout << c.value;   // 2",
    "Class = blueprint, object = one built from it.", OWN,
    { { "What does the example print?",
        { "0", "1", "2", "Nothing" }, 2,
        "tick runs twice on the same object." },
      { "An object is...",
        { "A kind of loop", "An instance of a class", "A header file", "A pointer" }, 1,
        "Each object is one instance built from the class." } }, 2, NOQ },
/* 4 */ {
    "A constructor runs automatically when an object is created and sets it up. "
    "A destructor (~ClassName) runs when the object goes away and cleans up.\n\n"
    "Prefer a member initialiser list to assigning inside the constructor body.",
    "class Timer {\npublic:\n    Timer(int s) : seconds(s) { }\n    ~Timer() { std::cout << \"done\\n\"; }\nprivate:\n    int seconds;\n};",
    "Constructors set up, destructors clean up, both automatically.", OWN,
    { { "When does a local object's destructor run?",
        { "Never", "When it goes out of scope", "When main starts", "When you call it" }, 1,
        "Leaving the enclosing block destroys the object." },
      { "How is a destructor named?",
        { "destroy()", "~ClassName()", "delete ClassName", "ClassName_free()" }, 1,
        "A tilde followed by the class name." } }, 2, NOQ },
/* 5 */ {
    "public members are the class's interface; private members are hidden details "
    "only its own methods can touch. Hiding data lets the class keep it valid.\n\n"
    "A getter reads a private value; a setter can check the new value first.",
    "class Account {\npublic:\n    void deposit(int amount) { if (amount > 0) balance += amount; }\n    int  getBalance() const { return balance; }\nprivate:\n    int balance = 0;\n};",
    "Keep data private and expose safe methods.", OWN,
    { { "Who can read a private member directly?",
        { "Anyone", "Only the class's own methods", "Only main", "Only derived classes" }, 1,
        "private restricts access to the class itself." },
      { "Why does deposit check amount > 0?",
        { "Speed", "To keep balance valid", "It is required by C++", "No reason" }, 1,
        "Encapsulation lets the class reject bad changes." } }, 2, NOQ },
/* 6 */ {
    "new creates an object on the heap and delete destroys it, like malloc and "
    "free but with constructors and destructors. Forgetting delete leaks.\n\n"
    "RAII ties a resource to an object's lifetime: the destructor releases it. "
    "std::unique_ptr does this for you.",
    "#include <memory>\n\nauto p = std::make_unique<Timer>(30);\n// no delete needed: freed when p goes out of scope",
    "Prefer smart pointers to raw new and delete.", OWN,
    { { "What pairs with new?",
        { "free", "delete", "malloc", "nothing" }, 1,
        "Memory from new must be released with delete." },
      { "What does RAII stand for in practice?",
        { "Random access", "Cleanup in the destructor", "Run all instructions", "A container" }, 1,
        "Resource Acquisition Is Initialisation: owning objects free what they hold." } }, 2, NOQ },
/* 7 */ {
    "Operator overloading lets your own types use operators such as + == and <<. "
    "Overload only when the meaning is obvious.\n\n"
    "Overloading << with std::ostream lets you print objects with std::cout.",
    "struct Vec2 { int x, y; };\n\nVec2 operator+(Vec2 a, Vec2 b)\n{\n    return { a.x + b.x, a.y + b.y };\n}\n\nVec2 c = Vec2{1, 2} + Vec2{3, 4};   // {4, 6}",
    "Overloads should do what the operator suggests.", OWN,
    { { "What is c in the example?",
        { "{1, 2}", "{4, 6}", "{3, 8}", "{4, 2}" }, 1,
        "x: 1 + 3 = 4, y: 2 + 4 = 6." },
      { "Which operator is overloaded to print with std::cout?",
        { ">>", "<<", "+", "==" }, 1,
        "operator<< taking an std::ostream &." } }, 2, NOQ },
/* 8 */ {
    "Inheritance builds a class on top of another: the derived class gets the base "
    "class's members and adds its own. Use it for genuine is-a relationships.\n\n"
    "protected members are visible to derived classes but not outside.",
    "class Shape {\npublic:\n    int x = 0, y = 0;\n};\n\nclass Circle : public Shape {\npublic:\n    int radius = 1;\n};\n\nCircle c;\nc.x = 5;   // inherited from Shape",
    "Derived classes reuse and extend a base class.", OWN,
    { { "Circle : public Shape means...",
        { "Shape is a Circle", "Circle is a Shape", "They are unrelated", "Circle contains a Shape pointer" }, 1,
        "The derived class is a kind of the base class." },
      { "Who can see protected members?",
        { "Everyone", "The class and its derived classes", "Only main", "Nobody" }, 1,
        "protected sits between public and private." } }, 2, NOQ },
/* 9 */ {
    "A virtual function can be overridden by derived classes. Calling it through a "
    "base pointer or reference runs the version for the object's real type: that "
    "is polymorphism.\n\n"
    "A base class meant for this needs a virtual destructor.",
    "struct Animal {\n    virtual ~Animal() = default;\n    virtual void speak() const { std::cout << \"...\\n\"; }\n};\nstruct Dog : Animal {\n    void speak() const override { std::cout << \"Woof\\n\"; }\n};\n\nAnimal *a = new Dog;\na->speak();   // Woof\ndelete a;",
    "virtual + override = the real type's version runs.", OWN,
    { { "What does a->speak() print in the example?",
        { "...", "Woof", "Nothing", "Compile error" }, 1,
        "speak is virtual, so the Dog version runs." },
      { "What does override do?",
        { "Hides the base function", "Asks the compiler to check it really overrides", "Makes it static", "Nothing" }, 1,
        "override catches signature mistakes at compile time." } }, 2, NOQ },
/* 10 */ {
    "Templates write code once for any type; the compiler generates a version per "
    "type you use. The STL is built on them: std::vector, std::string, std::map "
    "and algorithms like std::sort.\n\n"
    "std::vector grows by itself, so you rarely need new[] for arrays.",
    "#include <vector>\n#include <algorithm>\n\nstd::vector<int> v = { 5, 2, 9 };\nv.push_back(1);\nstd::sort(v.begin(), v.end());\nfor (int x : v) std::cout << x << ' ';   // 1 2 5 9",
    "Reach for std::vector and std::string first.", OWN,
    { { "What does the example print?",
        { "5 2 9 1", "1 2 5 9", "9 5 2 1", "5 2 9" }, 1,
        "push_back adds 1, then sort orders them ascending." },
      { "What does a template let you write?",
        { "Faster loops", "Code that works for many types", "Assembly", "Header guards" }, 1,
        "The compiler fills in the type for each use." } }, 2, NOQ },
};

/* ===========================================================================
 *  Lookup
 * ========================================================================= */
#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

const LessonContent *LessonContentGet(PathId path, int lesson)
{
    const LessonContent *table = NULL;
    int n = 0;
    switch (path) {
    case PATH_C_FUNDAMENTALS: table = kCFund; n = COUNT(kCFund); break;
    case PATH_EXAM1:          table = kExam1; n = COUNT(kExam1); break;
    case PATH_EXAM2:          table = kExam2; n = COUNT(kExam2); break;
    case PATH_ADVANCED_CPP:   table = kCpp;   n = COUNT(kCpp);   break;
    default: return NULL;
    }
    return (lesson >= 0 && lesson < n) ? &table[lesson] : NULL;
}

static const Note *SourceNote(const LessonContent *c)
{
    if (!c || c->notes || c->noteSection < 0) return NULL;
    int count = 0;
    const Note *notes = NotesFor(c->noteSection, &count);
    return (notes && c->noteIndex >= 0 && c->noteIndex < count) ? &notes[c->noteIndex] : NULL;
}

const char *LessonNotes(PathId path, int lesson)
{
    const LessonContent *c = LessonContentGet(path, lesson);
    const Note *n = SourceNote(c);
    if (n) return n->body;
    return c ? c->notes : "";
}

const char *LessonCode(PathId path, int lesson)
{
    const LessonContent *c = LessonContentGet(path, lesson);
    const Note *n = SourceNote(c);
    if (n) return n->code;
    return c ? c->code : NULL;
}

const char *LessonHeading(PathId path, int lesson)
{
    const Note *n = SourceNote(LessonContentGet(path, lesson));
    return n ? n->heading : NULL;
}

int LessonQuestionCount(PathId path, int lesson)
{
    const LessonContent *c = LessonContentGet(path, lesson);
    int n = 0;
    if (!c) return 0;
    while (n < LESSON_QIDS && c->questionIds[n]) n++;
    return n;
}
