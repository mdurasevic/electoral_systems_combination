package framework.calculo_reglas.local_search;

import java.util.ArrayList;
import java.util.List;

import framework.calculo_reglas.Regla;

public abstract class NSabstract {

	protected List<Regla> vecinos = new ArrayList<Regla>();

	public abstract List<Regla> get(Regla monticulo);

	// por defecto todos los indices
	public List<Integer> getI(Regla monticulo) {
		List<Integer> I = new ArrayList<Integer>();
		for (int i = 0; i < monticulo.nodos.length; i++)
			if (monticulo.nodos[i] != null)
				I.add(i);
		return I;
	}
	
	
	public List<Integer> getIndicesAplicaExtensiones(Regla monticulo) {
		List<Integer> I = new ArrayList<Integer>();
		if (monticulo.nodos[0].noAplicaRestriccionesDimension()) {
			I.add(0);
			I.add(1);
			if (!monticulo.nodos[0].esUnaria())
				I.add(2);
			for (int i = 1; i < monticulo.nodos.length / 2; i++)
				if (monticulo.nodos[i] != null && !monticulo.nodos[i].esTerminal()
						&& monticulo.nodos[i].noAplicaRestriccionesDimension() && I.contains(i)
						&& 2 * i + 2 < monticulo.nodos.length) {
					I.add(2 * i + 1);
					if (!monticulo.nodos[i].esUnaria())
						I.add(2 * i + 2);
				}
		}
		return I;
	}

}
