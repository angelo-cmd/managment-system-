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
/// @brief A user delete is own post
/// @param id_thread
/// @param id_user
/// @param id_post
/// @return -1 fails, 0 success
int Delete_posts(int id_thread, int id_user, int id_post)
{

    try
    {
        // Instantiate Driver
        Driver *driver = sql::mariadb::get_driver_instance();

        // Configure Connection, including initial database name "test":
        SQLString url("jdbc:mariadb://localhost");

        // Use a properties map for the other connection options
        Properties properties({
            {"user", "db_user"},
            {"password", "db_user_password"},

        });

        // Establish Connection
        // Use a smart pointer for extra safety
        unique_ptr<sql::Connection> conn(driver->connect(url, properties));
        conn->setSchema("CW2");

        // Use a smart pointer for extra safety

        sql::PreparedStatement *st = conn->prepareStatement("Delete From `posts`  WHERE id_thread=? AND id_post=? AND id=?"); // check if expired
        st->setInt(1, id_thread);
        st->setInt(2, id_post);
        st->setInt(3, id_user);
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
    }
    catch (exception_ptr &e) // catch general exception
    {

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
        cerr << "Exception:1 " << e.what() << endl;
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
    }
    if (Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0) // if its not 0 , the user didn't pass the otp code
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
        number = cgi.getElement("number")->getValue(); // get line number as string only if exists

        const_form_iterator iter = cgi.getElement(number);
        if (iter != (*cgi).end()){
        temp_id = cgi.getElement(number)->getValue(); // get id_post as string only if exist 
        }
        else
        {
            
            cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
            return -1;
        }

        
        id_post = stoi(temp_id);                      // convert string to int
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Invalid argument exception caught: " << e.what() << std::endl;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range exception caught: " << e.what() << std::endl;
    }
    catch (exception &e)
    {
        cerr << "Exception:2 " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Exception:3 " << endl;
    }

    if (Delete_posts(id_thread, user_id, id_post) == -1)
    {
        // cerr << "Error: could not delete post" << endl;
        cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
        return -1;
    }
    else
        cout << HTTPRedirectHeader(env.getReferrer()) << endl;

    return 0;
}
