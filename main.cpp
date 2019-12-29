#include <iostream>
#include "visitor_pattern.hpp"

/* Deklaracije klasa */
class B; class D1; class D2; class D3; class D4;

/* ConstTypeHierarchy nam treba za const Visitor-e */
#define ConstTypeHierarchy const B,const D1,const D2,const D3,const D4
/* TypeHierarchy nam treba za non-const Visitor-e */
#define TypeHierarchy B,D1,D2,D3,D4

/* Preimenovanje generickih Visitor-a u specificne za nasu hijerarhiju 
 * ovde pravimo nas interfejs (I) koji cini bazne klase za sve vizitore */

/* 1) Vizitor bez rezultata (Ref verzija) */
using MyVisitorI = Visitor<TypeHierarchy>;

/* 2) Vizitor bez rezultata (ConstRef verzija) */
using MyConstVisitorI = Visitor<ConstTypeHierarchy>;

/* 3) Vizitor sa rezultatom(retval) (Ref verzija) */
template <class ResultType>
using MyVisitorWithResultI = VisitorWithResult<MyVisitorI, ResultType>;

/* 4) Vizitor sa rezultatom(retval) (Ref verzija) */
template <class ResultType>
using MyConstVisitorWithResultI = VisitorWithResult<MyConstVisitorI, ResultType>;

/************** Bazna klasa **************/
class B : public Visitable<TypeHierarchy> {
public:
    int val = 0;
    virtual B& operator=(const B& other) = delete;
    virtual B& operator=(B&& other) = delete;
    virtual ~B() = default;
};

/***************** Izvedene klase ********************
 * Nasledjivanje bazne klase B se desava automatski  *
 * (zbog dijamant <> problema "moralo" na taj nacin) */
class D1 : public InheritVisitable<D1,B,TypeHierarchy> {};
class D2 : public InheritVisitable<D2,B,TypeHierarchy> {};
class D3 : public InheritVisitable<D3,B,TypeHierarchy> {};
class D4 : public InheritVisitable<D4,B,TypeHierarchy> {};

int main()
{
    /* Posetilac (ConstRef) - ne menja stanje objekta */
    class PrintVisitor : public MyConstVisitorI {
        void visit(const D1 &) override { std::cout << "d1\n"; }
        void visit(const D2 &) override { std::cout << "d2\n"; }
        void otherwise(const B &) override { std::cout << "Nesto drugo\n"; }
    } print;

    D1 d1; D2 d2; const B& b = d1;
    print(b);
    print(d1);
    print(d2);

    /* 'otherwise' se uvek mora implementirati (cak i ako sve ostale navedemo) */
    class ToStringVisitor : public MyConstVisitorWithResultI<std::string> {
        void visit(const D1 &) override { result("d1"); }
        void visit(const D2 &) override { result("d2"); }
        void otherwise(const B &) override { 
            /* Bez obzira sto smo naveli sve izvedene klase. 
             * Moze se desiti da dodamo novu izvedenu klasu
             * a ne izmenimo Visitor da radi i za nju!!! */
            std::cerr << "[ToString]: Ovaj slucaj nije obradjen "
                      << "(Vracam podrazumevani string)"
                      << std::endl;
            result("B"); 
        }
    } toStr;

    std::cout << toStr(b) << '\n';
    std::cout << toStr(d1) << '\n';
    D3 d3; const B &b3 = d3;
    std::cout << toStr(b3) << '\n';

    /* Posetilac koji menja objekat (Ref verzija) */
    class UpdateValVisitor : MyVisitorI {
        void visit(D1 &d1) override { d1.val = 1; }
        void visit(D2 &d2) override { d2.val = 2; }
        void visit(D3 &d3) override { d3.val = 3; }
        // void visit(D4 &d4) override { d4.val = 4; }
        void otherwise(B &b) override { b.val = 0; }
    } updateVal;

    /* Posetilac (Ref ver.) koji menja objekat i vraca rezultat (povratnu vrednost) */
    class UpdateValAndReturnVisitor : public MyVisitorWithResultI<int> {
        void visit(D1 &d1) override { d1.val = 1; result(d1.val); }
        void visit(D2 &d2) override { d2.val = 2; result(d2.val); }
        void visit(D3 &d3) override { d3.val = 3; result(d3.val); }
        // void visit(D4 &d4) override { d4.val = 4; }
        void otherwise(B &b) override { b.val = 0; result(b.val); }
    } updateValAndReturn;

    B& b2 = d2;
    std::cout << updateValAndReturn(b2) << '\n';
    std::cout << b2.val << '\n';
    D4 d4; B& b4 = d4;
    std::cout << updateValAndReturn(b4) << '\n';
    std::cout << b4.val << '\n';

    return 0;
}
