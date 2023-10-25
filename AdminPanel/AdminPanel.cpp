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
    {  cerr << "Exception: " << e.what() << endl;
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1; // so  this is a  way to ensure that  the code below is never executed
      
    }
    if(Otp_auth_check(user_id, env.getRemoteAddr(), session_id)!=0){  // 

        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1; // so  this is a  way to ensure that  the code below is never executed
    }
    if (Is_admin(user_id, env.getRemoteAddr(), session_id) != 1)
    {

        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1; // so  this is a  way to ensure that  the code below is never executed
    }
    if (SessionExpired(session_id) == -1)
    {

        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1; // so  this is a  way to ensure that  the code below is never executed
    }
    UpdateSession(session_id);

    setCookie(user_id, session_id);

    cout << HTTPHTMLHeader() << endl;

    cout << html() << head(title("Admin panel")).add(cgicc::link().set("rel", "stylesheet").set("href", "../styles.css").set("style", "text/css")) << endl;
    cout << script() << "\n";

    cout << "      function deleteCookies() {\n";
    cout << "        document.cookie = 'id_user=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;';\n";
    cout << "        document.cookie = 'id_session=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;';\n";
    cout << "        window.location.href = '/login.html';\n";
    cout << "      }\n";
    cout << script() << "\n";
    cout << head() << endl;
    cout << body() << endl;
    cout << input().set(" type", "button").set("value", "Logout").set("onclick", "deleteCookies()") << endl;
    cout << h1(" Welcome admin this you list of topic") << endl;
    cout << h2("Chose your topic") << endl;
    cout << ol();
    vector<string> names = ShowListThread();
    for (int st = 0; st < names.size(); st++)
    { // show all threads
        cout << li() << a(names.at(st)).set("href", "http://localhost/cgi-bin/Admintopic.cgi?topic=" + names.at(st + 1)) << li() << endl;
        st++;
    }
    cout << ol() << endl;
    cout << form().set("action", "http://localhost/cgi-bin/DeleteThread.cgi").set("method", "post").set("name", "form1");

    cout << h2("Insert the name of thread you want to delete") << endl;

    cout << input().set("type", "text").set("name", "threadname").set("required") << endl;

    cout << input().set("type", "submit").set("value", "Delete thread") << endl;
    cout << form() << endl;
    cout << br() << endl;
    cout << br() << endl;

    cout << form().set("action", "http://localhost/cgi-bin/AddThread.cgi").set("method", "post").set("name", "form2");

    cout << h2("Insert the name of thread you want to add") << endl;
    cout << h3("Add a thread. The name must be different from the others") << endl;
    cout << input().set("type", "text").set("name", "threadname1").set("required") << endl;
    cout << h3("Add the first comment") << endl;
    cout << input().set("type", "text").set("name", "comment").set("required") << endl;

    cout << input().set("type", "submit").set("value", "Add thread") << endl;
    cout << form() << endl;

    cout << body() << endl;
    cout << html() << endl;

    // isadmin next control page
    // isnot userPanel
    /*  int message = curl_send_email();
     std::cout << message << std::endl; */

    return 0;
}
