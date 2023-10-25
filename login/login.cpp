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
/// @brief run the query to get the user from the database if not exist, redirect to login page with error message ; used in login
/// @param email
/// @param password
/// @return user id if success, -1 failed
int QueryLogin(string email, string password)
{
   int result = -1;

   try
   {
      // Instantiate Driver
      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("jdbc:mariadb://localhost");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino19"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("villino_system");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("Select id,email from Users where email=? and password=?");
      st->setString(1, email);
      st->setString(2, password);
      sql::ResultSet *res = st->executeQuery();

      if (res->next())
      {
         result = res->getInt("id");
         // means user exists we retrive the parameter
      }
      else
      {
         conn->close();
         delete res;
         delete st;
         return result;
      }

      // Close Connection
      conn->close();
      delete res;
      delete st;
   }

   // Catch Exceptions
   catch (sql::SQLException &e)
   {
      cerr << "Error Connecting to the database: "
           << e.what() << endl;

      // Exit (Failed)
      return result;
   }
   catch (exception e)
   {
      cout << e.what();
      return result;
   }

   // Exit (Success)
   return result;
}
int main()
{

   Cgicc obj;

   string email;
   string password;
   int user_id = -1;

   // get all elements by name
   try
   {
      email = obj.getElement("email")->getValue();

      password = obj.getElement("password")->getValue();

      password = Generate_Hash(password);
   }
   catch (exception &e)
   {
      // cerr << "Error processing  " << endl;
      //  redirect to register page
      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      // Exit (Failed)
      return -1;
   }
   if ((Sanitize_Email(email)) == -1)
   {

      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      return -1;
   }
   user_id = QueryLogin(email, password);
   if (user_id == -1)
   {
      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      return -1; // we should redirect user to another page
   }

   const CgiEnvironment &env = obj.getEnvironment();
   string ENV = env.getRemoteAddr();
   int id_session;
   id_session = SaveSession(user_id, ENV);
   if (id_session == -1)
   { // error
      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      return -1; // we should redirect user to another page
   }

   setCookie(user_id, id_session);
   string key = Gets_key(user_id);
   std::string otp = Otp_Generate(key);
   if (!otp.empty())
   {
      if (curl_send_email(email, otp) == 0)
      {
         cout << HTTPRedirectHeader("http://localhost/otp.html") << endl;

         return 0;
      }
   }

   cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
   return -1; // we should redirect user to another page
}