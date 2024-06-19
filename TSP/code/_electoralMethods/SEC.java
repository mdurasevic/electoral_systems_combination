package _electoralMethods;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

import calculo_ensembles_colaborativos.Ensemble_Colaborativo;
import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class SEC {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] {
					"5", // card cola
					"6" // combination methods for SEC 
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = false;
		AlgoUtil.minutos = 9999;
		AlgoUtil.cardinalidadEnsembles = Integer.parseInt(args[0]);
		int combinationMethodSEC = Integer.parseInt(args[1]);

		String set = "21m";
		String setRules = "rulesElectoralMethods_" + set;
		AlgoUtil.trainingSetName = "InstancesTraining_" + set;

		int n = 1000;
		AlgoUtil.run("AnalysisEnsembles_CombinationMethods_" + n + "_" + setRules + "_"
				+ AlgoUtil.trainingSetName + "_" + AlgoUtil.cardinalidadEnsembles + "_" + combinationMethodSEC + "_"
				+ Ensemble_Colaborativo.names[combinationMethodSEC]);
		System.out.println(Ensemble_Colaborativo.names[combinationMethodSEC]);

		AlgoUtil.seleccionables = new ArrayList<Regla>(cargaFicheroReglas(setRules + ".csv"));
		AlgoUtil.cambiaConjuntoEntrenamiento();

		// generar el fichero con las reglas usadas
//		for (Regla r : AlgoUtil.seleccionables)
//			r.train();
//		Collections.sort(AlgoUtil.seleccionables);
//		StringBuilder file = new StringBuilder();
//		for (int i = 0; i < 50; i++)
////			System.out.println(AlgoUtil.seleccionables.get(i).train() + " " + AlgoUtil.seleccionables.get(i).test());
//			file.append(AlgoUtil.seleccionables.get(i).representaMonticulo() + ";"
//					+ AlgoUtil.seleccionables.get(i).train() + ";" + AlgoUtil.seleccionables.get(i).test() + ";\n");
//
//		guardarFichero(file.toString(), "rulesElectoralMethods_" + AlgoUtil.trainingSetName.split("_")[1]);

		AlgoUtil.combinationMethod = combinationMethodSEC;
		long time = System.currentTimeMillis();
		Ensemble_Colaborativo ensemble = run(n);
		time = System.currentTimeMillis() - time;
		AlgoUtil.vln(ensemble.train() + ";" + ensemble.test() +";" + time/1000);
		AlgoUtil.end();
	}

	public static Ensemble_Colaborativo run(int n) {
		Ensemble_Colaborativo ensemble, aux;
		ensemble = new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles).random(AlgoUtil.getSeleccionables());
//		System.out.println(1 + " " + ensemble.train() + " " + ensemble.test());
		for (int i = 2; i <= n; i++) {
			aux = new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles).random(AlgoUtil.getSeleccionables());
			if (aux.train() < ensemble.train()) {
				ensemble = aux;
//				System.out.println(i + " " + ensemble.train() + " " + ensemble.test());
			}
		}
		return ensemble;
	}

	public static Set<Regla> cargaFicheroReglas(String fichero) {
		Scanner kbd;
		String text;
		Set<Regla> reglas = new HashSet<Regla>();
		try {
			kbd = new Scanner(new File(fichero));
			while (kbd.hasNext()) {
				text = kbd.nextLine().split(";")[0];
				reglas.add(new Regla().generarRegla(text));
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return reglas;
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
