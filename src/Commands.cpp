#include <fmt/format.h>
#include <rapidjson/document.h>
#include <taigabot/Command.hpp>
#include <taigabot/Commands.hpp>
#include <taigabot/util/MathUtil.hpp>
#include <taigabot/util/StringUtil.hpp>
#include <taigabot/util/Util.hpp>

#ifdef COMMAND
#undef COMMAND
#define COMMAND(name)                                              \
	void TaigaBot::Commands::name(TaigaBot::Client& client,        \
								  SleepyDiscord::Message& message, \
								  std::deque<std::string>& params)
#endif

COMMAND(help) {
	constexpr char start[] = "```\\n";
	constexpr char the_end[] = "```";
	// estimate length
	auto length = strlen(start) + strlen(the_end);
	for (TaigaBot::Command::MappedCommand& command : TaigaBot::Command::all) {
		length += command.first.size();
		length += 2;  // ' ' and '\n'
		for (auto param_name : command.second.params) {
			length += 2;  // '<' and '> '
			length += param_name.size();
		}
	}

	std::string output;
	output.reserve(length);
	output += start;
	for (TaigaBot::Command::MappedCommand& command : TaigaBot::Command::all) {
		output += command.first;
		output += ' ';
		for (auto param_name : command.second.params) {
			output += '<';
			output += param_name;
			output += "> ";
		}
		output += "\\n";
	}
	output += the_end;
	client.sendMessage(message.channelID, output);
}

COMMAND(taiga) {
	auto post =
		TaigaBot::Util::get_post("https://reddit.com/r/taiga/random.json");

	client.sendMessage(
		message.channelID,
		post[0]["data"]["children"][0]["data"]["url"].GetString());
}

COMMAND(toradora) {
	auto post =
		TaigaBot::Util::get_post("https://reddit.com/r/toradora/random.json");

	client.sendMessage(
		message.channelID,
		post[0]["data"]["children"][0]["data"]["url"].GetString());
}

COMMAND(progress) {
	auto percent = TaigaBot::Util::year_progress();
	std::string msg;

	for (auto progress = 0; progress < 20; progress++) {
		msg += progress < percent / 5 ? "▓" : "░";
	}

	client.sendMessage(message.channelID,
					   fmt::format("{} {:.2f}%", msg, percent));
}

COMMAND(money) {
	float conversion_rate;
	try {
		conversion_rate = TaigaBot::Util::conversion_rate(
			params.front(), params.at(1),
			client.get_config().currency_api_token);
	} catch (const std::runtime_error& error) {
		client.sendMessage(message.channelID, error.what());
		return;
	}

	float amount;
	try {
		amount = TaigaBot::Util::String::string_to_number<float>(params.back());
	} catch (const std::runtime_error& error) {
		client.sendMessage(message.channelID, "Invalid arguments.");
		return;
	}

	auto worth = amount * conversion_rate;

	client.sendMessage(
		message.channelID,
		fmt::format("{:.2f} {} is worth {:.2f} {}",
					TaigaBot::Util::Math::round_to_dec_places(amount, 2),
					TaigaBot::Util::String::to_upper(params.front()),
					TaigaBot::Util::Math::round_to_dec_places(worth, 2),
					TaigaBot::Util::String::to_upper(params.at(1))));
}

void TaigaBot::Commands::add_commands() {
	TaigaBot::Command::add_command({"help", {}, TaigaBot::Commands::help});
	TaigaBot::Command::add_command({"taiga", {}, TaigaBot::Commands::taiga});
	TaigaBot::Command::add_command(
		{"toradora", {}, TaigaBot::Commands::toradora});
	TaigaBot::Command::add_command(
		{"progress", {}, TaigaBot::Commands::progress});
	TaigaBot::Command::add_command({"money",
									{"currency", "other currency", "amount"},
									TaigaBot::Commands::money});
}