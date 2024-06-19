package util;

import java.util.ArrayList;
import java.util.List;

import dominio.Instancia;
import framework.calculo_reglas.Regla;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante;
import framework.core.AlgoUtil;

public class RuleTest {

	public static void main(String[] args) {
		AlgoUtil.iniciaEvaluadorReglas();
		List<Regla> reglas = new ArrayList<Regla>();

		System.out.println("SM-MGP rule");
		String stringRule = "+ - Math.min * - - intervalo.inicio instancia.p[tarea] 0.7 instancia.d[tarea] intervalo.inicio instancia.d[tarea] instancia.p[tarea] null null";
		
//		System.out.println(rule.train()+" "+rule.test());
		
		Regla referencia = new Regla().generarRegla(stringRule);
		Regla ATC = new Regla().generarATC(0);
		for (double d=0.0; d<=1.0; d=d+0.1) {
			Regla nueva = referencia.getClon();
			Regla ATCg = ATC.getClon();
			nueva.nodos[8] = new NodoConstante(d);
			ATCg.nodos[25] = new NodoConstante(d);
			System.out.println(d + ";" + nueva.train() + ";" + nueva.test()+  ";" + ATCg.train() + ";" + ATCg.test());
			reglas.add(nueva);
		}		
		
		double total = 0.0;
		double min;
		double value;

		for (Instancia instancia : AlgoUtil.gestorInstancias.training) {
			min = Double.MAX_VALUE;
			for (Regla regla : reglas) {
				value = regla.train(instancia.nombre);
				if (value < min)
					min = value;
			}
			total += min;
		}
		System.out.println("Entrenamiento:" + total / AlgoUtil.gestorInstancias.training.size());

		total = 0.0;
		for (Instancia instancia : AlgoUtil.gestorInstancias.set_test) {
			min = Double.MAX_VALUE;
			for (Regla regla : reglas) {
				value = regla.test(instancia.nombre);
				if (value < min)
					min = value;
			}
			total += min;
		}
		System.out.println("Coordinated Test:" + total / AlgoUtil.gestorInstancias.set_test.size());
	}
}
