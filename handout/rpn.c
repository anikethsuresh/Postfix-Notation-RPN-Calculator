/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "token.h"
#include "hash.h"

/* Functions I created*/
void parse_token_type_operator(Symtab *symtab, Stack_head* stack, Token* tok);
int parse_token_type_assignment(Symtab *symtab, Stack_head* stack, Token* tok);
void parse_token_type_print(Symtab *symtab, Stack_head* stack);

/* Local Function Declarations (that you need to finish implementing) */
static int read_file(char *filename, char *line);
static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok);

/* Pre-Written Local Function Declarations */
static void print_header(char *filename, int step);
static void print_step_header(int step);
static void print_step_footer(Symtab *symtab, Stack_head *stack);
static void print_step_output(int val);

/* Defines the largest line that can be read from a file */
#define MAX_LINE_LEN 255

/* This has been written for you.
 * Main function to run your program (written for you).
 * 1) Opens the file using the passed in filename. (your function)
 * -- If the file is not found (ie. fopen returns NULL), then exit(-1);
 * 2) Reads one line from the file.
 * -- The programs will all only be one line in size.
 * -- That line may be up to MAX_LINE_LEN long.
 * 3) Closes the file.
 * 4) Calls token_read_line(line, strlen(line))
 * -- This parses the line and prepares the tokens to be ready to get.
 * 5) While there are tokens remaining to parse: token_has_next() != 0
 * -- a) Get the next token: token_get_next()
 * 6) Parse the token (your function)
 * 7) Print out all of the relevant information
 */
int rpn(Stack_head *stack, Symtab *symtab, char *filename) {
  int step = 0; /* Used to track the program steps */
  int ret = 0;
  char line[MAX_LINE_LEN];
  Token *tok = NULL;

  /* Complete the read_file function that is defined later in this file. */
  ret = read_file(filename, line);
  if(ret != 0) {
    printf("Error: Cannot Read File %s.  Exiting\n", filename);
    exit(-1);
  }

  /* Pass the line into the tokenizer to initialize that system */
  token_read_line(line, strlen(line));

  /* Prints out the nice program output header */
  print_header(filename, step);

  /* Iterate through all tokens */
  while(token_has_next()) {
    /* Begin the next step of execution and print out the step header */
    step++; /* Begin the next step of execution */
    print_step_header(step);

    /* Get the next token */
    tok = token_get_next();
    /* Complete the implementation of this function later in this file. */
    ret = parse_token(symtab, stack, tok);
    if(ret != 0) {
      printf("Critical Error in Parsing.  Exiting Program!\n");
      exit(-1);
    }

    /* Prints out the end of step information */
    print_step_footer(symtab, stack);
  }

  return 0;
}

/* (IMPLEMENT THIS FUNCTION)
 * Local function to open a file or exit.
 * Follow the Directions in the Project Documentation for this Function
 * Open filename, read its contents (up to MAX_LINE_LEN) into line, then
 *   close the file and return 0.
 * On any file error, return -1.
 */
static int read_file(char *filename, char *line) {
  FILE* fptr = fopen(filename,"r");
  if(fptr==NULL){
    return -1;
  }
  char* lineCheckFail = fgets(line, MAX_LINE_LEN,fptr);
  if(lineCheckFail!=NULL){
    fclose(fptr);
    return 0;
  }
  /* Implement This Function */
  return -1;
}

/* (IMPLEMENT THIS FUNCTION)
 * Parses the Token to implement the rpn calculator features
 * Follow the Directions in the Project Documentation for this Function
 * You may implement this how you like, but many small functions would be good!
 * If the token you are passed in is NULL, return -1.
 * If there are any memory errors, return -1.
 */
static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok) {
  if(tok==NULL || symtab == NULL || stack == NULL){
    return -1;
  }
  if(tok->type == TYPE_VALUE){
    // Push the token on to the stack.
    stack_push(stack, tok);
  }
  else if(tok->type == TYPE_VARIABLE){
    // Push the token on to the stack.
    stack_push(stack, tok);
  }
  else if(tok->type == TYPE_OPERATOR){
    // Pop two tokens off the stack, perform the operation on them, push the result.
    parse_token_type_operator(symtab,stack,tok);
  }
  else if(tok->type == TYPE_ASSIGNMENT){
    // Pop two tokens off the stack (value and variable), put it in the symbol table.
    int result = parse_token_type_assignment(symtab,stack,tok);
    if(result == -1){
      return -1;
    }
  }
  else if(tok->type == TYPE_PRINT){
    // Pop one token off the stack and print it to the screen as output.
    parse_token_type_print(symtab, stack);
    token_free(tok);
  }

  /* Implement This Function */
  return 0;
}

void parse_token_type_operator(Symtab *symtab, Stack_head* stack, Token* tok){
  // TYPE_OPERATOR = +,-,*,/
  Token* token2token = stack_pop(stack);
  Token* token1token = stack_pop(stack);
  int token1 = token1token->value;
  int token2 = token2token->value;
  if(token2token->type == TYPE_VARIABLE){
    Symbol* token2symbol = hash_get(symtab, token2token ->variable);
    Token* placeHolderToken = token_create_value(token2symbol->val);
    token2 = placeHolderToken->value;
    symbol_free(token2symbol);
    token_free(placeHolderToken);
  }
  if(token1token->type == TYPE_VARIABLE){
    Symbol* token1symbol = hash_get(symtab, token1token ->variable);
    Token* placeHolderToken = token_create_value(token1symbol->val);
    token1 = placeHolderToken->value;
    symbol_free(token1symbol);
    token_free(placeHolderToken);
  }
  int result = 0;
  switch(tok->oper) {
      case OPERATOR_PLUS:
        result = token1 + token2;
        break;
      case OPERATOR_MINUS: 
        result = token1 - token2;
        break;
      case OPERATOR_MULT: 
        result = token1 * token2;
        break;
      case OPERATOR_DIV: 
        result = token1 / token2;
        break;
  }
  stack_push(stack, token_create_value(result));
  token_free(token1token);
  token_free(token2token);
  token_free(tok);
}

void parse_token_type_print(Symtab *symtab, Stack_head* stack){
  Token* token = stack_pop(stack);
  int tokenValue = token->value;
  if(token->type == TYPE_VARIABLE){
    Symbol* tokenSymbol = hash_get(symtab, token ->variable);
    Token* placeHolderToken = token_create_value(tokenSymbol->val);
    tokenValue = placeHolderToken->value;
    token_free(placeHolderToken);
    symbol_free(tokenSymbol);
  }
   print_step_output(tokenValue);
   token_free(token);
}

int parse_token_type_assignment(Symtab *symtab, Stack_head* stack, Token* tok){
  Token* tokenValue = stack_pop(stack);
  Token* tokenVariable = stack_pop(stack);
  if(tokenValue->type == TYPE_VARIABLE){
    Symbol* tokenSymbol = hash_get(symtab, tok ->variable);
    tokenVariable = token_create_value(tokenSymbol->val);
    symbol_free(tokenSymbol);
  }
  token_free(tok);
  int inttokVal = tokenValue->value;
  char chartokVar[MAX_LINE_LEN];
  strncpy(chartokVar, tokenVariable->variable, MAX_LINE_LEN);
  token_free(tokenVariable);
  token_free(tokenValue);
  return hash_put(symtab, chartokVar, inttokVal);
}


/* This has been written for you.
 * Prints out the main output header
 */
static void print_header(char *filename, int step) {
  printf("######### Beginning Program (%s) ###########\n", filename);
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step);
  token_print_remaining();
  printf("o-------------------\n");
}

/* This has been written for you.
 * Prints out the information at the top of each step
 */
static void print_step_header(int step) {
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step++);
}

/* This has been written for you.
 * Prints out the output value (print token) nicely
 */
static void print_step_output(int val) {
  printf("|-----Program Output\n");
  printf("| %d\n", val);
}

/* This has been written for you.
 * Prints out the information at the bottom of each step
 */
static void print_step_footer(Symtab *symtab, Stack_head *stack) {
  hash_print_symtab(symtab);
  stack_print(stack);
  token_print_remaining();
  printf("o-------------------\n");
}
