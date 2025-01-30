#pragma once
// typedef struct
// {
//     Token *input;
//     Token *ret;
//     size_t ret_size;
//     size_t in_len;
//     int idx;
// } Lexer;

// Lexer *initLexerState(char *input)
// {
//     Lexer *lex_state = (Lexer *)malloc(sizeof(Lexer));
//     lex_state->ret = (Token *)malloc(sizeof(Token));
//     lex_state->idx = 0;
//     lex_state->ret_size = 0;
//     lex_state->in_len = strlen(input);
//     lex_state->input = malloc(strlen(input) + 1);
//     strcpy(lex_state->input, input);
//     return lex_state;
// }