#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include "my_vector.h"
#include <iosfwd>
#include <cstdint>
#include <utility>
#include <gmp.h>
#include <vector>

struct big_integer
{
private:
    typedef uint32_t ui;
    typedef uint64_t ull;
public:
    big_integer();
    big_integer(big_integer const& other) = default;
    big_integer(int a);
    big_integer(ui a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;

    big_integer& operator=(big_integer const& other);

    friend std::string to_string(big_integer const& a);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

private:
    my_vector data_;
    bool sign_ = 0;
    size_t size_ = 0;

    big_integer& clear(big_integer &a, size_t size);
    big_integer& resize(size_t capacity);
    big_integer& abs_add(big_integer const& rhs, bool sign);
    big_integer& abs_sub(big_integer const& rhs, bool sign, int comp);

    template<class FunctorT>
    big_integer& apply_bitwise_operation(big_integer const & rhs, FunctorT functor);

    void add_big_int(my_vector &a, ui x, size_t index);
    static void swap(big_integer &a, big_integer &b);
    static int abs_compare(big_integer const& a, big_integer const& b);
    static void normalize(big_integer &a);
    big_integer inverse(big_integer a);

};
big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H