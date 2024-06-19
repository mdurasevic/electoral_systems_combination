package framework;

import framework.calculo_ensembles_colaborativos.Evolutivo_Colaborativos;
import framework.calculo_ensembles_colaborativos.Voraz_Colaborativos;
import framework.calculo_ensembles_colaborativos.Voraz_ColaborativosLimitado;
import framework.calculo_ensembles_coordinados.Algoritmo;
import framework.calculo_ensembles_coordinados.AlgoritmoGenetico;
import framework.calculo_ensembles_coordinados.Ensemble_Coordinado;
import framework.calculo_ensembles_coordinados.Instancia;
import framework.calculo_ensembles_coordinados.Voraz;
import framework.calculo_reglas.Regla;
import framework.calculo_reglas.gp.GP_Reglas;
import framework.calculo_reglas.local_search.GRASP_reglas;
import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;
import framework.core.GestorInstancias;

@SuppressWarnings("rawtypes")
public class EvolvingRulesAndBothEnsembles extends AlgorithmAbstract {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"300", // tiempo
					"100", // intervalos
					"4", // di
					"8", // dm
					"10", // card coord
					"10", // card cola
					"1", // algo reglas
					"1", // algo coord
					"1", // algo cola
					"5", // cola X reglas
					"0", // SM
					"0", // nCrucesGP
					"0", // LSA reglas
					"0", // ratio LSA
					"0", // ME_LSA
					"0", // iteracionesMaximas 0:inf
					"0", // vecinosMaximos 0:inf
					"0" // limiteCola
			};
		}
		/*
		 * PARAMETROS
		 */
		AlgoUtil.monitoriza = true;
		// tiempo
		AlgoUtil.minutos = Integer.parseInt(args[0]);
		AlgoUtil.intervalosMedicion = Integer.parseInt(args[1]);
		// size solution
		AlgoUtil.gestorReglas.profundidadMaximaInicialReglas = Integer.parseInt(args[2]);
		AlgoUtil.gestorReglas.profundidadMaximaGlobal = Integer.parseInt(args[3]);
		AlgoUtil.gestorReglas.cardinalidadEnsemblesCoordinados = Integer.parseInt(args[4]);
		AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos = Integer.parseInt(args[5]);
		// algoritmos
		int algoritmoReglas = Integer.parseInt(args[6]);
		int algoritmoCoordinado = Integer.parseInt(args[7]);
		int algoritmoColaborativo = Integer.parseInt(args[8]);
		int iteracionesColaborativoXGP = Integer.parseInt(args[9]);
		// SM
		int SM = Integer.parseInt(args[10]);
		// conf calculo reglas
		int nCrucesGP = Integer.parseInt(args[11]);
		int LSA = Integer.parseInt(args[12]);
		double ratioLSA = Double.valueOf(1.0 * Integer.parseInt(args[13]) / 100);
		int ME_LSA = Integer.parseInt(args[14]);
		int iteracionesMaximas = Integer.parseInt(args[15]);
		int vecinosMaximo = Integer.parseInt(args[16]);
		// conf calculo cola
		int limiteCola = Integer.parseInt(args[17]);

		GestorInstancias.SM = SM;

		/*
		 * Incluirlos en el nombre
		 */
		EvolvingRulesAndBothEnsembles demo = new EvolvingRulesAndBothEnsembles("E_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
				+ AlgoUtil.gestorReglas.profundidadMaximaInicialReglas + "_"
				+ AlgoUtil.gestorReglas.profundidadMaximaGlobal + "_"
				+ AlgoUtil.gestorReglas.cardinalidadEnsemblesCoordinados + "_"
				+ AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos + "_" + algoritmoReglas + "_"
				+ algoritmoCoordinado + "_" + algoritmoColaborativo + "_" + iteracionesColaborativoXGP + "_"
				+ GestorInstancias.SM + "_" + nCrucesGP + "_" + LSA + "_" + ratioLSA + "_" + ME_LSA + "_"
				+ vecinosMaximo + "_" + iteracionesMaximas + "_" + limiteCola);

		demo.algoritmoReglas = algoritmoReglas;
		demo.algoritmoColaborativo = algoritmoColaborativo;
		demo.algoritmoCoordinado = algoritmoCoordinado;
		demo.iteracionesColaborativoXGP = iteracionesColaborativoXGP;
		demo.nCrucesGP = nCrucesGP;
		demo.LSA = LSA;
		demo.ratioLSA = ratioLSA;
		demo.ME_LSA = ME_LSA;
		demo.iteracionesMaximas = iteracionesMaximas;
		demo.vecinosMaximos = vecinosMaximo;
		demo.limiteCola = limiteCola;
		
		System.out.println("---");

		System.out.println(new Regla().generarATC(0.5).representa());
		
		demo.cabecera();
		demo.run();

		AlgoUtil.end();
	}

	// VARIABLES INTERNAS
	Ensemble_Coordinado coordinado;
	GP_Reglas evolutivo_reglas;
	Algoritmo algo_coordinado;
	Evolutivo_Colaborativos evolutivo_colaborativos;
	Voraz_Colaborativos voraz_colaborativos;

	// parametros
	int algoritmoReglas;
	int algoritmoColaborativo;
	int algoritmoCoordinado;
	int iteracionesColaborativoXGP;
	int nCrucesGP;
	int LSA;
	double ratioLSA;
	int ME_LSA;
	int iteracionesMaximas;
	int vecinosMaximos;
	int limiteCola;

	// METRICAS
	int iteracionesGP;
	int iteracionesCoord;
	int iteracionesCola;
	long inicio;
	long timeRules;
	long timeCola;
	long timeCoord;

	public EvolvingRulesAndBothEnsembles(String mensaje) {
		super(mensaje);
	}

	@Override
	public void run() {
		// reglas
		if (algoritmoReglas == 1)
			iniciaEvolutivoReglas();
		// colaborativos
		if (algoritmoColaborativo == 1)
			iniciaCola();
		// coordinados
		if (algoritmoCoordinado > 0)
			runCoordinado();
		while (!timeOut()) {
			// reglas siempre se ejecuta
			if (algoritmoReglas > 0)
				runReglas();
			// colaborativos
			if (algoritmoColaborativo > 0)
				runCola();
			// coordinados
			if (algoritmoCoordinado > 0)
				runCoordinado();
		}
	}

	void iniciaEvolutivoReglas() {
		inicio = System.currentTimeMillis();
		if (vecinosMaximos == 0)
			vecinosMaximos = Integer.MAX_VALUE;
		if (iteracionesMaximas == 0)
			iteracionesMaximas = Integer.MAX_VALUE;
		evolutivo_reglas = new GP_Reglas(nCrucesGP, LSA, ratioLSA, ME_LSA, iteracionesMaximas, vecinosMaximos);
		evolutivo_reglas.generarPoblacionInicial();
		evolutivo_reglas.getMejor();
		timeRules += (System.currentTimeMillis() - inicio);
	}

	void runReglas() {
		inicio = System.currentTimeMillis();
		iteracionesGP++;
		switch (algoritmoReglas) {
		case 1: // evolutivo
			evolutivo_reglas.run();
			break;
		case 2: // voraz
			new GRASP_reglas(LSA, ME_LSA, vecinosMaximos).run();
			break;
		default:
			break;
		}
		timeRules += (System.currentTimeMillis() - inicio);
	}

	void iniciaCola() {
		inicio = System.currentTimeMillis();
		evolutivo_colaborativos = new Evolutivo_Colaborativos();
		evolutivo_colaborativos.generarPoblacionInicial();
		evolutivo_colaborativos.getMejor();
		timeCola += (System.currentTimeMillis() - inicio);
	}

	void runCola() {
		if (iteracionesGP % iteracionesColaborativoXGP == 0) {
			iteracionesCola++;
			inicio = System.currentTimeMillis();
			switch (algoritmoColaborativo) {
			case 1: // evolutivo
				evolutivo_colaborativos.run();
				break;
			case 2: // voraz
				voraz_colaborativos = new Voraz_Colaborativos();
				voraz_colaborativos.run();
				break;
			case 3: // voraz limitado
				voraz_colaborativos = new Voraz_ColaborativosLimitado(limiteCola);
				voraz_colaborativos.run();
				break;
			default:
				break;
			}
			timeCola += (System.currentTimeMillis() - inicio);
		}
	}

	void runCoordinado() {
		iteracionesCoord++;
		inicio = System.currentTimeMillis();
		Instancia instancia = new Instancia();
		switch (algoritmoCoordinado) {
		case 1:
			algo_coordinado = new AlgoritmoGenetico(instancia, 500, 100, 0.8, 0.2, AlgoUtil.gestorReglas.cardinalidadEnsemblesCoordinados);
			break;
		case 2:
			algo_coordinado = new Voraz(instancia, AlgoUtil.gestorReglas.cardinalidadEnsemblesCoordinados);
			break;
		default:
			break;
		}
		coordinado = algo_coordinado.run();
		timeCoord += (System.currentTimeMillis() - inicio);
	}

	protected void cabecera() {
//		AlgoUtil.vln("Reglas;;;;Colaborativos;;;Coordinados;;;");
		AlgoUtil.vln("Ite.;Train;Test;Size;Time;Ite.;Train;Test;Time;Ite.;Train;Test;Time;");
	}

	@Override
	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
		String linea = iteracionesGP + ";";
		try {
			linea += AlgoUtil.evaluadorReglas.mejorGlobal.train() + ";" + AlgoUtil.evaluadorReglas.mejorGlobal.test()
					+ ";" + AlgoUtil.evaluadorReglas.mejorGlobal.getSize() + ";" + timeRules + ";";
		} catch (Exception e) {
			linea += ";" + ";" + ";" + ";";
		}
		linea += iteracionesCola + ";";
		try {
			linea += AlgoUtil.evaluadorEnsemblesColaborativos.mejorGlobal.train() + ";"
					+ AlgoUtil.evaluadorEnsemblesColaborativos.mejorGlobal.test() + ";" + timeCola + ";";
		} catch (Exception e) {
			linea += ";" + ";" + ";";
		}
		linea += iteracionesCoord + ";";
		try {
			linea += coordinado.train() + ";" + algo_coordinado.test(coordinado) + ";" + timeCoord + ";";
		} catch (Exception e) {
			linea += ";" + ";" + ";";
		}
		return linea;
	}
}
