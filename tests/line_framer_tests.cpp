#include "networking/line_framer.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace redix;

void testCompleteRequest()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PING\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "PING");
}

void testFragmentedRequest()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PI", request);

    assert(status == LineFramer::Status::Incomplete);

    status =
        framer.append("NG\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "PING");
}

void testTwoCommandsInOneAppend()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PING\nGET name\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "PING");

    status =
        framer.append("", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "GET name");
}

void testCRLF()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PING\r\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "PING");
}

void testExactlyMaximumPayload()
{
    LineFramer framer;
    std::string request;

    std::string input(4096, 'a');
    input.push_back('\n');

    LineFramer::Status status =
        framer.append(input, request);

    assert(status == LineFramer::Status::Complete);
    assert(request.size() == 4096);
    assert(request == std::string(4096, 'a'));
}

void testExactlyMaximumPayloadWithCRLF()
{
    LineFramer framer;
    std::string request;

    std::string input(4096, 'a');
    input += "\r\n";

    LineFramer::Status status =
        framer.append(input, request);

    assert(status == LineFramer::Status::Complete);
    assert(request.size() == 4096);
    assert(request == std::string(4096, 'a'));
}

void testOversizedPayload()
{
    LineFramer framer;
    std::string request;

    std::string input(4097, 'a');
    input.push_back('\n');

    LineFramer::Status status =
        framer.append(input, request);

    assert(status == LineFramer::Status::TooLarge);
}

void testEmptyLine()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request.empty());
}

void testIncompleteInput()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PING", request);

    assert(status == LineFramer::Status::Incomplete);
    assert(request.empty());
}

void testBufferedRequestAfterFirstCommand()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("SET name harsh\nGET name\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "SET name harsh");

    status =
        framer.append("", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "GET name");
}

void testFragmentedCRLF()
{
    LineFramer framer;
    std::string request;

    LineFramer::Status status =
        framer.append("PING\r", request);

    assert(status == LineFramer::Status::Incomplete);

    status =
        framer.append("\n", request);

    assert(status == LineFramer::Status::Complete);
    assert(request == "PING");
}

int main()
{
    testCompleteRequest();

    testFragmentedRequest();

    testTwoCommandsInOneAppend();

    testCRLF();

    testExactlyMaximumPayload();

    testExactlyMaximumPayloadWithCRLF();

    testOversizedPayload();

    testEmptyLine();

    testIncompleteInput();

    testBufferedRequestAfterFirstCommand();

    testFragmentedCRLF();

    std::cout
        << "All line framer tests passed.\n";

    return 0;
}
