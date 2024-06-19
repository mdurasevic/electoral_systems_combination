package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;

public class NS2estrella extends NS2 {

	public NS2estrella(int E) {
		super(E);
	}

	public List<Regla> get(Regla regla) {
		vecinos.clear();
		List<Integer> I = getIndicesAplicaExtensiones(regla);
		for (Integer i : I) {
			for (Regla expresion : expresiones)
				vecinos.add(generaVecino(regla, i, expresion));
		}
		return vecinos;
	}

}
