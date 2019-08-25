#ifndef STRINGUTIL_HPP
#define STRINGUTIL_HPP

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <deque>

namespace TaigaBot::Util::String {
template <typename T>
T string_to_number(const std::string& number_as_string) {
	T valor;

	std::stringstream stream(number_as_string);
	stream >> valor;
	if (stream.fail()) {
		throw std::runtime_error(
			fmt::format("Cannot convert {} to number", number_as_string));
		;
	}
	return valor;
}
std::deque<std::string> split_command(const std::string& source);
std::string& to_upper(std::string& string);
}  // namespace TaigaBot::Util::String

#endif
