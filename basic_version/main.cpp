#include <iostream>
#include "visitor_pattern_basic.hpp"

struct B; struct D1; struct D2;
#define TypeHierarchy B,D1,D2

using MyBaseVisitor = Visitor<TypeHierarchy>;

template <class ResultType>
using MyBaseVisitorWithResult = VisitorWithResult<MyBaseVisitor, ResultType>;

struct B : Visitable<TypeHierarchy> {};

/* Nasledjivanje bazne klase B se desava automatski */
struct D1 : InheritVisitable<D1,TypeHierarchy> {};

/* Nasledjivanje bazne klase B se desava automatski */
struct D2 : InheritVisitable<D2,TypeHierarchy> {};

int main()
{
    struct PrintVisitor : MyBaseVisitor {
        using Visitor::visit; // omogucava print(B &b);
        void visit(D1 &) override { std::cout << "d1\n"; }
        void visit(D2 &) override { std::cout << "d2\n"; }
    } print;

    D1 d1; D2 d2; B& b = d2;
    print(b); // <=> b.accept(print); 
    print(d1); // <=> d1.accept(print); 
    print(d2); // <=> d2.accept(print); 

    struct ToStringVisitor : MyBaseVisitorWithResult<std::string> {
        void visit(D1 &) override { result("d1"); }
        void visit(D2 &) override { result("d2"); }
    } toStr;

    std::cout << toStr(b) << '\n';
    std::cout << toStr(d1) << '\n';
    std::cout << toStr(d2) << '\n';

    return 0;
}
