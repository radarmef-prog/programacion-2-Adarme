 /*Programa Clinico de control 
Desarrollado por Rubén Adarme 32.453.470*/


#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <windows.h>

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

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];

    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool disponible;
};

struct Hospital {

    char nombre[100];
    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;
    int siguienteIdPaciente;

    Doctor* doctores;
    int cantidadDoctores;
    int capacidadDoctores;
    int siguienteIdDoctor;

};

//========================================================================================================================================================================
//================================================== FUNCION PARA INICIAR HOSPITAL =======================================================================================  
//========================================================================================================================================================================

Hospital* inicializarHospital(const char* nombre, int capacidadInicial) {
    Hospital* h = new Hospital;
    strncpy(h->nombre, nombre, 99);
    h->pacientes = new Paciente[capacidadInicial];
    h->cantidadPacientes = 0;
    h->capacidadPacientes = capacidadInicial;
    h->siguienteIdPaciente = 1;

    h->doctores = new Doctor[capacidadInicial];
    h->cantidadDoctores = 0;
    h->capacidadDoctores = capacidadInicial;
    h->siguienteIdDoctor = 1;
    return h;

}

//========================================================================================================================================================================
//================================================ FUNCION PARA REDIMENSIONAR ARRAY DE PACIENTES =========================================================================
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
}

//========================================================================================================================================================================
//================================================ FUNCION PARA REDIMENSIONAR ARRAY DE DOCTORES ==========================================================================
//========================================================================================================================================================================

void redimensionarArrayDoctores(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadDoctores * 2;
    Doctor* nuevoArray = new Doctor[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        nuevoArray[i] = hospital->doctores[i];
    }

    delete[] hospital->doctores;
    hospital->doctores = nuevoArray;
    hospital->capacidadDoctores = nuevaCapacidad;
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

bool cedulaDoctorYaRegistrada(Hospital* hospital, const char* cedula) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].cedula, cedula) == 0) {
            return true;
        }
    }
    return false;
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

        cout << "\n√ Paciente registrado con exito. ID asignado:" << p.id << endl;

        cout << "\n¿Desea agregar otro paciente? (S/N): ";
        cin >> wAgregarMas;

    } while (wAgregarMas == 'S' || wAgregarMas == 's');
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA INGRESAR NUEVO DOCTOR ===============================================================================  
//========================================================================================================================================================================

Doctor* INGRESO_NUEVO_DOCTOR(Hospital* hospital) {
    if (hospital->cantidadDoctores >= hospital->capacidadDoctores) {
        redimensionarArrayDoctores(hospital);
    }

    Doctor& d = hospital->doctores[hospital->cantidadDoctores];

    d.id = hospital->siguienteIdDoctor++;

    cout<<"\t\t╔════════════════════════════════════════╗"<<endl;
    cout<<"\t\t║       INGRESO DE NUEVOS DOCTORES       ║"<<endl;
    cout<<"\t\t╚════════════════════════════════════════╝"<<endl;

    cout << "Nombre: ";
    cin.ignore();
    cin.getline(d.nombre, 50);

    cout << "Apellido: ";
    cin.getline(d.apellido, 50);

    do {
        cout << "Cedula profesional: ";
        cin.getline(d.cedula, 20);
        if (cedulaDoctorYaRegistrada(hospital, d.cedula)) {
            cout << " Cédula ya registrada. Intente con otra.\n";
        }
    } while (cedulaDoctorYaRegistrada(hospital, d.cedula));

    cout << "Especialidad medica: ";
    cin.getline(d.especialidad, 50);

    cout << "Anios de experiencia: ";
    cin >> d.aniosExperiencia;

    cout << "Costo de consulta: ";
    cin >> d.costoConsulta;

    cout << "Horario de atención (ej: Lun-Vie 8:00-16:00): ";
    cin.ignore();
    cin.getline(d.horarioAtencion, 50);

    cout << "Teléfono: ";
    cin.getline(d.telefono, 15);

    cout << "Email: ";
    cin.getline(d.email, 50);

    // Inicializar arrays dinámicos
    d.capacidadPacientes = 5;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = 10;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    d.disponible = true;

    hospital->cantidadDoctores++;

    cout << "\n Doctor registrado con éxito. ID asignado: " << d.id << endl;

    return &d;
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
    

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
    delete[] hospital->doctores[i].pacientesAsignados;
    delete[] hospital->doctores[i].citasAgendadas;
    }

    delete[] hospital->doctores;
    delete hospital;
}

//========================================================================================================================================================================
//========================================================== PROGRAMA PRINCIPAL ==========================================================================================       
//========================================================================================================================================================================

int main() {
    int wOpcionIMP;
    int wOpcionMP;
    int wOpcionMAC;
    int wOpcionMD;
    Hospital* hospital = inicializarHospital("Hospital Central", 8);
    
    do{
    system("cls");
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
  
    system("cls");  
    switch(wOpcionIMP){

        
        case 1:
            
            do{
            system("cls");
            cout<<"\t\t╔════════════════════════════════════════╗"<<endl;
            cout<<"\t\t║           GESTION DE PACIENTES         ║"<<endl;
            cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
            cout<<"\t\t║          SELECCIONE UNA OPCION         ║"<<endl;
            cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
            cout<<"\t\t║  1. Ingresar nuevo paciente            ║"<<endl;
            cout<<"\t\t║  2. Buscar paciente por C.I            ║"<<endl;
            cout<<"\t\t║  3. Buscar paciente por nombre         ║"<<endl;
            cout<<"\t\t║  4. Ver historial medico completo      ║"<<endl;
            cout<<"\t\t║  5. Actualizar datos del paciente      ║"<<endl;
            cout<<"\t\t║  6. Ver lista de paciente clinica      ║"<<endl;       
            cout<<"\t\t║  7. Eliminar paciente                  ║"<<endl;
            cout<<"\t\t║  0. Volver a menu principal            ║"<<endl;
            cout<<"\t\t╚════════════════════════════════════════╝"<<endl;

                cin>>wOpcionMP;
                
                    system("cls");
                        switch(wOpcionMP){
                            case 1:
                                INGRESO_NUEVOS_PACIENTES(hospital);
                                system("pause");
                                break;
                            case 2:
                                cout << "Funcionalidad de busqueda por C.I en desarrollo." << endl;
                                system("pause");
                                break;
                            case 3:
                                cout << "Funcionalidad de busqueda por nombre en desarrollo." << endl;
                                system("pause");
                                break;
                            case 4:
                                cout << "Funcionalidad de ver historial medico completo en desarrollo." << endl;
                                system("pause");
                                break;
                            case 5:
                                cout << "Funcionalidad de actualizar datos del paciente en desarrollo." << endl;
                                system("pause");
                                break;
                            case 6:
                                cout << "Funcionalidad de ver lista de pacientes en desarrollo." << endl;
                                system("pause");
                                break;
                            case 7:
                                cout << "Funcionalidad de eliminar paciente en desarrollo." << endl;
                                system("pause");
                                break;
                            case 0:
                                cout << "Volviendo al menu principal..." << endl;
                                system("pause");
                                break;
                            default:
                                cout << "¡ERROR!: Opcion invalida (0-7)" << endl;
                                system("pause");
                                break;
                }
            }while (wOpcionMP != 0);
            break;

        case 2:
            do{
                 system("cls"); 
                    cout<<"\t\t╔═══════════════════════════════════════════════╗"<<endl;
                    cout<<"\t\t║              GESTION DE DOCTORES              ║"<<endl;
                    cout<<"\t\t╠═══════════════════════════════════════════════╣"<<endl; 
                    cout<<"\t\t║             SELECCIONE UNA OPCION             ║"<<endl;
                    cout<<"\t\t╠═══════════════════════════════════════════════╣"<<endl;
                    cout<<"\t\t║  1. Ingresar nuevo doctor                     ║"<<endl;
                    cout<<"\t\t║  2. Buscar doctor por ID                      ║"<<endl;
                    cout<<"\t\t║  3. Buscar doctor por especialidad            ║"<<endl;
                    cout<<"\t\t║  4. Asignar paciente a doctor                 ║"<<endl; 
                    cout<<"\t\t║  5. Remover paciente de doctor                ║"<<endl;
                    cout<<"\t\t║  6. Lista de pacientes asignados a doctor     ║"<<endl;       
                    cout<<"\t\t║  7. Ver lista de doctores clinica             ║"<<endl;
                    cout<<"\t\t║  8. Eliminar doctor                           ║"<<endl;
                    cout<<"\t\t║  0. Volver al menu principal                  ║"<<endl;
                    cout<<"\t\t╚═══════════════════════════════════════════════╝"<<endl;

                        cin>>wOpcionMD;
                
                             system("cls");
                                 switch(wOpcionMD) {
                                    case 1: 
                                        INGRESO_NUEVO_DOCTOR(hospital);
                                        system("pause");
                                        break;
                                    case 2:
                                        cout << "Funcionalidad de busqueda de doctor por ID en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 3:
                                        cout << "Funcionalidad de busqueda de doctor por especialidad en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 4:
                                        cout << "Funcionalidad de asignar paciente a doctor en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 5:
                                        cout << "Funcionalidad de remover paciente de doctor en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 6:
                                        cout << "Funcionalidad de ver lista de pacientes asignados a doctor en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 7:
                                        cout << "Funcionalidad de ver lista de doctores en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 8:
                                        cout << "Funcionalidad de eliminar doctor en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 0:
                                        cout << "Volviendo al menu principal..." << endl;
                                        system("pause");
                                        break;
                                    default:
                                        cout << "¡ERROR!: Opcion invalida (0-8)" << endl;
                                        system("pause");
                                        break;
                    
                    }
            }while (wOpcionMD != 0);
            break;
       
            

            case 3:
                do{
                    system("cls");
                        cout<<"\t\t╔════════════════════════════════════════╗"<<endl;
                        cout<<"\t\t║            GESTION DE CITAS            ║"<<endl;
                        cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
                        cout<<"\t\t║          SELECCIONE UNA OPCION         ║"<<endl;
                        cout<<"\t\t╠════════════════════════════════════════╣"<<endl;
                        cout<<"\t\t║  1. Agendar nueva cita                 ║"<<endl;
                        cout<<"\t\t║  2. Suspender cita                     ║"<<endl;
                        cout<<"\t\t║  3. Atender cita                       ║"<<endl;
                        cout<<"\t\t║  4. Ver citas de un pacientes          ║"<<endl;
                        cout<<"\t\t║  5. Ver citas de un doctor             ║"<<endl;
                        cout<<"\t\t║  6. Ver citas de una fecha             ║"<<endl;       
                        cout<<"\t\t║  7. Ver listar citas pendientes        ║"<<endl;
                        cout<<"\t\t║  0. Volver al menu principal           ║"<<endl;
                        cout<<"\t\t╚════════════════════════════════════════╝"<<endl;

                            cin>>wOpcionMAC;

                                switch(wOpcionMAC){
                                    case 1:
                                        cout << "Funcionalidad de agendar nueva cita en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 2:
                                        cout << "Funcionalidad de suspender cita en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 3:
                                        cout << "Funcionalidad de atender cita en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 4:
                                        cout << "Funcionalidad de ver citas de un paciente en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 5:
                                        cout << "Funcionalidad de ver citas de un doctor en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 6:
                                        cout << "Funcionalidad de ver citas de una fecha en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 7:
                                        cout << "Funcionalidad de ver lista de citas pendientes en desarrollo." << endl;
                                        system("pause");
                                        break;
                                    case 0:
                                        cout << "Volviendo al menu principal..." << endl;
                                        system("pause");
                                        break;
                                    default: 
                                        cout << "¡ERROR!: Opcion invalida (0-7)" << endl;
                                        system("pause");
                                        break;
                                }
                }while (wOpcionMAC != 0);
                break;

            case 4:
                cout << "Saliendo del programa. ¡Hasta luego!" << endl;
                system("pause");
                break;
            default:
                cout << "¡ERROR!: Opcion invalida (1-4)" << endl;
                system("pause");
                break;
        }
    }while(wOpcionIMP != 4);

    destruirHospital(hospital);
    return 0;
}
