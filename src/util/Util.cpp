#include <cpr/cpr.h>
#include <date/date.h>
#include <taigabot/util/MathUtil.hpp>
#include <taigabot/util/StringUtil.hpp>
#include <taigabot/util/Util.hpp>

rapidjson::Document TaigaBot::Util::get_post(const char* url) {
	auto post_json_string =
		cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "taigabot-cpp"}})
			.text;

	rapidjson::Document post_json;
	post_json.Parse(post_json_string.c_str());

	while (post_json.IsNull() ||
		   post_json[0]["data"]["children"][0]["data"]["is_self"].GetBool()) {
		post_json = TaigaBot::Util::get_post(url);
	}

	return post_json;
}

float TaigaBot::Util::year_progress() {
	auto now = std::chrono::system_clock::now();

	auto current_date = date::year_month_day{date::floor<date::days>(now)};
	auto days_in_year = current_date.year().is_leap() ? 366 : 365;

	auto next_date = date::year_month_day{
		date::January / 1 / current_date.year() + date::years{1}};
	auto days_until_next_year =
		(date::sys_days(next_date) - date::sys_days(current_date)).count();

	auto nonrounded_percent =
		100.f - (static_cast<float>(days_until_next_year) / days_in_year * 100);

	return TaigaBot::Util::Math::round_to_dec_places(nonrounded_percent, 2);
}

float TaigaBot::Util::conversion_rate(const std::string& from,
									  const std::string& to,
									  const std::string& api_key) {
	if (api_key.empty()) {
		throw std::runtime_error("Currency API key not set.");
	}
	auto currency_to_currency = fmt::format("{}_{}", from, to);
	TaigaBot::Util::String::to_upper(currency_to_currency);

	auto url = fmt::format(
		"https://free.currconv.com/api/v7/"
		"convert?q={}&compact=ultra&apiKey={}",
		currency_to_currency, api_key);

	auto request =
		cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "taigabot-cpp"}});
	switch (request.status_code) {
		case 200: {
			break;
		}
		case 429: {
			throw std::runtime_error("Ratelimited.");
			break;
		}
		case 400: {
			throw std::runtime_error("Invalid API key.");
			break;
		}
		case 503: {
			throw std::runtime_error(
				"Currency Conversion API is down. Please try again later.");
			break;
		}
		default: {
			throw std::runtime_error("Unknown error.");
			break;
		}
	}

	rapidjson::Document json;
	json.Parse(request.text.c_str());

	if (!json.HasMember(currency_to_currency.c_str())) {
		throw std::runtime_error("Invalid arguments.");
	}
	rapidjson::Value& conversion_rate = json[currency_to_currency.c_str()];
	return conversion_rate.GetFloat();
}