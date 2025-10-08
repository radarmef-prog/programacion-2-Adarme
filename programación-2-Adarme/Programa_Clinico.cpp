#include <iostream>
#include <stdlib.h>

using namespace std;  

main (){

int numero, *dir_numero;

cout<<"Digite un numero: ";
cin>>numero;

dir_numero=&numero;

if(*dir_numero%2==0){
    cout<< "El numero es par"<<*dir_numero<<endl;
    cout<< "La direccion del numero par es: "<<dir_numero<<endl;
}else{
    cout << "El numero es impar "<<*dir_numero<<endl;
    cout << "la dirección del numero impar es: "<<dir_numero<<endl;
}

}
