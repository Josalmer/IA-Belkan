/*
IA-Practica 2
*/
#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {


	//Detectar mensaje y capturarlo
	if(sensores.mensajeF != -1 && !estoy_bien_situado){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
		estoy_bien_situado = true;

		// Level 2: Vuelca lo que ha guardado en mapa provisional al mapa real
		// Actualizar efecto de ultima accion en mapa auxiliar
		if (sensores.nivel == 4){
			if (ultimaAccion == actFORWARD){
				switch(brujula) {
					case 0: filAux--; break;
					case 1: colAux++; break;
					case 2: filAux++; break;
					case 3: colAux--; break;
				}
			}
			int conversionFila = filAux - fil;
			int conversionCol = colAux - col;
			for (int m = 0; m < tamMap; m++){
				for (int n = 0; n < tamMap; n++){
					mapaResultado[m][n] = mapaProvisional[(m + conversionFila)][(n + conversionCol)];
				}
			}
		}
		//------------------------------------------------------------
		ultimaAccion = actIDLE;
	}
	//--------------------------------------------------------------

	//Actualizar efecto de ultima accion
	switch(ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula = (brujula+3)%4; break;
		case actFORWARD:
			switch(brujula) {
				case 0: fil--; filAux--; break;
				case 1: col++; colAux++; break;
				case 2: fil++; filAux++; break;
				case 3: col--; colAux--; break;
			}
		break;
	}
	//--------------------------------------------------------------

	// Comprobar destino
	if(sensores.destinoF != destino.fila || sensores.destinoC != destino.columna){
		destino.fila = sensores.destinoF;
		destino.columna = sensores.destinoC;
		hayPlan = false;
	}
	//--------------------------------------------------------------

	// Level 2: Dibujar mapa que se descubre Level 2
	if (sensores.nivel == 4 && estoy_bien_situado){ // Estoy en el reto
		int x = fil;
		int y = col;
		descubrirMapRes(x, y, sensores, tamMap);
	}
	//--------------------------------------------------------------

	// Level 2: Guarda lo que va viendo en un mapa auxiliar para despues ir a el
	if(sensores.nivel == 4 && !estoy_bien_situado){
		int x = fil;
		int y = col;
		descubrirMapProv(x, y, sensores, 198);
	}
	//--------------------------------------------------------------

	//Calcular plan a destino
	if(!hayPlan && estoy_bien_situado){
		actual.fila = fil;
		actual.columna = col;
		actual.orientacion = brujula;
		hayPlan = pathFinding(sensores.nivel, actual, destino, plan);
	}

	// Sistema de Movimiento------------------------------------------------------------------------
	Action sigAccion;

	// Level 2: Accion Doble
	if (accionDoble && !estoy_bien_situado && (sensores.terreno[2]!='P' && sensores.terreno[2]!='M' && sensores.terreno[2]!='D')){
		if(sigAccion == actFORWARD && sensores.superficie[2]=='a'){//Si hay aldeano esperar
			sigAccion=actIDLE;
		} else {
			accionDoble = false;
			sigAccion = actFORWARD;
		}
		ultimaAccion = sigAccion;
		return(sigAccion);
	}
	//--------------------------------------------------------------

	// Level 2: He encontrado PK y hay que trazar plan a objetivo
	if(hayPlan && plan.size() == 0){ // He encontrado PK
		hayPlan = false;
	}
	//--------------------------------------------------------------

	//Seguir plan con cuidado de aldeanos
	if(hayPlan && plan.size()>0){
		sigAccion = plan.front();
		if(sigAccion == actFORWARD && sensores.superficie[2]=='a'){//Si hay aldeano esperar
			sigAccion=actIDLE;
		} else { //Si no hay aldeano o se iba a girar, seguir plan
			if (sigAccion == actFORWARD && (sensores.terreno[2]=='P' || sensores.terreno[2]=='M' || sensores.terreno[2]=='D')){
				sigAccion = actIDLE;
				hayPlan = false;
				PKvisto = false;
			} else {
				plan.erase(plan.begin());
			}
		}
	}
	//--------------------------------------------------------------

	// Si no hay plan se activa comportamiento reactivo
	else {

		// Level 2: busqueda de punto PK
		if(sensores.nivel == 4 && !estoy_bien_situado){
			//Si alguno de los sensores terreno detecta el PK voy directo a el si es posible
			if(!PKvisto){
				for (int l = 1; l < 16; l++){
					if (sensores.terreno[l] == 'K'){
						PKvisto = true;
						posicionPK = l;
					}
				}
			}
			if(PKvisto){ // Si he visto el PK voy directo a el
				PKvisto = false;
				plan.clear();
				switch (posicionPK) {
					case 1:
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 2:
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 3:
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 4:
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 5:
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 6:
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 7:
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 8:
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 9:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 10:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 11:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_L);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 12:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 13:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 14:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
					case 15:
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actTURN_R);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						plan.push_back(actFORWARD);
						hayPlan = true; accionDoble = false; break;
				}
			}
			//--------------------------------------------------------------

			// Implemenacion del metodo Pulgarcito explicado en clase por el profesor
			// Si los sensores terreno ven casilla no transitable le ponen un valor alto para que no se elijan como destino
			if(sensores.terreno[2]=='P' || sensores.terreno[2]=='M' || sensores.terreno[2]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-1][colAux] = 3001; break; // No visitar
					case 1: mapaAux[filAux][colAux+1] = 3001; break; // No visitar
					case 2: mapaAux[filAux+1][colAux] = 3001; break; // No visitar
					case 3: mapaAux[filAux][colAux-1] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[1]=='P' || sensores.terreno[1]=='M' || sensores.terreno[1]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-1][colAux-1] = 3001; break; // No visitar
					case 1: mapaAux[filAux-1][colAux+1] = 3001; break; // No visitar
					case 2: mapaAux[filAux+1][colAux-1] = 3001; break; // No visitar
					case 3: mapaAux[filAux+1][colAux-1] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[3]=='P' || sensores.terreno[3]=='M' || sensores.terreno[3]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-1][colAux+1] = 3001; break; // No visitar
					case 1: mapaAux[filAux+1][colAux+1] = 3001; break; // No visitar
					case 2: mapaAux[filAux+1][colAux-1] = 3001; break; // No visitar
					case 3: mapaAux[filAux-1][colAux-1] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[4]=='P' || sensores.terreno[4]=='M' || sensores.terreno[4]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-2][colAux-2] = 3001; break; // No visitar
					case 1: mapaAux[filAux-2][colAux+2] = 3001; break; // No visitar
					case 2: mapaAux[filAux+2][colAux-2] = 3001; break; // No visitar
					case 3: mapaAux[filAux+2][colAux-2] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[8]=='P' || sensores.terreno[8]=='M' || sensores.terreno[8]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-2][colAux+2] = 3001; break; // No visitar
					case 1: mapaAux[filAux+2][colAux+2] = 3001; break; // No visitar
					case 2: mapaAux[filAux+2][colAux-2] = 3001; break; // No visitar
					case 3: mapaAux[filAux-2][colAux-2] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[9]=='P' || sensores.terreno[9]=='M' || sensores.terreno[9]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-3][colAux-3] = 3001; break; // No visitar
					case 1: mapaAux[filAux-3][colAux+3] = 3001; break; // No visitar
					case 2: mapaAux[filAux+3][colAux-3] = 3001; break; // No visitar
					case 3: mapaAux[filAux+3][colAux-3] = 3001; break; // No visitar
				}
			}
			if(sensores.terreno[15]=='P' || sensores.terreno[15]=='M' || sensores.terreno[15]=='D'){ // no visitar
				switch (brujula) {
					case 0: mapaAux[filAux-3][colAux+3] = 3001; break; // No visitar
					case 1: mapaAux[filAux+3][colAux+3] = 3001; break; // No visitar
					case 2: mapaAux[filAux+3][colAux-3] = 3001; break; // No visitar
					case 3: mapaAux[filAux-3][colAux-3] = 3001; break; // No visitar
				}
			}
			if(ultimaAccion == actFORWARD){ // Si ha avanzado a nueva casilla, actualizo el contador
				contador++;
				mapaAux[filAux][colAux] = contador;
			}
			sigAccion = metodoPulgarcito(filAux, colAux, brujula, accionDoble);

			/*// Para pruebas, imprime los valores del mapa auxiliar por donde se va desplazando el agente
			for (int h = (filAux - 5); h < (filAux + 5); h++){
				cout << endl;
				for (int j = (colAux - 5); j < (colAux + 5); j++){
					cout << mapaAux[h][j] << "\t";
				}
			}
			cout << endl << endl;
			//------------------------------------------------------------------------------------
*/
			// Por si algo falla y se le va olla girando
			if (sigAccion != actFORWARD){
				cuentaGiros++;
				if (cuentaGiros >= 3){
					sigAccion = actFORWARD;
					cuentaGiros = 0;
				}
			} else {
				cuentaGiros = 0;
			}
			//------------------------------------------------------------------------------------


			if (sigAccion == actFORWARD && (sensores.terreno[2]=='P' || sensores.terreno[2]=='M' || sensores.terreno[2]=='D')){
						sigAccion = actTURN_R;
					}	else if(sigAccion == actFORWARD && sensores.superficie[2]=='a'){//Esperar a que se quite el aldeano
							sigAccion = actIDLE;
					}
		}
		//--------------------------------------------------------------

		// Comportamiento reactivo basico
		else if (sensores.terreno[2]=='P' || sensores.terreno[2]=='M' || sensores.terreno[2]=='D'){
					sigAccion = actTURN_R;
				}
				else if(sensores.superficie[2]=='a'){//Esperar a que se quite el aldeano
					sigAccion = actIDLE;
				}
				else{
					sigAccion = actFORWARD;
				}
				//--------------------------------------------------------------
	}
	//---------------------------------------------------------------------------------------------

	// Para evitar atrapar a un aldeano en el camino sin que el mismo se pueda mover
	if(sigAccion == actIDLE){
		cuentaIddle++;
		if (cuentaIddle > 20){ // Demasiado tiempo esperando
			hayPlan = false;
			cuentaIddle = 0;
		}
	} else {
		cuentaIddle = 0;
	}
	//--------------------------------------------------------------

	//Recordar ultima accion
	ultimaAccion = sigAccion;
	return sigAccion;
}

// Para Level 2, comportamiento reactivo
Action ComportamientoJugador::metodoPulgarcito(int filAux, int colAux, int compass, bool &accionDoble){
	struct opcion{
		int visitado;
		int visitado2;
		int ejeX;
		int ejeY;
		Action accion;
		bool operator<(const opcion &otro)const{
			return visitado < otro.visitado;
		}
	};
	// Genero opciones, ordenadas por tiempo de ultima visita
	multiset <opcion> mejorOpcion;
	multiset <opcion>::iterator it;
	opcion aux, opcionA, opcionB;
	Action aDevolver, provisional;
	switch (compass) {
		case 0: // Norte
			aux.ejeX = filAux-1;
			aux.ejeY = colAux;
			aux.accion = actFORWARD;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux;
			aux.ejeY = colAux-1;
			aux.accion = actTURN_L;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux;
			aux.ejeY = colAux+1;
			aux.accion = actTURN_R;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			break;
		case 1: // Este
			aux.ejeX = filAux;
			aux.ejeY = colAux+1;
			aux.accion = actFORWARD;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux-1;
			aux.ejeY = colAux;
			aux.accion = actTURN_L;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux+1;
			aux.ejeY = colAux;
			aux.accion = actTURN_R;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			break;
		case 2: // Sur
			aux.ejeX = filAux+1;
			aux.ejeY = colAux;
			aux.accion = actFORWARD;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux;
			aux.ejeY = colAux-1;
			aux.accion = actTURN_R;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux;
			aux.ejeY = colAux+1;
			aux.accion = actTURN_L;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			break;
		case 3: // Oeste
			aux.ejeX = filAux;
			aux.ejeY = colAux-1;
			aux.accion = actFORWARD;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux-1;
			aux.ejeY = colAux;
			aux.accion = actTURN_R;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			aux.ejeX = filAux+1;
			aux.ejeY = colAux;
			aux.accion = actTURN_L;
			aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
			mejorOpcion.insert(aux);
			break;
	}
	// Me quedo con la mejor opcion
	it = mejorOpcion.begin();
	opcionA = (*it);

	if(opcionA.visitado > 0){ // Ver si a distancia 2 hay sin visitar
		mejorOpcion.clear();
		switch (compass) {
			case 0: // Norte
				aux.ejeX = filAux-2;
				aux.ejeY = colAux;
				aux.accion = actFORWARD;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX + 1][aux.ejeY];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux;
				aux.ejeY = colAux+2;
				aux.accion = actTURN_R;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY - 1];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux;
				aux.ejeY = colAux-2;
				aux.accion = actTURN_L;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY + 1];
				mejorOpcion.insert(aux);
				break;
			case 1: // Este
				aux.ejeX = filAux;
				aux.ejeY = colAux+2;
				aux.accion = actFORWARD;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY - 1];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux+2;
				aux.ejeY = colAux;
				aux.accion = actTURN_R;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX - 1][aux.ejeY];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux-2;
				aux.ejeY = colAux;
				aux.accion = actTURN_L;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX + 1][aux.ejeY];
				mejorOpcion.insert(aux);
				break;
			case 2: // Sur
				aux.ejeX = filAux+2;
				aux.ejeY = colAux;
				aux.accion = actFORWARD;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX - 1][aux.ejeY];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux;
				aux.ejeY = colAux-2;
				aux.accion = actTURN_R;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY + 1];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux;
				aux.ejeY = colAux+2;
				aux.accion = actTURN_L;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY - 1];
				mejorOpcion.insert(aux);
				break;
			case 3: // Oeste
				aux.ejeX = filAux;
				aux.ejeY = colAux-2;
				aux.accion = actFORWARD;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX][aux.ejeY + 1];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux-2;
				aux.ejeY = colAux;
				aux.accion = actTURN_R;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX + 1][aux.ejeY];
				mejorOpcion.insert(aux);
				aux.ejeX = filAux+2;
				aux.ejeY = colAux;
				aux.accion = actTURN_L;
				aux.visitado = mapaAux[aux.ejeX][aux.ejeY];
				aux.visitado2 = mapaAux[aux.ejeX - 1][aux.ejeY];
				mejorOpcion.insert(aux);
				break;
		}
		it = mejorOpcion.begin();
		opcionB = (*it);
		if ((opcionB.visitado < opcionA.visitado) && opcionB.visitado2 != 3001){
			aDevolver = opcionB.accion;
			accionDoble = true;
		}
	} else { // A distancia 1 mejor que distancia 2
		aDevolver = opcionA.accion;
	}

	return (aDevolver);
}

// Para Level 2, descubre mapa resultado segun donde mira la brujula
void ComportamientoJugador::descubrirMapRes(int x, int y, Sensores sensores, int tamano){
	modificarMapRes(x, y, sensores, 0, tamano);
	switch (brujula) {
		case 0: // Norte
			modificarMapRes((x-1),(y-1), sensores, 1, tamano);
			modificarMapRes((x-1),(y), sensores, 2, tamano);
			modificarMapRes((x-1),(y+1), sensores, 3, tamano);
			modificarMapRes((x-2),(y-2), sensores, 4, tamano);
			modificarMapRes((x-2),(y-1), sensores, 5, tamano);
			modificarMapRes((x-2),(y), sensores, 6, tamano);
			modificarMapRes((x-2),(y+1), sensores, 7, tamano);
			modificarMapRes((x-2),(y+2), sensores, 8, tamano);
			modificarMapRes((x-3),(y-3), sensores, 9, tamano);
			modificarMapRes((x-3),(y-2), sensores, 10, tamano);
			modificarMapRes((x-3),(y-1), sensores, 11, tamano);
			modificarMapRes((x-3),(y), sensores, 12, tamano);
			modificarMapRes((x-3),(y+1), sensores, 13, tamano);
			modificarMapRes((x-3),(y+2), sensores, 14, tamano);
			modificarMapRes((x-3),(y+3), sensores, 15, tamano);
			break;
		case 1: // Este
			modificarMapRes((x-1),(y+1), sensores, 1, tamano);
			modificarMapRes((x),(y+1), sensores, 2, tamano);
			modificarMapRes((x+1),(y+1), sensores, 3, tamano);
			modificarMapRes((x-2),(y+2), sensores, 4, tamano);
			modificarMapRes((x-1),(y+2), sensores, 5, tamano);
			modificarMapRes((x),(y+2), sensores, 6, tamano);
			modificarMapRes((x+1),(y+2), sensores, 7, tamano);
			modificarMapRes((x+2),(y+2), sensores, 8, tamano);
			modificarMapRes((x-3),(y+3), sensores, 9, tamano);
			modificarMapRes((x-2),(y+3), sensores, 10, tamano);
			modificarMapRes((x-1),(y+3), sensores, 11, tamano);
			modificarMapRes((x),(y+3), sensores, 12, tamano);
			modificarMapRes((x+1),(y+3), sensores, 13, tamano);
			modificarMapRes((x+2),(y+3), sensores, 14, tamano);
			modificarMapRes((x+3),(y+3), sensores, 15, tamano);
			break;
		case 2: // Sur
			modificarMapRes((x+1),(y+1), sensores, 1, tamano);
			modificarMapRes((x+1),(y), sensores, 2, tamano);
			modificarMapRes((x+1),(y-1), sensores, 3, tamano);
			modificarMapRes((x+2),(y+2), sensores, 4, tamano);
			modificarMapRes((x+2),(y+1), sensores, 5, tamano);
			modificarMapRes((x+2),(y), sensores, 6, tamano);
			modificarMapRes((x+2),(y-1), sensores, 7, tamano);
			modificarMapRes((x+2),(y-2), sensores, 8, tamano);
			modificarMapRes((x+3),(y+3), sensores, 9, tamano);
			modificarMapRes((x+3),(y+2), sensores, 10, tamano);
			modificarMapRes((x+3),(y+1), sensores, 11, tamano);
			modificarMapRes((x+3),(y), sensores, 12, tamano);
			modificarMapRes((x+3),(y-1), sensores, 13, tamano);
			modificarMapRes((x+3),(y-2), sensores, 14, tamano);
			modificarMapRes((x+3),(y-3), sensores, 15, tamano);
			break;
		case 3: // Oeste
			modificarMapRes((x+1),(y-1), sensores, 1, tamano);
			modificarMapRes((x),(y-1), sensores, 2, tamano);
			modificarMapRes((x-1),(y-1), sensores, 3, tamano);
			modificarMapRes((x+2),(y-2), sensores, 4, tamano);
			modificarMapRes((x+1),(y-2), sensores, 5, tamano);
			modificarMapRes((x),(y-2), sensores, 6, tamano);
			modificarMapRes((x-1),(y-2), sensores, 7, tamano);
			modificarMapRes((x-2),(y-2), sensores, 8, tamano);
			modificarMapRes((x+3),(y-3), sensores, 9, tamano);
			modificarMapRes((x+2),(y-3), sensores, 10, tamano);
			modificarMapRes((x+1),(y-3), sensores, 11, tamano);
			modificarMapRes((x),(y-3), sensores, 12, tamano);
			modificarMapRes((x-1),(y-3), sensores, 13, tamano);
			modificarMapRes((x-2),(y-3), sensores, 14, tamano);
			modificarMapRes((x-3),(y-3), sensores, 15, tamano);
			break;
	}
}

// Para modificar mapa resultado segun entrada de sensores.terreno
void ComportamientoJugador::modificarMapRes(int x, int y, Sensores sensores, int casilla, int limite){
	if(x >= 0 && x < limite && y >= 0 && y < limite){
		if(mapaResultado[x][y] == '?'){
				mapaResultado[x][y] = sensores.terreno[casilla];
		}
	}
}

// Para Level 2, descubre mapa resultado segun donde mira la brujula
void ComportamientoJugador::descubrirMapProv(int x, int y, Sensores sensores, int tamano){
	modificarMapProv(x, y, sensores, 0, tamano);
	switch (brujula) {
		case 0: // Norte
			modificarMapProv((x-1),(y-1), sensores, 1, tamano);
			modificarMapProv((x-1),(y), sensores, 2, tamano);
			modificarMapProv((x-1),(y+1), sensores, 3, tamano);
			modificarMapProv((x-2),(y-2), sensores, 4, tamano);
			modificarMapProv((x-2),(y-1), sensores, 5, tamano);
			modificarMapProv((x-2),(y), sensores, 6, tamano);
			modificarMapProv((x-2),(y+1), sensores, 7, tamano);
			modificarMapProv((x-2),(y+2), sensores, 8, tamano);
			modificarMapProv((x-3),(y-3), sensores, 9, tamano);
			modificarMapProv((x-3),(y-2), sensores, 10, tamano);
			modificarMapProv((x-3),(y-1), sensores, 11, tamano);
			modificarMapProv((x-3),(y), sensores, 12, tamano);
			modificarMapProv((x-3),(y+1), sensores, 13, tamano);
			modificarMapProv((x-3),(y+2), sensores, 14, tamano);
			modificarMapProv((x-3),(y+3), sensores, 15, tamano);
			break;
		case 1: // Este
			modificarMapProv((x-1),(y+1), sensores, 1, tamano);
			modificarMapProv((x),(y+1), sensores, 2, tamano);
			modificarMapProv((x+1),(y+1), sensores, 3, tamano);
			modificarMapProv((x-2),(y+2), sensores, 4, tamano);
			modificarMapProv((x-1),(y+2), sensores, 5, tamano);
			modificarMapProv((x),(y+2), sensores, 6, tamano);
			modificarMapProv((x+1),(y+2), sensores, 7, tamano);
			modificarMapProv((x+2),(y+2), sensores, 8, tamano);
			modificarMapProv((x-3),(y+3), sensores, 9, tamano);
			modificarMapProv((x-2),(y+3), sensores, 10, tamano);
			modificarMapProv((x-1),(y+3), sensores, 11, tamano);
			modificarMapProv((x),(y+3), sensores, 12, tamano);
			modificarMapProv((x+1),(y+3), sensores, 13, tamano);
			modificarMapProv((x+2),(y+3), sensores, 14, tamano);
			modificarMapProv((x+3),(y+3), sensores, 15, tamano);
			break;
		case 2: // Sur
			modificarMapProv((x+1),(y+1), sensores, 1, tamano);
			modificarMapProv((x+1),(y), sensores, 2, tamano);
			modificarMapProv((x+1),(y-1), sensores, 3, tamano);
			modificarMapProv((x+2),(y+2), sensores, 4, tamano);
			modificarMapProv((x+2),(y+1), sensores, 5, tamano);
			modificarMapProv((x+2),(y), sensores, 6, tamano);
			modificarMapProv((x+2),(y-1), sensores, 7, tamano);
			modificarMapProv((x+2),(y-2), sensores, 8, tamano);
			modificarMapProv((x+3),(y+3), sensores, 9, tamano);
			modificarMapProv((x+3),(y+2), sensores, 10, tamano);
			modificarMapProv((x+3),(y+1), sensores, 11, tamano);
			modificarMapProv((x+3),(y), sensores, 12, tamano);
			modificarMapProv((x+3),(y-1), sensores, 13, tamano);
			modificarMapProv((x+3),(y-2), sensores, 14, tamano);
			modificarMapProv((x+3),(y-3), sensores, 15, tamano);
			break;
		case 3: // Oeste
			modificarMapProv((x+1),(y-1), sensores, 1, tamano);
			modificarMapProv((x),(y-1), sensores, 2, tamano);
			modificarMapProv((x-1),(y-1), sensores, 3, tamano);
			modificarMapProv((x+2),(y-2), sensores, 4, tamano);
			modificarMapProv((x+1),(y-2), sensores, 5, tamano);
			modificarMapProv((x),(y-2), sensores, 6, tamano);
			modificarMapProv((x-1),(y-2), sensores, 7, tamano);
			modificarMapProv((x-2),(y-2), sensores, 8, tamano);
			modificarMapProv((x+3),(y-3), sensores, 9, tamano);
			modificarMapProv((x+2),(y-3), sensores, 10, tamano);
			modificarMapProv((x+1),(y-3), sensores, 11, tamano);
			modificarMapProv((x),(y-3), sensores, 12, tamano);
			modificarMapProv((x-1),(y-3), sensores, 13, tamano);
			modificarMapProv((x-2),(y-3), sensores, 14, tamano);
			modificarMapProv((x-3),(y-3), sensores, 15, tamano);
			break;
	}
}

// Para modificar mapa provisional segun entrada de sensores.terreno
void ComportamientoJugador::modificarMapProv(int x, int y, Sensores sensores, int casilla, int limite){
	if(x >= 0 && x < limite && y >= 0 && y < limite){
		if(mapaProvisional[x][y] == '?'){
				mapaProvisional[x][y] = sensores.terreno[casilla];
		}
	}
}

// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
						return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      return pathFinding_Anchura(origen,destino,plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
						return pathFinding_Costo(origen,destino,plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
						return pathFinding_BusquedaGuiada(origen,destino,plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}

//---------------------- Implementación de los algoritomos de busqueda ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

// Para costo uniforme y HdeN
bool ComportamientoJugador::avanzaEnCosto(estado &st, float &coste, Action accion){
	bool sePuede;
	if(accion == actFORWARD){
		int x = st.fila;
		int y = st.columna;
		// calculo cual es la casilla de delante del agente
		switch (st.orientacion) {
			case 0: x--; break;
			case 1: y++; break;
			case 2: x++; break;
			case 3: y--; break;
		}
		if ((x < 0 || x >= mapaResultado.size()) || (y < 0 || y >= mapaResultado[0].size())){
			sePuede = false;
		} else if(mapaResultado[x][y] == 'D' || mapaResultado[x][y] == 'P' || mapaResultado[x][y] == 'M'){
			sePuede = false;
		} else{
			st.fila = x;
			st.columna = y;
			switch (mapaResultado[x][y]) { // Pongo los costes
				case 'T': coste += 2; break; // Arenoso
				case 'B': coste += 5; break; // Bosque
				case 'A': coste += 10; break; // Agua
				case 'S': coste += 1; break; // suelo pedregoso
				case 'K': coste += 1; break; // PK
				case '?': coste += 4; break; // Inexistente
				break;
			}
			sePuede = true;
		}
	} else if(accion == actTURN_L){
		coste++;
		st.orientacion = (st.orientacion+3)%4;
		sePuede = true;
	} else if(accion == actTURN_R){
		coste++;
		st.orientacion = (st.orientacion+1)%4;
		sePuede = true;
	}
	return sePuede;
}

struct nodo{
	estado st;
	list<Action> secuencia;
	float coste; // para metodo del costo es el costo hasta n, para Busqueda guiada es g(n)+h(n)
  float GdeN; // costo hasta n
  float HdeN; // estimacion hasta objetivo
  bool operator<(const nodo &otro)const{
    return coste > otro.coste;
    // Le doy la vuelta para que se meta en la priority_queue con mas prioridad los costes bajos
  }
};

// Funcion heuristica
float funcionH(const estado &origen, const estado &destino){
	// Distancia de Euclides
	// float HdeN = (sqrt(pow(2,(origen.fila - destino.fila)) + pow(2,(origen.columna - destino.columna))));
	// Distancia Manhattan
	float HdeN = (abs(origen.fila - destino.fila) + abs(origen.columna - destino.columna));
	return (HdeN);
}

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}

};
/*
float funcionH(const estado &origen, const estado &destino)const{ // Funcion heuristica
	// Distancia de Euclides
	// float HdeN = (sqrt(pow(2,(origen.fila - destino.fila)) + pow(2,(origen.columna - destino.columna))));
	// Distancia Manhattan
	float HdeN = (abs(origen.fila - destino.fila) + abs(origen.columna - destino.columna));
	return (HdeN);
}
*/
// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> abiertos;									// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	abiertos.push(current);

  while (!abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		abiertos.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			abiertos.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la lista
		if (!abiertos.empty()){
			current = abiertos.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

// Implementación de la búsqueda con costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Costo(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; 	// Lista de Cerrados
	priority_queue<nodo> abiertos;			// Lista de Abiertos
	set<estado, ComparaEstados> estadosAbiertos;	// Estados en abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.coste = 0;

	abiertos.push(current);
	estadosAbiertos.insert(current.st);

  while (!abiertos.empty() && (current.st.fila!=destino.fila || current.st.columna != destino.columna)){

		abiertos.pop();
		estadosAbiertos.erase(current.st);
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		if(avanzaEnCosto(hijoTurnR.st, hijoTurnR.coste, actTURN_R)){
			if (generados.find(hijoTurnR.st) == generados.end()){
				if (estadosAbiertos.find(hijoTurnR.st) == estadosAbiertos.end()){
					hijoTurnR.secuencia.push_back(actTURN_R);
					abiertos.push(hijoTurnR);
					estadosAbiertos.insert(hijoTurnR.st);
				}
			}
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		if(avanzaEnCosto(hijoTurnL.st, hijoTurnL.coste, actTURN_L)){
			if (generados.find(hijoTurnL.st) == generados.end()){
				if (estadosAbiertos.find(hijoTurnL.st) == estadosAbiertos.end()){
					hijoTurnL.secuencia.push_back(actTURN_L);
					abiertos.push(hijoTurnL);
					estadosAbiertos.insert(hijoTurnL.st);
				}
			}
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if(avanzaEnCosto(hijoForward.st, hijoForward.coste, actFORWARD)){
			if (generados.find(hijoForward.st) == generados.end()){
				if (estadosAbiertos.find(hijoForward.st) == estadosAbiertos.end()){
					hijoForward.secuencia.push_back(actFORWARD);
					abiertos.push(hijoForward);
					estadosAbiertos.insert(hijoForward.st);
				}
			}
		}

		// Tomo el siguiente valor de la lista
		if (!abiertos.empty()){
			current = abiertos.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

// Implementación de la búsqueda para el nivel 2.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_BusquedaGuiada(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; 	// Lista de Cerrados
	priority_queue<nodo> abiertos;			// Lista de Abiertos
	set<estado, ComparaEstados> estadosAbiertos;	// Estados en abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.HdeN = 0;

	abiertos.push(current);
	estadosAbiertos.insert(current.st);

	while (!abiertos.empty() && (current.st.fila!=destino.fila || current.st.columna != destino.columna)){

		abiertos.pop();
		estadosAbiertos.erase(current.st);
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		if(avanzaEnCosto(hijoTurnR.st, hijoTurnR.GdeN, actTURN_R)){
			if (generados.find(hijoTurnR.st) == generados.end()){
				if (estadosAbiertos.find(hijoTurnR.st) == estadosAbiertos.end()){
					hijoTurnR.HdeN = funcionH(hijoTurnR.st, destino);
					hijoTurnR.coste = hijoTurnR.GdeN + hijoTurnR.HdeN;
					hijoTurnR.secuencia.push_back(actTURN_R);
					abiertos.push(hijoTurnR);
					estadosAbiertos.insert(hijoTurnR.st);
				}
			}
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		if(avanzaEnCosto(hijoTurnL.st, hijoTurnL.GdeN, actTURN_L)){
			if (generados.find(hijoTurnL.st) == generados.end()){
				if (estadosAbiertos.find(hijoTurnL.st) == estadosAbiertos.end()){
					hijoTurnL.HdeN = funcionH(hijoTurnL.st, destino);
					hijoTurnL.coste = hijoTurnL.GdeN + hijoTurnL.HdeN;
					hijoTurnL.secuencia.push_back(actTURN_L);
					abiertos.push(hijoTurnL);
					estadosAbiertos.insert(hijoTurnL.st);
				}
			}
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if(avanzaEnCosto(hijoForward.st, hijoForward.GdeN, actFORWARD)){
			if (generados.find(hijoForward.st) == generados.end()){
				if (estadosAbiertos.find(hijoForward.st) == estadosAbiertos.end()){
					hijoForward.HdeN = funcionH(hijoForward.st, destino);
					hijoForward.coste = hijoForward.GdeN + hijoForward.HdeN;
					hijoForward.secuencia.push_back(actFORWARD);
					abiertos.push(hijoForward);
					estadosAbiertos.insert(hijoForward.st);
				}
			}
		}

		// Tomo el siguiente valor de la lista
		if (!abiertos.empty()){
			current = abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
	}

// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
