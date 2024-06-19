package _electoralMethods;

import java.util.ArrayList;

import calculo_ensembles_colaborativos.Ensemble_Colaborativo;
import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class SEC_adicionalSCORE extends SEC {

	public static int minScore;
	public static int maxScore;

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"3", // card cola
					"1", // min
					"9", // max
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = false;
		AlgoUtil.minutos = 9999;
		AlgoUtil.cardinalidadEnsembles = Integer.parseInt(args[0]);
		int combinationMethodSEC = 30; // Score

		String set = "21m";
		String setRules = "rulesElectoralMethods_" + set;
		AlgoUtil.trainingSetName = "InstancesTraining_" + set;

		minScore = Integer.parseInt(args[1]);
		maxScore = Integer.parseInt(args[2]);

		int n = 1000;
		AlgoUtil.run("AnalysisEnsembles_CombinationMethods_" + n + "_" + setRules + "_" + AlgoUtil.trainingSetName + "_"
				+ +AlgoUtil.cardinalidadEnsembles + "_" + combinationMethodSEC + "_" + Integer.parseInt(args[1]) + "_"
				+ Integer.parseInt(args[2]));

		AlgoUtil.seleccionables = new ArrayList<Regla>(cargaFicheroReglas(setRules + ".csv"));
		AlgoUtil.cambiaConjuntoEntrenamiento();

		AlgoUtil.combinationMethod = combinationMethodSEC;
		long time = System.currentTimeMillis();
		Ensemble_Colaborativo ensemble = run(n);
		time = System.currentTimeMillis() - time;
		AlgoUtil.vln(ensemble.train() + ";" + ensemble.test() + ";" + time / 1000);
		AlgoUtil.end();
	}

}
