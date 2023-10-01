#include "object.h"

#include <stdexcept>

void Scope::SetParentScope(std::shared_ptr<Scope> parent_scope) {
    parent_scope_ = parent_scope;
}

void Scope::SetElementScope(std::string symbol, std::shared_ptr<Object> object) {
    scope_[symbol] = object;
}

std::shared_ptr<Object> Scope::GetElementScope(std::string symbol) {
    if (scope_.find(symbol) == scope_.end()) {
        if (parent_scope_) {
            return parent_scope_->GetElementScope(symbol);
        }
        throw NameError{symbol + " такого элемента нет!"};
    }
    return scope_[symbol];
}

std::shared_ptr<Object> Object::Eval(std::shared_ptr<Scope>) {
    throw RuntimeError("Don't use Eval");
}

std::shared_ptr<Object> Object::Apply(std::shared_ptr<Object>, std::shared_ptr<Scope>) {
    throw RuntimeError("Don't use Apply");
};

std::string Object::Print() {
    throw RuntimeError("Don't use Print");
};

Number::Number(int value) : value_(value) {
}

std::shared_ptr<Object> Number::Eval(std::shared_ptr<Scope>) {
    return shared_from_this();
}

std::string Number::Print() {
    return std::to_string(value_);
}

int Number::GetValue() const {
    return value_;
}

Symbol::Symbol(std::string name) : name_(name) {
}

std::shared_ptr<Object> Symbol::Eval(std::shared_ptr<Scope> scope) {
    return scope->GetElementScope(name_);
}

std::string Symbol::Print() {
    return name_;
}

const std::string& Symbol::GetName() const {
    return name_;
}

Boolean::Boolean(bool name) : bool_(name) {
}

std::shared_ptr<Object> Boolean::Eval(std::shared_ptr<Scope>) {
    return shared_from_this();
}

const bool& Boolean::GetBool() const {
    return bool_;
}

std::string Boolean::Print() {
    if (bool_) {
        return "#t";
    }
    return "#f";
}

Pair::Pair(int element_one, int element_two)
    : element_one_(element_one), element_two_(element_two) {
}

std::string Pair::Print() {
    return "(" + std::to_string(element_one_) + " . " + std::to_string(element_two_) + ")";
}

void Pair::SetElementOne(int element_one) {
    element_one_ = element_one;
}

void Pair::SetElementTwo(int element_two) {
    element_two_ = element_two;
}

const int& Pair::GetElementOne() const {
    return element_one_;
}

const int& Pair::GetElementTwo() const {
    return element_two_;
}

ListObj::ListObj(std::vector<std::shared_ptr<Object>> object_shared_ptr)
    : object_shared_ptr_(object_shared_ptr) {
}

std::string ListObj::Print() {
    std::string result = "(";
    for (size_t i = 0; i < object_shared_ptr_.size(); ++i) {
        if (i == object_shared_ptr_.size() - 1) {
            result += std::to_string(As<Number>(object_shared_ptr_[i])->GetValue());
            result += ")";
            return result;
        }
        result += std::to_string(As<Number>(object_shared_ptr_[i])->GetValue());
        result += " ";
    }
    return result;
}

Cell::Cell() : first_(nullptr), second_(nullptr) {
}

Cell::Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
    : first_(first), second_(second) {
}

std::shared_ptr<Object> Cell::Eval(std::shared_ptr<Scope> scope) {
    if (Is<Symbol>(first_) && As<Symbol>(first_)->GetName() == "lambda") {
        auto f = std::make_shared<Lambda>();
        return f->Apply(second_, scope);
    }
    auto f = first_->Eval(scope);
    return f->Apply(second_, scope);
}

std::string Cell::Print() {
    std::string result = "(";
    std::shared_ptr<Cell> curent = As<Cell>(first_);
    if (!curent) {
        throw RuntimeError{"Cell null"};
    }
    if (!curent->first_) {
        return "()";
    }
    while (curent) {
        result += curent->first_->Print();
        if (curent->second_ && !Is<Cell>(curent->second_)) {
            result += " . ";
            result += curent->second_->Print();
            return result + ")";
        } else if (curent->second_) {
            result += " ";
            curent = As<Cell>(curent->second_);
        } else {
            return result + ")";
        }
    }
    return "()";
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

void Cell::SetFirst(std::shared_ptr<Object> first) {
    first_ = first;
}

void Cell::SetSecond(std::shared_ptr<Object> second) {
    second_ = second;
}

std::shared_ptr<Object> QuoteSpecForm::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) {
    if (!Is<Cell>(As<Cell>(args)->GetFirst())) {
        return As<Cell>(args)->GetFirst();
    }
    return args;
}

std::shared_ptr<Object> NumberQ::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 1) {
        throw RuntimeError{"Неверное количество аргументов для number?"};
    }

    if (Is<Number>(args_list[0])) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> SymbolQ::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 1) {
        throw RuntimeError{"Неверное количество аргументов для symbol?"};
    }

    if (Is<Symbol>(args_list[0])) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> BooleanQ::Apply(std::shared_ptr<Object> args,
                                        std::shared_ptr<Scope> scope) {

    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 1) {
        throw RuntimeError{"Неверное количество аргументов для boolean?"};
    }

    if (Is<Boolean>(args_list[0])) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> PairQ::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) {
    std::shared_ptr<Object> curent = As<Cell>(args)->GetFirst();
    std::shared_ptr<Object> is_quote = As<Cell>(curent)->GetFirst();
    if (Is<Symbol>(is_quote) && As<Symbol>(is_quote)->GetName() != "quote") {
        throw SyntaxError{"Неверные аргументы"};
    }
    curent = As<Cell>(curent)->GetSecond();
    curent = As<Cell>(curent)->GetFirst();

    if (!As<Cell>(curent)->GetFirst() || !As<Cell>(curent)->GetSecond()) {
        return std::make_shared<Boolean>(false);
    } else if (Is<Number>(As<Cell>(curent)->GetFirst()) &&
               Is<Number>(As<Cell>(curent)->GetSecond())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Cell>(As<Cell>(curent)->GetFirst()) &&
               !Is<Cell>(As<Cell>(As<Cell>(curent)->GetSecond())->GetFirst())) {
        return std::make_shared<Boolean>(true);
    }

    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> NullQ::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) {
    auto curent = As<Cell>(args)->GetFirst();
    std::shared_ptr<Object> is_quote = As<Cell>(curent)->GetFirst();
    if (Is<Symbol>(is_quote) && As<Symbol>(is_quote)->GetName() != "quote") {
        throw SyntaxError{"Неверные аргументы"};
    }
    curent = As<Cell>(curent)->GetSecond();
    curent = As<Cell>(curent)->GetFirst();

    if (!As<Cell>(curent)->GetFirst() && !As<Cell>(curent)->GetSecond()) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> ListQ::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) {
    auto curent = As<Cell>(args)->GetFirst();
    std::shared_ptr<Object> is_quote = As<Cell>(curent)->GetFirst();
    if (Is<Symbol>(is_quote) && As<Symbol>(is_quote)->GetName() != "quote") {
        throw SyntaxError{"Неверные аргументы"};
    }
    curent = As<Cell>(curent)->GetSecond();
    curent = As<Cell>(curent)->GetFirst();

    if (!As<Cell>(curent)->GetFirst() && !As<Cell>(curent)->GetSecond()) {
        return std::make_shared<Boolean>(true);
    }
    while (curent) {
        if (!Is<Cell>(As<Cell>(curent)->GetFirst()) && Is<Cell>(As<Cell>(curent)->GetSecond())) {
            curent = As<Cell>(curent)->GetSecond();
        } else if (!As<Cell>(curent)->GetSecond()) {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Plus::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Number>(0);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    for (size_t i = 0; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            if (Is<Number>(args_list[i])) {
                result += As<Number>(args_list[i])->GetValue();
            }
        } else {
            throw RuntimeError{"Plus не работает с символами"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Minus::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для Minus"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = As<Number>(args_list[0])->GetValue();
    } else {
        throw RuntimeError{"Minus не работает с символами"};
    }

    for (size_t i = 1; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            result -= As<Number>(args_list[i])->GetValue();
        } else {
            throw RuntimeError{"Minus не работает с символами"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Multiplication::Apply(std::shared_ptr<Object> args,
                                              std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Number>(1);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = As<Number>(args_list[0])->GetValue();
    } else {
        throw RuntimeError{"Multiplication не работает с символами"};
    }

    for (size_t i = 1; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            result *= As<Number>(args_list[i])->GetValue();
        } else {
            throw RuntimeError{"Multiplication не работает с символами"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Division::Apply(std::shared_ptr<Object> args,
                                        std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для Division"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = As<Number>(args_list[0])->GetValue();
    } else {
        throw RuntimeError{"Division не работает с символами"};
    }

    for (size_t i = 1; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            result /= As<Number>(args_list[i])->GetValue();
        } else {
            throw RuntimeError{"Division не работает с символами"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Max::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для max"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = As<Number>(args_list[0])->GetValue();
    } else {
        throw RuntimeError{"max не работает для символов"};
    }

    for (size_t i = 1; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            result = std::max<int64_t>(result, As<Number>(args_list[i])->GetValue());
        } else {
            throw RuntimeError{"max не работает для символов"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Min::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для min"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    int64_t result = 0;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = As<Number>(args_list[0])->GetValue();
    } else {
        throw RuntimeError{"min не работает для символов"};
    }

    for (size_t i = 1; i < args_list.size(); ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        if (Is<Number>(args_list[i])) {
            result = std::min<int64_t>(result, As<Number>(args_list[i])->GetValue());
        } else {
            throw RuntimeError{"min не работает для символов"};
        }
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Abs::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для abs"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 1) {
        std::string error_message = "Неверное количество аргументов для abs";
        throw RuntimeError{error_message};
    }
    int64_t result;
    args_list[0] = args_list[0]->Eval(scope);
    if (Is<Number>(args_list[0])) {
        result = std::abs(As<Number>(args_list[0])->GetValue());
    } else {
        throw RuntimeError{"abs не применим к символам"};
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Equal::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() == 1) {
        args_list[0] = args_list[0]->Eval(scope);
        if (Is<Number>(args_list[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            throw RuntimeError{"Equal не работает с символами"};
        }
    }
    for (size_t i = 0; i < args_list.size() - 1; ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        args_list[i + 1] = args_list[i + 1]->Eval(scope);
        if (Is<Number>(args_list[i]) && Is<Number>(args_list[i + 1])) {
            if (As<Number>(args_list[i])->GetValue() != As<Number>(args_list[i + 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError{"Equal не работает с символами"};
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Less::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() == 1) {
        args_list[0] = args_list[0]->Eval(scope);
        if (Is<Number>(args_list[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            throw RuntimeError{"Less не работает с символами"};
        }
    }
    for (size_t i = 0; i < args_list.size() - 1; ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        args_list[i + 1] = args_list[i + 1]->Eval(scope);
        if (Is<Number>(args_list[i]) && Is<Number>(args_list[i + 1])) {
            if (As<Number>(args_list[i])->GetValue() >= As<Number>(args_list[i + 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError{"Less не работает с символами"};
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> LessEquals::Apply(std::shared_ptr<Object> args,
                                          std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() == 1) {
        args_list[0] = args_list[0]->Eval(scope);
        if (Is<Number>(args_list[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            throw RuntimeError{"LessEquals не работает с символами"};
        }
    }
    for (size_t i = 0; i < args_list.size() - 1; ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        args_list[i + 1] = args_list[i + 1]->Eval(scope);
        if (Is<Number>(args_list[i]) && Is<Number>(args_list[i + 1])) {
            if (As<Number>(args_list[i])->GetValue() > As<Number>(args_list[i + 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError{"LessEquals не работает с символами"};
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> More::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() == 1) {
        args_list[0] = args_list[0]->Eval(scope);
        if (Is<Number>(args_list[0])) {

            return std::make_shared<Boolean>(true);
        } else {
            throw RuntimeError{"More не работает с символами"};
        }
    }
    for (size_t i = 0; i < args_list.size() - 1; ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        args_list[i + 1] = args_list[i + 1]->Eval(scope);
        if (Is<Number>(args_list[i]) && Is<Number>(args_list[i + 1])) {
            if (As<Number>(args_list[i])->GetValue() <= As<Number>(args_list[i + 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError{"More не работает с символами"};
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> MoreEquals::Apply(std::shared_ptr<Object> args,
                                          std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() == 1) {
        args_list[0] = args_list[0]->Eval(scope);
        if (Is<Number>(args_list[0])) {
            return std::make_shared<Boolean>(true);
        } else {
            throw RuntimeError{"MoreEquals не работает с символами"};
        }
    }
    for (size_t i = 0; i < args_list.size() - 1; ++i) {
        args_list[i] = args_list[i]->Eval(scope);
        args_list[i + 1] = args_list[i + 1]->Eval(scope);
        if (Is<Number>(args_list[i]) && Is<Number>(args_list[i + 1])) {
            if (As<Number>(args_list[i])->GetValue() < As<Number>(args_list[i + 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError{"MoreEquals не работает с символами"};
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Not::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        throw RuntimeError{"Нет аргументов для not"};
    }
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 1) {
        std::string error_message = "Неверное количество аргументов для Not";
        throw RuntimeError{error_message};
    }
    if (Is<Boolean>(args_list[0])) {
        if (!As<Boolean>(args_list[0])->GetBool()) {
            return std::make_shared<Boolean>(true);
        }
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> And::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(true);
    }

    std::shared_ptr<Object> operand;

    std::shared_ptr<Cell> curent = As<Cell>(args);
    while (curent) {
        operand = curent->GetFirst();
        if (Is<Cell>(operand)) {
            operand = curent->GetFirst()->Eval(scope);
        }
        if (Is<Boolean>(operand)) {
            if (!As<Boolean>(operand)->GetBool()) {
                return std::make_shared<Boolean>(false);
            }
        }
        if (curent->GetSecond()) {
            curent = As<Cell>(curent->GetSecond());
        } else if (!curent->GetSecond()) {
            break;
        }
    }
    return operand;
}

std::shared_ptr<Object> Or::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Boolean>(false);
    }

    std::shared_ptr<Object> operand;

    std::shared_ptr<Cell> curent = As<Cell>(args);
    while (curent) {
        operand = curent->GetFirst();
        if (Is<Cell>(operand)) {
            operand = curent->GetFirst()->Eval(scope);
        }
        if (Is<Boolean>(operand)) {
            if (As<Boolean>(operand)->GetBool()) {
                return std::make_shared<Boolean>(true);
            }
        }
        if (curent->GetSecond()) {
            curent = As<Cell>(curent->GetSecond());
        } else if (!curent->GetSecond()) {
            break;
        }
    }
    return operand;
}

std::shared_ptr<Object> Define::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (args && Is<Cell>(As<Cell>(args)->GetFirst())) {
        auto lambda = std::make_shared<Lambda>();
        auto dec_arg = As<Cell>(As<Cell>(args)->GetFirst())->GetSecond();
        lambda->DeclarationOfArguments(dec_arg);
        lambda->SugarExpression(As<Cell>(args)->GetSecond());

        lambda->SetFlag("defined");

        std::string variable_name =
            As<Symbol>(As<Cell>(As<Cell>(args)->GetFirst())->GetFirst())->GetName();
        scope->SetElementScope(variable_name, lambda);
        return std::make_shared<Boolean>(true);
    }

    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 2) {
        throw SyntaxError{"Неверное количество аргументов для Define"};
    } else if (!Is<Symbol>(args_list[0]) && !Is<Number>(args_list[1])) {
        throw SyntaxError{"Неверные аргументы для Define"};
    }
    std::string variable_name = As<Symbol>(args_list[0])->GetName();
    scope->SetElementScope(variable_name, args_list[1]);
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Set::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 2) {
        throw SyntaxError{"Неверное количество аргументов для Set"};
    } else if (!Is<Symbol>(args_list[0]) && !Is<Number>(args_list[1])) {
        throw SyntaxError{"Неправильные аргументы для Set"};
    }
    std::string variable_name = As<Symbol>(args_list[0])->GetName();
    scope->GetElementScope(variable_name);
    scope->SetElementScope(variable_name, args_list[1]);
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> If::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    int number_of_arguments = NumberOfArguments(args);
    if (number_of_arguments != 2 && number_of_arguments != 3) {
        throw SyntaxError{"Неверное количество аргументов для If"};
    }

    std::shared_ptr<Cell> curent = As<Cell>(args);
    std::shared_ptr<Object> condition = curent->GetFirst()->Eval(scope);
    if (Is<Boolean>(condition) && !As<Boolean>(condition)->GetBool()) {
        if (number_of_arguments == 2) {
            return std::make_shared<Symbol>("()");
        } else {
            std::shared_ptr<Object> cell_with_second_value =
                As<Cell>(As<Cell>(args)->GetSecond())->GetSecond();
            std::shared_ptr<Object> second_value =
                As<Cell>(cell_with_second_value)->GetFirst()->Eval(scope);
            return second_value;
        }
    } else {
        std::shared_ptr<Object> cell_with_first_value = As<Cell>(args)->GetSecond();
        std::shared_ptr<Object> first_value =
            As<Cell>(cell_with_first_value)->GetFirst()->Eval(scope);
        return first_value;
    }
}

std::shared_ptr<Object> Cons::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    if (args_list.size() != 2) {
        throw SyntaxError{"Неверное количество аргументов для cons"};
    }

    return std::make_shared<Pair>(As<Number>(args_list[0])->GetValue(),
                                  As<Number>(args_list[1])->GetValue());
}

std::shared_ptr<Object> Car::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);

    if (args_list.size() != 1) {
        throw SyntaxError{"Введена не пара"};
    }

    if (Is<Symbol>(args_list[0])) {
        args_list[0] = args_list[0]->Eval(scope);
    } else if (!Is<Pair>(args_list[0])) {
        throw SyntaxError{"Введена не пара"};
    }

    return std::make_shared<Number>(As<Pair>(args_list[0])->GetElementOne());
}

std::shared_ptr<Object> Cdr::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);

    if (args_list.size() != 1) {
        throw SyntaxError{"Введена не пара"};
    }

    if (Is<Symbol>(args_list[0])) {
        args_list[0] = args_list[0]->Eval(scope);
    } else if (!Is<Pair>(args_list[0])) {
        throw SyntaxError{"Введена не пара"};
    }

    return std::make_shared<Number>(As<Pair>(args_list[0])->GetElementTwo());
}

std::shared_ptr<Object> SetCar::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);

    if (args_list.size() != 2) {
        throw SyntaxError{"Неверное количество аргументов для set-car!"};
    }
    if (!Is<Symbol>(args_list[0]) && !Is<Number>(args_list[1])) {
        throw SyntaxError{"Неправильные аргументы для set-car!"};
    }

    std::string variable_name = As<Symbol>(args_list[0])->GetName();

    auto pair = scope->GetElementScope(variable_name);
    if (Is<Pair>(pair)) {
        As<Pair>(pair)->SetElementOne(As<Number>(args_list[1])->GetValue());
    } else {
        throw RuntimeError{"Неверные аргументы для set-car!"};
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> SetCdr::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);

    if (args_list.size() != 2) {
        throw SyntaxError{"Неверное количество аргументов для set-car!"};
    }
    if (!Is<Symbol>(args_list[0]) && !Is<Number>(args_list[1])) {
        throw SyntaxError{"Неправильные аргументы для set-car!"};
    }

    std::string variable_name = As<Symbol>(args_list[0])->GetName();

    auto pair = scope->GetElementScope(variable_name);
    if (Is<Pair>(pair)) {
        As<Pair>(pair)->SetElementTwo(As<Number>(args_list[1])->GetValue());
    } else {
        throw RuntimeError{"Неверные аргументы для set-car!"};
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> List::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Symbol>("()");
    }

    std::vector<std::shared_ptr<Object>> args_list = EvalList(args, scope);
    return std::make_shared<ListObj>(args_list);
}

std::shared_ptr<Object> ListRef::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Symbol>("()");
    }
    int pos = As<Number>(As<Cell>(As<Cell>(args)->GetSecond())->GetFirst())->GetValue();
    std::vector<std::shared_ptr<Object>> args_list =
        EvalList(As<Cell>(As<Cell>(As<Cell>(args)->GetFirst())->GetSecond())->GetFirst(), scope);
    if (pos >= static_cast<int>(args_list.size())) {
        throw RuntimeError{"Вышли за диапозон list"};
    }
    return std::make_shared<Symbol>(std::to_string(As<Number>(args_list[pos])->GetValue()));
}

std::shared_ptr<Object> ListTail::Apply(std::shared_ptr<Object> args,
                                        std::shared_ptr<Scope> scope) {
    if (!args) {
        return std::make_shared<Symbol>("()");
    }
    int pos = As<Number>(As<Cell>(As<Cell>(args)->GetSecond())->GetFirst())->GetValue();
    std::vector<std::shared_ptr<Object>> args_list =
        EvalList(As<Cell>(As<Cell>(As<Cell>(args)->GetFirst())->GetSecond())->GetFirst(), scope);

    if (pos > static_cast<int>(args_list.size())) {
        throw RuntimeError{"Вышли за диапозон list"};
    } else if (pos == static_cast<int>(args_list.size())) {
        return std::make_shared<Symbol>("()");
    }
    std::vector<std::shared_ptr<Object>> res;
    for (int i = pos; i < static_cast<int>(args_list.size()); ++i) {
        res.push_back(args_list[i]);
    }
    return std::make_shared<ListObj>(res);
}

Lambda::Lambda() : scope_(std::make_shared<Scope>()) {
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

void Lambda::DeclarationOfArguments(std::shared_ptr<Object> args) {
    std::string name_of_variables;
    while (args) {
        name_of_variables = As<Symbol>(As<Cell>(args)->GetFirst())->GetName();
        arguments_.push_back(name_of_variables);
        scope_->SetElementScope(name_of_variables, std::make_shared<Number>(0));
        args = As<Cell>(args)->GetSecond();
    }
}

void Lambda::DefinitionOfArguments(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    for (auto argument : arguments_) {
        auto values = As<Cell>(args)->GetFirst()->Eval(scope);
        scope_->SetElementScope(argument, values);
        args = As<Cell>(args)->GetSecond();
    }
}

void Lambda::SugarExpression(std::shared_ptr<Object> expression) {
    expression_.push_back(As<Cell>(expression)->GetFirst());
}

std::shared_ptr<Object> Lambda::Defines(std::shared_ptr<Object> args,
                                        std::shared_ptr<Scope> scope) {
    int number_of_arguments = NumberOfArguments(args);
    std::shared_ptr<Object> arguments = As<Cell>(args)->GetFirst();
    DeclarationOfArguments(arguments);
    scope_->SetParentScope(scope);
    args = As<Cell>(args)->GetSecond();
    for (int i = 1; i < number_of_arguments; ++i) {
        expression_.push_back(As<Cell>(args)->GetFirst());
        args = As<Cell>(args)->GetSecond();
    }
    flag_ = "defined";
    return shared_from_this();
}

std::shared_ptr<Object> Lambda::Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope) {
    int number_of_arguments = NumberOfArguments(args);
    if (flag_ == "undefined" && number_of_arguments <= 1) {
        throw SyntaxError{"Неверное количество аргументов для Lambda"};
    }
    if (flag_ == "undefined") {
        Defines(args, scope);
    } else if (flag_ == "defined") {
        DefinitionOfArguments(args, scope);
        for (size_t i = 0; i < expression_.size(); ++i) {
            if (i + 1 == expression_.size()) {
                return expression_[i]->Eval(scope_);
            }
            expression_[i]->Eval(scope_);
        }
    }
    return shared_from_this();
}

int NumberOfArguments(std::shared_ptr<Object> args) {
    std::shared_ptr<Cell> curent = As<Cell>(args);
    int number_of_arguments = 0;
    while (curent) {
        ++number_of_arguments;
        curent = As<Cell>(curent->GetSecond());
    }
    return number_of_arguments;
}

std::vector<std::shared_ptr<Object>> EvalList(std::shared_ptr<Object> args,
                                              std::shared_ptr<Scope> scope) {
    std::vector<std::shared_ptr<Object>> args_list;
    std::vector<std::shared_ptr<Object>> args_list_new;
    std::shared_ptr<Cell> curent = As<Cell>(args);
    while (curent) {
        if (Is<Cell>(curent->GetFirst())) {
            args_list.push_back(curent->GetFirst()->Eval(scope));
        } else {
            args_list.push_back(curent->GetFirst());
        }
        if (!As<Cell>(curent->GetSecond()) && Is<Number>(curent->GetSecond())) {
            args_list.push_back(curent->GetSecond());
            break;
        }
        if (curent->GetSecond()) {
            curent = As<Cell>(curent->GetSecond());
        } else {
            break;
        }
    }

    for (size_t i = 0; i < args_list.size(); ++i) {
        if (Is<Cell>(args_list[i])) {
            auto current = As<Cell>(args_list[i])->GetFirst();
            auto first = As<Cell>(current)->GetFirst();
            auto second = As<Cell>(current)->GetSecond();
            if (Is<Number>(first) && Is<Number>(second)) {
                args_list_new.push_back(std::make_shared<Pair>(As<Number>(first)->GetValue(),
                                                               As<Number>(second)->GetValue()));
            } else if (Is<Number>(first) && Is<Number>(As<Cell>(second)->GetFirst())) {
                args_list_new.push_back(
                    std::make_shared<Pair>(As<Number>(first)->GetValue(),
                                           As<Number>(As<Cell>(second)->GetFirst())->GetValue()));
            } else {
                args_list_new.push_back(args_list[i]);
            }
        } else {
            args_list_new.push_back(args_list[i]);
        }
    }

    return args_list_new;
}
