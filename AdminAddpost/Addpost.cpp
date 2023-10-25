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
int Add_Admin_post(int id_thread, int id_user, string comment)
{
   int ret = -1;

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

      sql::PreparedStatement *st = conn->prepareStatement("INSERT INTO posts (id_thread,id,username,comment) VALUES(?, ?,(SELECT  username FROM `Users` WHERE id = ?),?)"); // insert statement

      st->setInt(1, id_thread);
      st->setInt(2, id_user);
      st->setInt(3, id_user);

      st->setString(4, comment);
     int result = st->executeUpdate();
      if (result==1) //means 1 row updated
      {
         ret = 0; //set return value
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
   return ret;
}
int main()
{
    Cgicc cgi;
    const CgiEnvironment &env = cgi.getEnvironment();

    int session_id = -1;
    int user_id = -1;
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
        cerr << "Exception: " << e.what() << endl;
    }
     if (Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0)
    {

        cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
        return -1;
    }
    if (Is_admin(user_id, env.getRemoteAddr(), session_id) == -1)
    {

        cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
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
        comment = cgi.getElement("comment")->getValue();
        
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    if(Sanitize_comment(comment)==-1){
     cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
     return -1;

    }
    if (Add_Admin_post(id_thread, user_id, comment) == -1)
    { 

 

        cout << HTTPRedirectHeader(env.getReferrer()) << endl; // go to the preavious page
        return -1;
    }
    

        cout << HTTPRedirectHeader(env.getReferrer()) << endl;

    return 0;
}
