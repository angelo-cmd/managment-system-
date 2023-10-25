
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <liboath/oath.h>

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
#include <hash-library/sha256.h>
#include <hash-library/sha256.cpp>
#include <vector>
#include <cstring>
#include <time.h>
#include <chrono>
#include <ctime>

#include <iomanip>
#include "curl/curl.h"
using namespace std;
using namespace cgicc;
using namespace sql;
using namespace std::chrono;

#pragma region Otp_Generate;
/// @brief Verify the otp code insert by the user
/// @param key // secret key of the user
/// @param Otp  // 6 digits code
/// @return  0 success,-1 failed
int VerifyOtp(string key, string Otp)
{
   int result = -1;
   const auto time_step = 300;
   const auto digits = 6;

   auto x = time(NULL);
   result = oath_totp_validate(key.c_str(),
                               key.length(),
                               x,
                               time_step,
                               OATH_TOTP_DEFAULT_START_TIME,
                               1,
                               Otp.c_str());
   if (result == OATH_OK)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
/// reference https://www.nongnu.org/oath-toolkit/liboath-api/liboath-oath.h.html
/// @brief Create an otp code with the secret key of the user,the otp key is valid for 300 seconds
/// @param key secret key
/// @return return the otp cod, empty string if failed
string Otp_Generate(string key)
{
   auto result = oath_init();
   if (result != OATH_OK)
   {
      cout << oath_strerror(result) << endl;
      return NULL;
   }

   const auto time_step = 300;
   const auto digits = 6;
   char otp[digits + 1] = {};

   auto t = time(NULL);
   auto left = time_step - (t % time_step);

   result = oath_totp_generate(
       key.c_str(),
       key.length(),
       t,
       time_step,
       0,
       digits,
       otp);
   oath_done();
   if (result != OATH_OK)
   {
      cerr << oath_strerror(result) << endl;
      return NULL;
   }
   else
   {
      return otp;
   }
}
#pragma endregion
#pragma region Hash
string Generate_Hash(string pass)
{
   SHA256 sha;
   return sha(pass);
}
#pragma endregion

#pragma region Sanitize
int Sanitize_Email(string input)
{

   std::regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,45})"); // withelistings
   if (std::regex_match(input, email_regex))
   {
      // email is valid
      return 0;
   }
   else
   {

      return -1;
   }
}
int Sanitize_Username(string input)
{
   std::regex username_regex(R"([a-zA-Z0-9._%+-]{2,45})"); // withelistings
   if (std::regex_match(input, username_regex))
   {
      // email is valid
      return 0;
   }
   else
   {

      return -1;
   }
}
int Sanitize_comment(string input)
{
   std::regex comment_regex(R"(^[a-zA-Z0-9._%+-,;][a-zA-Z0-9._\s%+-,;]{0,99}$)"); // whitelistings
   if (std::regex_match(input, comment_regex))
   {
      // email is valid
      return 0;
   }
   else
   {

      return -1;
   }
}
#pragma endregion

#pragma region Sendemail
// https://curl.se/libcurl/c/smtp-mail.html
struct upload_status
{
   int lines_read;
   string otp;
};
/// @brief generate the body of the email
/// @param OTP  one time password
/// @param user name of the user
/// @return the body string

vector<string> Buildbody(string otp)
{
   vector<string> body = {

       "From:<noreply@gmail.com>\n"
       "To:<Dear client>\n"
       "Subject: OTP  message\r\n"
       "\r\n" /* empty line to divide headers from body, see RFC5322 */
       "Hello.\r\n"
       "\r\n"
       "This is your code " +
       otp + ".\r\n"

   };
   return body;
}
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
   struct upload_status *upload_ctx = (struct upload_status *)userp;
   const char *data;
   vector<string> body = Buildbody(upload_ctx->otp);

   if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
   {
      return 0;
   }

   data = body[upload_ctx->lines_read].c_str();

   if (data)
   {
      size_t len = strlen(data);
      memcpy(ptr, data, len);
      upload_ctx->lines_read++;

      return len;
   }

   return 0;
}

int curl_send_email(string email, string otp)
{
   CURL *curl;
   CURLcode res = CURLE_OK;
   struct curl_slist *recipients = NULL;
   struct upload_status upload_ctx = {0};
   upload_ctx.otp = otp;

   curl = curl_easy_init();

   if (curl)
   {

      curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp-relay.sendinblue.com:465"); // secure protocol used sendinblue

      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<noreply@gmail.com>");

      recipients = curl_slist_append(recipients, email.c_str()); // set user email
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

      curl_easy_setopt(curl, CURLOPT_USE_SSL, 1L);

      curl_easy_setopt(curl, CURLOPT_USERNAME, "19233615@brookes.ac.uk"); //
      curl_easy_setopt(curl, CURLOPT_PASSWORD, "CchgTJDBSRpG0rUM");
      // size_t first =strlen(body.c);

      curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source); // payload_source is basically a callback
      curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);        // read data from struct
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);

      /* Send the message */
      res = curl_easy_perform(curl);

      /* Check for errors */
      if (res != CURLE_OK)
      {
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
                 curl_easy_strerror(res));
      }

      /* Free the list of recipients */
      curl_slist_free_all(recipients);
      curl_easy_cleanup(curl);
   }

   return (int)res;
}

#pragma endregion;

#pragma region Query
/// @brief  set the paramter otp in the db. This means that the user has successfully passed the 2fa
/// @param id_user id_user
/// @param id_session  id_session
/// @param ip  ip address of the user
/// @return 0 success, -1 otherwise
int Otp_ok(int id_user, int id_session, string ip)
{
   int ret = -1;

   try
   {
      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("tcp://database-1.chtqgy61q2oi.eu-north-1.rds.amazonaws.com:3306");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino10"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("ticket_system");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("UPDATE `Session` SET otp = 1  where id=? AND `ID-session` = ? AND IP = ?"); // set the otp only if respect  requirement
      st->setInt(1, id_user);
      st->setInt(2, id_session);
      st->setString(3, ip);

      if (st->executeUpdate() == 1)
      {
         ret = 0;
         // means user exists we retrive the parameter
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
/// @brief used to retrieve the secrete key of a user,
/// @param id_user  id of the user
/// @return the key or null
string Gets_key(int id_user)
{
   string result;
   try
   {
      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("tcp://database-1.chtqgy61q2oi.eu-north-1.rds.amazonaws.com:3306");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino10"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("ticket_system");

     
     

      

      sql::PreparedStatement *st = conn->prepareStatement("Select secret_key FROM Users where id=?"); // check if expired

      st->setInt(1, id_user);

      sql::ResultSet *res = st->executeQuery();
      if (res->next())
      {
         result = res->getString("secret_key");
         // means user exists we retrive the parameter
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
   catch (exception_ptr &e) // catch general exception
   {
      return result;
   }

   // Exit (Success)
   return result;
}

/// @brief This function check if the has been authenticathed with the otp code
/// @param id_user
/// @param ip
/// @param id_session
/// @return -1 not authenticated or fail , 0 success
int Otp_auth_check(int id_user, string ip, int id_session)
{
   int result = -1;

   try
   {

      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("tcp://database-1.chtqgy61q2oi.eu-north-1.rds.amazonaws.com:3306");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino10"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("ticket_system");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("SELECT otp FROM `Session` WHERE Session.id=? AND `IP`=? AND `ID-session`=?"); // check if expired

      st->setInt(1, id_user);
      st->setString(2, ip);
      st->setInt(3, id_session);

      sql::ResultSet *res = st->executeQuery();
      if (res->next())
      {
         result = res->getInt("otp");
         // means user exists we retrive the parameter
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
      return -1;
   }

   if (result == 1)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}

// tested be careful to the backticks
/// @brief run the query and check to save the user in the db
/// @param IP
/// @param id
/// @return id session  success or  -1 failed
int SaveSession(int id, string IP)
{

   try
   {
      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("tcp://database-1.chtqgy61q2oi.eu-north-1.rds.amazonaws.com:3306");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino10"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("ticket_system");

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("INSERT INTO Session ( id, IP) VALUES ( ?, ?) RETURNING `ID-session`"); // this statement return the id generated in the db

      st->setInt(1, id);
      st->setString(2, IP);
      sql::ResultSet *res = st->executeQuery();

      if (res->next())
      {
         id = res->getInt("ID-session");
         // means user exists we retrive the parameter
      }
      else
      {
         conn->close();
         delete res;
         delete st;
         return id;
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
   catch (exception_ptr &e) // catch general exception
   {
      return -1;
   }

   // Exit (Success)
   return id;
}

/// @brief Check if the session is expired
/// @param username
/// @param email
/// @param password
/// @return 0 success -1 failed
int SessionExpired(int session_id)
{
   int ret = -1;

   try
   {
      // Instantiate Driver

      Driver *driver = sql::mariadb::get_driver_instance();

      // Configure Connection, including initial database name "test":
      SQLString url("tcp://database-1.chtqgy61q2oi.eu-north-1.rds.amazonaws.com:3306");

      // Use a properties map for the other connection options
      Properties properties({
          {"user", "admin"},
          {"password", "Villino10"},

      });

      // Establish Connection
      // Use a smart pointer for extra safety
      unique_ptr<sql::Connection> conn(driver->connect(url, properties));
      conn->setSchema("ticket_system");

      
      
      ;
      

      // Use a smart pointer for extra safety

      sql::PreparedStatement *st = conn->prepareStatement("Select `ID-session` from Session where `ID-session`=?  AND timestamp > CURRENT_TIMESTAMP() "); // check if expired

      st->setInt64(1, session_id);
      sql::ResultSet *res = st->executeQuery();
      if (res->next())
      {
         ret = res->getInt("ID-session");
         // means user exists we retrive the parameter
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
      return -1;
   }
   catch (exception_ptr &e) // catch general exception
   {
      return -1;
   }

   // Exit (Success)
   return ret;
}
/// @brief  This function check if the id is not expired in that case it will update the session
/// @param session_id  id of the session
/// @return 0 if success -1 not success
void UpdateSession(int session_id)
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

      sql::PreparedStatement *st = conn->prepareStatement("UPDATE `Session` SET timestamp = DATE_ADD(CURRENT_TIMESTAMP(), INTERVAL 5 MINUTE) WHERE `ID-session` = ? AND timestamp > CURRENT_TIMESTAMP() AND otp=1;"); // check if expired
      st->setInt(1, session_id);

      st->executeQuery();

      // Close Connection

      delete st;
   }
   // Catch Exceptions
   catch (sql::SQLException &e)
   {
      cerr << "Error Connecting to the database: "
           << e.what() << endl;

      // Exit (Failed)
   }
   catch (exception_ptr &e) // catch general exception
   {
   }

   // Exit (Success)
}
/// @brief show the list of thread
/// @return return a vector containing thread name and id
vector<string> ShowListThread()
{
   vector<string> names;

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

      sql::PreparedStatement *st = conn->prepareStatement("Select name,id_thread from Threads"); // check if expired

      sql::ResultSet *res = st->executeQuery();

      if (res->rowsCount() == 0)
      {
         return names;
      }

      int count = 0;
      while (res->next())
      { // get all elements
         names.push_back((string)res->getString("name"));
         names.push_back(to_string(res->getInt("id_thread")));
         count++;
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
      return names;
   }
   catch (exception_ptr &e) // catch general exception
   {
      return names;
   }

   // Exit (Success)
   return names;
}

/// @brief show the posts given an id
/// @param thread_id
/// @return return a vector containing [id_post][username][comment] ,or empty vector
vector<string> ShowPosts(int thread_id)
{
   vector<string> posts;

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

      sql::PreparedStatement *st = conn->prepareStatement("SELECT id_post,`username`,`comment` FROM `posts` WHERE posts.id_thread = ?   ORDER BY posts.timestamp ASC"); // check if expired
      st->setInt(1, thread_id);
      sql::ResultSet *res = st->executeQuery();

      if (res->rowsCount() == 0)
      {
         return posts;
      }

      while (res->next())
      { // get all elements
         posts.push_back((string)res->getString("id_post"));
         posts.push_back((string)res->getString("username"));
         posts.push_back((string)res->getString("comment"));
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
      return posts;
   }
   catch (exception_ptr &e) // catch general exception
   {
      return posts;
   }

   // Exit (Success)
   return posts;
}

#pragma endregion

#pragma region setCookie
void setCookie(int id_user, int id_session)
{

   cout << "Set-Cookie:id_user=";
   cout << id_user;
   cout << "; Domain=localhost; ";
   cout << "Path=/; ";

   cout << "SameSite=Strict; ";
   cout << "Max-Age=300;\n";

   cout << "Set-Cookie:id_session=";
   cout << id_session;
   cout << "; Domain=localhost; ";
   cout << "Path=/; ";

   cout << "SameSite=Strict; ";
   cout << "Max-Age=300;\n";
}
