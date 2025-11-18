## 🏥 Sistema de Gestión Hospitalaria en C++
Este proyecto implementa un sistema modular en C++ para la gestión de pacientes, médicos y citas médicas en un entorno hospitalario. Fue desarrollado como parte de la formación en Ingeniería en Computación en la Universidad Rafael Urdaneta.

## ✅ Actualización reciente
Se ha realizado una mejora significativa en el manejo de datos del sistema. En uno de los módulos principales, se eliminaron los arreglos dinámicos (new y delete) y se reemplazaron por estructuras basadas en archivos binarios. Esto garantiza que la información ingresada por el usuario se conserve entre ejecuciones del programa, evitando la pérdida de datos tras cada compilación.

## 📌 Objetivos del sistema
🔒 Beneficios de la actualización
- Persistencia de datos mediante almacenamiento en ficheros.
- Mayor robustez y confiabilidad del sistema.
- Reducción de errores relacionados con la gestión dinámica de memoria.
- Preparación para futuras funciones como respaldo y restauración.

## 🧩 Estructura del proyecto
El sistema se organiza en módulos funcionales:

- **Pacientes**: Alta, baja, modificación, historial médico.
- **Doctores**: Registro, disponibilidad, especialidad.
- **Citas**: Agendamiento, cancelación, suspensión, atención.
- **Informes**: Registro de citas, pacientes, tratamientos, estadísticas.

## 🛠️ Tecnologías utilizadas
Gestión de datos mediante archivos binarios

- Lenguaje: C++
- IDE sugerido: Code::Blocks o Dev-C++
- Paradigma: Programación estructurada
- Gestión dinámica de memoria con `new` y `delete`
- Validación de entradas y control de errores
Validación de entradas con control de errores


## 👨‍💻 Autor
Rubén Adarme
Estudiante de Ingeniería de Computación Universidad Rafael Urdaneta | Venezuela
