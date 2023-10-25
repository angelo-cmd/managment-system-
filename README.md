# managment-system-
This project aims to build an online forum management system with a deep focus on the security aspects of the implementation.

Functional Requirements

FR1	There are two kinds of user: ordinary users and the administrator. Both must be able to register an account and set a password.
FR2	Both types of user can see a list of the threads available, view all the posts in a thread of their choice, add a post of their own at the end of a thread and delete any of their posts.
FR3	The administrator can also create a new thread with their contribution as the first post, delete posts of others, and delete whole threads. There is only one administrator account.

Security requirement
SR1	Both kinds of user must be able to log in with the password they have chosen.
SR2	Both kinds of user must be able to log out and will be automatically logged out after a fixed period of time of inactivity.
SR3	The software must maintain a session and it should do so using cookies.
SR4	If a user attempts to access a page without being logged in, they should be redirected to the log in page.
SR5	Passwords must be encrypted when stored in the database; you may use cryptographic libraries for this if you wish.
SR6	All user inputs should be sanitised before they are used.
SR7	The process of logging in should use two-factor authentication. The user must enter a second password sent by email after the main password has been entered. The email address to be used is the one entered when registering the account. If you are not able to install the relevant mail library, you can simulate the process of emailing by appending to a “mail spool” text file representing all the emails that have been sent.
SR8	The administrator account, in addition to the protections of SR7, must be authenticated by a challenge-response software that simulates the concept of a hardware token. You are encouraged to do the necessary research into the concept of hardware token to understand how they work and then think carefully about how to solve the problem of replicating that behaviour in software.


