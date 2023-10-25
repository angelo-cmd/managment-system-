Management System

# 1 Introduction

This project aims to build an online forum management system with a deep
focus on the security aspects of the implementation.

**2.1 Environment**

The software has been tested on a Virtual Machine (VM) running Lubunto.
The IDE Visual Studio code 2022 has been used. The APACHE web server has
been used. One of Apache's features is the ability to execute Common
Gateway Interface (CGI) scripts. A CGI script is a program that runs on
the web server and creates dynamic content for the client's web browser.

**3 Requirements**

*Table 1 Functional Requirements*

|      Name     |      Requirement                                                                                                                                                                                     |
|---------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|      FR1      |      There are two kinds of user: ordinary users and the administrator.    Both must be able to register an account and set a password.                                                              |
|     FR2       |     Both types of user can see a list of the threads available, view all   the posts in a thread of their choice, add a post of their own at the end of   a thread and delete any of their posts.    |
|     FR3       |     The administrator can also create a new thread with their contribution   as the first post, delete posts of others, and delete whole threads. There is   only one administrator account.         |

*Table 2 Security Requirements*

|      Name     |      Requirement                                                                                                                                                                                                                                                                                                                                                                                                                                          |
|---------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     SR1       |     Both kinds of user must be able to log in with the password they have   chosen.                                                                                                                                                                                                                                                                                                                                                                       |
|     SR2       |     Both kinds of user must be able to log out and will be automatically   logged out after a fixed period of time of inactivity.                                                                                                                                                                                                                                                                                                                         |
|     SR3       |     The   software must maintain a session and it should do so using cookies.                                                                                                                                                                                                                                                                                                                                                                             |
|     SR4       |     If a user attempts to access a page without being logged in, they   should be redirected to the log in page.                                                                                                                                                                                                                                                                                                                                          |
|     SR5       |     Passwords must be encrypted when stored in the database; you may use   cryptographic libraries for this if you wish.                                                                                                                                                                                                                                                                                                                                  |
|     SR6       |     All user inputs should be sanitised before they are used.                                                                                                                                                                                                                                                                                                                                                                                             |
|     SR7       |     The process of logging in should use two-factor authentication. The   user must enter a second password sent by email after the main password has   been entered. The email address to be used is the one entered when registering   the account. If you are not able to install the relevant mail library, you   can simulate the process of emailing by appending to a “mail spool” text file   representing all the emails that have been sent.    |
|     SR8       |     The administrator account, in addition to the protections of SR7, must   be authenticated by a challenge-response software that simulates the concept   of a hardware token. You are encouraged to do the necessary research into the   concept of hardware token to understand how they work and then think   carefully about how to solve the problem of replicating that behaviour in   software.                                                  |

*Table 2 Security Requirements*

**4 Design**

I chose a minimal design approach for this development phase and
incorporated a simple CSS document across all pages. The user interface
was crafted to facilitate seamless navigation and enhance system
efficiency.

**4.1 Page Hierarchy**

You\'ll find a detailed representation of the front-end pages that users
encounter during the navigation below.

![Immagine che contiene diagramma Descrizione generata
automaticamente](./image1.png){width="5.709027777777778in"
height="4.006944444444445in"}

*Figura 1 Page Hierarchy*

**4.2 Db design**

![Immagine che contiene diagramma Descrizione generata
automaticamente](./image2.png){width="6.483333333333333in"
height="2.9541666666666666in"}

The database design is developed to minimise redundancy. Constraint
using the primary key and the foreign key relationship has been
implemented to maintain data integrity. In the figure below, there is a
representation of the database model.

**4.3 Database privileges**

The following table illustrates the division of the privilege in the
database to give only the necessary privileges to each user.

  ----------------------------------------------------------------------------------------------
  ROLE         **USERS**       **THREADS**         **SESSIONS**           **POSTS**
  ------------ --------------- ------------------- ---------------------- ----------------------
  Guests       INSERT SELECT                                              

  Db_user      INSERT-SELECT                       INSERT-SELECT-UPDATE   INSERT-SELECT-DELETE

  admin        INSERT SELECT   INSERT DELETE       INSERT-SELECT-UPDATE   INSERT-SELECT-DELETE
                               SELECT                                     
  ----------------------------------------------------------------------------------------------

**5 System Functionality**

The system requirements have been met, allowing users to log in and
register successfully. Additionally, the blog is fully functional,
enabling the admin user to add and delete threads and posts while
allowing normal users to add and delete their posts.

**6 Security features**

**6.1 Password handling**

During registration, each password has been converted into a hashed
value using the SHA256 function and stored in the database per SR5
guidelines. When a user logs in, their password is also hashed and
compared to the stored hash in the database. However, this password
management method is vulnerable to attacks as the hash can be
intercepted and easily decrypted. To overcome this weakness, a practical
solution is to incorporate salt while creating the hash and to hash the
string multiple times.

**6.2 Input validation**

To ensure adherence to SR6, we have sanitised all inputs by only
allowing certain characters through the regex function. We have
developed three functions to protect email, password, and comment
inputs. Additionally, all queries are parameterised to prevent any
potential SQL injection. These measures guarantee compliance with SR6
requirements.

**6.3 Session handling**

The session ID is a sequentially assigned number generated by the
database. While it may seem like an insecure process, the combination of
the expiration date and unique IP address stored in the database creates
a distinct sequence of fields that cannot be replicated by attackers.
Additionally, various functions verify the validity of the session
during page navigation by retrieving the session ID and IP address from
the user\'s browser and comparing them with the data in the database.

**6.3.1 Cookie management**

Every session has 5 min lifetime. After 5 min, the cookie will expire if
no action by the user is made, and the user will be redirected to the
login page.

**6.4 Login process**

To access the system, a regular user must pass two levels of
authentication. The first requires the user to enter their email and
password, while the second involves entering a TOTP code sent to their
email within five minutes. This code is generated using a secret key
linked to the user\'s database and the current timestamp through the
oath library, and is not stored in the system to ensure maximum
security. Additionally, admin authentication involves a
challenge-response process based on hardware token authentication to
comply with requirement SR8.

**6.5 Register process**

To sign up for the website, a new user can register as an admin or a
regular user. As per the requirements outlined in FR3, the system can
only have one admin. If there is no existing admin, a new one can be
registered as per FR1. The user must provide their email, username, and
password during registration. All inputs are sanitised, and if the
registration is successful, they will be redirected to the next page. A
generic error message will be displayed on the screen in case of any
issues.

**6.6 Exception handling**

**6.6.1 Expired session**

A user will be redirected to the login page in the following case:

-   Expired session

-   Session does not fit with the data in the database

**6.7 User without privilege**

If a user attempts to access a page, they are not authorised; they will
be redirected to the login page immediately. This ensures compliance
with requirement SR4.

**7 Improvements**

Several improvements can be performed to this project. First, it is
required to use HTTPS connections; furthermore, custom encryption on
this protocol could be performed to ensure the authenticity of the data.
An additional improvement can be the firewall between the client and the
back-end services to filter the request and avoid any DOS attack on the
server. Also, the connection strings are not encrypted; these strings
should be encrypted and stored in a configuration file.

**8 Attacker view**

Upon closer examination, the programs in question exhibit certain
vulnerabilities from the perspective of a potential attacker.
Specifically, a brute force attack is possible due to the lack of limits
on login attempts. Also the Time-based One-Time Password (TOTP) system
is vulnerable, it employs only six digits, which can be cracked through
brute force methods. The attacker would need to try a maximum of 10\^6
combinations, thus making an attack potentially feasible. Furthermore,
while the connection is unencrypted, a user\'s hash can be retrieved
with relative ease during the login process using a simple tool like
Wireshark. It is possible to perform simple reverse engineering by using
a tool called \"cutter\" on one of the CGI pages. This can retrieve the
connection strings to the database, along with the password.
