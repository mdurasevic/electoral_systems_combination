package calculo_reglas;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import domain.Graph;

public class MapaSolucionesRegla {
	
	public Map<Graph, Double> mapa = new HashMap<Graph, Double>();
	
	public void setSolucion(Graph instance, double coste) {
		mapa.put(instance, coste);
	}
	
	public double getSolucion(Graph instance) {
		return mapa.get(instance);
	}
	
	public Set<Entry<Graph, Double>> getSet() {
		return mapa.entrySet();
	}
	
	public int size() {
		return mapa.size();
	}
	
}
