/*Programa Clinico de control 
Desarrollado por Ruben Adarme 32.453.470*/


#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <windows.h>
#include <cctype>

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

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[20];       // Ej: "2025-10-28"
    char hora[10];        // Ej: "14:30"
    char motivo[100];
    bool activa;
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
    
    Cita* citas;
    int cantidadCitas;
    int capacidadCitas;
    int siguienteIdCita;

    int siguienteIdConsulta;

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
    
    h->citas = new Cita[20]; // capacidad inicial
	h->cantidadCitas = 0;
	h->capacidadCitas = 20;
	h->siguienteIdCita = 1;
	
	h->siguienteIdConsulta = 1;

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
            return true; // Cedula duplicada
        }
    }
    return false; // Cedula no encontrada
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

        cout<<"\t\t+----------------------------------------+"<<endl;
        cout<<"\t\t+      INGRESO DE NUEVOS PACIENTES       +"<<endl;
        cout<<"\t\t+----------------------------------------+"<<endl;

        cout << "Nombre: ";
        cin.ignore();
        cin.getline(p.nombre, 50);

        cout << "Apellido: ";
        cin.getline(p.apellido, 50);

        do {
        cout << "Cedula: ";
        cin.getline(p.cedula, 20);

        if (cedulaYaRegistrada(hospital, p.cedula)) {
        cout << "ERROR!: Esta cedula ya esta registrada. Intente con otra.\n";
        }
        } while (cedulaYaRegistrada(hospital, p.cedula));

        cout << "Edad: ";
        cin >> p.edad;

        do{
        cout << "Sexo (M/F): ";
        cin >> p.sexo;
        if (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f') {
            cout << "ERROR!: Sexo inv?lido. Debe ser M o F.\n";
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

        cout << "Alergias: ";
        cin.getline(p.alergias, 500);

        cout << "Observaciones generales: ";
        cin.getline(p.observaciones, 500);

        // Inicializar arrays din?micos
        p.capacidadHistorial = 5;
        p.cantidadConsultas = 0;
        p.historial = new HistorialMedico[p.capacidadHistorial];

        p.capacidadCitas = 5;
        p.cantidadCitas = 0;
        p.citasAgendadas = new int[p.capacidadCitas];

        p.activo = true;

        hospital->cantidadPacientes++;

        cout << "\nPaciente registrado con exito. ID asignado:" << p.id << endl;

        cout << "\nDesea agregar otro paciente? (S/N): ";
        cin >> wAgregarMas;

    } while (wAgregarMas == 'S' || wAgregarMas == 's');
}



Doctor* buscarDoctorPorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            return &hospital->doctores[i];
        }
    }
    return nullptr;
}

Paciente* buscarPacientePorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            return &hospital->pacientes[i];
        }
    }
    return nullptr;
}

//========================================================================================================================================================================
//=================================================== FUNCION PARA BUSCAR PACUENTES POR C.I. =============================================================================
//========================================================================================================================================================================

void buscarPacientePorCedula(Hospital* hospital) {
    char cedulaBuscada[20];
    bool encontrado = false;

    cout << "\nIngrese la cedula del paciente a buscar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (strcmp(p.cedula, cedulaBuscada) == 0 && p.activo) {
            cout << "\nPaciente encontrado:\n";
            cout << "ID: " << p.id << endl;
            cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
            cout << "Cedula: " << p.cedula << endl;
            cout << "Edad: " << p.edad << endl;
            cout << "Sexo: " << p.sexo << endl;
            cout << "Tipo de sangre: " << p.tipoSangre << endl;
            cout << "Telefono: " << p.telefono << endl;
            cout << "Direccion: " << p.direccion << endl;
            cout << "Email: " << p.email << endl;
            cout << "Alergias: " << p.alergias << endl;
            cout << "Observaciones: " << p.observaciones << endl;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: No se encontro ningun paciente con esa cedula.\n";
    }
}


//========================================================================================================================================================================
//=================================================== FUNCION PARA BUSCAR PACUENTES POR NOMBRE ===========================================================================
//========================================================================================================================================================================

void buscarPacientePorNombre(Hospital* hospital) {
    char nombreBuscado[50];
    bool encontrado = false;

    cout << "\nIngrese el nombre del paciente a buscar: ";
    cin.ignore();
    cin.getline(nombreBuscado, 50);

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (strcmp(p.nombre, nombreBuscado) == 0 && p.activo) {
            cout << "\nPaciente encontrado:\n";
            cout << "ID: " << p.id << endl;
            cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
            cout << "Cedula: " << p.cedula << endl;
            cout << "Edad: " << p.edad << endl;
            cout << "Sexo: " << p.sexo << endl;
            cout << "Tipo de sangre: " << p.tipoSangre << endl;
            cout << "Telefono: " << p.telefono << endl;
            cout << "Direccion: " << p.direccion << endl;
            cout << "Email: " << p.email << endl;
            cout << "Alergias: " << p.alergias << endl;
            cout << "Observaciones: " << p.observaciones << endl;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun paciente con ese nombre.\n";
    }
}

//========================================================================================================================================================================
//=============================================== FUNCION VER EL HOSTORIAL MEDICO COMPLETO DE UN PACIENTE ================================================================
//========================================================================================================================================================================

void verHistorialMedico(Hospital* hospital) {
    int idPaciente;
    cout << "\nIngrese el ID del paciente: ";
    cin >> idPaciente;

    Paciente* paciente = buscarPacientePorId(hospital, idPaciente);
    if (paciente == nullptr || !paciente->activo) {
        cout << "\nERROR!: Paciente no válido o inactivo.\n";
        system("pause");
        return;
    }

    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "¦                  Historial médico de " << paciente->nombre << " " << paciente->apellido << "                  ¦" << endl;
    cout << "¦-------------------------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Fecha      ¦ Hora       ¦ Doctor     ¦ Diagnóstico                                   ¦" << endl;
    cout << "¦-----+------------+------------+------------+----------------------------------------------¦" << endl;

    bool hayConsultas = false;
    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& h = paciente->historial[i];
        Doctor* d = buscarDoctorPorId(hospital, h.idDoctor);
        hayConsultas = true;

        cout << "¦ " << setw(3) << h.idConsulta << " ¦ "
             << setw(10) << h.fecha << " ¦ "
             << setw(10) << h.hora << " ¦ "
             << setw(10) << (d ? d->nombre : "Desconocido") << " ¦ "
             << setw(45) << h.diagnostico << " ¦" << endl;
    }

    if (!hayConsultas) {
        cout << "¦                            No hay consultas registradas.                                 ¦" << endl;
    }

    cout << "+--------------------------------------------------------------------------------------------+\n" << endl;
    system("pause");
}


//========================================================================================================================================================================
//=================================================== FUNCION PARA ACTUALIZAR DATOS DEL PACIENTE =========================================================================
//========================================================================================================================================================================


void actualizarDatosPaciente(Hospital* hospital) {
	char opcion;
    char cedulaBuscada[20];
    bool encontrado = false;

    cout << "\nIngrese la cedula del paciente que desea actualizar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (strcmp(p.cedula, cedulaBuscada) == 0 && p.activo) {
            encontrado = true;

            cout << "\nPaciente encontrado. Datos actuales:\n";
            cout << "Nombre: " << p.nombre << endl;
            cout << "Apellido: " << p.apellido << endl;
            cout << "Edad: " << p.edad << endl;
            cout << "Sexo: " << p.sexo << endl;
            cout << "Tipo de sangre: " << p.tipoSangre << endl;
            cout << "Telefono: " << p.telefono << endl;
            cout << "Direccion: " << p.direccion << endl;
            cout << "Email: " << p.email << endl;
            cout << "Alergias: " << p.alergias << endl;
            cout << "Observaciones: " << p.observaciones << endl;

            cout << "\nDesea actualizar los datos? (S/N): ";
            cin >> opcion;
            cin.ignore();

            if (opcion == 'S' || opcion == 's') {
                cout << "\n--- ACTUALIZACION DE DATOS ---\n";

                cout << "Nuevo nombre: ";
                cin.getline(p.nombre, 50);

                cout << "Nuevo apellido: ";
                cin.getline(p.apellido, 50);

                cout << "Nueva edad: ";
                cin >> p.edad;
                cin.ignore();

                do {
                    cout << "Nuevo sexo (M/F): ";
                    cin >> p.sexo;
                    cin.ignore();
                    if (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f') {
                        cout << "ERROR!: Sexo invalido. Debe ser M o F.\n";
                    }
                } while (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f');

                cout << "Nuevo tipo de sangre: ";
                cin.getline(p.tipoSangre, 5);

                cout << "Nuevo telefono: ";
                cin.getline(p.telefono, 15);

                cout << "Nueva direccion: ";
                cin.getline(p.direccion, 100);

                cout << "Nuevo email: ";
                cin.getline(p.email, 50);

                cout << "Nuevas alergias: ";
                cin.getline(p.alergias, 500);

                cout << "Nuevas observaciones: ";
                cin.getline(p.observaciones, 500);

                cout << "\nDatos actualizados correctamente.\n";
            } else {
                cout << "\nOperacion cancelada.\n";
            }

            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun paciente con esa cedula.\n";
    }
}


//========================================================================================================================================================================
//===================================================== FUNCION PARA IMPRIMIR LA LISTA DE TODO LOS PACIENTE ==============================================================  
//========================================================================================================================================================================

void verListaPacientes(Hospital* hospital) {
    cout << "\n+------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Nombre y Apellido               | Cedula            | Edad | Sexo | Telefono                  | Estado      |" << endl;
    cout << "+------------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (p.activo) {
            cout << "| " << setw(2) << p.id << " | "
                 << setw(23) << p.nombre << " " << p.apellido << " | "
                 << setw(13) << p.cedula << " | "
                 << setw(4) << p.edad << " | "
                 << setw(4) << p.sexo << " | "
                 << setw(15) << p.telefono << " | "
                 << "Activo      |" << endl;
        }
    }

    cout << "+---------------------------------------------------------------------------------------------+" << endl;
}


//========================================================================================================================================================================
//===================================================== FUNCION PARA ElIMINAR PACIENTES  =================================================================================  
//========================================================================================================================================================================

void eliminarPacientePorCedula(Hospital* hospital) {
    char cedulaBuscada[20];
    bool encontrado = false;

    cout << "\nIngrese la cedula del paciente a eliminar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (strcmp(p.cedula, cedulaBuscada) == 0 && p.activo) {
            encontrado = true;

            cout << "\nPaciente encontrado:\n";
            cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
            cout << "Cedula: " << p.cedula << endl;

            char confirmacion;
            cout << "\nEsta seguro que desea eliminar este paciente? (S/N): ";
            cin >> confirmacion;

            if (confirmacion == 'S' || confirmacion == 's') {
                p.activo = false;
                cout << "\nPaciente eliminado correctamente.\n";
            } else {
                cout << "\nOperacion cancelada.\n";
            }

            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: No se encontro ningun paciente con esa cedula.\n";
    }
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

    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+       INGRESO DE NUEVOS DOCTORES       +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;

    cout << "Nombre: ";
    cin.ignore();
    cin.getline(d.nombre, 50);

    cout << "Apellido: ";
    cin.getline(d.apellido, 50);

    do {
        cout << "Cedula profesional: ";
        cin.getline(d.cedula, 20);
        if (cedulaDoctorYaRegistrada(hospital, d.cedula)) {
            cout << " C?dula ya registrada. Intente con otra.\n";
        }
    } while (cedulaDoctorYaRegistrada(hospital, d.cedula));

    cout << "Especialidad medica: ";
    cin.getline(d.especialidad, 50);

    cout << "Anios de experiencia: ";
    cin >> d.aniosExperiencia;

    cout << "Costo de consulta: ";
    cin >> d.costoConsulta;

    cout << "Horario de atencion (ej: Lun-Vie 8:00-16:00): ";
    cin.ignore();
    cin.getline(d.horarioAtencion, 50);

    cout << "Tel?fono: ";
    cin.getline(d.telefono, 15);

    cout << "Email: ";
    cin.getline(d.email, 50);

    // Inicializar arrays din?micos
    d.capacidadPacientes = 5;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = 10;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    d.disponible = true;

    hospital->cantidadDoctores++;

    cout << "\nDoctor registrado con exito. ID asignado: " << d.id << endl;

    return &d;
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA BUSCAR DOCTOR POR ID ================================================================================
//========================================================================================================================================================================

void buscarDoctorPorID(Hospital* hospital) {
    int ID_BUSCADO;
    bool encontrado = false;

    cout << "\nIngrese el ID del doctor a buscar: ";
    cin  >> ID_BUSCADO;
    cin.ignore();

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        if (d.id == ID_BUSCADO && d.disponible){
            cout << "\nDoctor encontrado:" << endl;
            cout << "Nombre: " << d.nombre << " " << d.apellido << endl;
            cout << "Cedula: " << d.cedula << endl;
            cout << "Anios de experiencia: "  << d.costoConsulta << endl;
            cout << "Costo de consulta: " << d.costoConsulta << endl;
            cout << "Horario de atencion (ej: Lun-Vie 8:00-16:00): " << d.horarioAtencion << endl;
            cout << "Telefono: " << d.telefono << endl;
            cout << "Correo: " << d.email << endl;
            
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: No se encontro ningun Doctor con ese ID.\n";
    }
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA BUSCAR DOCTOR POR ESPECIALIDAD ======================================================================
//========================================================================================================================================================================

bool compararEspecialidad(const char* a, const char* b) {
    char copiaA[50], copiaB[50];
    strncpy(copiaA, a, 49);
    strncpy(copiaB, b, 49);
    for (int i = 0; copiaA[i]; i++) copiaA[i] = tolower(copiaA[i]);
    for (int i = 0; copiaB[i]; i++) copiaB[i] = tolower(copiaB[i]);
    return strcmp(copiaA, copiaB) == 0;
}

void buscarDoctorPorEspecialidad(Hospital* hospital) {
    char Especialidad_BUSCADA[50];
    bool encontrado = false;

    cout << "\nIngrese la especialidad del doctor a buscar: ";
    cin.ignore();
    cin.getline(Especialidad_BUSCADA, 50);

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        if (compararEspecialidad(d.especialidad, Especialidad_BUSCADA) && d.disponible){
            cout << "\nDoctor encontrado:" << endl;
            cout << "Nombre: " << d.nombre << " " << d.apellido << endl;
            cout << "Cedula: " << d.cedula << endl;
            cout << "Anios de experiencia: "  << d.costoConsulta << endl;
            cout << "Costo de consulta: " << d.costoConsulta << endl;
            cout << "Horario de atenci?n (ej: Lun-Vie 8:00-16:00): " << d.horarioAtencion << endl;
            cout << "Tel?fono: " << d.telefono << endl;
            cout << "Correo: " << d.email << endl;
            cout << "ID: " << d.id << endl;
            
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: No se encontro ningun Doctor con esa especialidad.\n";
    }
}


//========================================================================================================================================================================
//===================================================== FUNCION PARA IMPRIMIR LA LISTA DE DOCTORES =======================================================================
//========================================================================================================================================================================

void verListaDoctores(Hospital* hospital) {
    cout << "\n+-------------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Nombre y Apellido               | Especialidad              | Experiencia | Consulta $. | Telefono             | Estado        |" << endl;
    cout << "+-------------------------------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        if (d.disponible) {
            cout << "| " << setw(2) << d.id << " | "
                 << setw(23) << d.nombre << " " << d.apellido << " | "
                 << setw(20) << d.especialidad << " | "
                 << setw(11) << d.aniosExperiencia << " | "
                 << setw(13) << fixed << setprecision(2) << d.costoConsulta << " | "
                 << setw(15) << d.telefono << " | "
                 << "Disponible |" << endl;
        }
    }

    cout << "+---------------------------------------------------------------------------------------------------------------+" << endl;
}

//========================================================================================================================================================================
//========================================================= FUNCION PARA ELIMINAR DOCTORES =============================================================================== 
//========================================================================================================================================================================

void eliminarDoctor(Hospital* hospital) {
    char nombreBuscado[50];
    char apellidoBuscado[50];
    bool encontrado = false;

    cout << "\nIngrese el nombre del doctor a eliminar: ";
    cin.ignore();
    cin.getline(nombreBuscado, 50);

    cout << "Ingrese el apellido del doctor: ";
    cin.getline(apellidoBuscado, 50);

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        if (strcmp(d.nombre, nombreBuscado) == 0 &&
            strcmp(d.apellido, apellidoBuscado) == 0 &&
            d.disponible) {

            encontrado = true;

            cout << "\nDoctor encontrado:\n";
            cout << "ID: " << d.id << endl;
            cout << "Especialidad: " << d.especialidad << endl;
            cout << "Telefono: " << d.telefono << endl;

            char confirmacion;
            cout << "\nEsta seguro que desea eliminar este doctor? (S/N): ";
            cin >> confirmacion;

            if (confirmacion == 'S' || confirmacion == 's') {
                d.disponible = false;
                cout << "\n? Doctor eliminado correctamente.\n";
            } else {
                cout << "\nOperacion cancelada.\n";
            }

            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: No se encontro ningun doctor con ese nombre y apellido.\n";
    }
}

//========================================================================================================================================================================
//========================================================= FUNCION PARA ASIGNAR PACIENTE A DOCTOR ======================================================================= 
//========================================================================================================================================================================

void asignarPacienteADoctor(Hospital* hospital) {
    int idDoctor, idPaciente;

    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;
    cout << "Ingrese el ID del paciente: ";
    cin >> idPaciente;

    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);
    Paciente* paciente = buscarPacientePorId(hospital, idPaciente);

    if (doctor == nullptr || !doctor->disponible || paciente == nullptr || !paciente->activo) {
        cout << "\nERROR!: Doctor o paciente no válido.\n";
        return;
    }

    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) {
            cout << "\nERROR!: El paciente ya está asignado a este doctor.\n";
            return;
        }
    }

    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCapacidad = doctor->capacidadPacientes * 2;
        int* nuevoArray = new int[nuevaCapacidad];
        for (int i = 0; i < doctor->cantidadPacientes; i++) {
            nuevoArray[i] = doctor->pacientesAsignados[i];
        }
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevoArray;
        doctor->capacidadPacientes = nuevaCapacidad;
    }

    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    cout << "\nPaciente asignado correctamente.\n";
    
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA REMOVER PACIENTE DE DOCTOR ========================================================================== 
//========================================================================================================================================================================

void removerPacienteDeDoctor(Hospital* hospital) {
    int idDoctor, idPaciente;

    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;
    cout << "Ingrese el ID del paciente a remover: ";
    cin >> idPaciente;

    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);
    if (doctor == nullptr || !doctor->disponible) {
        cout << "\nERROR!: Doctor no válido.\n";
        return;
    }

    bool encontrado = false;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) {
            for (int j = i; j < doctor->cantidadPacientes - 1; j++) {
                doctor->pacientesAsignados[j] = doctor->pacientesAsignados[j + 1];
            }
            doctor->cantidadPacientes--;
            encontrado = true;
            cout << "\nPaciente removido correctamente del doctor.\n";
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR!: El paciente no está asignado a este doctor.\n";
    }

}


//========================================================================================================================================================================
//============================================ FUNCION PARA VER LISTA DE PACIENTES ASIGNADOS A DOCTORES ================================================================== 
//========================================================================================================================================================================

void listarPacientesDeDoctor(Hospital* hospital) {
    int idDoctor;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;

    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);
    if (doctor == nullptr || !doctor->disponible) {
        cout << "\nERROR!: Doctor no válido o no disponible.\n";
        return;
    }

    cout << "\n+----------------------------------------------------------------------------+" << endl;
    cout << "¦               Pacientes asignados al Dr. " << doctor->nombre << " " << doctor->apellido << "               ¦" << endl;
    cout << "¦-------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Nombre completo     ¦ Cédula       ¦ Edad ¦ Teléfono              ¦" << endl;
    cout << "¦-----+---------------------+--------------+------+-----------------------¦" << endl;

    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        Paciente* p = buscarPacientePorId(hospital, doctor->pacientesAsignados[i]);
        if (p != nullptr && p->activo) {
            cout << "¦ " << setw(3) << p->id << " ¦ "
                 << setw(21) << p->nombre << " " << p->apellido << " ¦ "
                 << setw(12) << p->cedula << " ¦ "
                 << setw(4) << p->edad << " ¦ "
                 << setw(23) << p->telefono << " ¦" << endl;
        }
    }

    cout << "+-------------------------------------------------------------------------+\n" << endl;
}


//========================================================================================================================================================================
//================================================================= FUNCION PARA AGENDAR CITAS  ========================================================================== 
//========================================================================================================================================================================

void agendarCita(Hospital* hospital) {
    int idPaciente, idDoctor;
    char fecha[20], hora[10], motivo[100];

    cout << "\nIngrese ID del paciente: ";
    cin >> idPaciente;
    cout << "Ingrese ID del doctor: ";
    cin >> idDoctor;
    cin.ignore();

    cout << "Fecha (YYYY-MM-DD): ";
    cin.getline(fecha, 20);

    cout << "Hora (HH:MM): ";
    cin.getline(hora, 10);

    cout << "Motivo de la cita: ";
    cin.getline(motivo, 100);

    // Validaciones
    Paciente* paciente = buscarPacientePorId(hospital, idPaciente);
    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);

    if (paciente == nullptr || !paciente->activo || doctor == nullptr || !doctor->disponible) {
        cout << "\nERROR!: Paciente o doctor no valido.\n";
        return;
    }

    if (strlen(fecha) != 10 || fecha[4] != '-' || fecha[7] != '-') {
        cout << "\nERROR!: Formato de fecha invalido.\n";
        return;
    }

    if (strlen(hora) != 5 || hora[2] != ':') {
        cout << "\nERROR!: Formato de hora invalido.\n";
        return;
    }

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.idDoctor == idDoctor && strcmp(c.fecha, fecha) == 0 && strcmp(c.hora, hora) == 0 && c.activa) {
            cout << "\nERROR!: El doctor ya tiene una cita en esa fecha y hora.\n";
            return;
        }
    }

    // Redimensionar si es necesario
    if (hospital->cantidadCitas >= hospital->capacidadCitas) {
        int nuevaCapacidad = hospital->capacidadCitas * 2;
        Cita* nuevoArray = new Cita[nuevaCapacidad];
        for (int i = 0; i < hospital->cantidadCitas; i++) {
            nuevoArray[i] = hospital->citas[i];
        }
        delete[] hospital->citas;
        hospital->citas = nuevoArray;
        hospital->capacidadCitas = nuevaCapacidad;
    }

    // Crear cita
    Cita& nueva = hospital->citas[hospital->cantidadCitas++];
    nueva.id = hospital->siguienteIdCita++;
    nueva.idPaciente = idPaciente;
    nueva.idDoctor = idDoctor;
    strncpy(nueva.fecha, fecha, 19);
    strncpy(nueva.hora, hora, 9);
    strncpy(nueva.motivo, motivo, 99);
    nueva.activa = true;

    // Agregar ID a paciente y doctor
    paciente->citasAgendadas[paciente->cantidadCitas++] = nueva.id;
    doctor->citasAgendadas[doctor->cantidadCitas++] = nueva.id;

    cout << "\nCita agendada exitosamente. ID: " << nueva.id << endl;
}

//========================================================================================================================================================================
//============================================================ FUNCION PARA SUSPENDER CITAS ============================================================================== 
//========================================================================================================================================================================

void SuspenderCitaPorID(Hospital* hospital) {
    int idCita;
    bool encontrada = false;

    cout << "\nIngrese el ID de la cita que desea cancelar: ";
    cin >> idCita;
    cin.ignore();

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.id == idCita && c.activa) {
            encontrada = true;

            cout << "\nCita encontrada:\n";
            cout << "Paciente ID: " << c.idPaciente << endl;
            cout << "Doctor ID: " << c.idDoctor << endl;
            cout << "Fecha: " << c.fecha << " | Hora: " << c.hora << endl;
            cout << "Motivo: " << c.motivo << endl;

            char confirmacion;
            cout << "\nEsta seguro que desea cancelar esta cita? (S/N): ";
            cin >> confirmacion;

            if (confirmacion == 'S' || confirmacion == 's') {
                c.activa = false;

                // Remover ID de cita del paciente
                Paciente* p = buscarPacientePorId(hospital, c.idPaciente);
                if (p != nullptr) {
                    for (int j = 0; j < p->cantidadCitas; j++) {
                        if (p->citasAgendadas[j] == idCita) {
                            for (int k = j; k < p->cantidadCitas - 1; k++) {
                                p->citasAgendadas[k] = p->citasAgendadas[k + 1];
                            }
                            p->cantidadCitas--;
                            break;
                        }
                    }
                }

                // Remover ID de cita del doctor
                Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);
                if (d != nullptr) {
                    for (int j = 0; j < d->cantidadCitas; j++) {
                        if (d->citasAgendadas[j] == idCita) {
                            for (int k = j; k < d->cantidadCitas - 1; k++) {
                                d->citasAgendadas[k] = d->citasAgendadas[k + 1];
                            }
                            d->cantidadCitas--;
                            break;
                        }
                    }
                }

                cout << "\nCita cancelada correctamente.\n";
            } else {
                cout << "\nOperacion cancelada.\n";
            }

            break;
        }
    }

    if (!encontrada) {
        cout << "\nERROR!: No se encontro ninguna cita activa con ese ID.\n";
    }
}


//========================================================================================================================================================================
//============================================================ FUNCION PARA ATENDER CITA ============================================================================== 
//========================================================================================================================================================================

void atenderCita(Hospital* hospital) {
    int idCita;
    char diagnostico[200], tratamiento[200], medicamentos[150];

    cout << "\nIngrese el ID de la cita a atender: ";
    cin >> idCita;
    cin.ignore();

    cout << "Diagnostico: ";
    cin.getline(diagnostico, 200);

    cout << "Tratamiento: ";
    cin.getline(tratamiento, 200);

    cout << "Medicamentos recetados: ";
    cin.getline(medicamentos, 150);

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.id == idCita && c.activa) {
            c.activa = false;

            Paciente* p = buscarPacientePorId(hospital, c.idPaciente);
            Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);

            if (p == nullptr || d == nullptr) {
                cout << "\nERROR!: Paciente o doctor no valido.\n";
                return;
            }

            if (p->cantidadConsultas >= p->capacidadHistorial) {
                int nuevaCapacidad = p->capacidadHistorial * 2;
                HistorialMedico* nuevoArray = new HistorialMedico[nuevaCapacidad];
                for (int j = 0; j < p->cantidadConsultas; j++) {
                    nuevoArray[j] = p->historial[j];
                }
                delete[] p->historial;
                p->historial = nuevoArray;
                p->capacidadHistorial = nuevaCapacidad;
            }

            HistorialMedico& h = p->historial[p->cantidadConsultas++];
            h.idConsulta = p->cantidadConsultas;
            strncpy(h.fecha, c.fecha, 10);
            strncpy(h.hora, c.hora, 5);
            strncpy(h.diagnostico, diagnostico, 199);
            strncpy(h.tratamiento, tratamiento, 199);
            strncpy(h.medicamentos, medicamentos, 149);
            h.idDoctor = d->id;
            h.costo = d->costoConsulta;

            cout << "\nCita atendida y registrada en el historial medico.\n";
            system("pause");
            return;
        }
    }

    cout << "\nERROR!: No se encontro ninguna cita activa con ese ID.\n";
}

//========================================================================================================================================================================
//============================================ FUNCION PARA VER TODAS LAS CITAS DE UN PACIENTE =========================================================================== 
//========================================================================================================================================================================


void verCitasDePaciente(Hospital* hospital) {
    int idPaciente;
    cout << "\nIngrese el ID del paciente: ";
    cin >> idPaciente;

    Paciente* paciente = buscarPacientePorId(hospital, idPaciente);
    if (paciente == nullptr || !paciente->activo) {
        cout << "\n?? Paciente no valido o inactivo.\n";
        system("pause");
        return;
    }

    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "¦                     Citas activas del paciente: " << paciente->nombre << " " << paciente->apellido << "                     ¦" << endl;
    cout << "¦--------------------------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Fecha        ¦ Hora       ¦ Motivo                                                   ¦" << endl;
    cout << "¦-----+--------------+------------+----------------------------------------------------------¦" << endl;

    bool tieneCitas = false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.idPaciente == idPaciente && c.activa) {
            tieneCitas = true;
            cout << "¦ " << setw(3) << c.id << " ¦ "
                 << setw(12) << c.fecha << " ¦ "
                 << setw(10) << c.hora << " ¦ "
                 << setw(60) << c.motivo << " ¦" << endl;
        }
    }

    if (!tieneCitas) {
        cout << "¦                            No hay citas activas registradas.                              ¦" << endl;
    }

    cout << "+--------------------------------------------------------------------------------------------+\n" << endl;
}

//========================================================================================================================================================================
//============================================ FUNCION PARA VER TODAS LAS CITAS DE UN DOCTORR =========================================================================== 
//========================================================================================================================================================================

void verCitasDeDoctor(Hospital* hospital) {
    int idDoctor;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;

    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);
    if (doctor == nullptr || !doctor->disponible) {
        cout << "\nERROR!: Doctor no valido o no disponible.\n";
        system("pause");
        return;
    }

    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "¦                     Citas activas del Dr. " << doctor->nombre << " " << doctor->apellido << "                     ¦" << endl;
    cout << "¦--------------------------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Fecha        ¦ Hora       ¦ Motivo                                                   ¦" << endl;
    cout << "¦-----+--------------+------------+----------------------------------------------------------¦" << endl;

    bool tieneCitas = false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.idDoctor == idDoctor && c.activa) {
            tieneCitas = true;
            cout << "¦ " << setw(3) << c.id << " ¦ "
                 << setw(12) << c.fecha << " ¦ "
                 << setw(10) << c.hora << " ¦ "
                 << setw(60) << c.motivo << " ¦" << endl;
        }
    }

    if (!tieneCitas) {
        cout << "¦                            No hay citas activas registradas.                              ¦" << endl;
    }

    cout << "+--------------------------------------------------------------------------------------------+\n" << endl;
}

//========================================================================================================================================================================
//=========================================================== FUNCION PARA VER TODAS DE UN DIA =========================================================================== 
//========================================================================================================================================================================


void verCitasPorFecha(Hospital* hospital) {
    char fecha[20];
    cout << "\nIngrese la fecha que desea consultar (YYYY-MM-DD): ";
    cin.ignore();
    cin.getline(fecha, 20);

    // Validar formato de fecha
    if (strlen(fecha) != 10 || fecha[4] != '-' || fecha[7] != '-') {
        cout << "\nERROR!: Formato de fecha invalido. Debe ser YYYY-MM-DD.\n";
        system("pause");
        return;
    }

    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "¦                            Citas activas para el día " << fecha << "                            ¦" << endl;
    cout << "¦------------------------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Hora       ¦ Doctor     ¦ Motivo                                                   ¦" << endl;
    cout << "¦-----+------------+------------+----------------------------------------------------------¦" << endl;

    bool hayCitas = false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (strcmp(c.fecha, fecha) == 0 && c.activa) {
            Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);
            hayCitas = true;

            cout << "¦ " << setw(3) << c.id << " ¦ "
                 << setw(10) << c.hora << " ¦ "
                 << setw(10) << (d ? d->nombre : "Desconocido") << " ¦ "
                 << setw(60) << c.motivo << " ¦" << endl;
        }
    }

    if (!hayCitas) {
        cout << "¦                            No hay citas activas para esta fecha.                         ¦" << endl;
    }

    cout << "+--------------------------------------------------------------------------------------------+\n" << endl;
}


//========================================================================================================================================================================
//============================================ FUNCION PARA IMPRIMIR TODAS LAS CITAS QUE ESTAN PENDIENTES ================================================================ 
//========================================================================================================================================================================

void verCitasPendientes(Hospital* hospital) {
    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "¦                                Citas pendientes (activas y no atendidas)                   ¦" << endl;
    cout << "¦-------------------------------------------------------------------------------------------¦" << endl;
    cout << "¦ ID  ¦ Fecha      ¦ Hora       ¦ Doctor     ¦ Motivo                                       ¦" << endl;
    cout << "¦-----+------------+------------+------------+----------------------------------------------¦" << endl;

    bool hayPendientes = false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (c.activa) {
            Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);
            hayPendientes = true;

            cout << "¦ " << setw(3) << c.id << " ¦ "
                 << setw(10) << c.fecha << " ¦ "
                 << setw(10) << c.hora << " ¦ "
                 << setw(10) << (d ? d->nombre : "Desconocido") << " ¦ "
                 << setw(45) << c.motivo << " ¦" << endl;
        }
    }

    if (!hayPendientes) {
        cout << "¦                            No hay citas pendientes registradas.                          ¦" << endl;
    }

    cout << "+--------------------------------------------------------------------------------------------+\n" << endl;
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
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+            HOSPITAL CENTRAL            +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+  RIF: J-12345678-9                     +"<<endl;
    cout<<"\t\t+  Telefono: (0261) 123-4567             +"<<endl;
    cout<<"\t\t+  Email: ClinicaOdant@gmail.com         +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+  Atencion: Lunes a Viernes             +"<<endl;
    cout<<"\t\t+  8:00 am a 12:00 m./1:00 pm a 5:00 pm  +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+  Direccion: Calle 123, Urb. La Salud   +"<<endl;
    cout<<"\t\t+  Ciudad: Maracaibo, Edo. Zulia         +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+          SELECCIONE UNA OPCION         +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+  1. Gestion de Pacientes               +"<<endl;
    cout<<"\t\t+  2. Gestion de Doctores                +"<<endl;
    cout<<"\t\t+  3. Gestion de Citas                   +"<<endl;
    cout<<"\t\t+  4. Salir                              +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;

    cin>>wOpcionIMP;
  
    system("cls");  
    switch(wOpcionIMP){

        
        case 1:
            
            do{
            system("cls");
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+           GESTION DE PACIENTES         +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+         SELECCIONE UNA OPCION          +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+  1. Ingresar nuevo paciente            +"<<endl;
            cout<<"\t\t+  2. Buscar paciente por C.I            +"<<endl;
            cout<<"\t\t+  3. Buscar paciente por nombre         +"<<endl;
            cout<<"\t\t+  4. Ver historial medico completo      +"<<endl;
            cout<<"\t\t+  5. Actualizar datos del paciente      +"<<endl;
            cout<<"\t\t+  6. Ver lista de paciente clinica      +"<<endl;       
            cout<<"\t\t+  7. Eliminar paciente                  +"<<endl;
            cout<<"\t\t+  0. Volver a menu principal            +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;

                cin>>wOpcionMP;
                
                    system("cls");
                        switch(wOpcionMP){
                            case 1:
                                INGRESO_NUEVOS_PACIENTES(hospital);
                                system("pause");
                                break;
                            case 2:
                                buscarPacientePorCedula(hospital);
                                system("pause");
                                break;
                            case 3:
                                buscarPacientePorNombre(hospital);
                                system("pause");
                                break;
                            case 4:
                                verHistorialMedico(hospital);
                                system("pause");
                                break;
                            case 5:
                                actualizarDatosPaciente(hospital);
                                system("pause");
                                break;
                            case 6:
                                verListaPacientes(hospital);
                                system("pause");
                                break;
                            case 7:
                                eliminarPacientePorCedula (hospital);
                                system("pause");
                                break;
                            case 0:
                                cout << "Volviendo al menu principal..." << endl;
                                system("pause");
                                break;
                            default:
                                cout << "ERROR!: Opcion invalida (0-7)" << endl;
                                system("pause");
                                break;
                }
            }while (wOpcionMP != 0);
            break;

        case 2:
            do{
                 system("cls"); 
                    cout<<"\t\t+-----------------------------------------------+"<<endl;
                    cout<<"\t\t+             GESTION DE DOCTORES               +"<<endl;
                    cout<<"\t\t+-----------------------------------------------+"<<endl; 
                    cout<<"\t\t+            SELECCIONE UNA OPCION              +"<<endl;
                    cout<<"\t\t+-----------------------------------------------+"<<endl;
                    cout<<"\t\t+  1. Ingresar nuevo doctor                     +"<<endl;
                    cout<<"\t\t+  2. Buscar doctor por ID                      +"<<endl;
                    cout<<"\t\t+  3. Buscar doctor por especialidad            +"<<endl;
                    cout<<"\t\t+  4. Asignar paciente a doctor                 +"<<endl; 
                    cout<<"\t\t+  5. Remover paciente de doctor                +"<<endl;
                    cout<<"\t\t+  6. Lista de pacientes asignados a doctor     +"<<endl;       
                    cout<<"\t\t+  7. Ver lista de doctores clinica             +"<<endl;
                    cout<<"\t\t+  8. Eliminar doctor                           +"<<endl;
                    cout<<"\t\t+  0. Volver al menu principal                  +"<<endl;
                    cout<<"\t\t+-----------------------------------------------+"<<endl;

                        cin>>wOpcionMD;
							
                             system("cls");
                                 switch(wOpcionMD) {
                                    case 1: 
                                        INGRESO_NUEVO_DOCTOR(hospital);
                                        system("pause");
                                        break;
                                    case 2:
                                        buscarDoctorPorID(hospital);
                                        system("pause");
                                        break;
                                    case 3:
                                        buscarDoctorPorEspecialidad(hospital);
                                        system("pause");
                                        break;
                                    case 4: 
        								asignarPacienteADoctor(hospital);
        								system("pause");
										break;
    								case 5: 
    									removerPacienteDeDoctor(hospital);
        							    system("pause");
										break;
    								case 6: 
										listarPacientesDeDoctor(hospital);
										system("pause");
										break;
                                    case 7:
                                        verListaDoctores(hospital);
                                        system("pause");
                                        break;
                                    case 8:
                                        eliminarDoctor(hospital);
                                        system("pause");
                                        break;
                                    case 0:
                                        cout << "Volviendo al menu principal..." << endl;
                                        system("pause");
                                        break;
                                    default:
                                        cout << "ERROR!: Opcion invalida (0-8)" << endl;
                                        system("pause");
                                        break;
                    
                    }
                    
            }while (wOpcionMD != 0);
            break;
 

            case 3:
                do{
                    system("cls");
                        cout<<"\t\t+----------------------------------------+"<<endl;
                        cout<<"\t\t+            GESTION DE CITAS            +"<<endl;
                        cout<<"\t\t+----------------------------------------+"<<endl;
                        cout<<"\t\t+          SELECCIONE UNA OPCION         +"<<endl;
                        cout<<"\t\t+----------------------------------------+"<<endl;
                        cout<<"\t\t+  1. Agendar nueva cita                 +"<<endl;
                        cout<<"\t\t+  2. Suspender cita                     +"<<endl;
                        cout<<"\t\t+  3. Atender cita                       +"<<endl;
                        cout<<"\t\t+  4. Ver citas de un pacientes          +"<<endl;
                        cout<<"\t\t+  5. Ver citas de un doctor             +"<<endl;
                        cout<<"\t\t+  6. Ver citas de una fecha             +"<<endl;       
                        cout<<"\t\t+  7. Ver listar citas pendientes        +"<<endl;
                        cout<<"\t\t+  0. Volver al menu principal           +"<<endl;
                        cout<<"\t\t+----------------------------------------+"<<endl;

                            cin>>wOpcionMAC;
                            
								  switch(wOpcionMAC){
                                    case 1:
                                        agendarCita(hospital);
                                        system("pause");
                                        break;
                                    case 2:
                                        SuspenderCitaPorID(hospital);
                                        system("pause");
                                        break;
                                    case 3:
 									   atenderCita(hospital);
 									   system("pause");
 									   break;
                                    case 4:
                                        verCitasDePaciente(hospital);
                                        system("pause");
                                        break;
                                    case 5:
                                        verCitasDeDoctor(hospital);
                                        system("pause");
                                        break;
                                    case 6:
                                        verCitasPorFecha(hospital);
                                        system("pause");
                                        break;
                                    case 7:
                                        verCitasPendientes(hospital);
                                        system("pause");
                                        break;
                                    case 0:
                                        cout << "Volviendo al menu principal..." << endl;
                                        system("pause");
                                        break;
                                    default: 
                                        cout << "ERROR!: Opcion invalida (0-7)" << endl;
                                        system("pause");
                                        break;
                                }
                }while (wOpcionMAC != 0);
                break;

            case 4:
                cout << "Saliendo del programa... Vuelva PRonto!" << endl;
                system("pause");
                break;
            default:
                cout << "ERROR!: Opcion invalida (1-4)" << endl;
                system("pause");
                break;
        }
    }while(wOpcionIMP != 4);

    destruirHospital(hospital);
    return 0;
}
