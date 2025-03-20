// TODO: IMPLEMENT VECTORS ???

#pragma once
#include "lex.h"
#include <stdbool.h>

typedef struct Expr Expr;

typedef struct
{
    char *name;
    Expr *values;
    size_t amt_values;
} ADTConstructorsWithNameAndValues;

typedef union
{
    ADTConstructorsWithNameAndValues constructor_with_name_and_values;
    char *enumerated;
} AlgebraicDataTypeExpr;

typedef struct
{
    Expr *left;
    Expr *right;
} AssignmentExpr;

typedef enum
{
    Assignment,
    AlgebraicDataType
} ExprTy;

struct Expr
{
    ExprTy expr_ty;
    union
    {
        AssignmentExpr assignment;
        AlgebraicDataTypeExpr adt;
    } data;
};

/*

Expr { .expr_ty = AlgebraicDataType, .data = data.adt = AlgebraicDataTypeExpr {} }
*/

typedef struct
{
    Token *input;
    size_t in_size;
    int idx;
} Parser;

static void __advance(Parser *parser)
{
    parser->idx += 1;
}

static void __deadvance(Parser *parser)
{
    parser->idx -= 1;
}

Parser *initParserState(Token *input, size_t in_size)
{
    Parser *parse_state = (Parser *)malloc(sizeof(Parser));
    parse_state->in_size = in_size;
    parse_state->input = input;
    parse_state->idx = 0;

    return parse_state;
}

Token *peek(Parser *parser)
{
    if (parser->idx + 1 > parser->in_size)
    {
        return NULL;
    }
    return &parser->input[parser->idx + 1];
}
Token *__current(Parser *parser)
{
    if (parser->idx >= parser->in_size)
    {
        printf("11");
        abort();
    }
    return &parser->input[parser->idx];
}
bool __expect(Parser *parser, Token *in, TokenType expected_ty)
{
    return (in != NULL && in->tok == expected_ty);
}

Token *expect(Parser *parser, TokenType expected_ty)
{
    Token *current = __current(parser);
    if (__expect(parser, current, expected_ty) == false)
    {
        printf("NOT WHAT WE SPECTED => %s\n", enumStrings[current->tok]);
        return NULL;
    }
    return current;
}

/*
This function parsers everything after a `\` (backslash)
which may be either a:
    1. Lambda definition (\ Int, Int :: Int)
    2. Lambda impmentation (whateverLambda x, y = x + y)
    3. Both (\ Int, Int :: Int = x + y)
*/
Expr *parseLambda(Parser *parser)
{
    return NULL;
}

void parse(Parser *parser)
{
    Expr *new_expr = (Expr *)malloc(sizeof(Expr));
    Token *current = __current(parser);
    switch (current->tok)
    {
    case DATA_Primeform:
        __advance(parser);
        bool is_sum_type = false;
        Token *identifier = expect(parser, Identifier);
        size_t members_size = 0;
        Token *members = malloc(sizeof(Token) * 0);

        size_t constructors_size = 0;
        Expr *constructors = malloc(sizeof(Expr) * 0);
        __advance(parser);
        is_sum_type = parser->input[parser->idx].tok == DoubleColon;
        if (is_sum_type)
        {
            // if its a sum type we need to advance past the double
            // colon we literally like JUST handled.
            __advance(parser);
        }
        while (1)
        {
            Token *current = __current(parser);
            if (current->tok == Pipe)
            {
                if (!is_sum_type)
                {
                    printf("Make sure to add the `::` after the name (\"%s\") for sum types :)", identifier->tok_val);
                    abort();
                }
                printf("Pipe constructing constructor\n");
                if (members_size == 1 && members[0].tok == Identifier)
                {
                    // We have encountered what I like to call an "enumerated" variant.
                    // That is, this variant acts just like an enum in which its just a named
                    // field ...
                    printf("Enumerated\n");
                    Expr constructor = {.data.adt = {.enumerated = members[0].tok_val}, .expr_ty = AlgebraicDataType};
                    constructors = (Expr *)realloc(constructors, sizeof(Expr) * (constructors_size + 1));
                    constructors[constructors_size] = constructor;
                    constructors_size = constructors_size + 1;
                }
                // ADTConstructors constructor = { .amt_values = members_size, .name = };
                members_size = 0;
                members = (Token *)realloc(members, sizeof(Token) * 0);
                __advance(parser);
            }
            Token *name = expect(parser, Identifier);
            if (name == NULL)
            {
                printf("Expected identifier...breaking\n");
                free(name);
                break;
            }
            __advance(parser);
            // check for things that would invalidate the identifier.
            // such as, a backslash which indicates a lambda assignment
            if (expect(parser, Backslash) != NULL)
            {
                printf("Found a backslash following identifier. Breaking.\n");
                __deadvance(parser);
                break;
            }

            members = (Token *)realloc(members, sizeof(Token) * (members_size + 1));
            members[members_size] = *name;
            members_size = members_size + 1;
        }
        printf("Handled prime form data: %s", identifier->tok_val);
        for (size_t at = 0; at < members_size; at += 1)
        {
            printf("\n--> %s (@ %d)", enumStrings[members[at].tok], members[at].loc);
        }
        break;
    case Identifier:
        Token *current = __current(parser);
        __advance(parser);
        if (expect(parser, Backslash) != NULL)
        {
            __advance(parser);
            printf("\ %s\\",current->tok_val);
            Expr *lambda_body = parseLambda(parser);
        }
        break;
    default:
        printf("Default");
        break;
    }
}
