#include <aegis.hpp>
#include <mongocxx/instance.hpp>
#include <taiga/Client.hpp>
#include <taiga/Config.hpp>
#include <taiga/command/Command.hpp>

int main() {
	aegis::core bot(spdlog::level::trace);

	Taiga::Client client{};
	client.set_bot(bot);
	client.load_config();

	bot.set_on_message_create(std::bind(&Taiga::Client::message_create, &client,
										std::placeholders::_1));
	Taiga::Command::add_commands(*bot.log);

	mongocxx::instance instance{};

	bot.run();
	bot.yield();
}