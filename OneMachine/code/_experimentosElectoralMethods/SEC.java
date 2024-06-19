package _experimentosElectoralMethods;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Scanner;
import java.util.Set;

import framework.calculo_ensembles_colaborativos.Ensemble;
import framework.calculo_ensembles_colaborativos.Ensemble_Colaborativo;
import framework.calculo_ensembles_colaborativos.PlanificadorEnsembles;
import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public class SEC {

	public static String fichero = "best50set10";
	public static String directorio = "reglas_electoralMethods";

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"5", // card cola
					"6" // combination methods for SEC // suma=30,, oklahoma=23
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = false;
		AlgoUtil.minutos = 9999;
		AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos = Integer.parseInt(args[0]);
		int combinationMethodSEC = Integer.parseInt(args[1]);

		int n = 1000;
		AlgoUtil.iniciaMedicionTiempo();
		AlgoUtil.iniciaLogger("AnalysisEnsembles_CombinationMethods_" + n + "_" + directorio + "_" 
				+ AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos + "_" + combinationMethodSEC);
		AlgoUtil.iniciaEvaluadorReglas();

		System.out.println(PlanificadorEnsembles.names[combinationMethodSEC]);
		
		loadRulesInDirectorio();

		AlgoUtil.gestorReglas.combinationMethod = combinationMethodSEC;
		long inicio = System.currentTimeMillis();
		Ensemble ensemble = run(n);
		long fin = System.currentTimeMillis();
		AlgoUtil.vln(ensemble.train() + ";" + ensemble.test() + ";" + (fin - inicio) / 1000 + ";" + ensemble.representaMonticulo());
		AlgoUtil.end();
	}

	public static Ensemble run(int n) {
		Ensemble ensemble, aux;
		ensemble = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos)
				.random(AlgoUtil.evaluadorReglas.getSeleccionables());
//		System.out.println(1 + " " + ensemble.train() + " " + ensemble.test());
		for (int i = 2; i <= n; i++) {
			aux = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos)
					.random(AlgoUtil.evaluadorReglas.getSeleccionables());
			if (aux.train() < ensemble.train()) {
				ensemble = aux;
//				System.out.println(i + " " + ensemble.train() + " " + ensemble.test());
			}
		}
		return ensemble; 
	}

	public static void loadRules() {
		Scanner kbd;
		Set<String> strings = new HashSet<String>();
		int repetidos = 0;
		try {
			kbd = new Scanner(new File("conf/" + fichero + ".csv"));
			while (kbd.hasNextLine()) {
				String regla = kbd.nextLine();
				if (!regla.equals("")) {
					if (strings.contains(regla)) {
						repetidos++;
					} else {
						AlgoUtil.evaluadorReglas.addSeleccionable(new Regla().generarRegla(regla));
					}
					strings.add(regla);
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Reglas repetidas " + repetidos);
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
