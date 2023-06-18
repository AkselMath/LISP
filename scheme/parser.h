#pragma once

#include "object.h"
#include "tokenizer.h"
#include "error.h"

bool IfBracket(Tokenizer* tokenizer);

bool IsOpenBracket(Tokenizer* tokenizer);

bool IsQuote(Tokenizer* tokenizer);

std::shared_ptr<Object> Read(Tokenizer* tokenizer);
std::shared_ptr<Object> ReadSymbol(Tokenizer* tokenizer);
std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);