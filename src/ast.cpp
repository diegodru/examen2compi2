#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"
#include <unordered_map>

const char * floatTemps[] = {
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

extern Asm assemblyFile;

int globalStackPointer = 0;

unordered_map<string, int> variables;

int labelCounter = 0;
string getLabel(string prefix){
  stringstream ss;
  ss << prefix << labelCounter++;
  return ss.str();
}

int strCounter = 0;
string getStrLabel(){
  stringstream ss;
  ss << "str" << strCounter++;
  return ss.str();
}

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    cout<<"No more float registers!"<<endl;
    return "";
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

void FloatExpr::genCode(Code &code){
  stringstream ss;
  ss << code.code;
  code.place = getFloatTemp();
  char f[100];
  sprintf(f, "%.8f", this->number);
  ss << "li.s " << code.place << ", " << number << '\n';
  code.code += ss.str();
}

void SubExpr::genCode(Code &code){
  code.place = getFloatTemp();
  stringstream ss;
  ss << code.code;
  Code leftCode;
  expr1->genCode(leftCode);
  Code rightCode;
  expr2->genCode(rightCode);
  ss << leftCode.code << rightCode.code;
  ss << "sub.s " << code.place << ", " << leftCode.place << ", " << rightCode.place << '\n';
  code.code = ss.str();
}

void DivExpr::genCode(Code &code){
  code.place = getFloatTemp();
  stringstream ss;
  Code leftCode;
  expr1->genCode(leftCode);
  Code rightCode;
  expr2->genCode(rightCode);
  ss << leftCode.code << rightCode.code;
  ss << "div.s " << code.place << ", " << leftCode.place << ", " << rightCode.place << '\n';
  code.code = ss.str();
}

void IdExpr::genCode(Code &code){
  stringstream ss;
  ss << code.code;
  code.place = getFloatTemp();
  ss << "l.s " << code.place << ", " << variables[id] << "($sp)\n";
  code.code = ss.str();
}

string ExprStatement::genCode(){
  Code exprCode;
  expr->genCode(exprCode);
  return exprCode.code;
}

string IfStatement::genCode(){
  stringstream ss;
  Code code;
  this->conditionalExpr->genCode(code);
  ss << code.code;
  ss << code.trueLabel << ":\n";
  for(auto s : this->trueStatement)
    ss << s->genCode();
  ss << code.falseLabel << ":\n";
  for(auto s : this->falseStatement)
    ss << s->genCode();
  ss << getLabel("endif") << ":\n";
  return ss.str();
  //return "If statement code generation\n";
}

void MethodInvocationExpr::genCode(Code &code){
  stringstream ss;
  ss << code.code;
  ss << "sw $ra, " << globalStackPointer << "($sp)\n";
  globalStackPointer += 4;
  ss << "addi $sp, $sp, " << globalStackPointer << "\n";
  ss << "jal " << this->id << "\n";
  ss << "addi $sp, $sp, -" << globalStackPointer << "\n";
  globalStackPointer -= 4;
  ss << "lw $ra " << globalStackPointer << "($sp)\n";
  releaseFloatTemp(code.place);
  code.place = getFloatTemp();
  ss << "mtc1 $v0, " << code.place << '\n';
  code.code = ss.str();
}

string AssignationStatement::genCode(){
  stringstream ss;
  Code valueCode;
  value->genCode(valueCode);
  ss << valueCode.code;
  variables[id] = globalStackPointer;
  ss << "s.s " << valueCode.place << ", " << globalStackPointer << "($sp)\n";
  globalStackPointer += 4;
  return ss.str();
}

void GteExpr::genCode(Code &code){
  stringstream ss;
  ss << code.code;
  Code leftCode;
  expr1->genCode(leftCode);
  Code rightCode;
  expr2->genCode(rightCode);
  code.trueLabel = getLabel("true");
  code.falseLabel = getLabel("false");
  ss << leftCode.code << rightCode.code;
  string end = getLabel("end");
  code.place = getFloatTemp();
  ss << "c.le.s " <<  leftCode.place << ", " << rightCode.place << '\n';
  ss << "bc1f " << code.trueLabel << "\nbc1t " << code.falseLabel << '\n';
  ss << "j " << end << "\n";
  ss << code.trueLabel << ": \nli.s " << code.place << ", 1.0\nj end\n";
  ss << code.falseLabel << ":\nli.s " << code.place << ", 0.0\nj end\n";
  code.code = ss.str();
}

void LteExpr::genCode(Code &code){
  string endLabel = getLabel("endif");
  stringstream ss;
  ss << code.code;
  Code leftCode;
  expr1->genCode(leftCode);
  Code rightCode;
  expr2->genCode(rightCode);
  code.trueLabel = getLabel("true");
  code.falseLabel = getLabel("false");
  string end = getLabel("end");
  code.place = getFloatTemp();
  ss << leftCode.code << rightCode.code;
  ss << "c.lt.s " <<  leftCode.place << ", " << rightCode.place << '\n';
  ss << "bc1t " << code.trueLabel << "\nbc1f " << code.falseLabel << '\n';
  ss << "j " << end << "\n";
  ss << code.trueLabel << ": \nli.s " << code.place << ", 1.0\nj end\n";
  ss << code.falseLabel << ":\nli.s " << code.place << ", 0.0\nj end\n";
  code.code = ss.str();
}

void EqExpr::genCode(Code &code){
  string endLabel = getLabel("endif");
  stringstream ss;
  ss << code.code;
  Code leftCode;
  expr1->genCode(leftCode);
  Code rightCode;
  expr2->genCode(rightCode);
  code.trueLabel = getLabel("true");
  code.falseLabel = getLabel("false");
  string end = getLabel("end");
  code.place = getFloatTemp();
  ss << leftCode.code << rightCode.code;
  ss << "c.eq.s " <<  leftCode.place << ", " << rightCode.place << '\n';
  ss << "bc1t " << code.trueLabel << "\nbc1f " << code.falseLabel << '\n';
  ss << "j " << end << "\n";
  ss << code.trueLabel << ": \nli.s " << code.place << ", 1.0\nj end\n";
  ss << code.falseLabel << ":\nli.s " << code.place << ", 0.0\nj end\n";
  code.code = ss.str();
}

void ReadFloatExpr::genCode(Code &code){
  stringstream ss;
  ss << code.code;
  code.place = "$f0";
  ss << "li $v0, 6\nsyscall\n";
  code.code = ss.str();
}

string PrintStatement::genCode(){
  stringstream ss;
  Code code;
  stringstream data;
  string label = getStrLabel();
  data << assemblyFile.data << '\n';
  data << label << ": .asciiz " << this->id << "\n";
  assemblyFile.data = data.str();
  ss << "la $a0, " << label << "\nli $v0, 4\nsyscall\n";
  for(auto e : expressions){
    e->genCode(code);
    ss << code.code;
    ss << "mov.s $f12, " << code.place << "\nli $v0, 2\nsyscall\n";
  }
  return ss.str();
}

string ReturnStatement::genCode(){
  stringstream ss;
  Code exprCode;
  this->expr->genCode(exprCode);
  ss << exprCode.code;
  ss << "mfc1 $v0, " << exprCode.place << '\n';
  return ss.str();
}

string MethodDefinitionStatement::genCode(){
  stringstream ss;
  ss << id << ":\n";
  int i = 0;
  for(auto p : params){
    ss << "swc1 $a" << i++ << ", " << globalStackPointer << "($sp)\n";
  }
  for(auto s : stmts){
    ss << s->genCode();
  }
  ss << "j $ra\n";
  return ss.str();
}
