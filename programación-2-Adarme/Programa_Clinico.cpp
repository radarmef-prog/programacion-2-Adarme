#include <iostream>
#include <conio.h>
#include <stdlib.h>

using namespace std;  

main (){
    
int *Calf, NumCalif;

cout<<"Ingrese un numero de calificaciones: ";
cin>>NumCalif;

Calf= new int [NumCalif];

for (int i=0; i<NumCalif; i++) {
    cout <<"Ingrese la Calificacion #" << (i+1) << "= ";
    cin >> Calf[i];
}

cout<<"\n\nLAS CALIFICACIONES SON: \n"<<endl;
cout<<"=========================\n"<<endl;

for (int i=0; i<NumCalif; i++) {
    cout<<"Calificacion #"<< (i+1) <<"= "<<Calf[i]<<"\n"<<endl;
}

delete [] Calf; 

getch();
return 0;

}