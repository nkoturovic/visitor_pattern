#ifndef VISITOR_PATTERN_H
#define VISITOR_PATTERN_H

#include <memory> // std::move

/* Ova klasa rekurzivno nasledjuje sama sebe i tako pravi 
 * virtualni visit metod za svaku od Derived klasa */
template<typename Base, typename Derived, typename... Ds>
class Visitor : public Visitor<Base,Ds...> {

    template<typename IDerived, typename IBase, typename IBaseH, typename... IDs>
    friend class InheritVisitable;

protected:
    using Visitor<Base,Ds...>::visit;

    virtual void visit(Derived &visitable) { 
        otherwise(visitable);
    }
    virtual void otherwise(Base &) = 0; 
    // Mozda je moglo i: { std::cerr << "[Warning]: Otherwise
    // member function is being used, but not implemented!\n"; }

public:
    using visit_base_type = Base;
    using visit_return_type = void;
    void operator()(Base &b) { 
        b.accept(*this);
    }
};

/* ^^^ Bazni slucaj (samo jedna klasa) ^^^ */
template<typename Base, typename Derived>
class Visitor<Base,Derived> {
protected:
    virtual void visit(Derived &visitable) { 
        otherwise(visitable);
    }
    virtual void otherwise(Base &) = 0;
    // Mozda je moglo i: { std::cerr << "[Warning]: Otherwise 
    // member function is being used, but not implemented!\n"; }
};

/* Visitor koji ima Result (povratnu vrednost) */
template <typename Visitor, typename ResultType>
class VisitorWithResult : public Visitor {
protected:
    ResultType m_result;
    void result(ResultType r) { m_result = std::move(r); }
public:
    using visit_base_type = typename Visitor::visit_base_type;
    using visit_return_type = ResultType;

    ResultType&& operator()(visit_base_type &b) {
        b.accept(static_cast<Visitor&>(*this));
        return std::move(m_result);
    }
};

/* Ovu klasu nasledjuje bazna klasa (Base) */
template<typename Base, typename... Ds>
class Visitable {
    template <typename RVisitor, typename RResultType>
    friend class VisitorWithResult;

    template<typename VDerived, typename VBase, typename... VDs>
    friend class Visitor;

    virtual void accept(Visitor<Base,Ds...> &visitor) const = 0;
    virtual void accept(Visitor<const Base,const Ds...> &visitor) const {
        this->accept(reinterpret_cast<Visitor<Base, Ds...>&>(visitor));
    }
};

/* Nasledjivanjem ove klase automatski se nasledjuje i bazna
 * klasa. Ovu klasu nasledjuju izvedene (Derived) klase */
template<typename Derived, typename Base, typename BaseH, typename... Ds>
class InheritVisitable : public Base {
public:
    void accept(Visitor<BaseH,Ds...> &visitor) const override {
        if constexpr (std::is_const<BaseH>::value) {
            visitor.visit(static_cast<const Derived&>(*this));
        } else {
            visitor.visit(const_cast<Derived&>(static_cast<const Derived&>(*this)));
        }
    }
};
#endif // VISITOR_PATTERN_H
