#pragma once
#include "stdio.h"
#include "memory.h"
#include "stdlib.h"
#include "ctype.h"
static char *enumStrings[] = {"Arrow", "Dash", "Identifier", "String", "Colon", "D-Colon", "Comma", "Backslash", "Pipe", "OpenP", "CloseP", "OpenSP", "CloseSP", "OpenCP", "CloseCP"};

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
    CloseCP
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

void pushBackTok(Lexer *lexer, Token *to_push, TokenType tok_ty, char *collector)
{
    if (collector != NULL)
    {
        to_push->tok_val = malloc(strlen(collector));
        strcpy(to_push->tok_val, collector);
        free(collector);
    }
    to_push->tok = tok_ty;
    lexer->ret = (Token *)realloc(lexer->ret, (lexer->ret_size + 1) * sizeof(Token));
    lexer->ret[lexer->ret_size] = *to_push;
    lexer->ret_size = lexer->ret_size + 1;
    free(to_push);
}

void advance(Lexer *lexer)
{
    lexer->idx += 1;
}

int8_t collectUntil(Lexer *lexer, size_t len, char *collector, int8_t collecting_str)
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
    if ((((!isdigit(peeked) && !isalpha(peeked)) && peeked != '_') && collecting_str == 1) || collecting_str == 0 && peeked == '"')
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
        pushBackTok(lexer, new_token, Backslash, NULL);
        advance(lexer);
        break;
    case ',':
        pushBackTok(lexer, new_token, Comma, NULL);
        advance(lexer);
        break;
    case '(':
        pushBackTok(lexer, new_token, OpenP, NULL);
        advance(lexer);
        break;
    case ')':
        pushBackTok(lexer, new_token, CloseP, NULL);
        advance(lexer);
        break;
    case '{':
        pushBackTok(lexer, new_token, OpenCP, NULL);
        advance(lexer);
        break;
    case '}':
        pushBackTok(lexer, new_token, CloseCP, NULL);
        advance(lexer);
        break;
    case '[':
        pushBackTok(lexer, new_token, OpenSP, NULL);
        advance(lexer);
        break;
    case ']':
        pushBackTok(lexer, new_token, CloseSP, NULL);
        advance(lexer);
        break;
    case '|':
        pushBackTok(lexer, new_token, Pipe, NULL);
        advance(lexer);
        break;
    case ':':
        if (lexer->input[lexer->idx + 1] == ':')
        {
            pushBackTok(lexer, new_token, DoubleColon, NULL);
            advance(lexer);
        }
        else
        {
            pushBackTok(lexer, new_token, Colon, NULL);
        }
        advance(lexer);
        break;
    case '-':
        if (lexer->input[lexer->idx + 1] == '>')
        {
            pushBackTok(lexer, new_token, Arrow, NULL);

            advance(lexer);
        }
        else
        {
            pushBackTok(lexer, new_token, Dash, NULL);
        }
        advance(lexer);
        break;
    case ' ':
        advance(lexer);
        break;
    case '"':
        advance(lexer);
        char *collector = malloc(sizeof(char) * 1);
        strncat(collector, &lexer->input[lexer->idx], 1);
        int8_t res = collectUntil(lexer, 0, collector, 0);
        if (res == 0)
        {
            pushBackTok(lexer, new_token, String, collector);
            advance(lexer);
            break;
        }
    default:
        if (isdigit(current) || isalpha(current) || current == '_')
        {
            char *collector = malloc(sizeof(char) * 1);
            strncat(collector, &current, 1);
            int8_t res = collectUntil(lexer, 0, collector, 1);
            if (res == 0)
            {
                pushBackTok(lexer, new_token, Identifier, collector);
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