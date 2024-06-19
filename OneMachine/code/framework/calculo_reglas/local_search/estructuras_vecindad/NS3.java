package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.NSabstract;
import framework.calculo_reglas.representacion.nodos.operadores.NodoExponencial;
import framework.calculo_reglas.representacion.nodos.operadores.NodoLoge;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoNegacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoPotencia;
import framework.calculo_reglas.representacion.nodos.operadores.NodoRaiz;

public class NS3 extends NSabstract{

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getI(monticulo);
		for (Integer i : I) {
			if (monticulo.nodos[i].getDimension() == 0) {
				try {
					Regla exp = monticulo.getClon();
					exp.nodos[i] = new NodoExponencial();
					exp.insertar(2 * i + 1, i, monticulo);
					exp.borrar(2 * i + 2);

					Regla ln = monticulo.getClon();
					ln.nodos[i] = new NodoLoge();
					ln.insertar(2 * i + 1, i, monticulo);
					ln.borrar(2 * i + 2);

					Regla max0 = monticulo.getClon();
					max0.nodos[i] = new NodoMax0();
					max0.insertar(2 * i + 1, i, monticulo);
					max0.borrar(2 * i + 2);

					Regla min0 = monticulo.getClon();
					min0.nodos[i] = new NodoMin0();
					min0.insertar(2 * i + 1, i, monticulo);
					min0.borrar(2 * i + 2);

					Regla sqrt = monticulo.getClon();
					sqrt.nodos[i] = new NodoRaiz();
					sqrt.insertar(2 * i + 1, i, monticulo);
					sqrt.borrar(2 * i + 2);

					Regla pow2 = monticulo.getClon();
					pow2.nodos[i] = new NodoPotencia();
					pow2.insertar(2 * i + 1, i, monticulo);
					pow2.borrar(2 * i + 2);

					Regla neg = monticulo.getClon();
					neg.nodos[i] = new NodoNegacion();
					neg.insertar(2 * i + 1, i, monticulo);
					neg.borrar(2 * i + 2);

					vecinos.add(exp);
					vecinos.add(ln);
					vecinos.add(max0);
					vecinos.add(min0);
					vecinos.add(sqrt);
					vecinos.add(pow2);
					vecinos.add(neg);
				} catch (ArrayIndexOutOfBoundsException e) {
				}
			}
		}
		return vecinos;
	}

}
