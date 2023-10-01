#pragma once

#include <variant>
#include <istream>
#include <iostream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

enum class BooleanToken { True, False };

struct ConstantToken {
    int64_t value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

// Интерфейс позволяющий читать токены по одному из потока.
class Tokenizer {
public:
    Tokenizer(std::istream* in) : in_(in) {
        Next();
    };

    bool IsEnd() {
        return end_;
    }

    void Next() {
        if (IsEnd()) {
            return;
        }
        while (in_->peek() == ' ' || in_->peek() == '\n' || in_->peek() == '\t') {
            in_->get();
        }
        char symbol = in_->peek();
        if (symbol == EOF) {
            end_ = true;
        }
        std::string lexeme;
        if (symbol == '\'') {
            in_->get();
            token_ = QuoteToken();
        } else if (symbol == '.') {
            in_->get();
            token_ = DotToken();
        } else if (symbol == '(') {
            in_->get();
            token_ = BracketToken::OPEN;
        } else if (symbol == ')') {
            in_->get();
            token_ = BracketToken::CLOSE;
        } else if (std::isdigit(symbol)) {
            while (std::isdigit(in_->peek())) {
                lexeme += in_->get();
            }
            token_ = ConstantToken{std::stoi(lexeme)};
        } else {
            lexeme += in_->get();
            if (std::isdigit(in_->peek())) {
                while (std::isdigit(in_->peek())) {
                    lexeme += in_->get();
                }
                token_ = ConstantToken{std::stoi(lexeme)};
            } else {
                while (in_->peek() != EOF && in_->peek() != ' ' && in_->peek() != '\t' &&
                       in_->peek() != '\n' && in_->peek() != '(' && in_->peek() != ')' &&
                       in_->peek() != '.' && in_->peek() != '\'' && !std::isdigit(in_->peek())) {
                    lexeme += in_->get();
                }
                if (lexeme == "#t") {
                    token_ = BooleanToken::True;
                } else if (lexeme == "#f") {
                    token_ = BooleanToken::False;
                } else {
                    token_ = SymbolToken{lexeme};
                }
            }
        }
    }

    Token GetToken() {
        return token_;
    }

private:
    bool end_ = false;
    std::istream* in_;
    Token token_;
};
