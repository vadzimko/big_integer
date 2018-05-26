#include "my_vector.h"
#include <cassert>
#include <algorithm>
#include <cstring>
#include <iostream>

my_vector::my_vector()
        : is_small_(true), size_(0) {}

my_vector::my_vector(size_t size)
        : is_small_(size <= SMALL_SIZE), size_(size) {
    if (!is_small_) {
        to_big();
    }
}

my_vector::my_vector(size_t size, uint32_t element)
        : is_small_(size <= SMALL_SIZE), size_(size) {

    if (is_small_) {
        std::fill(data_.small, data_.small + size, element);
    } else {
        data_.big = std::make_shared<std::vector<uint32_t >>(size, element);
    }
}

my_vector::my_vector(my_vector const& other) {
    *this = other;
}

my_vector &my_vector::operator=(my_vector const &other) {
    size_ = other.size_;
    is_small_ = other.is_small_;

    if (is_small_) {
        std::copy(other.data_.small, other.data_.small + other.size_, data_.small);
        data_.big = nullptr;
    } else {
        data_.big = std::make_shared<std::vector<uint32_t >>(*other.data_.big);
    }

    return *this;
}

size_t my_vector::size() const {
    return size_;
}

size_t my_vector::capacity() const {
    if (is_small_) {
        return SMALL_SIZE;
    } else {
        return data_.big->capacity();
    }
}

void my_vector::push_back(uint32_t const &element) {
    if (is_small_ && size_ < SMALL_SIZE) {
        data_.small[size_] = element;
        ++size_;
        return;
    } else if (is_small_) {
        is_small_ = false;
        to_big();
    }

    data_.big->push_back(element);
    size_++;
}

void my_vector::pop_back() {
    assert(size_ > 0);

    if (!is_small_) {
        data_.big->pop_back();
    }
    size_--;
}

void my_vector::assign(size_t size, uint32_t element) {
    my_vector that(size, element);
    std::swap(*this, that);
}

void my_vector::resize(size_t size, uint32_t element) {
    if (is_small_ && size <= SMALL_SIZE) {
        std::fill(data_.small + size_, data_.small + size, element);
    } else {
        if (is_small_) {
            is_small_ = false;
            size_ = size;
            to_big();
        }
        data_.big->resize(size, element);
    }
    size_ = size;
}


uint32_t &my_vector::operator[](size_t index) {
    if (is_small_) {
        return data_.small[index];
    } else {
        return (*data_.big)[index];
    }
}

uint32_t const &my_vector::operator[](size_t index) const {
    if (is_small_) {
        return data_.small[index];
    } else {
        return (*data_.big)[index];
    }
}

void my_vector::to_big() {
    if (is_small_) {
        return;
    }

    data_.big = std::make_shared<std::vector<uint32_t>>(size_);

    for (size_t i = 0; i < SMALL_SIZE; i++) {
        (*data_.big)[i] = data_.small[i];
    }
}
