package framework.calculo_reglas.gp;

import java.util.ArrayList;
import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.LSA_reglas;
import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;
import framework.core.Evolutivo;
import framework.core.GestorInstancias;

public class GP_Reglas extends AlgorithmAbstract<Regla> implements Evolutivo {

	private int cruceDistinto;

	public static void main(String[] args) {
		if (args.length == 0) {
			// 60 10 4 4 0 1 1 5 0 0 0
			args = new String[] { 
					"100", // tiempo
					"100", // intervalos
					"4", // di
					"8", // dm
					"2", // SM
					"50", // nCrucesGP
					"1", // LSA reglas
					"5", // ratio LSA
					"50", // ME_LSA
					"0", // iteracionesMaximas 0:inf
					"0", // vecinosMaximos 0:inf
//					"0", // cache SM
//					"1", // cruce distinto
//					"1" // lsa cualquiera
					// GP inicializacion heuristica vs ensemble
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
		// SM
		int SM = Integer.parseInt(args[4]);
		// conf calculo reglas
		int nCrucesGP = Integer.parseInt(args[5]);
		int LSA = Integer.parseInt(args[6]);
		double ratioLSA = Double.valueOf(1.0 * Integer.parseInt(args[7]) / 100);
		int ME_LSA = Integer.parseInt(args[8]);
		int iteracionesMaximas = Integer.parseInt(args[9]);
		int vecinosMaximo = Integer.parseInt(args[10]);

//		int cacheSM = Integer.parseInt(args[11]);
//		int cruceDistinto = Integer.parseInt(args[11]);
//		int lsaCualquiera = Integer.parseInt(args[13]);

		GestorInstancias.SM = SM;
//		LSA_reglas.lsaCualquiera = lsaCualquiera;

		/*
		 * Incluirlos en el nombre
		 */
		GP_Reglas demo = new GP_Reglas(
//				"GPsincompruebaevaluado_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
				"gp_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
						+ AlgoUtil.gestorReglas.profundidadMaximaInicialReglas + "_"
						+ AlgoUtil.gestorReglas.profundidadMaximaGlobal + "_" + GestorInstancias.SM + "_" + nCrucesGP
						+ "_" + LSA + "_" + ratioLSA + "_" + ME_LSA + "_" + iteracionesMaximas + "_" + vecinosMaximo
//						+ "_" + cacheSM 
//						+ "_" + cruceDistinto 
//						+ "_" + lsaCualquiera
				, nCrucesGP, LSA, ratioLSA, ME_LSA, iteracionesMaximas, vecinosMaximo);

//		AlgoUtil.evaluadorReglas.incluyeSurrogados = cacheSM;
//		demo.cruceDistinto = cruceDistinto;
		demo.cabecera();
		demo.generarPoblacionInicial();
//		demo.generarPoblacionHuristica();
		while (!AlgoUtil.timeOut())
			demo.run();
		AlgoUtil.end();
	}

	// VARIABLES INTERNAS
	public Poblacion_Reglas reglas;
	public LSA_reglas lsa;

	// PARAMETROS
	public int popSize = 200;
	public int nCruces = 1; // si es 1 entonces no es SM-GP
	public double ratioCruce = 1.0;
	public double ratioMutacion = 0.02;
	public double ratioLSA = 0.2; // si es 0 entonces no es MGP

	// METRICAS
	public int generaciones;

	public GP_Reglas() {
		if (ratioLSA > 0)
			lsa = new LSA_reglas();
	}

	public GP_Reglas(String mensaje, int nCruces, int LSA, double ratioLSA, int ME_LSA, int iteracionesMaximas,
			int vecinosMaximos) {
		super(mensaje);
		this.nCruces = nCruces;
		this.ratioLSA = ratioLSA;
		if (ratioLSA > 0)
			lsa = new LSA_reglas(LSA, ME_LSA, iteracionesMaximas, vecinosMaximos);
	}

	public GP_Reglas(int nCruces, int LSA, double ratioLSA, int ME_LSA, int iteracionesMaximas, int vecinosMaximos) {
		this.nCruces = nCruces;
		this.ratioLSA = ratioLSA;
		if (ratioLSA > 0)
			lsa = new LSA_reglas(LSA, ME_LSA, iteracionesMaximas, vecinosMaximos);
	}

	@Override
	public void run() {
		evolucionar();
		mejor = reglas.getBest();
	}

	public void generarPoblacionInicial() {
		reglas = new Poblacion_Reglas(popSize);
		reglas.generaRandom();
		mejor = reglas.getBest();
	}

	public void generarPoblacionHuristica() {
		reglas = new Poblacion_Reglas(popSize);
		reglas.cargaReglas("reglas");
		mejor = reglas.getBest();
	}
	
	@Override
	public void evolucionar() { // always crossover 100%
		reglas.shuffle();
		generaciones++;
		for (int i = 0; i < popSize && !timeOut(); i = i + 2) {
		
			Regla padre1 = reglas.get(i);
			Regla padre2 = reglas.get(i + 1);
			if (Math.random() <= ratioCruce) {

				Regla hijo1;
				if (nCruces > 1)
					hijo1 = cruceFiltro(padre1, padre2);
				else
					hijo1 = cruzar(padre1, padre2);

				if (Math.random() <= ratioMutacion) // mutacion
					mutar(hijo1);

				if (Math.random() <= ratioLSA) // LSA
					hijo1 = lsa.run(hijo1);

				// Segundo hijo
				Regla hijo2;
				if (nCruces > 1)
					hijo2 = cruceFiltro(padre2, padre1);
				else
					hijo2 = cruzar(padre2, padre1);

				if (Math.random() <= ratioMutacion) // mutacion
					mutar(hijo2);

				if (Math.random() <= ratioLSA) // LSA
					hijo2 = lsa.run(hijo2);

				// torneo padres e hijos, pasa siempre un hijo
				Poblacion_Reglas tournament = new Poblacion_Reglas(3);
				tournament.add(padre1);
				tournament.add(padre2);

				// hijo1 mejor hijo2
				if (AlgoUtil.evaluadorReglas.mejoraReal(hijo1, hijo2)) {
					reglas.set(i, hijo1); // pasa hijo1
					tournament.add(hijo2); // y el mejor entre hijo2,
					reglas.set(i + 1, tournament.getBest());
				}
				// hijo2 mejor hijo1
				else {
					reglas.set(i, hijo2); // pasa hijo2
					tournament.add(hijo1); // y el mejor entre hijo1, padre1 y padre2
					reglas.set(i + 1, tournament.getBest());
				}
				
			} else {
				Regla hijo1 = padre1.getClon(), hijo2 = padre2.getClon();
				Regla mutacion1 = null, mutacion2 = null;
				// mutacion
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutar(mutacion1);
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutar(mutacion2);
				}
				
				if (Math.random() <= ratioLSA) // LSA
					hijo1 = lsa.run(hijo1);
				
				if (Math.random() <= ratioLSA) // LSA
					hijo2 = lsa.run(hijo2);

				if (mutacion1 == null && mutacion2 == null) {
					reglas.set(i, hijo1);
					reglas.set(i + 1, hijo2);
				} else {
					Poblacion_Reglas torneo;
					if (mutacion1 != null && mutacion2 == null) {
						torneo = new Poblacion_Reglas(3);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion1);
					} else if (mutacion1 == null && mutacion2 != null) {
						torneo = new Poblacion_Reglas(3);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion2);
					} else {
						torneo = new Poblacion_Reglas(4);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion1);
						torneo.add(mutacion2);
					}
					Regla first = torneo.getBest();
					reglas.set(i, first);
					torneo.remove(first);
					Regla second = torneo.getBest();
					reglas.set(i + 1, second);
				}
			}
			
		}
	}
	
//	@Override
//	public void evolucionar() {
//
//		reglas.shuffle();
//		generaciones++;
//		for (int i = 0; i < popSize && !timeOut(); i = i + 2) {
//
//			// selección por pares
//			Regla padre1 = reglas.get(i);
//			Regla padre2 = reglas.get(i + 1);
//
//			if (Math.random() <= ratioCruce) { // cruce: siempre 1.0
//
//			// Primer hijo
//			Regla hijo1;
//			if (nCruces > 1)
//				hijo1 = cruceFiltro(padre1, padre2);
//			else
//				hijo1 = cruzar(padre1, padre2);
//
//			if (Math.random() <= ratioMutacion) // mutacion
//				mutar(hijo1);
//
//			if (Math.random() <= ratioLSA) // LSA
//				hijo1 = lsa.run(hijo1);
//
//			// Segundo hijo
//			Regla hijo2;
//			if (nCruces > 1)
//				hijo2 = cruceFiltro(padre2, padre1);
//			else
//				hijo2 = cruzar(padre2, padre1);
//
//			if (Math.random() <= ratioMutacion) // mutacion
//				mutar(hijo2);
//
//			if (Math.random() <= ratioLSA) // LSA
//				hijo2 = lsa.run(hijo2);
//
//			// torneo padres e hijos, pasa siempre un hijo
//			Poblacion_Reglas tournament = new Poblacion_Reglas(3);
//			tournament.add(padre1);
//			tournament.add(padre2);
//
//			// hijo1 mejor hijo2
//			if (AlgoUtil.evaluadorReglas.mejoraReal(hijo1, hijo2)) {
//				reglas.set(i, hijo1); // pasa hijo1
//				tournament.add(hijo2); // y el mejor entre hijo2,
//				reglas.set(i + 1, tournament.getBest());
//			}
//			// hijo2 mejor hijo1
//			else {
//				reglas.set(i, hijo2); // pasa hijo2
//				tournament.add(hijo1); // y el mejor entre hijo1, padre1 y padre2
//				reglas.set(i + 1, tournament.getBest());
//			}
//
//			// SI SE QUIERE MUCHA PRESIÓN en algun momento tendremos que reconsiderar el
//			// esquema evolutivo
//			// torneo padres e hijos, pasan los dos mejores
////				PoblacionReglas tournament = new PoblacionReglas(4);
////				tournament.guarda(0, padre1);
////				tournament.guarda(1, padre2);
////				tournament.guarda(2, hijo1);
////				tournament.guarda(3, hijo2);
////				tournament.ordenaPoblacionEntrenamiento();
////				newPopulation.guarda(i, tournament.getCromosoma(0));
////				newPopulation.guarda(i+1, tournament.getCromosoma(1));	
////			} else { // sin implementar si cruce no es 100%
////			}
//		}
//		mejor = reglas.getBest();
//	}

	Regla cruceFiltro(Regla padre1, Regla padre2) {
		List<Regla> cruces = new ArrayList<Regla>();
		// se cruzan nCruces
		for (int i = 0; i < nCruces; i++)
			cruces.add(cruzar(padre1, padre2));
		// se selecciona el mejor
		Regla mejor = cruces.get(0);
//		if (cruceDistinto == 1) {
		for (Regla regla : cruces)
			if (AlgoUtil.evaluadorReglas.mejoraSurrogada(regla, mejor)
//						&& !regla.representa().equals(padre1.representa())
//						&& !regla.representa().equals(padre2.representa())
					&& AlgoUtil.evaluadorReglas.getPartialEvaluate(regla).filter() != AlgoUtil.evaluadorReglas.getPartialEvaluate(padre1).filter()
					&& AlgoUtil.evaluadorReglas.getPartialEvaluate(regla).filter() != AlgoUtil.evaluadorReglas.getPartialEvaluate(padre2).filter())
				mejor = regla;
//		} else // si es 0
//			for (Regla regla : cruces)
//				if (AlgoUtil.evaluadorReglas.mejoraSurrogada(regla, mejor))
//					mejor = regla;
		return mejor;
	}

	protected Regla cruzar(Regla padre1, Regla padre2) {
		int i, j, k = 0, n = 1000;
//		do {
			i = padre1.posAzar();
			j = padre2.posAzarMismaDim(padre1, i);
//			j = padre2.posAzar(); // esto hace que las reglas puedan no ser dimensionalmente correctas
			k++;
//		} while (j == -1 && k < n); // se hace solo un intento y ya al principio 
		if (j == -1) {
//			System.out.println("entro aqui????");
			return new Regla().random();
//			return padre1;
		}
		Regla hijo1 = new Regla(i, j, padre1, padre2);
		Regla hijo2 = new Regla(j, i, padre2, padre1);

		if (!hijo1.esValido())
			hijo1 = null;

		if (!hijo2.esValido())
			hijo2 = null;

//		// comprobamos el tamaño, si tamañoMaximo<2^profundidadMaxima -1, para compararse con el SSHE
//		if (hijo1 != null && hijo1.getSize() > Main.tamañoMaximoReglas)
//			hijo1 = null;
//		if (hijo2 != null && hijo2.getSize() > Main.tamañoMaximoReglas)
//			hijo2 = null;

		if (hijo1 == null && hijo2 == null) {
			return new Regla().random();
//			return padre1;
		} else if (hijo1 != null && hijo2 == null)
			return hijo1;
		else if (hijo1 == null && hijo2 != null) {
			return hijo2;
		} else {
//			if (cruceDistinto==0)
				return hijo1;
//			else {
//				if (AlgoUtil.evaluadorReglas.mejoraReal(hijo1, hijo2))  // con las instancias sobrecapacitadas no hay apenas diferencia, aqui si
//					return hijo1;
//				else // hijo2 mejor hijo1
//					return hijo2;
//			}
		}
	}

	protected Regla mutar(Regla cromosoma) {
		Regla mutacion = new Regla().random();
		int i, j, k = 0, n = 1000;
//		do {
			i = cromosoma.posAzar();
			j = mutacion.posAzarMismaDim(cromosoma, i);
			k++;
//		} while (j != -1 && k < n);
		if (j == -1) {
			return mutacion;
		}
		Regla hijo = new Regla(i, j, cromosoma, mutacion);
		if (hijo.esValido())
			return hijo;
		else {
			return mutacion;
		}
//		if (hijo.getSize() <= Main.tamañoMaximoReglas) // para compararse con el SSHE...
//			if (hijo.esValido())
//				return hijo;
//			else
//				return mutacion;
////				return cromosoma;
//		else
//			return mutacion;
////			return cromosoma;
	}

	protected void cabecera() {
		AlgoUtil.vln("Train;Test;Size;Depth;Generaciones;Distintos;Regla;Monticulo;");
	}

	@Override
	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
		String linea = "";
		try {
			linea += AlgoUtil.evaluadorReglas.mejorGlobal.train() + ";" + AlgoUtil.evaluadorReglas.mejorGlobal.test()
					+ ";" + AlgoUtil.evaluadorReglas.mejorGlobal.getSize() + ";"
					+ AlgoUtil.evaluadorReglas.mejorGlobal.getProfundidad() + ";" + generaciones + ";"
					+ AlgoUtil.evaluadorReglas.evaluatedFull.size() + ";"
					+ AlgoUtil.evaluadorReglas.mejorGlobal.representa() + ";"
					+ AlgoUtil.evaluadorReglas.mejorGlobal.representaMonticulo() + ";";
		} catch (Exception e) {
			linea += ";" + ";" + ";" + ";" + generaciones + ";" + AlgoUtil.evaluadorReglas.evaluatedFull.size() + ";" + ";" + ";";
		}
		return linea;
	}

}