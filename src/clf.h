#pragma once
#include <string>

class CLF
{
    private:
        std::string m_addr;
        std::string m_user;
        std::string m_time;
        std::string m_request;
        int m_status;
        int m_size;

        bool m_isValid;

    public:
        // CLF(string)
        // Constructs a CLF from a w3c-format line
        CLF(std::string line);

        // GetSection:
        //  Gets section from CLF
        //  A section defined as '/foo/bar' => '/foo'
        std::string getSection();

        inline std::string getAddr() { return m_addr; }
        inline std::string getUser() { return m_user; }
        inline std::string getTime() { return m_time; }
        inline std::string getRequest() { return m_request; }
        inline int getStatus() { return m_status; }
        inline int getSize() { return m_size; }
        inline bool isValid() { return m_isValid; }
};