package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import framework.calculo_reglas.local_search.NSabstract;
import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.Expresion;

public class NS2 extends NSabstract {

	protected static Map<Double, List<Regla>> cacheReglas;
	protected static List<Regla> expresiones;

	public NS2(int E) {
		inicializaEV2(E); // por defecto se generan todas las expresiones de tamaño 3!
	}
	
	/*
	 * Sobre todos los indidces de una regla
	 */
	public List<Regla> get(Regla regla) {
		vecinos.clear();
		List<Integer> I = getI(regla);
		for (Integer i : I) {
			expresiones = cacheReglas.get(regla.nodos[i].getDimension());
			if (expresiones != null)
				for (Regla expresion : expresiones)
					vecinos.add(generaVecino(regla, i, expresion));
		}
		return vecinos;
	}

	// es necesario revisar si esto genera bien las expresiones!->¿Se generan todas?
	public static void inicializaEV2(int tamañoMaximo) {
		cacheReglas = new HashMap<Double, List<Regla>>();
		expresiones = new ArrayList<Regla>();
		for (double dimension = -7.0; dimension <= 7.0; dimension += 0.25)
			cacheReglas.put(dimension, new ArrayList<Regla>());
		Pila ABIERTA = new Pila();
		List<Expresion> sucesores = new ArrayList<Expresion>();
		Expresion sucesor = new Expresion(sucesores, tamañoMaximo); // ojo con esto!
		for (Expresion estado : sucesores)
			ABIERTA.añadir(estado);
		while (!ABIERTA.vacia()) {
			sucesor = ABIERTA.get();
			if (sucesor.isCompleto())
				inserta(sucesor);
			if (sucesor.getId() > 0) {
				sucesor.expande(sucesores);
				for (Expresion estado : sucesores)
					ABIERTA.añadir(estado);
			}
		}
//		// prueba para ver que se genera todo ok
//		List<Regla> expresiones = cacheReglas.get(1.0);
//		for (Regla Regla : expresiones)
//			System.out.println(Regla.representa());
	}

	static void inserta(Expresion sucesor) {
		cacheReglas.get(sucesor.getDimension()).add(sucesor);
		expresiones.add(sucesor);
	}

	static Regla generaVecino(Regla regla, int i, Regla expresion) {
//		Regla clon = regla.getClon();
//		clon.borrar(i);
//		clon.insertar(i, 0, expresion);
//		return clon;
		return new Regla(i, 0, regla, expresion);
	}
}

class Pila {

	Deque<Expresion> ABIERTA;

	public Pila() {
		ABIERTA = new ArrayDeque<Expresion>();
	}

	public boolean vacia() {
		return ABIERTA.isEmpty();
	}

	public void añadir(Expresion estado) {
		ABIERTA.push(estado);
	}

	public Expresion get() {
		return ABIERTA.pop();
	}

	public long tamaño() {
		return ABIERTA.size();
	}

	@Override
	public String toString() {
		return "Pila";
	}

}
