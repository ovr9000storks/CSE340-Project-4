/**
 * @file parser.cc
 * @author Andrew Lauricella
 * ID - 1214833867
 * @version 2
 * @date 2021-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 * @brief 
 * A general parser to generate the linked list of structures 
 * to populate the program using a global struct InstructionNode * program;
 * 
 */

#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <string.h>
#include <cstdlib>
#include <algorithm>
#include "parser.h"

using namespace std;

struct InstructionNode * head = NULL;
struct InstructionNode * current = NULL;
struct InstructionNode * varChecker = NULL;

struct vector<location> vars;

int inputCount = 0;
int numIns = 0;

//define functions for parser.cc
void syntax_error()
{
    cout << "SYNTAX ERROR !!&%!!\n";
    exit(1);
}

Token expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

void parseProgram(){
    parseVarSection();
    parseBody();
    varChecker = head;
    parseInputs();
}

void parseVarSection(){
    parseIdList();
    expect(SEMICOLON);
}

void parseIdList(){
    Token id = expect(ID);
    //add the variable to the mem initialized as 0
    location newL;
    newL.name = id.lexeme;
    newL.value = 0;
    newL.address = next_available;
    vars.push_back(newL);
    mem[next_available] = newL.value;
    next_available++;

    //DO THIS FOR ALL PLACES A NUM IS EXPECTED 
    //BUT CHECK IF ITS A DUPLICATE

    //if there are more than one ID parse those too
    Token peek = lexer.peek(1);
    if(peek.token_type == COMMA){
        expect(COMMA);
        parseIdList();
    }

}

void parseBody(){
    expect(LBRACE);
    parseStmtList();
    expect(RBRACE);
}

void parseCaseBody(struct InstructionNode * finTarget){
    expect(LBRACE);
    parseStmtList();
    expect(RBRACE);

    //create the cushion jmp node at the end that leads to finTarget
    newNode();
    current->type = JMP;
    current->jmp_inst.target = finTarget;
}

void parseStmtList(){
    //allocate next InstructionNode and set current to the new node
    newNode();

    //parse the statement
    parseStmt();

    //if the next token isnt a RBRACE, assume theres more statements
    Token peek = lexer.peek(1);
    if(peek.token_type != RBRACE){
        parseStmtList();
    }
}

void parseStmt(){
    Token peek = lexer.peek(1);
    if(peek.token_type == WHILE){
        current->type = CJMP;
        parseWhileStmt();
    }else if(peek.token_type == IF){
        current->type = CJMP;
        parseIfStmt();
    }else if(peek.token_type == SWITCH){
        current->type = NOOP;
        parseSwitchStmt();
    }else if(peek.token_type == FOR){
        //first step of a for loop is to assign a variable
        //instruction after the assign is then CJMP
        current->type = ASSIGN;
        parseForStmt();
    }else if(peek.token_type == OUTPUT){
        current->type = OUT;
        parseOutputStmt();
    }else if(peek.token_type == INPUT){
        current->type = IN;
        parseInputStmt();
    }else{
        current->type = ASSIGN;
        parseAssignStmt(current);
    }
}

void parseAssignStmt(struct InstructionNode * curInstr){
    Token lhs = expect(ID);

    for(int i = 0; i < next_available; i++){
        if(lhs.lexeme.compare(vars[i].name) == 0){
            curInstr->assign_inst.left_hand_side_index = vars[i].address;
        }
    }

    expect(EQUAL);

    Token peek = lexer.peek(2);
    if(peek.token_type == SEMICOLON){
        curInstr->assign_inst.operand1_index = parsePrimary();
        curInstr->assign_inst.op = OPERATOR_NONE;
    }else{
        parseExpression(curInstr);
    }

    expect(SEMICOLON);
}

//suited for assigning only
void parseExpression(struct InstructionNode * curInstr){
    curInstr->assign_inst.operand1_index = parsePrimary();
    curInstr->assign_inst.op = parseOperator();
    curInstr->assign_inst.operand2_index = parsePrimary();
}

int parsePrimary(){
    Token peek = lexer.peek(1);
    if(peek.token_type == ID){
        Token id = expect(ID);

        for(int i = 0; i < next_available; i++){
            if((vars[i].name).compare(id.lexeme) == 0){
                return vars[i].address;
            }
        }

        return -1;
    }else if(peek.token_type == NUM){
        Token num = expect(NUM);

        //determine if the num was already added to mem
        bool found = false;
        for(int i = 0; i < next_available; i++){
            if((vars[i].name).compare(num.lexeme) == 0){
                found = true;
                return vars[i].address;
            }
        }

        //if the num was never added, add it to mem
        location newL; 
        newL.name = num.lexeme;
        newL.value = stoi(num.lexeme);
        newL.address = next_available;
        vars.push_back(newL);
        mem[next_available] = newL.value;
        next_available++;
        return newL.address;
        
    }else{
        syntax_error();
        return -1;
    }
    return -1;
}

ArithmeticOperatorType parseOperator(){
    Token peek = lexer.peek(1);
    if(peek.token_type == PLUS){
        expect(PLUS);
        return OPERATOR_PLUS;
    }else if(peek.token_type == MINUS){
        expect(MINUS);
        return OPERATOR_MINUS;
    }else if(peek.token_type == MULT){
        expect(MULT);
        return OPERATOR_MULT;
    }else if(peek.token_type == DIV){
        expect(DIV);
        return OPERATOR_DIV;
    }else{
        syntax_error();
    }
    return OPERATOR_PLUS;
}

void parseOutputStmt(){
    expect(OUTPUT);
    
    Token id = expect(ID);
    for(int i = 0; i < next_available; i++){
        if((vars[i].name).compare(id.lexeme) == 0){
            current->output_inst.var_index = vars[i].address;
            i = next_available; //stop the for loop early if found
            
        }
    }

    expect(SEMICOLON);
}

void parseInputStmt(){
    expect(INPUT);

    Token id = expect(ID);
    for(int i = 0; i < next_available; i++){
        if(vars[i].name.compare(id.lexeme) == 0){
            current->input_inst.var_index = vars[i].address;
            inputCount++;
        }
    }

    

    expect(SEMICOLON);
    
}

void parseWhileStmt(){
    /**
     * @brief Equivalent to
     * ifLabel: IF condition
     *          ....stmtList
     *          ....goto ifLabel
     * 
     */

    expect(WHILE);
    parseCondition();
    InstructionNode * temp = current;
    parseBody();

    //create the jump Instruction Node to loop
    newNode();
    current->type = JMP;
    current->jmp_inst.target = temp;

    //create the NOOP to jump to when done
    newNode();
    temp->cjmp_inst.target = current;

}

void parseIfStmt(){
    expect(IF);
    parseCondition();
    InstructionNode * temp = current;
    parseBody();

    //create a cushion node to jump to with NOOP
    newNode();

    //set the IF's target to this NOOP node
    temp->cjmp_inst.target = current;
}

void parseCondition(){
    current->cjmp_inst.operand1_index = parsePrimary();
    current->cjmp_inst.condition_op = parseRelOp();
    current->cjmp_inst.operand2_index = parsePrimary();
}

ConditionalOperatorType parseRelOp(){
    Token peek = lexer.peek(1);
    if(peek.token_type == GREATER){
        expect(GREATER);
        return CONDITION_GREATER;
    }else if(peek.token_type == LESS){
        expect(LESS);
        return CONDITION_LESS;
    }else if(peek.token_type == NOTEQUAL){
        expect(NOTEQUAL);
        return CONDITION_NOTEQUAL;
    }else{
        syntax_error();
    }
    return CONDITION_NOTEQUAL;
}

void parseSwitchStmt(){
    expect(SWITCH);
    Token var = expect(ID);
    
    //create the target to jump to
    InstructionNode * finalTarget = new InstructionNode;
    finalTarget->next = NULL;
    finalTarget->type = NOOP;

    expect(LBRACE);
    parseCaseList(var, finalTarget);

///*
    //if the next token is not a right brace, assume its a default case
    Token peek = lexer.peek(1);
    if(peek.token_type == DEFAULT){
        parseDefaultCase();
    }
//*/

    expect(RBRACE);
    
    //add finalTarget node to the end of the instruction list
    current->next = finalTarget;
    current = current->next;
}

void parseForStmt(){
    /**
     * @brief Equivalent to 
     * assignStmt
     * WHILE condition
     * ....stmtList
     * ....assignStmt
     * 
     */

    expect(FOR);
    expect(LPAREN);
    parseAssignStmt(current);

    //create the node for the actual conditional
    newNode();
    current->type = CJMP;   //set new instruction to a CJMP
    parseCondition();
    expect(SEMICOLON);

    InstructionNode * temp = current;   //pointer to loop back to
    
    //create the node for the second assign
    //add to list of instructions after body
    InstructionNode * assign2 = new InstructionNode;
    assign2->type = ASSIGN;
    parseAssignStmt(assign2);
    expect(RPAREN);

    parseBody();

    //set the next instruction to the second assignment
    current->next = assign2;
    current = current->next;

    //create the jump Instruction Node to loop
    newNode();
    current->type = JMP;
    current->jmp_inst.target = temp;

    //create the NOOP to jump to when done
    newNode();
    temp->cjmp_inst.target = current;
}

void parseCaseList(Token var, struct InstructionNode * finTarget){
    parseCase(var, finTarget);
    Token peek = lexer.peek(1);

    //if the switch end or a default is not reached, assume theres another case
    if(peek.token_type == CASE){
        parseCaseList(var, finTarget);
    }
}

void parseCase(Token var, struct InstructionNode * finTarget){
    /**
     * @brief Equivealent to an IF statement with a 
     * JMP added into the end of each case
     * 
     */

    //create a new CJMP node to add to the instruction nodes
    //only equal operator is NOT_EQUAL, next and target must be swapped
    //node's next is now the following switch case
    //node's jmp target consists of the stmt body and additional jmp instruction to finTarget at the end of the body
    newNode();
    current->type = CJMP;
    current->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

    //Let the CJMP nodes build on temp->next
    //and when complete, move it to temp->cjmp_inst.target
    struct InstructionNode * temp = current;

    expect(CASE);

    //find value of op1
    for(int i = 0; i < next_available; i++){
        if(var.lexeme.compare(vars[i].name) == 0){
            current->cjmp_inst.operand1_index = vars[i].address;
        }
    }
    
    //find value of op2, if it doesnt exist, create it and set it to 0
    Token num = expect(NUM);
    //determine if the num was already added to mem
    bool found = false;
    int addr = -1;
    for(int i = 0; i < next_available; i++){
        if((vars[i].name).compare(num.lexeme) == 0){
            found = true;
            current->cjmp_inst.operand2_index = vars[i].address;
            i = next_available; //stop the for loop early if found
        }
    }
    //if the num was never added, add it to mem
    location newL;
    if(!found){
        
        newL.name = num.lexeme;
        newL.value = stoi(num.lexeme);
        newL.address = next_available;
        vars.push_back(newL);
        mem[next_available] = newL.value;
        next_available++;
        current->cjmp_inst.operand2_index = newL.address;
    }

    expect(COLON);

    parseCaseBody(finTarget);

    //swap temp->next to the cjmp target to allow for the switch's next case to build on temp's ->next
    temp->cjmp_inst.target = temp->next;
    temp->next = NULL;
    current = temp;

/*
    //add the jump instruction at the end of the body
    newNode();
    current->type = JMP;
    current->jmp_inst.target = finTarget;
*/

}

void parseDefaultCase(){
    expect(DEFAULT);
    expect(COLON);
    parseBody();
}

void parseInputs(){
    parseNumList();
}

void parseNumList(){
    Token t = expect(NUM);
    inputs.push_back(stoi(t.lexeme));
    numIns++;
    
    Token peek = lexer.peek(1);
    if(peek.token_type != END_OF_FILE){
        parseNumList();
    }else{
        expect(END_OF_FILE);
    }
}

struct InstructionNode * parse_generate_intermediate_representation(){
    parseProgram();
    //printInstructions();
    return head;
}

//create the next Instruction Node and default it to NOOP
void newNode(){
    if(head == NULL){
        head = new InstructionNode;
        current = head;
        head->type = NOOP;
        head->next = NULL;
    }else{
        current->next = new InstructionNode;
        current = current->next;
        current->type = NOOP;
        current->next = NULL;
    }
}

void printInstructions(){
    for(int i = 0; i < next_available; i++){
        cout << vars[i].name << " " << vars[i].value << " " << mem[i] << endl;
    }
    cout << "==========\n";
    for(int i = 0; i < numIns; i++){
        cout << inputs[i] << endl;
    }

    InstructionNode * myC = head;
    while(myC != NULL){
        cout << endl;
        if(myC->type == IN){
            cout << "IN: " << vars[myC->input_inst.var_index].name << "|" << mem[myC->input_inst.var_index];
            
        }else if(myC->type == OUT){
            cout << "OUT: " << vars[myC->output_inst.var_index].name << "|" << mem[myC->output_inst.var_index];
            
        }else if(myC->type == ASSIGN){
            cout << "ASSIGN: " << vars[myC->assign_inst.left_hand_side_index].name << "|" << mem[myC->assign_inst.left_hand_side_index];
            cout << " = " << vars[myC->assign_inst.operand1_index].name << "|" << mem[myC->assign_inst.operand1_index];
            cout << " " << myC->assign_inst.op << " ";// << vars[myC->assign_inst.operand2_index].name  << "|" << mem[myC->assign_inst.operand2_index] << endl;
            
        }else if(myC->type == CJMP){
            cout << "CJMP || ";
            cout << myC->cjmp_inst.target->next->type;
        }else if(myC->type == JMP){
            cout << "JMP || ";
            cout << myC->jmp_inst.target->next->type;
        }else if(myC->type == NOOP){
            cout << "NOOP\n";
        }else{
            cout << "\nFUCKFUCKFUCK\n";
        }

        myC = myC->next;
    }
}
