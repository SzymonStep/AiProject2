/* =============================================================================
 *  content.c  --  Question bank and lecture notes.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  Every Section A answer in this file was produced by tools/verify.c, which
 *  implements each lab function to the lab-sheet specification and prints the
 *  real output. Two answers therefore differ from the circulated answer key;
 *  both are called out in the question's explanation so the difference is
 *  visible rather than silent.
 * ========================================================================== */
#include "content.h"

#include <stddef.h>

/* ---------------------------------------------------------------------------
 *  Topic labels
 * ------------------------------------------------------------------------- */
const char *TopicName(Topic t)
{
    switch (t) {
        case TOPIC_LAB1: return "Lab 1 - Core Logic & 1D Arrays";
        case TOPIC_LAB2: return "Lab 2 - 2D Arrays & Grid Mechanics";
        case TOPIC_LAB3: return "Lab 3 - Pointers & C Strings";
        default:         return "Unknown";
    }
}

const char *TopicShort(Topic t)
{
    switch (t) {
        case TOPIC_LAB1: return "Lab 1";
        case TOPIC_LAB2: return "Lab 2";
        case TOPIC_LAB3: return "Lab 3";
        default:         return "?";
    }
}

const char *TopicBlurb(Topic t)
{
    switch (t) {
        case TOPIC_LAB1:
            return "Leap years, palindromes, primes, binary conversion, "
                   "nested-loop patterns, searching, insert/delete with a "
                   "count, duplicates, reverse and rotate.";
        case TOPIC_LAB2:
            return "The 4x6 level grid: replacing the largest value, counting "
                   "zeros, row patterns, borders, column sums, shifting, "
                   "frequency and gravity.";
        case TOPIC_LAB3:
            return "Pointer arithmetic, swapping through pointers, walking a "
                   "null-terminated char array, and rewriting strlen, strcpy, "
                   "strcat and strcmp by hand.";
        default:
            return "";
    }
}

/* =============================================================================
 *  THE QUESTION BANK
 * ========================================================================== */
const Question QUESTIONS[] = {

/* ============================ LAB 1 - SECTION A ========================== */
{
    .id = "L1-A1", .title = "Leap year",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int isLeapYear(int y) - returns 1 if y is a leap year, else 0.\n"
            "Divisible by 4, except centuries, unless also divisible by 400.",
    .code =
        "printf(\"%d\\n\", isLeapYear(2000));\n"
        "printf(\"%d\\n\", isLeapYear(1900));\n"
        "printf(\"%d\\n\", isLeapYear(2024));\n"
        "printf(\"%d\\n\", isLeapYear(2025));",
    .answer = "1\n0\n1\n0",
    .explain =
        "2000 is divisible by 400, so the century exception is itself excepted "
        "-> leap. 1900 is divisible by 100 but not 400 -> not a leap year, and "
        "this is the case the exam always tests. 2024 is divisible by 4 and is "
        "not a century -> leap. 2025 is not divisible by 4 at all."
},
{
    .id = "L1-A2", .title = "Palindrome number",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int isPalindromeNumber(int n) - rebuilds n reversed digit by "
            "digit and returns 1 when the reversal equals the original.",
    .code =
        "printf(\"%d\\n\", isPalindromeNumber(12321));\n"
        "printf(\"%d\\n\", isPalindromeNumber(12021));",
    .answer = "1\n1",
    .explain =
        "Read 12021 backwards: 1-2-0-0... no: 1,2,0,2,1. It is symmetric, so it "
        "IS a palindrome and the function returns 1 for both calls.\n\n"
        "Watch out: the circulated answer key prints 0 for the second call. "
        "That is a mistake in the key - reversing 12021 gives 12021. Trace the "
        "digits yourself rather than trusting that a 'messy looking' number is "
        "not a palindrome."
},
{
    .id = "L1-A3", .title = "Prime check",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int isPrime(int n) - returns 1 when n is prime. Trial division "
            "while d*d <= n, which avoids needing sqrt().",
    .code =
        "printf(\"%d\\n\", isPrime(29));\n"
        "printf(\"%d\\n\", isPrime(1));\n"
        "printf(\"%d\\n\", isPrime(21));",
    .answer = "1\n0\n0",
    .explain =
        "29 has no divisor up to 5 (5*5=25 <= 29, 6*6=36 > 29) -> prime. 1 is "
        "rejected by the n < 2 guard: 1 is not prime by definition. 21 = 3 * 7, "
        "so the loop finds d = 3 and returns 0."
},
{
    .id = "L1-A4", .title = "Binary to decimal",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int binaryToDecimal(int b) - b holds the binary digits as a "
            "decimal integer (101101 means the bit pattern 101101). No shift "
            "operators allowed, so peel digits with % 10 and / 10.",
    .code =
        "printf(\"%d\\n\", binaryToDecimal(101101));\n"
        "printf(\"%d\\n\", binaryToDecimal(100000));",
    .answer = "45\n32",
    .explain =
        "Digits are taken from the right, each weighted by a running power of "
        "two: 1*1 + 0*2 + 1*4 + 1*8 + 0*16 + 1*32 = 45. For 100000 only the "
        "most significant digit is set, giving 2^5 = 32."
},
{
    .id = "L1-A5", .title = "Digit chars to int",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int parseDigitsToInt(char *s) - converts a char array of digits "
            "into the integer it spells. Returns 0 if any character is not a "
            "digit.",
    .code =
        "char s1[] = {'1','4','5','\\0'};\n"
        "char s2[] = {'5','x','2','\\0'};\n"
        "printf(\"%d\\n\", parseDigitsToInt(s1));\n"
        "printf(\"%d\\n\", parseDigitsToInt(s2));",
    .answer = "145\n0",
    .explain =
        "v = v*10 + (*s - '0') builds the number one digit at a time: 1, then "
        "14, then 145. Subtracting the character '0' turns a digit character "
        "into its numeric value. The second call hits 'x', which is outside "
        "'0'..'9', so the whole parse is abandoned and 0 is returned - note "
        "that it bails out immediately, it does not return the 5 it already "
        "read."
},
{
    .id = "L1-A6", .title = "Triangle pattern",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "printTriangleA(int n) - two for loops only. Row i prints i copies "
            "of 'A'.",
    .code = "printTriangleA(4);",
    .answer = "A\nAA\nAAA\nAAAA",
    .explain =
        "The outer loop walks the rows 1..n and carries the row number; the "
        "inner loop runs that many times. The newline belongs to the outer "
        "loop, after the inner loop finishes - putting it inside the inner "
        "loop is the classic way to get one letter per line."
},
{
    .id = "L1-A7", .title = "Up-down pattern",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "printUpDownA(int n) - two for loops and at most one if/else. The "
            "row length climbs to n then comes back down to 1.",
    .code = "printUpDownA(4);",
    .answer = "A\nAA\nAAA\nAAAA\nAAA\nAA\nA",
    .explain =
        "There are 2n-1 rows, not 2n: the peak row of length n is printed once, "
        "not twice. The single if/else picks the length - row when climbing, "
        "2*n - row on the way down. For n = 4 that is 7 rows."
},
{
    .id = "L1-A8", .title = "Linear search",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int find(int size, int arr[], int toFind) - returns the index of "
            "the first match, or -1 if the value is not present.",
    .code =
        "int a[6] = {4,9,2,9,7,5};\n"
        "printf(\"%d\\n\", find(6, a, 9));\n"
        "printf(\"%d\\n\", find(6, a, 3));",
    .answer = "1\n-1",
    .explain =
        "The search returns on the FIRST match, so the 9 at index 3 is never "
        "reached - the answer is 1, not 3. 3 is not in the array at all, so the "
        "loop finishes and the function falls through to return -1."
},
{
    .id = "L1-A9", .title = "Second largest",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int find2ndLargest(int size, int arr[]) - returns the second "
            "largest DISTINCT value in an array of positive integers, or -1 "
            "if there is not one.",
    .code =
        "int b[6] = {5,12,9,12,7,1};\n"
        "printf(\"%d\\n\", find2ndLargest(6, b));",
    .answer = "9",
    .explain =
        "The largest value is 12 and it appears twice. 'Second largest' means "
        "the second largest distinct value, so the repeated 12 does not count "
        "as the runner-up - the answer is 9. This is why the update uses "
        "arr[i] < largest && arr[i] > second rather than just > second."
},
{
    .id = "L1-A10", .title = "Copy array",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "void copyArraytoArray(int size, int arr1[], int arr2[]) - copies "
            "every element of arr1 into arr2. Both arrays are the same size.",
    .code =
        "int s[4] = {3,1,4,1};\n"
        "int d[4] = {0,0,0,0};\n"
        "copyArraytoArray(4, s, d);\n"
        "for (int i = 0; i < 4; i++) printf(\"%d \", d[i]);\n"
        "printf(\"\\n\");",
    .answer = "3 1 4 1",
    .explain =
        "An array parameter decays to a pointer, so arr2 refers to the caller's "
        "d - writing through it changes d itself. That is why this works even "
        "though the function returns void."
},
{
    .id = "L1-A11", .title = "Insert with count",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "bool insertElement(int& size, int& count, int arr[], int x, int "
            "idx) - shifts elements right from idx and inserts x. count tracks "
            "how many elements are live and may not exceed size. Returns true "
            "on success.",
    .code =
        "int size = 6, count = 3;\n"
        "int arr[6] = {2,4,6,0,0,0};\n"
        "printf(\"%d\\n\", insertElement(size, count, arr, 5, 1));\n"
        "for (int i = 0; i < count; i++) printf(\"%d \", arr[i]);\n"
        "printf(\"\\n\");",
    .answer = "1\n2 5 4 6",
    .explain =
        "count (3) is below size (6) and idx 1 is within 0..count, so the "
        "insert succeeds and prints 1. The shift runs DOWNWARDS from i = count "
        "so no element is overwritten before it is copied: arr[3]=6, arr[2]=4, "
        "then arr[1]=5. count becomes 4, which is why the print loop shows four "
        "values and not three."
},
{
    .id = "L1-A12", .title = "Delete with count",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "bool deleteElement(int& size, int& count, int arr[], int idx) - "
            "shifts the higher elements left over idx and decrements count. "
            "Returns true on success.",
    .code =
        "int size = 6, count = 5;\n"
        "int arr[6] = {2,5,4,6,9,0};\n"
        "printf(\"%d\\n\", deleteElement(size, count, arr, 2));\n"
        "for (int i = 0; i < count; i++) printf(\"%d \", arr[i]);\n"
        "printf(\"\\n\");",
    .answer = "1\n2 5 6 9",
    .explain =
        "Index 2 holds the 4. Everything above it slides down one place and "
        "count drops from 5 to 4, so the 4 is gone and the print loop stops "
        "before the stale copy of 9 that is still sitting in memory at index 4. "
        "count, not size, is what defines the live part of the array."
},
{
    .id = "L1-A13", .title = "Frequency of a value",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int frequencyCount(int size, int arr[], int value) - counts how "
            "many times value appears.",
    .code =
        "int c[8] = {1,2,2,3,2,4,2,5};\n"
        "printf(\"%d\\n\", frequencyCount(8, c, 2));",
    .answer = "4",
    .explain = "The 2s sit at indexes 1, 2, 4 and 6 - four of them."
},
{
    .id = "L1-A14", .title = "Count duplicates",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int countDuplicates(int n, int a[]) - counts each duplicated "
            "VALUE at most once, using a nested loop that breaks on the first "
            "later match.",
    .code =
        "int dups[8] = {1,1,2,3,3,3,4,5};\n"
        "printf(\"%d\\n\", countDuplicates(8, dups));",
    .answer = "3",
    .explain =
        "The break is the whole trick. i=0 finds the second 1 and counts once. "
        "i=1 has no later 1, so the pair is not double counted. i=3 finds a "
        "later 3 and counts; i=4 also finds a later 3 and counts; i=5 has none "
        "left. Total 3. Note this counts occurrences-with-a-later-twin, so the "
        "three 3s contribute 2, not 1 - trace it, do not assume it counts "
        "distinct duplicated values."
},
{
    .id = "L1-A15", .title = "Reverse in place",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "void reverse(int n, int a[]) - reverses the array in place using "
            "one array and two indexes walking towards each other.",
    .code =
        "int r[5] = {1,2,3,4,5};\n"
        "reverse(5, r);\n"
        "for (int i = 0; i < 5; i++) printf(\"%d \", r[i]);\n"
        "printf(\"\\n\");",
    .answer = "5 4 3 2 1",
    .explain =
        "Two indexes, L from the front and R from the back, swap and step "
        "inwards while L < R. The condition must be L < R and not L <= R: with "
        "an odd length the middle element would swap with itself, which is "
        "harmless, but with L <= R on an even length you would swap every pair "
        "twice and get the original array back."
},
{
    .id = "L1-A16", .title = "Rotate left by one",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int rotateLeft(int n, int a[]) - every element moves one place "
            "left and the first element wraps around to the end.",
    .code =
        "int rot[5] = {7,8,9,10,11};\n"
        "rotateLeft(5, rot);\n"
        "for (int i = 0; i < 5; i++) printf(\"%d \", rot[i]);\n"
        "printf(\"\\n\");",
    .answer = "8 9 10 11 7",
    .explain =
        "a[0] must be saved into a temporary BEFORE the shift, because the "
        "first assignment a[0] = a[1] destroys it. After shifting, the saved "
        "value goes into the last slot."
},
{
    .id = "L1-A17", .title = "Two movies fill the flight",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "bool twoMovies(int L, int m[], int n) - true if two DIFFERENT "
            "entries sum exactly to the flight length L.",
    .code =
        "int movies[6] = {90,85,75,60,120,150};\n"
        "printf(\"%d\\n\", twoMovies(180, movies, 6));",
    .answer = "1",
    .explain =
        "60 + 120 = 180, so the answer is 1 (true prints as 1). The reason the "
        "inner loop starts at j = i + 1 is that 90 + 90 would otherwise 'work' "
        "by using the same movie twice - starting past i guarantees two "
        "distinct entries."
},
{
    .id = "L1-A18", .title = "Word counter",
    .topic = TOPIC_LAB1, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int wordCounter(int n, char s[]) - counts words, where a word "
            "starts at a letter that follows a non-word position. Only "
            "letters count as word characters.",
    .code =
        "char txt[] = \"Hello,  world 42 is big\";\n"
        "printf(\"%d\\n\", wordCounter(sizeof(txt)-1, txt));",
    .answer = "4",
    .explain =
        "Words are Hello, world, is and big - four. '42' is digits, never "
        "letters, so it never flips the in-a-word flag. The double space is "
        "harmless because the flag is only raised on the transition into a "
        "letter, which is the standard way to avoid counting separators."
},

/* ============================ LAB 1 - SECTION B ========================== */
{
    .id = "L1-B1", .title = "Leap year operators",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the missing operator.",
    .spec = "A century year is only a leap year when it is also divisible "
            "by 400.",
    .code =
        "int isLeapYear(int y) {\n"
        "    if (y % 4 != 0) return 0;\n"
        "    if (y % 100 == 0 && y % 400 ___!=___ 0) return 0;\n"
        "    return 1;\n"
        "}",
    .blanks = {
        { { "!=", NULL }, "Divisible by 100 but NOT by 400 means it is not a "
                          "leap year, so the test must be 'not equal to 0'." }
    },
    .blankCount = 1,
    .explain = "Read it as: if it is a century AND it is not a 400-year, "
               "reject it."
},
{
    .id = "L1-B2", .title = "Palindrome number",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Build the reversed number, then compare it with the value you "
            "saved before the loop destroyed n.",
    .code =
        "int isPalindromeNumber(int n) {\n"
        "    int orig = n, rev = 0;\n"
        "    while (n > 0) {\n"
        "        rev = rev * 10 + ___n % 10___;\n"
        "        n /= 10;\n"
        "    }\n"
        "    return ___rev == orig___;\n"
        "}",
    .blanks = {
        { { "n%10", "n % 10", NULL }, "% 10 peels off the last digit." },
        { { "rev==orig", "rev == orig", "orig==rev", NULL },
          "Compare against the saved copy - n itself is 0 by now." }
    },
    .blankCount = 2,
    .explain =
        "rev = rev * 10 + digit is the standard 'build a number from digits' "
        "idiom, and n /= 10 throws the digit away. The copy in orig is "
        "essential: the loop leaves n at 0."
},
{
    .id = "L1-B3", .title = "Prime check without sqrt",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the two comparison operators.",
    .spec = "Trial division. d*d <= n is the same bound as d <= sqrt(n) but "
            "uses only integers.",
    .code =
        "int isPrime(int n) {\n"
        "    if (n < 2) return 0;\n"
        "    for (int d = 2; d * d ___<=___ n; d++)\n"
        "        if (n % d ___==___ 0) return 0;\n"
        "    return 1;\n"
        "}",
    .blanks = {
        { { "<=", NULL }, "Must be <=, so a perfect square such as 25 is "
                          "still tested at d = 5." },
        { { "==", NULL }, "Remainder zero means d divides n." }
    },
    .blankCount = 2,
    .explain =
        "If the bound were d * d < n then 25 would slip through and be "
        "reported as prime, because d = 5 would never be tested."
},
{
    .id = "L1-B4", .title = "Binary to decimal",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the blank. Shift operators are not allowed.",
    .spec = "Peel the binary digits off the right-hand end of b and weight "
            "each one with a running power of two.",
    .code =
        "int binaryToDecimal(int b) {\n"
        "    int val = 0, pow2 = 1;\n"
        "    while (b > 0) {\n"
        "        int bit = ___b % 10___;\n"
        "        val += bit * pow2;\n"
        "        pow2 *= 2;\n"
        "        b /= 10;\n"
        "    }\n"
        "    return val;\n"
        "}",
    .blanks = {
        { { "b%10", "b % 10", NULL },
          "b holds the bits as decimal digits, so % 10 (not % 2) gets the "
          "next bit." }
    },
    .blankCount = 1,
    .explain =
        "This catches people out: because the bit pattern is stored as a "
        "DECIMAL number, you divide by 10 and take % 10, while the weight "
        "doubles each round."
},
{
    .id = "L1-B5", .title = "Parse digit characters",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Reject anything that is not a digit character; otherwise build "
            "the integer value.",
    .code =
        "int parseDigitsToInt(char *s) {\n"
        "    int v = 0;\n"
        "    for (; *s; s++) {\n"
        "        if (*s < '0' || *s ___>___ '9') return 0;\n"
        "        v = v * 10 + (*s - ___'0'___);\n"
        "    }\n"
        "    return v;\n"
        "}",
    .blanks = {
        { { ">", NULL }, "Above '9' is out of range, so the test is >." },
        { { "'0'", "48", NULL },
          "Subtracting the character '0' converts '7' into the number 7." }
    },
    .blankCount = 2,
    .explain =
        "Digit characters are contiguous in ASCII, so one range check covers "
        "them all and one subtraction converts them. for (; *s; s++) stops at "
        "the null terminator."
},
{
    .id = "L1-B6", .title = "Insert element",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the three blanks.",
    .spec = "Refuse the insert if the array is full or the index is out of "
            "range, then shift right and place the new value.",
    .code =
        "bool insertElement(int &size, int &count, int arr[], int x, int idx) {\n"
        "    if (count ___>=___ size || idx < 0 || idx ___>___ count)\n"
        "        return false;\n"
        "    for (int i = count; i > idx; --i) arr[i] = arr[i-1];\n"
        "    arr[idx] = x;\n"
        "    count___++___;\n"
        "    return true;\n"
        "}",
    .blanks = {
        { { ">=", NULL }, "Full when count has reached size." },
        { { ">", NULL },
          "idx == count is legal - that is appending on the end - so only "
          "idx > count is rejected." },
        { { "++", NULL }, "One more live element." }
    },
    .blankCount = 3,
    .explain =
        "The shift must run downwards (i from count towards idx) so each "
        "element is copied before it is overwritten. Note idx == count is "
        "allowed; using >= there would wrongly block appending."
},
{
    .id = "L1-B7", .title = "Delete element",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the three blanks.",
    .spec = "Refuse if the array is empty or the index is out of range, then "
            "shift the higher elements left and decrement count.",
    .code =
        "bool deleteElement(int &size, int &count, int arr[], int idx) {\n"
        "    if (count ___==___ 0 || idx < 0 || idx ___>=___ count)\n"
        "        return false;\n"
        "    for (int i = idx; i < count-1; ++i) arr[i] = arr[i+1];\n"
        "    count ___-= 1___;\n"
        "    return true;\n"
        "}",
    .blanks = {
        { { "==", NULL }, "Nothing to delete when count is 0." },
        { { ">=", NULL },
          "Valid indexes are 0..count-1, so idx == count is already out of "
          "range - here it IS >=, unlike the insert." },
        { { "-=1", "-= 1", "--", NULL }, "One fewer live element." }
    },
    .blankCount = 3,
    .explain =
        "Compare with insert: inserting allows idx == count (append), deleting "
        "does not (there is no element there). Getting those two boundary tests "
        "the wrong way round is the most common mistake on this pair."
},
{
    .id = "L1-B8", .title = "Count duplicates",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the blank.",
    .spec = "Nested loop; break after the first later match so one element is "
            "never counted twice.",
    .code =
        "int countDuplicates(int n, int a[]) {\n"
        "    int dup = 0;\n"
        "    for (int i = 0; i < n; i++)\n"
        "        for (int j = i+1; j < n; j++)\n"
        "            if (a[i] ___==___ a[j]) { dup++; break; }\n"
        "    return dup;\n"
        "}",
    .blanks = {
        { { "==", NULL }, "A duplicate is an equal value." }
    },
    .blankCount = 1,
    .explain =
        "j starts at i+1 so each pair is examined once, and the break stops "
        "the same i from counting more than one of its twins."
},
{
    .id = "L1-B9", .title = "Reverse in place",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the loop condition.",
    .spec = "Two indexes walking towards each other, swapping as they go.",
    .code =
        "void reverse(int n, int a[]) {\n"
        "    for (int L = 0, R = n-1; L ___<___ R; L++, R--) {\n"
        "        int t = a[L]; a[L] = a[R]; a[R] = t;\n"
        "    }\n"
        "}",
    .blanks = {
        { { "<", NULL },
          "L < R. With <= an even-length array gets swapped twice and ends up "
          "unchanged." }
    },
    .blankCount = 1,
    .explain =
        "Stop as soon as the two indexes meet or cross. Continuing past that "
        "point undoes the swaps you already made."
},
{
    .id = "L1-B10", .title = "Rotate left",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the blank.",
    .spec = "Shift every element one place left and wrap the old first "
            "element around to the end.",
    .code =
        "int rotateLeft(int n, int a[]) {\n"
        "    int first = a[0];\n"
        "    for (int i = 0; i < n-1; i++) a[i] = a[i+1];\n"
        "    a[n-1] = ___first___;\n"
        "    return 1;\n"
        "}",
    .blanks = {
        { { "first", NULL },
          "The saved copy. a[0] has already been overwritten by the shift." }
    },
    .blankCount = 1,
    .explain =
        "Writing a[n-1] = a[0] instead would store whatever a[0] holds AFTER "
        "the shift, which is the old a[1] - a classic off-by-one-variable bug."
},
{
    .id = "L1-B11", .title = "Two movies pair sum",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Check every pair of distinct entries for a sum that matches the "
            "flight length exactly.",
    .code =
        "bool twoMovies(int L, int m[], int n) {\n"
        "    for (int i = 0; i < n; i++)\n"
        "        for (int j = i+1; j < n; j++)\n"
        "            if (m[i] ___+___ m[j] ___==___ L) return true;\n"
        "    return false;\n"
        "}",
    .blanks = {
        { { "+", NULL }, "The two running times add up." },
        { { "==", NULL }, "Exactly the flight length, not merely under it." }
    },
    .blankCount = 2,
    .explain =
        "j = i+1 keeps the two movies distinct, and == makes it an exact fit. "
        "If the question asked for 'fits within the flight' you would use <=."
},
{
    .id = "L1-B12", .title = "Word counter",
    .topic = TOPIC_LAB1, .kind = Q_BLANKS,
    .prompt = "Fill in the blank.",
    .spec = "Count a word when you step onto a letter and were not already "
            "inside a word.",
    .code =
        "int wordCounter(int n, char s[]) {\n"
        "    int words = 0, in = 0;\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        char c = s[i];\n"
        "        int isLetter = (c>='A'&&c<='Z') || (c>='a'&&c<='z');\n"
        "        if (isLetter && !in) { in = 1; words___++___; }\n"
        "        else if (!isLetter && c == ' ') in = 0;\n"
        "    }\n"
        "    return words;\n"
        "}",
    .blanks = {
        { { "++", NULL }, "Count on the transition into a word." }
    },
    .blankCount = 1,
    .explain =
        "The in flag is what makes runs of spaces harmless: the counter only "
        "ticks on the edge from 'not in a word' to 'in a word'."
},

/* ============================ LAB 2 - SECTION A ========================== */
{
    .id = "L2-A1", .title = "ReplaceLargestValue",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid, then the final line.",
    .spec = "int ReplaceLargestValue(int array[4][6]) - find the largest "
            "value in the whole grid, replace EVERY occurrence with -1 and "
            "return the largest value found.",
    .code =
        "int A[4][6] = {{1,2,4,1,3,4},\n"
        "               {2,4,2,3,1,2},\n"
        "               {1,3,4,4,2,1},\n"
        "               {2,1,3,2,4,4}};\n"
        "int maxv = ReplaceLargestValue(A);\n"
        "PrintArray(A);\n"
        "printf(\"maxv=%d\\n\", maxv);",
    .answer =
        "1 2 -1 1 3 -1\n"
        "2 -1 2 3 1 2\n"
        "1 3 -1 -1 2 1\n"
        "2 1 3 2 -1 -1\n"
        "maxv=4",
    .explain =
        "The largest value is 4 and there are six of them, so all six become "
        "-1 while everything else is untouched. It takes two passes: one to "
        "find the maximum, a second to replace. Trying to do it in one pass "
        "misses the earlier 4s you walked past before you knew 4 was the max."
},
{
    .id = "L2-A2", .title = "CountAndReplaceZeros",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid, then the final line.",
    .spec = "int CountAndReplaceZeros(int array[4][6]) - replace every zero "
            "with -1 and return how many zeros there were.",
    .code =
        "int B[4][6] = {{0,1,1,0,2,3},\n"
        "               {2,0,3,1,0,0},\n"
        "               {1,2,0,3,1,2},\n"
        "               {0,0,1,2,3,0}};\n"
        "int c = CountAndReplaceZeros(B);\n"
        "PrintArray(B);\n"
        "printf(\"count=%d\\n\", c);",
    .answer =
        "-1 1 1 -1 2 3\n"
        "2 -1 3 1 -1 -1\n"
        "1 2 -1 3 1 2\n"
        "-1 -1 1 2 3 -1\n"
        "count=9",
    .explain =
        "Count the zeros row by row: 2 + 3 + 1 + 3 = 9. Counting the -1s in "
        "the printed grid gives the same 9, which is a good self-check.\n\n"
        "Watch out: the circulated answer key says count=8 while printing nine "
        "-1 values - it contradicts itself. The grid is right, the number is "
        "not. Always recount against the grid you printed."
},
{
    .id = "L2-A3", .title = "ReplaceRowPattern",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid, then the final line.",
    .spec = "int ReplaceRowPattern(int array[4][6], int pattern[3]) - find "
            "horizontal runs of 3 cells matching the pattern, replace them "
            "with -1 and return the number of matches.",
    .code =
        "int P[4][6] = {{3,3,3,1,2,3},\n"
        "               {1,3,3,3,2,1},\n"
        "               {2,2,3,3,3,2},\n"
        "               {3,2,1,3,3,3}};\n"
        "int pat[3] = {3,3,3};\n"
        "int matches = ReplaceRowPattern(P, pat);\n"
        "PrintArray(P);\n"
        "printf(\"matches=%d\\n\", matches);",
    .answer =
        "-1 -1 -1 1 2 3\n"
        "1 -1 -1 -1 2 1\n"
        "2 2 -1 -1 -1 2\n"
        "3 2 1 -1 -1 -1\n"
        "matches=4",
    .explain =
        "Each row contains exactly one run of three 3s, at columns 0-2, 1-3, "
        "2-4 and 3-5 respectively - four matches. The column loop must stop at "
        "c <= 6 - 3, otherwise you read past the end of the row. After a match "
        "you also need to skip past the cells you just blanked, or the -1s "
        "could be re-examined."
},
{
    .id = "L2-A4", .title = "ReplaceBorder",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid, then the final line.",
    .spec = "int ReplaceBorder(int array[4][6]) - replace the first and last "
            "row and the first and last column with -1, returning how many "
            "cells were replaced.",
    .code =
        "int C[4][6] = {{9,8,7,6,5,4},\n"
        "               {1,2,3,4,5,6},\n"
        "               {6,5,4,3,2,1},\n"
        "               {4,5,6,7,8,9}};\n"
        "int r = ReplaceBorder(C);\n"
        "PrintArray(C);\n"
        "printf(\"replaced=%d\\n\", r);",
    .answer =
        "-1 -1 -1 -1 -1 -1\n"
        "-1 2 3 4 5 -1\n"
        "-1 5 4 3 2 -1\n"
        "-1 -1 -1 -1 -1 -1\n"
        "replaced=16",
    .explain =
        "In a 4x6 grid only the middle 2x4 block survives, so 24 - 8 = 16 "
        "cells are replaced. The count is 16 and not 20 because the four "
        "corners belong to a row AND a column - the single condition "
        "(r==0 || r==3 || c==0 || c==5) counts each cell once, whereas looping "
        "the rows and then the columns separately would double count them."
},
{
    .id = "L2-A5", .title = "FindColumnSum",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int FindColumnSum(int array[4][6], int col) - sum one column, "
            "ignoring -1 cells (they are blanks, not values).",
    .code =
        "int D[4][6] = {{ 1,-1, 3, 4, 5, 6},\n"
        "               {-1, 2,-1, 1, 0, 2},\n"
        "               { 3,-1, 1,-1, 2, 1},\n"
        "               { 4, 2,-1, 0,-1, 3}};\n"
        "printf(\"col2sum=%d\\n\", FindColumnSum(D, 2));",
    .answer = "col2sum=4",
    .explain =
        "Column 2 top to bottom is 3, -1, 1, -1. Skipping the -1s leaves "
        "3 + 1 = 4. If you had not skipped them you would get 2 - which is "
        "exactly what the 'ignore -1' instruction is testing. Note the ROW "
        "index varies while the column index stays fixed."
},
{
    .id = "L2-A6", .title = "FallDownAndReplace",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid.",
    .spec = "void FallDownAndReplace(int array[4][6]) - gravity. In each "
            "COLUMN every value that is not -1 falls to the bottom keeping "
            "its relative order; cells left empty at the top become -2.",
    .code =
        "int E[4][6] = {{ 1, 2,-1, 3, 4, 3},\n"
        "               { 1,-1,-1, 3, 4,-1},\n"
        "               { 1, 2,-1, 3,-1, 2},\n"
        "               {-1,-1,-1, 2, 3, 3}};\n"
        "FallDownAndReplace(E);\n"
        "PrintArray(E);",
    .answer =
        "-2 -2 -2 3 -2 -2\n"
        "1 -2 -2 3 4 3\n"
        "1 2 -2 3 4 2\n"
        "1 2 -2 2 3 3",
    .explain =
        "Work one column at a time, bottom up, with a write index. Column 0 "
        "holds 1,1,1,-1 so the three 1s land in rows 1-3 and row 0 becomes -2. "
        "Column 2 is all -1, so it becomes four -2s. Column 3 has no -1 at all "
        "and does not move. Column 4 is 4,4,-1,3 which compacts to 4,4,3 in "
        "rows 1-3.\n\n"
        "The circulated key's grid for this question is internally "
        "inconsistent - it shows values appearing in columns that were "
        "entirely -1. The grid above is what the stated rule actually produces."
},
{
    .id = "L2-A7", .title = "Unique values and most frequent",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "CountUniqueValues and FindMostFrequent both ignore 0 and -1 and "
            "use a frequency array indexed by the value itself (values 1..20).",
    .code =
        "int G[4][6] = {{1,2,2,0,3,-1},\n"
        "               {2,2,3,1,-1,4},\n"
        "               {4,0,1,2,3,3},\n"
        "               {1,1,2,2,4,0}};\n"
        "printf(\"unique=%d\\n\", CountUniqueValues(G));\n"
        "printf(\"mostFrequent=%d\\n\", FindMostFrequent(G));",
    .answer = "unique=4\nmostFrequent=2",
    .explain =
        "Ignoring 0 and -1 the distinct values are 1, 2, 3 and 4 -> unique=4. "
        "Tallying them: 1 appears 5 times, 2 appears 7, 3 appears 4, 4 appears "
        "3. So 2 is the most frequent.\n\n"
        "Using the value as the index into seen[]/freq[] is why the spec caps "
        "values at 20: the array must be big enough for the largest value, "
        "hence int freq[21] and not freq[20]."
},
{
    .id = "L2-A8", .title = "ShiftArrayRight",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid.",
    .spec = "void ShiftArrayRight(int array[4][6]) - shift every element one "
            "place right within its own row; the last column wraps round to "
            "become the first.",
    .code =
        "int H[4][6] = {{ 1, 2, 3, 4, 5, 6},\n"
        "               { 7, 8, 9,10,11,12},\n"
        "               {13,14,15,16,17,18},\n"
        "               {19,20,21,22,23,24}};\n"
        "ShiftArrayRight(H);\n"
        "PrintArray(H);",
    .answer =
        "6 1 2 3 4 5\n"
        "12 7 8 9 10 11\n"
        "18 13 14 15 16 17\n"
        "24 19 20 21 22 23",
    .explain =
        "Each row rotates independently, so no value ever moves between rows. "
        "Save the last column first, then copy downwards from c = 5 to c = 1 "
        "(a[r][c] = a[r][c-1]) so nothing is overwritten early, then drop the "
        "saved value into column 0. Shifting RIGHT means the loop runs "
        "right-to-left - the opposite direction to the shift itself."
},
{
    .id = "L2-A9", .title = "ReverseArrayRows then SwapRows",
    .topic = TOPIC_LAB2, .kind = Q_OUTPUT,
    .prompt = "Give the printed grid.",
    .spec = "ReverseArrayRows reverses each row in place. SwapRows(a, r1, r2) "
            "exchanges two whole rows.",
    .code =
        "int I[4][6] = {{ 1, 2, 3, 4, 5, 6},\n"
        "               { 7, 8, 9,10,11,12},\n"
        "               {13,14,15,16,17,18},\n"
        "               {19,20,21,22,23,24}};\n"
        "ReverseArrayRows(I);\n"
        "SwapRows(I, 0, 3);\n"
        "PrintArray(I);",
    .answer =
        "24 23 22 21 20 19\n"
        "12 11 10 9 8 7\n"
        "18 17 16 15 14 13\n"
        "6 5 4 3 2 1",
    .explain =
        "Do the operations in order. After reversing, row 0 is 6..1 and row 3 "
        "is 24..19. Swapping rows 0 and 3 then puts 24..19 on top and 6..1 at "
        "the bottom, while rows 1 and 2 keep their reversed order. Reversing a "
        "row uses the same two-index swap as the 1D reverse, run once per row."
},

/* ============================ LAB 2 - SECTION B ========================== */
{
    .id = "L2-B1", .title = "CountUniqueValues",
    .topic = TOPIC_LAB2, .kind = Q_BLANKS,
    .prompt = "Fill in the three blanks.",
    .spec = "Count distinct values in the grid, ignoring 0 and -1. Values are "
            "in the range 1..20, so a seen[] array indexed by value works.",
    .code =
        "int CountUniqueValues(int a[4][6]) {\n"
        "    int seen[21] = {0};\n"
        "    int cnt = 0;\n"
        "    for (int r = 0; r < 4; r++)\n"
        "        for (int c = 0; c < 6; c++) {\n"
        "            int v = a[r][c];\n"
        "            if (v ___<= 0___) continue;\n"
        "            if (seen[v] ___== 0___) { seen[v] = 1; cnt___++___; }\n"
        "        }\n"
        "    return cnt;\n"
        "}",
    .blanks = {
        { { "<=0", "<= 0", NULL },
          "One test skips both 0 and -1, since both are <= 0." },
        { { "==0", "== 0", NULL }, "Not seen yet." },
        { { "++", NULL }, "A new distinct value." }
    },
    .blankCount = 3,
    .explain =
        "seen[] is a lookup table indexed BY THE VALUE, which turns 'have I "
        "met this before?' into a single array read instead of a search. Note "
        "the size is 21, not 20, because index 20 must exist."
},
{
    .id = "L2-B2", .title = "FindMostFrequent",
    .topic = TOPIC_LAB2, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Tally every value into a frequency array, then pick the index "
            "with the highest tally. 0 and -1 are ignored.",
    .code =
        "int FindMostFrequent(int a[4][6]) {\n"
        "    int freq[21] = {0};\n"
        "    for (int r = 0; r < 4; r++)\n"
        "        for (int c = 0; c < 6; c++) {\n"
        "            int v = a[r][c];\n"
        "            if (v <= 0) continue;\n"
        "            freq[v] ___= freq[v] + 1___;\n"
        "        }\n"
        "    int best = 1;\n"
        "    for (int v = 2; v <= 20; v++)\n"
        "        if (freq[v] ___>___ freq[best]) best = v;\n"
        "    return best;\n"
        "}",
    .blanks = {
        { { "=freq[v]+1", "= freq[v] + 1", "+=1", "+= 1", NULL },
          "Increment the tally for that value." },
        { { ">", NULL },
          "Strictly greater keeps the FIRST value on a tie; >= would keep the "
          "last." }
    },
    .blankCount = 2,
    .explain =
        "The second loop is a plain 'find the maximum' over the tallies, but "
        "it returns the INDEX (the value) rather than the count. Whether ties "
        "resolve to the lowest or highest value is decided entirely by > "
        "versus >=."
},
{
    .id = "L2-B3", .title = "ShiftArrayRight",
    .topic = TOPIC_LAB2, .kind = Q_BLANKS,
    .prompt = "Fill in the blank.",
    .spec = "Cyclic shift right, one row at a time.",
    .code =
        "void ShiftArrayRight(int a[4][6]) {\n"
        "    for (int r = 0; r < 4; r++) {\n"
        "        int last = a[r][5];\n"
        "        for (int c = 5; c > 0; c--) a[r][c] = a[r][c-1];\n"
        "        a[r][0] = ___last___;\n"
        "    }\n"
        "}",
    .blanks = {
        { { "last", NULL },
          "The saved copy of the old last column - a[r][5] has already been "
          "overwritten." }
    },
    .blankCount = 1,
    .explain =
        "Same shape as rotateLeft, mirrored: save the element that is about to "
        "be lost, shift away from it, then put it back at the other end."
},
{
    .id = "L2-B4", .title = "SwapRows",
    .topic = TOPIC_LAB2, .kind = Q_BLANKS,
    .prompt = "Fill in the three blanks to complete the swap.",
    .spec = "void SwapRows(int array[4][6], int row1, int row2) - exchange "
            "two whole rows, column by column.",
    .code =
        "void SwapRows(int a[4][6], int r1, int r2) {\n"
        "    for (int c = 0; c < 6; c++) {\n"
        "        int tmp = ___a[r1][c]___;\n"
        "        a[r1][c] = ___a[r2][c]___;\n"
        "        a[r2][c] = ___tmp___;\n"
        "    }\n"
        "}",
    .blanks = {
        { { "a[r1][c]", NULL }, "Stash the value you are about to overwrite." },
        { { "a[r2][c]", NULL }, "Copy the other row's value in." },
        { { "tmp", NULL }, "And the stashed value goes the other way." }
    },
    .blankCount = 3,
    .explain =
        "The three-step temporary swap, applied once per column. You cannot "
        "swap two whole rows with a single assignment in C - arrays are not "
        "assignable - so the column loop is doing real work."
},
{
    .id = "L2-B5", .title = "ReplaceBorder",
    .topic = TOPIC_LAB2, .kind = Q_BLANKS,
    .prompt = "Fill in the border condition.",
    .spec = "Replace the outer ring of a 4x6 grid with -1 and return the "
            "number of cells replaced, counting each cell only once.",
    .code =
        "int ReplaceBorder(int a[4][6]) {\n"
        "    int n = 0;\n"
        "    for (int r = 0; r < 4; r++)\n"
        "        for (int c = 0; c < 6; c++)\n"
        "            if (___r == 0___ || r == 3 || c == 0 || ___c == 5___) {\n"
        "                a[r][c] = -1;\n"
        "                n___++___;\n"
        "            }\n"
        "    return n;\n"
        "}",
    .blanks = {
        { { "r==0", "r == 0", NULL }, "First row." },
        { { "c==5", "c == 5", NULL },
          "Last column. 5 is 6-1 - the last valid index, not 6." },
        { { "++", NULL }, "One cell replaced." }
    },
    .blankCount = 3,
    .explain =
        "Testing all four edges inside one pass over every cell is what keeps "
        "the corners from being counted twice, giving 16 rather than 20."
},

/* ============================ LAB 3 - SECTION A ========================== */
{
    .id = "L3-A1", .title = "swap_ints",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "void swap_ints(int *a, int *b) - swap the two values the "
            "pointers point at.",
    .code =
        "int x = 7, y = 11;\n"
        "swap_ints(&x, &y);\n"
        "printf(\"x=%d y=%d\\n\", x, y);",
    .answer = "x=11 y=7",
    .explain =
        "&x passes the ADDRESS of x, so *a = ... writes into the caller's "
        "variable. This is the whole reason the function takes pointers: a "
        "plain swap(int a, int b) would only shuffle its own copies and the "
        "caller would still see 7 and 11."
},
{
    .id = "L3-A2", .title = "reverse_array with pointers",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "void reverse_array(int *arr, int size) - reverse in place using "
            "two pointers walking towards each other.",
    .code =
        "int a[6] = {1,2,3,4,5,6};\n"
        "reverse_array(a, 6);\n"
        "for (int i = 0; i < 6; i++) printf(\"%d \", a[i]);\n"
        "printf(\"\\n\");",
    .answer = "6 5 4 3 2 1",
    .explain =
        "Identical logic to the index version, but L and R are pointers: L = "
        "arr, R = arr + size - 1, and the loop runs while L < R. Comparing and "
        "incrementing pointers is legal and idiomatic inside one array."
},
{
    .id = "L3-A3", .title = "strlen_ptr",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int strlen_ptr(char *s) - length of a null-terminated char "
            "array, found by walking a pointer to the terminator.",
    .code =
        "char s[] = \"Hello, world!\";\n"
        "printf(\"len=%d\\n\", strlen_ptr(s));",
    .answer = "len=13",
    .explain =
        "H-e-l-l-o-,-space-w-o-r-l-d-! is 13 characters. The comma, the space "
        "and the '!' all count - only the invisible '\\0' does not, and that is "
        "exactly why the length is p - s and not p - s + 1. The array itself "
        "occupies 14 bytes."
},
{
    .id = "L3-A4", .title = "count_vowels",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int count_vowels(char *s) - count vowels, upper and lower case.",
    .code =
        "char t[] = \"OpenAI rocks\";\n"
        "printf(\"vowels=%d\\n\", count_vowels(t));",
    .answer = "vowels=5",
    .explain =
        "O, e, A, I and o - five. Both cases have to be tested; checking only "
        "a-e-i-o-u would find just e and o and give 2. 'y' is not counted."
},
{
    .id = "L3-A5", .title = "sum_array",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int sum_array(int *arr, int n) - add up n ints reached through "
            "pointer arithmetic.",
    .code =
        "int z[5] = {10, -3, 7, 0, 6};\n"
        "printf(\"sum=%d\\n\", sum_array(z, 5));",
    .answer = "sum=20",
    .explain =
        "10 - 3 + 7 + 0 + 6 = 20. *(arr + i) is exactly what arr[i] compiles "
        "to, so the negative value is added normally - there is no special "
        "case."
},
{
    .id = "L3-A6", .title = "compare_strings",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int compare_strings(char *a, char *b) - like strcmp: negative if "
            "a sorts before b, 0 if equal, positive otherwise. Here only the "
            "sign is printed.",
    .code =
        "char p[] = \"apple\";\n"
        "char q[] = \"apples\";\n"
        "int cmp = compare_strings(p, q);\n"
        "printf(\"cmp=%d\\n\", cmp == 0 ? 0 : (cmp < 0 ? -1 : 1));",
    .answer = "cmp=-1",
    .explain =
        "The walk stops where the strings differ: p has reached its '\\0' (0) "
        "while q still has 's' (115). 0 - 115 is negative, so \"apple\" sorts "
        "before \"apples\" - a prefix is always smaller. Note the function "
        "returns a difference, not necessarily -1, which is why real code tests "
        "the SIGN of strcmp and never compares it to -1."
},
{
    .id = "L3-A7", .title = "find_max with pointers",
    .topic = TOPIC_LAB3, .kind = Q_OUTPUT,
    .prompt = "What exactly does this print?",
    .spec = "int find_max(int *arr, int size) - largest value, read through "
            "*(arr + i).",
    .code =
        "int arr[5] = {3,9,2,8,5};\n"
        "printf(\"max=%d\\n\", find_max(arr, 5));",
    .answer = "max=9",
    .explain =
        "Seed best with *arr (the first element) and start the loop at i = 1. "
        "Seeding with 0 instead would be a bug on an all-negative array - "
        "always seed a max search from real data."
},

/* ============================ LAB 3 - SECTION B ========================== */
{
    .id = "L3-B1", .title = "find_max (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks. No [] indexing allowed.",
    .spec = "Walk the array with pointer arithmetic and keep the largest "
            "value seen.",
    .code =
        "int find_max(int *arr, int size) {\n"
        "    int best = *arr;\n"
        "    for (int i = 1; i < size; i++) {\n"
        "        int val = *(___arr + i___);\n"
        "        if (val ___>___ best) best = val;\n"
        "    }\n"
        "    return best;\n"
        "}",
    .blanks = {
        { { "arr+i", "arr + i", NULL },
          "*(arr + i) is the pointer spelling of arr[i]." },
        { { ">", NULL }, "Keep the larger value." }
    },
    .blankCount = 2,
    .explain =
        "arr + i advances by i WHOLE INTS, not i bytes - the compiler scales "
        "by sizeof(int) for you. That is why the same expression works "
        "unchanged for a char array."
},
{
    .id = "L3-B2", .title = "reverse_array (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the four blanks.",
    .spec = "Two pointers, one at each end, swapping and stepping inwards.",
    .code =
        "void reverse_array(int *arr, int size) {\n"
        "    int *L = arr;\n"
        "    int *R = arr + ___size - 1___;\n"
        "    while (L ___<___ R) {\n"
        "        int tmp = *L; *L = *R; *R = tmp;\n"
        "        L = ___L + 1___;\n"
        "        R = ___R - 1___;\n"
        "    }\n"
        "}",
    .blanks = {
        { { "size-1", "size - 1", NULL },
          "arr + size would point one PAST the end - the last element is at "
          "size - 1." },
        { { "<", NULL }, "Stop when the pointers meet." },
        { { "L+1", "L + 1", NULL }, "Step forwards one int." },
        { { "R-1", "R - 1", NULL }, "Step backwards one int." }
    },
    .blankCount = 4,
    .explain =
        "arr + size is a valid pointer to form but must never be dereferenced. "
        "Off-by-one here reads memory you do not own, which is the whole reason "
        "the last index is size - 1."
},
{
    .id = "L3-B3", .title = "strlen_ptr (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Walk a pointer to the null terminator, then work out how far it "
            "travelled.",
    .code =
        "int strlen_ptr(char *s) {\n"
        "    char *p = s;\n"
        "    while (*p ___!= '\\0'___) p++;\n"
        "    return ___p - s___;\n"
        "}",
    .blanks = {
        { { "!='\\0'", "!= '\\0'", "!=0", "!= 0", NULL },
          "Stop at the null terminator." },
        { { "p-s", "p - s", NULL },
          "Pointer difference = number of elements between them." }
    },
    .blankCount = 2,
    .explain =
        "Subtracting two pointers into the same array gives a COUNT of "
        "elements, not a byte count. Since p stopped ON the terminator, p - s "
        "is the length without it - no +1 or -1 needed."
},
{
    .id = "L3-B4", .title = "copy_string (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Your own strcpy. Assume dest has enough room.",
    .code =
        "void copy_string(char *dest, char *src) {\n"
        "    while (___*src___) { *dest = *src; dest++; src++; }\n"
        "    ___*dest = '\\0'___;\n"
        "}",
    .blanks = {
        { { "*src", "*src!=0", "*src != '\\0'", NULL },
          "Loop while the current source character is not the terminator - a "
          "non-zero char is already true." },
        { { "*dest='\\0'", "*dest = '\\0'", "*dest=0", "*dest = 0", NULL },
          "The loop stops BEFORE copying the terminator, so write it by hand." }
    },
    .blankCount = 2,
    .explain =
        "Forgetting the final terminator is the classic bug: the characters "
        "are all there but the string has no end, so every later read runs off "
        "into whatever happens to follow in memory."
},
{
    .id = "L3-B5", .title = "concat_strings (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the two blanks.",
    .spec = "Your own strcat. Walk to the end of dest first, then append.",
    .code =
        "void concat_strings(char *dest, char *src) {\n"
        "    while (*dest) dest++;\n"
        "    while (___*src___) { *dest = *src; dest++; src++; }\n"
        "    ___*dest = '\\0'___;\n"
        "}",
    .blanks = {
        { { "*src", "*src!=0", "*src != '\\0'", NULL },
          "Same copy loop as copy_string." },
        { { "*dest='\\0'", "*dest = '\\0'", "*dest=0", "*dest = 0", NULL },
          "Terminate the joined string." }
    },
    .blankCount = 2,
    .explain =
        "The first loop leaves dest sitting exactly ON the old terminator, "
        "which is where the new text must start - so the old '\\0' is "
        "overwritten by the first copied character and a fresh one is added at "
        "the very end."
},
{
    .id = "L3-B6", .title = "sum_array (pointers only)",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the blank.",
    .spec = "Add up n ints reached with *(arr + i).",
    .code =
        "int sum_array(int *arr, int n) {\n"
        "    int sum = 0;\n"
        "    for (int i = 0; i < n; i++) sum ___+=___ *(arr + i);\n"
        "    return sum;\n"
        "}",
    .blanks = {
        { { "+=", NULL }, "Accumulate into the running total." }
    },
    .blankCount = 1,
    .explain =
        "sum += *(arr + i) is the accumulator pattern: initialise outside the "
        "loop, add inside, return after."
},
{
    .id = "L3-B7", .title = "swap_ints and swap_chars",
    .topic = TOPIC_LAB3, .kind = Q_BLANKS,
    .prompt = "Fill in the three blanks.",
    .spec = "Swap the values the two pointers point at - not the pointers "
            "themselves.",
    .code =
        "void swap_ints(int *a, int *b) {\n"
        "    int tmp = ___*a___;\n"
        "    ___*a___ = *b;\n"
        "    *b = ___tmp___;\n"
        "}",
    .blanks = {
        { { "*a", NULL }, "Dereference: the VALUE at a, not the address a." },
        { { "*a", NULL }, "Write through the pointer." },
        { { "tmp", NULL }, "The stashed value." }
    },
    .blankCount = 3,
    .explain =
        "Every line needs the * . Writing tmp = a; a = b; b = tmp; compiles "
        "with warnings and swaps the local pointer variables, leaving the "
        "caller's x and y completely untouched - a bug that looks right and "
        "does nothing."
},
};

const int QUESTION_COUNT = (int)(sizeof QUESTIONS / sizeof QUESTIONS[0]);

/* ---------------------------------------------------------------------------
 *  Topic indexing helpers. The bank is small enough that a linear walk is
 *  cheaper than maintaining a second index table.
 * ------------------------------------------------------------------------- */
int TopicQuestionCount(Topic t)
{
    int n = 0;
    for (int i = 0; i < QUESTION_COUNT; i++) if (QUESTIONS[i].topic == t) n++;
    return n;
}

int TopicQuestionIndex(Topic t, int nth)
{
    int n = 0;
    for (int i = 0; i < QUESTION_COUNT; i++) {
        if (QUESTIONS[i].topic != t) continue;
        if (n == nth) return i;
        n++;
    }
    return -1;
}

/* =============================================================================
 *  LECTURE NOTES
 * ========================================================================== */

static const Note kNotesLab1[] = {
{
    "Functions, parameters and return values",
    "Every lab answer is a function with a fixed signature - the exam gives you "
    "the prototype and you fill in the body, so read the return type first. A "
    "function returning int that reports yes/no returns 1 or 0; bool prints as "
    "1 or 0 too, which is why Section A answers for insert/delete show 1 rather "
    "than 'true'.\n\n"
    "Arguments are passed BY VALUE: the function gets a copy. To let a function "
    "change the caller's variable you pass a reference (int& in C++) or a "
    "pointer (int* in C). Arrays are the exception - an array argument decays "
    "to a pointer to its first element, so a function can always modify an "
    "array that was passed to it.",
    "int   isLeapYear(int y);                 // value in, value out\n"
    "void  copyArraytoArray(int size, int a[], int b[]);  // b IS the caller's\n"
    "bool  insertElement(int& size, int& count, int arr[], int x, int idx);"
},
{
    "The size / count pair",
    "size is how many slots the array HAS. count is how many of them currently "
    "hold real data. They are not the same thing and the exam leans on the "
    "difference constantly.\n\n"
    "Insert fails when count has reached size (no room). Delete fails when "
    "count is 0 (nothing there). Printing loops run to count, not size - which "
    "is why after a delete the stale value still sitting in memory is not "
    "printed. Valid indexes are 0..count-1, except when inserting, where idx "
    "== count is legal because that means appending on the end.",
    "// insert:  idx may equal count   -> reject only idx > count\n"
    "// delete:  idx must be < count   -> reject idx >= count\n"
    "if (count >= size || idx < 0 || idx > count)  return false;  // insert\n"
    "if (count == 0    || idx < 0 || idx >= count) return false;  // delete"
},
{
    "Shifting in the safe direction",
    "Whenever you move elements inside one array, the loop must run AWAY from "
    "where you are writing, or you overwrite a value before you have copied "
    "it.\n\n"
    "Inserting shifts right, so walk from the far end down towards the gap. "
    "Deleting shifts left, so walk from the gap up towards the end. If you ever "
    "need the element that is about to be destroyed - as in rotate - save it "
    "in a temporary BEFORE the loop starts.",
    "// insert at idx: walk downwards\n"
    "for (int i = count; i > idx; --i) arr[i] = arr[i-1];\n"
    "\n"
    "// delete at idx: walk upwards\n"
    "for (int i = idx; i < count-1; ++i) arr[i] = arr[i+1];\n"
    "\n"
    "// rotate left: save first, THEN shift\n"
    "int first = a[0];\n"
    "for (int i = 0; i < n-1; i++) a[i] = a[i+1];\n"
    "a[n-1] = first;"
},
{
    "Digit peeling: % 10 and / 10",
    "Three separate exam questions are the same trick. To work through a "
    "number's decimal digits from the right: n % 10 gives the last digit and "
    "n /= 10 throws it away. Loop while n > 0.\n\n"
    "To build a number up from digits, go the other way: v = v * 10 + digit. "
    "Palindrome rebuilds the number reversed and compares; binaryToDecimal "
    "weights each peeled digit by a doubling power of two; parseDigitsToInt "
    "builds from characters using (*s - '0').\n\n"
    "Always keep a copy of the original if you need to compare against it - "
    "the loop leaves n at 0.",
    "int orig = n, rev = 0;\n"
    "while (n > 0) { rev = rev*10 + n % 10; n /= 10; }\n"
    "return rev == orig;\n"
    "\n"
    "// binary stored as a decimal number: still % 10, but weight doubles\n"
    "int val = 0, pow2 = 1;\n"
    "while (b > 0) { val += (b % 10) * pow2; pow2 *= 2; b /= 10; }"
},
{
    "Nested loop patterns",
    "The outer loop owns the rows, the inner loop owns the characters on a "
    "row, and the newline belongs to the OUTER loop - putting it inside the "
    "inner loop gives you one character per line.\n\n"
    "For the up-down triangle, there are 2n-1 rows, not 2n: the peak is "
    "printed once. One if/else picks the row length - row on the way up, "
    "2*n - row on the way down.",
    "// triangle\n"
    "for (int i = 1; i <= n; i++) {\n"
    "    for (int j = 0; j < i; j++) putchar('A');\n"
    "    putchar('\\n');\n"
    "}\n"
    "\n"
    "// up then down: 2n-1 rows\n"
    "for (int row = 1; row <= 2*n - 1; row++) {\n"
    "    int len = (row <= n) ? row : 2*n - row;\n"
    "    for (int j = 0; j < len; j++) putchar('A');\n"
    "    putchar('\\n');\n"
    "}"
},
{
    "Searching, maximums and pairs",
    "Linear search returns on the FIRST match, so a repeated value reports its "
    "earliest index. If nothing matched, the loop finishes and you return -1 "
    "after it - not inside it.\n\n"
    "Second largest means second largest DISTINCT value, so a repeated maximum "
    "is not its own runner-up: the update needs arr[i] < largest && arr[i] > "
    "second. Seed a maximum search from real data (*arr or arr[0]), never from "
    "0, or an all-negative array breaks it.\n\n"
    "For any 'is there a pair that...' question, the inner loop starts at "
    "j = i + 1. That both avoids testing a pair twice and stops an element "
    "pairing with itself.",
    "int find(int size, int arr[], int toFind) {\n"
    "    for (int i = 0; i < size; i++)\n"
    "        if (arr[i] == toFind) return i;   // first match wins\n"
    "    return -1;                            // after the loop\n"
    "}\n"
    "\n"
    "for (int i = 0; i < n; i++)\n"
    "    for (int j = i+1; j < n; j++)         // distinct pairs only\n"
    "        if (m[i] + m[j] == L) return true;"
},
};

static const Note kNotesLab2[] = {
{
    "Walking a 4x6 grid",
    "A 2D array is rows of columns: array[row][column]. The outer loop almost "
    "always walks the rows and the inner loop the columns, which visits cells "
    "left to right, top to bottom - the order the lab sheet specifies for the "
    "cluster scan.\n\n"
    "To work down a COLUMN instead, fix the column and vary the row. That is "
    "the only change FindColumnSum needs, and it is the one people get "
    "backwards under time pressure.\n\n"
    "When a 2D array is a parameter, the column count is part of the type: "
    "int a[4][6] or int a[][6]. You cannot leave the 6 out, because the "
    "compiler needs it to find the start of each row.",
    "// every cell, row by row\n"
    "for (int r = 0; r < 4; r++)\n"
    "    for (int c = 0; c < 6; c++)\n"
    "        use(a[r][c]);\n"
    "\n"
    "// one column, top to bottom\n"
    "for (int r = 0; r < 4; r++)\n"
    "    if (a[r][col] != -1) sum += a[r][col];"
},
{
    "-1 means empty, and two passes beat one",
    "Across Lab 2, -1 is the 'blank cell' marker, which is why sums and "
    "frequency counts skip it. 0 is also excluded from the unique/frequency "
    "questions, so one test, v <= 0, handles both.\n\n"
    "ReplaceLargestValue needs TWO passes: you cannot replace occurrences of "
    "the maximum while still looking for it, because the 4s you walked past "
    "before you knew 4 was the maximum would be missed. Find it, then replace "
    "it.",
    "// pass 1: find\n"
    "int maxv = a[0][0];\n"
    "for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)\n"
    "    if (a[r][c] > maxv) maxv = a[r][c];\n"
    "\n"
    "// pass 2: replace every occurrence\n"
    "for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++)\n"
    "    if (a[r][c] == maxv) a[r][c] = -1;\n"
    "return maxv;"
},
{
    "Borders without double counting",
    "A cell is on the border if it is in the first or last row OR the first or "
    "last column. Test all four conditions inside a single pass over every "
    "cell and each cell is visited once, so the count comes out right.\n\n"
    "Loop the two rows and then the two columns separately and the four "
    "corners get hit twice: you would report 20 replacements on a 4x6 grid "
    "instead of 16. The surviving interior is 2x4 = 8, and 24 - 8 = 16.",
    "for (int r = 0; r < 4; r++)\n"
    "    for (int c = 0; c < 6; c++)\n"
    "        if (r == 0 || r == 3 || c == 0 || c == 5) {\n"
    "            a[r][c] = -1;\n"
    "            n++;              // each cell counted exactly once\n"
    "        }"
},
{
    "Patterns and runs inside a row",
    "To look for a run of 3 cells in a row of 6, the column loop must stop "
    "early: c <= 6 - 3, i.e. c < 4. Reading a[r][c+2] with c = 5 runs off the "
    "end of the row.\n\n"
    "After you blank a match, skip past the cells you just wrote, or the -1s "
    "may themselves line up as a new pattern. The same bound idea scales: a "
    "run of length k in a row of n needs c <= n - k.",
    "for (int c = 0; c <= 6 - 3; c++) {\n"
    "    if (a[r][c]   == pattern[0] &&\n"
    "        a[r][c+1] == pattern[1] &&\n"
    "        a[r][c+2] == pattern[2]) {\n"
    "        a[r][c] = a[r][c+1] = a[r][c+2] = -1;\n"
    "        matches++;\n"
    "        c += 2;          // do not re-scan what we just blanked\n"
    "    }\n"
    "}"
},
{
    "Frequency arrays: use the value as the index",
    "Both CountUniqueValues and FindMostFrequent avoid searching entirely by "
    "using the value itself as an array index. Because the lab caps values at "
    "20, int freq[21] covers indexes 0..20 - note 21, not 20, or writing "
    "freq[20] is out of bounds.\n\n"
    "= {0} zeroes the whole array, which matters: an uninitialised tally "
    "contains garbage. Then 'have I seen this?' is one read and 'how many of "
    "these?' is one increment.\n\n"
    "Finding the most frequent is a plain maximum search over the tallies that "
    "returns the INDEX rather than the count. > keeps the first value on a tie; "
    ">= would keep the last.",
    "int freq[21] = {0};            // 21 slots: indexes 0..20\n"
    "for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++) {\n"
    "    int v = a[r][c];\n"
    "    if (v <= 0) continue;      // skips both 0 and -1\n"
    "    freq[v]++;\n"
    "}\n"
    "int best = 1;\n"
    "for (int v = 2; v <= 20; v++)\n"
    "    if (freq[v] > freq[best]) best = v;   // returns the value"
},
{
    "Shifting rows and falling values",
    "A cyclic row shift is rotateLeft mirrored, done once per row: save the "
    "element about to be lost, shift away from it, then put it back at the "
    "other end. Shifting RIGHT means the copy loop runs right to left.\n\n"
    "Gravity (FallDownAndReplace) works per COLUMN with a write index starting "
    "at the bottom row. Walk upwards; every cell that is not -1 is written at "
    "the write index, which then moves up. Whatever is left above gets the "
    "empty marker. That compaction pattern - read index and write index moving "
    "at different speeds - is worth recognising, it is how you remove items "
    "from an array in a single pass.",
    "// cyclic shift right, per row\n"
    "int last = a[r][5];\n"
    "for (int c = 5; c > 0; c--) a[r][c] = a[r][c-1];\n"
    "a[r][0] = last;\n"
    "\n"
    "// gravity, per column: compact downwards\n"
    "int write = 3;\n"
    "for (int r = 3; r >= 0; r--)\n"
    "    if (a[r][c] != -1) a[write--][c] = a[r][c];\n"
    "while (write >= 0) a[write--][c] = -2;"
},
};

static const Note kNotesLab3[] = {
{
    "What a pointer actually holds",
    "A pointer holds an ADDRESS. &x gives you the address of x; *p gives you "
    "the value stored at the address in p. Those two operators are inverses "
    "and nearly every pointer bug is a missing or extra one.\n\n"
    "Passing a pointer is how a function reaches out and changes the caller's "
    "variable. Inside swap_ints, tmp = *a reads the caller's value and *a = *b "
    "writes it. Drop the stars and you swap the function's own local pointer "
    "variables: it compiles, it runs, and the caller's x and y are unchanged.",
    "int x = 7;\n"
    "int *p = &x;      // p holds the ADDRESS of x\n"
    "*p = 11;          // writes through p: x is now 11\n"
    "\n"
    "void swap_ints(int *a, int *b) {\n"
    "    int tmp = *a;   // the VALUE at a\n"
    "    *a = *b;\n"
    "    *b = tmp;\n"
    "}"
},
{
    "Pointer arithmetic is in elements, not bytes",
    "arr + i advances by i whole elements - the compiler multiplies by "
    "sizeof(element) for you. So *(arr + i) is precisely what arr[i] compiles "
    "to, and the Lab 3 'pointers only' rule is a notation change, not a logic "
    "change.\n\n"
    "The last element is at arr + size - 1. arr + size is a legal pointer to "
    "FORM (it marks one past the end) but must never be dereferenced. "
    "Subtracting two pointers into the same array gives a COUNT of elements, "
    "which is why strlen is p - s with no adjustment.",
    "*(arr + i)        // identical to arr[i]\n"
    "arr + size - 1    // last element\n"
    "arr + size        // one PAST the end: never dereference\n"
    "\n"
    "int *L = arr, *R = arr + size - 1;\n"
    "while (L < R) {                  // pointers compare inside one array\n"
    "    int t = *L; *L = *R; *R = t;\n"
    "    L++; R--;\n"
    "}"
},
{
    "C strings are char arrays ending in '\\0'",
    "There is no length stored anywhere. A string is just characters followed "
    "by a zero byte, and every library function finds the end by looking for "
    "it. That is why while (*p) is the idiomatic loop: a non-zero character is "
    "true, the terminator is false and stops it.\n\n"
    "\"Hello, world!\" has length 13 but occupies 14 bytes. The length is "
    "p - s, not p - s + 1, because p stops ON the terminator rather than past "
    "it.",
    "char s[] = \"Hello\";   // 6 bytes: 'H','e','l','l','o','\\0'\n"
    "\n"
    "int strlen_ptr(char *s) {\n"
    "    char *p = s;\n"
    "    while (*p != '\\0') p++;   // or just while (*p)\n"
    "    return p - s;              // a count of elements\n"
    "}"
},
{
    "Write the terminator yourself",
    "copy_string and concat_strings both stop their copy loop AT the "
    "terminator, which means they never copy it. You have to add it by hand "
    "after the loop. Leave it out and the characters are all present but the "
    "string has no end, so the next read walks into whatever follows in "
    "memory.\n\n"
    "concat starts by walking dest to its existing terminator - that is "
    "exactly where the new text belongs, so the old '\\0' is overwritten by "
    "the first copied character and a fresh one is appended at the very end.",
    "void copy_string(char *dest, char *src) {\n"
    "    while (*src) { *dest = *src; dest++; src++; }\n"
    "    *dest = '\\0';            // the loop did NOT copy it\n"
    "}\n"
    "\n"
    "void concat_strings(char *dest, char *src) {\n"
    "    while (*dest) dest++;     // walk to the end of dest\n"
    "    while (*src) { *dest = *src; dest++; src++; }\n"
    "    *dest = '\\0';\n"
    "}"
},
{
    "Comparing strings returns a difference",
    "strcmp-style comparison walks both strings while the characters match, "
    "then returns the difference between the first pair that differs. It "
    "returns a NEGATIVE, ZERO or POSITIVE number - not necessarily -1, 0 or 1 "
    "- so correct code tests the sign and never compares the result to -1.\n\n"
    "\"apple\" against \"apples\": the walk stops with the first string on its "
    "terminator (0) and the second on 's' (115). 0 - 115 is negative, so a "
    "prefix always sorts before the longer string.",
    "int compare_strings(char *a, char *b) {\n"
    "    while (*a && (*a == *b)) { a++; b++; }\n"
    "    return (unsigned char)*a - (unsigned char)*b;\n"
    "}\n"
    "\n"
    "if (compare_strings(p, q) < 0) /* p sorts first */ ;   // test the SIGN"
},
{
    "Counting through a string",
    "count_vowels is the template for every 'count characters that...' "
    "question: one pointer walk, one condition, one counter. Remember both "
    "cases - testing only a-e-i-o-u on \"OpenAI rocks\" finds 2 instead of 5.",
    "int count_vowels(char *s) {\n"
    "    int n = 0;\n"
    "    for (char *p = s; *p; p++) {\n"
    "        char c = *p;\n"
    "        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||\n"
    "            c=='A'||c=='E'||c=='I'||c=='O'||c=='U') n++;\n"
    "    }\n"
    "    return n;\n"
    "}"
},
};

static const Note kNotesExam[] = {
{
    "The shape of the paper",
    "The paper has two sections and both are about reading code, not writing "
    "it from scratch.\n\n"
    "Section A - 'What is the output / value?'. You are given a snippet and "
    "you write down exactly what it prints. Marks go for exact output: the "
    "right values, in the right order, on the right lines.\n\n"
    "Section B - 'Fill in the blanks'. A correct function with pieces removed, "
    "usually an operator, a bound or a single expression. You supply the "
    "missing code.\n\n"
    "It is an open-book exam in a lab. You may use your own notes and the "
    "internet; you may not communicate with anyone. Sign out with your leave "
    "time, and do not touch your material after signing out.",
    NULL
},
{
    "How to answer Section A",
    "Trace, do not guess. Keep a small table of the variables on paper and "
    "update it line by line - the questions are specifically built so that the "
    "plausible-looking answer is wrong.\n\n"
    "Check these every time:\n"
    "  - Does the loop print a newline per row, or per element?\n"
    "  - Does the print loop run to count or to size? After an insert or "
    "delete, count has CHANGED.\n"
    "  - Did the function return on the first match, or keep going?\n"
    "  - Are 0 and -1 being skipped?\n"
    "  - bool prints as 1 or 0, never as true or false.\n\n"
    "For the grid questions, write the 4x6 out and edit it cell by cell. Then "
    "cross-check the returned count against the grid you just drew - if you "
    "replaced nine cells, the count had better be nine.",
    NULL
},
{
    "How to answer Section B",
    "The surrounding code tells you the answer. Read what the variables are "
    "called, what the loop is clearly trying to do, and what would break at "
    "the boundaries.\n\n"
    "The blanks are drawn from a small set:\n"
    "  - A comparison: <, <=, >, >=, ==, !=. Decide by asking what must "
    "happen at the very first and very last value.\n"
    "  - A bound: size - 1, count - 1, n - k.\n"
    "  - A step: ++, --, += 1, -= 1.\n"
    "  - A dereference or address: *p, *(arr + i), &x.\n"
    "  - A saved temporary being put back: first, last, tmp.\n\n"
    "Where an off-by-one is possible, test the extremes mentally: does d*d <= "
    "n still catch 25? Does L < R stop in the middle? Would arr + size read "
    "past the end?",
    NULL
},
{
    "Boundary cases that come up again and again",
    "These few decisions account for most of the marks lost:\n\n"
    "  - d*d <= n, not <. With < you report 25 as prime.\n"
    "  - L < R, not <=. With <= an even-length reverse swaps everything twice "
    "and ends up unchanged.\n"
    "  - Insert allows idx == count (append); delete does not.\n"
    "  - A run of 3 in a row of 6 stops at c <= 6 - 3.\n"
    "  - The last index is size - 1; arr + size is one past the end.\n"
    "  - int freq[21] for values up to 20.\n"
    "  - Save a value BEFORE a shift destroys it (rotate, cyclic shift).\n"
    "  - Write the '\\0' yourself after a copy loop.\n"
    "  - Test the SIGN of a string comparison, not == -1.",
    NULL
},
{
    "Comments and submission",
    "Comments carry marks and they are checked for understanding, not "
    "presence.\n\n"
    "  - Every file needs a header comment: your name, your student number, "
    "and what the file does.\n"
    "  - Comment anything non-trivial and anything that was not covered in "
    "class.\n"
    "  - Comment each block of code - what it is for, not a restatement of "
    "the syntax. 'increment i' earns nothing; 'walk downwards so no element is "
    "overwritten before it is copied' earns the mark.\n"
    "  - Write them in your own words.\n\n"
    "Also: indent consistently, name variables meaningfully, read the mark "
    "allocation on the paper before you start, and upload to the location the "
    "paper specifies.",
    NULL
},
};

const char *NoteSectionName(int section)
{
    switch (section) {
        case 0: return "Lab 1 - Core Logic & Arrays";
        case 1: return "Lab 2 - 2D Arrays";
        case 2: return "Lab 3 - Pointers & Strings";
        case 3: return "Exam Technique";
        default: return "?";
    }
}

const Note *NotesFor(int section, int *countOut)
{
    switch (section) {
        case 0: *countOut = (int)(sizeof kNotesLab1 / sizeof kNotesLab1[0]);
                return kNotesLab1;
        case 1: *countOut = (int)(sizeof kNotesLab2 / sizeof kNotesLab2[0]);
                return kNotesLab2;
        case 2: *countOut = (int)(sizeof kNotesLab3 / sizeof kNotesLab3[0]);
                return kNotesLab3;
        case 3: *countOut = (int)(sizeof kNotesExam / sizeof kNotesExam[0]);
                return kNotesExam;
        default: *countOut = 0; return NULL;
    }
}
