//
// Created by Quentin on 25/05/2025.
//

#ifndef CLAUSE_HH
#define CLAUSE_HH

namespace fabko::sat {

/**
 * Variable in the CNF representation.
 * This value is an index into the memory object and thus is not abstracted.
 */
using variable = unsigned;

/**
 * Represent a literal in the CNF representation.
 *
 * The value of the literal is equivalent to the var it represents mult by 2
 *  +1 if the var is positive
 *  +0 if the var is negative
 */
class literal {

  public:
    explicit literal(variable var, bool positive = true) : _val(var + var + static_cast<unsigned>(positive)) {}

    bool operator<=>(const literal& other) const = default;

    literal operator~() const {
        auto tmp = literal(*this);
        tmp._val ^= 1;
        return tmp;
    }

    //! true or false depending on the current assignment designation of the literal
    explicit operator bool() const {
        const auto v = _val;
        return (v & 1) == 1;
    }

    [[nodiscard]] variable var() const {
        const auto v = _val;
        return v >> 1;
    };

    [[nodiscard]] unsigned value() const { return _val; };

    friend std::string to_string(const literal&);

  private:
    unsigned _val;
};

enum class solver_status : unsigned {
    BUILDING = 0,
    SOLVING  = 1,
    SAT      = 2,
    UNSAT    = 3,
};

// using clause = std::vector<literal>;

/**
 * A clause is a set of literals linked by disjunction (∨).
 * It represents a single constraint in the SAT formula that must be satisfied.
 * A clause is satisfied if at least one of its literals is true.
 * For example, (x1 ∨ ¬x2 ∨ x3) is a clause that is satisfied if either:
 * - x1 is true, or
 * - x2 is false, or
 * - x3 is true
 */
class clause {
  public:
    enum class clause_type {
        initial, //!< original clause from the input formula. These clauses are always kept during solving.
        learned, //!< clause derived during conflict analysis. These clauses can be forgotten during solving to manage memory.
    };

    const std::vector<literal>& get() const { return _literals; }
    bool empty() const { return _literals.empty(); }

    const literal& get(std::size_t index_clause) const {
        fabko_assert(index_clause < _literals.size(), "indexClause out of bound");
        return _literals[index_clause];
    }

    //!< @return the type of this clause, either initial (from input formula) or learned (derived during solving).
    clause_type type() const { return _type; }

  private:
    std::vector<literal> _literals;          //!< collection of literals that form this clause, connected by disjunction
    clause_type _type{clause_type::initial}; //!< type of the clause
};
} // namespace fabko::sat

#endif //CLAUSE_HH
