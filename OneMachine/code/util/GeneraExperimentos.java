package util;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class GeneraExperimentos {

	static int ejecuciones = 1;
	static int cola = 3;
	static String msg = "sshe";

	public static void main(String[] args) {
		String fichero = cargaFichero("conf/ejecuciones.txt");
		String[] lineas = fichero.split("\n");
		String[] experimentos = new String[lineas.length * ejecuciones];
		int k = 0;
		for (int i = 0; i < lineas.length; i++) {
			for (int j = 0; j < ejecuciones; j++) {
				guardarFicheroExperimento(msg + k, lineas[i]);
				experimentos[k] = msg + k;
				k++;
			}
		}
		guardarFicheroExperimentos(experimentos);
	}

	public static String cargaFichero(String fichero) {
		Scanner kbd;
		String text = "";
		try {
			kbd = new Scanner(new File(fichero));
			while (kbd.hasNext()) {
				text += kbd.nextLine().replace("\t", " ") + "\n";
			}
//			System.out.println(text);
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return text;
	}

	public static void guardarFicheroExperimento(String nombre, String experimento) {
		BufferedWriter bw;
		try {
			bw = new BufferedWriter(new FileWriter(new File(nombre + ".sh")));
			bw.write("#!/bin/bash\n");
			bw.write("jdk1.8.0_202/bin/java -Xms1024m -Xmx2048m  -jar " + msg + ".jar " + experimento);
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void guardarFicheroExperimentos(String[] experimentos) {
		BufferedWriter bw;
		try {
			bw = new BufferedWriter(new FileWriter(new File("experimentos_" + msg + ".sh")));
			bw.write("#!/bin/bash");
			for (String exp : experimentos)
				bw.write("\nqsub -binding linear:1 -pe make 1 -q all.q@slave"+cola+".iscop " + exp + ".sh; sleep " + getRandom() + ";");
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private static int getRandom() {
		return(int) (Math.random() * (41 - 30)) + 30;
	}

}
