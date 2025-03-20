#include "lex.h"
#include "parse.h"

int main()
{
    FILE *in_file = fopen("./test.ft", "r");
    fseek(in_file, 0, SEEK_END);
    long size = ftell(in_file);
    rewind(in_file);

    char *in_file_content = malloc(size + 1);
    if (!in_file_content)
    {
        perror("Memory allocation failed");
        fclose(in_file);
        return 1;
    }
    fread(in_file_content, 1, size, in_file);
    in_file_content[size] = '\0';
    Lexer *lexer = initLexerState(in_file_content);
    lexAll(lexer);
    printf("Ret size %zu\n", lexer->ret_size);
    // for (size_t i = 0; i < lexer->ret_size; i++)
    // {
    //     printf("Employee ID: %d, Name: %s\n", lexer->ret[i].loc, enumStrings[lexer->ret[i].tok]);
    //     if (lexer->ret[i].tok == Identifier || lexer->ret[i].tok == String && lexer->ret[i].tok_val != NULL)
    //     {
    //         printf("\tIdent/String Value -> %s\n", lexer->ret[i].tok_val);
    //     }
    // }
    Parser *parser = initParserState(lexer->ret, lexer->ret_size);
    parse(parser);
    parse(parser);
    parse(parser);
    free(in_file_content);
    free(lexer);
    fclose(in_file);
    return 0;
}
