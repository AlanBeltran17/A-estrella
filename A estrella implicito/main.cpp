/** A* implícito */
#include <bits/stdc++.h>
using namespace std;

/// =====================================
/// Estructura Nodo
/// =====================================
struct Nodo {
    int x, y;
    double g, h;
    Nodo* padre;
    double f() const { return g + h; }
};

/// =====================================
/// Estructura para guardar pasos del camino
/// =====================================
struct Paso {
    int x, y;
    double g, h, f;
};

/// =====================================
/// Verifica límites del mapa
/// =====================================
bool dentro(int x, int y, int filas, int columnas) {
    return x >= 0 && x < filas && y >= 0 && y < columnas;
}

/// =====================================
/// Heurística corazón del algoritmo
/// =====================================
double heuristica(pair<int,int> a, pair<int,int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

/// =====================================
/// Reconstrucción del camino
/// =====================================
vector<Paso> reconstruirCamino(Nodo* actual) {
    vector<Paso> camino;
    while (actual != nullptr) {
        camino.push_back({actual->x, actual->y, actual->g, actual->h, actual->f()});
        actual = actual->padre;
    }
    reverse(camino.begin(), camino.end());
    return camino;
}

/// =====================================
/// Verifica si una diagonal está bloqueada
/// =====================================
bool bloqueadoDiagonal(int x, int y, int dx, int dy, const vector<vector<int>>& mapa) {
    int x1 = x + dx, y1 = y;
    int x2 = x, y2 = y + dy;
    if (!dentro(x1, y1, mapa.size(), mapa[0].size()) ||
        !dentro(x2, y2, mapa.size(), mapa[0].size()))
        return false;
    return (mapa[x1][y1] == 1 && mapa[x2][y2] == 1);
}

/// =====================================
/// Leer mapa desde archivo con i (inicio) y f (fin)
/// =====================================
vector<vector<int>> leerMapa(const string& archivo,
                             pair<int,int>& inicio,
                             pair<int,int>& fin)
{
    ifstream arch(archivo);
    if (!arch.is_open()) {
        cerr << "Error al abrir " << archivo << endl;
        return {};
    }

    vector<vector<int>> mapa;
    string linea;

    int fila = 0;
    bool hayInicio = false, hayFin = false;

    while (getline(arch, linea)) {

        if (linea.empty()) continue;

        vector<int> filaMapa;

        for (int col = 0; col < (int)linea.size(); col++) {
            char c = linea[col];

            if (c == '.') filaMapa.push_back(0);
            else if (c == '#') filaMapa.push_back(1);
            else if (c == 'i') {
                filaMapa.push_back(0);
                inicio = {fila, col};
                hayInicio = true;
            }
            else if (c == 'f') {
                filaMapa.push_back(0);
                fin = {fila, col};
                hayFin = true;
            }
            else if (c == ' ') {
                cerr << "Hay un espacio de mas en el mapa.\n";
                return {};
            }
            else {
                cerr << "Caracter inválido en el mapa: " << c << endl;
                return {};
            }
        }

        mapa.push_back(filaMapa);
        fila++;
    }

    if (!hayInicio || !hayFin) {
        cerr << "ERROR: El mapa debe incluir 'i' y 'f'.\n";
        return {};
    }

    return mapa;
}


/// =====================================
/// Algoritmo A*
/// =====================================
vector<Paso> A_estrella_lista(const vector<vector<int>>& mapa, pair<int,int> inicio, pair<int,int> fin) {
    int filas = mapa.size();
    int columnas = mapa[0].size();

    vector<vector<bool>> visitado(filas, vector<bool>(columnas, false));
    vector<Nodo*> abierta;

    Nodo* inicial = new Nodo{inicio.first, inicio.second, 0, heuristica(inicio, fin), nullptr};
    abierta.push_back(inicial);

    vector<pair<int,int>> movimientos = {
        {-1,-1}, {-1,1}, {1,-1}, {1,1}, /// diagonales
        {-1,0}, {1,0}, {0,-1}, {0,1}    /// rectos
    };

    while (!abierta.empty()) {
        /// Buscar nodo con menor f, desempate por menor h
        auto it = min_element(abierta.begin(), abierta.end(), [](Nodo* a, Nodo* b){
            if (fabs(a->f() - b->f()) < 1e-6)
                return a->h < b->h; /// desempate por h
            return a->f() < b->f();
        });

        Nodo* actual = *it;
        abierta.erase(it);

        if (visitado[actual->x][actual->y]) continue;
        visitado[actual->x][actual->y] = true;

        if (make_pair(actual->x, actual->y) == fin)
            return reconstruirCamino(actual); /// Retorna el camino encontrado

        for (auto [dx, dy] : movimientos) {
            int nx = actual->x + dx;
            int ny = actual->y + dy;

            if (!dentro(nx, ny, filas, columnas)) continue;
            if (mapa[nx][ny] == 1) continue;
            if (abs(dx) == 1 && abs(dy) == 1 && bloqueadoDiagonal(actual->x, actual->y, dx, dy, mapa))
                continue;

            double costo = actual->g + ((abs(dx) == 1 && abs(dy) == 1) ? 1.414 : 1.0);
            Nodo* vecino = new Nodo{nx, ny, costo, heuristica({nx, ny}, fin), actual};
            abierta.push_back(vecino);
        }
    }

    return {}; /// Retorna lista vacía si no se encontró camino
}

/// =====================================
/// Programa principal
/// =====================================
int main() {
    pair<int,int> inicio, fin;

    // Leer mapa desde archivo
    vector<vector<int>> mapa = leerMapa("mapa10.txt", inicio, fin);

    if (mapa.empty()) {
        cout << "Mapa inválido.\n";
        return 0;
    }

    // Ejecutar A*
    auto camino = A_estrella_lista(mapa, inicio, fin);

    if (camino.empty()) {
        cout << "No se encontró camino.\n";
    } else {
        cout << "      <<< Camino encontrado >>>\n\n";
        cout << "Coordenada |    G    |      H     |    F\n";
        cout << "--------------------------------------------\n";
        for (auto p : camino) {
            cout << "   (" << p.x << "," << p.y << ")   |   "
                 << "g=" << fixed << setprecision(2) << p.g << "  |  "
                 << "h=" << p.h << "  |  "
                 << "f=" << p.f << "\n";
        }
    }

    return 0;
}

