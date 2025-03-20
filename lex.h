#pragma once
#include "ctype.h"
#include "memory.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
static char *enumStrings[] = {"Arrow", "Dash", "Identifier", "String",
                              "Colon", "D-Colon", "Comma", "Backslash",
                              "Pipe", "OpenP", "CloseP", "OpenSP",
                              "CloseSP", "OpenCP", "CloseCP", "DATA_Primeform"};

typedef enum
{
    Data,
    Struct,
    Unkown
} ReservedKeywords;

static char *reserved_keyworsd[] = {"data", "struct"};
ReservedKeywords string_to_enum(const char *str)
{
    for (size_t i = 0;
         i < sizeof(reserved_keyworsd) / sizeof(reserved_keyworsd[0]); i++)
    {
        if (strcmp(str, reserved_keyworsd[i]) == 0)
        {
            return (ReservedKeywords)i;
        }
    }
    return Unkown;
}

typedef enum
{
    Arrow,
    Dash,
    Identifier,
    String,
    Colon,
    DoubleColon,
    Comma,
    Backslash,
    Pipe,
    OpenP,
    CloseP,
    OpenSP,
    CloseSP,
    OpenCP,
    CloseCP,
    DATA_Primeform
} TokenType;

typedef struct
{
    int loc;
    char *tok_val;
    TokenType tok;
} Token;

typedef struct
{
    char *input;
    Token *ret;
    size_t ret_size;
    size_t in_len;
    int idx;
} Lexer;

Lexer *initLexerState(char *input)
{
    Lexer *lex_state = (Lexer *)malloc(sizeof(Lexer));
    lex_state->ret = (Token *)malloc(sizeof(Token));
    lex_state->idx = 0;
    lex_state->ret_size = 0;
    lex_state->in_len = strlen(input);
    lex_state->input = malloc(strlen(input) + 1);
    strcpy(lex_state->input, input);
    return lex_state;
}

void pushBackTok(Lexer *lexer, Token *to_push, TokenType tok_ty)
{
    to_push->tok = tok_ty;
    lexer->ret =
        (Token *)realloc(lexer->ret, (lexer->ret_size + 1) * sizeof(Token));
    lexer->ret[lexer->ret_size] = *to_push;
    lexer->ret_size = lexer->ret_size + 1;
    free(to_push);
}

static void advance(Lexer *lexer) { lexer->idx += 1; }

int8_t collectUntil(Lexer *lexer, size_t len, char *collector,
                    int8_t collecting_str)
{
    // check can peek
    if (lexer->idx + 1 > lexer->in_len)
    {
        // We reached the the end :( .. no more running here,
        // this is where we finally meet, and we stare blankly,
        // in the end, was it worth it? All that pain? Why
        // did we put each other through it? Was the love
        // ever real...? So many questions left unanswered
        // and every day I feel a piece of me chip away at the answer
        // and get lost to time forever...I miss her :/
        return 1;
    }
    char peeked = lexer->input[lexer->idx + 1];
    if (peeked == '\'')
    {
        return 2;
    }
    if ((((!isdigit(peeked) && !isalpha(peeked)) && peeked != '_') &&
         collecting_str == 1) ||
        collecting_str == 0 && peeked == '"')
    {
        return 0;
    }
    collector = realloc(collector, (len + 1) * sizeof(char));
    strncat(collector, &peeked, 1);
    advance(lexer);
    return collectUntil(lexer, len + 1, collector, collecting_str);
}

void lexOne(Lexer *lexer)
{
    Token *new_token = (Token *)malloc(sizeof(Token));
    new_token->loc = lexer->idx;
    new_token->tok_val = NULL;
    char current = lexer->input[lexer->idx];
    switch (current)
    {
    case '\\':
        pushBackTok(lexer, new_token, Backslash);
        advance(lexer);
        break;
    case ',':
        pushBackTok(lexer, new_token, Comma);
        advance(lexer);
        break;
    case '(':
        pushBackTok(lexer, new_token, OpenP);
        advance(lexer);
        break;
    case ')':
        pushBackTok(lexer, new_token, CloseP);
        advance(lexer);
        break;
    case '{':
        pushBackTok(lexer, new_token, OpenCP);
        advance(lexer);
        break;
    case '}':
        pushBackTok(lexer, new_token, CloseCP);
        advance(lexer);
        break;
    case '[':
        pushBackTok(lexer, new_token, OpenSP);
        advance(lexer);
        break;
    case ']':
        pushBackTok(lexer, new_token, CloseSP);
        advance(lexer);
        break;
    case '|':
        pushBackTok(lexer, new_token, Pipe);
        advance(lexer);
        break;
    case ':':
        if (lexer->input[lexer->idx + 1] == ':')
        {
            pushBackTok(lexer, new_token, DoubleColon);
            advance(lexer);
        }
        else
        {
            pushBackTok(lexer, new_token, Colon);
        }
        advance(lexer);
        break;
    case '-':
        if (lexer->input[lexer->idx + 1] == '>')
        {
            pushBackTok(lexer, new_token, Arrow);

            advance(lexer);
        }
        else
        {
            pushBackTok(lexer, new_token, Dash);
        }
        advance(lexer);
        break;
    case ' ':
        advance(lexer);
        break;
    case '/':
        advance(lexer);
        if (lexer->input[lexer->idx] == '/')
        {
            advance(lexer);
            for (size_t at = lexer->idx; at < lexer->in_len && lexer->input[at] != '\n'; at += 1)
            {
                advance(lexer);
            }
        }
        else if (lexer->input[lexer->idx] == '*')
        {
            for (size_t at = lexer->idx; at < lexer->in_len; at += 1)
            {
                if (lexer->input[at] == '*' && lexer->input[at + 1] == '/')
                {
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                advance(lexer);
            }
        }
        break;
    case '"':
        advance(lexer);
        char *collector = malloc(sizeof(char) * 1);
        strncat(collector, &lexer->input[lexer->idx], 1);
        int8_t res = collectUntil(lexer, 0, collector, 0);
        if (res == 0)
        {
            pushBackTok(lexer, new_token, String);
            advance(lexer);
            break;
        }
    default:
        if (isdigit(current) || isalpha(current) || current == '_')
        {
            char *collector = malloc(sizeof(char));
            strncat(collector, &current, 1);
            int8_t res = collectUntil(lexer, 0, collector, 1);
            if (res == 2)
            {
                switch (*collector)
                {
                case 'd':
                    pushBackTok(lexer, new_token, DATA_Primeform);
                    break;
                }
            }
            else if (res == 0)
            {
                new_token->tok_val = malloc(sizeof(char) * strlen(collector));
                strcpy(new_token->tok_val, collector);
                pushBackTok(lexer, new_token, Identifier);
                advance(lexer);
                break;
            }
        }
        advance(lexer);
        break;
    }
    return;
}
void lexAll(Lexer *lexer)
{
    if (lexer->idx >= lexer->in_len)
    {
        return;
    }
    lexOne(lexer);
    return lexAll(lexer);
}
