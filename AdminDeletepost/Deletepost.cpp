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
int Delete_any_posts(int id_thread, int id_post)
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

        sql::PreparedStatement *st = conn->prepareStatement("Delete From `posts` WHERE posts.id_thread=? AND posts.id_post=?"); // delete statemant
        st->setInt(1, id_thread);
        st->setInt(2, id_post);

        int res = st->executeUpdate(); // return number of row updated,deleted or inserted;

        if (res == 0)
        {
            conn->close();
            delete st;
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
    } catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
        return -1;
    }


    // Exit (Success)
    return 0;
}

int main()
{
    Cgicc cgi;
    const CgiEnvironment &env = cgi.getEnvironment();

    int session_id = -1;
    int user_id = -1;
    int id_thread = -1;
    int line = -1;
    int id_post = -1;
    string number;
    string temp_id;
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
    if (Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0) // if its not 1 , the user is not an admin
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
    }

    if (Is_admin(user_id, env.getRemoteAddr(), session_id) != 1) // if its not 1 , the user is not an admin
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
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
        id_thread = stoi(cgi.getElement("id_thread")->getValue());
        number = cgi.getElement("number")->getValue(); // get line number as string

        const_form_iterator iter = cgi.getElement(number);
        if (iter != (*cgi).end()){
        temp_id = cgi.getElement(number)->getValue(); // get id_post as string only if exist 
        }
        else
        {
            
            cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
            return -1;
        }
        id_post = stoi(temp_id);                       // convert string to int
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }

    if (Delete_any_posts(id_thread, id_post) == -1)
    {
        cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
        return -1;
    }
    else
        cout << HTTPRedirectHeader(env.getReferrer()) << endl;

    // isadmin next control page
    // isnot userPanel

    return 0;
}
