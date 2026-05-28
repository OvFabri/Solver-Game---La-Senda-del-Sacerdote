
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

const int APRENDIZ = 0;
const int AH_PUCH  = 99;

using Senda = std::vector<int>;  // vector de valores de cartas

// Convierte la senda a string clave para el mapa de memo
std::string claveEstado(const Senda& s) {
    std::string k;
    k.reserve(s.size() * 3);
    for (int v : s) {
        k += std::to_string(v);
        k += ',';
    }
    return k;
}

bool esMaestro(int v) {
    return v == 5 || v == 7 || v == 8 || v == 9 ||
           v == 10 || v == 12 || v == 13 || v == 14;
}

bool esFalsoMaestro(int v) {
    return v == 1 || v == 2 || v == 3 || v == 4 || v == 6 || v == 11;
}

std::string nombreCarta(int v) {
    switch(v) {
        case 0:  return "Aprendiz";
        case 1:  return "Duda";
        case 2:  return "Rencor";
        case 3:  return "Miedo";
        case 4:  return "Pereza";
        case 5:  return "Ejercicio";
        case 6:  return "Cizanoso";
        case 7:  return "Filosofia";
        case 8:  return "Herbolaria";
        case 9:  return "Meditacion";
        case 10: return "Oracion";
        case 11: return "Envidia";
        case 12: return "Peregrino";
        case 13: return "Magia";
        case 14: return "Cuerpo Astral";
        case 99: return "Ah Puch";
        default: return "?";
    }
}


bool victoria(const Senda& s) {
    int ia = -1, ip = -1;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == APRENDIZ) ia = i;
        if (s[i] == AH_PUCH)  ip = i;
    }
    return ia != -1 && ip != -1 && std::abs(ia - ip) == 1;
}

bool derrota(const Senda& s) {
    for (int v : s) if (v == APRENDIZ) return false;
    return true; // Aprendiz fue destruido
}

int buscar(const Senda& s, int val) {
    for (int i = 0; i < (int)s.size(); i++)
        if (s[i] == val) return i;
    return -1;
}

Senda moverCarta(Senda s, int desde, int destino) {
    int n = (int)s.size();
    if (destino < 0) destino = 0;
    if (destino >= n) destino = n - 1;
    if (desde == destino) return s;
    int val = s[desde];
    s.erase(s.begin() + desde);
    if (desde < destino) destino--;
    s.insert(s.begin() + destino, val);
    return s;
}

Senda intercambiar(Senda s, int a, int b) {
    std::swap(s[a], s[b]);
    return s;
}

Senda destruirCarta(Senda s, int idx) {
    s.erase(s.begin() + idx);
    return s;
}

int adyMenorValor(const Senda& s, int pos) {
    int n = (int)s.size();
    int mejor = -1, menorVal = 9999;
    for (int d : {-1, 1}) {
        int i = pos + d;
        if (i < 0 || i >= n) continue;
        if (s[i] == AH_PUCH) continue;
        int v = (s[i] == APRENDIZ) ? 0 : s[i];
        if (v < menorVal) { menorVal = v; mejor = i; }
    }
    return mejor;
}

Senda simularFalsosMaestros(Senda s) {
    std::vector<int> orden = {1, 2, 3, 4, 6, 11};

    for (int fm : orden) {
        int pos = buscar(s, fm);
        if (pos == -1) continue; // fue destruido antes

        int n = (int)s.size();

        if (fm == 1) { // DUDA
            bool hayIzq = (pos - 1 >= 0);
            bool hayDer = (pos + 1 < n);
            if (hayIzq && hayDer) {
                s = intercambiar(s, pos-1, pos+1);
            } else {
                int dir = hayDer ? 1 : -1;
                s = moverCarta(s, pos, pos + dir);
            }
        }
        else if (fm == 2) { // RENCOR
            int t = adyMenorValor(s, pos);
            if (t != -1) {
                bool esAprendiz = (s[t] == APRENDIZ);
                s = destruirCarta(s, t);
                if (esAprendiz) return {}; // derrota
            }
        }
        else if (fm == 3) { // MIEDO
            int t = adyMenorValor(s, pos);
            if (t != -1) s = moverCarta(s, t, 1);
        }
        else if (fm == 4) { // PEREZA
            int t = adyMenorValor(s, pos);
            if (t != -1) {
                if (t - 2 < 1) {
                    // no puede retroceder → Pereza avanza 1
                    int posP = buscar(s, 4);
                    if (posP != -1) s = moverCarta(s, posP, posP + 1);
                } else {
                    s = moverCarta(s, t, t - 2);
                }
            }
        }
        else if (fm == 6) { // CIZANOSO
            int t   = adyMenorValor(s, pos);
            int idxR = buscar(s, 2);
            if (t != -1) {
                if (t == idxR) {
                    s = moverCarta(s, idxR, idxR - 2);
                } else {
                    int idxRnow = buscar(s, 2);
                    int dest = (idxRnow != -1) ? idxRnow - 1 : 1;
                    s = moverCarta(s, t, dest);
                }
            }
        }
        else if (fm == 11) { // ENVIDIA
            int t    = adyMenorValor(s, pos);
            int idxR = buscar(s, 2);
            if (t != -1) {
                if (t == idxR) {
                    s = moverCarta(s, idxR, idxR - 2);
                } else {
                    int dir = (idxR == -1 || idxR > t) ? 1 : -1;
                    s = moverCarta(s, t, t + 2 * dir);
                }
            }
        }
    }
    return s;
}

struct Jugada {
    int  maestro;       
    int  posMaestro;    
    int  opcion;        
    int  target;        
    char dir;           
    std::string descripcion;
};


struct Nodo {
    Senda senda;
    std::vector<Jugada> camino; // jugadas hechas hasta llegar aquí
};

std::vector<std::pair<Senda, Jugada>> generarMovimientos(const Senda& s) {
    std::vector<std::pair<Senda, Jugada>> resultados;
    int n = (int)s.size();

    for (int pos = 0; pos < n; pos++) {
        int val = s[pos];
        if (!esMaestro(val)) continue;

        if (val == 5) { // EJERCICIO
            for (int dist : {1, 2}) {
                for (int dir : {-1, 1}) {
                    int t = pos + dist * dir;
                    if (t < 0 || t >= n) continue;
                    if (s[t] == AH_PUCH) continue; // no mover deidad
                    Senda ns = intercambiar(s, pos, t);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t;
                    j.descripcion = "Ejercicio (pos " + std::to_string(pos) +
                                    ") intercambia con pos " + std::to_string(t) +
                                    " (" + nombreCarta(s[t]) + ")";
                    resultados.push_back({ns, j});
                }
            }
        }
        else if (val == 7) { // FILOSOFIA
            for (int dist : {1, 2, 3}) {
                int movimiento = 4 - dist; 
                for (int ddir : {-1, 1}) {
                    int t = pos + dist * ddir;
                    if (t < 0 || t >= n) continue;
                    if (s[t] == AH_PUCH || s[t] == APRENDIZ) continue;
                    for (int mdir : {-1, 1}) {
                        Senda ns = moverCarta(s, t, t + movimiento * mdir);
                        Jugada j;
                        j.maestro = val; j.posMaestro = pos; j.target = t;
                        j.dir = (mdir == 1) ? '+' : '-';
                        j.descripcion = "Filosofia (pos " + std::to_string(pos) +
                                        ") mueve " + nombreCarta(s[t]) +
                                        " (pos " + std::to_string(t) + ") " +
                                        std::to_string(movimiento) + " espacios " +
                                        (mdir == 1 ? "adelante" : "atras");
                        resultados.push_back({ns, j});
                    }
                }
            }
        }
        else if (val == 8) { // HERBOLARIA
            for (int ddir : {-1, 1}) {
                int t = pos + ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH) continue;
                // Opcion 1: retrocede 2
                {
                    Senda ns = moverCarta(s, t, t - 2);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t; j.opcion = 1;
                    j.descripcion = "Herbolaria (pos " + std::to_string(pos) +
                                    ") retrocede " + nombreCarta(s[t]) + " 2 espacios";
                    resultados.push_back({ns, j});
                }
                // Opcion 2: adelanta 1
                {
                    Senda ns = moverCarta(s, t, t + 1);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t; j.opcion = 2;
                    j.descripcion = "Herbolaria (pos " + std::to_string(pos) +
                                    ") adelanta " + nombreCarta(s[t]) + " 1 espacio";
                    resultados.push_back({ns, j});
                }
            }
        }
        else if (val == 9) { // MEDITACION:
            for (int ddir : {-1, 1}) {
                int t = pos + 3 * ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH) continue;
                Senda ns = intercambiar(s, pos, t);
                Jugada j;
                j.maestro = val; j.posMaestro = pos; j.target = t;
                j.descripcion = "Meditacion (pos " + std::to_string(pos) +
                                ") intercambia con pos " + std::to_string(t) +
                                " (" + nombreCarta(s[t]) + ")";
                resultados.push_back({ns, j});
            }
        }
        else if (val == 10) { // ORACION:
            for (int ddir : {-1, 1}) {
                int t = pos + ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH) continue;
                Senda ns = intercambiar(s, pos, t);
                Jugada j;
                j.maestro = val; j.posMaestro = pos; j.target = t;
                j.descripcion = "Oracion (pos " + std::to_string(pos) +
                                ") intercambia con " + nombreCarta(s[t]) +
                                " (pos " + std::to_string(t) + ")";
                resultados.push_back({ns, j});
            }
        }
        else if (val == 12) { // PEREGRINO:
            for (int ddir : {-1, 1}) {
                int t = pos + ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH || s[t] == APRENDIZ) continue;
                for (int mdir : {-1, 1}) {
                    Senda ns = moverCarta(s, t, t + 2 * mdir);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t;
                    j.dir = (mdir == 1) ? '+' : '-';
                    j.descripcion = "Peregrino (pos " + std::to_string(pos) +
                                    ") mueve " + nombreCarta(s[t]) + " 2 espacios " +
                                    (mdir == 1 ? "adelante" : "atras");
                    resultados.push_back({ns, j});
                }
            }
        }
        else if (val == 13) { // MAGIA
            for (int ddir : {-1, 1}) {
                int t = pos + ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH || s[t] == APRENDIZ) continue;
                // Opcion 1: retrocede 3
                {
                    Senda ns = moverCarta(s, t, t - 3);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t; j.opcion = 1;
                    j.descripcion = "Magia (pos " + std::to_string(pos) +
                                    ") retrocede " + nombreCarta(s[t]) + " 3 espacios";
                    resultados.push_back({ns, j});
                }
                // Opcion 2: adelanta 2
                {
                    Senda ns = moverCarta(s, t, t + 2);
                    Jugada j;
                    j.maestro = val; j.posMaestro = pos; j.target = t; j.opcion = 2;
                    j.descripcion = "Magia (pos " + std::to_string(pos) +
                                    ") adelanta " + nombreCarta(s[t]) + " 2 espacios";
                    resultados.push_back({ns, j});
                }
            }
        }
        else if (val == 14) { // CUERPO ASTRAL:
            for (int ddir : {-1, 1}) {
                int t = pos + 4 * ddir;
                if (t < 0 || t >= n) continue;
                if (s[t] == AH_PUCH) continue;
                Senda ns = intercambiar(s, pos, t);
                Jugada j;
                j.maestro = val; j.posMaestro = pos; j.target = t;
                j.descripcion = "Cuerpo Astral (pos " + std::to_string(pos) +
                                ") intercambia con pos " + std::to_string(t) +
                                " (" + nombreCarta(s[t]) + ")";
                resultados.push_back({ns, j});
            }
        }
    }
    return resultados;
}

void imprimirSenda(const Senda& s) {
    std::cout << "  [";
    for (int i = 0; i < (int)s.size(); i++) {
        if (i > 0) std::cout << " | ";
        std::string nombre = nombreCarta(s[i]);
        if (s[i] == APRENDIZ)     std::cout << "\033[36m";
        else if (s[i] == AH_PUCH) std::cout << "\033[35m";
        else if (esMaestro(s[i])) std::cout << "\033[32m";
        else                       std::cout << "\033[31m";
        std::cout << std::setw(13) << std::left << nombre << "\033[0m";
    }
    std::cout << "]\n";
}

std::vector<Jugada> resolver(const Senda& inicial, int maxRondas = 50) {
    std::unordered_set<std::string> visitados;
    std::queue<Nodo> cola;

    cola.push({inicial, {}});
    visitados.insert(claveEstado(inicial));

    int nodos = 0;

    while (!cola.empty()) {
        Nodo actual = cola.front();
        cola.pop();
        nodos++;

        if ((int)actual.camino.size() >= maxRondas) continue;

        auto movimientos = generarMovimientos(actual.senda);

        for (auto& [sendaTrasJugador, jugada] : movimientos) {

            Senda sendaFinal = simularFalsosMaestros(sendaTrasJugador);

            // Derrota: Aprendiz destruido
            if (sendaFinal.empty()) continue;

            std::vector<Jugada> nuevoCamino = actual.camino;
            nuevoCamino.push_back(jugada);

            // Victoria: Aprendiz adyacente a Ah Puch al final del turno
            if (victoria(sendaFinal)) {
                std::cout << "\033[32m\033[1m"
                          << "Solucion encontrada en " << nuevoCamino.size()
                          << " ronda(s). Estados explorados: " << nodos
                          << "\033[0m\n";
                return nuevoCamino;
            }

            // Si este estado ya fue visitado, saltarlo
            std::string clave = claveEstado(sendaFinal);
            if (visitados.count(clave)) continue;
            visitados.insert(clave);

            cola.push({sendaFinal, nuevoCamino});
        }
    }

    std::cout << "\033[31mNo se encontro solucion en " << maxRondas
              << " rondas. Estados explorados: " << nodos << "\033[0m\n";
    return {};
}

int main() {
    std::cout << "\033[1m";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║   SOLVER — La Senda del Sacerdote            ║\n";
    std::cout << "║   DP con memoizacion (BFS sobre estados)     ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    std::cout << "Ingresa el orden de las 14 cartas de la senda\n";
    std::cout << "(sin el Aprendiz ni Ah Puch, separadas por espacios)\n";
    std::cout << "Valores validos — Maestros: 5 7 8 9 10 12 13 14\n";
    std::cout << "                  F.Maest.: 1 2 3 4 6 11\n\n";
    std::cout << "Ejemplo: 3 12 1 8 11 5 9 2 14 4 7 6 13 10\n\n";
    std::cout << "Senda: ";

    std::vector<int> cartas;
    std::string linea;
    std::getline(std::cin, linea);
    std::istringstream iss(linea);
    int v;
    while (iss >> v) cartas.push_back(v);

    if (cartas.size() != 14) {
        std::cout << "\033[31mError: debes ingresar exactamente 14 cartas.\033[0m\n";
        return 1;
    }


    Senda senda;
    senda.push_back(APRENDIZ);
    for (int c : cartas) senda.push_back(c);
    senda.push_back(AH_PUCH);

    std::cout << "\nEstado inicial:\n";
    imprimirSenda(senda);
    std::cout << "\nBuscando solucion...\n\n";

    auto solucion = resolver(senda);

    if (solucion.empty()) {
        std::cout << "\033[31mEsta configuracion no tiene solucion.\033[0m\n";
        return 0;
    }

    std::cout << "\n\033[1m══ SOLUCION OPTIMA ══\033[0m\n\n";

    Senda actual = senda;
    for (int i = 0; i < (int)solucion.size(); i++) {
        const Jugada& j = solucion[i];
        std::cout << "\033[1mRonda " << (i+1) << ":\033[0m\n";
        std::cout << "  \033[32mJugador activa: " << j.descripcion << "\033[0m\n";

        auto movs = generarMovimientos(actual);

        for (auto& [ns, jj] : movs) {
            if (jj.descripcion == j.descripcion) {
                actual = ns;
                break;
            }
        }

        std::cout << "  Tras jugador:   ";
        imprimirSenda(actual);

        actual = simularFalsosMaestros(actual);
        std::cout << "  Tras F.Maestros:";
        imprimirSenda(actual);
        std::cout << "\n";
    }

    std::cout << "\033[32m\033[1m¡Victoria! El Aprendiz conecta con Ah Puch.\033[0m\n";
    return 0;
}