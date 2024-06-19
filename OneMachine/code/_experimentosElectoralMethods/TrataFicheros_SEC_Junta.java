package _experimentosElectoralMethods;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import framework.calculo_ensembles_colaborativos.PlanificadorEnsembles;

import java.util.Scanner;

public class TrataFicheros_SEC_Junta {

	protected static String directorio = "_results" + "/";
	protected static int nColumnas;
	protected static int nEjecuciones = 30; // nEjecuciones debe ser igual para todos los experimentos!
	protected static int filaDatos = 0;
	protected static List<String> nombres = new ArrayList<String>();
	protected static List<String> ficheros = new ArrayList<String>();

	public static void main(String[] args) {
		// se cargan el nombre y contenido de los ficheros en el directorio
		cargarFicheros(directorio);
		// se trata el contenido de los ficheros
		String fichero = tratarFicheros(nombres, ficheros);
		// se guardan los datos tratados
		guardarFichero(fichero, "OMSP_data");
	}

	static String tratarFicheros(List<String> nombres, List<String> datos) {
		// se extraen los datos concretos y se agrupan
		Map<String, List<String[]>> datos_agrupados = new HashMap<String, List<String[]>>();
		int nParametros = 0;
		System.out.println(datos.size());
		String[] columnas = datos.get(0).split("\n")[filaDatos].split(";");
		String nombre;
		String[] pars;
		int n = 0;
		for (int i = 0; i < nombres.size(); i++) {
			pars = nombres.get(i).replace(directorio, "").split("_");
			nParametros = pars.length - 3; // porque no consideramos la semilla, ni fecha ni hora
			nombre = "";
//			for (int j = 0; j < nParametros; j++)
			nombre = pars[5] + ";" + pars[6] + ";"+ pars[7] + ";" + pars[8] + ";";
			if (!datos_agrupados.containsKey(nombre))
				datos_agrupados.put(nombre, new ArrayList<String[]>());
			n++;
			datos_agrupados.get(nombre).add(datos.get(i).split("\n")[datos.get(i).split("\n").length-1].split(";"));
		}

		// se tratan los datos: best, avg, ... todas las metricas
		String fichero = "Size;CombinationMethod;Train;Test;Time(s)\n";

		double value;
		String[] line;
		for (Entry<String, List<String[]>> entry : datos_agrupados.entrySet()) {
			line = entry.getKey().split(";");
			for (int ejecucion = 0; ejecucion < nEjecuciones; ejecucion++) {
				System.out.println(Integer.parseInt(line[1]));
				fichero += line[0] + ";"  + PlanificadorEnsembles.names[Integer.parseInt(line[1])] + ";" + line[2] + ";" + line[3] + ";";
				for (int columna = 0; columna < columnas.length-1; columna++) { // -1 si lo ultimo es el monticulo
					try {
						fichero += Double.parseDouble(entry.getValue().get(ejecucion)[columna]) + ";";
					} catch (Exception e) {
						System.out.println("-->"+line[0] + ";" + PlanificadorEnsembles.names[Integer.parseInt(line[1])] + ";" + ejecucion +" " +columna);
	//					value = Double.MAX_VALUE;
					}
				}
				fichero += "\n";
			}
		}
		return fichero;
	}

	static double desviacion(double[] v) {
		double prom, sum = 0;
		int i, n = v.length;
		prom = promedio(v);
		for (i = 0; i < n; i++)
			sum += Math.pow(v[i] - prom, 2);
		return Math.sqrt(sum / (double) n);
	}

	static double promedio(double[] v) {
		double prom = 0.0;
		for (int i = 0; i < v.length; i++)
			prom += v[i];
		return prom / (double) v.length;
	}

	static void cargarFicheros(String directorio) {
		// se carga el directorio donde estan los ficheros
		File file = new File(directorio);
		// se saca el nombre de cada fichero
		if (file.isDirectory()) {
			File[] ficheros = file.listFiles();
			for (File fichero : ficheros)
				nombres.add(directorio + fichero.getName());
		}
		// se carga el contenido de cada fichero
		for (String fichero : nombres)
			ficheros.add(cargaFichero(fichero));
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
