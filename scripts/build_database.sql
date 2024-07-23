# CREATE DATABASE dete_res;

USE dete_res;

CREATE TABLE prebase_yolo5_0_5_0 (
  uc      CHAR(10) NOT NULL,
  tag     CHAR(10) NOT NULL,
  x1      DOUBLE NOT NULL,
  y1      DOUBLE NOT NULL,
  x2      DOUBLE NOT NULL,
  y2      DOUBLE NOT NULL,
  conf    DOUBLE NOT NULL
  );
  

# DROP TABLE prebase_yolo5_0_4_0;

# 远程登录  mysql -h 192.168.3.101 -u root -p , 回车之后输入密码即可登录上

# ucd dete_res;
# select count(*) from prebase_yolo5_0_4_0;







