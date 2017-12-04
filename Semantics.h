#include "Parser.h"

extern double Parameter;
extern double Origin_x, Origin_y;
extern double Rot_angle;
extern double Scale_x, Scale_y;
extern double forStart, forEnd, forStep;


extern double GetExprValue(struct ExprNode *root);
extern void CalcCoordinate(struct ExprNode *xRoot, struct ExprNode *yRoot, double *x_val, double *y_val);
extern void DrawLoop(double start, double end, double step, struct ExprNode *HorPtr, struct ExprNode *VerPtr);
extern struct ExprNode* Statement(struct Token *tokenChain, int lenStatement);
extern void Program(Token *tokenChain);
extern void Parser(char*filename);