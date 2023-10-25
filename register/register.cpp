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
/// @brief  check if an admin exists, if not exist is impossible to register another admin
/// @param username
/// @param email
/// @param password hash password
/// @return -1 error , 0 not  exist, 1  exists
int AdminExist()
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
          {"user", "guest"},
          {"password", "guest"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("CW2");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("Select isadmin FROM Users Where isadmin=1 LIMIT 1");

      sql::ResultSet *res = st->executeQuery();

      if (res->next())
      {
         result = 1; // admin already exist
      }
      else
      {

         conn->close();
         delete res;
         delete st;
         result = 0;
      }

      // Close Connection
      conn->close();
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
   return result;
}
/// @brief  register a user
/// @param username
/// @param email
/// @param password hash password
/// @return 0 success -1 failed
int QueryRegisterAdmin(string username, string email, string password)
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
          {"user", "guest"},
          {"password", "guest"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("CW2");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("INSERT INTO Users ( email, username, password,isadmin) VALUES ( ?, ?, ?,1) RETURNING id");

      st->setString(1, email);
      st->setString(2, username);
      st->setString(3, password);
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
   return result;
}
/// @brief  register a user
/// @param username
/// @param email
/// @param password hash password
/// @return 0 success -1 failed
int QueryRegister(string username, string email, string password)
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
          {"user", "guest"},
          {"password", "guest"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("CW2");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("INSERT INTO Users ( email, username, password) VALUES ( ?, ?, ?) RETURNING id");

      st->setString(1, email);
      st->setString(2, username);
      st->setString(3, password);
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
   return result;
}
int main()
{

   Cgicc obj;
   const CgiEnvironment &env = obj.getEnvironment();
   std::string email;
   std::string password;
   string username;
   string tempcheck;
   int checkbox = -1;
   int user_id = -1;
   // get all elements by name
   try
   {

      if (obj.queryCheckbox("admin") == true)
      {
         checkbox=1;
      }
    

     
      email = obj.getElement("Email")->getValue();
      password = obj.getElement("Password")->getValue();
      password = Generate_Hash(password);
      username = obj.getElement("Username")->getValue();
   }
   catch (exception &e)
   {
      // cerr << "Error processing  " << endl;
      //  redirect to register page
      cout << HTTPRedirectHeader("http://localhost/register.html?error=1") << endl;
      // Exit (Failed)
      return -1;
   }
   if ((Sanitize_Email(email)) == -1)
   {
      // cout << "Email is not valid" << endl;
      cout << HTTPRedirectHeader("http://localhost/register.html?error=1") << endl;

      return -1;
   }
   if ((Sanitize_Username(username)) == -1)
   {

      // cout << "username is not valid" << endl;
      cout << HTTPRedirectHeader("http://localhost/register.html?error=1") << endl;
      return -1;
   }

   if (checkbox == 1)
   {

      if (AdminExist() == 0)
      {
         user_id = QueryRegisterAdmin(username, email, password);
      }
      else
      {
         cout << HTTPRedirectHeader("http://localhost/register.html?error=1") << endl;
         return -1; // we should redirect user to another page
      }
   }
   else
   {

      user_id = QueryRegister(username, email, password);
      // result is the id of the user
   }

   if (user_id == -1)
   {
      cout << HTTPRedirectHeader("http://localhost/register.html?error=1") << endl;
      return -1; // we should redirect user to another page
   }
   string ENV = env.getRemoteAddr(); // get IP
   int id_session = -1;
   id_session = SaveSession(user_id, ENV);
   if (id_session == -1)
   { // error
      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      return -1; // we should redirect user to another page
   }

   setCookie(user_id, id_session);
   string key = Gets_key(user_id);
   std::string otp = Otp_Generate(key);
   if (otp.empty())
   {
      cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
      return -1;
   }

   if (curl_send_email(email, otp) == 0)
   {
      cout << HTTPRedirectHeader("http://localhost/otp.html") << endl;
      return 0;
   }
   cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
   return -1;
}
