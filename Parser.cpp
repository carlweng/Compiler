#include "Parser.h"

struct ExprNode *xRoot, *yRoot;
struct ExprNode *rotRoot;
struct ExprNode *startRoot, *endRoot, *stepRoot;
struct ExprNode *xRootFor, *yRootFor;

void nodePrint(struct ExprNode *node)
{
    if(node == NULL)
        printf("  ");
    else if(node->OpCode == PLUS)
        printf("+ ");
    else if(node->OpCode == MINUS)
        printf("- ");
    else if(node->OpCode == MUL)
        printf("* ");
    else if(node->OpCode == DIV)
        printf("/ ");
    else if(node->OpCode == POWER)
        printf("** ");
    else if(node->OpCode == FUNC)
    {
        for(int i = 3; i <= 8; i++)
        {
            if(TokenTab[i].FuncPtr == node->Content.CaseFunc.MathFuncPtr)
                printf("%s ", TokenTab[i].lexeme);
        }
    }
    else if(node->OpCode == CONST_ID)
        printf("%f ", node->Content.CaseConst);
    else if(node->OpCode == T)
        printf("T ");
}

int SyntaxTreeHeight(struct ExprNode *root)
{
    if(root == NULL)
        return 0;
    else if(root->OpCode == CONST_ID)
    {
        return 2;
    }
    else if(root->OpCode == FUNC)
        return SyntaxTreeHeight(root->Content.CaseFunc.Child) + 1;
    else if(root->OpCode == T)
        return 1;
    else if(root->OpCode == PLUS || root->OpCode == MINUS || root->OpCode == MUL || root->OpCode == DIV || root->OpCode ==  POWER)
    {
        int a = SyntaxTreeHeight(root->Content.CaseOperator.Left);
        int b = SyntaxTreeHeight(root->Content.CaseOperator.Right);
        return (a>b?a:b)+2;
    }
}

void SyntaxTreeOutput(struct ExprNode *root)
{
    int treeHeight = SyntaxTreeHeight(root);
    struct ExprNode **stack = (ExprNode**)malloc(sizeof(ExprNode)*100);
    int former = 0, latter = 0;
    stack[latter++] = root;
    struct ExprNode *node;
    //printf("[debug] tree height: %d\n", treeHeight);
    for(int i = 0; i < treeHeight; i++)
    {
        for(int j = 0; j < pow(2, i); j++)
        {
            node = stack[former++];
            nodePrint(node);
            if(node == NULL)
            {
                stack[latter++] = NULL;
                stack[latter++] = NULL;
            }
            else{
                if(node->OpCode == CONST_ID)
                {
                    stack[latter++] = NULL;
                    stack[latter++] = NULL;
                }else if(node->OpCode == FUNC) 
                {
                    stack[latter++] = node->Content.CaseFunc.Child;
                    stack[latter++] = NULL;
                }else{
                    if(node->Content.CaseOperator.Left != NULL)
                    {
                        stack[latter++] = node->Content.CaseOperator.Left;
                        //printf("left node\n");
                        //printf("op: %d\n", node->OpCode);
                    }else if(node->Content.CaseOperator.Left == NULL)
                        stack[latter++] = NULL;
                    if(node->Content.CaseOperator.Right != NULL)
                    {
                        stack[latter++] = node->Content.CaseOperator.Right;
                        //printf("right node\n");
                        //printf("op: %d\n", node->Content.CaseOperator.Right->OpCode);
                    }else if(node->Content.CaseOperator.Right == NULL)
                        stack[latter++] = NULL;
                    }
            
            }
        }
        printf("\n");
    }
    printf("\n");
}

void SyntaxError(int case_of)
{
    printf("ERROR code %d\n", case_of);
    exit(0);
}

struct Token* MatchToken(enum Token_Type AToken, struct Token *tokenChain, int lenStatement, int index)
{
    for(int i = index; i <= lenStatement; i++)
    {
        if(tokenChain[i].type == AToken)
            return &tokenChain[i];
    }
    if(AToken == POWER || PLUS || MINUS || MUL || DIV)
        return NULL;
    SyntaxError(AToken);    
}

ExprNode *MakeExprNode(enum Token_Type opcode, void* arg1, void* arg2)
{
    struct ExprNode *ExprPtr = (ExprNode*)malloc(sizeof(ExprNode));
    ExprPtr->OpCode = opcode;

    switch(opcode)
    {
        case CONST_ID:
            ExprPtr->Content.CaseConst = *(double*)arg1;
            break;
        case T:
            ExprPtr->Content.CaseParmPtr = (double*)arg1;
        case FUNC:
            ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)arg1;
            ExprPtr->Content.CaseFunc.Child = (ExprNode*)arg2;
            break;
        default:
            ExprPtr->Content.CaseOperator.Left = (ExprNode*)arg1;
            ExprPtr->Content.CaseOperator.Right = (ExprNode*)arg2;
            break;
    }   
    return ExprPtr; 
}

//generate the node
struct ExprNode* Expression(struct Token *tokenChain, struct Token *start, struct Token *end)
{
    struct ExprNode *node, *left, *right;
    int index = 1;
    struct Token *plusToken = MatchToken(PLUS, start, (int)(end - start), index);
    struct Token *minusToken = MatchToken(MINUS, start, (int)(end - start), index);
    struct Token *termEnd;
    if(plusToken == NULL && minusToken == NULL)
    {
        node = Term(tokenChain, ++start, --end);
    }else if(plusToken != NULL)
    {
        if(((minusToken != NULL) && (plusToken < minusToken)) || (minusToken == NULL))
        {
            node = (ExprNode*)malloc(sizeof(ExprNode));
            termEnd = --plusToken;
            plusToken++;
            left = Term(tokenChain, ++start, termEnd);
            //printf("[DEBUG] EXPRESSION %d %f\n", left->OpCode, left->Content.CaseConst);
            node->OpCode = PLUS;
            node->Content.CaseOperator.Left = left;
            right = Term(tokenChain, ++plusToken, --end);   //change it to Expression()
            //printf("[DEBUG] EXPRESSION %d %f\n", right->OpCode, right->Content.CaseConst);
            //if(right->OpCode == DIV)
            //    printf("[DEBUG] EXPRESSION %f %f\n", right->Content.CaseOperator.Left->Content.CaseConst, right->Content.CaseOperator.Right->Content.CaseConst);
            node->Content.CaseOperator.Right = right;
        }
    }else if(minusToken != NULL)
    {
        if(((minusToken != NULL) && (minusToken < plusToken)) || (plusToken == NULL))
        {
            node = (ExprNode*)malloc(sizeof(ExprNode));
            termEnd = minusToken--;
            left = Term(tokenChain, ++start, termEnd);
            node->OpCode = MINUS;
            node->Content.CaseOperator.Left = left;
            node->Content.CaseOperator.Right = Term(tokenChain, ++minusToken, --end);
        }
    }
    //printf("DEBUG: op of root: %d\n", node->OpCode);
    return node;
}

struct ExprNode* Term(struct Token *tokenChain, struct Token *start, struct Token *end)
{
    struct ExprNode *node, *left, *right;
    int index = 1;
    struct Token *mulToken = MatchToken(MUL, start, (int)(end - start), index);
    struct Token *divToken = MatchToken(DIV, start, (int)(end - start), index);
    struct Token *facEnd;
    if(mulToken == NULL && divToken == NULL)
    {
        node = Factor(tokenChain, start, end);
        //printf("[DEBUG] TERM %d %f\n", node->OpCode, node->Content.CaseConst);
    }else if(mulToken != NULL)
    {
        if(((divToken != NULL) && (mulToken < divToken)) || (divToken == NULL))
        {
            node = (ExprNode*)malloc(sizeof(ExprNode));
            facEnd = --mulToken;
            mulToken++;
            left = Factor(tokenChain, start, facEnd);
            node->OpCode = MUL;
            node->Content.CaseOperator.Left = left;
            node->Content.CaseOperator.Right = Factor(tokenChain, ++mulToken, end);
        }
    }else if(divToken != NULL)
    {
        if(((mulToken != NULL) && (divToken < mulToken)) || (mulToken == NULL))
        {
            node = (ExprNode*)malloc(sizeof(ExprNode));
            facEnd = --divToken;
            divToken++;
            left = Factor(tokenChain, start, facEnd);
            //printf("[DEBUG] TERM %d %f\n", left->OpCode, left->Content.CaseConst);
            node->OpCode = DIV;
            node->Content.CaseOperator.Left = left;
            right = Factor(tokenChain, ++divToken, end);
            //printf("[DEBUG] TERM %d %f\n", right->OpCode, right->Content.CaseConst);
            node->Content.CaseOperator.Right = right;
        }
    }
    //printf("[DEBUG] TERM %d %f\n", node->OpCode, node->Content.CaseConst);
    return node;
}

struct ExprNode* Factor(struct Token *tokenChain, struct Token *start, struct Token *end)
{
    struct ExprNode *node;
    if(start->type == PLUS || start->type == MINUS)
    {
        node = (ExprNode*)malloc(sizeof(ExprNode));
        node->OpCode = start->type;
        node->Content.CaseOperator.Left = NULL;
        node->Content.CaseOperator.Right = Factor(tokenChain, ++start, end);
    }else{
        node = Component(tokenChain, start, end);
    }
    //printf("[DEBUG] FACTOR %d %f\n", node->OpCode, node->Content.CaseConst);
    return node;
}

struct ExprNode *Component(struct Token *tokenChain, struct Token *start, struct Token *end)
{
    struct ExprNode *node, *left, *right;
    int index = 0;
    struct Token *powerPoint;
    powerPoint = MatchToken(POWER, start, (int)(end - start), index++);
    if(powerPoint == NULL)
    {
        node = Atom(tokenChain, start, end);
    }else{
        node = (ExprNode*)malloc(sizeof(ExprNode));
        node->OpCode = POWER;
        left = Atom(tokenChain, start, --powerPoint);
        powerPoint++;
        //printf("[DEBUG] COMPONENT LEFT %d %f\n", left->OpCode, left->Content.CaseConst);
        node->Content.CaseOperator.Left = left;
        right = Component(tokenChain, ++powerPoint, end);
        //printf("[DEBUG] COMPONENT RIGHT %d %f\n", right->OpCode, right->Content.CaseConst);
        node->Content.CaseOperator.Right = right;
    }
    //printf("[DEBUG] COMPONENT %d %f\n", node->OpCode, node->Content.CaseConst);
    return node;
}

struct ExprNode* Atom(struct Token *tokenChain, struct Token *start, struct Token *end)
{
    struct ExprNode *node, *child;
    if(start == end)
    {
        node = (ExprNode*)malloc(sizeof(ExprNode));
        if(start->type == CONST_ID)
        {
            node->OpCode = CONST_ID;
            node->Content.CaseConst = start->value;
            //printf("[DEBUG] ATOM %d %f\n", start->type, start->value);
            //printf("[DEBUG] ATOM %f\n", node->Content.CaseConst);
        }else if(start->type == T)
        {
            //printf("[Debug] ATOM T %d\n", node->OpCode);
            node->OpCode = T;
            //double *ParamPtr;
            //*ParamPtr = 0.0;
            //node->Content.CaseParmPtr = ParamPtr; 
            //printf("[Debug] ATOM T %d %f\n", node->OpCode, node->Content.CaseConst);
        }else{
            SyntaxError(0);
        }
    }
    struct Token *lBracket, *rBracket;
    if(start->type == FUNC)
    {
        node = (ExprNode*)malloc(sizeof(ExprNode));
        int index = 0;
        lBracket = MatchToken(L_BRACKET, start, (int)(end-start), index++);
        rBracket = MatchToken(R_BRACKET, start, (int)(end-start), index++);
        node->OpCode = FUNC;
        node->Content.CaseFunc.MathFuncPtr = start->FuncPtr;
        //printf("[DEBUG] ATOM FUNC\n");
        //printf("[DEBUG] ATOM START %s\n", lBracket->lexeme);
        //printf("[DEBUG] ATOM END %s\n", rBracket->lexeme);
        //if(start->FuncPtr == TokenTab[4].FuncPtr)
        //    printf("[DEBUG] FUNC: %s\n", TokenTab[3].lexeme);
        //printf("[DEBUG] diff: %d %d %d %d\n", r - l, l, r, end->type);
        child = Expression(tokenChain, lBracket, rBracket);
        node->Content.CaseFunc.Child = child;
    }else if(start->type == L_BRACKET)
    {
        int index = 0;
        lBracket = start;
        rBracket = MatchToken(R_BRACKET, start, (int)(end-start), index++);
        node = Expression(tokenChain, lBracket, rBracket);
    }
    //if(rBracket != end)
    //    SyntaxError(0);
    //printf("[DEBUG] ATOM %d %f\n", node->OpCode, node->Content.CaseConst);
    return node;
}

//analyse the single statement
void OriginStatement(struct Token *tokenChain, int lenStatement)
{
    int index = 0;
    //Token *chainPoint = tokenChain;
    struct Token *start, *end;
    struct ExprNode *root;
    MatchToken(ORIGIN, tokenChain, lenStatement, index);
    index++;
    MatchToken(IS, tokenChain, lenStatement, index);
    index++;
    start = MatchToken(L_BRACKET, tokenChain, lenStatement, index);
    index++;
    end = MatchToken(COMMA, tokenChain, lenStatement, index);
    index++;    
    xRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(xRoot);

    start = end;
    end = MatchToken(R_BRACKET, tokenChain, lenStatement, index);
    index++;
    yRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(yRoot);
}

void RotStatement(struct Token *tokenChain, int lenStatement)
{
    int index = 0;
    struct Token *start, *end;
    struct ExprNode *root;
    MatchToken(ROT, tokenChain, lenStatement, index);
    index++;
    start = MatchToken(IS, tokenChain, lenStatement, index);
    end = &tokenChain[lenStatement- 1];
    rotRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(rotRoot);
}

void ScaleStatement(struct Token *tokenChain, int lenStatement)
{
    int index = 0;
    struct Token *start, *end;
    struct ExprNode *root;
    MatchToken(SCALE, tokenChain, lenStatement, index++);
    MatchToken(IS, tokenChain, lenStatement, index++);

    //printf("[DEBUG] Scale Statement\n");

    start = MatchToken(L_BRACKET, tokenChain, lenStatement, index++);
    end = MatchToken(COMMA, tokenChain, lenStatement, index++);
    xRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(xRoot);

    start = end;
    end = MatchToken(R_BRACKET, tokenChain, lenStatement, index++);
    yRoot = Expression(tokenChain, start, end);
    //if(root->Content.CaseOperator.Left != NULL)
    //    printf("%d %d\n", root->OpCode, root->Content.CaseOperator.Left);
    SyntaxTreeOutput(yRoot);
}

void ForStatement(struct Token *tokenChain, int lenStatement)
{
    int index = 0;
    struct Token *start, *end, *point;
    struct ExprNode *root;
    MatchToken(FOR, tokenChain, lenStatement, index++);
    MatchToken(T, tokenChain, lenStatement, index++);
    start = MatchToken(FROM, tokenChain, lenStatement, index++);
    end = MatchToken(TO, tokenChain, lenStatement, index++);
    startRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(startRoot);

    while(&tokenChain[index] <= end)
        index++;
    start = end;
    end = MatchToken(STEP, tokenChain, lenStatement, index++);
    endRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(endRoot);

    while(&tokenChain[index] <= end)
        index++;
    start = end;
    end = MatchToken(DRAW, tokenChain, lenStatement, index++);
    stepRoot = Expression(tokenChain, start, end);
    SyntaxTreeOutput(stepRoot);

    while(&tokenChain[index] < end)
        index++;
    start = MatchToken(L_BRACKET, tokenChain, lenStatement, index++);
    end = MatchToken(COMMA, tokenChain, lenStatement, index++);
    //printf("[DEBUG] FOR %s %s\n", start->lexeme, end->lexeme);
    xRootFor = Expression(tokenChain, start, end);
    //printf("[DEBUG] FOR %d\n", xRootFor->OpCode);
    SyntaxTreeOutput(xRootFor);

    while(&tokenChain[index] < end)
        index++;
    start = end;
    end = MatchToken(R_BRACKET, tokenChain, lenStatement, index++);
    //printf("[DEBUG] FOR %s %s\n", start->lexeme, end->lexeme);
    yRootFor = Expression(tokenChain, start, ++end);
    SyntaxTreeOutput(yRootFor);
}
/*
//analyse the whole statement
struct ExprNode* Statement(struct Token *tokenChain, int lenStatement)
{
    struct ExprNode* exprRoot;
    if(tokenChain[0].type == ORIGIN)
        exprRoot = OriginStatement(tokenChain, lenStatement);
    else if(tokenChain[0].type == ROT)
        exprRoot = RotStatement(tokenChain, lenStatement);
    else if(tokenChain[0].type == SCALE)
        exprRoot = ScaleStatement(tokenChain, lenStatement);
    else if(tokenChain[0].type == FOR)
        exprRoot = ForStatement(tokenChain, lenStatement);
    else{
        SyntaxError(tokenChain[0].type);
        return NULL;
    }
    return exprRoot;
}

void Program(Token *tokenChain)
{
    struct ExprNode** ExprRootChain = (struct ExprNode**)malloc(sizeof(struct ExprNode) * 100);
    int countExprRoot = 0;
    struct Token *tokenPoint = tokenChain;
    int head = 0;
    for(int i = 0; i < lenTokenChain; i++)
    {
        if(tokenChain[i].type == SEMICO)
        {
            int lenStatement = i - head;
            ExprRootChain[countExprRoot++] = Statement(tokenPoint, lenStatement);
            head = i;
            tokenPoint = &tokenChain[i + 1];
        }
    }
}

void Parser(char*filename)
{
    struct Token *tokenChain = Scanner(filename);
    Program(tokenChain);
}

int main(int argc, char* argv[])
{
    Parser(argv[1]);
    return 0;
}
*/