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

    string get_request = env.getQueryString();                                    // retrive parameter from url
    int pos = get_request.find("=");                                              // find the char =
    string value_request = get_request.substr(pos + 1, get_request.length() - 1); // retrive the value from the string
    int id_thread = atoi(value_request.c_str());

    vector<string> posts = ShowPosts(id_thread); // get all the posts of a thread from the db

    int session_id = -1;
    int user_id = -1;
    try
    {
        const_cookie_iterator iter;

        for (iter = env.getCookieList().begin(); iter != env.getCookieList().end(); ++iter)
        {
            if (iter->getName() == "id_session") // get id session in the coockies
            {
                session_id = stoi(iter->getValue());
            }
            if (iter->getName() == "id_user") // get user id in the coockies
            {
                user_id = stoi(iter->getValue());
            }
        }
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    if (Otp_auth_check(user_id, env.getRemoteAddr(), session_id) != 0) // if its not 0 , the user is not an admin
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
    }
    if (Is_admin(user_id, env.getRemoteAddr(), session_id) != 1) // if its not 1 , the user is not an admin
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
    }
    if (SessionExpired(session_id) == -1) // check session is valid
    {
        cout << HTTPRedirectHeader("http://localhost/login.html?error=2") << endl;
        return -1;
    }
    UpdateSession(session_id); // update session
    setCookie(user_id, session_id);


    cout << HTTPHTMLHeader() << endl;

    cout << html() << head(title("Display table")).add(cgicc::link().set("rel", "stylesheet").set("href", "styles/default.css").set("style", "text/css")) << endl;

    cout << head() << endl;
    cout << input().set("type", "button").set("value", "Go back!").set("onclick", "history.back()") << endl;

    cout << h2("Read  topic") << endl;
    int line = 0;
    cout << form().set("action", "http://localhost/cgi-bin/Deletepost.cgi").set("method", "post").set("id", "form2");

    cout << table().set("id", "table").set("name", "table").set("border", "1").set("rules", "") << endl;
    cout << tr().add(th("line")).add(th("username")).add(th("comment")) << endl;
    for (size_t i = 0; i < posts.size(); i++) // posts struct [id_post][username][comment]
    {
        cout << tr().add(td(to_string(line))).add(td(posts[i + 1])).add(td(posts[i + 2])); // each line is unique so name=line+"="+post[i] will be an unique string
        cout << tr() << endl;
        cout << input().set("type", "hidden").set("name", to_string(line)).set("value", posts[i]) << endl; // use this hidden field to link the line to the post_id
        i = i + 2;
        line++;
    }
    cout << table() << endl;

    cout << h1("Delete any  post") << endl;

    cout << h2("Insert number of post that you want delete") << endl;
    cout << input().set("type", "hidden").set("name", "id_thread").set("value", to_string(id_thread)) << endl; // used to pass the id_thread of tge thread to delete the post
    cout << input().set("type", "number").set("name", "number").set("required") << endl;
    cout << input().set("type", "submit").set("value", "Delete post") << endl;
    cout << form() << endl;

    cout << h2("Insert  post") << endl;

    cout << form().set("action", "http://localhost/cgi-bin/Addpost.cgi").set("method", "post").set("id", "form2");
    cout << input().set("type", "text").set("name", "comment").set("size", "100").set("style", "width: 100%").set("required") << endl;
    cout << input().set("type", "hidden").set("name", "id_thread").set("value", to_string(id_thread)) << endl; // used to pass the id_thread of tge thread to delete the post
    cout << input().set("type", "submit").set("value", "Add post") << endl;
    cout << form() << endl;

    cout << body() << endl;
    cout << html() << endl;
}