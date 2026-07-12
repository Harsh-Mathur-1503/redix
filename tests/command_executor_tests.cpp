#include "core/command_executor.hpp"
#include "parser/parser.hpp"
#include "storage/key_value_store.hpp"

#include <cassert>
#include <iostream>

using namespace redix;

void testPing()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    auto command = parser.parse("PING");

    assert(executor.execute(command) == "PONG");
}

void testSet()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    auto command = parser.parse("SET name harsh");

    assert(executor.execute(command) == "OK");
}

void testGetExistingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    static_cast<void>(
    executor.execute(
        parser.parse("SET name harsh")));

    assert(
        executor.execute(
            parser.parse("GET name")) == "harsh");
}

void testGetMissingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    assert(
        executor.execute(
            parser.parse("GET unknown")) == "NIL");
}

void testDeleteExistingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    executor.execute(
        parser.parse("SET name harsh"));

    assert(
        executor.execute(
            parser.parse("DEL name")) == "1");
}

void testDeleteMissingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    assert(
        executor.execute(
            parser.parse("DEL unknown")) == "0");
}

void testExistsExistingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    executor.execute(
        parser.parse("SET name harsh"));

    assert(
        executor.execute(
            parser.parse("EXISTS name")) == "1");
}

void testExistsMissingKey()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    assert(
        executor.execute(
            parser.parse("EXISTS unknown")) == "0");
}

void testUnknownCommand()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    auto command = parser.parse("FOOBAR name");

    assert(
        executor.execute(command) ==
        "ERR unknown command");
}

void testInvalidArity()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    auto command = parser.parse("SET name");

    assert(
        executor.execute(command) ==
        "ERR invalid arity");
}

void testEmptyInput()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    auto command = parser.parse("");

    assert(
        executor.execute(command) ==
        "ERR empty input");
}

void testOverwriteValue()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    executor.execute(
        parser.parse("SET name harsh"));

    executor.execute(
        parser.parse("SET name john"));

    assert(
        executor.execute(
            parser.parse("GET name")) == "john");
}

void testFullRequestPipeline()
{
    KeyValueStore store;
    CommandExecutor executor(store);
    Parser parser;

    assert(
        executor.execute(
            parser.parse("SET language cpp")) == "OK");

    assert(
        executor.execute(
            parser.parse("GET language")) == "cpp");

    assert(
        executor.execute(
            parser.parse("EXISTS language")) == "1");

    assert(
        executor.execute(
            parser.parse("DEL language")) == "1");

    assert(
        executor.execute(
            parser.parse("GET language")) == "NIL");
}

int main()
{
    testPing();

    testSet();

    testGetExistingKey();
    testGetMissingKey();

    testDeleteExistingKey();
    testDeleteMissingKey();

    testExistsExistingKey();
    testExistsMissingKey();

    testUnknownCommand();
    testInvalidArity();
    testEmptyInput();

    testOverwriteValue();

    testFullRequestPipeline();

    std::cout
        << "All command executor tests passed.\n";

    return 0;
}