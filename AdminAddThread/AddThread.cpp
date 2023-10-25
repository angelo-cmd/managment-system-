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
/// @brief Add a thread giving the name if the name already exists return -1
/// @param ThreadName
/// @return -1 not success, id_thread success
int Add_Thread(string ThreadName)
{
    int id_thread = -1;

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

        sql::PreparedStatement *st = conn->prepareStatement("Insert into  `Threads`(name) Values (?) returning id_thread"); // check if expired
        st->setString(1, ThreadName);

        ResultSet *res = st->executeQuery(); // return number of row updated,deleted or inserted;

        if (res->next())
        {

            id_thread = res->getInt("id_thread");
            // cout<<" not success"<<endl;
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

    return id_thread;
}

int Add_Comment(int id_thread, int id_user, string comment)
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

        sql::PreparedStatement *st = conn->prepareStatement("INSERT INTO posts (id_thread, id, comment, username) SELECT ?, ?, ?, username FROM Users WHERE Users.id = ?; "); // check if expired
        st->setInt(1, id_thread);
        st->setInt(2, id_user);

        st->setString(3, comment);
        st->setInt(4, id_user);

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
        // cout << "Error Connecting to the database: "
        //  << e.what() << endl;

        // Exit (Failed)
        return -1;
    }
    catch (exception &ex)
    {
        // cout << ex.what() << endl;
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
    string Thread;
    int id_thread = -1;

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
        cout << "Exception: " << e.what() << endl;
    }
    if(Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0){
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
        Thread = cgi.getElement("threadname1")->getValue();
        comment = cgi.getElement("comment")->getValue();
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    if (Sanitize_comment(Thread) == -1 || Sanitize_comment(comment) == -1)
    {
        cout << HTTPRedirectHeader(env.getReferrer()) << endl;
        return -1;
    }
    id_thread = Add_Thread(Thread);
    if (id_thread == -1)
    {
        cout << HTTPRedirectHeader(env.getReferrer()) << endl;
        return -1;
    }
    if (Add_Comment(id_thread, user_id, comment) == -1)
    {
        cout << HTTPRedirectHeader(env.getReferrer()) << endl;
        return -1;
        // error message
    }
    cout << HTTPRedirectHeader(env.getReferrer()) << endl;

    return 0;
}