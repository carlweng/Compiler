#include "Semantics.h"

double Parameter = 0.0;
double Origin_x = 0.0, Origin_y = 0.0;
double Rot_angle = 0.0;
double Scale_x = 1.0, Scale_y = 1.0;
double forStart, forEnd, forStep;

double GetExprValue(struct ExprNode *root)
{
    if(root == NULL)
    {
        return 0.0;
    }
    int opCode = root->OpCode;
    //printf("[DEBUG] GetExprValue %d\n", opCode);
    if(opCode >= 14 && opCode <= 18)
    {
        double left = GetExprValue(root->Content.CaseOperator.Left);
        double right = GetExprValue(root->Content.CaseOperator.Right);
        switch(root->OpCode)
        {
            case PLUS:
                return left + right;
            case MINUS:
                return left - right;
            case MUL:
                return left * right;
            case DIV:
                return left / right;
            case POWER:
                return pow(left, right);
        }
    }else if(opCode == FUNC)
    {
        return (*root->Content.CaseFunc.MathFuncPtr)(GetExprValue(root->Content.CaseFunc.Child));
    }else if(opCode == CONST_ID)
    {
        return root->Content.CaseConst;
    }else if(opCode == T)
    {
        return Parameter;
    }else return 0.0;
    
}

void CalcCoordinate(struct ExprNode *xRoot, struct ExprNode *yRoot, double *x_val, double *y_val)
{
    double local_x, local_y, temp;
    //printf("[DEBUG] CalcCoordinate \n");
    local_x = GetExprValue(xRoot);
    local_y = GetExprValue(yRoot);
    //printf("[DEBUG] CalcCoordinate %f %f\n", local_x, local_y);
    local_x *= Scale_x;
    local_y *= Scale_y;

    temp = local_x * cos(Rot_angle) + local_y * sin(Rot_angle);
    local_y = local_y * cos(Rot_angle) - local_x * sin(Rot_angle);
    local_x = temp;
    local_x += Origin_x;
    local_y += Origin_y;
    *x_val = local_x;
    *y_val = local_y;
}
/*
void DrawPixel(unsigned long x, unsigned long y)
{
#ifdef _VC_COMPILER
	SetPixel(hDC, x, y, red);
	SetPixel(hDC, x+1, y, red);
	SetPixel(hDC, x, y+1, red);
	SetPixel(hDC, x+1, y+1, red);
#endif

#ifdef _BC_COMPILER
	putpixel(x, y, white);
	putpixel(x+1, y, white);
	putpixel(x, y+1, white);
	putpixel(x+1, y+1, white);
#endif
}*/

void DrawLoop(double start, double end, double step, struct ExprNode *HorPtr, struct ExprNode *VerPtr)
{
    double x = 0.0, y = 0.0;
    //printf("[DEBUG] DrawLoop %d %d\n", HorPtr->OpCode, VerPtr->OpCode);
    for(Parameter = start; Parameter <= end; Parameter += step)
    {
        CalcCoordinate(HorPtr, VerPtr, &x, &y);
        printf("(%f, %f) ", x, y);
        //DrawPixel((unsigned long)x, (unsigned long)y);
    }
}

//analyse the whole statement
struct ExprNode* Statement(struct Token *tokenChain, int lenStatement)
{
    struct ExprNode* exprRoot;
    if(tokenChain[0].type == ORIGIN)
    {
        OriginStatement(tokenChain, lenStatement);
        //printf("[DEBUG] Statement Origin %d %d\n", xRoot->OpCode, yRoot->OpCode);
        Origin_x = GetExprValue(xRoot);
        Origin_y = GetExprValue(yRoot);
        //printf("[DEBUG] Statement Origin\n");
    }
    else if(tokenChain[0].type == ROT)
    {
        RotStatement(tokenChain, lenStatement);
        Rot_angle = GetExprValue(rotRoot);
    }
    else if(tokenChain[0].type == SCALE)
    {
        ScaleStatement(tokenChain, lenStatement);
        Scale_x = GetExprValue(xRoot);
        Scale_y = GetExprValue(yRoot);
    }
    else if(tokenChain[0].type == FOR)
    {
        ForStatement(tokenChain, lenStatement);
        forStart = GetExprValue(startRoot);
        forEnd = GetExprValue(endRoot);
        forStep = GetExprValue(stepRoot);
        //printf("[DEBUG] Statement For\n");
        DrawLoop(forStart, forEnd, forStep, xRootFor, yRootFor);
    }
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
