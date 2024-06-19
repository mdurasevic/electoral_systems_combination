package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.NSabstract;

public class NS4 extends NSabstract{

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getI(monticulo);
		for (Integer i : I) {
			// es +, -, max o min y, por tanto, ambos hijos tienen la misma dimension
			if (monticulo.nodos[i].esOperacion() && !monticulo.nodos[i].esUnaria() && !monticulo.nodos[i].noAplicaRestriccionesDimension()) {
				vecinos.add(new Regla(i, 2 * i + 1, monticulo, monticulo));
				vecinos.add(new Regla(i, 2 * i + 2, monticulo, monticulo));
			}
		}
		return vecinos;
	}

}
