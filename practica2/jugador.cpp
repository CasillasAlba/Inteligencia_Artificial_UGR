#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <cstdlib>

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {


    Action sigAccion;

    //sensores.F y sensores.C me devuelve la posicion en la que estoy
    //en el reto, me devuelve -1 hasta que encuentro una casilla amarilla
	if(sensores.mensajeF != -1){
		//MensajeF muestra la fila del jugador
		fil = sensores.mensajeF;
		//MensajeC muestra la fila del jugador
		col = sensores.mensajeC;
		ultimaAccion = actIDLE;
		reactivo = false;

	}


    //Actualizar el efecto de la ultima accion
	//sensores no nos devuelve la orientacion, para esto, sabiendo que siempre emepzamos mirando hacia arriba
	//modificamos nuestra orientacion con respecto al ultimo giro
		switch (ultimaAccion){
		case actTURN_R:
			brujula = (brujula+1)%4;
		break;

		case actTURN_L:
			brujula = (brujula+3)%4;
		break;

		case actFORWARD:
			switch (brujula){
				case 0:
					fil--;
				break;

				case 1:
					col++;
				break;

				case 2:
					fil++;
				break;

				case 3:
					col--;
				break;
			}
		break;
	}

	if(sensores.nivel == 4 && !reactivo){
        PintaMapa(sensores);
	}

    if(reactivo){ //Para buscar una casilla amarilla
        
        for(int i = 1; i < sensores.terreno.size(); i++){
            if(sensores.terreno[i] == 'K'){
                hayPlan = buscaPK(i, plan);
            }
        }


    }else{
        // Mirar si ha cambiado el destino
        if ( sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
            cout << "El mapa ha cambiado" << endl;
            destino.fila = sensores.destinoF;
            destino.columna = sensores.destinoC;
            hayPlan = false;
        }

        // Calcular un camino hasta el destino
        if(!hayPlan && !reactivo){
            actual.fila = fil;
            actual.columna = col;
            actual.orientacion = brujula;
            hayPlan = pathFinding(sensores.nivel, actual, destino, plan);
        }
    }


        if(hayPlan && plan.size() > 0){

            sigAccion = plan.front();
             plan.erase(plan.begin());
            if((sensores.terreno[2] == 'P' || sensores.terreno[2] == 'M' || sensores.terreno[2] == 'D' || sensores.superficie[2] == 'a' ) && (sigAccion == actFORWARD)){
                sigAccion = actTURN_L;
                hayPlan = false;
            }


        }else{
          if(reactivo){
            hayPlan = false; //Para que recalcule el mapa
           
            if(cuentaPasos < 5){
                sigAccion = actFORWARD;
                cuentaPasos++;
            }else if(cuentaPasos == 5){
                int num = rand()%8;
                if(num == 0 || num == 2 || num == 4 || num == 6){
                    sigAccion = actTURN_L;
                }else if (num == 1 || num == 3 || num == 5 || num == 7){
                    sigAccion = actTURN_R;
                }
                
                cuentaPasos = 0;
                
            }

            if (sensores.terreno[2] == 'P' || sensores.terreno[2] == 'M' || sensores.terreno[2] == 'D' || sensores.superficie[2] == 'a'){
                sigAccion = actTURN_R;
		     }
          }

        }


	//Recordar la ultima accion
	ultimaAccion = sigAccion;

   return sigAccion;
}



void ComportamientoJugador::PintaMapa(Sensores sensores){
    int var_x, var_y;

    for (int i = 0; i < 15; i++){
        switch (i){
            case 0:
                var_x = 0; var_y = 0; break;

            case 1:
                var_x = -1;
                var_y = 1;
            break;

            case 2:
                var_x = 0;
                var_y = 1;
            break;

            case 3:
                var_x = 1;
                var_y = 1;
            break;

            case 4:
                var_x = -2;
                var_y = 2;
            break;

            case 5:
                var_x = -1;
                var_y = 2;
            break;

            case 6:
                var_x = 0;
                var_y = 2;
            break;

            case 7:
                var_x = 1;
                var_y = 2;
            break;

            case 8:
                var_x = 2;
                var_y = 2;
            break;

            case 9:
                var_x = -3;
                var_y = 3;
            break;

            case 10:
                var_x = -2;
                var_y = 3;
            break;

            case 11:
                var_x = -1;
                var_y = 3;
            break;

            case 12:
                var_x = 0;
                var_y = 3;
            break;

            case 13:
                var_x = 1;
                var_y = 3;
            break;

            case 14:
                var_x = 2;
                var_y = 3;
            break;

            case 15:
                var_x = 3;
                var_y = 3;
            break;

        }

            switch(brujula){
            case 0: //norte
                mapaResultado[fil - var_y][col + var_x] = sensores.terreno[i];
            break;

            case 1: //este
                mapaResultado[fil + var_x][col + var_y] = sensores.terreno[i];
            break;

            case 2: //sur
                mapaResultado[fil + var_y][col - var_x] = sensores.terreno[i];
            break;

            case 3: //oeste
                mapaResultado[fil - var_x][col - var_y] = sensores.terreno[i];
            break;

            }

    }


}


bool ComportamientoJugador::buscaPK(int posicion, list<Action> &plan){
	//Borro la listas
	plan.clear();    
    
    if( posicion == 1 || posicion == 2 || posicion == 3){
        plan.push_back(actFORWARD);
    }else if ( posicion < 9){
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }else{
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }
    
    if (posicion == 9){
        plan.push_back(actTURN_L);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }else if (posicion == 4 || posicion == 10){
        plan.push_back(actTURN_L);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }else if (posicion == 1 || posicion == 5 || posicion == 11){
        plan.push_back(actTURN_L);
        plan.push_back(actFORWARD);
    }else if (posicion == 15){
        plan.push_back(actTURN_R);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }else if (posicion == 8 || posicion == 14){
        plan.push_back(actTURN_R);
        plan.push_back(actFORWARD);
        plan.push_back(actFORWARD);
    }else if (posicion == 3 || posicion == 7 || posicion == 13){
        plan.push_back(actTURN_R);
        plan.push_back(actFORWARD);
    }
        
  return true;  
    
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
                return pathFinding_Costo_Uniforme(origen,destino,plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
                // Incluir aqui la llamada al algoritmo de búsqueda usado en el nivel 2
               //return pathFinding_Costo_Uniforme(origen,destino,plan);
                //return pathFinding_Profundidad(origen,destino,plan);
                 return pathFinding_AEstrella(origen, destino, plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

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


struct nodo{
	estado st;
	list<Action> secuencia;
	int coste;
	int prioridad;

};


struct ComparaCoste{
  bool operator()(const nodo & nodo1, const nodo & nodo2)const{
    return nodo1.coste > nodo2.coste;
  }
};


struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


int calculaCoste (const nodo & el_nodo, char el_terreno){
	int coste = 1;

	switch(el_terreno){
		case 'A':
			coste = 10;
		break;

		case 'T':
			coste = 2;
        break;

        case 'B':
            coste = 5;
        break;

        case '?':
            coste = 11;
        break;
	}

	return coste;
}

int calcularDistanciaManhattan(const estado & origen, const estado & destino){
    return abs(origen.fila - destino.fila) + abs(origen.columna - destino.columna);
}

struct ComparaManhattan{
  bool operator()(const nodo & nodo1, const nodo & nodo2)const{
    return nodo1.prioridad > nodo2.prioridad;
  }
};
// Implementacion de la busqueda en profundidad.
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
	current.coste = 0;
	current.prioridad = 0;

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



// Implementacion de la busqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;							  // Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.coste = 0;
	current.prioridad = 0;

	cola.push(current);

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!cola.empty()){
			current = cola.front();
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


// Implementacion de la busqueda de costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.

bool ComportamientoJugador::pathFinding_Costo_Uniforme(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodo, vector<nodo>, ComparaCoste> prioridad; // Lista de Abiertos
	//priority_queue<nodo> prioridad; // Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.prioridad = 0;
	current.secuencia.empty();
	char terreno = mapaResultado[current.st.fila][current.st.columna];
	current.coste = calculaCoste(current, terreno);

	prioridad.push(current);

	while (!prioridad.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		prioridad.pop();
		generados.insert(current.st);


		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		hijoTurnR.coste ++;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			prioridad.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
        hijoTurnL.coste ++ ;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			prioridad.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;

		terreno = mapaResultado[hijoForward.st.fila][hijoForward.st.columna];
		hijoForward.coste += calculaCoste(hijoForward, terreno);
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				prioridad.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!prioridad.empty()){
			current = prioridad.top();
		}

	}

	if(current.st.fila == destino.fila && current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		//ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}else{
		cout << "No encontrado plan\n" ;
	}

	return false;

}

bool ComportamientoJugador::pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan){
    	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodo, vector<nodo>, ComparaManhattan> prioridad; // Lista de Abiertos
	//priority_queue<nodo> prioridad; // Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();
	char terreno = mapaResultado[current.st.fila][current.st.columna];
	current.coste = calculaCoste(current, terreno);
	current.prioridad = current.coste + calcularDistanciaManhattan(current.st, destino);

	prioridad.push(current);

	while (!prioridad.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		prioridad.pop();
		generados.insert(current.st);


		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		hijoTurnR.coste ++;
		hijoTurnR.prioridad++;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			prioridad.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
        hijoTurnL.coste ++ ;
        hijoTurnL.prioridad++;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			prioridad.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;

		terreno = mapaResultado[hijoForward.st.fila][hijoForward.st.columna];
		hijoForward.coste += calculaCoste(hijoForward, terreno);
		hijoForward.prioridad = calcularDistanciaManhattan(hijoForward.st, destino) + hijoForward.coste;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				prioridad.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!prioridad.empty()){
			current = prioridad.top();
		}

	}

	if(current.st.fila == destino.fila && current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		//ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}else{
		cout << "No encontrado plan\n" ;
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
