#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "error.h"

class Object;

class Scope {
public:
    void SetParentScope(std::shared_ptr<Scope> parent_scope);

    void SetElementScope(std::string symbol, std::shared_ptr<Object> object);

    std::shared_ptr<Object> GetElementScope(std::string symbol);

private:
    std::map<std::string, std::shared_ptr<Object>> scope_;
    std::shared_ptr<Scope> parent_scope_ = nullptr;
};

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope>);

    virtual std::shared_ptr<Object> Apply(std::shared_ptr<Object>, std::shared_ptr<Scope>);

    virtual std::string Print();

    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int value);

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope>) override;

    std::string Print() override;

    int GetValue() const;

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol(std::string name);

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Print() override;

    const std::string& GetName() const;

private:
    std::string name_;
};

class Boolean : public Object {
public:
    Boolean(bool name);

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope>) override;

    const bool& GetBool() const;

    std::string Print() override;

private:
    bool bool_;
};

class Pair : public Object {
public:
    Pair(int element_one, int element_two);

    std::string Print() override;

    void SetElementOne(int element_one);

    void SetElementTwo(int element_two);

    const int& GetElementOne() const;

    const int& GetElementTwo() const;

private:
    int element_one_;
    int element_two_;
};

class ListObj : public Object {
public:
    ListObj(std::vector<std::shared_ptr<Object>> object_shared_ptr);

    std::string Print() override;

private:
    std::vector<std::shared_ptr<Object>> object_shared_ptr_;
};

class Cell : public Object {
public:
    Cell();

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second);

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Print() override;

    std::shared_ptr<Object> GetFirst() const;

    std::shared_ptr<Object> GetSecond() const;

    void SetFirst(std::shared_ptr<Object> first);

    void SetSecond(std::shared_ptr<Object> second);

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class QuoteSpecForm : public Object {
public:
    QuoteSpecForm() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) override;
};

class NumberQ : public Object {
public:
    NumberQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class SymbolQ : public Object {
public:
    SymbolQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class BooleanQ : public Object {
public:
    BooleanQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class PairQ : public Object {
public:
    PairQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) override;
};

class NullQ : public Object {
public:
    NullQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) override;
};

class ListQ : public Object {
public:
    ListQ() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args, std::shared_ptr<Scope>) override;
};

class Plus : public Object {
public:
    Plus() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Minus : public Object {
public:
    Minus() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Multiplication : public Object {
public:
    Multiplication() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Division : public Object {
public:
    Division() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Max : public Object {
public:
    Max() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Min : public Object {
public:
    Min() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Abs : public Object {
public:
    Abs() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Equal : public Object {
public:
    Equal() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Less : public Object {
public:
    Less() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class LessEquals : public Object {
public:
    LessEquals() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class More : public Object {
public:
    More() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class MoreEquals : public Object {
public:
    MoreEquals() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Not : public Object {
public:
    Not() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class And : public Object {
public:
    And() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Or : public Object {
public:
    Or() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Define : public Object {
public:
    Define() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

int NumberOfArguments(std::shared_ptr<Object> args);

std::vector<std::shared_ptr<Object>> EvalList(std::shared_ptr<Object> args,
                                              std::shared_ptr<Scope> scope);

class Set : public Object {
public:
    Set() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class If : public Object {
public:
    If() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Cons : public Object {
public:
    Cons() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Car : public Object {
public:
    Car() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Cdr : public Object {
public:
    Cdr() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class SetCar : public Object {
public:
    SetCar() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class SetCdr : public Object {
public:
    SetCdr() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class List : public Object {
public:
    List() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class ListRef : public Object {
public:
    ListRef() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class ListTail : public Object {
public:
    ListTail() = default;

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;
};

class Lambda : public Object {
public:
    Lambda();

    void SugarExpression(std::shared_ptr<Object> expression);

    void DeclarationOfArguments(std::shared_ptr<Object> variables);

    void DefinitionOfArguments(std::shared_ptr<Object> variables, std::shared_ptr<Scope> scope);

    std::shared_ptr<Object> Defines(std::shared_ptr<Object> args, std::shared_ptr<Scope> scope);

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> args,
                                  std::shared_ptr<Scope> scope) override;

    void SetFlag(std::string flag) {
        flag_ = flag;
    }

private:
    std::vector<std::string> arguments_;
    std::shared_ptr<Scope> scope_;
    std::vector<std::shared_ptr<Object>> expression_;
    std::string flag_ = "undefined";
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return dynamic_cast<T*>(obj.get());
}