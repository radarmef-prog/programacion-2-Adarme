/*Programa Clinico de control 
Desarrollado por Ruben Adarme 32.453.470*/


#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;

//========================================================================================================================================================================
//================================================================== ESTRUCTURAS =========================================================================================      
//========================================================================================================================================================================

struct HistorialMedico {
    // Datos originales
    int id;
    int pacienteID;                 // NUEVO: Referencia al paciente
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int doctorID;
    float costo;
    
    // NUEVO: Navegación enlazada
    int siguienteConsultaID;        // ID de siguiente consulta del mismo paciente
                                    // -1 si es la última
    
    // Metadata
    bool eliminado;
    time_t fechaRegistro;
};

struct Paciente {
    // Datos originales (sin cambios)
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
    char alergias[500];
    char observaciones[500];
    bool activo;
    
    // NUEVO: Índices para relaciones
    int cantidadConsultas;          // Total de consultas en historial
    int primerConsultaID;           // ID de primera consulta en historiales.bin
    
    int cantidadCitas;              // Total de citas agendadas
    int citasIDs[20];               // Array FIJO de IDs de citas (máx 20)
    
    // Metadata de registro
    bool eliminado;                 // Flag para borrado lógico
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct Doctor {
    // Datos originales
    int id;
    char nombre[50];
    char apellido[50];
    char cedulaProfesional[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];
    bool disponible;
    
    // NUEVO: Relaciones con arrays fijos
    int cantidadPacientes;
    int pacientesIDs[50];           // Máximo 50 pacientes
    
    int cantidadCitas;
    int citasIDs[30];               // Máximo 30 citas
    
    // Metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};


struct Cita {
    // Datos originales (sin cambios significativos)
    int id;
    int pacienteID;
    int doctorID;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];                // "Agendada", "Atendida", "Cancelada"
    char observaciones[200];
    bool atendida;
    
    // NUEVO: Referencia al historial
    int consultaID;                 // ID de consulta creada al atender
                                    // -1 si no ha sido atendida
    
    // Metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct ArchivoHeader {
    int cantidadRegistros;      // Cantidad actual de registros
    int proximoID;              // Siguiente ID disponible
    int registrosActivos;       // Registros no eliminados
    int version;                // Versión del formato (ej: 1, 2, 3...)
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    
    // Contadores de IDs (auto-increment)
    int siguienteIDPaciente;
    int siguienteIDDoctor;
    int siguienteIDCita;
    int siguienteIDConsulta;
    
    // Estadísticas generales
    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

//========================================================================================================================================================================
//================================================== FUNCION PARA INICIAR HOSPITAL =======================================================================================  
//========================================================================================================================================================================

Hospital* inicializarHospital(const char* nombre, const char* direccion, const char* telefono) {
    Hospital* h = new Hospital;

    strncpy(h->nombre, nombre, sizeof(h->nombre) - 1);
    strncpy(h->direccion, direccion, sizeof(h->direccion) - 1);
    strncpy(h->telefono, telefono, sizeof(h->telefono) - 1);

    h->siguienteIDPaciente = 1;
    h->siguienteIDDoctor = 1;
    h->siguienteIDCita = 1;
    h->siguienteIDConsulta = 1;

    h->totalPacientesRegistrados = 0;
    h->totalDoctoresRegistrados = 0;
    h->totalCitasAgendadas = 0;
    h->totalConsultasRealizadas = 0;

    return h;
}

//========================================================================================================================================================================
//================================================= FUNCION PARA VERIFICAR EL INGRESO DE CEDULAS REPETIDAS ===============================================================  
//========================================================================================================================================================================

bool cedulaYaRegistrada(const char* cedula) {
    // Verificar en pacientes.bin
    ifstream archivoPac("pacientes.bin", ios::binary);
    if (archivoPac.is_open()) {
        ArchivoHeader headerPac;
        archivoPac.read((char*)&headerPac, sizeof(ArchivoHeader));

        Paciente p;
        for (int i = 0; i < headerPac.cantidadRegistros; i++) {
            archivoPac.read((char*)&p, sizeof(Paciente));
            if (!p.eliminado && strcmp(p.cedula, cedula) == 0) {
                archivoPac.close();
                return true;
            }
        }
        archivoPac.close();
    }

    // Verificar en doctores.bin
    ifstream archivoDoc("doctores.bin", ios::binary);
    if (archivoDoc.is_open()) {
        ArchivoHeader headerDoc;
        archivoDoc.read((char*)&headerDoc, sizeof(ArchivoHeader));

        Doctor d;
        for (int i = 0; i < headerDoc.cantidadRegistros; i++) {
            archivoDoc.read((char*)&d, sizeof(Doctor));
            if (!d.eliminado && strcmp(d.cedulaProfesional, cedula) == 0) {
                archivoDoc.close();
                return true;
            }
        }
        archivoDoc.close();
    }

    return false; // No se encontró en ninguno
}


//========================================================================================================================================================================
//=================================================== FUNCION PARA INGRESAR NUEVO PACIENTE ===============================================================================  
//========================================================================================================================================================================

void INGRESO_NUEVOS_PACIENTES() {
    fstream archivo("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    // Leer header
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char opcion;
    do {
        Paciente p = {};
        p.id = header.proximoID++;
        p.activo = true;
        p.eliminado = false;
        p.fechaCreacion = time(nullptr);
        p.fechaModificacion = p.fechaCreacion;
        p.cantidadConsultas = 0;
        p.primerConsultaID = -1;
        p.cantidadCitas = 0;
        memset(p.citasIDs, -1, sizeof(p.citasIDs));

        cout<<"\t\t+----------------------------------------+"<<endl;
        cout<<"\t\t+      INGRESO DE NUEVOS PACIENTES       +"<<endl;
        cout<<"\t\t+----------------------------------------+"<<endl;

        cin.ignore();
        cout << "Nombre: "; cin.getline(p.nombre, 50);
        cout << "Apellido: "; cin.getline(p.apellido, 50);

        // Verificación de cédula duplicada
        do {
            cout << "Cedula: "; cin.getline(p.cedula, 20);
            if (cedulaYaRegistrada(p.cedula)) {
                cout << "ERROR: Esta cédula ya está registrada. Intente con otra.\n";
            }
        } while (cedulaYaRegistrada(p.cedula));

        cout << "Edad: "; cin >> p.edad;
        do {
            cout << "Sexo (M/F): "; cin >> p.sexo;
        } while (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f');
        cin.ignore();
        cout << "Tipo de sangre: "; cin.getline(p.tipoSangre, 5);
        cout << "Telefono: "; cin.getline(p.telefono, 15);
        cout << "Direccion: "; cin.getline(p.direccion, 100);
        cout << "Email: "; cin.getline(p.email, 50);
        cout << "Alergias: "; cin.getline(p.alergias, 500);
        cout << "Observaciones: "; cin.getline(p.observaciones, 500);

        // Posicionarse al final y escribir el paciente
        archivo.seekp(0, ios::end);
        archivo.write((char*)&p, sizeof(Paciente));

        // Actualizar header
        header.cantidadRegistros++;
        header.registrosActivos++;
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));

        cout << "\nPaciente registrado con ID: " << p.id << endl;
        cout << "Desea agregar otro paciente? (S/N): ";
        cin >> opcion;

    } while (opcion == 'S' || opcion == 's');

    archivo.close();
}

//========================================================================================================================================================================
//=================================================== FUNCION PARA BUSCAR PACUENTES POR C.I. =============================================================================
//========================================================================================================================================================================

void buscarPacientePorCedula() {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char cedulaBuscada[20];
    cout << "\nIngrese la cedula del paciente a buscar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    bool encontrado = false;
    Paciente p;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado && strcmp(p.cedula, cedulaBuscada) == 0) {
            encontrado = true;

            cout << "\nPaciente encontrado:\n";
            cout << "ID: " << p.id << endl;
            cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
            cout << "Cedula: " << p.cedula << endl;
            cout << "Edad: " << p.edad << endl;
            cout << "Sexo: " << p.sexo << endl;
            cout << "Tipo de sangre: " << p.tipoSangre << endl;
            cout << "Telefono: " << p.telefono << endl;
            cout << "Dirección: " << p.direccion << endl;
            cout << "Email: " << p.email << endl;
            cout << "Alergias: " << p.alergias << endl;
            cout << "Observaciones: " << p.observaciones << endl;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun paciente con esa cedula.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//=================================================== FUNCION PARA BUSCAR PACUENTES POR NOMBRE ===========================================================================
//========================================================================================================================================================================

void buscarPacientePorNombre() {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char nombreBuscado[50];
    cout << "\nIngrese el nombre del paciente a buscar: ";
    cin.ignore();
    cin.getline(nombreBuscado, 50);

    bool encontrado = false;
    Paciente p;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado && strcmp(p.nombre, nombreBuscado) == 0) {
            encontrado = true;

            cout << "\nPaciente encontrado:\n";
            cout << "ID: " << p.id << endl;
            cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
            cout << "Cedula: " << p.cedula << endl;
            cout << "Edad: " << p.edad << endl;
            cout << "Sexo: " << p.sexo << endl;
            cout << "Tipo de sangre: " << p.tipoSangre << endl;
            cout << "Telefono: " << p.telefono << endl;
            cout << "Dirección: " << p.direccion << endl;
            cout << "Email: " << p.email << endl;
            cout << "Alergias: " << p.alergias << endl;
            cout << "Observaciones: " << p.observaciones << endl;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun paciente con ese nombre.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//=============================================== FUNCION VER EL HOSTORIAL MEDICO COMPLETO DE UN PACIENTE ================================================================
//========================================================================================================================================================================

void verHistorialMedico() {
    int idPaciente;
    cout << "\nIngrese el ID del paciente: ";
    cin >> idPaciente;

    // Buscar paciente por ID
    ifstream archivoPacientes("pacientes.bin", ios::binary);
    if (!archivoPacientes.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerPac;
    archivoPacientes.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente paciente;
    bool encontrado = false;

    for (int i = 0; i < headerPac.cantidadRegistros; i++) {
        archivoPacientes.read((char*)&paciente, sizeof(Paciente));
        if (!paciente.eliminado && paciente.id == idPaciente) {
            encontrado = true;
            break;
        }
    }

    archivoPacientes.close();

    if (!encontrado) {
        cout << "\nERROR: Paciente no encontrado.\n";
        return;
    }

    if (paciente.primerConsultaID == -1 || paciente.cantidadConsultas == 0) {
        cout << "\nEste paciente no tiene historial medico registrado.\n";
        return;
    }

    // Leer historial enlazado
    ifstream archivoHistorial("historiales.bin", ios::binary);
    if (!archivoHistorial.is_open()) {
        cerr << "ERROR: No se pudo abrir historiales.bin\n";
        return;
    }

    ArchivoHeader headerHist;
    archivoHistorial.read((char*)&headerHist, sizeof(ArchivoHeader));

    int consultaID = paciente.primerConsultaID;
    int consultasMostradas = 0;

    cout << "\n+--------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID  | Fecha      | Hora    | DoctorID | Diagnostico                                        |" << endl;
    cout << "+--------------------------------------------------------------------------------------------+" << endl;

    while (consultaID != -1 && consultasMostradas < paciente.cantidadConsultas) {
        HistorialMedico consulta;
        bool consultaEncontrada = false;

        // Buscar consulta por ID
        for (int i = 0; i < headerHist.cantidadRegistros; i++) {
            archivoHistorial.read((char*)&consulta, sizeof(HistorialMedico));
            if (!consulta.eliminado && consulta.id == consultaID) {
                consultaEncontrada = true;
                break;
            }
        }

        if (!consultaEncontrada) break;

        cout << "| " << setw(3) << consulta.id << " | "
             << setw(10) << consulta.fecha << " | "
             << setw(6) << consulta.hora << " | "
             << setw(8) << consulta.doctorID << " | "
             << setw(45) << consulta.diagnostico << " |" << endl;

        consultaID = consulta.siguienteConsultaID;
        consultasMostradas++;
        archivoHistorial.clear();
        archivoHistorial.seekg(sizeof(ArchivoHeader)); // Reiniciar para buscar siguiente
    }

    cout << "+--------------------------------------------------------------------------------------------+\n";

    archivoHistorial.close();
}

//========================================================================================================================================================================
//=================================================== FUNCION PARA ACTUALIZAR DATOS DEL PACIENTE =========================================================================
//========================================================================================================================================================================

void actualizarDatosPaciente() {
    fstream archivo("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char cedulaBuscada[20];
    cout << "\nIngrese la cédula del paciente que desea actualizar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    Paciente p;
    bool encontrado = false;
    int posicionRegistro = -1;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long posicion = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivo.seekg(posicion);
        archivo.read((char*)&p, sizeof(Paciente));

        if (!p.eliminado && strcmp(p.cedula, cedulaBuscada) == 0) {
            encontrado = true;
            posicionRegistro = posicion;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun paciente con esa cedula.\n";
        archivo.close();
        return;
    }

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

    char opcion;
    cout << "\nDesea actualizar los datos? (S/N): ";
    cin >> opcion;
    cin.ignore();

    if (opcion == 'S' || opcion == 's') {
        cout<<"\t\t+----------------------------------------+"<<endl;
        cout<<"\t\t+      	ACTUALIZACION DE DATOS          +"<<endl;
        cout<<"\t\t+----------------------------------------+"<<endl;


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
                cout << "ERROR: Sexo inválido. Debe ser M o F.\n";
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

        p.fechaModificacion = time(nullptr);

        // Posicionarse y sobrescribir
        archivo.seekp(posicionRegistro);
        archivo.write((char*)&p, sizeof(Paciente));

        cout << "\nDatos actualizados correctamente.\n";
    } else {
        cout << "\nOperacion cancelada.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA IMPRIMIR LA LISTA DE TODO LOS PACIENTE ==============================================================  
//========================================================================================================================================================================

void verListaPacientes() {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    cout << "\n+------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Nombre y Apellido               | Cedula            | Edad | Sexo | Telefono                  | Estado      |" << endl;
    cout << "+------------------------------------------------------------------------------------------------------------------+" << endl;

    Paciente p;
    int totalActivos = 0;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado) {
            cout << "| " << setw(2) << p.id << " | "
                 << setw(23) << p.nombre << " " << p.apellido << " | "
                 << setw(13) << p.cedula << " | "
                 << setw(4) << p.edad << " | "
                 << setw(4) << p.sexo << " | "
                 << setw(25) << p.telefono << " | "
                 << "Activo      |" << endl;
            totalActivos++;
        }
    }

    cout << "+------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "Total de pacientes activos: " << totalActivos << endl;

    archivo.close();
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA ElIMINAR PACIENTES  =================================================================================  
//========================================================================================================================================================================

void eliminarPacientePorCedula() {
    fstream archivo("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char cedulaBuscada[20];
    cout << "\nIngrese la cedula del paciente a eliminar: ";
    cin.ignore();
    cin.getline(cedulaBuscada, 20);

    Paciente p;
    bool encontrado = false;
    long posicion = -1;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivo.seekg(pos);
        archivo.read((char*)&p, sizeof(Paciente));

        if (!p.eliminado && strcmp(p.cedula, cedulaBuscada) == 0) {
            encontrado = true;
            posicion = pos;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR: No se encontro ningun paciente con esa cedula.\n";
        archivo.close();
        return;
    }

    cout << "\nPaciente encontrado:\n";
    cout << "Nombre: " << p.nombre << " " << p.apellido << endl;
    cout << "Cédula: " << p.cedula << endl;

    char confirmacion;
    cout << "\nEsta seguro que desea eliminar este paciente? (S/N): ";
    cin >> confirmacion;

    if (confirmacion == 'S' || confirmacion == 's') {
        p.eliminado = true;
        p.activo = false;
        p.fechaModificacion = time(nullptr);

        archivo.seekp(posicion);
        archivo.write((char*)&p, sizeof(Paciente));

        header.registrosActivos--;
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));

        cout << "\nPaciente eliminado correctamente.\n";
    } else {
        cout << "\nOperacion cancelada.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================







//========================================================================================================================================================================

//========================================================================================================================================================================
//===================================================== FUNCION PARA INGRESAR NUEVO DOCTOR ===============================================================================  
//========================================================================================================================================================================

void INGRESO_NUEVO_DOCTOR() {
    fstream archivo("doctores.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    // Leer header
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char opcion;
    do {
        Doctor d = {};
        d.id = header.proximoID++;
        d.fechaCreacion = time(nullptr);
        d.fechaModificacion = d.fechaCreacion;
        d.eliminado = false;
        d.disponible = true;
        d.cantidadPacientes = 0;
        d.cantidadCitas = 0;
        memset(d.pacientesIDs, -1, sizeof(d.pacientesIDs));
        memset(d.citasIDs, -1, sizeof(d.citasIDs));

        cout<<"\t\t+----------------------------------------+"<<endl;
        cout<<"\t\t+      INGRESO DE NUEVO DOCTOR           +"<<endl;
        cout<<"\t\t+----------------------------------------+"<<endl;

        cin.ignore();
        cout << "Nombre: "; cin.getline(d.nombre, 50);
        cout << "Apellido: "; cin.getline(d.apellido, 50);

        // Verificación de cédula profesional duplicada
        do {
            cout << "Cedula profesional: "; cin.getline(d.cedulaProfesional, 20);
            if (cedulaYaRegistrada(d.cedulaProfesional)) {
                cout << "ERROR: Esta cédula profesional ya está registrada. Intente con otra.\n";
            }
        } while (cedulaYaRegistrada(d.cedulaProfesional));

        cout << "Especialidad médica: "; cin.getline(d.especialidad, 50);
        cout << "Años de experiencia: "; cin >> d.aniosExperiencia;
        cout << "Costo de consulta: "; cin >> d.costoConsulta;
        cin.ignore();
        cout << "Horario de atención (ej: Lun-Vie 8:00-16:00): "; cin.getline(d.horarioAtencion, 50);
        cout << "Teléfono: "; cin.getline(d.telefono, 15);
        cout << "Email: "; cin.getline(d.email, 50);

        // Escribir al final del archivo
        archivo.seekp(0, ios::end);
        archivo.write((char*)&d, sizeof(Doctor));

        // Actualizar header
        header.cantidadRegistros++;
        header.registrosActivos++;
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));

        cout << "\nDoctor registrado con éxito. ID asignado: " << d.id << endl;
        cout << "¿Desea agregar otro doctor? (S/N): ";
        cin >> opcion;

    } while (opcion == 'S' || opcion == 's');

    archivo.close();
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA BUSCAR DOCTOR POR ID ================================================================================
//========================================================================================================================================================================

void buscarDoctorPorID() {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    int idBuscado;
    cout << "\nIngrese el ID del doctor a buscar: ";
    cin >> idBuscado;

    Doctor d;
    bool encontrado = false;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado && d.id == idBuscado) {
            encontrado = true;

            cout << "\nDoctor encontrado:\n";
            cout << "ID: " << d.id << endl;
            cout << "Nombre: " << d.nombre << " " << d.apellido << endl;
            cout << "Cedula profesional: " << d.cedulaProfesional << endl;
            cout << "Especialidad: " << d.especialidad << endl;
            cout << "Años de experiencia: " << d.aniosExperiencia << endl;
            cout << "Costo de consulta: " << fixed << setprecision(2) << d.costoConsulta << endl;
            cout << "Horario de atencion: " << d.horarioAtencion << endl;
            cout << "Telefono: " << d.telefono << endl;
            cout << "Email: " << d.email << endl;
            cout << "Estado: " << (d.disponible ? "Disponible" : "No disponible") << endl;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun doctor con ese ID.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA BUSCAR DOCTOR POR ESPECIALIDAD ======================================================================
//========================================================================================================================================================================

void buscarDoctorPorEspecialidad() {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    char especialidadBuscada[50];
    cout << "\nIngrese la especialidad del doctor a buscar: ";
    cin.ignore();
    cin.getline(especialidadBuscada, 50);

    // Convertir a minúsculas para comparación insensible
    for (int i = 0; especialidadBuscada[i]; i++)
        especialidadBuscada[i] = tolower(especialidadBuscada[i]);

    Doctor d;
    bool encontrado = false;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado) {
            char especialidadActual[50];
            strncpy(especialidadActual, d.especialidad, 49);
            for (int j = 0; especialidadActual[j]; j++)
                especialidadActual[j] = tolower(especialidadActual[j]);

            if (strcmp(especialidadActual, especialidadBuscada) == 0) {
                encontrado = true;

                cout << "\nDoctor encontrado:\n";
                cout << "ID: " << d.id << endl;
                cout << "Nombre: " << d.nombre << " " << d.apellido << endl;
                cout << "Cedula profesional: " << d.cedulaProfesional << endl;
                cout << "Especialidad: " << d.especialidad << endl;
                cout << "Años de experiencia: " << d.aniosExperiencia << endl;
                cout << "Costo de consulta: " << fixed << setprecision(2) << d.costoConsulta << endl;
                cout << "Horario de atención: " << d.horarioAtencion << endl;
                cout << "Telefono: " << d.telefono << endl;
                cout << "Email: " << d.email << endl;
                cout << "Estado: " << (d.disponible ? "Disponible" : "No disponible") << endl;
                // Puedes seguir mostrando más si lo deseas
            }
        }
    }

    if (!encontrado) {
			cout << "\nNo se encontraron doctores con esa especialidad.\n";
    }

    archivo.close();
    
    }

//========================================================================================================================================================================
//===================================================== FUNCION PARA IMPRIMIR LA LISTA DE DOCTORES =======================================================================
//========================================================================================================================================================================

void verListaDoctores() {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Doctor d;

    cout << "\n+-----------------------------------------------------------------------------------------+\n";
    cout << "| Nombre y Apellido         | Especialidad         | Experiencia | Citas | Teléfono       |\n";
    cout << "+-----------------------------------------------------------------------------------------+\n";

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado) {
            cout << "| " << setw(25) << (string(d.nombre) + " " + string(d.apellido)) << " | "
                 << setw(20) << d.especialidad << " | "
                 << setw(10) << d.aniosExperiencia << " | "
                 << setw(5) << d.cantidadCitas << " | "
                 << setw(15) << d.telefono << " |\n";
        }
    }

    cout << "+-----------------------------------------------------------------------------------------+\n";

    archivo.close();
}

//========================================================================================================================================================================
//========================================================= FUNCION PARA ELIMINAR DOCTORES =============================================================================== 
//========================================================================================================================================================================

void eliminarDoctorPorID() {
    fstream archivo("doctores.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    int idBuscado;
    cout << "\nIngrese el ID del doctor a eliminar: ";
    cin >> idBuscado;

    Doctor d;
    bool encontrado = false;
    long posicionRegistro = -1;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long posicion = sizeof(ArchivoHeader) + i * sizeof(Doctor);
        archivo.seekg(posicion);
        archivo.read((char*)&d, sizeof(Doctor));

        if (!d.eliminado && d.id == idBuscado) {
            encontrado = true;
            posicionRegistro = posicion;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nNo se encontro ningun doctor con ese ID.\n";
        archivo.close();
        return;
    }

    cout << "\nDoctor encontrado:\n";
    cout << "Nombre: " << d.nombre << " " << d.apellido << endl;
    cout << "Especialidad: " << d.especialidad << endl;

    char confirmacion;
    cout << "\nEsta seguro que desea eliminar este doctor? (S/N): ";
    cin >> confirmacion;

    if (confirmacion == 'S' || confirmacion == 's') {
        d.eliminado = true;
        d.fechaModificacion = time(nullptr);

        archivo.seekp(posicionRegistro);
        archivo.write((char*)&d, sizeof(Doctor));

        // Actualizar header
        header.registrosActivos--;
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));

        cout << "\nDoctor eliminado correctamente.\n";
    } else {
        cout << "\nOperacion cancelada.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//========================================================= FUNCION PARA ASIGNAR PACIENTE A DOCTOR ======================================================================= 
//========================================================================================================================================================================

void asignarPacienteADoctor() {
    int idDoctor, idPaciente;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;
    cout << "Ingrese el ID del paciente: ";
    cin >> idPaciente;

    // Abrir doctores.bin
    fstream archivoDoctor("doctores.bin", ios::in | ios::out | ios::binary);
    if (!archivoDoctor.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader headerDoc;
    archivoDoctor.read((char*)&headerDoc, sizeof(ArchivoHeader));

    Doctor d;
    bool doctorEncontrado = false;
    long posicionDoctor = -1;

    for (int i = 0; i < headerDoc.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Doctor);
        archivoDoctor.seekg(pos);
        archivoDoctor.read((char*)&d, sizeof(Doctor));

        if (!d.eliminado && d.id == idDoctor) {
            doctorEncontrado = true;
            posicionDoctor = pos;
            break;
        }
    }

    if (!doctorEncontrado) {
        cout << "\nERROR: Doctor no encontrado.\n";
        archivoDoctor.close();
        return;
    }

    // Abrir pacientes.bin
    ifstream archivoPaciente("pacientes.bin", ios::binary);
    if (!archivoPaciente.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        archivoDoctor.close();
        return;
    }

    ArchivoHeader headerPac;
    archivoPaciente.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente p;
    bool pacienteEncontrado = false;

    for (int i = 0; i < headerPac.cantidadRegistros; i++) {
        archivoPaciente.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado && p.id == idPaciente) {
            pacienteEncontrado = true;
            break;
        }
    }

    archivoPaciente.close();

    if (!pacienteEncontrado) {
        cout << "\nERROR: Paciente no encontrado.\n";
        archivoDoctor.close();
        return;
    }

    // Verificar si ya está asignado
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            cout << "\nEste paciente ya esta asignado a este doctor.\n";
            archivoDoctor.close();
            return;
        }
    }

    // Verificar espacio disponible
    if (d.cantidadPacientes >= 50) {
        cout << "\nERROR: Este doctor ya tiene el maximo de pacientes asignados.\n";
        archivoDoctor.close();
        return;
    }

    // Asignar paciente
    d.pacientesIDs[d.cantidadPacientes++] = idPaciente;
    d.fechaModificacion = time(nullptr);

    archivoDoctor.seekp(posicionDoctor);
    archivoDoctor.write((char*)&d, sizeof(Doctor));

    cout << "\nPaciente asignado correctamente al doctor.\n";

    archivoDoctor.close();
}

//========================================================================================================================================================================
//===================================================== FUNCION PARA REMOVER PACIENTE DE DOCTOR ========================================================================== 
//========================================================================================================================================================================

void removerPacienteDeDoctor() {
    int idDoctor, idPaciente;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;
    cout << "Ingrese el ID del paciente a remover: ";
    cin >> idPaciente;

    fstream archivo("doctores.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Doctor d;
    bool encontrado = false;
    long posicionDoctor = -1;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Doctor);
        archivo.seekg(pos);
        archivo.read((char*)&d, sizeof(Doctor));

        if (!d.eliminado && d.id == idDoctor) {
            encontrado = true;
            posicionDoctor = pos;
            break;
        }
    }

    if (!encontrado) {
        cout << "\nERROR: Doctor no encontrado.\n";
        archivo.close();
        return;
    }

    // Buscar paciente en el array
    int indice = -1;
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        cout << "\nEste paciente no esta asignado a este doctor.\n";
        archivo.close();
        return;
    }

    // Remover paciente desplazando el array
    for (int i = indice; i < d.cantidadPacientes - 1; i++) {
        d.pacientesIDs[i] = d.pacientesIDs[i + 1];
    }
    d.pacientesIDs[d.cantidadPacientes - 1] = -1;
    d.cantidadPacientes--;
    d.fechaModificacion = time(nullptr);

    // Sobrescribir doctor actualizado
    archivo.seekp(posicionDoctor);
    archivo.write((char*)&d, sizeof(Doctor));

    cout << "\nPaciente removido correctamente del doctor.\n";

    archivo.close();
}

//========================================================================================================================================================================
//============================================ FUNCION PARA VER LISTA DE PACIENTES ASIGNADOS A DOCTORES ================================================================== 
//========================================================================================================================================================================

void verPacientesDeDoctor() {
    int idDoctor;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;

    // Buscar doctor
    ifstream archivoDoctor("doctores.bin", ios::binary);
    if (!archivoDoctor.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader headerDoc;
    archivoDoctor.read((char*)&headerDoc, sizeof(ArchivoHeader));

    Doctor d;
    bool encontrado = false;

    for (int i = 0; i < headerDoc.cantidadRegistros; i++) {
        archivoDoctor.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado && d.id == idDoctor) {
            encontrado = true;
            break;
        }
    }

    archivoDoctor.close();

    if (!encontrado) {
        cout << "\nERROR: Doctor no encontrado.\n";
        return;
    }

    if (d.cantidadPacientes == 0) {
        cout << "\nEste doctor no tiene pacientes asignados.\n";
        return;
    }

    // Mostrar pacientes
    ifstream archivoPaciente("pacientes.bin", ios::binary);
    if (!archivoPaciente.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerPac;
    archivoPaciente.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente p;
    cout << "\n+--------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Nombre y Apellido               | Cedula            | Edad | Sexo | Telefono                |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < d.cantidadPacientes; i++) {
        int idPaciente = d.pacientesIDs[i];
        archivoPaciente.seekg(sizeof(ArchivoHeader)); // Reiniciar búsqueda

        for (int j = 0; j < headerPac.cantidadRegistros; j++) {
            archivoPaciente.read((char*)&p, sizeof(Paciente));
            if (!p.eliminado && p.id == idPaciente) {
                cout << "| " << setw(2) << p.id << " | "
                     << setw(30) << p.nombre << " " << p.apellido << " | "
                     << setw(15) << p.cedula << " | "
                     << setw(4) << p.edad << " | "
                     << setw(4) << p.sexo << " | "
                     << setw(22) << p.telefono << " |" << endl;
                break;
            }
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------+" << endl;

    archivoPaciente.close();
}

//========================================================================================================================================================================







//========================================================================================================================================================================

//========================================================================================================================================================================
//================================================================= FUNCION PARA AGENDAR CITAS  ========================================================================== 
//========================================================================================================================================================================

void agendarCita() {
    int idPaciente, idDoctor;
    cout << "\nIngrese el ID del paciente: ";
    cin >> idPaciente;
    cout << "Ingrese el ID del doctor: ";
    cin >> idDoctor;

    // Verificar paciente
    fstream archivoPac("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!archivoPac.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerPac;
    archivoPac.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente paciente;
    bool pacienteEncontrado = false;
    long posPaciente = -1;

    for (int i = 0; i < headerPac.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivoPac.seekg(pos);
        archivoPac.read((char*)&paciente, sizeof(Paciente));
        if (!paciente.eliminado && paciente.id == idPaciente) {
            pacienteEncontrado = true;
            posPaciente = pos;
            break;
        }
    }

    if (!pacienteEncontrado) {
        cout << "\nERROR: Paciente no encontrado.\n";
        archivoPac.close();
        return;
    }

    // Verificar doctor
    fstream archivoDoc("doctores.bin", ios::in | ios::out | ios::binary);
    if (!archivoDoc.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        archivoPac.close();
        return;
    }

    ArchivoHeader headerDoc;
    archivoDoc.read((char*)&headerDoc, sizeof(ArchivoHeader));

    Doctor doctor;
    bool doctorEncontrado = false;
    long posDoctor = -1;

    for (int i = 0; i < headerDoc.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Doctor);
        archivoDoc.seekg(pos);
        archivoDoc.read((char*)&doctor, sizeof(Doctor));
        if (!doctor.eliminado && doctor.id == idDoctor) {
            doctorEncontrado = true;
            posDoctor = pos;
            break;
        }
    }

    if (!doctorEncontrado) {
        cout << "\nERROR: Doctor no encontrado.\n";
        archivoPac.close();
        archivoDoc.close();
        return;
    }

    // Crear nueva cita
    fstream archivoCita("citas.bin", ios::in | ios::out | ios::binary);
    if (!archivoCita.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        archivoPac.close();
        archivoDoc.close();
        return;
    }

    ArchivoHeader headerCita;
    archivoCita.read((char*)&headerCita, sizeof(ArchivoHeader));

    Cita cita = {};
    cita.id = headerCita.proximoID++;
    cita.pacienteID = idPaciente;
    cita.doctorID = idDoctor;
    cita.atendida = false;
    cita.consultaID = -1;
    cita.eliminado = false;
    cita.fechaCreacion = time(nullptr);
    cita.fechaModificacion = cita.fechaCreacion;

    cin.ignore();

    // Validar fecha
    bool fechaValida = false;
    do {
        cout << "Fecha (YYYY-MM-DD): ";
        cin.getline(cita.fecha, 11);

        int anio, mes, dia;
        if (sscanf(cita.fecha, "%4d-%2d-%2d", &anio, &mes, &dia) == 3) {
            if (anio >= 2025 && mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31) {
                fechaValida = true;
            } else {
                cout << "ERROR: Fecha invalida. Año = 2025, mes = 12, día = 31.\n";
            }
        } else {
            cout << "ERROR: Formato incorrecto. Use YYYY-MM-DD.\n";
        }
    } while (!fechaValida);

    // Validar hora
    bool horaValida = false;
    do {
        cout << "Hora (HH:MM): ";
        cin.getline(cita.hora, 6);

        int hora, minuto;
        if (sscanf(cita.hora, "%2d:%2d", &hora, &minuto) == 2) {
            if (hora >= 0 && hora <= 23 && minuto >= 0 && minuto <= 59) {
                horaValida = true;
            } else {
                cout << "ERROR: Hora inválida. Debe estar entre 00:00 y 23:59.\n";
            }
        } else {
            cout << "ERROR: Formato incorrecto. Use HH:MM.\n";
        }
    } while (!horaValida);

    cout << "Motivo de la cita: ";
    cin.getline(cita.motivo, 150);
    strcpy(cita.estado, "Agendada");
    strcpy(cita.observaciones, "");

    // Guardar cita
    archivoCita.seekp(0, ios::end);
    archivoCita.write((char*)&cita, sizeof(Cita));

    // Actualizar header de citas
    headerCita.cantidadRegistros++;
    headerCita.registrosActivos++;
    archivoCita.seekp(0);
    archivoCita.write((char*)&headerCita, sizeof(ArchivoHeader));
    archivoCita.close();

    // Actualizar paciente
    if (paciente.cantidadCitas < 20) {
        paciente.citasIDs[paciente.cantidadCitas++] = cita.id;
        paciente.fechaModificacion = time(nullptr);
        archivoPac.seekp(posPaciente);
        archivoPac.write((char*)&paciente, sizeof(Paciente));
    }

    // Actualizar doctor
    if (doctor.cantidadCitas < 30) {
        doctor.citasIDs[doctor.cantidadCitas++] = cita.id;
        doctor.fechaModificacion = time(nullptr);
        archivoDoc.seekp(posDoctor);
        archivoDoc.write((char*)&doctor, sizeof(Doctor));
    }

    cout << "\nCita agendada correctamente. ID de cita: " << cita.id << endl;

    archivoPac.close();
    archivoDoc.close();
}

//========================================================================================================================================================================
//============================================================ FUNCION PARA SUSPENDER CITAS ============================================================================== 
//========================================================================================================================================================================

void suspenderCita() {
    int idCita;
    cout << "\nIngrese el ID de la cita a suspender: ";
    cin >> idCita;

    fstream archivo("citas.bin", ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Cita cita;
    bool encontrada = false;
    long posicion = -1;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Cita);
        archivo.seekg(pos);
        archivo.read((char*)&cita, sizeof(Cita));

        if (!cita.eliminado && cita.id == idCita) {
            encontrada = true;
            posicion = pos;
            break;
        }
    }

    if (!encontrada) {
        cout << "\nERROR: No se encontro ninguna cita activa con ese ID.\n";
        archivo.close();
        return;
    }

    cout << "\nCita encontrada:\n";
    cout << "Fecha: " << cita.fecha << " | Hora: " << cita.hora << endl;
    cout << "Motivo: " << cita.motivo << endl;
    cout << "Estado actual: " << cita.estado << endl;

    char confirmacion;
    cout << "\nEsta seguro que desea suspender esta cita? (S/N): ";
    cin >> confirmacion;

    if (confirmacion == 'S' || confirmacion == 's') {
        cita.eliminado = true;
        cita.fechaModificacion = time(nullptr);
        strcpy(cita.estado, "Cancelada");

        archivo.seekp(posicion);
        archivo.write((char*)&cita, sizeof(Cita));

        // Actualizar header
        header.registrosActivos--;
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));

        cout << "\nCita suspendida correctamente.\n";
    } else {
        cout << "\nOperacion cancelada.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//============================================================ FUNCION PARA ATENDER CITA ============================================================================== 
//========================================================================================================================================================================

void atenderCita() {
    fstream archivoCitas("citas.bin", ios::in | ios::out | ios::binary);
    if (!archivoCitas.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader headerCitas;
    archivoCitas.read((char*)&headerCitas, sizeof(ArchivoHeader));

    int idCita;
    cout << "\nIngrese el ID de la cita a atender: ";
    cin >> idCita;

    Cita cita;
    long posicionCita = -1;
    bool encontrada = false;

    for (int i = 0; i < headerCitas.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Cita);
        archivoCitas.seekg(pos);
        archivoCitas.read((char*)&cita, sizeof(Cita));

        if (!cita.eliminado && cita.id == idCita && !cita.atendida) {
            posicionCita = pos;
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        cout << "\nERROR: No se encontro una cita valida con ese ID.\n";
        archivoCitas.close();
        return;
    }

    // Mostrar cita
    cout << "\nCita encontrada:\n";
    cout << "PacienteID: " << cita.pacienteID << " | DoctorID: " << cita.doctorID << endl;
    cout << "Fecha: " << cita.fecha << " | Hora: " << cita.hora << endl;
    cout << "Motivo: " << cita.motivo << endl;

    char confirmar;
    cout << "\n¿Desea atender esta cita? (S/N): ";
    cin >> confirmar;
    if (confirmar != 'S' && confirmar != 's') {
        cout << "\nOperación cancelada.\n";
        archivoCitas.close();
        return;
    }

    // Crear historial médico
    fstream archivoHistorial("historiales.bin", ios::in | ios::out | ios::binary);
    if (!archivoHistorial.is_open()) {
        cerr << "ERROR: No se pudo abrir historiales.bin\n";
        archivoCitas.close();
        return;
    }

    ArchivoHeader headerHist;
    archivoHistorial.read((char*)&headerHist, sizeof(ArchivoHeader));

    HistorialMedico consulta = {};
    consulta.id = headerHist.proximoID++;
    consulta.pacienteID = cita.pacienteID;
    consulta.doctorID = cita.doctorID;
    strcpy(consulta.fecha, cita.fecha);
    strcpy(consulta.hora, cita.hora);
    consulta.fechaRegistro = time(nullptr);
    consulta.eliminado = false;
    consulta.siguienteConsultaID = -1;

    cin.ignore();
    
        cout<<"\t\t+----------------------------------------+"<<endl;
        cout<<"\t\t+       REGISTRO DE CONSULTA MEDICA      +"<<endl;
        cout<<"\t\t+----------------------------------------+"<<endl;

    cout << "Diagnostico: "; cin.getline(consulta.diagnostico, 200);
    cout << "Tratamiento: "; cin.getline(consulta.tratamiento, 200);
    cout << "Medicamentos: "; cin.getline(consulta.medicamentos, 150);
    cout << "Costo de consulta: "; cin >> consulta.costo;

    // Guardar historial
    archivoHistorial.seekp(0, ios::end);
    archivoHistorial.write((char*)&consulta, sizeof(HistorialMedico));

    headerHist.cantidadRegistros++;
    headerHist.registrosActivos++;
    archivoHistorial.seekp(0);
    archivoHistorial.write((char*)&headerHist, sizeof(ArchivoHeader));
    archivoHistorial.close();

    // Actualizar cita
    cita.atendida = true;
    cita.consultaID = consulta.id;
    strcpy(cita.estado, "Atendida");
    cita.fechaModificacion = time(nullptr);

    archivoCitas.seekp(posicionCita);
    archivoCitas.write((char*)&cita, sizeof(Cita));
    archivoCitas.close();

    // Actualizar paciente
    fstream archivoPac("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!archivoPac.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerPac;
    archivoPac.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente p;
    long posicionPaciente = -1;

    for (int i = 0; i < headerPac.cantidadRegistros; i++) {
        long pos = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivoPac.seekg(pos);
        archivoPac.read((char*)&p, sizeof(Paciente));

        if (!p.eliminado && p.id == cita.pacienteID) {
            posicionPaciente = pos;

            // Enlazar historial
            if (p.primerConsultaID == -1) {
                p.primerConsultaID = consulta.id;
            } else {
                // Enlazar al final del historial
                fstream archivoHist("historiales.bin", ios::in | ios::out | ios::binary);
                archivoHist.read((char*)&headerHist, sizeof(ArchivoHeader));
                HistorialMedico h;
                for (int j = 0; j < headerHist.cantidadRegistros; j++) {
                    long posHist = sizeof(ArchivoHeader) + j * sizeof(HistorialMedico);
                    archivoHist.seekg(posHist);
                    archivoHist.read((char*)&h, sizeof(HistorialMedico));
                    if (!h.eliminado && h.pacienteID == p.id && h.siguienteConsultaID == -1) {
                        h.siguienteConsultaID = consulta.id;
                        archivoHist.seekp(posHist);
                        archivoHist.write((char*)&h, sizeof(HistorialMedico));
                        break;
                    }
                }
                archivoHist.close();
            }

            p.cantidadConsultas++;
            p.fechaModificacion = time(nullptr);

            archivoPac.seekp(posicionPaciente);
            archivoPac.write((char*)&p, sizeof(Paciente));
            break;
        }
    }

    archivoPac.close();

    cout << "\nCita atendida y consulta registrada con ID: " << consulta.id << endl;
}

//========================================================================================================================================================================
//============================================ FUNCION PARA VER TODAS LAS CITAS DE UN PACIENTE =========================================================================== 
//========================================================================================================================================================================

void verCitasDePaciente() {
    int idPaciente;
    cout << "\nIngrese el ID del paciente: ";
    cin >> idPaciente;

    // Buscar paciente
    ifstream archivoPac("pacientes.bin", ios::binary);
    if (!archivoPac.is_open()) {
        cerr << "ERROR: No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerPac;
    archivoPac.read((char*)&headerPac, sizeof(ArchivoHeader));

    Paciente p;
    bool encontrado = false;

    for (int i = 0; i < headerPac.cantidadRegistros; i++) {
        archivoPac.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado && p.id == idPaciente) {
            encontrado = true;
            break;
        }
    }

    archivoPac.close();

    if (!encontrado) {
        cout << "\nERROR: Paciente no encontrado.\n";
        return;
    }

    if (p.cantidadCitas == 0) {
        cout << "\nEste paciente no tiene citas registradas.\n";
        return;
    }

    // Mostrar citas
    ifstream archivoCita("citas.bin", ios::binary);
    if (!archivoCita.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader headerCita;
    archivoCita.read((char*)&headerCita, sizeof(ArchivoHeader));

    Cita c;
    cout << "\n+--------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Fecha       | Hora  | DoctorID | Estado     | Motivo de la cita                                       |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < p.cantidadCitas; i++) {
        int citaID = p.citasIDs[i];
        archivoCita.seekg(sizeof(ArchivoHeader)); // Reiniciar búsqueda

        for (int j = 0; j < headerCita.cantidadRegistros; j++) {
            archivoCita.read((char*)&c, sizeof(Cita));
            if (!c.eliminado && c.id == citaID) {
                cout << "| " << setw(2) << c.id << " | "
                     << setw(11) << c.fecha << " | "
                     << setw(6) << c.hora << " | "
                     << setw(8) << c.doctorID << " | "
                     << setw(10) << c.estado << " | "
                     << setw(50) << c.motivo << " |" << endl;
                break;
            }
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    archivoCita.close();
}

//========================================================================================================================================================================
//============================================ FUNCION PARA VER TODAS LAS CITAS DE UN DOCTORR =========================================================================== 
//========================================================================================================================================================================

void verCitasDeDoctor() {
    int idDoctor;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;

    // Buscar doctor
    ifstream archivoDoc("doctores.bin", ios::binary);
    if (!archivoDoc.is_open()) {
        cerr << "ERROR: No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader headerDoc;
    archivoDoc.read((char*)&headerDoc, sizeof(ArchivoHeader));

    Doctor d;
    bool encontrado = false;

    for (int i = 0; i < headerDoc.cantidadRegistros; i++) {
        archivoDoc.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado && d.id == idDoctor) {
            encontrado = true;
            break;
        }
    }

    archivoDoc.close();

    if (!encontrado) {
        cout << "\nERROR: Doctor no encontrado.\n";
        return;
    }

    if (d.cantidadCitas == 0) {
        cout << "\nEste doctor no tiene citas registradas.\n";
        return;
    }

    // Mostrar citas
    ifstream archivoCita("citas.bin", ios::binary);
    if (!archivoCita.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader headerCita;
    archivoCita.read((char*)&headerCita, sizeof(ArchivoHeader));

    Cita c;
    cout << "\n+--------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Fecha       | Hora  | PacienteID | Estado     | Motivo de la cita                                       |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < d.cantidadCitas; i++) {
        int citaID = d.citasIDs[i];
        archivoCita.seekg(sizeof(ArchivoHeader)); // Reiniciar búsqueda

        for (int j = 0; j < headerCita.cantidadRegistros; j++) {
            archivoCita.read((char*)&c, sizeof(Cita));
            if (!c.eliminado && c.id == citaID) {
                cout << "| " << setw(2) << c.id << " | "
                     << setw(11) << c.fecha << " | "
                     << setw(6) << c.hora << " | "
                     << setw(10) << c.pacienteID << " | "
                     << setw(10) << c.estado << " | "
                     << setw(50) << c.motivo << " |" << endl;
                break;
            }
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    archivoCita.close();
}

//========================================================================================================================================================================
//=========================================================== FUNCION PARA VER TODAS DE UN DIA =========================================================================== 
//========================================================================================================================================================================

void verCitasPorFecha() {
    char fechaBuscada[11];
    cin.ignore();
    cout << "\nIngrese la fecha a consultar (YYYY-MM-DD): ";
    cin.getline(fechaBuscada, 11);

    // Validar formato y rango
    int anio, mes, dia;
    if (sscanf(fechaBuscada, "%4d-%2d-%2d", &anio, &mes, &dia) != 3 ||
        anio < 2025 || mes < 1 || mes > 12 || dia < 1 || dia > 31) {
        cout << "ERROR: Fecha invalida. Use formato YYYY-MM-DD y asegurese de que sea valida.\n";
        return;
    }

    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Cita c;
    bool encontrado = false;

    cout << "\n+--------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Fecha       | Hora  | PacienteID | DoctorID | Estado     | Motivo de la cita                           |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (!c.eliminado && strcmp(c.fecha, fechaBuscada) == 0) {
            encontrado = true;
            cout << "| " << setw(2) << c.id << " | "
                 << setw(11) << c.fecha << " | "
                 << setw(6) << c.hora << " | "
                 << setw(10) << c.pacienteID << " | "
                 << setw(9) << c.doctorID << " | "
                 << setw(10) << c.estado << " | "
                 << setw(45) << c.motivo << " |" << endl;
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    if (!encontrado) {
        cout << "\nNo se encontraron citas para la fecha indicada.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//============================================ FUNCION PARA IMPRIMIR TODAS LAS CITAS QUE ESTAN PENDIENTES ================================================================ 
//========================================================================================================================================================================

void verCitasPendientes() {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Cita c;
    bool hayPendientes = false;

    cout << "\n+--------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Fecha       | Hora  | PacienteID | DoctorID | Estado     | Motivo de la cita                           |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (!c.eliminado && strcmp(c.estado, "Agendada") == 0) {
            hayPendientes = true;
            cout << "| " << setw(2) << c.id << " | "
                 << setw(11) << c.fecha << " | "
                 << setw(6) << c.hora << " | "
                 << setw(10) << c.pacienteID << " | "
                 << setw(9) << c.doctorID << " | "
                 << setw(10) << c.estado << " | "
                 << setw(45) << c.motivo << " |" << endl;
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    if (!hayPendientes) {
        cout << "\nNo hay citas pendientes registradas.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================







//========================================================================================================================================================================

//========================================================================================================================================================================
//========================================================== FUNCION PARA VER CONSULTAS DE CADA DOCTOR ==========================================================================================       
//========================================================================================================================================================================

void verConsultasPorDoctor() {
    int idDoctor;
    cout << "\nIngrese el ID del doctor: ";
    cin >> idDoctor;

    ifstream archivo("historiales.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir historiales.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    HistorialMedico consulta;
    bool encontrado = false;

    cout << "\n+--------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID | Fecha       | Hora  | PacienteID | Diagnostico                                      | Tratamiento       |" << endl;
    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&consulta, sizeof(HistorialMedico));
        if (!consulta.eliminado && consulta.doctorID == idDoctor) {
            encontrado = true;
            cout << "| " << setw(2) << consulta.id << " | "
                 << setw(11) << consulta.fecha << " | "
                 << setw(6) << consulta.hora << " | "
                 << setw(10) << consulta.pacienteID << " | "
                 << setw(45) << consulta.diagnostico << " | "
                 << setw(18) << consulta.tratamiento << " |" << endl;
        }
    }

    cout << "+--------------------------------------------------------------------------------------------------------------+" << endl;

    if (!encontrado) {
        cout << "\nNo se encontraron consultas para este doctor.\n";
    }

    archivo.close();
}

//========================================================================================================================================================================
//========================================================== FUNCION PARA REPORTAR CITA POR ESTADO =============================================================================       
//========================================================================================================================================================================

void reporteCitasPorEstado() {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "ERROR: No se pudo abrir citas.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    int agendadas = 0, atendidas = 0, canceladas = 0;
    Cita cita;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&cita, sizeof(Cita));
        if (cita.eliminado) continue;

        if (strcmp(cita.estado, "Agendada") == 0) agendadas++;
        else if (strcmp(cita.estado, "Atendida") == 0) atendidas++;
        else if (strcmp(cita.estado, "Cancelada") == 0) canceladas++;
    }

    cout<<"\t\t+----------------------------------------+"<<endl;
    cout<<"\t\t+   	REPORTE DE CITAS POR ESTADO         +"<<endl;
    cout<<"\t\t+----------------------------------------+"<<endl;

    cout << "Total de citas registradas: " << (agendadas + atendidas + canceladas) << endl;
    cout << " - Agendadas: " << agendadas << endl;
    cout << " - Atendidas: " << atendidas << endl;
    cout << " - Canceladas: " << canceladas << endl;

    archivo.close();
}



//========================================================================================================================================================================
//============================================================= FUNCION PARA VERIFICAR LA INTEGRIDAD DE LOS ARCHIVOS ========================================================================       
//========================================================================================================================================================================

bool verificarIntegridadReferencial() {
    bool todoCorrecto = true;

    // Verificar citas
    ifstream citas("citas.bin", ios::binary);
    ifstream pacientes("pacientes.bin", ios::binary);
    ifstream doctores("doctores.bin", ios::binary);
    ifstream historiales("historiales.bin", ios::binary);

    if (!citas.is_open() || !pacientes.is_open() || !doctores.is_open() || !historiales.is_open()) {
        cerr << "ERROR: No se pudieron abrir todos los archivos necesarios.\n";
        return false;
    }

    ArchivoHeader headerCitas, headerPac, headerDoc, headerHist;
    citas.read((char*)&headerCitas, sizeof(ArchivoHeader));
    pacientes.read((char*)&headerPac, sizeof(ArchivoHeader));
    doctores.read((char*)&headerDoc, sizeof(ArchivoHeader));
    historiales.read((char*)&headerHist, sizeof(ArchivoHeader));

    Cita c;
    for (int i = 0; i < headerCitas.cantidadRegistros; i++) {
        citas.read((char*)&c, sizeof(Cita));
        if (c.eliminado) continue;

        bool pacienteValido = false, doctorValido = false;

        Paciente p;
        pacientes.seekg(sizeof(ArchivoHeader));
        for (int j = 0; j < headerPac.cantidadRegistros; j++) {
            pacientes.read((char*)&p, sizeof(Paciente));
            if (!p.eliminado && p.id == c.pacienteID) {
                pacienteValido = true;
                break;
            }
        }

        Doctor d;
        doctores.seekg(sizeof(ArchivoHeader));
        for (int j = 0; j < headerDoc.cantidadRegistros; j++) {
            doctores.read((char*)&d, sizeof(Doctor));
            if (!d.eliminado && d.id == c.doctorID) {
                doctorValido = true;
                break;
            }
        }

        if (!pacienteValido || !doctorValido) {
            cout << "Referencia rota en cita ID " << c.id << endl;
            todoCorrecto = false;
        }
    }

    citas.close(); pacientes.close(); doctores.close(); historiales.close();
    return todoCorrecto;
}

//========================================================================================================================================================================
//============================================================= FUNCION PARA COMPACTAR ARCHIVO ========================================================================       
//========================================================================================================================================================================

bool compactarArchivoPacientes() {
    ifstream original("pacientes.bin", ios::binary);
    ofstream temporal("pacientes_temp.bin", ios::binary);

    if (!original.is_open() || !temporal.is_open()) {
        cerr << "ERROR: No se pudo abrir archivos para compactar.\n";
        return false;
    }

    ArchivoHeader header;
    original.read((char*)&header, sizeof(ArchivoHeader));

    ArchivoHeader nuevoHeader = {0, 1, 0, header.version};
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    Paciente p;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        original.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado) {
            p.id = nuevoHeader.proximoID++;
            temporal.write((char*)&p, sizeof(Paciente));
            nuevoHeader.cantidadRegistros++;
            nuevoHeader.registrosActivos++;
        }
    }

    temporal.seekp(0);
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    original.close();
    temporal.close();

    remove("pacientes.bin");
    rename("pacientes_temp.bin", "pacientes.bin");

    cout << "Archivo compactado exitosamente.\n";
    return true;
}

//========================================================================================================================================================================
//============================================================= FUNCION PARA HACER RESPANDO DE LOS DATOS ========================================================================       
//========================================================================================================================================================================

bool hacerRespaldo() {
    const char* archivos[] = {"pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin", "hospital.bin"};
    const char* respaldo[] = {"pacientes.bak", "doctores.bak", "citas.bak", "historiales.bak", "hospital.bak"};

    for (int i = 0; i < 5; i++) {
        ifstream origen(archivos[i], ios::binary);
        ofstream destino(respaldo[i], ios::binary);

        if (!origen.is_open() || !destino.is_open()) {
            cerr << "ERROR: No se pudo respaldar " << archivos[i] << endl;
            return false;
        }

        destino << origen.rdbuf();
        origen.close();
        destino.close();
    }

    cout << "Respaldo completado.\n";
    return true;
}

//========================================================================================================================================================================
//============================================================= FUNCION PARA RESTAURAR DESDE RESPALDO ========================================================================       
//========================================================================================================================================================================

bool restaurarDesdeRespaldo() {
    const char* archivos[] = {"pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin", "hospital.bin"};
    const char* respaldo[] = {"pacientes.bak", "doctores.bak", "citas.bak", "historiales.bak", "hospital.bak"};

    for (int i = 0; i < 5; i++) {
        ifstream origen(respaldo[i], ios::binary);
        ofstream destino(archivos[i], ios::binary);

        if (!origen.is_open() || !destino.is_open()) {
            cerr << "ERROR: No se pudo restaurar " << respaldo[i] << endl;
            return false;
        }

        destino << origen.rdbuf();
        origen.close();
        destino.close();
    }

    cout << "Restauración completada.\n";
    return true;
}

//========================================================================================================================================================================
//============================================================= FUNCION PARA INICIALIZAR ARCHIVOS ========================================================================       
//========================================================================================================================================================================

void mostrarEstadisticasArchivos() {
    const char* archivos[] = {"pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin"};
    const char* nombres[] = {"Pacientes", "Doctores", "Citas", "Historiales"};

    for (int i = 0; i < 4; i++) {
        ifstream archivo(archivos[i], ios::binary);
        if (!archivo.is_open()) {
            cerr << "ERROR: No se pudo abrir " << archivos[i] << endl;
            continue;
        }

        ArchivoHeader header;
        archivo.read((char*)&header, sizeof(ArchivoHeader));
        archivo.close();

        cout << nombres[i] << ":\n";
        cout << "  Total registros: " << header.cantidadRegistros << endl;
        cout << "  Activos: " << header.registrosActivos << endl;
        cout << "  Eliminados: " << (header.cantidadRegistros - header.registrosActivos) << endl;
        cout << "  Próximo ID: " << header.proximoID << "\n\n";
    }
}







//========================================================================================================================================================================
//============================================================= FUNCION PARA INICIALIZAR ARCHIVOS ========================================================================       
//========================================================================================================================================================================

void inicializarArchivo(const char* nombreArchivo) {
    ifstream prueba(nombreArchivo, ios::binary);
    if (!prueba.good()) {
        ofstream nuevo(nombreArchivo, ios::binary);
        if (!nuevo.is_open()) {
            cerr << "ERROR: No se pudo crear " << nombreArchivo << endl;
            return;
        }

        ArchivoHeader header = {};
        header.cantidadRegistros = 0;
        header.proximoID = 1;
        header.registrosActivos = 0;
        header.version = 1;

        nuevo.write((char*)&header, sizeof(ArchivoHeader));
        nuevo.close();

        cout << "Archivo creado: " << nombreArchivo << endl;
    } else {
        prueba.close();
    }
}

void inicializarArchivos() {
    inicializarArchivo("pacientes.bin");
    inicializarArchivo("doctores.bin");
    inicializarArchivo("citas.bin");
    inicializarArchivo("historiales.bin");
}

//========================================================================================================================================================================
//========================================================== PROGRAMA PRINCIPAL ==========================================================================================       
//========================================================================================================================================================================

int main() {
    int wOpcionIMP;
    int wOpcionMP;
    int wOpcionMAC;
    int wOpcionMD;
    int wOpcionMA;
    int wOpcionMCR;
    
    inicializarArchivos();
    
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
    cout<<"\t\t+  4. Consultas y Reportes               +"<<endl;
    cout<<"\t\t+  5. Mantenimiento de Archivos          +"<<endl;
    cout<<"\t\t+  6. Guardar y Salir                    +"<<endl;
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
                                INGRESO_NUEVOS_PACIENTES();
                                system("pause");
                                break;
                            case 2:
                                buscarPacientePorCedula();
                                system("pause");
                                break;
                            case 3:
                                buscarPacientePorNombre();
                                system("pause");
                                break;
                            case 4:
                                verHistorialMedico();
                                system("pause");
                                break;
                            case 5:
                                actualizarDatosPaciente();
                                system("pause");
                                break;
                            case 6:
                                verListaPacientes();
                                system("pause");
                                break;
                            case 7:
                                eliminarPacientePorCedula ();
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
                                        INGRESO_NUEVO_DOCTOR();
                                        system("pause");
                                        break;
                                    case 2:
                                        buscarDoctorPorID();
                                        system("pause");
                                        break;
                                    case 3:
                                        buscarDoctorPorEspecialidad();
                                        system("pause");
                                        break;
                                    case 4: 
        								asignarPacienteADoctor();
        								system("pause");
										break;
    								case 5: 
    									removerPacienteDeDoctor();
        							    system("pause");
										break;
    								case 6: 
										verPacientesDeDoctor();
										system("pause");
										break;
                                    case 7:
                                        verListaDoctores();
                                        system("pause");
                                        break;
                                    case 8:
                                        eliminarDoctorPorID();
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
                                        agendarCita();
                                        system("pause");
                                        break;
                                    case 2:
                                        suspenderCita();
                                        system("pause");
                                        break;
                                    case 3:
 									   atenderCita();
 									   system("pause");
 									   break;
                                    case 4:
                                        verCitasDePaciente();
                                        system("pause");
                                        break;
                                    case 5:
                                        verCitasDeDoctor();
                                        system("pause");
                                        break;
                                    case 6:
                                        verCitasPorFecha();
                                        system("pause");
                                        break;
                                    case 7:
                                        verCitasPendientes();
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
            	do{
            		system("cls");    
        	    		cout<<"\t\t+--------------------------------------------+"<<endl;
    	        		cout<<"\t\t+          CONSULTAS Y REPORTES              +"<<endl;
    	        		cout<<"\t\t+--------------------------------------------+"<<endl;
	            		cout<<"\t\t+         SELECCIONE UNA OPCION              +"<<endl;
            			cout<<"\t\t+--------------------------------------------+"<<endl;
        	    		cout<<"\t\t+  1. Ver historial médico de un paciente    +"<<endl;
  	        			cout<<"\t\t+  2. Consultar el historial de un doctor    +"<<endl;
    	        		cout<<"\t\t+  3. Reporte de citas                       +"<<endl;
            			cout<<"\t\t+  0. Volver a menu principal                +"<<endl;
            			cout<<"\t\t+--------------------------------------------+"<<endl;

                		cin>>wOpcionMCR;
                
                    		system("cls");
                        		switch(wOpcionMCR){
                            	case 1:
                                		verHistorialMedico();
                                		system("pause");
                                	break;
                            	case 2:
                                		verConsultasPorDoctor();
                                		system("pause");
                                	break;
                            	case 3:
                                		reporteCitasPorEstado();
                                		system("pause");
                                	break;
                            	default:
                                		cout << "ERROR!: Opcion invalida (0-7)" << endl;
                                		system("pause");
                                	break;
                				}
       				}while (wOpcionMCR != 0);
     				break;
            
          	case 5:
            
        	do{
            system("cls");    
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+        MANTENIMIENTO DE ARCHIVOS       +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+         SELECCIONE UNA OPCION          +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;
            cout<<"\t\t+  1. Verificar integridad de archivo    +"<<endl;
            cout<<"\t\t+  2. Compactar archivos                 +"<<endl;
            cout<<"\t\t+  3. Hacer respaldo de datos            +"<<endl;
            cout<<"\t\t+  4. Restaurar desde respaldo           +"<<endl;
            cout<<"\t\t+  5. Estadísticas de uso de archivos    +"<<endl;
            cout<<"\t\t+  0. Volver a menu principal            +"<<endl;
            cout<<"\t\t+----------------------------------------+"<<endl;

                cin>>wOpcionMA;
                
                    system("cls");
                        switch(wOpcionMA){
                            case 1:
                                verificarIntegridadReferencial();
                                system("pause");
                                break;
                            case 2:
                                compactarArchivoPacientes();
                                system("pause");
                                break;
                            case 3:
                                 hacerRespaldo();
                                system("pause");
                                break;
                            case 4:
                                restaurarDesdeRespaldo();
                                system("pause");
                                break;
                            case 5:
                                mostrarEstadisticasArchivos();
                                system("pause");
                                break;
                            default:
                                cout << "ERROR!: Opcion invalida (0-7)" << endl;
                                system("pause");
                                break;
                }
            }while (wOpcionMA != 0);
            break;
            
            case 6:
                cout << "Saliendo del programa... Vuelva PRonto!" << endl;
                system("pause");
                break;
            default:
                cout << "ERROR!: Opcion invalida (1-6)" << endl;
                system("pause");
                break;
        }
    }while(wOpcionIMP != 6);
    
    return 0;
}
