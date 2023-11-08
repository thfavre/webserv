#ifndef SPLIT_HPP
# define SPLIT_HPP
# include <string>
# include <vector>
#include <sstream>

template <typename T>
std::vector<T> split(const std::string &str, const T &delimiter);

template <typename T>
std::vector<T> split(const std::string &str, const T &delimiter, int max);

#endif
