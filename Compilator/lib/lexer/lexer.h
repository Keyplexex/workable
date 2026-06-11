#pragma once

#include "token.h"
#include <vector>

class Lexer {
    public:
        explicit Lexer(std::string source);
        Token getNextToken();
        std::vector<Token> getAllTokens();

    private:
        std::string lexer_source;
        size_t lexer_current_pos;
        size_t lexer_line;

        char peek() const;
        char peekNext() const;
        char advance();
        bool isEnd() const; //нет, это не конец моей лабы((
        void skipSpaceAndComments();

        Token makeToken(TokenType type, const std::string& lexema) const;
        Token makeToken(TokenType type) const;
        Token errorToken(const std::string& message) const;

        Token readStringLiteral();
        Token readNumberLiteral();
        Token readIdentifierOrKeyWord();

        bool match(char expected);
};
