#include "parser/parser.hpp"

#include <cassert>
#include <iostream>

using namespace redix;

void testSetCommand()
{
    Parser parser;

    Command command = parser.parse("SET name harsh");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Set);

    assert(command.arguments.size() == 2);
    assert(command.arguments[0] == "name");
    assert(command.arguments[1] == "harsh");
}

void testGetCommand()
{
    Parser parser;

    Command command = parser.parse("GET name");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Get);

    assert(command.arguments.size() == 1);
    assert(command.arguments[0] == "name");
}

void testDelCommand()
{
    Parser parser;

    Command command = parser.parse("DEL name");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Del);
}

void testExistsCommand()
{
    Parser parser;

    Command command = parser.parse("EXISTS name");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Exists);
}

void testPingCommand()
{
    Parser parser;

    Command command = parser.parse("PING");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Ping);

    assert(command.arguments.empty());
}

void testCaseInsensitiveCommands()
{
    Parser parser;

    assert(parser.parse("set name harsh").type == CommandType::Set);
    assert(parser.parse("gEt name").type == CommandType::Get);
    assert(parser.parse("dEl name").type == CommandType::Del);
    assert(parser.parse("eXiStS name").type == CommandType::Exists);
    assert(parser.parse("pInG").type == CommandType::Ping);
}

void testWhitespaceHandling()
{
    Parser parser;

    Command command =
        parser.parse("     SET      name      harsh     ");

    assert(command.status == ParseStatus::Success);
    assert(command.type == CommandType::Set);

    assert(command.arguments.size() == 2);
}

void testUnknownCommand()
{
    Parser parser;

    Command command = parser.parse("FOOBAR name");

    assert(command.status == ParseStatus::UnknownCommand);
    assert(command.type == CommandType::Unknown);
}

void testEmptyInput()
{
    Parser parser;

    Command command = parser.parse("");

    assert(command.status == ParseStatus::EmptyInput);
}

void testWhitespaceOnlyInput()
{
    Parser parser;

    Command command = parser.parse("      ");

    assert(command.status == ParseStatus::EmptyInput);
}

void testInvalidSetArityMissingValue()
{
    Parser parser;

    Command command = parser.parse("SET name");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Set);
}

void testInvalidSetArityExtraValue()
{
    Parser parser;

    Command command =
        parser.parse("SET name harsh extra");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Set);
}

void testInvalidDelArityMissingKey()
{
    Parser parser;

    Command command = parser.parse("DEL");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Del);
}

void testInvalidDelArityExtraArgument()
{
    Parser parser;

    Command command = parser.parse("DEL key extra");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Del);

    assert(command.arguments.size() == 2);
    assert(command.arguments[0] == "key");
    assert(command.arguments[1] == "extra");
}

void testInvalidExistsArityMissingKey()
{
    Parser parser;

    Command command = parser.parse("EXISTS");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Exists);
}

void testInvalidExistsArityExtraArgument()
{
    Parser parser;

    Command command = parser.parse("EXISTS key extra");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Exists);

    assert(command.arguments.size() == 2);
    assert(command.arguments[0] == "key");
    assert(command.arguments[1] == "extra");
}

void testInvalidGetExtraArgument()
{
    Parser parser;

    Command command = parser.parse("GET key extra");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Get);
}

void testInvalidGetArity()
{
    Parser parser;

    Command command = parser.parse("GET");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Get);
}

void testInvalidPingArity()
{
    Parser parser;

    Command command = parser.parse("PING extra");

    assert(command.status == ParseStatus::InvalidArity);
    assert(command.type == CommandType::Ping);
}

int main()
{
    testSetCommand();
    testGetCommand();
    testDelCommand();
    testExistsCommand();
    testPingCommand();

    testCaseInsensitiveCommands();
    testWhitespaceHandling();

    testUnknownCommand();
    testEmptyInput();
    testWhitespaceOnlyInput();

    testInvalidDelArityMissingKey();
    testInvalidGetExtraArgument();
    testInvalidDelArityExtraArgument();
    testInvalidExistsArityMissingKey();
    testInvalidExistsArityExtraArgument();
    testInvalidSetArityMissingValue();
    testInvalidSetArityExtraValue();
    testInvalidGetArity();
    testInvalidPingArity();

    std::cout << "All parser tests passed.\n";

    return 0;
}