package util;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class GeneraExperimentos2 {

	static int ejecuciones = 30;
	static int cola = 1;
	static int cola2 = 2;
	static String msg = "gp";

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
			int k = 0;
			for (String exp : experimentos) {
				if (k % 2 == 0)
					bw.write("\nqsub -binding linear:1 -pe make 1 -q all.q@slave" + cola + ".iscop " + exp + ".sh; sleep " + (getRandom()) + ";");
				else
					bw.write("\nqsub -binding linear:1 -pe make 1 -q all.q@slave" + cola2 + ".iscop " + exp + ".sh; sleep " + (getRandom()) + ";");
				k++;
			}
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static int getRandom() {
		return(int) (Math.random() * (41 - 30)) + 30;
	}

}
