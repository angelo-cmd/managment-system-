
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
#include <vector>
#include <cstring>
#include <time.h>
#include "curl/curl.h"
#include </home/malware/AngeloAffatati/secure programming/cwk2/Useful.cpp>
using namespace std;
using namespace cgicc;

int main()
{
  Cgicc cgi;
  const CgiEnvironment &env = cgi.getEnvironment();
  int session_id = -1;
  int user_id = -1;
  char *otp;
  
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
  if (SessionExpired(session_id) == -1) // i do't update the session in this page because the otp is valid only for 300 second as the session
  {


    cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
  }

  try
  {

    otp = (char *)cgi.getElement("otp")->getValue().c_str(); // get the otp code inserted by the user

    if (sizeof(otp) < 6 || sizeof(otp) > 8)
    {
      cout << HTTPRedirectHeader(cgi.getEnvironment().getReferrer()) << endl; // return previous page
      return -1;
    }
  }
  catch (exception &e)
  {
    // cerr << "Error processing  " << endl;
    // redirect to register page
    cout << HTTPRedirectHeader("http://localhost/login.html?error=1") << endl;
    // Exit (Failed)
    return -1;
  }
  if (VerifyOtp(Gets_key(user_id), otp) == 0)
  {
    // otp is ok set session authenticator
    if (Otp_ok(user_id, session_id, cgi.getEnvironment().getRemoteAddr()) == -1)
    {
      cout << HTTPRedirectHeader(cgi.getEnvironment().getReferrer()) << endl; // return previous page
      return -1;
    }

    int is_admin = Is_admin(user_id, env.getRemoteAddr(), session_id);
    if (is_admin == 1)
    {
      cout << HTTPRedirectHeader("http://localhost/cgi-bin/cruserpage.cgi") << endl;
      return 0;
    }
    else if (is_admin == 0)
    {
      cout << HTTPRedirectHeader("http://localhost/cgi-bin/userpanel.cgi") << endl; // user page
      return 0;
    }
    else
    {
      cout << HTTPRedirectHeader(cgi.getEnvironment().getReferrer()) << endl; // return previous page
      return -1;
    }
  }
  else
  {
    cout << HTTPRedirectHeader(cgi.getEnvironment().getReferrer()) << endl; // return previous page
    return -1;
  }

  return 0;
}
