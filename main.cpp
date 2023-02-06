/*
  -> Ayoub EL BAKKACH
  -> Faical Sid Ahmed RAHMANI
*/
#include "Bignum.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <err.h>
#include <stdint.h>
#include <iostream>


using namespace std;

int main(){
  std:: string input;
  srand(time(0));
  
  cout << "Bonjour dans le chiffrement RSA" << endl; 
  cout << "Pour commencer entrer le text que vous voulez chiffrer" << endl;
  getline(cin, input);
  Bignum s= ascci_to_Bignum(input, Bignum(0));

RSA(s, 8);


}
