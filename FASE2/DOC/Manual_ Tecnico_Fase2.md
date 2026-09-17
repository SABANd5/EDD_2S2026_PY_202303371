# Manual Técnico — CinemaUSAC (EDD Fase 2)

**Universidad de San Carlos de Guatemala**

**Facultad de Ingeniería — Escuela de Ciencias y Sistemas**

**Curso:** Estructuras de Datos

**Proyecto:** EDD CinemaUSAC — Fase 2

**Carné:** 202303371

**Tecnología de interfaz:** Qt Widgets (C++), aplicación de escritorio


## 1. Introducción

Este manual documenta la implementación real de la **Fase 2** del sistema **CinemaUSAC**, contenida en la carpeta `FASE2` del repositorio. Sobre la base de la Fase 1 (BST de películas, lista circular doble de solicitudes, lista circular simple de promociones y matriz dispersa), esta fase agrega tres estructuras nuevas implementadas manualmente con punteros crudos, sin STL de por medio salvo `std::vector` y `std::string` como contenedores auxiliares de entrada/salida:

| Estructura | Clase | Archivo | Clave |
|---|---|---|---|
| Árbol AVL | `ArbolFunciones` | `ArbolFunciones.h` / `.cpp` | `codigo_funcion` |
| Árbol B de orden 4 | `ArbolBClientes` | `ArbolBClientes.h` / `.cpp` | `id` del cliente |
| Tabla Hash | `TablaHashReservas` | `TablaHashReservas.h` / `.cpp` | `codigo_reserva` |

Además, la clase `MatrizDispersa` (heredada de Fase 1) se amplía con métodos para vaciarse, cargarse y persistirse por función (`vaciarMatriz`, `cargarDesdeArchivo`, `guardarEnArchivo`), y el controlador `MainWindow` incorpora la lectura/escritura de archivos JSON mediante `QJsonDocument`, `QJsonObject` y `QJsonArray` de Qt, en lugar del parseo manual carácter por carácter usado en algunos métodos internos de las estructuras.

Todas las clases de estructuras de Fase 1 y Fase 2 se instancian como **miembros persistentes** de `MainWindow`, por lo que su estado vive en memoria durante toda la ejecución de la aplicación; los archivos `.json` son el mecanismo de persistencia entre ejecuciones.

---

## 2. Árbol AVL — Gestión de Funciones

**Clase:** `ArbolFunciones` · **Archivos:** `ArbolFunciones.h` / `ArbolFunciones.cpp`

### 2.1 Estructuras de datos

```cpp
struct Funcion {
    std::string codigo_funcion;    // Clave de indexación del AVL
    std::string codigo_pelicula;   // Vincula con el BST de cartelera
    std::string horario;
    std::string sala;
    int filas;
    int columnas;
    std::string archivo_asientos;  // Ej: "F001_funcion.json"
    bool ya_realizada;             // Usado para pintar el nodo verde/rojo en Graphviz
};

struct NodoAVL {
    Funcion dato;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;
};
```

La clase mantiene únicamente `NodoAVL* raiz`. La altura se guarda en cada nodo (en vez de calcularse recursivamente en cada consulta) para que el cálculo del factor de balanceo sea `O(1)` por nodo.

### 2.2 Operaciones implementadas

- **`insertar(Funcion f)`**: si `archivo_asientos` viene vacío, se genera como `codigo_funcion + "_funcion.json"`. Llama a `insertarRecursivo` (inserción estándar de BST comparando `codigo_funcion` como `std::string`) y, tras el balanceo, **crea físicamente** el archivo JSON de asientos con `asientos_ocupados: []`. Los códigos duplicados se ignoran silenciosamente (`return nodo;` sin insertar).
- **`insertarRecursivo`**: aplica los cuatro casos clásicos de reequilibrio AVL (`Izquierda-Izquierda`, `Derecha-Derecha`, `Izquierda-Derecha`, `Derecha-Izquierda`) comparando el `codigo_funcion` contra la clave del nodo desbalanceado, usando `rotacionDerecha` / `rotacionIzquierda`.
- **`eliminar(std::string codigo_funcion)` → `eliminarRecursivo`**: localiza el nodo por búsqueda binaria; si tiene 0 o 1 hijo, lo reemplaza directamente; si tiene 2 hijos, copia el dato del sucesor inorden (`obtenerMinimo(nodo->derecho)`) y elimina recursivamente ese sucesor. Tras cada eliminación se recalcula la altura y se aplican las mismas cuatro rotaciones para mantener el árbol balanceado.
- **`editar(codigo_funcion, fModificada)`**: localiza el nodo con `buscarFuncion` y sobrescribe directamente su `Funcion` (`*f = fModificada`), sin reinsertar ni reordenar el árbol, ya que `codigo_funcion` —la clave— no cambia.
- **`buscarFuncion(codigo_funcion)`**: búsqueda binaria iterativa; retorna un puntero directo al dato del nodo (permite edición in-place desde `MainWindow`) o `nullptr`.
- **`obtenerFuncionesPreorden/Inorden/Postorden()`**: recorridos recursivos clásicos que acumulan resultados en un `std::vector<Funcion>`, usados para alimentar la tabla de funciones según el orden que el administrador seleccione en la interfaz.
- **`generarReporteGraphviz(ArbolPeliculas* cartelera)`**: recibe un puntero a la cartelera para cruzar cada función con su película y determinar el color del nodo: consulta `cartelera->buscarPelicula(codigo_pelicula)`, convierte `fechaFin` con `QDate::fromString` y, si ya pasó respecto a `QDate::currentDate()`, pinta el nodo en rojo pastel (`#EF5350`); si no, en verde pastel (`#81C784`). Genera `reporte_avl.dot` → `reporte_avl.png`.

### 2.3 Decisiones de diseño

- Se usó **altura almacenada por nodo** (`int altura`) en vez de recalcularla en cada llamada, siguiendo el patrón estándar de implementación de AVL para mantener las operaciones de balanceo en tiempo logarítmico.
- La creación del archivo `.json` de asientos se realiza **dentro de `insertar()`**, acoplando la responsabilidad de persistencia a la inserción en el árbol; esto garantiza que nunca exista un nodo en el AVL sin su archivo de respaldo correspondiente.
- El color del reporte Graphviz (verde/rojo) depende del **estado real de la película asociada** (`fechaFin` vencida o no) en el momento de generar el reporte, en lugar de depender del campo `ya_realizada` de la propia `Funcion`, el cual queda declarado en la estructura pero no se actualiza dinámicamente en ningún punto del código; se documenta como una inconsistencia menor entre el modelo de datos y el reporte, ya que el color mostrado realmente refleja el estado de la película, no el de la función.
- `editar()` no permite cambiar el `codigo_funcion` (la clave), evitando así tener que reubicar el nodo dentro del árbol; solo se editan los datos no clave (película, horario, sala, filas, columnas).

---

## 3. Árbol B de Orden 4 — Gestión de Clientes

**Clase:** `ArbolBClientes` · **Archivos:** `ArbolBClientes.h` / `ArbolBClientes.cpp`

### 3.1 Estructuras de datos

```cpp
struct Cliente {
    std::string id;                          // Clave principal del Árbol B
    std::string nombre, correo, telefono, password, tipo;
    std::vector<std::string> codigos_reservas; // Solo referencias, no las reservas completas
};

struct NodoB {
    bool hoja;
    int numClaves;
    Cliente claves[3];   // Orden 4 - 1 = máximo 3 claves por nodo
    NodoB* hijos[4];      // máximo 4 hijos por nodo
};
```

El grado mínimo se fija en `t = 2` en el constructor, lo que implementa exactamente un **Árbol B de orden 4** (máximo `2t - 1 = 3` claves y `2t = 4` hijos por nodo). La raíz se inicializa como un nodo hoja vacío (`raiz = new NodoB(true)`), nunca como `nullptr`.

### 3.2 Operaciones implementadas

- **`insertar(Cliente c)`**: si la raíz está llena (3 claves), se crea una nueva raíz interna, se le asigna la raíz vieja como único hijo y se divide con `dividirHijo` **antes** de insertar, siguiendo el algoritmo clásico de inserción "top-down" de Árboles B (evita tener que propagar divisiones hacia arriba después de insertar).
- **`dividirHijo(padre, i, hijoLleno)`**: cuando un hijo tiene sus 3 claves llenas, su clave central (índice 1) sube al padre, la clave de la derecha (índice 2) se traslada a un nuevo nodo hermano, y los hijos del nodo lleno se reparten entre el nodo original y el nuevo (si no es hoja).
- **`insertarNoLleno(nodo, c)`**: si el nodo es hoja, inserta la clave desplazando las mayores hacia la derecha (inserción ordenada tipo *insertion sort* local). Si es un nodo interno, localiza el hijo correspondiente y, si ese hijo también está lleno, lo divide antes de descender recursivamente.
- **`buscar(id)` → `buscarRecursivo`**: recorre cada nodo comparando secuencialmente las claves (máximo 3 comparaciones por nodo) hasta encontrar la clave exacta o descender al hijo correspondiente; retorna un puntero directo al `Cliente` dentro del árbol, permitiendo edición in-place (usado por `on_btnGuardarPerfil_clicked`).
- **`obtenerClientes()` / `obtenerClientesInorden()`**: recorren el árbol visitando cada hijo antes de su clave correspondiente y el último hijo al final, lo que produce el orden natural ascendente por `id` (equivalente al inorden de un árbol binario, pero visitando varias claves por nodo).
- **`obtenerClientesPreorden()` / `obtenerClientesPostorden()`**: variantes que listan primero todas las claves de un nodo y luego sus hijos (preorden), o primero todos los hijos y luego las claves del nodo (postorden), usadas para el combo de ordenamiento de la tabla de clientes.
- **`eliminar(id)`**: implementación completa del algoritmo de borrado en Árboles B, con los siguientes casos manejados en `eliminarDeNodo`:
  - **Clave en un nodo hoja** (`removerDeHoja`): se desplazan las claves siguientes una posición a la izquierda.
  - **Clave en un nodo interno** (`removerDeNoHoja`): si el hijo izquierdo tiene ≥2 claves, se reemplaza por su **predecesor** (`obtenerPredecesor`, el máximo del subárbol izquierdo) y se elimina el predecesor recursivamente; si no, y el hijo derecho tiene ≥2 claves, se reemplaza por el **sucesor** (`obtenerSucesor`, el mínimo del subárbol derecho); si ninguno tiene claves de sobra, se **fusionan** ambos hijos (`fusionar`) y se elimina del nodo fusionado.
  - **Descenso a un hijo con el mínimo de claves (1)**: antes de bajar, `llenar(nodo, idx)` decide si tomar prestada una clave del hermano izquierdo (`prestarDeAnterior`), del hermano derecho (`prestarDeSiguiente`), o fusionar con uno de ellos (`fusionar`), garantizando que el nodo al que se desciende siempre tenga al menos 2 claves.
  - **Reducción de la raíz**: si tras eliminar la raíz se queda con 0 claves, se reemplaza por su único hijo (o por `nullptr` si era hoja), disminuyendo la altura del árbol.
- **`generarReporteGraphviz()`**: genera cada nodo como un `record` de Graphviz con un compartimento (`<fN>`) por cada clave más uno adicional para los enlaces a hijos, mostrando `id` y `nombre` de cada cliente. Luego dibuja las aristas desde cada compartimento `fN` del nodo hacia su hijo correspondiente, replicando visualmente la estructura de "caja dividida en compartimentos" que exige el enunciado. Genera `reporte_arbolb.dot` → `reporte_arbolb.png`.

### 3.3 Decisiones de diseño

- Se usó un **arreglo estático `Cliente claves[3]`** en vez de un `std::vector`, ya que el orden 4 fija de antemano el número máximo de claves por nodo, evitando la sobrecarga de un contenedor dinámico para un tamaño constante y conocido.
- El algoritmo de inserción es **"top-down" (dividir antes de bajar)**: cualquier nodo lleno se divide en el camino de bajada, de modo que la inserción nunca necesita propagar una división hacia arriba después de insertar, simplificando la recursión respecto a una variante "bottom-up".
- La eliminación se implementó siguiendo estrictamente el algoritmo estándar de Árboles B (predecesor/sucesor/fusión), asegurando que **ningún nodo baje de `t - 1 = 1` clave** salvo la raíz, que puede quedar vacía y ser eliminada para reducir la altura.
- El cliente **solo guarda referencias** (`codigos_reservas`, un `std::vector<std::string>`) a sus reservas, nunca los objetos completos, delegando la información real de cada reserva a la Tabla Hash, tal como exige el enunciado para evitar duplicidad de datos.
- El reporte Graphviz usa `shape=record` con puertos (`<f0>`, `<f1>`...) precisamente para poder dibujar flechas que salen de un compartimento específico de la "caja" hacia el hijo correspondiente, en lugar de una única flecha genérica por nodo.

---

## 4. Tabla Hash — Gestión de Reservas

**Clase:** `TablaHashReservas` · **Archivos:** `TablaHashReservas.h` / `TablaHashReservas.cpp`

### 4.1 Estructuras de datos

```cpp
struct Reserva {
    std::string codigo_reserva;   // Clave de la tabla
    std::string id_cliente;
    std::string codigo_funcion;
    int fila;
    int columna;
    std::string fecha_reserva;
};

struct NodoHash {
    Reserva dato;
    NodoHash* siguiente; // Encadenamiento para resolver colisiones
};
```

La tabla se representa como `std::vector<NodoHash*> tabla`, con **encadenamiento separado (separate chaining)** para resolver colisiones. La capacidad por defecto es **37** (número primo), elegido para reducir agrupamientos sistemáticos en la distribución de los códigos.

### 4.2 Operaciones implementadas

- **`funcionHash(clave)`**: suma los valores ASCII de cada carácter del `codigo_reserva` y aplica módulo sobre la capacidad de la tabla (`suma % capacidad`).
- **`insertar(Reserva r)`**: calcula el índice con `funcionHash` y, si el bucket está vacío, coloca ahí el nuevo nodo; si ya existe una cadena, recorre hasta el final e inserta el nuevo nodo al final de la lista enlazada (resolviendo la colisión por encadenamiento).
- **`buscar(codigo_reserva)`**: calcula el índice y recorre la lista enlazada de ese bucket comparando el código hasta encontrar la coincidencia; retorna un puntero directo a la `Reserva` (permite que `MainWindow` la modifique/consulte sin duplicar datos).
- **`eliminar(codigo_reserva)`**: localiza el nodo en su bucket llevando un puntero `anterior`, y reconecta la lista enlazada saltando el nodo eliminado (contempla el caso especial de que el nodo eliminado sea el primero del bucket).
- **`eliminarPorFuncion(codigo_funcion)`**: recorre **todos** los buckets de la tabla y elimina cualquier reserva cuyo `codigo_funcion` coincida, usado al eliminar una función completa (elimina en cascada todas sus reservas asociadas, sin necesidad de conocer sus códigos individuales).
- **`obtenerReservasPorFuncion(codigo_funcion)`**: recorre igualmente toda la tabla y acumula en un `std::vector<Reserva>` todas las reservas que pertenecen a esa función; es la función utilizada para pintar el mapa de asientos y para generar el reporte de la matriz (ver sección 6.2).
- **`generarReporteGraphviz()`**: dibuja el arreglo de buckets como una tabla HTML de Graphviz (`shape=none`, `<table>`), coloreando en gris los buckets vacíos y en verde claro los ocupados; desde cada bucket ocupado traza una cadena de nodos azul claro (uno por cada reserva encadenada, mostrando código, cliente y función), calculando en el mismo recorrido el total de reservas, buckets ocupados y colisiones (cualquier bucket con más de un nodo encadenado cuenta como colisión). Al final agrega un nodo de estadísticas en amarillo. Genera `reporte_hash.dot` → `reporte_hash.png`.

### 4.3 Decisiones de diseño

- Se eligió **encadenamiento separado** en lugar de direccionamiento abierto porque simplifica la eliminación (no requiere marcadores de "borrado lógico" ni reordenamiento de clústeres) y porque el reporte Graphviz exigido en el enunciado describe explícitamente las colisiones como "listas enlazadas", lo cual encaja directamente con este esquema.
- La **función hash por suma de códigos ASCII** se escogió por su simplicidad de implementación manual (sin usar `std::hash`); al usar módulo sobre un número primo (37) se reduce el riesgo de que claves con patrones similares (como códigos con prefijos comunes) colisionen sistemáticamente en los mismos buckets.
- `eliminarPorFuncion` y `obtenerReservasPorFuncion` requieren **recorrer toda la tabla** (`O(n)`) porque la tabla está indexada por `codigo_reserva`, no por `codigo_funcion`; se documenta como una limitación consciente: consultar por función es más costoso que consultar por código de reserva, pero se prioriza mantener una única clave de indexación como pide el enunciado, en lugar de mantener un índice secundario.
- La capacidad de la tabla es **fija** (no se implementó *rehashing* dinámico); se documenta como punto de mejora para una fase posterior si el volumen de reservas creciera de forma significativa.

---

## 5. Persistencia JSON

La persistencia de la Fase 2 se maneja en dos niveles distintos, lo cual es una decisión de diseño relevante a documentar:

1. **Dentro de las clases de estructuras** (`ArbolFunciones::insertar`, `MatrizDispersa::guardarEnArchivo` / `cargarDesdeArchivo`): se usa `std::ofstream` / `std::ifstream` con escritura y parseo **manual** de texto JSON (concatenación de cadenas para escribir, y búsqueda de subcadenas como `"fila"`, `"columna"` con `std::string::find` para leer). Este es el mecanismo previsto originalmente para que la matriz dispersa se autogestione por archivo de función.
2. **Dentro de `MainWindow`** (carga masiva, reservar/cancelar asiento, edición de perfil): se usa la clase `QJsonDocument` / `QJsonObject` / `QJsonArray` de Qt, que parsea y serializa JSON de forma robusta y con manejo de errores de sintaxis (`QJsonParseError`).

En la práctica, **la ruta que efectivamente se ejecuta en la interfaz gráfica es la segunda** (Qt Json): tanto `on_btnReservar_clicked` como `on_btnCancelarReserva_clicked` leen y reescriben directamente el archivo `F00X_funcion.json` con `QJsonDocument`, sin invocar los métodos manuales de `MatrizDispersa`. Esta decisión se tomó porque `QJsonDocument` ofrece manejo de errores y de tipos (arrays, objetos anidados) mucho más confiable que el parseo por `std::string::find` usado en `matrizdispersa.cpp`, a costa de que el archivo de una función pueda tener nombres de campo ligeramente distintos según qué código lo generó (`on_btnCrearFuncion_clicked` escribe `asientos`, mientras que `ArbolFunciones::insertar` y la lectura/escritura de reservas usan `asientos_ocupados`); se documenta como una inconsistencia de nombres de campo entre ambos flujos, pendiente de unificar.

### 5.1 Validaciones implementadas en la carga masiva de películas (`cargarMasivaPeliculas`)

Antes de insertar cada película en el BST, se valida (rechazando y acumulando el motivo en un reporte final si falla):

- **Código**: debe cumplir la expresión regular `^P\d{3}$` (letra `P` seguida de exactamente 3 dígitos).
- **Título**: no puede estar vacío.
- **Duración**: debe ser un valor numérico nativo de JSON (`isDouble()`) mayor a 0; si viene como texto, se rechaza.
- **Fechas**: `fecha_estreno` y `fecha_fin` deben poder convertirse a un `QDate` válido en formato `yyyy-MM-dd`.
- **Consistencia temporal**: `fecha_fin` no puede ser anterior a `fecha_estreno`.

Cada película rechazada se reporta al final en un cuadro de diálogo, sin detener el resto de la carga; las funciones anidadas de cada película válida se validan mínimamente (código de función y filas no vacíos/positivos) antes de insertarse en el AVL.

### 5.2 Validaciones implementadas en la carga masiva de clientes (`cargarMasivaClientes`)

Se valida únicamente que `id` y `correo` no vengan vacíos antes de insertar el cliente en el Árbol B, y que `codigo_reserva` no venga vacío antes de insertar cada reserva anidada en la Tabla Hash. **A diferencia de la carga de películas**, esta función no valida formato de fechas, duplicidad de correos ni existencia previa del `codigo_funcion` referenciado por cada reserva; se documenta como una asimetría de robustez entre ambos flujos de carga masiva, pendiente de reforzar en una futura iteración.

---

## 6. Integración con la Matriz Dispersa (heredada de Fase 1)

**Clase:** `MatrizDispersa` · **Archivos:** `matrizdispersa.h` / `matrizdispersa.cpp`

### 6.1 Cambios respecto a Fase 1

- El campo `cliente` de cada `NodoMatriz` ahora almacena el **`codigo_reserva`** en vez del nombre del cliente, para poder cruzar la matriz con la Tabla Hash sin duplicar datos personales.
- `reservarAsiento` y `cargarDesdeArchivo` reciben `codigo_reserva` en lugar de un nombre de cliente.
- Se eliminó `cancelarPorCliente` (ya no aplica: ahora se cancela por `codigo_reserva`, resuelto a través de la Tabla Hash).
- Se agregaron tres métodos nuevos para el ciclo de vida por función:
  - **`vaciarMatriz()`**: recorre y libera con `delete` todos los nodos de asiento y las cabeceras de fila y columna, dejando `raiz->abajo` y `raiz->derecha` en `nullptr`. Se invoca automáticamente al inicio de `inicializarSala()`, garantizando que nunca queden en memoria los nodos de la función previamente activa.
  - **`guardarEnArchivo(nombreArchivo, codigoFuncion)`**: recorre la matriz completa y escribe únicamente las celdas cuyo `cliente != "LIBRE"` en un archivo JSON con la estructura `{"codigo_funcion": ..., "asientos_ocupados": [...]}`, escrito manualmente con `std::ofstream`.
  - **`cargarDesdeArchivo(nombreArchivo, filas, columnas, ...)`**: primero llama a `inicializarSala` (que crea todos los asientos como libres), y luego lee el archivo línea por línea buscando las claves `"fila"`, `"columna"` y `"codigo_reserva"` con `std::string::find`, invocando `reservarAsiento` por cada terna completa encontrada.

### 6.2 Estado real de integración en la interfaz

Es importante documentar honestamente que, si bien la clase `MatrizDispersa` fue actualizada con los métodos anteriores para cumplir el ciclo de "cargar/vaciar por función" que pide el enunciado, **el objeto `sala` (instancia de `MatrizDispersa` dentro de `MainWindow`) no se invoca en los flujos de reserva, cancelación ni en el reporte de la matriz dispersa**. En su lugar:

- El **mapa de asientos visual** (`on_btnRefrescarSala_clicked`) se pinta directamente en un `QTableWidget`, marcando cada celda como libre y luego sobrescribiendo en rojo las que aparecen en `tablaReservas.obtenerReservasPorFuncion(codigoFuncion)` (es decir, se consulta la **Tabla Hash**, no la matriz dispersa).
- El **reporte Graphviz de la matriz** (`on_btnReporteMatrizDispersa_clicked`) construye el archivo `.dot` iterando manualmente sobre `filas x columnas` y consultando, para cada coordenada, un `std::map<std::pair<int,int>, std::string>` construido a partir de `tablaReservas.obtenerReservasPorFuncion(...)`, sin llamar a `MatrizDispersa::generarReporteGraphviz()`.
- Las líneas de código que instanciarían la matriz en memoria (`sala.insertar(...)`, `sala = MatrizDispersa();`) aparecen **comentadas** dentro de `on_btnRefrescarSala_clicked`, como un recordatorio de la integración pendiente.

En términos de comportamiento observable para el usuario, el resultado es equivalente (el mapa de asientos y el reporte reflejan correctamente el estado real de ocupación), porque tanto la Tabla Hash como el Árbol AVL contienen toda la información necesaria; sin embargo, **la Matriz Dispersa como estructura de datos activa en memoria durante la ejecución no llega a poblarse** a través de la interfaz gráfica actual, quedando su ciclo de vida (`vaciarMatriz` / `cargarDesdeArchivo` / `guardarEnArchivo`) implementado pero sin conectar a los botones de la UI. Se documenta como la principal brecha entre el diseño exigido en el enunciado (matriz dispersa como única estructura activa que se carga y vacía por función) y la implementación actual, y como punto prioritario a resolver antes de la entrega si se desea cumplir el requisito de forma literal.

### 6.3 Eliminación de función y limpieza de archivos

Al eliminar una función desde `on_btnEliminarFun_clicked`, el sistema elimina la función del Árbol AVL y todas sus reservas de la Tabla Hash (`tablaReservas.eliminarPorFuncion`), pero **no elimina físicamente el archivo `.json`** de asientos de esa función del disco, a diferencia de lo descrito en el enunciado ("se elimina su archivo .json de asientos"). Se documenta como una limpieza pendiente para evitar archivos huérfanos en el directorio del proyecto.

---

## 7. Controlador de Interfaz — `MainWindow` (adiciones de Fase 2)

**Archivos:** `mainwindow.h`, `mainwindow.cpp`, `mainwindow.ui`

### 7.1 Nuevas estructuras como miembros de la ventana

```cpp
ArbolFunciones avlFunciones;
ArbolBClientes arbolClientes;
TablaHashReservas tablaReservas;
```

Se suman a las cuatro estructuras heredadas de Fase 1 (`cartelera`, `solicitudes`, `sala`, `promociones`), todas viviendo en memoria mientras la aplicación está abierta. Se agrega también `std::string clienteLogueadoID`, que sustituye el mecanismo de credenciales fijas de Fase 1 por una sesión real ligada a un nodo del Árbol B.

### 7.2 Autenticación

`on_btnIniciarSesion_clicked` primero compara contra las credenciales **fijas** de administrador (`AdminCine@gmail.com` / `admin123Pass`, codificadas literalmente en el controlador). Si no coincide, recorre linealmente `arbolClientes.obtenerClientes()` comparando correo y contraseña en texto plano contra cada `Cliente`; si encuentra coincidencia, guarda su `id` en `clienteLogueadoID`, precarga sus combos de películas, su historial de reservas y su perfil, y redirige a la vista de Taquilla. Las contraseñas se almacenan y comparan **sin cifrar**, consistente con el alcance declarado de esta fase.

### 7.3 Flujos principales conectados a cada estructura nueva

| Slot | Estructura | Acción |
|---|---|---|
| `on_btnCrearFuncion_clicked` | AVL | Valida datos y unicidad de código, inserta en `avlFunciones` y crea el archivo `.json` base de asientos (vía `QJsonDocument`, con la clave `"asientos"`) |
| `on_btnEditFuncion_clicked` | AVL | Valida película existente, arma una `Funcion` modificada y llama `avlFunciones.editar(...)` |
| `on_btnEliminarFun_clicked` | AVL + Hash | Elimina las reservas de la función (`tablaReservas.eliminarPorFuncion`) y luego el nodo del AVL (`avlFunciones.eliminar`) |
| `on_btnBuscarFuncion_clicked` | AVL | `avlFunciones.buscarFuncion(codigo)` |
| `on_btnActualizarTablaFuncion_clicked` / `on_cmbGestionFuncion_currentTextChanged` | AVL | Recorre el árbol en el orden elegido (`obtenerFuncionesPreorden/Inorden/Postorden`) |
| `on_btnReporteAVL_clicked` | AVL | `avlFunciones.generarReporteGraphviz(&cartelera)` |
| `on_btnRegistrarUsuario_clicked` | Árbol B | Valida campos y correo único, genera un `id` correlativo de 4 dígitos e inserta en `arbolClientes` |
| `on_btnBuscarCliente_clicked` / `on_btnEliminarCliente_clicked` | Árbol B + Hash | Busca por `id`; al eliminar, primero borra en cascada cada `codigo_reserva` del cliente en la Tabla Hash y luego elimina el nodo del Árbol B |
| `on_btnCargaMasivaClientes_clicked` | Árbol B + Hash | Llama a `cargarMasivaClientes(ruta)` |
| `on_cmbOrdenClientes_currentTextChanged` | Árbol B | Recorre en el orden elegido (`obtenerClientesInorden/Preorden/Postorden`) |
| `on_btnReporteArbolB_clicked` | Árbol B | `arbolClientes.generarReporteGraphviz()` |
| `on_btnGuardarPerfil_clicked` | Árbol B | Valida campos y correo único (excluyéndose a sí mismo), y modifica el `Cliente` in-place a través del puntero devuelto por `buscar` |
| `on_btnReservar_clicked` | AVL + Hash + Árbol B + JSON | Valida función, dimensiones de sala y sesión activa; genera `codigo_reserva`, inserta en `tablaReservas`, referencia el código en el `Cliente` del Árbol B, y actualiza el archivo `F00X_funcion.json` con `QJsonDocument` |
| `on_btnCancelarReserva_clicked` | Hash + Árbol B + JSON | Valida propiedad y vigencia de la reserva, libera el asiento en el archivo JSON, quita la referencia del Árbol B y elimina el nodo de la Tabla Hash |
| `on_btnConsulEstado_clicked` (admin) / `on_btnActualizarReservaUsuario_clicked` (cliente) | Árbol B + Hash | Recorren `codigos_reservas` del cliente y consultan cada una en `tablaReservas.buscar` para mostrar función, fecha, asiento y estado (VALIDA/VENCIDA según `QDate`) |
| `on_btnRefrescarSala_clicked` | AVL + Hash | Pinta la tabla visual de asientos consultando `avlFunciones.buscarFuncion` (dimensiones) y `tablaReservas.obtenerReservasPorFuncion` (ocupación) |
| `on_btnReportesReservas_clicked` | Hash | `tablaReservas.generarReporteGraphviz()` |
| `on_btnReporteMatrizDispersa_clicked` | AVL + Hash + Árbol B | Construye el `.dot` de la matriz manualmente a partir de las dimensiones del AVL y la ocupación de la Tabla Hash (ver sección 6.2) |

### 7.4 Decisiones de diseño relevantes

- Las **validaciones de consistencia referencial** (que la película exista antes de crear una función, que la función exista y tenga cupo antes de reservar, que la reserva pertenezca al cliente antes de cancelarla) se implementan en el controlador, no dentro de las clases de estructuras, siguiendo el mismo patrón arquitectónico de Fase 1: las estructuras son responsables únicamente de su propia integridad interna, y `MainWindow` es responsable de la integridad **entre** estructuras.
- El **código de reserva** se genera de forma determinística y legible (`R-<idCliente>-<codigoFuncion>-F<fila>C<columna>`) en vez de un identificador aleatorio, lo que facilita la depuración visual del sistema a costa de no garantizar unicidad ante una hipotética reserva duplicada del mismo asiento por el mismo cliente en la misma función (escenario ya bloqueado antes por la validación de asiento ocupado).
- Los reportes se **regeneran bajo demanda** justo antes de mostrarse, igual que en Fase 1, garantizando que el `.png` desplegado siempre refleje el estado más reciente de cada estructura.

---

## 8. Manejo de Memoria

- `ArbolFunciones` y `ArbolBClientes` no implementan un destructor que libere recursivamente todos sus nodos; al ser estructuras de vida única durante la sesión de la aplicación, el sistema operativo libera esa memoria al cerrar el programa, igual que se documentó para las estructuras de Fase 1.
- `TablaHashReservas` tampoco implementa un destructor que recorra los `capacidad` buckets liberando cada lista enlazada de colisiones.
- `MatrizDispersa::vaciarMatriz()` sí libera explícitamente con `delete` cada nodo de asiento y cada cabecera de fila/columna antes de reconstruir la matriz para una nueva función, corrigiendo la fuga de memoria que se había documentado en el manual técnico de Fase 1 para esta misma clase (donde `sala = MatrizDispersa();` no liberaba los nodos previos). Esta mejora, sin embargo, no llega a ejercitarse en tiempo de ejecución mientras el objeto `sala` no se invoque desde la interfaz (ver sección 6.2).

---

## 9. Conclusiones

- El **Árbol AVL de funciones** cumple el balanceo automático exigido, verificado mediante las cuatro rotaciones clásicas tanto en inserción como en eliminación, y expone los tres recorridos solicitados para los listados administrativos.
- El **Árbol B de orden 4 de clientes** implementa el algoritmo completo de inserción con división "top-down" y eliminación con redistribución/fusión, cumpliendo la invariante de mantener entre `t-1` y `2t-1` claves por nodo salvo la raíz.
- La **Tabla Hash de reservas** resuelve colisiones por encadenamiento y expone tanto acceso puntual por `codigo_reserva` como recorridos completos por `codigo_funcion`, aunque estos últimos son en `O(n)` al no existir un índice secundario.
- La **integración de la Matriz Dispersa** con las nuevas estructuras quedó **parcialmente resuelta**: la clase cuenta con los métodos de ciclo de vida por función (`vaciarMatriz`, `cargarDesdeArchivo`, `guardarEnArchivo`) que el enunciado exige, pero el controlador de interfaz actualmente resuelve el mapa de asientos y su reporte consultando directamente la Tabla Hash y el Árbol AVL, sin instanciar ni poblar el objeto `MatrizDispersa` en memoria. Esta es la principal deuda técnica identificada en esta fase y debe priorizarse antes de la entrega si se requiere cumplir literalmente el requisito de "matriz dispersa como única estructura activa en memoria".
- La persistencia JSON combina dos mecanismos (texto manual dentro de las clases, `QJsonDocument` dentro del controlador); se recomienda unificar hacia `QJsonDocument` en una futura refactorización para eliminar la inconsistencia de nombres de campo (`asientos` vs. `asientos_ocupados`) detectada entre `on_btnCrearFuncion_clicked` y el resto del flujo de reservas.
