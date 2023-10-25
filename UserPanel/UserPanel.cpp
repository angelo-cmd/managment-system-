#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTMLClasses.h>
#include <mariadb/conncpp.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <cgicc/HTTPCookie.h>
#include </home/malware/AngeloAffatati/secure programming/cwk2/Useful.cpp>

using namespace std;
using namespace sql;
using namespace cgicc;

int main()
{
    Cgicc cgi;
    const CgiEnvironment &env = cgi.getEnvironment();
    vector<string> names = ShowListThread();

    int session_id = -1;
    int user_id = -1;

    try
    {
        const_cookie_iterator iter;

        for (iter = env.getCookieList().begin(); iter != env.getCookieList().end(); ++iter)
        {
            if (iter->getName() == "id_session")
            {
                session_id = stoi(iter->getValue());
            }
            if (iter->getName() == "id_user")
            {
                user_id = stoi(iter->getValue());
            }
        }
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    if (SessionExpired(session_id) == -1)
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
    }
    UpdateSession(session_id);


    setCookie(user_id, session_id);

    cout << HTTPHTMLHeader() << endl;

    cout << html() << head(title("Display table")).add(cgicc::link().set("rel", "stylesheet").set("href", "../styles.css").set("style", "text/css")) << endl;
    cout << script() << "\n";

cout << "      function deleteCookies() {\n";
  cout << "        document.cookie = 'id_user=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;';\n";
  cout << "        document.cookie = 'id_session=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;';\n";
  cout << "        window.location.href = '/login.html';\n";
  cout << "      }\n";
    cout << script() << "\n";
    cout << head() << endl;
    cout << body() << endl;
    cout << form();
    cout << input().set(" type", "button").set("value", "Logout").set("onclick", "deleteCookies()") << endl;
    cout << form() << endl;
    cout << h1("LIST TOPICS") << endl;
    cout << h2("Chose your topic") << endl;
    cout << ol();
    for (int st = 0; st < names.size(); st++)
    { // show all threads
        cout << li() << a(names.at(st)).set("href", "http://localhost/cgi-bin/topic.cgi?topic=" + names.at(st + 1)) << li() << endl;
        st++;
    }
    cout << ol() << endl;
    cout << body() << endl;
    cout << html() << endl;

    // isadmin next control page
    // isnot userPanel

    return 0;
}
