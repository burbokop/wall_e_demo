#include "either.h"


bool Either::operator==(const Either &other) const {
    return m_defined == other.m_defined && m_left == other.m_left && m_right == other.m_right;
}
