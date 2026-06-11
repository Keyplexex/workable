#include "token.h"

//попытка дебага
std::string tokenTypeToStr(TokenType type) {
    switch(type) {
        case TokenType::tIdentifier: return "Identifier";
        case TokenType::tNumber: return "Number";
        case TokenType::tString: return "String";
        case TokenType::tIf: return "If";
        case TokenType::tElse: return "Else";
        case TokenType::tThen: return "Then";
        case TokenType::tWhile: return "While";
        case TokenType::tFor: return "For";
        case TokenType::tIn: return "In";
        case TokenType::tFunc: return "Funciton";
        case TokenType::tReturn: return "Return";
        case TokenType::tNil: return "Nil";
        case TokenType::tTrue: return "True";
        case TokenType::tFalse: return "False";
        case TokenType::tAnd: return "And";
        case TokenType::tOr: return "Or";
        case TokenType::tNot: return "Not";
        case TokenType::tBreak: return "Break";
        case TokenType::tContinue: return "Continue";
        case TokenType::tEndIf: return "End if";
        case TokenType::tEndWhile: return "End while";
        case TokenType::tEndFor: return "End for";
        case TokenType::tEndFunc: return "End function";
        case TokenType::tPlus: return "Plus";
        case TokenType::tMinus: return "Minus";
        case TokenType::tMultiply: return "Multiply";
        case TokenType::tDivide: return "Divide";
        case TokenType::tModule: return "Module";
        case TokenType::tPower: return "Power";
        case TokenType::tAssign: return "Assign";
        case TokenType::tPlusAssign: return "Plus Assign";
        case TokenType::tMinusAssign: return "Minus Assign";
        case TokenType::tMultiplyAssign: return "Multiply Assign";
        case TokenType::tDivideAssign: return "Divide Assign";
        case TokenType::tModuleAssign: return "Module Assign";
        case TokenType::tPowerAssign: return "Power Assign";
        case TokenType::tEqual: return "Equal";
        case TokenType::tNotEqual: return "Not Equal";
        case TokenType::tLess: return "Less";
        case TokenType::tGreater: return "Greater";
        case TokenType::tGreaterOrEqual: return "Greater or Equal";
        case TokenType::tLessOrEqual: return "Less or Equal";
        case TokenType::tLParenthesis: return "Left Parenthesis";
        case TokenType::tRParenthesis: return "Right Parenthesis";
        case TokenType::tComma: return "Comma";
        case TokenType::tColon: return "Colon";
        case TokenType::tRBracket: return "Right Bracket";
        case TokenType::tLBracket: return "Left Bracket";
        case TokenType::tEOF: return "EOF";
        case TokenType::tERROR: return "Error";

        default: return "Unknown token type";
    }
}

const std::unordered_map<std::string, TokenType> keywords = {
    {"if", TokenType::tEndIf},
    {"else", TokenType::tElse},
    {"then", TokenType::tThen},
    {"while", TokenType::tWhile},
    {"for", TokenType::tFor},
    {"function", TokenType::tFunc},
    {"in", TokenType::tIn},
    {"return", TokenType::tReturn},
    {"nil", TokenType::tNil},
    {"true", TokenType::tTrue},
    {"false", TokenType::tFalse},
    {"and", TokenType::tAnd},
    {"or", TokenType::tOr},
    {"not", TokenType::tNot},
    {"break", TokenType::tBreak},
    {"continue", TokenType::tContinue},
    {"end if", TokenType::tEndIf},
    {"end while", TokenType::tEndWhile},
    {"end for", TokenType::tEndFor},
    {"end function", TokenType::tEndFunc}
};


