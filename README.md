# Intelligent Log Analyzer

A command-line **C program** that reads a log file, analyzes its
contents, and generates a summary report. It calculates general text
statistics, letter and digit frequencies, and the occurrence of selected
log keywords. The report is printed in the terminal and saved to
`report.txt`.

## Features

-   Reads a log file line by line.
-   Accepts the log-file path as a command-line argument or asks for it
    at runtime.
-   Calculates:
    -   Total lines
    -   Total words
    -   Total characters
    -   Alphabets, digits, special characters, and whitespace
-   Counts the frequency of each letter (`A`--`Z`), without
    distinguishing uppercase and lowercase.
-   Counts the frequency of each digit (`0`--`9`).
-   Identifies the most and least frequent letter and digit among those
    that occur.
-   Searches for these log keywords, without distinguishing uppercase
    and lowercase:
    -   `ERROR`
    -   `WARN`
    -   `INFO`
    -   `DEBUG`
    -   `FATAL`
    -   `FAIL`
    -   `SUCCESS`
-   Displays a simple status insight based on the `ERROR`, `FATAL`, and
    `WARN` counts.
-   Prints the report on screen and writes the same report to
    `report.txt`.

## Requirements

-   A C compiler such as GCC.
-   A terminal or command prompt.
-   A text log file to analyze.

## Project File

``` text
project-folder/
├── ist.c          # C source code
├── sample.log     # Example input log file (create your own)
└── report.txt     # Generated report after running the program
```

`report.txt` is created in the program's current working directory. It
is overwritten each time the program successfully creates a new report.

## Compilation

Open a terminal in the folder containing `ist.c` and compile it with
GCC.

### Linux / macOS

``` bash
gcc ist.c -o log_analyzer
```

### Windows (MinGW GCC)

``` bash
gcc ist.c -o log_analyzer.exe
```

## How to Run

### Option 1: Provide the file path as an argument

Linux / macOS:

``` bash
./log_analyzer sample.log
```

Windows:

``` powershell
.\log_analyzer.exe sample.log
```

Replace `sample.log` with the name or path of the log file you want to
analyze.

### Option 2: Enter the file path when prompted

Run the program without an argument:

Linux / macOS:

``` bash
./log_analyzer
```

Windows:

``` powershell
.\log_analyzer.exe
```

When prompted, enter the log-file path.

> **Note:** The interactive input uses `%s`, so it accepts a path
> without spaces. For a path containing spaces, pass it as a quoted
> command-line argument, for example `".\logs\server log.txt"`.

## Example Input

Create a text file named `sample.log`:

``` text
INFO Server started successfully
WARN Memory usage is high
ERROR Unable to connect to database
DEBUG Retrying connection
FATAL Service stopped
```

Run the program:

``` bash
./log_analyzer sample.log
```

The program displays a report with general statistics, alphabet
frequency, digit frequency, most/least frequent characters, keyword
counts, and an insight. It also saves the report to `report.txt`.

The exact counts depend on the contents of the input file.

## How It Works

1.  **Input selection:** The program reads the filename from `argv[1]`;
    if no argument is supplied, it prompts for a filename.
2.  **File opening:** It opens the selected file in read mode. If
    opening fails, it displays an error and exits.
3.  **Line-by-line reading:** `fgets()` reads the file in chunks using a
    buffer of `MAX_LINE` (4096) bytes.
4.  **Character analysis:** Each character is classified using functions
    from `<ctype.h>`:
    -   `isalpha()` for letters
    -   `isdigit()` for digits
    -   `isspace()` for whitespace
    -   Other characters are counted as special characters.
5.  **Word counting:** A word is counted when a non-whitespace sequence
    begins.
6.  **Frequency analysis:** Arrays store the counts of the 26 letters
    and 10 digits.
7.  **Keyword analysis:** `count_word()` compares each keyword against
    the line without case sensitivity and adds the occurrences.
8.  **Report generation:** `print_report()` formats the report. It is
    called once with `stdout` and again with the output file stream for
    `report.txt`.

## Main Functions and Data Structures

  -----------------------------------------------------------------------
  Function / item                     Purpose
  ----------------------------------- -----------------------------------
  `count_word()`                      Counts occurrences of a keyword in
                                      a line, ignoring letter case.

  `print_report()`                    Prints the complete analysis report
                                      to a supplied output stream.

  `main()`                            Handles input, reads and analyzes
                                      the log file, and saves the report.

  `KEYWORDS[]`                        Stores the seven keywords to search
                                      for.

  `alpha[26]`                         Stores the frequency of each
                                      alphabet letter.

  `dig[10]`                           Stores the frequency of each
                                      decimal digit.

  `keyword_count[]`                   Stores the occurrence count for
                                      each keyword.

  `MAX_LINE`                          Sets the input buffer size to 4096
                                      bytes.
  -----------------------------------------------------------------------

## Understanding the Insight

The program applies the following priority:

1.  If at least one `ERROR` or `FATAL` occurrence is found, it reports:
    **"Errors/fatal events detected - review the log immediately."**
2.  Otherwise, if at least one `WARN` occurrence is found, it reports:
    **"Warnings found - system is running but needs attention."**
3.  Otherwise, it reports: **"No errors or warnings detected - log looks
    healthy."**

This is a basic rule-based insight, not an automated diagnosis of the
system.

## Important Counting Details

-   Letter frequencies and keyword matching are case-insensitive.
-   Keyword matching counts matching text occurrences, including
    occurrences inside a larger word; it does not require a whole-word
    match.
-   A word is defined by this program as a continuous sequence of
    non-whitespace characters.
-   The character total includes every character read, including newline
    characters.
-   The line counter counts newline-terminated lines and a final
    non-empty text chunk when it reaches end-of-file.
-   The most/least frequent letter and digit are selected only from
    characters that appear in the input.
-   A line longer than the 4096-byte buffer is processed in multiple
    chunks. The program reads those chunks safely, but its line counter
    is based on newline-terminated chunks and the final end-of-file
    chunk.

## Error Handling

-   If the user does not provide a valid filename at the prompt, the
    program displays `Invalid input.` and exits.
-   If the input file cannot be opened, `perror()` displays the
    file-opening error and the program exits.
-   If `report.txt` cannot be created, the program displays
    `Could not create report.txt`.
-   If the input file opens successfully, the analysis report is printed
    to the terminal.

## Technologies Used

-   **C programming language**
-   **Standard C libraries:** `<stdio.h>`, `<string.h>`, and `<ctype.h>`
-   **GCC** (for compilation)

## Limitations

-   The program analyzes text files; it is not a parser for a particular
    logging format.
-   The keyword list is fixed in the source code.
-   Keyword matching is substring-based rather than whole-word-based.
-   The status insight uses only the `ERROR`, `FATAL`, and `WARN`
    counts.
-   The interactive filename prompt does not handle paths containing
    spaces; use a quoted command-line argument for those paths.

## Possible Future Enhancements

-   Allow users to configure keywords without changing the source code.
-   Support paths containing spaces in interactive input.
-   Add date/time and log-level parsing for structured log formats.
-   Provide filtering by keyword or severity.
-   Export reports in CSV or HTML format.
-   Improve line counting for very long lines that span multiple
    input-buffer chunks.

## Author

Add your name, course, department, and institution here if required by
your project submission format.
