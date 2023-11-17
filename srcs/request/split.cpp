#include <map>
#include <sstream>
#include <vector>

/**
 * @brief Splits a string into a vector of tokens based on a delimiter.
 *
 * @param str The string to split.
 * @param delimiter The delimiter to split the string on.
 * @return std::vector<std::string> A vector of tokens.
 */
std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t pos = 0;

	while ((pos = str.find(delimiter, start)) != std::string::npos)
	{
		tokens.push_back(str.substr(start, pos - start));
		start = pos + delimiter.length();
	}
	if (start < str.length())
		tokens.push_back(str.substr(start));
	return tokens;
}

/**
 * @brief Splits a string into a vector of tokens using a delimiter with a maximum number of splits.
 *
 * @param str The string to split.
 * @param delimiter The delimiter to use for splitting.
 * @param max The maximum number of splits to perform. If max is reached, the function stops splitting.
 * @return std::vector<std::string> A vector of tokens.
 */
std::vector<std::string> split(const std::string &str, const std::string &delimiter, int max)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t pos = 0;
	int splits = 0;

	while ((pos = str.find(delimiter, start)) != std::string::npos)
	{
		if (splits < max - 1 || max == -1)
		{
			tokens.push_back(str.substr(start, pos - start));
			splits++;
			start = pos + delimiter.length();
		}
		else
			break;
	}
	if (start < str.length())
		tokens.push_back(str.substr(start));
	return tokens;
}
