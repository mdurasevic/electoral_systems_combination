package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.NSabstract;
import framework.calculo_reglas.representacion.nodos.operadores.NodoRaiz;

public class NS6 extends NSabstract {

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getI(monticulo);
		for (Integer i : I) {
			// es / y der izq es 2 veces la dim der,
			// pow2(d) / p --> a) sqrt(pow2(d)) y b) p
			if (monticulo.nodos[i].orden() == 17
					&& 2.0 * monticulo.nodos[2 * i + 1].getDimension() == monticulo.nodos[2 * i + 2].getDimension()) {

				Regla a = monticulo.getClon();
				a.nodos[i] = new NodoRaiz();
				a.borrar(2 * i + 2);
				a.dimensionSimboloEn(i);

				Regla b = new Regla(i, 2 * i + 2, monticulo, monticulo);

				vecinos.add(a);
				vecinos.add(b);
			}
		}
		return vecinos;
	}

}
