package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.representacion.nodos.operadores.NodoDivision;
import framework.calculo_reglas.representacion.nodos.operadores.NodoExponencial;
import framework.calculo_reglas.representacion.nodos.operadores.NodoLoge;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMultiplicacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoNegacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoPotencia;
import framework.calculo_reglas.representacion.nodos.operadores.NodoRaiz;
import framework.calculo_reglas.representacion.nodos.operadores.NodoResta;
import framework.calculo_reglas.representacion.nodos.operadores.NodoSuma;

public class NS1estrella extends NS1 {

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getIndicesAplicaExtensiones(monticulo);
		for (Integer i : I) {
			if (monticulo.nodos[i].esConstante())
				V1constantes(monticulo, i);
			else if (monticulo.nodos[i].esTerminal()) // && !monticulo.nodos[i].esConstante()
				V1terminales(monticulo, i);
			else if (monticulo.nodos[i].esUnaria())
				V1FUnarias(monticulo, i);
			else // es binaria
				V1FBinarias(monticulo, i);
		}
		return vecinos;
	}

	private void V1FBinarias(Regla monticulo, Integer i) {
		String simbolo = monticulo.nodos[i].simbolo();
		if (!simbolo.equals("*")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoMultiplicacion();
			vecinos.add(v1);
		}
		if (!simbolo.equals("/")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoDivision();
			vecinos.add(v1);
		}
		if (!simbolo.equals("+")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoSuma();
			vecinos.add(v1);
		}
		if (!simbolo.equals("-")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoResta();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.max")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoMax();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.min")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoMin();
			vecinos.add(v1);
		}
	}

	private void V1FUnarias(Regla monticulo, Integer i) {
		String simbolo = monticulo.nodos[i].simbolo();
		if (!simbolo.equals("-")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoNegacion();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.max")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoMax0();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.min")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoMin0();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.log")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoLoge();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.exp")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoExponencial();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.pow")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoPotencia();
			vecinos.add(v1);
		}
		if (!simbolo.equals("Math.sqrt")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoRaiz();
			vecinos.add(v1);
		}
	}
}
