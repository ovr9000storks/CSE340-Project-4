/**
 * @file parser.h
 * @author Andrew Lauricella
 * ID - 1214833867
 * @version 2
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 * @brief 
 * A header file that contains the interface for parsing a program 
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "execute.h"

using namespace std;

//declare global InstructionList*
extern struct InstructionNode * program;


struct location{
    string name;
    int value;
    int address;
};

//define parser class

void printSyntaxError();
void parseProgram();
void parseVarSection();
void parseIdList();
void parseBody();
void parseCaseBody(struct InstructionNode * finTarget);
void parseStmtList();
void parseStmt();
void parseAssignStmt(struct InstructionNode * curInstr);
void parseExpression(struct InstructionNode * curInstr);
int parsePrimary();
ArithmeticOperatorType parseOperator();
void parseOutputStmt();
void parseInputStmt();
void parseWhileStmt();
void parseIfStmt();
void parseCondition();
ConditionalOperatorType parseRelOp();
void parseSwitchStmt();
void parseForStmt();
void parseCaseList(Token var, struct InstructionNode * finTarget);
void parseCase(Token var, struct InstructionNode * finTarget);
void parseDefaultCase();
void parseInputs();
void parseNumList();
struct InstructionNode * parse_generate_intermediate_representation();
void newNode();
void printInstructions();
LexicalAnalyzer lexer;
void syntax_error();
Token expect(TokenType expected_type);





#endif