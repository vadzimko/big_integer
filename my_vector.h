#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>

struct my_vector {
    my_vector();
    explicit my_vector(size_t size);
    my_vector(size_t size, uint32_t element);
    my_vector(my_vector const& other);
    my_vector& operator=(my_vector const& other);

    ~my_vector() = default;

    size_t size() const;
    size_t capacity() const;
    void push_back(uint32_t const& element);
    void pop_back();
    void assign(size_t size, uint32_t element);
    void resize(size_t size, uint32_t element);

    uint32_t& operator[](size_t index);
    uint32_t const& operator[](size_t index) const;

private:
    static const size_t SMALL_SIZE = 3;
    bool is_small_;
    size_t size_;
    struct data {
        std::shared_ptr<std::vector <uint32_t>> big;
        uint32_t small[SMALL_SIZE] = {0};
        data() : big(nullptr) {}
        ~data() {big = nullptr;}
    };
    data data_;
    void to_big();
};

#endif //BIGINT_MY_VECTOR_H
