typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数
    TK_EOF,      // endmarkers
} TokenKind;

//トークン
typedef struct Token Token;
struct Token{
    TokenKind kind; //整数、記号etc
    Token *next;    //次のトークン
    int val;        //
    char *str;      //
};

//Input
char *user_input;

//Current Token
Token *token;

// Create a new token
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// Tokenize
Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

  while (*p) {
    //スペースをとばす
    if (isspace(*p)) {
        p++;
        continue;
    }

    //記号
    if (strchr("+-*/()", *p)) {
        cur = new_token(TK_RESERVED, cur, p++);
        continue;
    }

    //整数
    if (isdigit(*p)) {
        cur = new_token(TK_NUM, cur, p);
        cur->val = strtol(p, &p, 10);
        continue;
    }

    error_at(p, "invalid token");
  }

    new_token(TK_EOF, cur, p);
    return head.next;
}


// Parser

typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_NUM,     // 整数
} NodeKind;

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *expression();
Node *mul();
Node *primary();

//
Node *expression() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
        node = new_binary(ND_ADD, node, mul());
    else if (consume('-'))
        node = new_binary(ND_SUB, node, mul());
    else
        return node;
  }
}

//×÷
Node *mul() {
    Node *node = primary();

    for (;;) {
        if (consume('*'))
            node = new_binary(ND_MUL, node, primary());
        else if (consume('/'))
            node = new_binary(ND_DIV, node, primary());
        else
            return node;
  }
}

// primary = "(" expr ")" | num
Node *primary() {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_num(expect_number());
}


//code generator

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break; 
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }

    printf("  push rax\n");
}


int main(int argc,char **argv){
    if(argc !=2)
        error("%s: invalid number of arguments",argv[0]);

    user_input = argv[1]
    token = tokenize();
    Node *node = expr();

    //
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //
    gen(node);

    //
    printf("  pop rax\n");
    printf("  ret\n");
    return 0   
}