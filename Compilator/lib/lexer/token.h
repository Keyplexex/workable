#pragma once

#include <string>
#include <unordered_map>
#include <ostream>

enum class TokenType {

    tIdentifier,
    tNumber,
    tString,

    tIf,
    tThen,
    tElse,
    tWhile,
    tFor,
    tIn,
    tFunc,
    tReturn,
    tNil,
    tTrue,
    tFalse,
    tAnd,
    tOr,
    tNot,
    tBreak,
    tContinue,
    tEndIf,
    tEndWhile,
    tEndFor,
    tEndFunc,

    tPlus,
    tMinus,
    tMultiply,
    tDivide,
    tModule,
    tPower,

    tAssign,
    tPlusAssign,
    tMinusAssign,
    tMultiplyAssign,
    tDivideAssign,
    tModuleAssign,
    tPowerAssign,

    tEqual,
    tNotEqual,
    tLess,
    tGreater,
    tLessOrEqual,
    tGreaterOrEqual,

    tLParenthesis,
    tRParenthesis,
    tLBracket,
    tRBracket,
    tComma,
    tColon,

    tEOF,
    tERROR
};

std::string tokenTypeToStr(TokenType type);

struct Token {

    TokenType type;
    std::string lexema;
    int line;

    Token(TokenType type, std::string lexema, int line)
        : type(type), lexema(std::move(lexema)), line(line) {}

};

extern const std::unordered_map<std::string, TokenType> keywords;