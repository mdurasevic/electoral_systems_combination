package framework.calculo_reglas.local_search.estructuras_vecindad;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.NSabstract;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_1;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_2;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_8;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_9;
import framework.calculo_reglas.representacion.nodos.operadores.NodoExponencial;
import framework.calculo_reglas.representacion.nodos.operadores.NodoLoge;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoNegacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoResta;
import framework.calculo_reglas.representacion.nodos.operadores.NodoSuma;
import framework.calculo_reglas.representacion.nodos.terminales.NodoDValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoInicioIntervaloValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPmediaValue;
import framework.core.AlgoUtil;

public class NS1 extends NSabstract{

	public List<Regla> get(Regla monticulo) {
		vecinos.clear();
		List<Integer> I = getI(monticulo);
		for (Integer i : I) {
			if (monticulo.nodos[i].esConstante())
				V1constantes(monticulo, i);
			if (monticulo.nodos[i].esTerminal() && !monticulo.nodos[i].esConstante())
				V1terminales(monticulo, i);
			if (monticulo.nodos[i].esUnaria()) {
				if (esUnariaNoAlteraDimension(monticulo.nodos[i]))
					V1FUnariasNAD(monticulo, i);
				if (esUnariaAdimensional(monticulo.nodos[i]))
					V1FUnariasAdimensional(monticulo, i);
			} else {
				if (esBinariaNoAlteraDimension(monticulo.nodos[i]))
					V1FBinariasNAD(monticulo, i);
			}
		}
		return vecinos;
	}

	// hemos cambiado los limite, no consideramos 0.0 y 1.0 en la gramatica
	void V1constantes(Regla monticulo, int i) {
		if (monticulo.nodos[i].value() > 0.1 && monticulo.nodos[i].value() < 0.9) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = AlgoUtil.gestorReglas
					.getConstante(monticulo.nodos[i].value() - AlgoUtil.gestorReglas.pasoConstantes);
			Regla v2 = monticulo.getClon();
			v2.nodos[i] = AlgoUtil.gestorReglas
					.getConstante(monticulo.nodos[i].value() + AlgoUtil.gestorReglas.pasoConstantes);
			vecinos.add(v1);
			vecinos.add(v2);
		} else if (monticulo.nodos[i].value() == 0.9) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoConstante0_8.getInstance();
			vecinos.add(v1);
		} else if (monticulo.nodos[i].value() == 1.0) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoConstante0_9.getInstance();
			vecinos.add(v1);
		} else if (monticulo.nodos[i].value() == 0.1) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoConstante0_2.getInstance();
			vecinos.add(v1);
		} else if (monticulo.nodos[i].value() == 0.0) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoConstante0_1.getInstance();
			vecinos.add(v1);
		} else { // si no esta en el rango... qué??? pues restamos/sumamos 0.1...
			System.out.println("constante fuera de rango");
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoConstante(
					new BigDecimal(monticulo.nodos[i].value() - AlgoUtil.gestorReglas.pasoConstantes)
							.setScale(1, RoundingMode.HALF_EVEN).doubleValue());
			Regla v2 = monticulo.getClon();
			v2.nodos[i] = new NodoConstante(
					new BigDecimal(monticulo.nodos[i].value() + AlgoUtil.gestorReglas.pasoConstantes)
							.setScale(1, RoundingMode.HALF_EVEN).doubleValue());
			vecinos.add(v1);
			vecinos.add(v2);
		}
	}

	void V1terminales(Regla monticulo, int i) {
		String simbolo = monticulo.nodos[i].simbolo();
		if (!simbolo.equals("instancia.d[tarea]")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoDValue.getInstance();
			vecinos.add(v1);
		}
		if (!simbolo.equals("promedioDuraciones()")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoPmediaValue.getInstance();
			vecinos.add(v1);
		}
		if (!simbolo.equals("instancia.p[tarea]")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoPValue.getInstance();
			vecinos.add(v1);
		}
		if (!simbolo.equals("intervalo.inicio")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = NodoInicioIntervaloValue.getInstance();
			vecinos.add(v1);
		}
	}

	void V1FBinariasNAD(Regla monticulo, int i) {
		String simbolo = monticulo.nodos[i].simbolo();
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

	void V1FUnariasNAD(Regla monticulo, int i) {
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
	}

	void V1FUnariasAdimensional(Regla monticulo, Integer i) {
		String simbolo = monticulo.nodos[i].simbolo();
		if (!simbolo.equals("Math.log")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoLoge();
			vecinos.add(v1);
		} else if (!simbolo.equals("Math.exp")) {
			Regla v1 = monticulo.getClon();
			v1.nodos[i] = new NodoExponencial();
			vecinos.add(v1);
		}
	}

	boolean esUnariaAdimensional(Nodo nodo) {
		switch (nodo.simbolo()) {
		case "Math.log":
			return true;
		case "Math.exp":
			return true;
		default:
			return false;
		}
	}

	boolean esUnariaNoAlteraDimension(Nodo nodo) {
		if (nodo.esUnaria() && nodo.simbolo().equals("-"))
			return true;
		else if (nodo.esUnaria() && nodo.simbolo().equals("max"))
			return true;
		else if (nodo.esUnaria() && nodo.simbolo().equals("min"))
			return true;
		else
			return false;
	}

	boolean esBinariaNoAlteraDimension(Nodo nodo) {
		if (nodo.simbolo().equals("+"))
			return true;
		else if (!nodo.esUnaria() && nodo.simbolo().equals("-"))
			return true;
		else if (!nodo.esUnaria() && nodo.simbolo().equals("max"))
			return true;
		else if (!nodo.esUnaria() && nodo.simbolo().equals("min"))
			return true;
		else
			return false;
	}
}
