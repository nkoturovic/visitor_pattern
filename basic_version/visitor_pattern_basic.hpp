#ifndef VISITOR_PATTERN_BASIC_H
#define VISITOR_PATTERN_BASIC_H

#include <memory> // std::move

/******************************************************************************
 * Ovaj primer je prvi nastao i nije u potpunosti isti kao advanced primer    *
 * iz (../) dir. Advanced primeru su neke stvari promenjene (interfejs), ali  *
 * je ovo odlicna ilustracija sustine implementacije (iza) advanced primera   *
 *****************************************************************************/

/* Ova klasa rekurzivno nasledjuje sama sebe i tako pravi 
 * virtualni visit metod za svaku od Derived klasa */
template<typename Base, typename Derived, typename... Ds>
struct Visitor : Visitor<Base,Ds...> {
    using visit_base_type = Base;
    using Visitor<Base,Ds...>::visit;

    virtual void visit(Derived & visitable) = 0;
    void operator()(Base &b) { b.accept(*this); }
};

/* ^^^ Bazni slucaj (samo jedna klasa) ^^^ */
template<typename Base, typename Derived>
struct Visitor<Base,Derived> {
    virtual void visit(Derived &visitable) = 0;
};

/* Visitor koji ima Result (povratnu vrednost) */
template <typename Visitor, typename ResultType>
struct VisitorWithResult : Visitor {
private:
    ResultType m_result;
public:
    ResultType result() const { return m_result; }
    void result(ResultType &&r) { m_result = std::move(r); }

    ResultType operator()(typename Visitor::visit_base_type &b) {
        b.accept(*this);
        return this->result();
    }
};

/* Ovu klasu nasledjuje bazna klasa (Base) */
template<typename Base, typename... Ds>
struct Visitable {
    virtual void accept(Visitor<Base,Ds...> &visitor);
};

/* Nasledjivanjem ove klase automatski se nasledjuje i bazna
 * klasa. Ovu klasu nasledjuju izvedene (Derived) klase */
template<typename Derived, typename Base, typename... Ds>
struct InheritVisitable : Base {
    void accept(Visitor<Base,Ds...> &visitor) override {
        visitor.visit(static_cast<Derived&>(*this));
    }
};

#endif // VISITOR_PATTERN_BASIC_H
