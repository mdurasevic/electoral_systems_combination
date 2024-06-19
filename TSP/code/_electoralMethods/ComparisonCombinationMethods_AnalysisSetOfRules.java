package _electoralMethods;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class ComparisonCombinationMethods_AnalysisSetOfRules {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { "5", // card cola
					"6" // combination methods for SEC
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = false;
		AlgoUtil.minutos = 9999;
		String set = "21m";
		String setRules = "rulesElectoralMethods_" + set;
		AlgoUtil.trainingSetName = "InstancesTraining_" + set;

		AlgoUtil.run("AnalysisReglasCombinationMethods_" + setRules + "_" + AlgoUtil.trainingSetName);
		AlgoUtil.seleccionables = new ArrayList<Regla>(cargaFicheroReglas(setRules + ".csv"));
		AlgoUtil.cambiaConjuntoEntrenamiento();

		// generar el fichero con las reglas usadas
		StringBuilder file = new StringBuilder();
		for (int i = 0; i < 50; i++) {
			//System.out.println(AlgoUtil.seleccionables.get(i).train() + " " + AlgoUtil.seleccionables.get(i).test());
			file.append(AlgoUtil.seleccionables.get(i).train() + ";" + AlgoUtil.seleccionables.get(i).test() + ";"
					+ AlgoUtil.seleccionables.get(i).size + ";" + AlgoUtil.seleccionables.get(i).representa() + ";"
					+ AlgoUtil.seleccionables.get(i).representaMonticulo() + "\n");
		}
		System.out.println(file.toString());
		guardarFichero(file.toString(), "AnalysisRulesElectoralMethods_" + AlgoUtil.trainingSetName.split("_")[1]);
		AlgoUtil.end();
	}

	static Set<Regla> cargaFicheroReglas(String fichero) {
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
