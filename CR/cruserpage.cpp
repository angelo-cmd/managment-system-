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
    cout << body() << endl;

      cout << form().set("action", "http://localhost/cgi-bin/clientcr.cgi").set("method", "post").set("name", "form2");

    cout << h2("Challenge response page ") << endl;
   std::srand(std::time(nullptr));
    
    // Generate a random 6-digit number
    int random_num = std::rand() % 1000000;
    cout << input().set("type", "text").set("name", "number").set("readonly").set("value",std::to_string(random_num))<< endl;
  string res=  Generate_Hash(std::to_string(random_num)+Gets_key(user_id));
    cout << input().set("type", "hidden").set("name", "string").set("readonly").set("value",res)<< endl;


    cout << input().set("type", "submit").set("value", "submit") << endl;
    cout << form() << endl;

    cout << body() << endl;
    cout << html() << endl;
}