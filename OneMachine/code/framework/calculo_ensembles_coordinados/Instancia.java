package framework.calculo_ensembles_coordinados;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public class Instancia {

	List<Regla> seleccionables;
	protected int[][] train;
	protected int[][] test;

	public Instancia() {
		train = new int[AlgoUtil.evaluadorReglas.getSeleccionables().size() + AlgoUtil.evaluadorEnsemblesColaborativos
				.getSeleccionables().size()][AlgoUtil.gestorInstancias.training.size()];

		seleccionables = new ArrayList<Regla>();
		
		for (int i = 0; i < AlgoUtil.evaluadorReglas.getSeleccionables().size(); i++)
			for (int j = 0; j < AlgoUtil.gestorInstancias.training.size(); j++)
				train[i][j] = AlgoUtil.evaluadorReglas.getSeleccionables().get(i).train(AlgoUtil.gestorInstancias.training.get(j).nombre);

		seleccionables.addAll(AlgoUtil.evaluadorReglas.getSeleccionables());
		
		for (int i = AlgoUtil.evaluadorReglas.getSeleccionables().size(); i < AlgoUtil.evaluadorReglas
				.getSeleccionables().size() + AlgoUtil.evaluadorEnsemblesColaborativos.getSeleccionables().size(); i++)
			
			for (int j = 0; j < AlgoUtil.gestorInstancias.training.size(); j++)
				train[i][j] = AlgoUtil.evaluadorEnsemblesColaborativos.getSeleccionables().get(i-AlgoUtil.evaluadorReglas.getSeleccionables().size())
										.train(AlgoUtil.gestorInstancias.training.get(j).nombre);
		
		seleccionables.addAll(AlgoUtil.evaluadorEnsemblesColaborativos.getSeleccionables());

	}

	public Instancia(String ficheroTrain, String ficheroTest) {
		train = cargaFichero(ficheroTrain);
		test = cargaFichero(ficheroTest);
	}

	public Instancia(int[][] train, int[][] test) {
		this.train = train;
		this.test = test;
	}

	public void intercambiaTrainTest() {
		int[][] copia = train;
		train = test;
		test = copia;
	}

	public int getNreglas() {
		return train.length;
	}

	public int getNinstanciasTraining() {
		return train[0].length;
	}

	public int getNinstanciasTest() {
		return test[0].length;
	}

	public int getReglaRandom() {
		return (int) (Math.random() * getNreglas());
	}

	int[][] cargaFichero(String fichero) {
		int[][] matriz;
		String datos = "";
		Scanner kbd;
		try {
			kbd = new Scanner(new File(fichero));
			while (kbd.hasNextLine())
				datos += kbd.nextLine() + "\n";
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		String[] lineas = datos.split("\n");
		String[] linea;
		matriz = new int[lineas.length][lineas[0].split(";").length - 1];
		for (int l = 0; l < lineas.length; l++) { // cada una de las lineas
			linea = lineas[l].split(";");
			for (int i = 1; i < linea.length; i++) // cada una de las columnas; la primera columna i=0 es el
													// método/regla/ensemble
				matriz[l][i - 1] = Integer.parseInt(linea[i]);
		}
		return matriz;
	}

	public int training(int regla, int instancia) {
		return train[regla][instancia];
	}

	public int test(int regla, int instancia) {
		return test[regla][instancia];
	}

	public void imprimeTest() {
		imprime(test);
	}

	public void imprimeTraining() {
		imprime(train);
	}

	private void imprime(int[][] matriz) {
		for (int regla = 0; regla < matriz.length; regla++) {
			for (int instancia = 0; instancia < matriz[0].length; instancia++)
				System.out.print(matriz[regla][instancia] + " ");
			System.out.println();
		}
	}

}