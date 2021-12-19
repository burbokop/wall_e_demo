#ifndef BUFFERED_REVERSE_RANGE_H
#define BUFFERED_REVERSE_RANGE_H

#include <list>

template<typename OriginalRange>
class buffered_reverse_range {
    typedef decltype (((OriginalRange*)nullptr)->begin()) orig_iterator;
    typedef typename std::remove_const<typename std::remove_reference<decltype (((orig_iterator*)nullptr)->operator*())>::type>::type item_type;

    std::list<item_type> m_buffer;
public:
    buffered_reverse_range(const OriginalRange& original_range) {
        for(const auto& c : original_range) {
            m_buffer.push_back(c);
        }
    }

    auto begin() const { return m_buffer.rbegin(); };
    auto begin() { return m_buffer.rbegin(); };
    auto end() const { return m_buffer.rend(); };
    auto end() { return m_buffer.rend(); };
};

#endif // BUFFERED_REVERSE_RANGE_H
