package framework.calculo_reglas.local_search;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Random;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.estructuras_vecindad.NS1;
import framework.calculo_reglas.local_search.estructuras_vecindad.NS1estrella;
import framework.calculo_reglas.local_search.estructuras_vecindad.NS2;
import framework.calculo_reglas.local_search.estructuras_vecindad.NS2estrella;

public class GestorNS {
	
	public int E = 3; // tamaño de las subexpresiones
	public List<NSabstract> estructuras;
	public List<Integer> indices;
	List<Regla> N = new ArrayList<Regla>();
	
	public GestorNS() {
		estructuras = new ArrayList<NSabstract>();
		// estructuras de vecindad que se utilizan
		estructuras.add(new NS1());
		estructuras.add(new NS1estrella()); 
		estructuras.add(new NS2(E));
		estructuras.add(new NS2estrella(E));
	}
			
	public List<Regla> getVecinos(Regla regla) {
		for (NSabstract ns : estructuras)
			N.addAll(ns.get(regla));
		return N;
	}
	
	public List<Regla> getVecinos(Regla regla, int maximo) {
		HashSet<Regla> unicos = new HashSet<>();
		List<Integer> I = getI(regla);
		Random r = new Random();
		Regla vecino;
		int maxIntentos = 1000;
		int i=0;
		while (unicos.size() < maximo && i < maxIntentos) {
			vecino = generaVecino(regla, I.get(r.nextInt(I.size())), N.get(r.nextInt(N.size())));
			if (vecino.esValido())
				unicos.add(vecino);
			i++;
		}
		return new ArrayList<>(unicos);
	}
	

	Regla generaVecino(Regla regla, int i, Regla expresion) {
		return new Regla(i, 0, regla, expresion);
	}
	
	
	// por defecto todos los indices
	List<Integer> getI(Regla monticulo) {
		List<Integer> I = new ArrayList<Integer>();
		for (int i = 0; i < monticulo.nodos.length; i++)
			if (monticulo.nodos[i] != null)
				I.add(i);
		return I;
	}
	
}
