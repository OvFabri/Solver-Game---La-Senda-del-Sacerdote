// ============================================================
//  senda_sacerdote.cpp — La Senda del Sacerdote
//  Juego interactivo en consola
//  Comando 's' durante tu turno activa el solver automatico
// ============================================================
#include "logica.h"
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <thread>

// ============================================================
//  IMPRIMIR SENDA VERTICAL (para el juego interactivo)
// ============================================================
void imprimirSendaV(const Senda& s) {
    std::cout << "\n" << C_NEGRITA
              << "══════════════════════════════════════════════════════\n"
              << C_RESET;
    std::cout << C_NEGRITA << "  SENDA  (pos : tipo : carta)\n" << C_RESET;
    std::cout << "──────────────────────────────────────────────────────\n";
    for(int i=0;i<(int)s.size();i++){
        std::string col, etq;
        if(s[i]==APRENDIZ)      { col=C_APRENDIZ; etq="[APRENDIZ] "; }
        else if(s[i]==AH_PUCH)  { col=C_DEIDAD;   etq="[DEIDAD]   "; }
        else if(esMaestro(s[i])){ col=C_MAESTRO;   etq="[MAESTRO]  "; }
        else                    { col=C_FALSO;     etq="[F.MAESTRO]"; }
        std::cout<<"  Pos "<<std::setw(2)<<i<<": "
                 <<col<<etq
                 <<" "<<std::setw(2)<<(s[i]==APRENDIZ||s[i]==AH_PUCH?0:s[i])
                 <<" - "<<std::setw(13)<<std::left<<nombreCarta(s[i])
                 <<C_RESET<<"\n";
    }
    std::cout<<"══════════════════════════════════════════════════════\n\n";
}

void imprimirHabilidades(){
    std::cout<<C_AMARILLO<<C_NEGRITA<<"\n  HABILIDADES DE MAESTROS:\n"<<C_RESET;
    std::cout<<"  5  Ejercicio    : Intercambia con carta a hasta 2 de distancia\n";
    std::cout<<"  7  Filosofia    : dist3->mueve1 / dist2->mueve2 / dist1->mueve3\n";
    std::cout<<"  8  Herbolaria   : Retrocede adyacente 2 / Adelanta adyacente 1\n";
    std::cout<<"  9  Meditacion   : Intercambia con carta exactamente a 3 espacios\n";
    std::cout<<" 10  Oracion      : Intercambia con carta adyacente\n";
    std::cout<<" 12  Peregrino    : Mueve adyacente exactamente 2 espacios\n";
    std::cout<<" 13  Magia        : Retrocede adyacente 3 / Adelanta adyacente 2\n";
    std::cout<<" 14  Cuerpo Astral: Intercambia con carta exactamente a 4 espacios\n\n";
}

// ============================================================
//  EJECUTAR SOLVER Y ANIMAR LA SOLUCION PASO A PASO
// ============================================================
void ejecutarSolver(Senda senda) {
    std::cout<<C_AMARILLO<<C_NEGRITA
             <<"\n[SOLVER] Buscando solucion optima...\n"<<C_RESET;

    auto solucion = resolver(senda);

    if(solucion.empty()){
        std::cout<<C_FALSO<<"[SOLVER] Esta configuracion no tiene solucion.\n"<<C_RESET;
        return;
    }

    std::cout<<"\n"<<C_NEGRITA<<"══ EJECUTANDO SOLUCION OPTIMA ══\n"<<C_RESET;
    std::cout<<"Presiona Enter para avanzar ronda a ronda (o escribe 'r' para correr todo rapido): ";
    std::string modo; std::getline(std::cin, modo);
    bool rapido = (modo=="r"||modo=="R");

    Senda actual = senda;
    for(int i=0;i<(int)solucion.size();i++){
        const Jugada& j = solucion[i];

        std::cout<<C_NEGRITA<<"\nRonda "<<(i+1)<<"/"<<solucion.size()<<":\n"<<C_RESET;

        // Aplicar movimiento del jugador
        for(auto& [ns, jj] : generarMovimientos(actual)){
            if(jj.descripcion==j.descripcion){ actual=ns; break; }
        }

        std::cout<<"  "<<C_MAESTRO<<"Jugador: "<<j.descripcion<<C_RESET<<"\n";
        std::cout<<"  Tras jugador:\n";
        imprimirSendaH(actual);

        // Simular falsos maestros con mensajes
        std::cout<<"  "<<C_FALSO<<"Falsos Maestros actuan..."<<C_RESET<<"\n";
        actual = simularFalsosMaestros(actual);
        std::cout<<"  Tras F.Maestros:\n";
        imprimirSendaH(actual);

        if(!rapido){
            std::cout<<"  [Enter para continuar]";
            std::cin.get();
        }
    }

    std::cout<<"\n"<<C_MAESTRO<<C_NEGRITA
             <<"¡VICTORIA! El Aprendiz conecta con Ah Puch en "
             <<solucion.size()<<" ronda(s).\n"<<C_RESET;
}

// ============================================================
//  TURNO DEL JUGADOR
// ============================================================
bool turnoJugador(Senda& senda, int ronda) {
    int n=(int)senda.size();
    std::cout<<C_MAESTRO<<C_NEGRITA<<"── Tu turno (Ronda "<<ronda<<") ──\n"<<C_RESET;
    std::cout<<"Maestros disponibles:\n";
    bool hayMaestro=false;
    for(int i=0;i<n;i++){
        if(esMaestro(senda[i])){
            hayMaestro=true;
            std::cout<<C_MAESTRO<<"  Pos "<<std::setw(2)<<i
                     <<": ["<<senda[i]<<"] "<<nombreCarta(senda[i])<<C_RESET<<"\n";
        }
    }
    if(!hayMaestro){
        std::cout<<C_FALSO<<"  No hay maestros disponibles.\n"<<C_RESET;
        return true;
    }

    std::cout<<"\nElige posicion del Maestro "
             <<"(h=habilidades, s=solver automatico, q=salir): ";
    std::string entrada; std::cin>>entrada;

    if(entrada=="q"||entrada=="Q") return false;
    if(entrada=="h"||entrada=="H"){ imprimirHabilidades(); return turnoJugador(senda,ronda); }
    if(entrada=="s"||entrada=="S"){
        ejecutarSolver(senda);
        return false; // el solver tomo el control, terminar modo manual
    }

    int posM;
    try{ posM=std::stoi(entrada); }
    catch(...){ std::cout<<"  Entrada invalida.\n"; return turnoJugador(senda,ronda); }

    if(posM<0||posM>=n||!esMaestro(senda[posM])){
        std::cout<<"  Posicion invalida o no es un Maestro.\n";
        return turnoJugador(senda,ronda);
    }

    int val=senda[posM];
    std::cout<<C_MAESTRO<<"  Activando: "<<nombreCarta(val)<<C_RESET<<"\n";

    if(val==5){
        std::cout<<"  Elige posicion a intercambiar (dist 1 o 2): ";
        int t; std::cin>>t;
        if(std::abs(t-posM)<1||std::abs(t-posM)>2||t<0||t>=n){
            std::cout<<"  Distancia invalida.\n"; return turnoJugador(senda,ronda);
        }
        senda=intercambiar(senda,posM,t);
    }
    else if(val==7){
        std::cout<<"  Elige posicion de carta a mover (dist 1-3 de pos "<<posM<<"): ";
        int t; std::cin>>t;
        int dist=std::abs(t-posM);
        if(dist<1||dist>3||t<0||t>=n){
            std::cout<<"  Distancia invalida.\n"; return turnoJugador(senda,ronda);
        }
        int mov=4-dist;
        std::cout<<"  Direccion (+ adelantar / - retroceder): ";
        char dir; std::cin>>dir;
        senda=moverCarta(senda,t,t+mov*(dir=='+'?1:-1));
    }
    else if(val==8){
        std::cout<<"  1) Retrocede adyacente 2  2) Adelanta adyacente 1\n  > ";
        int op; std::cin>>op;
        std::cout<<"  Elige posicion adyacente a "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=1||t<0||t>=n){
            std::cout<<"  No es adyacente.\n"; return turnoJugador(senda,ronda);
        }
        senda=moverCarta(senda,t,(op==1)?t-2:t+1);
    }
    else if(val==9){
        std::cout<<"  Elige posicion exactamente a 3 de pos "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=3||t<0||t>=n){
            std::cout<<"  Debe ser distancia exacta 3.\n"; return turnoJugador(senda,ronda);
        }
        senda=intercambiar(senda,posM,t);
    }
    else if(val==10){
        std::cout<<"  Elige posicion adyacente a "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=1||t<0||t>=n){
            std::cout<<"  No es adyacente.\n"; return turnoJugador(senda,ronda);
        }
        senda=intercambiar(senda,posM,t);
    }
    else if(val==12){
        std::cout<<"  Elige posicion adyacente a "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=1||t<0||t>=n){
            std::cout<<"  No es adyacente.\n"; return turnoJugador(senda,ronda);
        }
        std::cout<<"  Direccion (+ adelantar / - retroceder): ";
        char dir; std::cin>>dir;
        senda=moverCarta(senda,t,t+2*(dir=='+'?1:-1));
    }
    else if(val==13){
        std::cout<<"  1) Retrocede adyacente 3  2) Adelanta adyacente 2\n  > ";
        int op; std::cin>>op;
        std::cout<<"  Elige posicion adyacente a "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=1||t<0||t>=n){
            std::cout<<"  No es adyacente.\n"; return turnoJugador(senda,ronda);
        }
        senda=moverCarta(senda,t,(op==1)?t-3:t+2);
    }
    else if(val==14){
        std::cout<<"  Elige posicion exactamente a 4 de pos "<<posM<<": ";
        int t; std::cin>>t;
        if(std::abs(t-posM)!=4||t<0||t>=n){
            std::cout<<"  Debe ser distancia exacta 4.\n"; return turnoJugador(senda,ronda);
        }
        senda=intercambiar(senda,posM,t);
    }
    return true;
}

// ============================================================
//  TURNO FALSOS MAESTROS (con mensajes)
// ============================================================
void turnoFalsosMaestros(Senda& senda) {
    std::cout<<C_FALSO<<C_NEGRITA<<"── Turno de Falsos Maestros ──\n"<<C_RESET;
    for(int fm:{1,2,3,4,6,11}){
        int pos=buscar(senda,fm);
        if(pos==-1) continue;
        int n=(int)senda.size();
        std::cout<<C_FALSO<<"  ["<<fm<<"] "<<nombreCarta(fm)<<" (pos "<<pos<<") actua\n"<<C_RESET;

        Senda anterior=senda;
        if(fm==1){
            bool hayIzq=(pos-1>=0), hayDer=(pos+1<n);
            if(hayIzq&&hayDer) senda=intercambiar(senda,pos-1,pos+1);
            else { int d=hayDer?1:-1; senda=moverCarta(senda,pos,pos+d); }
        }
        else if(fm==2){
            int t=adyMenorValor(senda,pos);
            if(t!=-1){
                bool esAp=(senda[t]==APRENDIZ);
                std::cout<<"    Destruye a "<<nombreCarta(senda[t])<<"\n";
                senda=destruirCarta(senda,t);
                if(esAp){
                    std::cout<<C_FALSO<<C_NEGRITA
                             <<"\nGAME OVER: El Aprendiz fue destruido por el Rencor.\n"
                             <<C_RESET;
                }
            }
        }
        else if(fm==3){
            int t=adyMenorValor(senda,pos);
            if(t!=-1){ std::cout<<"    Manda "<<nombreCarta(senda[t])<<" al inicio\n"; senda=moverCarta(senda,t,1); }
        }
        else if(fm==4){
            int t=adyMenorValor(senda,pos);
            if(t!=-1){
                if(t-2<1){ int p=buscar(senda,4); if(p!=-1){ std::cout<<"    Pereza avanza 1\n"; senda=moverCarta(senda,p,p+1); } }
                else { std::cout<<"    Retrocede "<<nombreCarta(senda[t])<<" 2\n"; senda=moverCarta(senda,t,t-2); }
            }
        }
        else if(fm==6){
            int t=adyMenorValor(senda,pos), idxR=buscar(senda,2);
            if(t!=-1){
                if(t==idxR){ std::cout<<"    Rencor 2 hacia Aprendiz\n"; senda=moverCarta(senda,idxR,idxR-2); }
                else { int d=(idxR!=-1)?idxR-1:1; std::cout<<"    Manda "<<nombreCarta(senda[t])<<" antes del Rencor\n"; senda=moverCarta(senda,t,d); }
            }
        }
        else if(fm==11){
            int t=adyMenorValor(senda,pos), idxR=buscar(senda,2);
            if(t!=-1){
                if(t==idxR){ std::cout<<"    Rencor 2 hacia Aprendiz\n"; senda=moverCarta(senda,idxR,idxR-2); }
                else { int dir=(idxR==-1||idxR>t)?1:-1; std::cout<<"    Mueve "<<nombreCarta(senda[t])<<" 2 hacia Rencor\n"; senda=moverCarta(senda,t,t+2*dir); }
            }
        }
    }
}

// ============================================================
//  INICIALIZAR JUEGO
// ============================================================
Senda iniciarJuego(){
    std::vector<int> vals={1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    unsigned seed=std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(vals.begin(),vals.end(),std::default_random_engine(seed));
    Senda s;
    s.push_back(APRENDIZ);
    for(int v:vals) s.push_back(v);
    s.push_back(AH_PUCH);
    return s;
}

// ============================================================
//  MAIN
// ============================================================
int main(){
    std::cout<<C_MAESTRO<<C_NEGRITA;
    std::cout<<"╔══════════════════════════════════════════════╗\n";
    std::cout<<"║        LA SENDA DEL SACERDOTE                ║\n";
    std::cout<<"║        Un juego de Xibalba                   ║\n";
    std::cout<<"╚══════════════════════════════════════════════╝\n";
    std::cout<<C_RESET<<"\n";
    std::cout<<"Comandos: numero=seleccionar maestro | h=habilidades | s=solver | q=salir\n\n";
    std::cout<<"Presiona Enter para comenzar...";
    std::cin.ignore(); std::cin.get();

    Senda senda=iniciarJuego();
    int ronda=1;

    while(true){
        imprimirSendaV(senda);

        if(victoria(senda)){
            std::cout<<C_MAESTRO<<C_NEGRITA
                     <<"\nFELICIDADES! Te has convertido en Sacerdote en ronda "<<ronda<<".\n"
                     <<C_RESET;
            break;
        }
        if(derrota(senda)){
            std::cout<<C_FALSO<<C_NEGRITA<<"\nGAME OVER: El Aprendiz fue destruido.\n"<<C_RESET;
            break;
        }

        bool continuar=turnoJugador(senda,ronda);
        if(!continuar) break;

        if(victoria(senda)){
            // Los F.Maestros aun juegan
            turnoFalsosMaestros(senda);
            ronda++;
            imprimirSendaV(senda);
            if(victoria(senda))
                std::cout<<C_MAESTRO<<C_NEGRITA<<"\nFELICIDADES! Te has convertido en Sacerdote.\n"<<C_RESET;
            else
                std::cout<<C_FALSO<<"\nLos Falsos Maestros impidieron tu conexion con la Deidad...\n"<<C_RESET;
            break;
        }

        turnoFalsosMaestros(senda);
        ronda++;

        if(derrota(senda)){
            imprimirSendaV(senda);
            std::cout<<C_FALSO<<C_NEGRITA<<"\nGAME OVER: El Aprendiz fue destruido por el Rencor.\n"<<C_RESET;
            break;
        }
    }
    return 0;
}