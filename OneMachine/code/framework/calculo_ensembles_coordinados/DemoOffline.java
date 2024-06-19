package framework.calculo_ensembles_coordinados;

public class DemoOffline {

	public static void main(String[] args) {
		// tamaño de los ensembles
		int cardinalidadEnsembles = 10;

		// Carga la instancia
		String directorio = "conf/";
		String text = "OEPRP_all";
		String extension = ".csv";
		String ficherotrain = directorio + text + "_training" + extension;
		String ficherotest = directorio + text + "_test" + extension;

		Instancia instancia = new Instancia(ficherotrain, ficherotest);

		System.out.println("Matriz de " + instancia.getNreglas() + " reglas X " + instancia.getNinstanciasTraining() + " instancias entrenamiento");
		System.out.println("Matriz de " + instancia.getNreglas() + " reglas X " + instancia.getNinstanciasTest() + " instancias test");

		// Voraz
//		Algoritmo voraz = new Voraz(instancia, cardinalidadEnsembles);
//		Ensemble solucion_voraz = voraz.run();
//		System.out.println("Algoritmo voraz " + solucion_voraz.training() + " " + solucion_voraz.test() + " " + voraz.tiempo());

		System.out.println("Inicio GA");

		// Algoritmo genetico
		int generaciones = 500;
		int sizePoblacion = 100;
		double ratioCruce = 0.8;
		double ratioMutacion = 0.2;
		Algoritmo genetico = new AlgoritmoGenetico(instancia, generaciones, sizePoblacion, ratioCruce, ratioMutacion,
				cardinalidadEnsembles);
		Ensemble_Coordinado solucion_ga = genetico.run();
		System.out.println("Algoritmo genetico " + solucion_ga.train() + " " + solucion_ga.test() + " " + genetico.tiempo());

		// Algoritmo memetico
		double ratioLSA = 0.2;
		int iteraciones = 5;
		int sizeVecindario = 100;
		boolean HC = false;
//		Algoritmo memetico = new AlgoritmoMemetico(instancia, generaciones, sizePoblacion, ratioCruce, ratioMutacion, cardinalidadEnsembles, ratioLSA, iteraciones, sizeVecindario, HC);
//		Ensemble solucion_ma = memetico.run();
//		System.out.println("Algoritmo memetico " + solucion_ma.training() + " " + solucion_ma.test() + " " + memetico.tiempo());

		// memetico n veces
//		int n = 30;
//		Algoritmo memetico;
//		Ensemble solucion_ma;
//		for (int i=0; i<n; i++) {
//			memetico = new AlgoritmoMemetico(instancia, generaciones, sizePoblacion, ratioCruce, ratioMutacion, cardinalidadEnsembles, ratioLSA, iteraciones, sizeVecindario, HC);
//			solucion_ma = memetico.run();
//			System.out.println(solucion_ma.training() + ";" + solucion_ma.test() + ";" + memetico.tiempo());
//		}
	}
}
