@@ -1,32 +1,46 @@
# 🏥 Sistema de Gestión Hospitalaria en C++
🏥 Sistema de Gestión Hospitalaria en C++ (Actualización)
Este proyecto implementa un sistema modular en C++ para la gestión de pacientes, médicos y citas médicas en un entorno hospitalario. Fue desarrollado como parte de la formación en Ingeniería en Computación en la Universidad Rafael Urdaneta.

Este proyecto implementa un sistema modular en C++ para la gestión de pacientes, doctores y citas médicas en un entorno hospitalario. Fue desarrollado como parte de la formación en Ingeniería en Computación en la Universidad Rafael Urdaneta.
✅ Actualización reciente
Se ha realizado una mejora significativa en el manejo de datos del sistema. En uno de los módulos principales, se eliminaron los arreglos dinámicos (new y delete) y se reemplazaron por estructuras basadas en archivos binarios. Esto garantiza que la información ingresada por el usuario se conserve entre ejecuciones del programa, evitando la pérdida de datos tras cada compilación.

# 📌 Objetivos del sistema
🔒 Beneficios de la actualización
Persistencia de datos mediante almacenamiento en ficheros.

- Registrar y administrar pacientes y doctores.
- Agendar, cancelar, suspender y atender citas médicas.
- Generar historial médico por paciente.
- Visualizar citas por fecha, por doctor, por paciente o por pendientes.
- Aplicar buenas prácticas de programación estructurada y modular.
Mayor robustez y confiabilidad del sistema.

# 🧩 Estructura del proyecto
Reducción de errores relacionados con la gestión dinámica de memoria.

🎯 Objetivos del sistema
Registrar y administrar pacientes y doctores.

Asignar citas médicas y atender a los mismos.

Visualizar información relevante de cada módulo por separado.

Establecer una estructura modular que permita expandir el proyecto.

🧩 Estructura del proyecto
El sistema se organiza en módulos funcionales:

- **Pacientes**: Alta, baja, modificación, historial médico.
- **Doctores**: Registro, disponibilidad, especialidad.
- **Citas**: Agendamiento, cancelación, suspensión, atención.
- **Historial médico**: Registro de diagnósticos, tratamientos y medicamentos.
Pacientes: Alta, baja, modificación, historial médico.

Médicos: Alta, baja, modificación, historial médico.

Citas: Agendamiento, modificación, atención, tratamiento.

Informes: Registro de citas, pacientes, tratamientos, estadísticas.

🛠️ Tecnologías utilizadas
Lenguaje de programación: C++

IDE sugerido: Code::Blocks o Dev-C++

Paradigma: Programación estructurada

## 🛠️ Tecnologías utilizadas
Gestión de datos mediante archivos binarios

- Lenguaje: C++
- IDE sugerido: Code::Blocks o Dev-C++
- Paradigma: Programación estructurada
- Gestión dinámica de memoria con `new` y `delete`
- Validación de entradas y control de errores
Validación de entradas con control de errores

## 🧠 Autor
Rubén Adarme 
Estudiante de Ingeniería en Computación Universidad Rafael Urdaneta 📍 Venezuela
👨‍💻 Autor
Rubén Adameer Estudiante de Ingeniería en Computación Universidad Rafael Urdaneta | Venezuela
