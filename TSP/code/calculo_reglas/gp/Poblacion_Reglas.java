package calculo_reglas.gp;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class Poblacion_Reglas {

	List<Regla> poblacion;
	int sizeMax;
	Regla mejor;

	public Poblacion_Reglas(int size) {
		poblacion = new ArrayList<Regla>(size);
		sizeMax = size;
	}
	
	public void cambia(List<Regla> reglas) {
		poblacion = reglas;
		mejor = null;
	}

	public void generaRandom() {
		poblacion.clear();
		for (int i = 0; i < sizeMax; i++) {
			poblacion.add(new Regla().random());
//			System.out.println(poblacion.get(i).representa());
		}
	}

	public Regla getBest() {
		if (mejor == null) {
			mejor = get(0);
			for (Regla cromosoma : poblacion) {
//				System.out.println(cromosoma.representa());
				if (AlgoUtil.timeOut())
					break;
				if (AlgoUtil.mejoraTraining(cromosoma, mejor))
					mejor = cromosoma;
			}
		}
		return mejor;
	}

	public Collection<? extends Regla> getBest(int n) {
		List<Regla> bests = new ArrayList<>();
		Collections.sort(poblacion, new Comparator<Regla>() {
			@Override
			public int compare(Regla p1, Regla p2) {
				return new Double(p1.train()).compareTo(p2.train());
			}
		});
		for (int i = 0; i < n; i++) {
			bests.add(poblacion.get(i).getClon());
		}
		return bests;
	}

	public void reiniciaEvaluacion() {
		mejor = null;
		for (Regla cromosoma : poblacion) {
			cromosoma.evalTrain = false;
		}
	}

	public Regla mejorParaValidacion() {
		for (Regla cromosoma : poblacion) {
			cromosoma.evalTrain = false;
			if (AlgoUtil.mejoraTraining(cromosoma, mejor))
				mejor = cromosoma;
		}
		return mejor;
	}

	public double getAvg() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += poblacion.get(i).train();
		return 1.0 * coste / poblacion.size();
	}

	public double getSizeMedio() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += poblacion.get(i).getSize();
		return 1.0 * coste / poblacion.size();
	}

	public double getProfundidadMedia() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += poblacion.get(i).getProfundidad();
		return 1.0 * coste / poblacion.size();
	}

	public Regla get(int index) {
		return poblacion.get(index);
	}

	public void add(Regla cromosoma) {
		poblacion.add(cromosoma);
		mejor = null;
	}
	
	public void clearMetrics() {
		mejor = null;
		for (Regla cromosoma : poblacion) 
			cromosoma.evalTrain = false;
	}

	public void set(int i, Regla cromosoma) {
		poblacion.set(i, cromosoma);
		mejor = null;
	}

	public int size() {
		return poblacion.size();
	}

	public void remove(Regla cromosoma) {
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

	public Regla getRandom() {
		return poblacion.get((int) (poblacion.size() * Math.random()));
	}

//	public void marcaSinEvaluar() {
//		for (int i = 0; i < poblacion.size(); i++) {
//			poblacion.get(i).evalTrain = false;
//			poblacion.get(i).evalTest = false;
//		}
//
//	}

	public void guardaPop() {
		AlgoUtil.vln("Train;Rule;BinaryHeap;");	
		for (Regla regla : poblacion)
			AlgoUtil.vln(regla.train + ";" + regla.representa() + ";" + regla.representaMonticulo());
	}

	public void generaHeuristica() {
		if (AlgoUtil.seleccionables.size()  < sizeMax) {
			for (int i = 0; i < AlgoUtil.seleccionables.size(); i++)
				add(AlgoUtil.seleccionables.get(i));
			for (int i = AlgoUtil.seleccionables.size(); i < sizeMax; i++)
				add(new Regla().random());
		} else {
			Collections.shuffle(AlgoUtil.seleccionables);
			for (int i = 0; i < sizeMax; i++)
				add(AlgoUtil.seleccionables.get(i));
			
		}
	}




}
