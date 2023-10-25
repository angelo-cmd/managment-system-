-- Active: 1679666571307@@127.0.0.1@3306@CW2
GRANT SELECT ON CW2.`Session` TO 'db_user'@'localhost';
GRANT INSERT ON CW2.`Session` TO 'db_user'@'localhost';

GRANT UPDATE ON CW2.`Session` TO 'db_user'@'localhost';
REVOKE DROP ON TABLE CW2.`Session` FROM 'db_user'@'localhost';

REVOKE DELETE ON TABLE CW2.`Session` FROM 'db_user'@'localhost';


GRANT SELECT ON CW2.`Threads` TO 'db_user'@'localhost';
GRANT INSERT ON CW2.`Threads` TO 'db_user'@'localhost';

REVOKE UPDATE ON TABLE CW2.`Threads` FROM 'db_user'@'localhost';
REVOKE DROP ON TABLE CW2.`Threads` FROM 'db_user'@'localhost';

REVOKE DELETE ON TABLE CW2.`Threads` FROM 'db_user'@'localhost';

GRANT SELECT ON CW2.`Users` TO 'db_user'@'localhost';
GRANT INSERT ON CW2.`Users` TO 'db_user'@'localhost';

REVOKE UPDATE ON TABLE CW2.`Users` FROM 'db_user'@'localhost';
REVOKE DROP ON TABLE CW2.`Users` FROM 'db_user'@'localhost';

REVOKE DELETE ON TABLE CW2.`Users` FROM 'db_user'@'localhost';

GRANT SELECT ON CW2.`posts` TO 'db_user'@'localhost';
GRANT INSERT ON CW2.`posts` TO 'db_user'@'localhost';

REVOKE UPDATE ON TABLE CW2.`posts` FROM 'db_user'@'localhost';
REVOKE DROP ON TABLE CW2.`posts` FROM 'db_user'@'localhost';

SHOW GRANTS FOR 'db_user'@'localhost';
 

CREATE TABLE `Users`
(
 `id`        BIGINT AUTO_INCREMENT , #id user 
 `email`     varchar(45)  unique NOT NULL , 
 `username`  varchar(45)  UNIQUE NOT NULL ,
 `password`  varchar(256) NOT NULL ,
 `timestamp` timestamp NOT NULL DEFAULT current_timestamp() , 
 `isadmin`   boolean NOT NULL DEFAULT 0, # 0 user ,1  is admin
 `secret_key` VARCHAR(36) NOT NULL DEFAULT UUID() UNIQUE, #secret key used to create the otp of each user

PRIMARY KEY (`id`)
);

CREATE TABLE `Threads`
(
 `id_thread`   BIGINT AUTO_INCREMENT NOT NULL , 
 `thread_name` varchar(45) UNIQUE NOT NULL , 
 `timestamp`   timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP()   , #creation of the thread

PRIMARY KEY (`id_thread`)
);

CREATE TABLE `posts`
(
 `id_post`   BIGINT AUTO_INCREMENT NOT NULL ,
 `id_thread` BIGINT NOT NULL ,
 `id`        BIGINT NOT NULL , #id user
 `username`  varchar(45) NOT NULL , #user username
 `comment`   varchar(100) NOT NULL ,
 `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP(), 

PRIMARY KEY (`id_post`),
KEY `FK_2` (`id_thread`),
CONSTRAINT `FK_1` FOREIGN KEY `FK_2` (`id_thread`) REFERENCES `Threads` (`id_thread`)
On  delete CASCADE,  
KEY `FK_3` (`id`),
CONSTRAINT `FK_2` FOREIGN KEY `FK_3` (`id`) REFERENCES `Users` (`id`)
ON DELETE CASCADE;
);

CREATE TABLE `Session` (
 `ID-session` INT AUTO_INCREMENT NOT NULL, #id session
 `id` BIGINT NOT NULL,  # id user 
 `timestamp` timestamp NOT NULL DEFAULT DATE_ADD(CURRENT_TIMESTAMP(), INTERVAL 5 MINUTE), # date session expired current timestamp + 5 minutes
 `IP` varchar(32) NOT NULL,DEFAULT current_timestamp()

 # ip address user session
 `otp` TINYINT NOT NULL DEFAULT 0, # 0 user didn't pass the otp authorization, 1 user is authorize

PRIMARY KEY (`ID-session`),
KEY `FK_2` (`id`),
CONSTRAINT `FK_3` FOREIGN KEY `FK_2` (`id`) REFERENCES `Users` (`id`)
);











SELECT CURRENT_TIMESTAMP();