package _experimentosElectoralMethods;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Scanner;
import java.util.Set;

import framework.calculo_ensembles_colaborativos.Ensemble;
import framework.calculo_ensembles_colaborativos.Ensemble_Colaborativo;
import framework.calculo_ensembles_colaborativos.PlanificadorEnsembles;
import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public class ComparisonCombinationMethodsRandom {

	public static String directorio = "reglas_electoralMethods";

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"3" // card cola
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = false;
		AlgoUtil.minutos = 9999;
		AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos = Integer.parseInt(args[0]);

		int n = 50;
		int id = new Random().nextInt();
		AlgoUtil.iniciaMedicionTiempo();
//		AlgoUtil.iniciaLogger("AnalysisEnsembles_CombinationMethods_" + methodString + "_" + n + "_" + directorio + "_"
		String fileName = "CombinationMethodsV2_" + directorio + "_" + n + "_"
				+ AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos;
		AlgoUtil.iniciaEvaluadorReglas();

//		loadRules();
		loadRulesInDirectorio();

		int nCombinationMethods = PlanificadorEnsembles.names.length;

		// para cada ensemble, sus soluciones con los combinationMethods
		List<List<Double>> train = new ArrayList<>();
		List<Ensemble> bestEnsemble = new ArrayList<>();
		List<Double> bestTrain = new ArrayList<>();
		for (int i = 0; i < nCombinationMethods; i++) {
			train.add(new ArrayList<Double>());
			bestEnsemble.add(null);
			bestTrain.add(Double.MAX_VALUE);
		}

		// los n ensembles random son usados con todos los combination methods on the
		// test set
		Ensemble ensemble;
		double coste;
		double avg2 = 0.0;
		for (int i = 0; i < n; i++) {
			ensemble = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos)
					.random(AlgoUtil.evaluadorReglas.getSeleccionables());
			int c=16; 
//			for (int c = 0; c < nCombinationMethods; c++) {
				AlgoUtil.gestorReglas.combinationMethod = c;
				ensemble.evalTrain = false;
				coste = ensemble.train();
				train.get(c).add(coste);
				if (coste < bestTrain.get(c)) {
					bestEnsemble.set(c, ensemble);
					bestTrain.set(c, coste);
				}
				System.out.println(coste);
				avg2 += coste;
//			}
		}
		System.out.println(avg2 /n);

		// cabecera
		StringBuilder cabecera = new StringBuilder();
		for (int c = 0; c < nCombinationMethods; c++)
			cabecera.append(PlanificadorEnsembles.getPlanificador(c) + ";");
		cabecera.append("\n");

		// una matriz de ensemble x combination method
		StringBuilder matrix = new StringBuilder();
		for (int i = 0; i < n; i++) {
			for (int c = 0; c < nCombinationMethods; c++) {
				matrix.append(train.get(c).get(i) + ";");
			}
			matrix.append("\n");
		}
		guardarFichero(cabecera.toString() + matrix.toString(), fileName + "_all" + "_" + id);

		// un fichero por cada estadistico
		// avg
		StringBuilder avg = new StringBuilder();
		double val = 0.0;
		for (int c = 0; c < nCombinationMethods; c++) {
			val = 0.0;
			for (int i = 0; i < n; i++) {
				val += train.get(c).get(i);
			}
			val = val / n;
			avg.append(val + ";");
		}
		avg.append("\n");
		guardarFichero(cabecera.toString() + avg.toString(), fileName + "_avgTrain" + "_" + id);
		
		// un fichero por cada estadistico
		// median
		StringBuilder median = new StringBuilder();
		val = 0.0;
		for (int c = 0; c < nCombinationMethods; c++) {
			val = train.get(c).get(train.get(c).size() / 2);
			median.append(val + ";");
		}
		median.append("\n");
		guardarFichero(cabecera.toString() + median.toString(), fileName + "_medianTrain" + "_" + id);

		// best ensemble in train per combination method
		StringBuilder bestTrainSB = new StringBuilder();
		for (int c = 0; c < nCombinationMethods; c++) {
			ensemble = bestEnsemble.get(c);
			AlgoUtil.gestorReglas.combinationMethod = c;
			ensemble.evalTrain = false;
			bestTrainSB.append(ensemble.train() + ";");
		}
		guardarFichero(cabecera.toString() + bestTrainSB.toString(), fileName + "_bestTrain" + "_" + id);

		// best ensemble in test per combination method
		StringBuilder bestTestSB = new StringBuilder();
		for (int c = 0; c < nCombinationMethods; c++) {
			ensemble = bestEnsemble.get(c);
			AlgoUtil.gestorReglas.combinationMethod = c;
			ensemble.evalTest = false;
			bestTestSB.append(ensemble.test() + ";");
		}
		guardarFichero(cabecera.toString() + bestTestSB.toString(), fileName + "_bestBestTTest" + "_" + id);
	}

	public static void loadRulesInDirectorio() {
		List<String> ficheros = cargarFicheros(directorio + "/");
		Set<String> strings = new HashSet<String>();
		int repetidos = 0;
		for (int i = 0; i < ficheros.size(); i++) {
			String[] lineasFichero = ficheros.get(i).split("\n");
			lineasFichero = lineasFichero[lineasFichero.length - 1].split(";");
			String regla = lineasFichero[lineasFichero.length - 1];
			if (!regla.equals("")) {
				if (strings.contains(regla)) {
					repetidos++;
				} else {
					AlgoUtil.evaluadorReglas.addSeleccionable(new Regla().generarRegla(regla));
				}
				strings.add(regla);
			}
		}
		System.out.println("Reglas repetidas " + repetidos);
	}

	static List<String> cargarFicheros(String directorio) {
		List<String> nombres = new ArrayList<String>();
		List<String> ficheros = new ArrayList<String>();
		File file = new File(directorio);
		if (file.isDirectory()) {
			File[] f = file.listFiles();
			for (File fichero : f)
				nombres.add(directorio + fichero.getName());
		}
		for (String fichero : nombres)
			ficheros.add(cargaFichero(fichero));
		return ficheros;
	}

	static String cargaFichero(String fichero) {
		Scanner kbd;
		String text = "";
		try {
			kbd = new Scanner(new File(fichero));
			while (kbd.hasNext()) {
				text += kbd.nextLine() + "\n";
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return text;
	}

	static void guardarFichero(String fichero, String nombre) {
		BufferedWriter bw;
		try {
			bw = new BufferedWriter(new FileWriter(new File(nombre + ".csv")));
			bw.write(fichero);
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
