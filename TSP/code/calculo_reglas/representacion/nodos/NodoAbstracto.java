package calculo_reglas.representacion.nodos;

import calculo_reglas.representacion.Nodo;

public abstract class NodoAbstracto implements Nodo {

	@Override
	public boolean equals(Object obj) {
		if (obj == null)
			return false;
		return orden() == ((Nodo) obj).orden();
	}

	@Override
	public String toString() {
		return simbolo();
	}
}