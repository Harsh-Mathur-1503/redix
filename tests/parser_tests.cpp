#include "../src/parser/parser.hpp"

#include <cassert>
#include <iostream>

void testSetCommand()
{
    Parser parser;

    Command command = parser.parse("SET name Harsh");

    assert(command.type == CommandType::Set);
    assert(command.key == "name");
    assert(command.value == "Harsh");

    assert(command.arguments.size() == 2);
    assert(command.arguments[0] == "name");
    assert(command.arguments[1] == "Harsh");
}

void testGetCommand()
{
    Parser parser;

    Command command = parser.parse("GET name");

    assert(command.type == CommandType::Get);
    assert(command.key == "name");
    assert(command.value.empty());

    assert(command.arguments.size() == 1);
    assert(command.arguments[0] == "name");
}

void testDelCommand()
{
    Parser parser;

    Command command = parser.parse("DEL name");

    assert(command.type == CommandType::Del);
    assert(command.key == "name");

    assert(command.arguments.size() == 1);
}

void testExistsCommand()
{
    Parser parser;

    Command command = parser.parse("EXISTS name");

    assert(command.type == CommandType::Exists);
    assert(command.key == "name");

    assert(command.arguments.size() == 1);
}

void testPingCommand()
{
    Parser parser;

    Command command = parser.parse("PING");

    assert(command.type == CommandType::Ping);

    assert(command.key.empty());
    assert(command.value.empty());
    assert(command.arguments.empty());
}

void testCaseInsensitiveCommands()
{
    Parser parser;

    assert(parser.parse("set name harsh").type == CommandType::Set);
    assert(parser.parse("gEt name").type == CommandType::Get);
    assert(parser.parse("DeL name").type == CommandType::Del);
    assert(parser.parse("eXiStS name").type == CommandType::Exists);
    assert(parser.parse("PiNg").type == CommandType::Ping);
}

void testExtraWhitespace()
{
    Parser parser;

    Command command = parser.parse("   SET    name     Harsh   ");

    assert(command.type == CommandType::Set);
    assert(command.key == "name");
    assert(command.value == "Harsh");

    assert(command.arguments.size() == 2);
}

void testUnknownCommand()
{
    Parser parser;

    Command command = parser.parse("UNKNOWN key");

    assert(command.type == CommandType::Unknown);

    assert(command.arguments.size() == 1);
    assert(command.arguments[0] == "key");
}

void testEmptyInput()
{
    Parser parser;

    Command command = parser.parse("");

    assert(command.type == CommandType::Unknown);
    assert(command.arguments.empty());
}

void testWhitespaceOnlyInput()
{
    Parser parser;

    Command command = parser.parse("      ");

    assert(command.type == CommandType::Unknown);
    assert(command.arguments.empty());
}

void testMissingSetValue()
{
    Parser parser;

    Command command = parser.parse("SET name");

    assert(command.type == CommandType::Set);

    assert(command.key == "name");
    assert(command.value.empty());

    assert(command.arguments.size() == 1);
}

void testTooManyArguments()
{
    Parser parser;

    Command command = parser.parse("SET name Harsh extra");

    assert(command.type == CommandType::Set);

    assert(command.arguments.size() == 3);
    assert(command.arguments[0] == "name");
    assert(command.arguments[1] == "Harsh");
    assert(command.arguments[2] == "extra");
}

int main()
{
    testSetCommand();
    testGetCommand();
    testDelCommand();
    testExistsCommand();
    testPingCommand();

    testCaseInsensitiveCommands();
    testExtraWhitespace();

    testUnknownCommand();
    testEmptyInput();
    testWhitespaceOnlyInput();

    testMissingSetValue();
    testTooManyArguments();

    std::cout << "All parser tests passed.\n";

    return 0;
}