package util;

import java.util.ArrayList;
import java.util.List;

import dominio.Instancia;
import framework.calculo_ensembles_colaborativos.Ensemble_Colaborativo;
import framework.calculo_reglas.Regla;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante;
import framework.core.AlgoUtil;

public class EnsembleTest {

	public static void main(String[] args) {
		AlgoUtil.iniciaEvaluadorReglas();
		List<Regla> reglas = new ArrayList<Regla>();

//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.5 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.1 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.2 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.3 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.4 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.5 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.6 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.7 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.8 null"));
//		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.9 null"));

		System.out.println("SSHE rule");
		reglas.add(new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - Math.pow intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null 0.5 null"));
		Regla referencia = new Regla().generarRegla("/ / - Math.min instancia.d[tarea] intervalo.inicio * + instancia.d[tarea] null null null null - 0.25 intervalo.inicio instancia.p[tarea] null null null null null null null null null null instancia.p[tarea] null null null");
		for (double d=0.0; d<=1.0; d=d+0.1) {
			Regla nueva = referencia.getClon();
			nueva.nodos[14] = new NodoConstante(d);
			reglas.add(nueva);
		}
		

//		System.out.println("ATC rule");
//		Regla atc = new Regla().generarATC(0.1);
//		for (double d=0.0; d<=1.0; d=d+0.1) {
//			Regla nueva = atc.getClon();
//			nueva.nodos[25] = new NodoConstante(d);
//			reglas.add(nueva);
//		}
		
		Ensemble_Colaborativo ensemble = new Ensemble_Colaborativo(reglas);
		System.out.println("Colaborative Trainning " + ensemble.train() + " test " + ensemble.test());
		
		double total = 0.0;
		double min;
		double value;
//
//		for (Regla regla : reglas) {
//			System.out.println("Trainning " + regla.train() + " test " + regla.test());
//		}
//
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
