#include "storage/key_value_store.hpp"

#include <cassert>
#include <iostream>

using namespace redix;

void testSetAndGet()
{
    KeyValueStore store;

    store.set("name", "harsh");

    auto value = store.get("name");

    assert(value.has_value());
    assert(value.value() == "harsh");
}

void testGetMissingKey()
{
    KeyValueStore store;

    auto value = store.get("unknown");

    assert(!value.has_value());
}

void testOverwriteValue()
{
    KeyValueStore store;

    store.set("name", "harsh");
    store.set("name", "john");

    auto value = store.get("name");

    assert(value.has_value());
    assert(value.value() == "john");
}

void testExists()
{
    KeyValueStore store;

    store.set("name", "harsh");

    assert(store.exists("name"));
    assert(!store.exists("unknown"));
}

void testDeleteExistingKey()
{
    KeyValueStore store;

    store.set("name", "harsh");

    bool removed = store.del("name");

    assert(removed);
    assert(!store.exists("name"));
}

void testDeleteRemovesValue()
{
    KeyValueStore store;

    store.set("name", "harsh");

    store.del("name");

    auto value = store.get("name");

    assert(!value.has_value());
}

void testExistsAfterOverwrite()
{
    KeyValueStore store;

    store.set("name", "harsh");
    store.set("name", "john");

    assert(store.exists("name"));

    auto value = store.get("name");

    assert(value.has_value());
    assert(value.value() == "john");
}

void testDeleteMissingKey()
{
    KeyValueStore store;

    bool removed = store.del("unknown");

    assert(!removed);
}

int main()
{
    testSetAndGet();
    testGetMissingKey();
    testOverwriteValue();
    testExists();
    testDeleteExistingKey();
    testDeleteMissingKey();
    testDeleteRemovesValue();
    testExistsAfterOverwrite();

    std::cout<< "All storage tests passed.\n";

    return 0;
}