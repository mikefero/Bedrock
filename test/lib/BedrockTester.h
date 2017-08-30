#pragma once
#include <libstuff/libstuff.h>
#include <sqlitecluster/SQLite.h>
#include <test/lib/TestHTTPS.h>
#include <test/lib/tpunit++.hpp>

class BedrockTester {
  public:
    // Generate a temporary filename for a test DB, with an optional prefix.
    static string getTempFileName(string prefix = "");

    // Returns the name of the server binary, by finding the first path that exists in `locations`.
    static string getServerName();

    // Search paths for `getServerName()`. Allowed to be modified before startup by implementer.
    static list<string> locations;

    // Default values for the location of the DB file and the server to talk to.
    // These can be over-ridden when instantiating a tester.
    // Typically, these values will be set in main().
    static string dbFile;
    static string serverAddr;

    // This is expected to be set by main, built from argv, to expose command-line options to tests.
    static SData globalArgs;

    // Shuts down all bedrock servers associated with any testers.
    static void stopAll();

    SQLite* db = nullptr;
    SQLite* writableDB = nullptr;


    string getServerAddr() { return _serverAddr; };

    // Constructor/destructor
    BedrockTester(const string& filename = "",
                  const string& serverAddress = "",
                  const list<string>& queries = {},
                  const map<string, string>& args = {},
                  bool start = true,
                  bool keepFilesWhenFinished = false);
    ~BedrockTester();

    // Start and stop the bedrock server.
    void stopServer();
    void startServer();

    // Takes a list of requests, and returns a corresponding list of responses.
    // Uses `connections` parallel connections to the server to send the requests.
    vector<SData> executeWaitMultipleData(vector<SData> requests, int connections = 10);

    // Sends a single request, returning the response content.
    // If the response method line doesn't begin with the expected result, throws.
    string executeWaitVerifyContent(SData request, const string& expectedResult = "200");

    string readDB(const string& query);
    bool readDB(const string& query, SQResult& result);
    SQLite& getSQLiteDB();
    SQLite& getWritableSQLiteDB();

  private:
    // returns a list of arguments with which to start the server.
    list<string> getServerArgs();

    // Args passed on creation, which will be used to start the server if the `start` flag is set, or if `startServer`
    // is called later on with an empty args list.
    const map<string, string> _args;

    // If these are set, they'll be used instead of the global defaults.
    string _serverAddr;
    string _dbName;

    // The PID of the bedrock server we started.
    int _serverPID = 0;

    // A set of all bedrock testers.
    static set<BedrockTester*> _testers;

    // Flag indicating whether the DB should be kept when the tester is destroyed.
    bool _keepFilesWhenFinished;
};
