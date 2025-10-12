 /*Programa Clinico de control 
Desarrollado por Rubén Adarme 32.453.470*/
#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

using namespace std;

//========================================================================================================================================================================
//================================================================== ESTRUCTURAS =========================================================================================      
//========================================================================================================================================================================

struct HistorialMedico {
    int idConsulta;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
};

struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];

    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    char alergias[500];
    char observaciones[500];

    bool activo;
};

struct Hospital {
    char nombre[100];
    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;
    int siguienteIdPaciente;
};

//========================================================================================================================================================================
//================================================== FUNCION PARA INICIAR HOSPITALÑ ======================================================================================  
//========================================================================================================================================================================

Hospital* inicializarHospital(const char* nombre, int capacidadInicial) {
    Hospital* h = new Hospital;
    strncpy(h->nombre, nombre, 99);
    h->pacientes = new Paciente[capacidadInicial];
    h->cantidadPacientes = 0;
    h->capacidadPacientes = capacidadInicial;
    h->siguienteIdPaciente = 1;
    return h;
}

//========================================================================================================================================================================
//================================================ FUNCION PARA REDIMENSIONAR ARRAY ======================================================================================
//========================================================================================================================================================================

void redimensionarArrayPacientes(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadPacientes * 2;
    Paciente* nuevoArray = new Paciente[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        nuevoArray[i] = hospital->pacientes[i];
    }

    delete[] hospital->pacientes;
    hospital->pacientes = nuevoArray;
    hospital->capacidadPacientes = nuevaCapacidad;

    cout << "🔄 Capacidad de pacientes duplicada a " << nuevaCapacidad << endl;
}

//========================================================================================================================================================================
//================================================= FUNCION PARA VERIFICAR EL INGRESO DE CEDULAS REPETIDAS ===============================================================  
//========================================================================================================================================================================

bool cedulaYaRegistrada(Hospital* hospital, const char* cedula) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcmp(hospital->pacientes[i].cedula, cedula) == 0) {
            return true; // Cédula duplicada
        }
    }
    return false; // Cédula no encontrada
}

//========================================================================================================================================================================
//=================================================== FUNCION PARA INGRESAR NUEVO PACIENTE ===============================================================================  
//========================================================================================================================================================================

void INGRESO_NUEVOS_PACIENTES(Hospital* hospital) {
    char wAgregarMas;

    do {
        if (hospital->cantidadPacientes >= hospital->capacidadPacientes) {
            redimensionarArrayPacientes(hospital);
        }

        Paciente& p = hospital->pacientes[hospital->cantidadPacientes];
        p.id = hospital->siguienteIdPaciente++;

        cout<<"\t\t╔════════════════════════════════════════╗"<<endl;
        cout<<"\t\t║      INGRESO DE NUEVOS PACIENTES       ║"<<endl;
        cout<<"\t\t╚════════════════════════════════════════╝"<<endl;

        cout << "Nombre: ";
        cin.ignore();
        cin.getline(p.nombre, 50);

        cout << "Apellido: ";
        cin.getline(p.apellido, 50);

        do {
        cout << "Cedula: ";
        cin.getline(p.cedula, 20);

        if (cedulaYaRegistrada(hospital, p.cedula)) {
        cout << "!ERROR¡: Esta cedula ya esta registrada. Intente con otra.\n";
        }
        } while (cedulaYaRegistrada(hospital, p.cedula));

        cout << "Edad: ";
        cin >> p.edad;

        do{
        cout << "Sexo (M/F): ";
        cin >> p.sexo;
        if (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f') {
            cout << "¡ERROR!: Sexo inválido. Debe ser M o F.\n";
            }
        }while(p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f');

        cout << "Tipo de sangre (ej: O+, A-): ";
        cin.ignore();
        cin.getline(p.tipoSangre, 5);

        cout << "Telefono: ";
        cin.getline(p.telefono, 15);

        cout << "Direccion: ";
        cin.getline(p.direccion, 100);

        cout << "Email: ";
        cin.getline(p.email, 50);

        cout << "Alergias (si no tiene, escriba 'Ninguna'): ";
        cin.getline(p.alergias, 500);

        cout << "Observaciones generales (si no tiene, escriba 'Ninguna'): ";
        cin.getline(p.observaciones, 500);

        // Inicializar arrays dinámicos
        p.capacidadHistorial = 5;
        p.cantidadConsultas = 0;
        p.historial = new HistorialMedico[p.capacidadHistorial];

        p.capacidadCitas = 5;
        p.cantidadCitas = 0;
        p.citasAgendadas = new int[p.capacidadCitas];

        p.activo = true;

        hospital->cantidadPacientes++;

        cout << "\n√ Paciente registrado con exito. ID asignado: 00000" << p.id << endl;

        cout << "\n¿Desea agregar otro paciente? (S/N): ";
        cin >> wAgregarMas;

    } while (wAgregarMas == 'S' || wAgregarMas == 's');
}

//========================================================================================================================================================================
//============================================ FUNCION PARA DESTRUIR HOSPITAL Y LIBERAR ESPACIO ========================================================================== 
//========================================================================================================================================================================

void destruirHospital(Hospital* hospital) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        delete[] hospital->pacientes[i].historial;
        delete[] hospital->pacientes[i].citasAgendadas;
    }
    delete[] hospital->pacientes;
    delete hospital;
}

//========================================================================================================================================================================
//========================================================== PROGRAMA PRINCIPAL ==========================================================================================       
//========================================================================================================================================================================

int main() {
    int wOpcionIMP;
    Hospital* hospital = inicializarHospital("Hospital Central", 8);
    

    cout<<"\t\t╔════════════════════════════════════════╗"<<endl;
    cout<<"\t\t║            HOSPITAL CENTRAL            ║"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t║  RIF: J-12345678-9                     ║"<<endl;
    cout<<"\t\t║  Telefono: (0261) 123-4567             ║"<<endl;
    cout<<"\t\t║  Email: ClinicaOdant@gmail.com         ║"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t║  Atencion: Lunes a Viernes             ║"<<endl;
    cout<<"\t\t║  8:00 am a 12:00 m./1:00 pm a 5:00 pm  ║"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t║  Direccion: Calle 123, Urb. La Salud   ║"<<endl;
    cout<<"\t\t║  Ciudad: Maracaibo, Edo. Zulia         ║"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t║          SELECCIONE UNA OPCION         ║"<<endl;
    cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
    cout<<"\t\t║  1. Gestion de Pacientes               ║"<<endl;
    cout<<"\t\t║  2. Gestion de Doctores                ║"<<endl;
    cout<<"\t\t║  3. Gestion de Citas                   ║"<<endl;
    cout<<"\t\t║  4. Salir                              ║"<<endl;
    cout<<"\t\t╚════════════════════════════════════════╝"<<endl;

    cin>>wOpcionIMP;
  
    do{
    switch(wOpcionIMP){
        case 1:
            INGRESO_NUEVOS_PACIENTES(hospital);
            break;
        case 2:
            cout << "Funcionalidad de gestion de doctores en desarrollo." << endl;
            break;
        case 3:
            cout << "Funcionalidad de gestion de citas en desarrollo." << endl;
            break;
        case 4:
            cout << "Saliendo del programa. ¡Hasta luego!" << endl;
            break;
        default:
            cout << "¡ERROR!: Opcion invalida (1-4)" << endl;
            system("pause");
            break;
            return 1;
    }
    }while(wOpcionIMP != 4);

    destruirHospital(hospital);
    return 0;
}
