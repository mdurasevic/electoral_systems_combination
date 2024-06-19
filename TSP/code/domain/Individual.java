package domain;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Individual {
	private List<Point> representation = new ArrayList<Point>();
	private double coste;

	public Individual(List<Point> representation) {
		this.representation = Collections.unmodifiableList(representation);
	}

	public Individual(List<Point> individualRepresentation, double computedDistance) {
		this(individualRepresentation);
		coste = computedDistance;
	}

	public List<Point> getRepresentation() {
		return representation;
	}
	
	public Point[] getPoints() {
		Point[] points = new Point[representation.size()];
		for (int i = 0; i < representation.size() - 1; i++)
			points[i] = representation.get(i);
		return points;
	}
	

	public boolean valida() {
		Set<Point> conjunto = new HashSet<Point>();
		for (int i = 0; i < representation.size() - 1; i++) // no repetidos
			if (conjunto.contains(representation.get(i))) {
				System.out.println("Contiene repetidos");
				return false;
			} else
				conjunto.add(representation.get(i));
		if (!representation.get(0).equals(representation.get(representation.size() - 1))) {
			System.out.println("Distinta ciudad inicio fin");
			return false;
		}
		if (conjunto.size() + 1 != representation.size()) {
			System.out.println("No visitó todas las ciudades");
			return false;
		}
		return true;
	}
	
	public Individual getClon() {
		return new Individual(getRepresentation(), coste);
	}

	public int length() {
		return representation.size();
	}

	public double getCoste() {
		return this.coste;
	}

	public void setCoste(double coste) {
		this.coste = coste;
	}

//	public double evaluateIndividual(Graph instance) {
//		double costeAcumulado = 0;
//		int previous = representation.get(0);
//		int selected;
//		for (int i = 1; i < representation.size(); i++) {
//			selected = representation.get(i);
//			costeAcumulado += instance.getEdge(previous, selected);
//			previous = selected;
//		}
//		return costeAcumulado;
//	}

	@Override
	public String toString() {
		return representation.toString();
	}

}