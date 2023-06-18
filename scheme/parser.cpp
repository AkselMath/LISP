#include "parser.h"

#include <stdexcept>

bool IfBracket(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();
    return std::get_if<BracketToken>(&token);
}

bool IsOpenBracket(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();
    return BracketToken::OPEN == *std::get_if<BracketToken>(&token);
}

bool IsQuote(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();
    if (std::get_if<QuoteToken>(&token)) {
        return true;
    }
    return false;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto obj = ReadSymbol(tokenizer);
    if (!tokenizer->IsEnd()) {
        throw SyntaxError{"AAAAAA"};
    }
    return obj;
}

std::shared_ptr<Object> ReadSymbol(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }
    Token token = tokenizer->GetToken();
    if (IsQuote(tokenizer)) {
        std::shared_ptr<Cell> root_ptr = std::make_shared<Cell>();
        std::shared_ptr<Cell> cell_current_ptr = root_ptr;
        std::shared_ptr<Symbol> symbol_ptr = std::make_shared<Symbol>("quote");
        cell_current_ptr->SetFirst(symbol_ptr);
        tokenizer->Next();

        std::shared_ptr<Cell> cell_ptr = std::make_shared<Cell>();
        cell_current_ptr->SetSecond(cell_ptr);
        cell_current_ptr = cell_ptr;

        if (IfBracket(tokenizer) && IsOpenBracket(tokenizer)) {
            tokenizer->Next();
            if (IfBracket(tokenizer) && !IsOpenBracket(tokenizer)) {
                std::shared_ptr<Cell> cell_ptr2 = std::make_shared<Cell>();
                cell_current_ptr->SetFirst(cell_ptr2);
                cell_current_ptr = cell_ptr2;
                tokenizer->Next();
            } else {
                cell_current_ptr->SetFirst(ReadList(tokenizer));
            }
        } else {
            cell_current_ptr->SetFirst(ReadSymbol(tokenizer));
        }
        return root_ptr;
    }
    if (!IfBracket(tokenizer)) {
        if (auto constant_token = std::get_if<ConstantToken>(&token)) {
            std::shared_ptr<Number> number_ptr = std::make_shared<Number>(constant_token->value);
            tokenizer->Next();
            return number_ptr;
        } else if (auto symbol_token = std::get_if<SymbolToken>(&token)) {
            std::shared_ptr<Symbol> symbol_token_ptr = std::make_shared<Symbol>(symbol_token->name);
            tokenizer->Next();
            return symbol_token_ptr;
        } else if (auto boolean_token = std::get_if<BooleanToken>(&token)) {
            std::shared_ptr<Boolean> symbol_token_ptr =
                std::make_shared<Boolean>(*boolean_token == BooleanToken::True);
            tokenizer->Next();
            return symbol_token_ptr;
        }
    } else {
        if (IsOpenBracket(tokenizer)) {
            tokenizer->Next();
            return ReadList(tokenizer);
        } else {
            throw SyntaxError{"AAAAAA"};
        }
    }
    return nullptr;
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::shared_ptr<Cell> root_ptr = std::make_shared<Cell>();
    std::shared_ptr<Cell> cell_current_ptr = root_ptr;
    while (true) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError{"AAAAAA"};
        }
        Token token = tokenizer->GetToken();
        if (IfBracket(tokenizer) && !IsOpenBracket(tokenizer)) {
            tokenizer->Next();
            break;
        }
        if (std::get_if<DotToken>(&token)) {
            if (!cell_current_ptr->GetFirst()) {
                throw SyntaxError{"AAAAAA"};
            }
            tokenizer->Next();
            cell_current_ptr->SetSecond(ReadSymbol(tokenizer));
            token = tokenizer->GetToken();
            if (!IfBracket(tokenizer)) {
                throw SyntaxError{"AAAAAA"};
            }
        } else {
            cell_current_ptr->SetFirst(ReadSymbol(tokenizer));
            token = tokenizer->GetToken();
            if (!std::get_if<DotToken>(&token)) {
                if (IfBracket(tokenizer) && !IsOpenBracket(tokenizer)) {
                    tokenizer->Next();
                    break;
                } else {
                    std::shared_ptr<Cell> cell_ptr = std::make_shared<Cell>();
                    cell_current_ptr->SetSecond(cell_ptr);
                    cell_current_ptr = cell_ptr;
                }
            }
        }
    }

    if ((!root_ptr->GetFirst() && !root_ptr->GetSecond())) {
        return nullptr;
    }
    return root_ptr;
}
