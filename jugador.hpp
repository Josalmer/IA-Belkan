/*
IA-Practica 2
*/
#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      estoy_bien_situado = false;
      hayPlan = false;
      tamMap = mapaResultado[5].size();
      for (int i = 0; i < 198; i++){
        for (int j = 0; j < 198; j++){
          mapaAux[i][j] = 0;
          mapaProvisional[i][j] = '?';
        }
      }
      filAux = 99;
      colAux = 99;
      contador = 0;
      accionDoble = false;
      PKvisto = false;
      posicionPK = 0;
      mapaVolcado = false;
      cuentaGiros = 0;
      cuentaIddle = 0;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      estoy_bien_situado = false;
      hayPlan = false;
      tamMap = mapaResultado[5].size();
      for (int i = 0; i < 198; i++){
        for (int j = 0; j < 198; j++){
          mapaAux[i][j] = 0;
          mapaProvisional[i][j] = '?';
        }
      }
      filAux = 99;
      colAux = 99;
      contador = 0;
      accionDoble = false;
      PKvisto = false;
      posicionPK = 0;
      mapaVolcado = false;
      cuentaGiros = 0;
      cuentaIddle = 0;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;

    //JOSE: nuevas variables de Estado
    Action ultimaAccion;
    bool estoy_bien_situado;
    bool hayPlan;
    int tamMap;
    int mapaAux[198][198];
    char mapaProvisional[198][198];
    int filAux;
    int colAux;
    int contador;
    bool accionDoble;
    bool PKvisto;
    int posicionPK;
    bool mapaVolcado;
    int cuentaGiros;
    int cuentaIddle;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Costo(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_BusquedaGuiada(const estado &origen, const estado &destino, list<Action> &plan); // Busqueda dirigida para reto2

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

    // JOSE: Métodos nuevos
    void descubrirMapRes(int x, int y, Sensores sensores, int tamano); // Para Level 2, descubre mapa
    void modificarMapRes(int x, int y, Sensores sensores, int casilla, int limite); // Para Level 2, modifica mapa resultado
    void descubrirMapProv(int x, int y, Sensores sensores, int tamano); // Para Level 2, guarda mapa provisional
    void modificarMapProv(int x, int y, Sensores sensores, int casilla, int limite); // Para Level 2, modifica mapa provisional
    Action metodoPulgarcito(int filAux, int colAux, int compass, bool &accionDoble); // Para level 2, movimiento reactivo
    bool avanzaEnCosto(estado &st, float &coste, Action accion); // Para costo uniforme y busqueda guiada
};

#endif
