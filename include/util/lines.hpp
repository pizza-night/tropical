#pragma once

#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <string>

class LineIter {
  private:
    std::istream* input;
    std::string line_buf;

    friend class Lines;
    explicit LineIter(std::istream& input) noexcept;

  public:
    struct Sentinel {};

    using difference_type = std::ptrdiff_t;
    using value_type = std::string;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;

    LineIter(LineIter const&) = delete;
    LineIter(LineIter&&) noexcept;

    LineIter& operator=(LineIter const&) = delete;
    LineIter& operator=(LineIter&&) noexcept;

    reference operator*() noexcept;

    pointer operator->() noexcept;

    LineIter& operator++();

    bool operator==(Sentinel) const noexcept;
};

class Lines {
  private:
    std::istream& input;

    explicit Lines(std::istream& input) noexcept;

  public:
    static Lines of(std::istream& input) noexcept;

    LineIter begin() noexcept;

    LineIter::Sentinel end() const noexcept;
};
