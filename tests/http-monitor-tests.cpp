#include "../src/http-monitor.h"
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

using namespace std;
using namespace HttpMonitorHelper;

// Test helpers
bool assertTrue(bool condition, string funcName)
{
    cout << funcName << (condition ? " passed!" : " failed") << endl;

    return condition;
}
#define AssertTrueRet(c, f) if(!assertTrue(c,f)) { return; }

// Alert Buffer tests
void alertbuffer_basicTest()
{
    int SIZE_OF_BUFFER = 1;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);

    AlertUnit a = { 0 };
    bool alert = cb.insert(a);
    AssertTrueRet(!alert, __func__);
}

void alertbuffer_largeCorrectness()
{
    int SIZE_OF_BUFFER = 50;
    int ALERTMIN = 10;

    bool passed;
    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);

    AlertUnit a = { 0 };
    for(int i = 0; i < SIZE_OF_BUFFER; i++)
    {
        cb.insert(a);
    }

    for(int i = 0; i < SIZE_OF_BUFFER; i++)
    {
        passed = (cb.insert(a) == false);
    }

    AssertTrueRet(passed, __func__);
}

void alertbuffer_alertBasic()
{
    int SIZE_OF_BUFFER = 1;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);
    AlertUnit a = { 10 };

    bool alert = cb.insert(a);
    AssertTrueRet(alert, __func__);
}

void alertbuffer_alertLarge()
{
    int SIZE_OF_BUFFER = 10;
    int ALERTMIN = 10;

    AlertBuffer cb(SIZE_OF_BUFFER, ALERTMIN);
    AlertUnit a = { 1 };

    for(int i = 0; i < SIZE_OF_BUFFER - 1; i++)
    {
        cb.insert(a);
    }

    bool alert = cb.insert(a);
    AssertTrueRet(alert, __func__);
}

// CLF Parser Tests
void clfparser_basicTest()
{
    string clfLine = "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123";
    CLF parsedCLF(clfLine);
    AssertTrueRet(
        parsedCLF.getAddr().compare("127.0.0.1") == 0 &&
        parsedCLF.getUser().compare("james") == 0 &&
        parsedCLF.getTime().compare("09/May/2018:16:00:39 +0000") == 0 &&
        parsedCLF.getRequest().compare("GET /report HTTP/1.0") == 0 &&
        parsedCLF.getStatus() == 200 &&
        parsedCLF.getSize() == 123, __func__);
}

void clfparser_invalidFormatTest()
{
    string clfLine = "127.0.0.1 james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123";
    CLF parsedCLF(clfLine);
    AssertTrueRet(!parsedCLF.isValid(), __func__);
}

void clfparser_invalidEmptyTest()
{
    string clf = "";
    CLF parsedCLF(clf);
    AssertTrueRet(!parsedCLF.isValid(), __func__);
}

// Alert E2E Tests
void e2e_runTest(int INTERVAL, int ALERTRANGE, int STATSRANGE, int ALERTMIN, int TIMEOUT, string TESTFILE,
    vector<string> STRINGS_TO_INSERT, vector<string> STRINGS_TO_FIND, string callingFunction)
{
    cout << "Running " << callingFunction << " for " << TIMEOUT << " seconds." << endl;

    thread t([INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_FIND, callingFunction]{
        HttpMonitor hm(TESTFILE, INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT);
        
        // Capture stdout
        std::ostringstream ss;
        std::streambuf *psbuf, *backup;
        backup = std::cout.rdbuf();
        psbuf = ss.rdbuf();
        std::cout.rdbuf(psbuf);

        // Perform log parsing
        hm.parseLog();

        // Restore stdout
        std::cout.rdbuf(backup);

        bool passed = true;
        string output = ss.str();
        for(int i = 0; i < STRINGS_TO_FIND.size(); i++)
        {
            passed = (output.find(STRINGS_TO_FIND[i]) != string::npos) && passed;
        }

        assertTrue(passed, callingFunction);
    });
    
    ofstream outfile;
    outfile.open(TESTFILE, std::ios_base::app);

    this_thread::sleep_for(chrono::seconds(2));
    for (int i = 0; i < STRINGS_TO_INSERT.size(); i++)
    {
        outfile << STRINGS_TO_INSERT[i] << endl;
    }

    t.join();
}

void alerter_basicTest()
{
    int INTERVAL = 1;
    int ALERTRANGE = 12;
    int STATSRANGE = 99999;
    int ALERTMIN = 10;
    int TIMEOUT = 10;
    string TESTFILE = "tests/test.txt";
    vector<string> STRINGS_TO_INSERT = {
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123"
    };
    vector<string> STRINGS_TO_FIND = { "High traffic generated an alert" };
    string callingFunction = __func__;

    e2e_runTest(INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_INSERT, STRINGS_TO_FIND, callingFunction);    
}

void alerter_basicRecoveryTest()
{
    int INTERVAL = 1;
    int ALERTRANGE = 12;
    int STATSRANGE = 99999;
    int ALERTMIN = 10;
    int TIMEOUT = 15;
    string TESTFILE = "tests/test.txt";
    vector<string> STRINGS_TO_INSERT = {
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123"
    };
    vector<string> STRINGS_TO_FIND = { "High traffic generated an alert", "High traffic alert recovered at" };
    string callingFunction = __func__;

    e2e_runTest(INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_INSERT, STRINGS_TO_FIND, callingFunction);    
}

void alerter_basicCorruptionTest()
{
    int INTERVAL = 1;
    int ALERTRANGE = 12;
    int STATSRANGE = 99999;
    int ALERTMIN = 10;
    int TIMEOUT = 15;
    string TESTFILE = "tests/test.txt";
    vector<string> STRINGS_TO_INSERT = {
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123"
    };
    vector<string> STRINGS_TO_FIND = { "High traffic generated an alert", "High traffic alert recovered at" };
    string callingFunction = __func__;

    e2e_runTest(INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_INSERT, STRINGS_TO_FIND, callingFunction);    
}

// Stats E2E tests
void stats_basicTest()
{
    int INTERVAL = 1;
    int ALERTRANGE = 12;
    int STATSRANGE = 10;
    int ALERTMIN = 10;
    int TIMEOUT = 11;
    string TESTFILE = "tests/test.txt";
    vector<string> STRINGS_TO_INSERT = {
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /report HTTP/1.0\" 200 123",
        "127.0.0.1 - jill [09/May/2018:16:00:41 +0000] \"GET /api/user HTTP/1.0\" 200 234",
        "127.0.0.1 - frank [09/May/2018:16:00:42 +0000] \"POST /api/user HTTP/1.0\" 200 34",
        "127.0.0.1 - mary [09/May/2018:16:00:42 +0000] \"POST /api/user HTTP/1.0\" 503 12"
    };
    vector<string> STRINGS_TO_FIND = {
        "1. /api",
        "Success%: 75%",
        "1. 127.0.0.1",
        "Largest Request size: 234 bytes",
        "Average Request size: 100 bytes",
        };
    string callingFunction = __func__;

    e2e_runTest(INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_INSERT, STRINGS_TO_FIND, callingFunction);    
}

void stats_basicCorruptedLogTest()
{
    int INTERVAL = 1;
    int ALERTRANGE = 12;
    int STATSRANGE = 10;
    int ALERTMIN = 10;
    int TIMEOUT = 11;
    string TESTFILE = "tests/test.txt";
    vector<string> STRINGS_TO_INSERT = {
        "127.0.0.1 - james [09/May/2018:16:00:39 +0000] \"GET /api HTTP/1.0\" 200 123",
        "127.0.0.1frank [09/May/2018:16:00:42 +0000] \"POST /api/user HTTP/1.0\" 200 34",
        "127.0.0.1 - mary [09/May/2018:16:00:42 +0000] \"POST /api/user HTTP/1.0\" 503 12"
    };
    vector<string> STRINGS_TO_FIND = {
        "1. /api",
        "Success%: 50%",
        "1. 127.0.0.1",
        "Largest Request size: 123 bytes",
        "Average Request size: 67 bytes",
        };
    string callingFunction = __func__;

    e2e_runTest(INTERVAL, ALERTRANGE, STATSRANGE, ALERTMIN, TIMEOUT, TESTFILE, STRINGS_TO_INSERT, STRINGS_TO_FIND, callingFunction);    
}


// Main test driver
// With more time I'd want to make this more customizable,
// Run multiple times, run only certain tests, etc.
// But for now it runs everything

int main()
{
    alertbuffer_basicTest();
    alertbuffer_largeCorrectness();
    alertbuffer_alertBasic();
    alertbuffer_alertLarge();
    clfparser_basicTest();
    clfparser_invalidEmptyTest();
    clfparser_invalidFormatTest();
    alerter_basicTest();
    alerter_basicRecoveryTest();
    alerter_basicCorruptionTest();
    stats_basicTest();
    stats_basicCorruptedLogTest();
}