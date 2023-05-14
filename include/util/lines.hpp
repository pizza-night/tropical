#pragma once

#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <string>

class Lines {
  private:
    std::istream& input;

    explicit Lines(std::istream& input) noexcept;

  public:
    struct Sentinel {};

    [[nodiscard]]
    static Lines of(std::istream& input) noexcept;

    class LineIter begin() noexcept;

    [[nodiscard]]
    Sentinel end() const noexcept;
};

class LineIter {
  private:
    std::istream* input;
    std::string line_buf;

    friend class Lines;
    explicit LineIter(std::istream& input) noexcept;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::string;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;

    LineIter(LineIter const&) = delete;
    LineIter(LineIter&&) noexcept;

    LineIter& operator=(LineIter const&) = delete;
    LineIter& operator=(LineIter&&) noexcept;

    [[nodiscard]]
    reference
    operator*() noexcept;

    [[nodiscard]]
    pointer
    operator->() noexcept;

    LineIter& operator++();

    [[nodiscard]] bool operator==(Lines::Sentinel) const noexcept;
};
