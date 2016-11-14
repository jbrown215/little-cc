#include "../diff.h"
#include "../token_q.h"
#include "tokens.h"
#include "../vars.h"

#include "tokenize.h"

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_char_in_range(char c, char bottom, char top) {
    return bottom <= c && c <= top;
}

bool is_beginning_name_char(char c) {
    return ('_' == c) 
        || is_char_in_range(c, 'a', 'z')
        || is_char_in_range(c, 'A', 'Z');
}

bool is_number_char(char c) {
    return is_char_in_range(c, '0', '9');
}

bool is_name_char(char c) {
    return is_beginning_name_char(c) || is_number_char(c);
}

void inspect_for_keywords(queue* Q) {
    assert(!queue_empty(Q));

    token* tok = peek(Q);
    if (tok->type != NAME) {
        return;
    }

}

// void parse_long(queue* Q, char** pline, token_type type, classifier* f) {
//     int len = 0;
//     // Find the length of the token by scanning 
//     while(len < MAX_TOKEN_LENGTH 
//             && (*pline)[len] != 0
//             && (*f)((*pline)[len])) {
//         len++;
//     }

//     char* repr = calloc(len+1, sizeof(char));
//     for (int i = 0; i < len && **pline != 0 && (*f)(**pline); i++) {
//         repr[i] = **pline;
//         (*pline)++;
//     }
//     repr[len] = '\0';

//     token* tok = token_new(type, repr);
//     enq(Q, tok);
// }

void parse_number(queue* Q, char** pline) {
    int len = 0;
    // Find the length of the token by scanning 
    while(len < MAX_TOKEN_LENGTH 
            && (*pline)[len] != 0
            && is_number_char((*pline)[len])) {
        len++;
    }

    char* repr = calloc(len+1, sizeof(char));
    for (int i = 0; i < len && **pline != 0 && is_number_char(**pline); i++) {
        repr[i] = **pline;
        (*pline)++;
    }
    repr[len] = '\0';

    token* tok = token_new(NUMBER, repr);
    enq(Q, tok);
}


void parse_name(queue* Q, char** pline) {
    int len = 0;
    // Find the length of the token by scanning 
    while(len < MAX_TOKEN_LENGTH 
            && (*pline)[len] != 0
            && is_name_char((*pline)[len])) {
        len++;
    }


    char* repr = calloc(len+1, sizeof(char));
    for (int i = 0; i < len && **pline != 0 && is_name_char(**pline); i++) {
        repr[i] = **pline;
        (*pline)++;
    }
    repr[len] = 0;


    if (!strncmp(repr, "int", 4)) {
        free(repr);
        enq(Q, token_new(KW_INT, "int"));
        return;
    } 

    if (!strcmp(repr, "if")) {
        free(repr);
        enq(Q, token_new(KW_IF, "if"));
    } else if (!strcmp(repr, "while")) {
        free(repr);
        enq(Q, token_new(KW_WHILE, "while"));
    } else if (!strcmp(repr, "return")) {
        free(repr);
        enq(Q, token_new(KW_RETURN, "return"));
    } else if (!strcmp(repr, "void")) {
        free(repr);
        enq(Q, token_new(KW_VOID, "void"));
    } else if (!strcmp(repr, "else")) {
        free(repr);
        enq(Q, token_new(KW_ELSE, "else"));
    } else if (!strcmp(repr, "struct")) {
        free(repr);
        enq(Q, token_new(KW_STRUCT, "struct"));
    } else if (!strcmp(repr, "sizeof")) {
        free(repr);
        enq(Q, token_new(KW_SIZEOF, "sizeof"));
    } else if (!strcmp(repr, "typedef")) {
        free(repr);
        enq(Q, token_new(KW_TYPEDEF, "typedef"));
    } else if (!strcmp(repr, "for")) {
        free(repr);
        enq(Q, token_new(KW_FOR, "for"));
    } else if (!strcmp(repr, "switch")) {
        free(repr);
        enq(Q, token_new(KW_SWITCH, "switch"));
    } else if (!strcmp(repr, "case")) {
        free(repr);
        enq(Q, token_new(KW_CASE, "case"));
    } else if (!strcmp(repr, "default")) {
        free(repr);
        enq(Q, token_new(KW_DEFAULT, "default"));
    } else if (!strcmp(repr, "break")) {
        free(repr);
        enq(Q, token_new(KW_BREAK, "break"));
    } else if (!strcmp(repr, "char")) {
        free(repr);
        enq(Q, token_new(KW_CHAR, "char"));
    } else if (!strcmp(repr, "enum")) {
        free(repr);
        enq(Q, token_new(KW_ENUM, "enum"));
    }
    else {
        token* tok = token_new(NAME, repr);
        enq(Q, tok);
    }
}

char parse_string_advance(char** pline) {
    char c = 0;
    if (**pline == '\\') {
        (*pline)++;
        if (**pline == 'n') {
            c = '\n';
        } else if (**pline == 't') {
            c = '\t';
        } else if (**pline == '0') {
            c = '\0';
        } else {
            c = **pline;
        }
    } else {
        c = **pline;
    }
    (*pline)++;
    return c;
}

void parse_string(queue* Q, char** pline) {
    int len = 0;

    char* temp_p = *pline;

    parse_string_advance(&temp_p);
    // Find the length of the token by scanning 
    while(*temp_p != 0 && *temp_p != '"') {

        len++;
        parse_string_advance(&temp_p);
    }



    parse_string_advance(pline);

    char* repr = calloc(len+1, sizeof(char));
    for (int i = 0; i < len && **pline != 0 && **pline != '"'; i++) {
        repr[i] = parse_string_advance(pline);
    }
    repr[len] = '\0';

    parse_string_advance(pline);

    token* tok = token_new(STRING, repr);
    enq(Q, tok);

}

void parse_character(queue* Q, char** pline) {
    parse_string_advance(pline);

    char c = parse_string_advance(pline);
    parse_string_advance(pline);

    char* repr = malloc(sizeof(char)*2);
    repr[0] = c;
    repr[1] = '\0';

    token* tok = token_new(CHARACTER, repr);
    enq(Q, tok);

}

void parse_symbol(queue* Q, char** pline) {
    token_type type;
    char* repr;
    switch((char) **pline) {
        case '*':
            type = OP_MUL;
            repr = "*";
            break;
        case '/':
            type = OP_DIV;
            repr = "/";
            break;
        case '{':
            type = OPEN_BRACE;
            repr = "{";
            break;
        case '}':
            type = CLOSED_BRACE;
            repr = "}";
            break;
        case ';':
            type = SEMICOLON;
            repr = ";";
            break;
        case '.':
            type = OP_DOT;
            repr = ".";
            break;
        case '(':
            type = OPEN_PAREN;
            repr = "(";
            break;
        case ')':
            type = CLOSED_PAREN;
            repr = ")";
            break;
        case ',':
            type = OP_COMMA;
            repr = ",";
            break;
        case '[':
            type = OPEN_BRACKET;
            repr = "[";
            break;
        case ']':
            type = CLOSED_BRACKET;
            repr = "]";
            break;
        case ':':
            type = OP_COLON;
            repr = ":";
            break;
        case '?':  
            type = OP_QUESTION;
            repr = "?";
            break;
        case '+':
            if ((*pline)[1] == '+') {
                type = OP_INC;
                repr = "++";
                (*pline)++;
            }
            else {
                type = OP_PLUS;
                repr = "+";
            }
            break;
        case '-':
            if ((*pline)[1] == '>') {
                type = OP_ARROW;
                repr = "->";
                (*pline)++;
            } else if ((*pline)[1] == '-') {
                type = OP_DEC;
                repr = "--";
                (*pline)++;
            }
            else {
                type = OP_MINUS;
                repr = "-";
            }
            break;
        case '&':
            if ((*pline)[1] == '&') {
                type = OP_BOOL_AND;
                repr = "&&";
                (*pline)++;
            }
            else {
                type = OP_SINGLE_AND;
                repr = "&";
            }
            break;
        case '|':
            if ((*pline)[1] == '|') {
                type = OP_BOOL_OR;
                repr = "||";
                (*pline)++;
            }
            else {
                type = OP_SINGLE_OR;
                repr = "|";
            }
            break;
        case '=':
            if ((*pline)[1] == '=') {
                type = OP_EQ;
                repr = "==";
                (*pline)++;
            }
            else {
                type = OP_ASSIGN;
                repr = "=";
            }
            break;
        case '>':
            if ((*pline)[1] == '=') {
                type = OP_GTE;
                repr = ">=";
                (*pline)++;
            }
            else {
                type = OP_GT;
                repr = ">";
            }
            break;
        case '<':
            if ((*pline)[1] == '=') {
                type = OP_LTE;
                repr = "<=";
                (*pline)++;
            }
            else {
                type = OP_LT;
                repr = "<";
            }
            break;
        case '!':
            if ((*pline)[1] == '=') {
                type = OP_NEQ;
                repr = "!=";
                (*pline)++;
            }
            else {
                type = OP_NOT_BANG;
                repr = "!";
            }
            break;
        default:
            repr = calloc(sizeof(char), 2);
            *repr = **pline;
            type = OTHER;
    }

    (*pline)++;
    token* tok = token_new(type, repr);
    enq(Q, tok);
}

void skip_whitespace(char** pline) {

    while (is_whitespace(**pline)){
        (*pline)++;
    }
}

bool next_token(queue* Q, char** pline) {
    skip_whitespace(pline);
    if (**pline == 0) return true;
    if (**pline == '/' && *(*pline+1) == '/') return true;
    if (**pline == '#') return true;

    char c = **pline;
    if (is_beginning_name_char(c)) parse_name(Q, pline);
    else if (is_number_char(c)) parse_number(Q, pline);
    else if (c == '"') parse_string(Q, pline);
    else if (c == '\'') parse_character(Q, pline);
    else parse_symbol(Q, pline);
    return false;
}

int parse_line(queue* Q, FILE* f) {
    size_t n;
    char* buf = NULL;
    int read = getline(&buf, &n, f);
    if (-1 == read) return -1;


    char* line = buf;
    while (!next_token(Q, &line)) {
    }
    free(buf); line = NULL; buf = NULL;

    return 0;
}