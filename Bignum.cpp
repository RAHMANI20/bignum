/*
  -> Ayoub EL BAKKACH
  -> Faical Sid Ahmed RAHMANI
*/
#include "Bignum.hpp"
#include <cstdint>
#include <string>
#include <iostream>
#include <stdint.h>
#include <math.h>
#include <time.h>

using namespace std;

Bignum::Bignum(int x) : top (1), size (1) {
  tab = new unsigned [1];
  if (x >= 0) {
    tab[0] = x;
    sign = 0;
  }
  else {
    tab[0] = -x;
    sign = 1;
  }
}

Bignum::Bignum(unsigned x) : top (1), size (1), sign (0) {
  tab = new unsigned [1];
  tab[0] = x;
}

Bignum::Bignum(Bignum const & x) : top (x.top), size (x.size), sign (x.sign) {
  tab = new unsigned [size];
  for (unsigned i = 0; i < top; ++i) tab[i] = x.tab[i];
}

Bignum::Bignum(Bignum && x) : tab (x.tab), top (x.top), size (x.size), sign (x.sign) {
  x.tab = nullptr;
}

Bignum::Bignum(unsigned *xtab, unsigned xtop, unsigned xsize, unsigned xsign) : tab (xtab), top (xtop), size (xsize), sign (xsign) {}

Bignum::~Bignum() {
  delete [] tab;
}


Bignum & Bignum::operator=(Bignum const & x) {
  if (x.top > size) {
    delete [] tab;
    tab = new unsigned [x.size];
    size = x.size;
  }
  top = x.top;
  sign = x.sign;
  for (unsigned i = 0; i < top; ++i) tab[i] = x.tab[i];
  return *this;
}


Bignum & Bignum::operator=(Bignum && x) {
  delete [] tab;
  tab = x.tab;
  x.tab = nullptr;
  top = x.top;
  size = x.size;
  sign = x.sign;
  return *this;
}

/* surcharger l'operateur + pour faire l'addition */
Bignum Bignum::operator+(Bignum const & x) {
  if (sign != x.sign) {
    return addDifferentSign(*this, x);
  }
  return addSameSign(*this, x);
}

/* surcharger l'operateur += */
Bignum Bignum::operator+=(Bignum const & x) {
  *this = *this + x;
  return *this;
}

/* surcharger l'operateur - pour la soustaction */
Bignum Bignum::operator-(Bignum const & x) {
  return substraction(*this,x);
}

/* surcharger * pour la multiplication */ 
Bignum Bignum::operator*(Bignum const & x) {
  Bignum aux = Bignum(0);
  for (unsigned i = 0; i < x.top; ++i) {
    aux += multiplication(*this, x.tab[i], i);
  }
  aux.sign = sign ^ x.sign;
  return aux;
}

/* surcharger % pour effectuer le modulo */
Bignum Bignum::operator%(Bignum const & x) {
  return mod(*this,x);
}

/* surcharger / pour récuperer la quotient */
Bignum Bignum::operator/(Bignum const & x) {
  Bignum s = quotient(*this,x);
  return s;
}

/* surcharger -= */
Bignum Bignum::operator-=(Bignum const & x) {
  *this = *this - x;
  return *this;
}

/* surcharger == pour comparer deux bignum */
bool Bignum::operator==(Bignum const & x) {
  if (sign != x.sign)
    return false;
  for (int i = 0; i < x.top; i++)
    if (tab[i] != x.tab[i]) {
      return false;
    }
  return true;
}

/* surcharger != pour comparer deux bignum */
bool operator!=(Bignum const & x, Bignum const & y) {
  bool diff = false;
  if (x.sign != y.sign || x.top != y.top)
    return true;
  for (int i = 0; i < x.top; i++)
    if (x.tab[i] != y.tab[i])
      diff = true;
  return diff;
}

/* surcharger < pour verifier si le premier bignum est inferieur au deuxieme */
bool operator<(Bignum const & x, Bignum const & y) {
  int index = y.top-1;
  
  if ( x.top < y.top)
    return true;
    
  if (x.top > y.top)
    return false;
  
  if (x.top == 1 && y.top == 1 && x.tab[0] == y.tab[0])
    return false;
    
  while (x.tab[index] == y.tab[index]) {
    index--;
  }
  if (x.tab[index] < y.tab[index])
    return true;
    
  return false;
}

/* surcharger > pour verifier si le premier bignum est superieur au deuxième */
bool operator>(Bignum x, Bignum y) {
  int index = y.top-1;
  
  if ( x.top > y.top)
    return true;
  else if (x.top < y.top)
    return false;
  
  while (x.tab[index] == y.tab[index]) {
    index--;
  }
  if (x.tab[index] > y.tab[index])
    return true;
    
  return false;
}

/* décaler les bignum en n * 32 bit à droite */
Bignum Bignum::operator>>(unsigned n) {
  if (top < n)  // si la taille du décalage est plus grande que la taille du nombre de case du Bignum 
    return Bignum(0);
  
  unsigned *tab_aux = new unsigned [size];
  for (int i = n; i < top ; i++){
    tab_aux[i - n] = tab[i];
  }
  
  return Bignum(tab_aux, top - n, size, sign);
}

/* décaler les bignum en n * 32 bit à gauche */
Bignum Bignum::operator<<(unsigned n) {
  unsigned size_aux = size + n;
  unsigned *tab_aux = new unsigned [size_aux];
  
  for (int i = top; i>0 ;--i){
    tab_aux[i+n-1] = tab[i-1];
  }
  
  for (int i = n; i>0 ;--i){
    tab_aux[i-1] = 0;
  }
    
  return Bignum(tab_aux, top + n, size_aux, sign);
}

/* calculer la somme entre deux Bignum avec le meme signe, le Bignum x du premier paramètre 
   de la fonction doit toujours estre plus grand que le deuxieme parametre y. */
Bignum addSameSign(Bignum const & x, Bignum const & y) {
  if (x.top < y.top) return addSameSign(y,x);
  
  unsigned size = (x.top == x.size) ? 2*x.size : x.size; // si le tableau du plus grand nombre est plain alors doublé la taille du nouveau tableau
  unsigned * tab = new unsigned [size];
  uint64_t carry = 0;
  
  for (unsigned i = 0; i < y.top; ++i) {
    uint64_t tmp = carry + x.tab[i] + y.tab[i];
    carry = tmp >> 32;
    tab[i] = tmp;
  }
  
  for (unsigned i = y.top; i < x.top; ++i) {
    uint64_t tmp = carry + x.tab[i];
    carry = tmp >> 32;
    tab[i] = tmp;
  }
  tab[x.top] = carry;
  return Bignum(tab, x.top + carry, size, x.sign);
}

/* cette fonction calcule la valeur absolue entre x et y, 
   puis une autre fonction soustraction qui fixe le signe */
Bignum addDifferentSign(Bignum const & x, Bignum const & y) {
  if (y > x) return addDifferentSign(y,x); // le premier parametre toujours plus grand que le deuxieme
  unsigned size = x.size;
  unsigned * tab = new unsigned [size];
  uint64_t carry = 0;
  unsigned top = 1 ;
  
  /* initialisation du tableau de la résultat pour qu'il nous aide aprés un determiner le top */
  for (unsigned i = 0; i < size; ++i) {
    tab[i] = 0;
  }
  
  for (unsigned i = 0; i < y.top; ++i) {
    uint64_t tmp = (uint64_t)0 + x.tab[i] - y.tab[i] - carry; 
    carry = tmp >> 63;
    tab[i] = tmp;
  }
  
  for (unsigned i = y.top; i < x.top; ++i) {
    uint64_t tmp = (uint64_t)0 + x.tab[i] - carry;
    carry = tmp >> 63;
    tab[i] = tmp;
  }
  
  /* determiner le top du tableau du résultat calculer */
  for (unsigned i = size-1; i > 0; i--) {
    if ( tab[i] != 0) {
      top = i+1;
      break;
    }
  }
  return Bignum(tab, top, size, x.sign);
}

/* on peut appliquer la soustraction à l'aide deux operation la somme de deux nombre de meme 
   signe ou bien la somme de deux nombre avec un signe different, pour cela on distingue 4 cas :
   1)- x negative et y positif  ==(x - y)==>> ajouter x a y (addSameSign) le signe toujour negative
   2)- x positif  et y negative ==(x - y)==>> soutraire x a y (addDifferentSign) le signe toujour positif
   3)- x positif  et y positif  ==(x - y)==>> soutraire x a y (addDifferentSign) il faut faire une comparaison entre x et y pour le sign
   4)- x negative et y negative ==(x - y)==>> soutraire y a x (addDifferentSign) il faut faire une comparaison entre x et y pour le sign
*/ 
Bignum substraction(Bignum const & x, Bignum const & y) {
  if (x.sign > y.sign) {
    Bignum tmp = addSameSign(x, y);
    tmp.sign = 1;
    return tmp;
  }
 
  if (x.sign < y.sign) {
    Bignum tmp = addSameSign(x, y);
    tmp.sign = 0;
    return tmp;
  }
  
  if (x.sign * y.sign == 0) {
    Bignum tmp = addDifferentSign(x, y);
    if ( x < y)
      tmp.sign = 1;
    else
      tmp.sign = 0;
    return tmp;
  }
  
  if (x.sign * y.sign == 1) {
    Bignum tmp = addDifferentSign(x, y);
    if ( x < y)
      tmp.sign = 0;
    else
      tmp.sign = 1;
    return tmp;
  }

  
}

/* multiplier chaque case du tableau de Bignum x avec un entier 
   y est stocker dans carry le retenu quand on depase 32 bit  */
Bignum multiplication(Bignum const & x, uint32_t y, unsigned dec) { 
  unsigned size = (x.top == x.size) ? 2*x.size + dec : x.size + dec;
  unsigned * tab = new unsigned [size];
  uint64_t carry = 0;
  
  if( y == 0)   // si on multiplie un nombre par 0;
    return Bignum(0);
  
  for (unsigned i = 0; i < dec; ++i) {
    tab[i] = 0;
  }
  for (unsigned i = 0; i < x.top; ++i) {
    uint64_t aux = x.tab[i];
    uint64_t tmp = carry + aux * y; 
    carry = tmp >> 32;
    tab[i+dec] = tmp;
  }
  tab[x.top+dec] = carry;
  
  
  return Bignum(tab, (carry > 0) ? x.top+1+dec : x.top+dec, size, x.sign);
}


std::tuple<Bignum,Bignum> division(Bignum x, Bignum y) {
  unsigned top = x.top; 
  unsigned * q = new unsigned [x.top-y.top+1];
  Bignum tmp;
  if (y > x )
    return {Bignum(0), x};
  if (x.top == 1 && x.tab[0] == 0)
    return {Bignum(0), Bignum(0)};
  if ( x.top == 1 && y.top == 1 )
    return {Bignum(x.tab[0]/y.tab[0]), Bignum(x.tab[0]%y.tab[0])}; 
  
  if ( y.top == 1)
    return division(x, y.tab[0]);
    
  for (int i = 0; i <= x.top-y.top; i++)
    q[i] = 0;
  
  tmp = (y << x.top-y.top);
  
  while(x > tmp || x==tmp ){
    q[x.top - y.top ] += 1;
    x -= tmp;
  }
  
  
    for (unsigned i = x.top; i > y.top; --i) {
    if (x.tab[i-1] == y.tab[y.top-1])
      q[i-y.top-1] = (unsigned)-1;
    else {
      
      q[i-y.top-1] = ( ((uint64_t)x.tab[i-1] << 32 ) + x.tab[i-2]) / y.tab[y.top-1];
    }
    
    if ( x.top >= 3 && y.top >= 2) {
    
    Bignum y_tmp = Bignum(0);
    Bignum x_tmp = Bignum(0);
    
     q[i-y.top-1] = q[i-y.top-1] + 1;
    
    do {
    
     q[i-y.top-1] = q[i-y.top-1] - 1;
     
     y_tmp = Bignum(y.tab[y.top-1]);
     y_tmp = y_tmp << 1;
     y_tmp += y.tab[y.top-2];
     y_tmp = y_tmp * q[i-y.top-1];
    
     x_tmp = Bignum(x.tab[i-1]); 
     x_tmp = x_tmp << 1;
     x_tmp += x.tab[i-2];
     x_tmp = x_tmp << 1;
     x_tmp += x.tab[i-3];
    }
    while ( y_tmp > x_tmp );
    
    
    }
    x -= (y << (i - y.top - 1))* q[i-y.top-1];
    
    if (x.sign == 1) {
      x += (y << (i - y.top - 1));
      q[i-y.top-1] -=  1;
    }
    
  }
  
  for (int i = x.top-y.top; i>=0; i--) {
     if ( q[i] != 0) {
     top = i+1;
     break;
     }
  }
  return {Bignum(q, top , x.size, x.sign), x};// top

}

/* la division entre un Bignum est un unsigned pour diminuer le temps de calcul */
std::tuple<Bignum,Bignum> division(Bignum x, unsigned n) {
  unsigned * q = new unsigned [x.size];
  unsigned * r = new unsigned [1];
  Bignum tmp_x = x;
  uint64_t carry = 0;
  uint64_t tmp = 0;
  
  for (int i = x.top-1 ; i >= 0; i--) {
    tmp = (carry << 32) + tmp_x.tab[i];
    q[i] = tmp / n;
    carry = tmp - n * q[i];
  }
  r[0] = carry;
  
  return {Bignum(q, (q[x.top-1]==0) ? x.top-1 : x.top , x.size, x.sign), Bignum(r, 1, 1, 0)};  //quotient top faux
}

/* pour écrire en base 10, on appliqe une division du bignum sur 10 est on sauvegarde le reste */
void print_base_10(Bignum x) {
    Bignum tmp = x;
    std::string output ("");
    while (tmp.top >= 1 && tmp.tab[0]!= 0) {
      auto [q,r] = division(tmp, 10);
      tmp = q;
      output.insert (0,std::to_string(r.tab[0]));
    }
    cout << output << endl;
}

/* verifie si un nombre est pair */
bool even(Bignum x){
  auto [q,r] = division(x,2);
  return (r.tab[0] == 0);
}

/* l'algorithme GCD binaire est plus efficace que l'lagorithme euclidienne 
   car il réduit la taile des chiffres plus rapidement que euclide */
Bignum gcd(Bignum x, Bignum y) { 
  Bignum g = Bignum(1);
  
  while (even(x) && even(x)) {
    x = x / 2;
    y = y / 2;
    g = g * 2;
  }
  
  while (!(x == 0)) {
    while (even(x)) {
      x = x / 2;
    }

    while (even(y) ) {
      y = y / 2;
    }
    
    Bignum t = (x - y) / 2;
    t.sign = 0;
    
    if (x > y || x == y)
      x = t;
    else
      y = t; 
  }
  
  return g*y;
}

/* calculer le modulo d'un bignum */
Bignum mod(Bignum x, Bignum n) {
if (x < n)
    return x;
  if ( x == n)
    return Bignum(0);
  auto [q,r] = division(x, n);
  return r;
}

/* calculer quotient d'un bignum */
Bignum quotient(Bignum x, Bignum n) {
  auto [q,r] = division(x, n);
  return q;
}

/* verifie si un nombre est impaire */
bool odd(Bignum x){
  auto [q,r] = division(x, Bignum(2));
  return (r.tab[0] != 0);
}

/* générer un bignum avec une taille size * 32 */
Bignum generate(Bignum a, int size) {
  
  unsigned * tab = new unsigned [size*2];
  for (int i = 0; i < size; i++)
    tab[i] = rand() ;
  
  return Bignum(tab, size, size, 0);
}


/* effectuer le petit theoreme de fermat pour verifier si un bignum 
   est premier : pour un bignum a < p -1 alors a^p = a[P] */
bool primality_test(Bignum b, Bignum p) {  
  Bignum e = to_binary(p);
  Bignum result = square_and_multiply(b, e, p);
  return (result == b);  
}

/* Convertir un bignum en binaire avec la division sur 2, cette étape 
   va etre utilise pour l'algorithme square and multiply */
Bignum to_binary(Bignum x) {
  unsigned size = x.top * 32; // 32 la taille max de chaque case du tab
  unsigned * tab = new unsigned [size];
  unsigned index = 0;
  Bignum x_tmp = x;
  while (x_tmp.top > 1 || (x_tmp.top == 1 && x_tmp.tab[0] > 0)) {
    auto [q,r] = division(x_tmp,2);
    tab[index] = (r == 0) ? 0 : 1;
    x_tmp = q;
    index++;
  }
  return Bignum(tab, index, size, 0);
}

/* appliquation de l'algorithme square and multiply pour calculer b^e mod m */
Bignum square_and_multiply(Bignum b, Bignum e, Bignum m) { // base, exponential, mod
  Bignum r = b;
  unsigned index = e.top - 1;
  while (index > 0) {
    r = (r*r) % m;
    if (e.tab[--index] == 1) {
      r = (r*b) % m;  
    }
  }
  return r;
}

/* Pour générer les nombre premiers, on génère aleatoirement des nombres, puis à l'aide 
   de l'algo square and multiply on verifie avec fermat si le nombre est premier */ 
Bignum generate_prime(Bignum a, int size){
  Bignum x(0);
  Bignum d(0);
  Bignum e(0);
  Bignum result(0);
  
  x = Bignum(9);
  d = generate(a,size); 

  e = to_binary(d);
  result = square_and_multiply(x, e, d);
  if (result == x)
    return d;
  else
    return generate_prime(Bignum(0),size);
  
  return d; 
}

/* utilisation de l'algorithme euclide étendu pour trouver les coefficient 
   de bézout afin de calculer l'inverse modulaire */
std::tuple<Bignum, Bignum, Bignum> extended_euclidean(Bignum a, Bignum b) {

Bignum a1, b1, lasta, lastb, temp, temp1, temp2, temp3;

    if (b>a) {
        temp=a; a=b; b=temp;
    }
    a1 = Bignum(0);
    b1 = Bignum(1);
    lasta=Bignum(1);
    lastb=Bignum(0);
    while (b!=Bignum(0)) {
        auto [q,r] = division(a, b);
        temp1= r;
        a=b;
        b=temp1;
        temp2 = a1;
        
        a1=lasta-q*a1;
        Bignum m = a1*q;
        lasta = temp2;
        
        temp3 = b1;
        b1 = lastb-q*b1;
        lastb=temp3;
        
    }
    
    return make_tuple(a, lasta, lastb);
}

/* chaque case du tableau des element d'un bignum peut prendre 32 bit et comme la representation 
   ascci est sur 8 bit, on peut a chaque case du tableau stocker 4 representation d'ascii, 
   pour cela a chaque fois on doit décaler notre bignum par 8 bit pour insérer un nouveau caractère */
Bignum ascci_to_Bignum(string text, Bignum a) {
  Bignum num = Bignum(0);
  for (int i = 0; i < text.size(); i++) {
    if (i % 4 == 0 && i != 0) { // a chaque fois quand %4 == 0 on décale le bignum par 32 bit (une case de tableau)
      num = num << 1;
      num += int(text[i]);
    }
    else {
      unsigned tmp = (num.tab[0] << 8) + int(text[i]);
      num.tab[0] = 0;
      num += tmp;
      
    }
  }
  return num;
}

/* pour convertir un bignum en ascii on va inverser les étape de la fonction ascci_to_Bignum */
void Bignum_to_ascci(Bignum num) {
  std::string output ("");
  unsigned top = num.top;
  
  for (int i = 0; i < top; i++) {
    for (int j = 0; j < 4; j++) {
      unsigned tmp = num.tab[0] << ((3 - j)*8);
      tmp = tmp >> 3*8;
      output.insert (0,1,char(tmp));
    }
    num = num >> 1;
  }
  cout << output << endl;
}

/* calculer l'inverse modulaire a l'aide de inverse_modular */
Bignum inverse_modular(Bignum a, Bignum b) {
  auto [gcd,x,y] = extended_euclidean(a, b);
  if (y.sign == 1)
    y = y + b;
  return y;
}

/* 
   pour chiffré et dichifré en rsa on applique les étape suivant :
   - génerer deux nombre premier de taille 512 bit p et q 
   - calculer n = p*q
   - initialiser e avec  65537
   - calculer la totient t
   - calculer le clés privé avec l'inverse modulaire
   - chifre avec cl"s publique
   - déchiffre avec le clé privée
*/
void RSA(Bignum text, int size) {
  Bignum p, q, n, e, t, d, ebin, c, dbin, m;
  
  cout << "[*............]" << "  génération d'un nombre premier p en cours" << endl << endl;
  p =  generate_prime(Bignum(0), size);
  cout << "[**...........]  p = "; print_base_10(p); cout << endl ;
  
  cout << "[***..........]" <<  "  génération d'un nombre premier q en cours" << endl << endl;
  q =  generate_prime(Bignum(0), size);
  cout << "[****.........]  q = "; print_base_10(q); cout << endl;
  
  n = p*q;
  cout << "[*****........]" << "  N = ";  print_base_10(n); cout << endl;
  
  e = Bignum(65537);
  cout << "[******.......]" << "  e = ";  print_base_10(e); cout << endl;
  
  t = (p-1)*(q-1);
  cout << "[*******......]" << "  t = ";  print_base_10(t); cout << endl;
  
  d = inverse_modular(e,t);
  cout << "[********.....]" << "  d = ";  print_base_10(d); cout << endl;
  
  cout << "[*********....]" << "  Chiffrement du message " << endl << endl;
  ebin = to_binary(e);
  c = square_and_multiply(text, ebin, n);
  cout << "[**********...]" << "  c = ";  print_base_10(c); cout << endl;
  
  
  cout << "[***********..]" << "  déchiffrement du message " << endl << endl;
  dbin = to_binary(d);
  m = square_and_multiply(c, dbin, n);
  cout << "[************.]" << "  m = ";  print_base_10(m); cout << endl;
  
  cout << "[*************]" << endl;
  cout << "le message en ascii est : "; Bignum_to_ascci(m);
}
