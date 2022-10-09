#include <ctype.h>

typedef enum TokenKind {
    // Special Character
    Bang,

    // Stack Operations
    Over,
    Dup,

    // Operations
    Plus,
    Minus,
    Times,
    Divide,

    // Types
    Character, 
    Number,
    
    Empty,
    End,
    Err,
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char err[255];
    int value;
    int line;
    int col;
} Token;

const char printable_tokens[][10] = {
    "Bang",
    "Over",
    "Dup",
    "Plus",
    "Minus",
    "Times",
    "Divide",
    "Character", 
    "Number",
    "Empty",
    "End",
    "Err",
};

TokenKind to_token_kind(char t)
{
    if      (t == '!')        { return Bang; }
    else if (t == ' ')        { return Empty; }
    else if (t == '\n')       { return Empty; }
    else if (t == '+')        { return Plus; }
    else if (t == '-')        { return Minus; }
    else if (t == '*')        { return Times; }
    else if (t == '\\')       { return Divide; }
    else if (isdigit(t) == 0) { return Character; }
    else if (isdigit(t) != 0) { return Number; }
    else                      { return Err; }
}

TokenKind to_stack_operation(char *t)
{
    if (strcmp(t, "dup") == 0) { return Dup; }
    else if (strcmp(t, "over") == 0) { return Over; }
    else { return Err; }
}

Token *tokenize(char *s, int l)
{
    size_t len = strlen(s);
    size_t count = 0;
    Token tokens_kind[len + 1];
    Token *tokens = malloc(sizeof(Token) * 255);
    
    for (size_t i = 0; i < len; i++) {
        tokens_kind[i].kind = to_token_kind(s[i]);
    }

    for (size_t i = 0; i < len; i++) {
        TokenKind tk = tokens_kind[i].kind;
        TokenKind next_tk = tokens_kind[i + 1].kind;

        if (tk == Err || tk == Plus || tk == Minus || tk == Times || tk == Divide || tk == Bang) {
            Token t = {
                .kind = tk,
                .value = 0,
                .line = l,
                .col = i,
            };
            tokens[count] = t;
            count++;
            continue;
        }
        
        if (tk == Empty) {
            continue;
        }

        if (tk == Number && next_tk != Number) {
            Token t = {
                .kind = tk,
                .value = s[i] - '0',
                .line = l,
                .col = i,
            };
            tokens[count] = t;
            count++;
            continue;
        }

        char tmp[255] = "";
        int col = i;
        if (tk == next_tk && tk == Number) {
            while (tokens_kind[i].kind == tk) {
                strncat(tmp, &s[i], 1);
                i++;
            }
            Token t = {
                .kind = tk,
                .value = atoi(tmp),
                .line = l,
                .col = col,
            };
            tokens[count] = t;
            count++;
        } else {
            while (tokens_kind[i].kind == tk) {
                strncat(tmp, &s[i], 1);
                i++;
            }
            Token t = {
                .kind = to_stack_operation(tmp),
                .value = 0,
                .line = l,
                .col = col,
            };
            strcpy(t.err, tmp);
            tokens[count] = t;
            count++;
        }
    }
    
    tokens[count++].kind = End;

    return tokens;
}

void print_tokens(Token *t)
{
    for (int i = 0; t[i].kind != End; i++) {
        TokenKind tk = t[i].kind;
        if (tk == Number) { printf("%d, (%d, %d)\n", t[i].value, t[i].line, t[i].col); }
        else if (tk == Err) { printf("%s, (%d, %d)\n", t[i].err, t[i].line, t[i].col); }
        else { printf("%s, (%d, %d)\n", printable_tokens[t[i].kind], t[i].line, t[i].col);}
    }
}
