# Manual Técnico - EDD CinemaUSAC (Fase 3)

**Universidad de San Carlos de Guatemala**

**Facultad de Ingeniería — Escuela de Ciencias y Sistemas**

**Curso:** Estructuras de Datos

**Proyecto:** EDD CinemaUSAC — Fase 3

**Carné:** 202303371

## 1. Introducción
El presente manual técnico detalla la arquitectura, estructuras de datos y algoritmos implementados en la **Fase 3** del sistema informático "CinemaUSAC". El proyecto está desarrollado en el lenguaje **C++** nativo, utilizando el framework **Qt** para el diseño de la interfaz gráfica y **Graphviz** para la generación de reportes visuales.

El objetivo principal de esta fase es dotar al sistema de componentes avanzados que garanticen persistencia eficiente, auditoría y relaciones complejas entre los datos, sin utilizar bibliotecas de terceros para la lógica estructural.

## 2. Arquitectura del Sistema
El sistema emplea una arquitectura monolítica modular, donde la lógica de negocio (estructuras de datos) está estrictamente separada de la capa de presentación (GUI en Qt). 

### 2.1 Tecnologías y Librerías Utilizadas
*   **Lenguaje Base:** C++17
*   **Framework UI:** Qt (QtWidgets)
*   **Criptografía:** `QCryptographicHash` (Qt Core) para SHA-256.
*   **Manejo JSON:** `QJsonDocument`, `QJsonObject`, `QJsonArray` (Qt Core).
*   **Generación de Reportes:** Graphviz (Llamadas al sistema mediante el comando `dot`).

---

## 3. Estructuras de Datos y Algoritmos (Fase 3)

### 3.1 Grafo No Dirigido Ponderado (Lista de Adyacencia)
Para modelar la red de sedes de cine, se implementó un Grafo No Dirigido utilizando listas de adyacencia dinámicas mediante punteros.

*   **Vértices (`VerticeSede`):** Cada nodo representa una sede física del cine.
*   **Aristas (`Arista`):** Cada conexión representa que dos sedes comparten al menos una película en su cartelera.
*   **Ponderación (Peso):** El peso de la arista es un entero (`int`) que representa la cantidad exacta de películas únicas que ambas sedes comparten.
*   **Complejidad:** La inserción y actualización es de $O(V + E)$. Se ejecuta un recálculo dinámico automático cada vez que el administrador crea, edita o elimina una función.

### 3.2 Búsqueda de Camino Mínimo (BFS - Breadth-First Search)
Para recomendar a los administradores la ruta más corta entre dos sedes en función de su afinidad de cartelera, se implementó el algoritmo BFS.

*   **Lógica:** Al no basarse en la distancia física o en minimizar los pesos (sino en minimizar la cantidad de *saltos* entre sedes afines), se utiliza BFS estándar en lugar de Dijkstra.
*   **Implementación:** Se utiliza un `std::queue` para el recorrido por niveles y un `std::map` para marcar los nodos ya visitados, evitando ciclos infinitos.

### 3.3 Blockchain y Criptografía (Proof of Work)
La seguridad e inmutabilidad de las transacciones financieras (ventas de boletos) se maneja a través de una cadena de bloques construida desde cero.

*   **Estructura del Bloque:** Cada nodo de la cadena (`Bloque`) guarda un índice, *timestamp*, un arreglo de transacciones (`DATA`), el hash del bloque anterior (`PREVIOUSHASH`), la raíz de Merkle (`ROOTMERKLE`) y el `nonce`.
*   **Proof of Work (PoW):** Al minar un bloque, el sistema entra en un ciclo iterativo donde concatena la cabecera e incrementa el valor de `nonce` progresivamente, aplicando el algoritmo **SHA-256**. El ciclo termina cuando el hash resultante contiene un prefijo específico establecido en el sistema (`0000`).
*   **Auditoría Interna:** Durante el arranque del programa, la Blockchain lee la carpeta `/blocks/`. Si se detecta una asimetría entre el `ROOTMERKLE` guardado en el archivo y el árbol calculado a partir de la `DATA` en crudo, el sistema aisla el bloque y lo cataloga como *corrupto* (visualmente en rojo).

### 3.4 Árbol Binario de Merkle
Para resumir la información del bloque y asegurar la integridad de las transacciones, se emplea un Árbol de Merkle por cada bloque minado.

*   **Construcción:** Cada transacción se convierte a cadena (`string`) y se le aplica `SHA-256` para formar los *nodos hoja* (`NodoMerkle`).
*   **Combinación Ascendente:** El algoritmo agrupa los nodos de dos en dos, concatenando sus *hashes* y aplicando nuevamente `SHA-256` para crear los nodos padre, hasta alcanzar la raíz única. Si el nivel posee un número impar de nodos, el último nodo se duplica para mantener la paridad binaria.

### 3.5 Persistencia y Compresión LZW
Para el cierre de la aplicación y salvaguardado de memoria sin usar archivos intermedios desprotegidos, se implementó el algoritmo de compresión Lempel-Ziv-Welch (LZW).

*   **Proceso de Serialización:** Al recibir el evento de cierre (`QCloseEvent`), el sistema recorre las estructuras en memoria (Grafo, ArbolPeliculas, ArbolFunciones), las parsea a strings JSON minificados y compactos.
*   **Codificación:** El algoritmo inicializa un diccionario con el set de caracteres ASCII estándar (0-255). A medida que lee la cadena de entrada, asigna códigos enteros nuevos (de 16 bits) a patrones de texto repetitivos, logrando la reducción de tamaño.
*   **Archivos Físicos:** Los códigos resultantes se guardan en flujo binario puro en los archivos `sedes.edd`, `peliculas.edd` y `funciones.edd`.

---

## 4. Requisitos del Sistema
Para compilar y ejecutar el proyecto, se requiere un entorno configurado con:
*   MinGW 64-bit o equivalente (Soporte para C++17 o superior).
*   **Qt Creator** (Qt 5.x o Qt 6.x).
*   **Graphviz** instalado a nivel de sistema operativo y su ejecutable `dot` debe estar debidamente configurado en las variables de entorno del sistema (`PATH`).