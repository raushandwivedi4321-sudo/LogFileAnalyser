/*
 * Intelligent Log Analyzer - String and Digit Frequency Analysis
 *
 * Compile:  gcc log_analyzer.c -o log_analyzer
 * Run:      ./log_analyzer sample.log          (or just ./log_analyzer and type the path)
 * Output:   prints a report on screen and saves it to report.txt
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

/* Log keywords to look for (case-insensitive) */
static const char *KEYWORDS[] = {"ERROR", "WARN", "INFO", "DEBUG", "FATAL", "FAIL", "SUCCESS"};
#define NUM_KEYWORDS (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]))

/* Counts occurrences of 'word' in 'line', ignoring case */
int count_word(const char *line, const char *word)
{
    int count = 0;
    size_t wlen = strlen(word);
    size_t llen = strlen(line);

    for (size_t i = 0; i + wlen <= llen; i++) {
        size_t j = 0;
        while (j < wlen && toupper((unsigned char)line[i + j]) == word[j])
            j++;
        if (j == wlen)
            count++;
    }
    return count;
}

/* Writes the whole report to the given stream (screen or file) */
void print_report(FILE *out, const char *filename,
                  long lines, long words, long total_chars,
                  long letters, long digits, long specials, long spaces,
                  const long alpha[26], const long dig[10],
                  const long keyword_count[])
{
    int i;
    int max_a = -1, min_a = -1, max_d = -1, min_d = -1;

    /* Find most/least frequent letter and digit (only among those that appear) */
    for (i = 0; i < 26; i++) {
        if (alpha[i] == 0) continue;
        if (max_a == -1 || alpha[i] > alpha[max_a]) max_a = i;
        if (min_a == -1 || alpha[i] < alpha[min_a]) min_a = i;
    }
    for (i = 0; i < 10; i++) {
        if (dig[i] == 0) continue;
        if (max_d == -1 || dig[i] > dig[max_d]) max_d = i;
        if (min_d == -1 || dig[i] < dig[min_d]) min_d = i;
    }

    fprintf(out, "==============================================\n");
    fprintf(out, "        INTELLIGENT LOG ANALYZER REPORT\n");
    fprintf(out, "==============================================\n");
    fprintf(out, "File analysed : %s\n\n", filename);

    fprintf(out, "--- General Statistics ---\n");
    fprintf(out, "Total lines        : %ld\n", lines);
    fprintf(out, "Total words        : %ld\n", words);
    fprintf(out, "Total characters   : %ld\n", total_chars);
    fprintf(out, "Alphabets          : %ld\n", letters);
    fprintf(out, "Digits             : %ld\n", digits);
    fprintf(out, "Special characters : %ld\n", specials);
    fprintf(out, "Whitespace         : %ld\n\n", spaces);

    fprintf(out, "--- Alphabet Frequency (case-insensitive) ---\n");
    for (i = 0; i < 26; i++)
        if (alpha[i] > 0)
            fprintf(out, "  %c : %ld\n", 'A' + i, alpha[i]);

    fprintf(out, "\n--- Digit Frequency ---\n");
    for (i = 0; i < 10; i++)
        fprintf(out, "  %d : %ld\n", i, dig[i]);

    fprintf(out, "\n--- Most / Least Frequent ---\n");
    if (max_a != -1) {
        fprintf(out, "Most frequent letter  : %c (%ld times)\n", 'A' + max_a, alpha[max_a]);
        fprintf(out, "Least frequent letter : %c (%ld times)\n", 'A' + min_a, alpha[min_a]);
    } else {
        fprintf(out, "No letters found.\n");
    }
    if (max_d != -1) {
        fprintf(out, "Most frequent digit   : %d (%ld times)\n", max_d, dig[max_d]);
        fprintf(out, "Least frequent digit  : %d (%ld times)\n", min_d, dig[min_d]);
    } else {
        fprintf(out, "No digits found.\n");
    }

    fprintf(out, "\n--- Log Keyword Summary ---\n");
    for (i = 0; i < (int)NUM_KEYWORDS; i++)
        fprintf(out, "  %-8s : %ld\n", KEYWORDS[i], keyword_count[i]);

    /* Simple "intelligent" insight based on the counts */
    fprintf(out, "\n--- Insight ---\n");
    if (keyword_count[0] + keyword_count[4] > 0)
        fprintf(out, "Errors/fatal events detected - review the log immediately.\n");
    else if (keyword_count[1] > 0)
        fprintf(out, "Warnings found - system is running but needs attention.\n");
    else
        fprintf(out, "No errors or warnings detected - log looks healthy.\n");
    fprintf(out, "==============================================\n");
}

int main(int argc, char *argv[])
{
    char filename[256];
    char line[MAX_LINE];
    long alpha[26] = {0}, dig[10] = {0};
    long keyword_count[NUM_KEYWORDS] = {0};
    long lines = 0, words = 0, total_chars = 0;
    long letters = 0, digits = 0, specials = 0, spaces = 0;
    int in_word = 0;
    FILE *fp, *rep;

    /* Get file name from command line or ask the user */
    if (argc >= 2) {
        strncpy(filename, argv[1], sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
    } else {
        printf("Enter log file path: ");
        if (scanf("%255s", filename) != 1) {
            printf("Invalid input.\n");
            return 1;
        }
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    /* Read the file line by line so large files are handled safely */
    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t len = strlen(line);

        /* Count a line only when it ends with newline, or it is the last text chunk */
        if (len > 0 && line[len - 1] == '\n')
            lines++;
        else if (len > 0 && feof(fp))
            lines++;

        for (size_t i = 0; i < len; i++) {
            unsigned char c = (unsigned char)line[i];
            total_chars++;

            if (isalpha(c)) {
                alpha[toupper(c) - 'A']++;
                letters++;
            } else if (isdigit(c)) {
                dig[c - '0']++;
                digits++;
            } else if (isspace(c)) {
                spaces++;
            } else {
                specials++;
            }

            /* Word counting: a word is a run of non-space characters */
            if (isspace(c)) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }

        for (int k = 0; k < (int)NUM_KEYWORDS; k++)
            keyword_count[k] += count_word(line, KEYWORDS[k]);
    }
    fclose(fp);

    /* Print to screen */
    print_report(stdout, filename, lines, words, total_chars,
                 letters, digits, specials, spaces, alpha, dig, keyword_count);

    /* Save to report.txt */
    rep = fopen("report.txt", "w");
    if (rep != NULL) {
        print_report(rep, filename, lines, words, total_chars,
                     letters, digits, specials, spaces, alpha, dig, keyword_count);
        fclose(rep);
        printf("\nReport saved to report.txt\n");
    } else {
        printf("\nCould not create report.txt\n");
    }

    return 0;
}