#include "big_integer.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>

typedef uint32_t ui;
typedef uint64_t ull;

ui const SHIFT = 32;
ui const UMAX = UINT32_MAX;
ull const SHIFTED = (ull) 1 << SHIFT;

big_integer &big_integer::clear(big_integer &a, size_t size) {
    a.sign_ = 0;
    a.size_ = size;
    a.data_.assign(size_, 0);
    return a;
}

big_integer &big_integer::resize(size_t size) {
    data_.resize(size + 1, 0);
    return *this;
}

big_integer::big_integer() {
    clear(*this, 1);
}

ui cast(int x) {
    return static_cast<ui>(abs(x));
}

template<typename T>
ui cast(T x) {
    return static_cast<ui>(x);
}

big_integer::big_integer(int a)
        : data_(1, cast(a)), sign_(a < 0), size_(1) {
}

big_integer::big_integer(ui a) {
    clear(*this, 1);
    data_[0] = a;
}

big_integer::big_integer(std::string const &str) {
    bool sign = str[0] == '-';
    for (size_t i = cast(sign); i < str.length(); i++) {
        *this *= 10;
        *this += str[i] - '0';
    }
    if (*this != 0)
        sign_ = sign;
}

void big_integer::swap(big_integer &a, big_integer &b) {
    std::swap(a.data_, b.data_);
    std::swap(a.sign_, b.sign_);
    std::swap(a.size_, b.size_);
}

big_integer &big_integer::operator=(big_integer const &other) {
    big_integer temp(other);
    swap(*this, temp);
    return *this;
}

big_integer &big_integer::abs_add(big_integer const &rhs, bool sign) {
    sign_ = sign;
    size_t m = std::max(size_, rhs.size_);
    if (m >= size_ + 1) {
        *this = resize(m + 1);
    }
    ull sum = 0;
    bool carry = 0;
    for (size_t i = 0; i < m; i++) {
        sum = (ull) data_[i] + (rhs.size_ > i ? rhs.data_[i] : 0) + carry;
        carry = (sum >> SHIFT) > 0;
        data_[i] = cast(sum & UMAX);
    }

    size_ = m;
    if (carry != 0) {
        data_[m] = cast(carry);
        size_++;
    }

    return *this;
}

big_integer &big_integer::abs_sub(big_integer const &rhs, bool sign, int comp) {
    if (comp == -1) {
        big_integer temp(rhs);
        swap(*this, temp);
        return abs_sub(temp, sign, 1);
    }

    ull buf = 0;
    bool carry = 0;
    for (size_t i = 0; i < size_; i++) {
        buf = SHIFTED + data_[i];
        buf -= (rhs.size_ > i ? rhs.data_[i] : 0) + carry;
        if (buf >> SHIFT == 0)
            carry = 1;
        else carry = 0;
        data_[i] = cast(buf & UMAX);
    }

    normalize(*this);
    sign_ = sign;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (!sign_ && !rhs.sign_)
        return abs_add(rhs, 0);
    if (sign_ && rhs.sign_)
        return abs_add(rhs, 1);

    int comp = abs_compare(*this, rhs);
    if (comp == 0)
        return clear(*this, 1);

    if (sign_ && !rhs.sign_) {
        if (comp == -1) // abs(this) < abs(rhs)
            return abs_sub(rhs, 0, comp);
        else return abs_sub(rhs, 1, comp);
    } else {
        if (comp == 1) // abs(this) > abs(rhs)
            return abs_sub(rhs, 0, comp);
        else return abs_sub(rhs, 1, comp);
    }
}


big_integer &big_integer::operator-=(big_integer const &rhs) {
    if (sign_ && !rhs.sign_)
        return abs_add(rhs, 1);
    if (!sign_ && rhs.sign_)
        return abs_add(rhs, 0);

    int comp = abs_compare(*this, rhs);
    if (comp == 0)
        return clear(*this, 1);

    if (sign_ && rhs.sign_) {
        if (comp == -1) // abs(this) < abs(rhs)
            return abs_sub(rhs, 0, comp);
        else return abs_sub(rhs, 1, comp);
    } else {
        if (comp == 1) // abs(this) > abs(rhs)
            return abs_sub(rhs, 0, comp);
        else return abs_sub(rhs, 1, comp);
    }
}

void big_integer::add_big_int(my_vector &a, ui x, size_t index) {
    ull sum = x;
    bool carry = 0;
    while (true) {
        sum += a[index] + carry;
        carry = (sum >> SHIFT) > 0;
        a[index] = cast(sum & UMAX);
        sum = 0;
        index++;
        if (carry == 0)
            break;
    }
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    my_vector temp(size_ + rhs.size_);

    ull prod;
    for (size_t i = 0; i < size_; i++) {
        for (size_t j = 0; j < rhs.size_; j++) {
            prod = (ull) data_[i] * rhs.data_[j];
            ui lower = cast(prod & UMAX);
            ui higher = cast(prod >> SHIFT);

            add_big_int(temp, lower, i + j);
            add_big_int(temp, higher, i + j + 1);
        }
    }

    sign_ ^= rhs.sign_;
    size_ += rhs.size_;

    this->data_ = temp;
    normalize(*this);

    return *this;
}

void big_integer::normalize(big_integer &a) {
    while (a.size_ > 1 && a.data_[a.size_ - 1] == 0) {
        a.size_--;
        if (a.size_ + 1 < a.data_.size())
            a.data_.pop_back();
    }
    if (a.size_ == 1 && a.data_[0] == 0)
        a.sign_ = 0;
}

static int max_bit(ui n) {
    int temp = -1;
    while (n) {
        n >>= 1;
        ++temp;
    }
    return temp;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    if (rhs == 0)
        throw "DBZ";
    if (abs_compare(*this, rhs) == -1) {
        return *this = 0;
    }
    bool neg = sign_ ^rhs.sign_;

    if (rhs.size_ == 1) {
        ui carry = 0;
        ui divisor = rhs.data_[0];
        for (size_t i = size_ - 1; i + 1 != 0; --i) {
            ull cur = data_[i] + carry * SHIFTED;
            data_[i] = cast(cur / divisor);
            carry = cast(cur % divisor);
        }
        normalize(*this);
        sign_ = neg;
        return *this;
    }

    ui shift = SHIFT - 1 - max_bit(rhs.data_[rhs.size_ - 1]);
    big_integer b = rhs << shift;
    sign_ = false;
    b.sign_ = false;
    *this <<= shift;;

    size_t n = size_;
    size_t m = b.size_;

    big_integer q;
    q.resize(n - m + 1);
    q.size_ = n - m + 1;
    ull d1 = b.data_[m - 1];
    if (data_.capacity() == size_)
        resize(size_ + 1);
    for (size_t k = n - m + 1; k && *this >= b; --k) {
        size_t km = k + m;
        ull r2 = (data_[km - 1] * SHIFTED) + data_[km - 2];
        ui trial = (ui) std::min(static_cast<ull>(r2 / d1), SHIFTED - 1);

        *this -= (b * trial) << (SHIFT * (k - 1));
        q.data_[k - 1] = cast(trial);
        while (*this < 0) {
            *this += b << (SHIFT * (k - 1));
            q.data_[k - 1]--;
        }
    }
    q.sign_ = neg;
    normalize(q);
    swap(*this, q);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this = *this - (*this / rhs) * rhs;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    if (!sign_ && !rhs.sign_) {
        for (size_t i = 0; i < size_; i++)
            data_[i] &= rhs.data_[i];
    } else if (sign_ && rhs.sign_) {
        ++*this;
        big_integer temp = rhs + 1;
        for (size_t i = 0; i < size_; i++)
            data_[i] = ~(~data_[i] & ~temp.data_[i]);
        --*this;
    } else {
        if (sign_) {
            ++*this;
            for (size_t i = 0; i < size_; i++)
                data_[i] = (~data_[i] & rhs.data_[i]);
            sign_ = 0;
        } else {
            big_integer temp = rhs + 1;
            for (size_t i = 0; i < size_; i++)
                data_[i] = data_[i] & (~temp.data_[i]);
        }
    }

    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    if (!sign_ && !rhs.sign_) {
        for (size_t i = 0; i < size_; i++)
            data_[i] |= rhs.data_[i];
    } else if (sign_ && rhs.sign_) {
        ++*this;
        big_integer temp = rhs + 1;
        for (size_t i = 0; i < size_; i++)
            data_[i] = ~(~data_[i] | ~temp.data_[i]);
        --*this;
    } else {
        if (sign_) {
            ++*this;
            for (size_t i = 0; i < size_; i++)
                data_[i] = ~(~data_[i] | rhs.data_[i]);
            --*this;
        } else {
            big_integer temp = rhs + 1;
            for (size_t i = 0; i < size_; i++)
                data_[i] = ~(data_[i] | ~temp.data_[i]);
            *this = ~*this;
        }
    }
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    if (!sign_ && !rhs.sign_) {
        for (size_t i = 0; i < size_; i++)
            data_[i] ^= rhs.data_[i];
    } else if (sign_ && rhs.sign_) {
        ++*this;
        big_integer temp = rhs + 1;
        for (size_t i = 0; i < size_; i++)
            data_[i] = ~(data_[i] ^ ~temp.data_[i]);
        *this = ~(--*this);
    } else {
        if (sign_) {
            ++*this;
            for (size_t i = 0; i < size_; i++)
                data_[i] = ~(~data_[i] ^ rhs.data_[i]);
            sign_ = 0;
            *this = ~*this;
        } else {
            big_integer temp = rhs + 1;
            for (size_t i = 0; i < size_; i++)
                data_[i] = ~(data_[i] ^ ~temp.data_[i]);
            *this = ~*this;
        }
    }
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0) {
        return *this >>= -rhs;
    }

    ui shift = cast(rhs / SHIFT);
    rhs %= SHIFT;

    if (data_.size() <= size_ + shift)
        resize(size_ + shift + 1);

    ull num;
    ull last = (SHIFTED * data_[size_] + data_[size_ - 1]) << rhs;

    for (size_t i = size_ - 1; --i + 1;) {
        num = SHIFTED * data_[i + 1] + data_[i];
        num <<= rhs;

        data_[i + shift + 2] = cast((last >> SHIFT) & UMAX);
        last = num;
    }
    data_[shift + 1] = cast((last >> SHIFT) & UMAX);
    data_[shift] = cast(last & UMAX);

    for (size_t i = shift; --i + 1;)
        data_[i] = 0;

    size_ += shift + 1;
    normalize(*this);
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return *this <<= -rhs;
    }

    bool neg = false;
    if (sign_) {
        neg = true;
        *this = ~*this;
    }

    if (size_ == 1) {
        data_[0] >>= rhs;
    } else {
        ui shift = cast(rhs / SHIFT);
        rhs %= SHIFT;
        ull num;
        ull last = (SHIFTED * data_[1] + data_[0]) >> rhs;

        for (size_t i = 1 + shift; i < size_ - 1; i++) {
            num = SHIFTED * data_[i + 1] + data_[i];
            num >>= rhs;

            data_[i - shift - 1] = cast(last & UMAX);
            last = num;
        }
        data_[size_ - 1 - shift] = cast((last >> SHIFT) & UMAX);
        data_[size_ - 2 - shift] = cast(last & UMAX);

        size_ -= shift;
    }

    if (neg)
        *this = ~*this;

    normalize(*this);
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r(*this);
    if (r != 0)
        r.sign_ = !r.sign_;
    return r;
}

big_integer big_integer::operator~() const {
    big_integer r(*this);
    r = -(r + 1);
    return r;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}


int big_integer::abs_compare(big_integer const &a, big_integer const &b) {
    if (a.size_ < b.size_)
        return -1;
    if (a.size_ > b.size_)
        return 1;
    for (size_t i = a.size_ - 1; i + 1 != 0; --i) {
        if (a.data_[i] < b.data_[i])
            return -1;
        if (a.data_[i] > b.data_[i])
            return 1;
    }

    return 0;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.sign_ == b.sign_ && ((big_integer::abs_compare(a, b)) == 0);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign_ > b.sign_) {
        return true;
    } else if (a.sign_ < b.sign_) {
        return false;
    } else {
        int comp = big_integer::abs_compare(a, b);
        return (a.sign_ == 1 && comp == 1) || (a.sign_ == 0 && comp == -1);
    }
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    if (a == 0) {
        return "0";
    }

    std::string res;
    big_integer temp(a);
    bool neg = temp.sign_;
    temp.sign_ = false;

    while (temp > 0) {
        big_integer r;
        r = temp % 10;
        temp /= 10;
        res += '0' + r.data_[0];
    }

    if (neg) {
        res += '-';
    }
    reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}