#pragma once
// ============================================================
//  logica.h — La Senda del Sacerdote
//  Logica compartida entre el juego y el solver
// ============================================================
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <iostream>
#include <iomanip>

// ============================================================
//  CONSTANTES
// ============================================================
const int APRENDIZ = 0;
const int AH_PUCH  = 99;

// ============================================================
//  TIPO PRINCIPAL
// ============================================================
using Senda = std::vector<int>;   // vector de valores de cartas

// ============================================================
//  INFO DE CARTAS
// ============================================================
inline bool esMaestro(int v) {
    return v==5||v==7||v==8||v==9||v==10||v==12||v==13||v==14;
}
inline bool esFalsoMaestro(int v) {
    return v==1||v==2||v==3||v==4||v==6||v==11;
}
inline std::string nombreCarta(int v) {
    switch(v){
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

// ============================================================
//  COLORES ANSI
// ============================================================
const std::string C_MAESTRO  = "\033[32m";
const std::string C_FALSO    = "\033[31m";
const std::string C_APRENDIZ = "\033[36m";
const std::string C_DEIDAD   = "\033[35m";
const std::string C_AMARILLO = "\033[33m";
const std::string C_NEGRITA  = "\033[1m";
const std::string C_RESET    = "\033[0m";

// ============================================================
//  OPERACIONES SOBRE SENDA
// ============================================================
inline bool victoria(const Senda& s) {
    int ia=-1, ip=-1;
    for(int i=0;i<(int)s.size();i++){
        if(s[i]==APRENDIZ) ia=i;
        if(s[i]==AH_PUCH)  ip=i;
    }
    return ia!=-1 && ip!=-1 && std::abs(ia-ip)==1;
}

inline bool derrota(const Senda& s) {
    for(int v:s) if(v==APRENDIZ) return false;
    return true;
}

inline int buscar(const Senda& s, int val) {
    for(int i=0;i<(int)s.size();i++) if(s[i]==val) return i;
    return -1;
}

inline Senda moverCarta(Senda s, int desde, int destino) {
    int n=(int)s.size();
    if(destino<0) destino=0;
    if(destino>=n) destino=n-1;
    if(desde==destino) return s;
    int val=s[desde];
    s.erase(s.begin()+desde);
    if(desde<destino) destino--;
    s.insert(s.begin()+destino, val);
    return s;
}

inline Senda intercambiar(Senda s, int a, int b) {
    std::swap(s[a],s[b]);
    return s;
}

inline Senda destruirCarta(Senda s, int idx) {
    s.erase(s.begin()+idx);
    return s;
}

inline int adyMenorValor(const Senda& s, int pos) {
    int n=(int)s.size(), mejor=-1, menorVal=9999;
    for(int d:{-1,1}){
        int i=pos+d;
        if(i<0||i>=n) continue;
        if(s[i]==AH_PUCH) continue;
        int v=(s[i]==APRENDIZ)?0:s[i];
        if(v<menorVal){ menorVal=v; mejor=i; }
    }
    return mejor;
}

// ============================================================
//  SIMULAR TURNO FALSOS MAESTROS
//  Retorna senda vacía si el Aprendiz fue destruido (derrota)
// ============================================================
inline Senda simularFalsosMaestros(Senda s) {
    for(int fm:{1,2,3,4,6,11}){
        int pos=buscar(s,fm);
        if(pos==-1) continue;
        int n=(int)s.size();

        if(fm==1){ // DUDA
            bool hayIzq=(pos-1>=0), hayDer=(pos+1<n);
            if(hayIzq&&hayDer) s=intercambiar(s,pos-1,pos+1);
            else { int dir=hayDer?1:-1; s=moverCarta(s,pos,pos+dir); }
        }
        else if(fm==2){ // RENCOR
            int t=adyMenorValor(s,pos);
            if(t!=-1){
                bool esAp=(s[t]==APRENDIZ);
                s=destruirCarta(s,t);
                if(esAp) return {};
            }
        }
        else if(fm==3){ // MIEDO
            int t=adyMenorValor(s,pos);
            if(t!=-1) s=moverCarta(s,t,1);
        }
        else if(fm==4){ // PEREZA
            int t=adyMenorValor(s,pos);
            if(t!=-1){
                if(t-2<1){ int p=buscar(s,4); if(p!=-1) s=moverCarta(s,p,p+1); }
                else s=moverCarta(s,t,t-2);
            }
        }
        else if(fm==6){ // CIZANOSO
            int t=adyMenorValor(s,pos), idxR=buscar(s,2);
            if(t!=-1){
                if(t==idxR) s=moverCarta(s,idxR,idxR-2);
                else { int d=(idxR!=-1)?idxR-1:1; s=moverCarta(s,t,d); }
            }
        }
        else if(fm==11){ // ENVIDIA
            int t=adyMenorValor(s,pos), idxR=buscar(s,2);
            if(t!=-1){
                if(t==idxR) s=moverCarta(s,idxR,idxR-2);
                else { int dir=(idxR==-1||idxR>t)?1:-1; s=moverCarta(s,t,t+2*dir); }
            }
        }
    }
    return s;
}

// ============================================================
//  JUGADA (descripcion de un movimiento del jugador)
// ============================================================
struct Jugada {
    int  maestro=0, posMaestro=0, opcion=0, target=0;
    char dir='+';
    std::string descripcion;
};

// ============================================================
//  GENERAR TODOS LOS MOVIMIENTOS POSIBLES DEL JUGADOR
// ============================================================
inline std::vector<std::pair<Senda,Jugada>> generarMovimientos(const Senda& s) {
    std::vector<std::pair<Senda,Jugada>> res;
    int n=(int)s.size();

    auto agr = [&](Senda ns, Jugada j){ res.push_back({ns,j}); };

    for(int pos=0;pos<n;pos++){
        int val=s[pos];
        if(!esMaestro(val)) continue;

        if(val==5){ // EJERCICIO
            for(int dist:{1,2}) for(int d:{-1,1}){
                int t=pos+dist*d;
                if(t<0||t>=n||s[t]==AH_PUCH) continue;
                Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t;
                j.descripcion="Ejercicio (pos "+std::to_string(pos)+") intercambia con pos "+std::to_string(t)+" ("+nombreCarta(s[t])+")";
                agr(intercambiar(s,pos,t),j);
            }
        }
        else if(val==7){ // FILOSOFIA
            for(int dist:{1,2,3}) for(int dd:{-1,1}){
                int t=pos+dist*dd;
                if(t<0||t>=n||s[t]==AH_PUCH||s[t]==APRENDIZ) continue;
                int mov=4-dist;
                for(int md:{-1,1}){
                    Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t; j.dir=(md==1)?'+':'-';
                    j.descripcion="Filosofia (pos "+std::to_string(pos)+") mueve "+nombreCarta(s[t])+" (pos "+std::to_string(t)+") "+std::to_string(mov)+" espacios "+(md==1?"adelante":"atras");
                    agr(moverCarta(s,t,t+mov*md),j);
                }
            }
        }
        else if(val==8){ // HERBOLARIA
            for(int d:{-1,1}){
                int t=pos+d;
                if(t<0||t>=n||s[t]==AH_PUCH) continue;
                Jugada j1; j1.maestro=val; j1.posMaestro=pos; j1.target=t; j1.opcion=1;
                j1.descripcion="Herbolaria (pos "+std::to_string(pos)+") retrocede "+nombreCarta(s[t])+" 2 espacios";
                agr(moverCarta(s,t,t-2),j1);
                Jugada j2; j2.maestro=val; j2.posMaestro=pos; j2.target=t; j2.opcion=2;
                j2.descripcion="Herbolaria (pos "+std::to_string(pos)+") adelanta "+nombreCarta(s[t])+" 1 espacio";
                agr(moverCarta(s,t,t+1),j2);
            }
        }
        else if(val==9){ // MEDITACION
            for(int d:{-1,1}){
                int t=pos+3*d;
                if(t<0||t>=n||s[t]==AH_PUCH) continue;
                Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t;
                j.descripcion="Meditacion (pos "+std::to_string(pos)+") intercambia con pos "+std::to_string(t)+" ("+nombreCarta(s[t])+")";
                agr(intercambiar(s,pos,t),j);
            }
        }
        else if(val==10){ // ORACION
            for(int d:{-1,1}){
                int t=pos+d;
                if(t<0||t>=n||s[t]==AH_PUCH) continue;
                Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t;
                j.descripcion="Oracion (pos "+std::to_string(pos)+") intercambia con "+nombreCarta(s[t])+" (pos "+std::to_string(t)+")";
                agr(intercambiar(s,pos,t),j);
            }
        }
        else if(val==12){ // PEREGRINO
            for(int d:{-1,1}){
                int t=pos+d;
                if(t<0||t>=n||s[t]==AH_PUCH||s[t]==APRENDIZ) continue;
                for(int md:{-1,1}){
                    Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t; j.dir=(md==1)?'+':'-';
                    j.descripcion="Peregrino (pos "+std::to_string(pos)+") mueve "+nombreCarta(s[t])+" 2 espacios "+(md==1?"adelante":"atras");
                    agr(moverCarta(s,t,t+2*md),j);
                }
            }
        }
        else if(val==13){ // MAGIA
            for(int d:{-1,1}){
                int t=pos+d;
                if(t<0||t>=n||s[t]==AH_PUCH||s[t]==APRENDIZ) continue;
                Jugada j1; j1.maestro=val; j1.posMaestro=pos; j1.target=t; j1.opcion=1;
                j1.descripcion="Magia (pos "+std::to_string(pos)+") retrocede "+nombreCarta(s[t])+" 3 espacios";
                agr(moverCarta(s,t,t-3),j1);
                Jugada j2; j2.maestro=val; j2.posMaestro=pos; j2.target=t; j2.opcion=2;
                j2.descripcion="Magia (pos "+std::to_string(pos)+") adelanta "+nombreCarta(s[t])+" 2 espacios";
                agr(moverCarta(s,t,t+2),j2);
            }
        }
        else if(val==14){ // CUERPO ASTRAL
            for(int d:{-1,1}){
                int t=pos+4*d;
                if(t<0||t>=n||s[t]==AH_PUCH) continue;
                Jugada j; j.maestro=val; j.posMaestro=pos; j.target=t;
                j.descripcion="Cuerpo Astral (pos "+std::to_string(pos)+") intercambia con pos "+std::to_string(t)+" ("+nombreCarta(s[t])+")";
                agr(intercambiar(s,pos,t),j);
            }
        }
    }
    return res;
}

// ============================================================
//  IMPRIMIR SENDA (version compacta horizontal)
// ============================================================
inline void imprimirSendaH(const Senda& s) {
    std::cout << "  [";
    for(int i=0;i<(int)s.size();i++){
        if(i>0) std::cout<<" | ";
        if(s[i]==APRENDIZ)      std::cout<<C_APRENDIZ;
        else if(s[i]==AH_PUCH)  std::cout<<C_DEIDAD;
        else if(esMaestro(s[i]))std::cout<<C_MAESTRO;
        else                    std::cout<<C_FALSO;
        std::cout<<std::setw(13)<<std::left<<nombreCarta(s[i])<<C_RESET;
    }
    std::cout<<"]\n";
}

// ============================================================
//  NODO INTERNO DEL BFS
// ============================================================
struct NodoBFS {
    Senda senda;
    std::vector<Jugada> camino;
};

// ============================================================
//  CLAVE DE ESTADO (para memoizacion)
// ============================================================
inline std::string claveEstado(const Senda& s){
    std::string k; k.reserve(s.size()*3);
    for(int v:s){ k+=std::to_string(v); k+=','; }
    return k;
}

// ============================================================
//  RESOLVER — BFS + DP con memoizacion
//  Retorna la secuencia optima de jugadas, o vector vacío si no hay solución
// ============================================================
inline std::vector<Jugada> resolver(const Senda& inicial, int maxRondas=50) {
    std::unordered_set<std::string> visitados;
    std::queue<NodoBFS> cola;
    cola.push({inicial,{}});
    visitados.insert(claveEstado(inicial));
    int nodos=0;

    while(!cola.empty()){
        NodoBFS actual=cola.front(); cola.pop();
        nodos++;
        if((int)actual.camino.size()>=maxRondas) continue;

        for(auto& [sendaTrasJugador, jugada] : generarMovimientos(actual.senda)){
            Senda sendaFinal=simularFalsosMaestros(sendaTrasJugador);
            if(sendaFinal.empty()) continue; // derrota

            std::vector<Jugada> nuevoCamino=actual.camino;
            nuevoCamino.push_back(jugada);

            if(victoria(sendaFinal)){
                std::cout<<C_MAESTRO<<C_NEGRITA
                         <<"Solucion encontrada en "<<nuevoCamino.size()
                         <<" ronda(s). Estados explorados: "<<nodos
                         <<C_RESET<<"\n";
                return nuevoCamino;
            }

            std::string clave=claveEstado(sendaFinal);
            if(visitados.count(clave)) continue;
            visitados.insert(clave);
            cola.push({sendaFinal, nuevoCamino});
        }
    }
    std::cout<<C_FALSO<<"No se encontro solucion en "<<maxRondas
             <<" rondas. Estados explorados: "<<nodos<<C_RESET<<"\n";
    return {};
}