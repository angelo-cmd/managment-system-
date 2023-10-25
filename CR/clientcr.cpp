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
    int code;
    
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



     
   string hash=Generate_Hash(cgi.getElement("number")->getValue()+Gets_key(user_id));

   if(hash==cgi.getElement("string")->getValue()){
    cout<<HTTPRedirectHeader("http://localhost/cgi-bin/AdminPanel.cgi")<<endl;
    return 0;
   }else{
        cout<<HTTPRedirectHeader(cgi.getEnvironment().getReferrer())<<endl;
        return -1;

   }
     


}