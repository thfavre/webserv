#ifndef SPLIT_HPP
# define SPLIT_HPP
# include <string>
# include <vector>

std::vector<std::string> split(const std::string &str, const std::string &delimiter);

std::vector<std::string> split(const std::string &str, const std::string &delimiter, int max);

#endif
