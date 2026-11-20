/* =============================================================================
 *  paths.c  --  Learning path content.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  The Coding Exam 1 and 2 paths follow the Advanced Programming labs and
 *  the lecture notes in CExam Trainer. A new lesson is a new table row, not
 *  a code change.
 * ========================================================================== */
#include "paths.h"
#include "save.h"

#include <stddef.h>

#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

/* ---------------------------------------------------------------------------
 *  C Fundamentals
 * ------------------------------------------------------------------------- */
static const Lesson kCFundamentals[] = {
    { "Your first C program",               "main, printf and how a program is built",       5 },
    { "Variables and data types",           "int, float, char and choosing the right one",   6 },
    { "Operators and expressions",          "Arithmetic, integer division and precedence",   6 },
    { "Reading input with scanf",           "Format specifiers and the & in front",          5 },
    { "Making decisions with if / else",    "Conditions, comparisons and && / ||",           6 },
    { "switch and multi-way choices",       "Cases, break and the default branch",           5 },
    { "Loops: while and do-while",          "Repeat until a condition changes",              7 },
    { "Loops: for and nested loops",        "Counting loops and loops inside loops",         7 },
    { "Functions, parameters and returns",  "Splitting a program into reusable pieces",      8 },
    { "Scope and the stack",                "Where a variable lives and when it dies",       6 },
    { "Arrays: storing many values",        "Indexing from 0 and walking with a loop",       7 },
    { "The size / count pair",              "Capacity versus how many slots are in use",     6 },
    { "Searching and sorting arrays",       "Linear search, maximums and bubble sort",       8 },
    { "2D arrays and grids",                "Rows, columns and nested loop traversal",       8 },
    { "Pointers: what an address holds",    "&, * and why pointers exist",                   8 },
    { "Pointer arithmetic",                 "Moving through an array one element at a time", 7 },
    { "C strings and '\\0'",                "Char arrays and the terminator",                7 },
    { "Structs: grouping data",             "Defining a type and using the . and -> operators", 7 },
    { "Dynamic memory: malloc and free",    "Asking for memory at run time and giving it back", 8 },
    { "Files: reading and writing",         "fopen, fprintf, fscanf and fclose",             7 },
};

/* ---------------------------------------------------------------------------
 *  Coding Exam 1  -  Labs 1 and 2: core logic, 1D and 2D arrays
 * ------------------------------------------------------------------------- */
static const Lesson kExam1[] = {
    { "Functions and return values",        "Section A traces through small functions",      15 },
    { "Digit peeling: % 10 and / 10",       "Reversing numbers, palindromes, digit sums",    15 },
    { "Nested loop patterns",               "Predicting the shapes nested loops print",      15 },
    { "Searching, maximums and pairs",      "Linear search and pair-sum questions",          20 },
    { "Insert and delete with a count",     "Shifting in the safe direction",                20 },
    { "Reverse, rotate and duplicates",     "Classic 1D array transformations",              20 },
    { "Walking a 4x6 grid",                 "Row-major traversal and bounds",                20 },
    { "Borders without double counting",    "Edges, corners and neighbours",                 20 },
    { "Frequency arrays",                   "Using the value as the index",                  15 },
    { "Mock paper: Exam 1",                 "Section A and B on the clock",                  45 },
};

/* ---------------------------------------------------------------------------
 *  Coding Exam 2  -  Lab 3 onwards: pointers, strings, structs, memory
 * ------------------------------------------------------------------------- */
static const Lesson kExam2[] = {
    { "What a pointer actually holds",      "Addresses, & and * in Section A traces",        15 },
    { "Pointer arithmetic in elements",     "p + 1 moves one element, not one byte",         15 },
    { "Arrays and pointers together",       "Why a[i] and *(a + i) are the same",            15 },
    { "C strings end in '\\0'",             "Lengths, loops and the terminator",             15 },
    { "Write the terminator yourself",      "Copying and building strings safely",           20 },
    { "Comparing strings",                  "What strcmp really returns",                    15 },
    { "Counting through a string",          "Vowels, words and character classes",           20 },
    { "Structs and pointers to structs",    "The -> operator in exam snippets",              20 },
    { "Dynamic memory: malloc and free",    "Leaks, dangling pointers and NULL checks",      20 },
    { "Mock paper: Exam 2",                 "Section A and B on the clock",                  45 },
};

/* ---------------------------------------------------------------------------
 *  Advanced C++
 * ------------------------------------------------------------------------- */
static const Lesson kAdvancedCpp[] = {
    { "From C to C++",                      "What stays the same and what changes",          6 },
    { "References versus pointers",         "Aliases, const references and passing",         7 },
    { "Classes and objects",                "Bundling data with the functions that use it",  8 },
    { "Constructors and destructors",       "Setting up and cleaning up automatically",      8 },
    { "Encapsulation: public and private",  "Hiding the details behind an interface",        6 },
    { "new, delete and RAII",               "Owning memory without leaking it",              8 },
    { "Operator overloading",               "Making your types work with + and <<",          7 },
    { "Inheritance",                        "Building a class on top of another",            8 },
    { "Virtual functions and polymorphism", "One call, different behaviour per type",        9 },
    { "Templates and the STL",              "vector, string and generic code",               9 },
};

/* ---------------------------------------------------------------------------
 *  The paths
 * ------------------------------------------------------------------------- */
static const LearningPath kPaths[PATH_COUNT] = {
    [PATH_C_FUNDAMENTALS] = {
        "C", "C Fundamentals",
        "Variables, conditions, loops and functions",
        "Start from your first program and build up to pointers, structs and "
        "files. Every lesson is short and ends with something you can run.",
        "lessons", "IN PROGRESS",
        { "Write and build a C program", "Control flow with if and loops",
          "Arrays, strings and pointers", "Structs, memory and files" },
        kCFundamentals, COUNT(kCFundamentals)
    },
    [PATH_EXAM1] = {
        "E", "Coding Exam 1",
        "Problem-solving, arrays, strings and functions",
        "Exam practice built from Labs 1 and 2. Each set mixes Section A "
        "(what is the output?) and Section B (fill in the blanks).",
        "practice sets", "EXAM PRACTICE",
        { "Trace code like the marker does", "Loops, digits and patterns",
          "1D array insert, delete, rotate", "2D grids and frequency arrays" },
        kExam1, COUNT(kExam1)
    },
    [PATH_EXAM2] = {
        "2", "Coding Exam 2",
        "Pointers, strings, structs and memory",
        "Exam practice built from Lab 3 onwards. Pointers and strings are "
        "where marks are lost, so every set drills the boundary cases.",
        "practice sets", "EXAM PRACTICE",
        { "Read pointer code with confidence", "Handle C strings safely",
          "Use structs and ->", "Manage memory with malloc and free" },
        kExam2, COUNT(kExam2)
    },
    [PATH_ADVANCED_CPP] = {
        "+", "Advanced C++",
        "Classes, objects and memory management",
        "Take what you know from C into C++: classes, constructors, "
        "inheritance, polymorphism and the standard library.",
        "lessons", "UP NEXT",
        { "Design classes and objects", "Manage resources with RAII",
          "Use inheritance and virtuals", "Write generic code with templates" },
        kAdvancedCpp, COUNT(kAdvancedCpp)
    },
};

const LearningPath *PathGet(PathId id)
{
    if ((int)id < 0 || id >= PATH_COUNT) return NULL;
    return &kPaths[id];
}

int PathCompleted(PathId id)
{
    return SaveLessonsDone(id);
}

float PathProgress(PathId id)
{
    const LearningPath *p = PathGet(id);
    if (!p || p->lessonCount == 0) return 0.0f;
    return (float)PathCompleted(id) / (float)p->lessonCount;
}

int PathTotalMinutes(PathId id)
{
    const LearningPath *p = PathGet(id);
    int total = 0;
    if (!p) return 0;
    for (int i = 0; i < p->lessonCount; ++i) total += p->lessons[i].minutes;
    return total;
}

int PathNextLesson(PathId id)
{
    const LearningPath *p = PathGet(id);
    if (!p) return -1;
    for (int i = 0; i < p->lessonCount; ++i)
        if (!SaveLessonDone(id, i)) return i;
    return -1;
}
