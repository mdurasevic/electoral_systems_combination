#pragma once
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class StringUtils
{
public:
	static vector<string> Split(string s, string delimiter)
	{
		auto start = 0U;
		auto end = s.find(delimiter);

		vector<string> split;

		while (end != string::npos)
		{
			if(start != end)
			{
				split.push_back(s.substr(start, end - start));
			}
			start = end + delimiter.length();
			end = s.find(delimiter, start);
		}
		split.push_back(s.substr(start, s.length()));
		return split;
	}


	static void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	// trim from end (in place)
	static void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	// trim from both ends (in place)
	static void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static std::string rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}

	static std::string zero_prefix_string(int number, int length)
	{
		std::stringstream ss;
		ss << std::setw(length) << std::setfill('0') << number;
		std::string stackString = ss.str();
		return stackString;
	}

	static bool ends_with(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}
	
};

