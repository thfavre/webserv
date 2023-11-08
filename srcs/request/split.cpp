#include <map>
#include <sstream>
#include <vector>

// ! TODO put in a separate file
// ? TODO is it C++98 compatible?
/**
 * @brief Splits a string into a vector of tokens based on a delimiter.
 *
 * @tparam T The type of the delimiter.
 * @param str The string to split.
 * @param delimiter The delimiter to split the string on.
 * @return std::vector<T> A vector of tokens.
 */
template <typename T>
std::vector<T> split(const std::string &str, const T &delimiter)
{
	std::vector<T> tokens;
	std::size_t start = 0;
	std::size_t pos = 0;

	while ((pos = str.find(delimiter, start)) != std::string::npos)
	{
		tokens.push_back(str.substr(start, pos - start));
		start = pos + delimiter.length();
	}

	if (start < str.length())
	{
		tokens.push_back(str.substr(start));
	}

	return tokens;
}

/**
 * @brief Splits a string into a vector of tokens using a delimiter.
 *
 * @tparam T The type of the delimiter.
 * @param str The string to split.
 * @param delimiter The delimiter to use for splitting.
 * @param max The maximum number of splits to perform. If max is reached, the function stops splitting.
 * @return std::vector<T> A vector of tokens.
 */
template <typename T>
std::vector<T> split(const std::string &str, const T &delimiter, int max)
{
	std::vector<T> tokens;
	std::size_t start = 0;
	std::size_t pos = 0;
	int splits = 0;

	while ((pos = str.find(delimiter, start)) != std::string::npos)
	{
		if (splits < max - 1)
		{
			tokens.push_back(str.substr(start, pos - start));
			splits++;
			start = pos + delimiter.length();
		}
		else
		{
			break;
		}
	}

	if (start < str.length())
	{
		tokens.push_back(str.substr(start));
	}

	return tokens;
}
