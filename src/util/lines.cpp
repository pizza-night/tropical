#include "util/lines.hpp"

#include <istream>
#include <utility>

Lines::Lines(std::istream& input) noexcept : input(input) {}

[[nodiscard]]
Lines Lines::of(std::istream& input) noexcept {
    return Lines(input);
}

LineIter Lines::begin() noexcept {
    return LineIter(this->input);
}

[[nodiscard]]
Lines::Sentinel Lines::end() const noexcept {
    return Sentinel();
}

LineIter::LineIter(std::istream& input) noexcept : input(&input), line_buf() {
    ++(*this);
}

LineIter::LineIter(LineIter&& other) noexcept
  : input(other.input), line_buf(std::move(other.line_buf)) {
    other.input = nullptr;
}

LineIter& LineIter::operator=(LineIter&& other) noexcept {
    this->input = other.input;
    this->line_buf = std::move(other.line_buf);
    other.input = nullptr;
    return *this;
}

[[nodiscard]]
LineIter::reference LineIter::operator*() noexcept {
    return this->line_buf;
}

[[nodiscard]]
LineIter::pointer LineIter::operator->() noexcept {
    return &(this->line_buf);
}

LineIter& LineIter::operator++() {
    if (! std::getline(*(this->input), this->line_buf)) {
        this->input = nullptr;
    }
    return *this;
}

[[nodiscard]]
bool LineIter::operator==(Lines::Sentinel) const noexcept {
    return this->input == nullptr;
}
