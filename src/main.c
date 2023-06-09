// Last Change: 2023-03-27  Monday: 07:54:40 PM

/*
  Licence: GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007
*/

/*
  ChatGPT: This program converts a Mozilla Firefox bookmarks-HTML file
  to a CSV file format that is accepted by KeePassXC.
  The output CSV file contains two columns: "Title" and "URL".
  The program can handle multi-level bookmarks and supports both
  MS Windows and Linux operating systems.
*/

/*
  Here's a sample HTML bookmarks file exported by
  Mozilla Firefox 111.0.1 Windows x64.

  ```
  <DL><p>
    <DT><H3 ADD_DATE="1661171785" LAST_MODIFIED="1679567379">Mozilla Firefox</H3>
    <DL><p>
        <DT><A HREF="https://support.mozilla.org/products/firefox" ADD_DATE="1661171785" LAST_MODIFIED="1679567379">Get Help</A>
        <DT><A HREF="https://support.mozilla.org/kb/customize-firefox-controls-buttons-and-toolbars?utm_source=firefox-browser&utm_medium=default-bookmarks&utm_campaign=customize" ADD_DATE="1661171785" LAST_MODIFIED="1679567379">Customize Firefox</A>
        <DT><A HREF="https://www.mozilla.org/contribute/" ADD_DATE="1661171785" LAST_MODIFIED="1679567379">Get Involved</A>
        <DT><A HREF="https://www.mozilla.org/about/" ADD_DATE="1661171785" LAST_MODIFIED="1679567379">About Us</A>
    </DL><p>
    ...
  ```

  KeepassXC Password Manager (https://keepassxc.org/) can be used
  for bookmarks management, too.

  KeePassXC imports password/bookmark databases
  in CSV (another Spreadsheet-like text-based) format.
  We will have to import the DB and convert it
  to KeePassXC's native *.kdbx format
  before we can use it.

  Here's a sample CSV file that KeePassXC can accept.

  ```
  "Group","Title","Username","Password","URL","Notes","TOTP","Icon","Last Modified","Created"
  "Root","firefox addons","","",https://addons.mozilla.org/en-US/firefox/,"","","0","2023-03-23T11:24:26Z","2023-03-23T11:24:18Z"
  "Root/group","chatgpt","","",https://chat.openai.com/chat,"","","0","2023-03-23T11:24:43Z","2023-03-23T11:24:36Z"
  "Root/group/sub-group","g calender","","",https://calendar.google.com/calendar,"","","0","2023-03-23T11:25:01Z","2023-03-23T11:24:52Z"
  ```

  By now, we know the pattern in which KeePassXC keeps
  the database elements arranged.

  So, we now have narrowed down our focus on
  the converted CSV file if it has to be generated from
  the given HTML sample input as follows:

  ```
  "Group","Title","Username","Password","URL","Notes","TOTP","Icon","Last Modified","Created"
  "Mozilla Firefox","Get Help","","",https://support.mozilla.org/products/firefox,
  "Mozilla Firefox","Customize Firefox","","",https://support.mozilla.org/kb/customize-firefox-controls-buttons-and-toolbars?utm_source=firefox-browser&utm_medium=default-bookmarks&utm_campaign=customize,
  "Mozilla Firefox","Get Involved","","",https://www.mozilla.org/contribute/,
  "Mozilla Firefox","About Us","","",https://www.mozilla.org/about/,
  ```

  It's time to start looking through our code.
*/

/*
  Mostly generated by ChatGPT on 2023/MAR/23.
  https://chat.openai.com/
  Manual intervention and modification to the code
  were performed frequently to achieve
  the code's intended goal.
  Most code explanations under comments
  have been generated by Codeium-AI's Vim plugin.
  https://codeium.com/download
*/

/*
  Usage:
  gcc -Wall -Wextra -O2 main.c -o html2csv -s
  chmod +x html2csv
  or (MS Windows),
  gcc -Wall -Wextra -O2 main.c -o html2csv.exe -s
  or,
  run the CMake script:
  mkdir build
  cd build
  cmake -G "GNU Makefiles" ..
  cmake -G "MinGW Makefiles" ..
  make -j4
  or,
  mingw32-make -j4
  Run the program:
  html2csv bookmarks.html bookmarks.csv
*/

#include <stdio.h> // General input/output functions
#include <stdlib.h> // General memory functions
#include <string.h> // String manipulation functions

#define VERSION "1.0" // defines a constant string called "VERSION" with the value "1.0"
#define MAX_LINE_SIZE 40000 // Maximum line size that is allowed to be read.

char output_str[MAX_LINE_SIZE]; // Global variable to store and manipulate the output string.

int main(int argc, char *argv[]) { // Main function. argc means the number of arguments. argv[0] is the program name. argv[1] is the first argument. argv[2] is the second argument.
  if(argc == 2 && strcmp(argv[1], "--version") == 0) {  // checks if the program was called with the argument "--version". If it was, it prints out the value of the "VERSION" constant and returns 0.
    printf("%s\n", VERSION);
    return 0;
  }

  if(argc != 3) {
    /* checks if the program was called with exactly two arguments. */
    /* Checks if the number of arguments is correct (3).
                       1. prog. name 2. input 3. output */
    printf("Usage: html2csv input_file.html output_file.csv\n"); // Prints how to use the program on the screen.
    return 1; /* Exit if the no. of arguments supplied is not 3 */
  }

  char *input_file = argv[1]; // Input file name
  char *output_file = argv[2];  // Output file name
  FILE *fp_in = fopen(input_file, "r"); // Open input file in readonly mode

  if(fp_in == NULL) { // Checks if the input file is opened successfully
    printf("Error: could not open input file %s\n", input_file); // Prints error message if the input file is not opened
    return 1; // Exit if the input file is not opened
  }

  FILE *fp_out = fopen(output_file, "w"); // Open output file in write mode

  if(fp_out == NULL) { // Checks if the output file is opened successfully
    printf("Error: could not open output file %s\n", output_file); // Prints error message if the output file is not opened
    return 1; // Exit if the output file is not opened
  }

  char line[MAX_LINE_SIZE]; // Line buffer
  char *ptr, *group; // Pointer to line buffer and group name
  // Write header row to output file
  fprintf(fp_out, "\"Group\",\"Title\",\"Username\",\"Password\",\"URL\",\"Notes\",\"TOTP\",\"Icon\",\"Last Modified\",\"Created\"\n"); // Prints the CSV header row to output file

  while(fgets(line, MAX_LINE_SIZE, fp_in) != NULL) { // Reads the input file line by line
    // Find start of group
    ptr = strstr(line, "<H3 ");

    if(ptr != NULL) { // Checks if the group is opened til the end
      // Find end of group
      char *end = strstr(ptr, "</H3>");

      if(end != NULL) { // Checks if the group is not closed before extracting the group name
        // Extract group name
        group = ptr + strlen("<H3 ");
        char *group_end = end - 1; // Finds the end of the group name

        if(group == NULL || group_end == NULL) { // Checks if the group name is extracted successfully
          continue; // Skips the current line and restarts the loop
        }

        // Write to output file
        //sprintf(output_str, "\"%.*s\",\n", (int)(group_end - group + 1), group);
        sprintf(output_str, "\"%.*s\",", (int)(group_end - group + 1), group);  // Converts the group name to a string. Example: "Mozilla Firefox (folder gr. name)"
        //printf("%s", output_str);
        // Find the position of '>' character
        char *pos = strchr(output_str, '>');  // Find the first occurrence of '>'

        if(pos != NULL) { // Checks if the '>' character is found
          pos++;  // Move the pointer past '>'. Explanation: ">Mozilla Firefox</H3> (folder gr. name) to be changed to "Mozilla Firefox". First, we have to pick the position of the '>' character.
          /*
            The reason for moving the pointer pos past the > character is to remove
            the leading > character from the group name.

            In the HTML file, the group name is enclosed in a <H3> tag with
            an opening > character, like this: <H3>Mozilla Firefox</H3>.
            When we extract the group name
            using
            sprintf(output_str, "\"%.*s\",", (int)(group_end - group + 1), group);,
            the resulting output_str string will include
            the leading > character.

            To remove the > character and get only the group name,
            we find the position of the > character using
            char *pos = strchr(output_str, '>');
            and then move the pos pointer
            past the > character using pos++;.
            This way, we get the group name
            without the leading > character, which we can then
            use in the output CSV file.

            But, it was not under a direct loop!

            It's correct that it wasn't under a direct loop.
            However, the pos variable is being updated
            within the while loop that processes
            each line of the input file.

            Specifically, when the ptr variable is
            pointing to the start of
            an anchor tag (<A HREF="...">),
            the pos variable is set to
            the position of the first > character
            that appears in output_str, which is
            the string that was generated in the
            previous iteration of the while loop
            when the group variable was processed.

            Since pos points to the > character
            in output_str, incrementing pos by 1 moves the pointer
            past the > character, effectively skipping over it.
            This is necessary because we want to extract only
            the text between the <H3> and </H3> tags,
            not including the tags themselves.
            By moving the pointer past the > character, we skip over
            the opening tag and start extracting the text
            from the first character of the group name.
          */
          //printf("%s\n", pos);  // Output: Mozilla Firefox (folder gr. name)
          strcpy(output_str, pos); // Copy the group name. Reminder: strcpy(dest, source).
          /* In the next line, strcpn() function is used to find the position of the '\n' character. */
          /* The strcspn function returns the
            length of the initial segment of
            a string that consists entirely
            of characters not in a specified set.
            In this case, the specified set is
            the newline character "\n".

            So, strcspn(output_str, "\n") returns
            the number of characters
            in output_str up until the first occurrence
            of the newline character.
            If there is no newline character,
            strcspn returns the length of the entire string.

            By assigning the null character '\0' to
            the position in output_str where
            the newline character was found (i.e.,
            output_str[strcspn(output_str, "\n")] = '\0';),
            we essentially truncate output_str at that point,
            effectively removing the trailing newline character.

            This step is necessary because when we read in lines
            from the input file using functions
            like fgets, fprintf, sprintf etc., the newline character
            is included at the end of the line.
            We don't want this newline character to be present
            in the output CSV file, so we remove it
            using strcspn and the null character.

            To simply put, we want to remove
            the trailing newline character from the output string
            before using the string in the last stage
            where we take the final output.

            output_str[strcspn(output_str, "\n")] means that
            the position of the '\n' character is found.
            It (the position where the '\n' is found) is
            assigned to '\0' because we want to truncate
            the string without any extra '\n'-like enitity into it.
          */
          output_str[strcspn(output_str, "\n")] = '\0'; // Remove trailing newline character
          //printf("%s\n", output_str);  // Output: Mozilla Firefox (folder gr. name)
        }
      }
    }

    // Find start of anchor tag
    ptr = strstr(line, "HREF=\""); // Finds the start of the anchor tag

    if(ptr != NULL) { // Checks if the anchor tag is opened til the end
      // Find end of anchor tag
      char *end = strstr(ptr, "</A>");

      if(end != NULL) { // Checks if the anchor tag is not closed before extracting the URL
        // Extract name and URL
        char *name_start = strchr(ptr, '>') + 1; // Finds the start of the name
        char *name_end = end - 1; // Finds the end of the name. It is (end -1) because the '>' character is not included
        //char *url_start = ptr + strlen("<A HREF=\"");
        char *url_start = ptr + strlen("HREF=\""); // Finds the start of the URL
        char *url_end = strchr(url_start, '\"'); // Finds the end of the URL
        // Write to output file
        fprintf(fp_out, "\"%s\"%.*s\",\"\",\"\",%.*s,\n", output_str,
                (int)(name_end - name_start + 1), name_start,
                (int)(url_end - url_start), url_start); // Converts the name and URL to a string, writes the same to the output file in order: "Group","Title","Username","Password","URL"
        // Note that \" means " in the output file. %.*s means the number of characters to be printed.
      }
    }
  }

  fclose(fp_in); // Close input file
  fclose(fp_out); // Close output file
  return 0; // Return exit code 0 if everything went fine
}

