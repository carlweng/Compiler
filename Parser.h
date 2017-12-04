#include "Scanner.h"

struct ExprNode {
    enum Token_Type OpCode;
    union{
        struct {
            ExprNode *Left, *Right;
        }CaseOperator;
        struct {
            FuncPtr MathFuncPtr;
            ExprNode *Child;
        }CaseFunc;
        double CaseConst;
        double *CaseParmPtr;
    }Content;
};

extern struct ExprNode *xRoot, *yRoot;
extern struct ExprNode *rotRoot;
extern struct ExprNode *startRoot, *endRoot, *stepRoot;
extern struct ExprNode *xRootFor, *yRootFor;

void nodePrint(struct ExprNode *node);
extern int SyntaxTreeHeight(struct ExprNode *root);
extern void SyntaxTreeOutput(struct ExprNode *root);
extern void SyntaxError(int case_of);
extern struct Token* MatchToken(enum Token_Type AToken, struct Token *tokenChain, int lenStatement, int index);
extern ExprNode *MakeExprNode(enum Token_Type opcode, void* arg1, void* arg2);
extern struct ExprNode* Expression(struct Token *tokenChain, struct Token *start, struct Token *end);
extern struct ExprNode* Term(struct Token *tokenChain, struct Token *start, struct Token *end);
extern struct ExprNode* Factor(struct Token *tokenChain, struct Token *start, struct Token *end);
extern struct ExprNode *Component(struct Token *tokenChain, struct Token *start, struct Token *end);
extern struct ExprNode* Atom(struct Token *tokenChain, struct Token *start, struct Token *end);

extern void OriginStatement(struct Token *tokenChain, int lenStatement);
extern void RotStatement(struct Token *tokenChain, int lenStatement);
extern void ScaleStatement(struct Token *tokenChain, int lenStatement);
extern void ForStatement(struct Token *tokenChain, int lenStatement);
/*
extern void OriginStatement(struct Token *tokenChain, int lenStatement, struct ExprNode *xRoot, struct ExprNode *yRoot);
extern void RotStatement(struct Token *tokenChain, int lenStatement, struct ExprNode *rotRoot);
extern void ScaleStatement(struct Token *tokenChain, int lenStatement, struct ExprNode *xRoot, struct ExprNode *yRoot);
extern void ForStatement(struct Token *tokenChain, int lenStatement, struct ExprNode *startRoot, struct ExprNode *endRoot, struct ExprNode *stepRoot, struct ExprNode *xRoot, struct ExprNode *yRoot);
*/
/*
extern struct ExprNode* Statement(struct Token *tokenChain, int lenStatement);
extern void Program(Token *tokenChain);
extern void Parser(char*filename);
*/
