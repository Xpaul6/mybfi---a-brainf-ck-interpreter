#include <stdio.h>
#include <stdlib.h>

char* InterpretBF(char* program, int programLength) {

    // Matching loop brackets check
    int bracketDiff = 0;
    for (int i = 0; i < programLength; i++) {
        if (program[i] == '[') { bracketDiff++; }
        else if (program[i] == ']') { bracketDiff--; }
    }
    if (bracketDiff != 0) {
        return "Unmatched bracket detected, check if provided code is valid, aborting...";
    }

    // Variables definition
    int outputLength = 0; // Output buffer length
    char* output = (char*)malloc(sizeof(char)); // Output buffer 
    if (output == NULL) {
        return "Unable to allocate memory for output buffer, aborting...";
    }
    unsigned char* tape = (unsigned char*)malloc(30000); // "Array" of zeros with length of 30000
    if (tape == NULL) {
        free(output);
        return "Unable to allocate memory for the tape, aborting...";
    }
    int tapeHeadPosition = 0; // Tape head position index
    int programHeadPosition = 0; // Program poiner position index
    long iterationLimit = 10000000; // Hardcoded iteration limit that prevents infinite loops
    long iterationCounter = 0; // Interpreter's iterations counter

    // Main interpreter loop
    while (programHeadPosition < programLength) {

        // Infinite loop detection
        if (++iterationCounter > iterationLimit) {
            free(tape);
            free(output);
            return "Infinite loop detected, check if provided code is valid";
        }

        // Main switch case
        switch (program[programHeadPosition]) {
            case '+': // Increasing current cell by 1
                tape[tapeHeadPosition]++;
                break;
            case '-': // Decreasing currrent cell by 1
                tape[tapeHeadPosition]--;
                break;
            case '>': // Moving tape head right
                if (tapeHeadPosition >= 29999) {
                    free(tape);
                    free(output);
                    return "Tape length exceeded, check if provided code is valid, aborting...";
                }
                tapeHeadPosition++;
                break;
            case '<': // Moving tape head left
                if (tapeHeadPosition <= 0) {
                    free(tape);
                    free(output);
                    return "Tape head was moved lower than 0, check if provided code is valid, aborting...";
                }
                tapeHeadPosition--;
                break;
            case '.': // Wrinting current symbol(from ASCII code in the cell) to the output buffer
                outputLength++;
                char* tempBuf = (char*)realloc(output, outputLength * sizeof(char)); // New buffer is longer by 1 symbol
                if (tempBuf == NULL) {
                    free(output);
                    free(tape);
                    return "Unable to allocate memory, aborting...";
                }
                output = tempBuf; // Updating actual buffer location
                output[outputLength - 1] = (char)tape[tapeHeadPosition]; // Converting ASCII code into its symbol
                break;
            case ',': // Writing the folowing symbol(its ASCII code) into the cell
                tape[tapeHeadPosition] = (int)program[programHeadPosition + 1]; // Converting symbol into its ASCII representation
                programHeadPosition++;
                break;
            case '[': // Loop block start
                if (tape[tapeHeadPosition] == 0) { // Loop ends if current cell is equal to 0
                    int loopCounter = 1; // Nested loops counter
                    while (loopCounter > 0) {
                        programHeadPosition++; // Program pointer head moves forward until it reaches its corresponding bracket
                        if (program[programHeadPosition] == '[') {
                            loopCounter++;
                        } else if (program[programHeadPosition] == ']') {
                            loopCounter--;
                        }
                    }
                }
                break;
            case ']': // Loop block end
                if (tape[tapeHeadPosition] != 0) { // Loop breaks if current cell is equal to 0
                    int loopCounter = 1; // Nested loops counter
                    while (loopCounter > 0) {
                        programHeadPosition--; // Program pointer head moves forward until it reaches its corresponding bracket
                        if (program[programHeadPosition] == ']') {
                            loopCounter++;
                        } else if (program[programHeadPosition] == '[') {
                            loopCounter--;
                        }
                    }
                }
                break; 
        }

        // Moving to the next instruction 
        programHeadPosition++;
    }

    free(tape); 
    return output; 
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid arguments, aborting...\nmybfi usage: mybfi <filename>\n");
        return 1;
    }

    // File opening
    FILE* fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        perror("Unable to open this file, aborting...\n");
        return 1;
    }

    // Determening provided program's length
    fseek(fptr, 0, SEEK_END);
    long filesize = ftell(fptr); // Filsize in bytes = length of the program
    if (filesize < 0) {
        perror("Unable to determine file size, aborting...\n");
        fclose(fptr);
        return 1;
    }
    rewind(fptr); // Returns file pointer to the initial position

    // Writing provided program to a buffer
    char* programmBuffer = (char*)malloc(filesize);
    if (programmBuffer == NULL) {
        perror("Unable to allocate memory, aborting...\n");
        fclose(fptr);
        return 1;
    }
    fread(programmBuffer, 1, filesize, fptr);

    // File closing
    fclose(fptr);

    // Interpreter call
    char* result = InterpretBF(programmBuffer, filesize);
    printf("%s\n", result);

    free(result);
    free(programmBuffer);

    return 0;
}
