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

/// @brief // delete a thread irreversible process,
/// @param ThreadName 
/// @return -1 fail, 0 success
int Delete_Thread(string ThreadName) // delete irreversible process,
{

    try
    {
        // Instantiate Driver
        Driver *driver = sql::mariadb::get_driver_instance();

        // Configure Connection, including initial database name "test":
        SQLString url("jdbc:mariadb://localhost");

        // Use a properties map for the other connection options
        Properties properties({
            {"user", "admin"},
            {"password", "mariadbpassword"},

        });

        // Establish Connection
        // Use a smart pointer for extra safety
        unique_ptr<sql::Connection> conn(driver->connect(url, properties));
        conn->setSchema("CW2");

        // Use a smart pointer for extra safety

        sql::PreparedStatement *st = conn->prepareStatement("DELETE FROM `Threads` WHERE Threads.name=?"); // delete the thread, if the name already exists exception
        st->setString(1, ThreadName);

        int res = st->executeUpdate(); // return number of row updated,deleted or inserted;

        if (res == 0)
        {
            conn->close();
            delete st;
            // cout<<" not success"<<endl;
            return -1;
        }

        // Close Connection
        conn->close();
        delete st;
    }
    // Catch Exceptions
    catch (sql::SQLException &e)
    {
        cerr << "Error Connecting to the database: "
             << e.what() << endl;

        // Exit (Failed)
        return -1;
    }

    // Exit (Success)
    // cout<<"success"<<endl;
    return 0;
}
int main()
{
    Cgicc cgi;
    const CgiEnvironment &env = cgi.getEnvironment();

    int session_id = -1;
    int user_id = -1;
    ;
    string Thread;
    string comment;

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
      if (Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0)
    {

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
        return -1;
    }
    UpdateSession(session_id);
   
    setCookie(user_id, session_id);
    try
    {

        Thread = cgi.getElement("threadname")->getValue();
    }
    catch (exception &e)
    {
         cerr << "Exception: " << e.what() << endl;
    }

    if (Sanitize_comment(Thread) == -1)
    {
        // cout<<" not success"<<endl;
        cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
        return -1;
    }
    if (Delete_Thread(Thread) == -1) 
    {
        // cout<<" not success"<<endl;
        cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
        return -1;
    }
    cout << HTTPRedirectHeader(env.getReferrer()) << endl;



    return 0;
}