package framework.core;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public abstract class Poblacion<T> {

	protected int sizeMax;
	protected List<T> poblacion;
	protected T mejor;

	public Poblacion(int size) {
		poblacion = new ArrayList<T>(size);
		sizeMax = size;
	}

	public T get(int index) {
		return poblacion.get(index);
	}

	public void add(T cromosoma) {
		poblacion.add(cromosoma);
		mejor = null;
	}

	public void set(int i, T cromosoma) {
		poblacion.set(i, cromosoma);
		mejor = null;
	}

	public int size() {
		return poblacion.size();
	}

	public void remove(T cromosoma) {
//		for (int i = 0; i < size(); i++)
//			if (poblacion.get(i).equals(cromosoma)) {
//				poblacion.set(i, null);
//				break;
//			}
//		List<T> aux = new ArrayList<T>();
//		for (int i = 0; i < size(); i++)
//			if (poblacion.get(i) != null)
//				aux.add(poblacion.get(i));
//		poblacion = aux;
		poblacion.remove(cromosoma);
		mejor = null;
	}

	public void shuffle() {
		Collections.shuffle(poblacion);
	}

	public abstract T getBest();

	public abstract double getAvg();

	public T getRandom() {
		return poblacion.get((int) (poblacion.size() * Math.random()));
	}

}
