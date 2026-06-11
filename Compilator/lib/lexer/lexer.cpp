#include "lexer.h"

Lexer::Lexer(std::string source) 
    : lexer_source(std::move(source)), lexer_current_pos(0), lexer_line(1) {}

std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token = getNextToken();

    while (token.type != TokenType::tEOF && token.type != TokenType::tERROR) {
        tokens.push_back(token);
        token = getNextToken();
    }

    if (token.type == TokenType::tERROR) {
        tokens.push_back(token);
    } else {
        tokens.push_back(token);
    }

    return tokens;
}

Token Lexer::getNextToken() {
    skipSpaceAndComments();

    if (isEnd()) {
        return makeToken(TokenType::tEOF, "");
    }

    char c = peek();

    if (c == '"') {
        return readStringLiteral();
    }

    if (isdigit(c) || (c == '.' && isdigit(peekNext()))) {
        return readNumberLiteral();
    }

    if (isalpha(c) || c == '_') {
        return readIdentifierOrKeyWord();
    }

    advance();
    switch(c) {
        case '!':
            if (match('=')) {
                return makeToken(TokenType::tNotEqual);
            } else {
                return makeToken(TokenType::tERROR, std::string(1,c) + (peek() == '=' ? "=" : ""));
            }
        case '=':
            return makeToken(match('=') ? TokenType::tEqual : TokenType::tAssign);
        case '<':
            return makeToken(match('=') ? TokenType::tLessOrEqual : TokenType::tLess);
        case '>':
            return makeToken(match('=') ? TokenType::tGreaterOrEqual : TokenType::tGreater);
        case '+':
            return makeToken(match('=') ? TokenType::tPlusAssign : TokenType::tPlus);
        case '-':
            return makeToken(match('=') ? TokenType::tMinusAssign : TokenType::tMinus);
        case '*':
            return makeToken(match('=') ? TokenType::tMultiplyAssign : TokenType::tMultiply);
        case '/':
            return makeToken(match('=') ? TokenType::tDivideAssign : TokenType::tDivide);
        case '%':
            return makeToken(match('=') ? TokenType::tModuleAssign : TokenType::tModule);
        case '^':
            return makeToken(match('=') ? TokenType::tPowerAssign : TokenType::tPower);
        case '(':
            return makeToken(TokenType::tLParenthesis);
        case ')':
            return makeToken(TokenType::tRParenthesis);
        case '[':
            return makeToken(TokenType::tLBracket);
        case ']':
            return makeToken(TokenType::tRBracket);
        case ',':
            return makeToken(TokenType::tComma);
        case ':':
            return makeToken(TokenType::tColon);
        default:
            return errorToken("Unexpected character: " + std::string(1, c));
    }
}

Token Lexer::readIdentifierOrKeyWord() {
    size_t start = lexer_current_pos;

    if (isalpha(peek()) || peek() == '_') {
        advance();
        while (isalnum(peek()) || peek() == '_') {
            advance();
        }

        if (lexer_source.substr(start, lexer_current_pos - start) == "end") {
            advance();
            while (isalnum(peek()) || peek() == '_') {
                advance();
            }
        }
    } else {
        return errorToken("Identifier cannot start with " + std::string(1, peek()));
    }

    std::string lexema = lexer_source.substr(start, lexer_current_pos - start);
    auto it = keywords.find(lexema);

    if (it != keywords.end()) {
        return makeToken(it->second, lexema);
    }

    return makeToken(TokenType::tIdentifier, lexema);
}

Token Lexer::readNumberLiteral() {
    size_t start = lexer_current_pos;

    while (isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) {
            advance();
        }
    }

    if (tolower(peek()) == 'e') {
        advance();

        if (peek() == '+' || peek() == '-') {
            advance();
        }

        if (!isdigit(peek())) {
            return errorToken("Uncorrectly exponent in number");
        }

        while (isdigit(peek())) {
            advance();
        }
    }

    std::string lexema = lexer_source.substr(start, lexer_current_pos - start);
    return makeToken(TokenType::tNumber, lexema);
}

Token Lexer::readStringLiteral() {
    std::string value;
    size_t start_pos = lexer_current_pos;
    advance();

    while (peek() != '"' && !isEnd()) {
        if (peek() == '\n') {
            return errorToken("Newline in string or unterminated string");
        }
        if (peek() == '\\') {
            advance();

            if (isEnd()) {
                return errorToken("Unterminated escape sequence");
            }

            switch(peek()) {
                case '"':
                    value += '"';
                    break;
                case '\\':
                    value += '\\';
                    break;
                case 'n':
                    value += '\n';
                    break;
                case 't':
                    value += '\t';
                    break;
                default:
                    value += peek();
                    break;
            }

            advance();
        } else {
            value += advance();
        }
    }

    if (isEnd()) {
        return errorToken("Unterminated string");
    }

    advance();
    return makeToken(TokenType::tString, value);
}

void Lexer::skipSpaceAndComments() {
    while (!isEnd()) {
        char c = peek();

        switch(c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                ++lexer_line;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    advance();
                    advance(); 
                    while (peek() != '\n' && !isEnd()) {
                        advance();
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexema) const {
    return Token(type, lexema, static_cast<int>(lexer_line));
}

Token Lexer::makeToken(TokenType type) const {
    return Token(type, std::string(1, lexer_source[lexer_current_pos - 1]), static_cast<int>(lexer_line));
}

Token Lexer::errorToken(const std::string& message) const {
    return Token(TokenType::tERROR, message, static_cast<int>(lexer_line));
}

char Lexer::peek() const {
    if (isEnd()) {
        return '\0';
    }
    return lexer_source[lexer_current_pos];
}

char Lexer::peekNext() const {
    if (lexer_current_pos + 1 >= lexer_source.length()) {
        return '\0';
    }
    return lexer_source[lexer_current_pos + 1];
}

char Lexer::advance() {
    if (isEnd()) return '\0';
    return lexer_source[lexer_current_pos++];
}

bool Lexer::isEnd() const {
    return lexer_current_pos >= lexer_source.length();
}

bool Lexer::match(char expected) {
    if (isEnd()) {
        return false;
    }

    if (lexer_source[lexer_current_pos] != expected) {
        return false;
    }

    ++lexer_current_pos;
    return true;
}