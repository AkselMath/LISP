#include "scheme.h"

#include <stdexcept>

Scheme::Scheme() : scope_(std::make_shared<Scope>()) {
    scope_->SetElementScope("quote", std::make_shared<QuoteSpecForm>());

    scope_->SetElementScope("number?", std::make_shared<NumberQ>());
    scope_->SetElementScope("boolean?", std::make_shared<BooleanQ>());
    scope_->SetElementScope("symbol?", std::make_shared<SymbolQ>());
    scope_->SetElementScope("pair?", std::make_shared<PairQ>());
    scope_->SetElementScope("null?", std::make_shared<NullQ>());
    scope_->SetElementScope("list?", std::make_shared<ListQ>());

    scope_->SetElementScope("=", std::make_shared<Equal>());
    scope_->SetElementScope("<", std::make_shared<Less>());
    scope_->SetElementScope("<=", std::make_shared<LessEquals>());
    scope_->SetElementScope(">", std::make_shared<More>());
    scope_->SetElementScope(">=", std::make_shared<MoreEquals>());

    scope_->SetElementScope("+", std::make_shared<Plus>());
    scope_->SetElementScope("-", std::make_shared<Minus>());
    scope_->SetElementScope("/", std::make_shared<Division>());
    scope_->SetElementScope("*", std::make_shared<Multiplication>());

    scope_->SetElementScope("max", std::make_shared<Max>());
    scope_->SetElementScope("min", std::make_shared<Min>());
    scope_->SetElementScope("abs", std::make_shared<Abs>());

    scope_->SetElementScope("not", std::make_shared<Not>());
    scope_->SetElementScope("and", std::make_shared<And>());
    scope_->SetElementScope("or", std::make_shared<Or>());

    scope_->SetElementScope("define", std::make_shared<Define>());
    scope_->SetElementScope("set!", std::make_shared<Set>());

    scope_->SetElementScope("if", std::make_shared<If>());

    scope_->SetElementScope("cons", std::make_shared<Cons>());
    scope_->SetElementScope("car", std::make_shared<Car>());
    scope_->SetElementScope("cdr", std::make_shared<Cdr>());
    scope_->SetElementScope("set-car!", std::make_shared<SetCar>());
    scope_->SetElementScope("set-cdr!", std::make_shared<SetCdr>());

    scope_->SetElementScope("list", std::make_shared<List>());
    scope_->SetElementScope("list-ref", std::make_shared<ListRef>());
    scope_->SetElementScope("list-tail", std::make_shared<ListTail>());
}

std::string Scheme::Evaluate(const std::string& expression) {
    std::stringstream ss{expression};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);
    if (!obj) {
        throw RuntimeError{"Пусто"};
    }
    auto asd = obj->Eval(scope_);
    return asd->Print();
}
