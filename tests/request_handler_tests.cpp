#include "core/request_handler.hpp"
#include "storage/key_value_store.hpp"

#include <cassert>
#include <iostream>

using namespace redix;

void testPing()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(handler.handleLine("PING") == "PONG");
}

void testSetCommand()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("SET name harsh")
        == "OK");
}

void testGetExistingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    assert(
        handler.handleLine("GET name")
        == "harsh");
}

void testGetMissingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("GET unknown")
        == "NIL");
}

void testDeleteExistingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    assert(
        handler.handleLine("DEL name")
        == "1");
}

void testDeleteMissingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("DEL unknown")
        == "0");
}

void testDeleteThenGet()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    static_cast<void>(
        handler.handleLine("DEL name"));

    assert(
        handler.handleLine("GET name")
        == "NIL");
}

void testExistsExistingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    assert(
        handler.handleLine("EXISTS name")
        == "1");
}

void testExistsMissingKey()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("EXISTS unknown")
        == "0");
}

void testOverwriteValue()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    static_cast<void>(
        handler.handleLine("SET name john"));

    assert(
        handler.handleLine("GET name")
        == "john");
}

void testCaseInsensitiveCommands()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("pInG")
        == "PONG");

    assert(
        handler.handleLine("sEt name harsh")
        == "OK");

    assert(
        handler.handleLine("gEt name")
        == "harsh");

    assert(
        handler.handleLine("eXiStS name")
        == "1");
}

void testUnknownCommand()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("FOOBAR name")
        == "ERR unknown command");
}

void testInvalidSetArity()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("SET name")
        == "ERR invalid arity");
}

void testInvalidGetArity()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("GET")
        == "ERR invalid arity");
}

void testInvalidExistsArity()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("EXISTS")
        == "ERR invalid arity");
}

void testInvalidPingArity()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("PING extra")
        == "ERR invalid arity");
}

void testEmptyInput()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("")
        == "ERR empty input");
}

void testUnknownCommandDoesNotMutateStore()
{
    KeyValueStore store;
    RequestHandler handler(store);

    static_cast<void>(
        handler.handleLine("SET name harsh"));

    assert(
        handler.handleLine("FOOBAR test")
        == "ERR unknown command");

    assert(
        handler.handleLine("GET name")
        == "harsh");
}

void testFullRequestPipeline()
{
    KeyValueStore store;
    RequestHandler handler(store);

    assert(
        handler.handleLine("SET language cpp")
        == "OK");

    assert(
        handler.handleLine("GET language")
        == "cpp");

    assert(
        handler.handleLine("EXISTS language")
        == "1");

    assert(
        handler.handleLine("DEL language")
        == "1");

    assert(
        handler.handleLine("GET language")
        == "NIL");
}

int main()
{
    testPing();

    testSetCommand();

    testGetExistingKey();
    testGetMissingKey();

    testDeleteExistingKey();
    testDeleteMissingKey();
    testDeleteThenGet();

    testExistsExistingKey();
    testExistsMissingKey();

    testOverwriteValue();

    testCaseInsensitiveCommands();

    testUnknownCommand();

    testInvalidSetArity();
    testInvalidGetArity();
    testInvalidExistsArity();
    testInvalidPingArity();

    testEmptyInput();

    testUnknownCommandDoesNotMutateStore();

    testFullRequestPipeline();

    std::cout
        << "All request handler tests passed.\n";

    return 0;
}