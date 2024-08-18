#ifndef User_h
#define User_h

#include <Arduino.h>

class User {
 public:
  String username;

 public:
  User(String username) : username(username) {
  }
};

#endif