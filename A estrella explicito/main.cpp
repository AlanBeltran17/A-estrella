/** A* explícito  */
#include <bits/stdc++.h>
using namespace std;

/// ===============================
/// Estructura Nodo en el grafo
/// ===============================
struct Nodo {
    int x, y;  /// coordenada en el mapa (x,y)
    vector<pair<Nodo*, double>> vecinos; /// nodo vecino + costo
};

/// ===============================
/// Nodo para A* (exploración)
/// ===============================
struct NodoA {
    Nodo* nodo;
    double g, h;
    NodoA* padre;
    double f() const { return g + h; }
};

/// ===============================
/// Verificar si está dentro del mapa
/// ===============================
bool dentro(int x, int y, int filas, int columnas) {
    return x >= 0 && x < filas && y >= 0 && y < columnas;
}

/// ===============================
/// Verificar si está bloqueado en diagonal
/// ===============================
bool bloqueadoDiagonal(int x, int y, int dx, int dy, const vector<vector<int>>& mapa) {
    int x1 = x + dx, y1 = y;
    int x2 = x, y2 = y + dy;
    if (!dentro(x1, y1, mapa.size(), mapa[0].size()) ||
        !dentro(x2, y2, mapa.size(), mapa[0].size()))
        return false;
    return (mapa[x1][y1] == 1 && mapa[x2][y2] == 1);
}

/// ===============================
/// Heurística Manhattan (clave del algoritmo)
/// ===============================
double heuristica(Nodo* a, Nodo* b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

/// ===============================
/// Estructura para guardar el camino con g, h y f
/// ===============================
struct Paso {
    int x, y;
    double g, h, f;
};

/// ===============================
/// Recosntruye el camino desde la meta al inicio
/// ===============================
vector<Paso> reconstruirCamino(NodoA* actual) {
    vector<Paso> camino;
    while (actual != nullptr) {
        camino.push_back({actual->nodo->x, actual->nodo->y, actual->g, actual->h, actual->f()});
        actual = actual->padre;
    }
    reverse(camino.begin(), camino.end());
    return camino;
}

/// ===============================
/// Construir grafo explícito
/// ===============================
vector<vector<Nodo*>> construirGrafo(const vector<vector<int>>& mapa) {
    int filas = mapa.size();
    int columnas = mapa[0].size();
    vector<vector<Nodo*>> nodos(filas, vector<Nodo*>(columnas, nullptr));

    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            if (mapa[i][j] == 0)
                nodos[i][j] = new Nodo{i, j};
        }
    }

    vector<pair<int,int>> movimientos = {
        {-1,-1}, {-1,1}, {1,-1}, {1,1},
        {-1,0}, {1,0}, {0,-1}, {0,1}
    };

    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            if (!nodos[i][j]) continue;
            for(auto [dx, dy] : movimientos) {
                int nx = i + dx;
                int ny = j + dy;
                if (!dentro(nx, ny, filas, columnas)) continue;
                if (!nodos[nx][ny]) continue;
                if (abs(dx)==1 && abs(dy)==1 && bloqueadoDiagonal(i,j,dx,dy,mapa))
                    continue;
                double costo = (abs(dx)==1 && abs(dy)==1) ? 1.414 : 1.0;
                nodos[i][j]->vecinos.push_back({nodos[nx][ny], costo});
            }
        }
    }
    return nodos;
}

/// ===============================
/// Leer Archivo con mapa con i (inicio) y f (fin)
/// ===============================
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
                cout<< "Hay un espacio demas!!!\n";
                return {};
            }
            else {
                cout << "Caracter invalido en el mapa!!!\n" << c << endl;
                return {};
            }
        }

        mapa.push_back(filaMapa);
        fila++;
    }

    if (!hayInicio || !hayFin) {
        cerr << "ERROR: El mapa debe incluir 'i' y 'f'." << endl;
        return {};
    }

    return mapa;
}

/// ===============================
/// Algoritmo A*
/// ===============================
vector<Paso> A_Estrella(Nodo* inicio, Nodo* fin) {
    vector<NodoA*> abierta;
    map<Nodo*, bool> visitado;

    NodoA* nodoInicial = new NodoA{inicio, 0, heuristica(inicio, fin), nullptr};
    abierta.push_back(nodoInicial);

    while(!abierta.empty()) {
        auto it = min_element(abierta.begin(), abierta.end(), [](NodoA* a, NodoA* b){
            if (fabs(a->f() - b->f()) < 1e-6)
                return a->h < b->h; /// Desempate por menor heurística
            return a->f() < b->f();
        });

        NodoA* actual = *it;
        abierta.erase(it);

        if (visitado[actual->nodo]) continue;
        visitado[actual->nodo] = true;

        if (actual->nodo == fin)
            return reconstruirCamino(actual); /// Retorna el camino encontrado

        for(auto [vecino, costo] : actual->nodo->vecinos) {
            if (visitado[vecino]) continue;
            NodoA* vecinoA = new NodoA{vecino, actual->g + costo, heuristica(vecino, fin), actual};
            abierta.push_back(vecinoA);
        }
    }
    return {}; /// Retorna vacío si no hay camino
}

/// ===============================
/// Programa principal
/// ===============================
int main() {
    /*
    vector<vector<int>> mapa = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    */
    pair<int,int> inicioPos, finPos;
    vector<vector<int>> mapa = leerMapa("mapa10.txt", inicioPos, finPos);

    if (mapa.empty()) {
        cout << "Mapa vacio o incorrecto.\n";
        return 0;
    }

    auto nodos = construirGrafo(mapa);

    Nodo* inicio = nodos[inicioPos.first][inicioPos.second];
    Nodo* fin    = nodos[finPos.first][finPos.second];

    auto camino = A_Estrella(inicio, fin);

    if(camino.empty())
        cout << "No se encontro camino.\n";
    else {
        cout << "      <<< Camino encontrado>>>\n\n";
        cout<<  "Coordenada |    G    |      H     |    F\n";
        cout<<  "--------------------------------------------\n";
        for(auto p : camino) {

            cout << "   (" << p.x << "," << p.y << ")   |   "
                 << "g=" << fixed << setprecision(2) << p.g << "  |  "
                 << "h=" << p.h << "  |  "
                 << "f=" << p.f << "\n";
        }
    }

    return 0;
}
