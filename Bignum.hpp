#ifndef BIGNUM_HPP
#define BIGNUM_HPP


#include <stdint.h>
#include <map>
#include <string>

using namespace std;
/*
  -> Ayoub EL BAKKACH
  -> Faical Sid Ahmed RAHMANI
*/
class Bignum {
public:
  Bignum(int = 0);
  Bignum(unsigned);
  Bignum(Bignum const &);
  Bignum(Bignum &&);

  ~Bignum();

  Bignum & operator=(Bignum const &);
  Bignum & operator=(Bignum &&);
  Bignum operator+(Bignum const &);
  Bignum operator-(Bignum const &);
  Bignum operator*(Bignum const &);
  Bignum operator+=(Bignum const &);
  Bignum operator-=(Bignum const &);
  Bignum operator%(Bignum const &);
  
  Bignum operator/(Bignum const &);
  bool operator==(Bignum const &);
  Bignum operator<<(unsigned);
  Bignum operator>>(unsigned);
  
private:
  unsigned *tab;
  unsigned top;
  unsigned size;
  unsigned sign;

  Bignum(unsigned *tab, unsigned top, unsigned size, unsigned sign);



friend Bignum addSameSign(Bignum const &, Bignum const &);
friend Bignum addDifferentSign(Bignum const &, Bignum const &);
friend Bignum substraction(Bignum const & x, Bignum const & y);
friend Bignum multiplication(Bignum const & x, uint32_t y, unsigned dec);
friend std::tuple<Bignum,Bignum> division(Bignum x, Bignum y);
friend std::tuple<Bignum,Bignum> division(Bignum x, unsigned n);
friend void print_base_10(Bignum x);
friend Bignum gcd(Bignum x, Bignum y);
friend Bignum mod(Bignum x, Bignum n);
friend Bignum quotient(Bignum x, Bignum n);
friend bool odd(Bignum x);
friend Bignum generate(Bignum a, int size);
friend bool primality_test(Bignum x, Bignum n);
friend Bignum to_binary(Bignum x);
friend Bignum square_and_multiply(Bignum b, Bignum e, Bignum m);
friend Bignum generate_prime(Bignum a, int size);
friend bool operator<(Bignum const & x,Bignum const & y);
friend bool operator>(Bignum  x,Bignum  y);
friend bool operator!=(Bignum const & x, Bignum const & y);
friend bool even(Bignum x);
friend std::tuple<Bignum, Bignum, Bignum> extended_gcd(Bignum a, Bignum b);
friend std::tuple<Bignum, Bignum, Bignum> extended_euclidean(Bignum a, Bignum b);
friend Bignum inverse_modular(Bignum a, Bignum b);
friend Bignum ascci_to_Bignum(string text, Bignum a);
friend void Bignum_to_ascci(Bignum num);
friend void RSA(Bignum text, int size);


};
#endif
