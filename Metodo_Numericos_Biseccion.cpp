#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

// Definimos la función f(x) = ln(x+1) + x - 2
double f(double x) {
    return log(x + 1) + x - 2;
}

int main() {
    double a = 0.0;         // Límite inferior
    double b = 2.0;         // Límite superior
    double tol = 1e-8;      // Tolerancia
    int iter = 0;
    double c;

    // Verificamos que f(a) y f(b) tengan signos opuestos
    if (f(a) * f(b) >= 0) {
        cout << "No se puede aplicar el método: f(a) y f(b) no tienen signos opuestos." << endl;
        return 1;
    }

    cout << fixed << setprecision(10);
    

    cout << "Iter\t       a\t\t       b\t\t     f(a)\t\t     f(b)\t\t       c\t\t     f(c)\t\t Criterio" << endl;
    while (true) {
        
        c = (a + b) / 2.0;

        // Evaluamos criterios de parada
        bool criterio_intervalo = ((b - a) / 2.0 < tol);
        bool criterio_funcion   = (fabs(f(c)) < tol);

        cout << setw(4) << iter + 1 << "\t"
             << setw(10) << a << "\t"
             << setw(10) << b << "\t"
             << setw(10) << f(a) << "\t"
             << setw(10) << f(b) << "\t"
             << setw(10) << c << "\t"
             << setw(10) << f(c) << "\t";

        if (criterio_intervalo || criterio_funcion) {
            cout << "Se cumple criterio" << endl;
            break;
        } else {
            cout << "No cumple" << endl;
        }

        // Actualizamos intervalo
        if (f(a) * f(c) < 0) {
            b = c;
        } else {
            a = c;
        }

        iter++;
    }

    cout << "\nRaiz aproximada encontrada: x = " << c 
         << " con f(x) = " << f(c) << endl;
    cout << "Iteraciones realizadas: " << iter + 1 << endl;

    return 0;
}