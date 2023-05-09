#include "util/lines.hpp"

#include <istream>
#include <utility>

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

LineIter::reference LineIter::operator*() noexcept {
    return this->line_buf;
}

LineIter::pointer LineIter::operator->() noexcept {
    return &(this->line_buf);
}

LineIter& LineIter::operator++() {
    if (! std::getline(*(this->input), this->line_buf)) {
        this->input = nullptr;
    }
    return *this;
}

bool LineIter::operator==(Sentinel) const noexcept {
    return this->input == nullptr;
}

Lines::Lines(std::istream& input) noexcept : input(input) {}

Lines Lines::of(std::istream& input) noexcept {
    return Lines(input);
}

LineIter Lines::begin() noexcept {
    return LineIter(this->input);
}

LineIter::Sentinel Lines::end() const noexcept {
    return LineIter::Sentinel();
}
