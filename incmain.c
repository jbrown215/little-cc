#include "diff.h"

#include "util/queue.h"
#include "tokenizer/tokenize.h"
#include "parser/parser.h"
#include "parser/ast.h"
#include "parser/env.h"
#include "writer/driver.h"

#include "util/hash.h"

int main(int argc, char** argv) {

    queue* token_queue = queue_new();

    int ok = 0;
    // char* filename;
    // if (argc > 1) filename = argv[1];
    // else filename = "test.asm";
    // FILE* f = fopen(filename, "w");
    // write_header(f);
    // env* E = env_new();
    printf("Allocated e'rything\n");

    while (ok == 0) {
        ok = parse_line(token_queue);
    }

    while (!queue_empty(token_queue)) {
        token_deq(token_queue);
    }
    // write_footer(f, E);
    // fclose(f);
    return 0;
}