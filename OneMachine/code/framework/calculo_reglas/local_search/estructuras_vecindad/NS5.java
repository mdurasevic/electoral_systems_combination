package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.NSabstract;
import framework.calculo_reglas.representacion.nodos.operadores.NodoPotencia;

public class NS5 extends NSabstract{

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getI(monticulo);
		for (Integer i : I) {
			// x entonces se cambia por pow y cualquiera de sus hijos
			if (monticulo.nodos[i].orden() == 10
					&& monticulo.nodos[2 * i + 1].getDimension() == monticulo.nodos[2 * i + 2].getDimension()) {
				Regla izq = new Regla(i, i, monticulo, monticulo);
				izq.borrar(2 * i + 2);
				izq.nodos[i] = new NodoPotencia();
				izq.dimensionSimboloEn(i);
				// se intercambian los subarboles
				Regla der = new Regla(2 * i + 1, 2 * i + 2, monticulo, monticulo);
				der.borrar(2 * i + 2);
				der.nodos[i] = new NodoPotencia();
				der.dimensionSimboloEn(i);
				vecinos.add(der);
				vecinos.add(izq);
			}
		}
		return vecinos;
	}

}
