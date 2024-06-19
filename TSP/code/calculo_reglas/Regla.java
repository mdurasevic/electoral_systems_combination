package calculo_reglas;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ThreadLocalRandom;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.operadores.NodoDivision;
import calculo_reglas.representacion.nodos.operadores.NodoExponencial;
import calculo_reglas.representacion.nodos.operadores.NodoLoge;
import calculo_reglas.representacion.nodos.operadores.NodoMax;
import calculo_reglas.representacion.nodos.operadores.NodoMax0;
import calculo_reglas.representacion.nodos.operadores.NodoMin;
import calculo_reglas.representacion.nodos.operadores.NodoMin0;
import calculo_reglas.representacion.nodos.operadores.NodoMultiplicacion;
import calculo_reglas.representacion.nodos.operadores.NodoNegacion;
import calculo_reglas.representacion.nodos.operadores.NodoPotencia;
import calculo_reglas.representacion.nodos.operadores.NodoRaiz;
import calculo_reglas.representacion.nodos.operadores.NodoResta;
import calculo_reglas.representacion.nodos.operadores.NodoSuma;
import calculo_reglas.representacion.nodos.terminales.NodoT1Nn;
import calculo_reglas.representacion.nodos.terminales.NodoT2Nrn;
import calculo_reglas.representacion.nodos.terminales.NodoT3Dcn;
import calculo_reglas.representacion.nodos.terminales.NodoT4Din;
import calculo_reglas.representacion.nodos.terminales.NodoT5Dc;
import calculo_reglas.representacion.nodos.terminales.NodoT6Pd;
import calculo_reglas.representacion.nodos.terminales.NodoT7Dle;
import domain.Graph;
import domain.Individual;
import domain.Point;
import domain.TSPFunction;

public class Regla extends TSPFunction implements Comparable<Regla>{
	/*
	 * VARIABLES
	 */
	public Nodo[] nodos;
	public int size;
	public String regla;
	public boolean valido = false;

	/*
	 * CONSTRUCTORES
	 */
	public Regla() {
		this.nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.profundidadMaximaGlobal) - 1)];
		size = 0;
	}

	public Regla(int length) {
		this.nodos = new Nodo[length];
		size = 0;
	}

	/**
	 * Crea un hijo
	 * 
	 * @param posicionPadre1
	 * @param posicionPadre2
	 * @param padre1
	 * @param padre2
	 */
	public Regla(int posicionPadre1, int posicionPadre2, Regla padre1, Regla padre2) {
		this(padre1);
		borrar(posicionPadre1);
		insertar(posicionPadre1, posicionPadre2, padre2);
	}

	public Regla generarNN() {
		nodos[0] = new NodoDivision();
		nodos[1] = AlgoUtil.getConstante(1.0);
		nodos[2] = NodoT3Dcn.getInstance();
		size = 3;
		asignaDimension();
		return this;
	}

	public Regla generarNN2() {
		nodos[0] = NodoT3Dcn.getInstance();
		size = 1;
		asignaDimension();
		return this;
	}

	public void asignaDimension() {
		for (int i = nodos.length - 1; i >= 0; i--) {
			if (nodos[i] != null)
				dimensionSimboloEn(i);
		}
	}

	public void dimensionSimboloEn(int i) {
		try {
			if (nodos[i].esOperacion()) {
				switch (nodos[i].simbolo()) {
				case "/":
					nodos[i].setDimension(nodos[2 * i + 1].getDimension() - nodos[2 * i + 2].getDimension());
					break;
				case "*":
					nodos[i].setDimension(nodos[2 * i + 1].getDimension() + nodos[2 * i + 2].getDimension());
					break;
				case "Math.pow":
					nodos[i].setDimension(nodos[2 * i + 1].getDimension() * 2.0);
					break;
				case "Math.sqrt":
					nodos[i].setDimension(nodos[2 * i + 1].getDimension() / 2.0);
					break;
				case "Math.exp":
					nodos[i].setDimension(0);
					break;
				case "Math.log":
					nodos[i].setDimension(0);
					break;
				default:
					nodos[i].setDimension(nodos[2 * i + 1].getDimension());
					break;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("FALLO AL ASIGNAR DIMENSION");
		}
	}

	/**
	 * Clona el monticulo recibido
	 * 
	 * @param monticulo
	 */
	public Regla getClon() {
		return new Regla(this);
	}

	public Regla(Regla monticulo) {
		this.nodos = new Nodo[monticulo.nodos.length];
		this.size = monticulo.size;
		for (int i = 0; i < nodos.length; i++)
			if (monticulo.nodos[i] != null) {
				nodos[i] = monticulo.nodos[i].clona();
				nodos[i].setDimension(monticulo.nodos[i].getDimension());
			}
	}

	public Regla random() {
		if (AlgoUtil.metodoKoza)
			return metodoKoza();
		else
			return random_private();
	}

	private Regla random_private() {
		do {
			nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.profundidadMaximaGlobal) - 1)];
			size = 0;
			generarDegenerado(nodos.length);
			// se comprueba que la regla es vÃ¡lida
			if (esValido())
				valido = true;
		} while (!valido);
		return this;
	}

	public Regla metodoKoza() {
		// completo o degenerado aleatoriamente
		boolean completo = java.util.concurrent.ThreadLocalRandom.current().nextBoolean();

		// profundidad mÃ¡xima ente 1 y n
		int profundidadMaxima = (ThreadLocalRandom.current().nextInt(1, AlgoUtil.profundidadMaximaInicialReglas) + 1);

		// si nos pasamos de un limite
		if (completo && profundidadMaxima > AlgoUtil.profundidadMaximaCompletasReglas)
			profundidadMaxima = AlgoUtil.profundidadMaximaCompletasReglas;

		int sizeMax = (int) (Math.pow(2, profundidadMaxima) - 1);

		// generamos la regla
		do {
			nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.profundidadMaximaGlobal) - 1)];
			size = 0;
			if (completo)
				generarCompleto(sizeMax);
			else
				generarDegenerado(sizeMax);

			// se comprueba que la regla es vÃ¡lida
			if (esValido())
				valido = true;

		} while (!valido);
		return this;
	}

	private void generarCompleto(int sizeMax) {
		for (int i = sizeMax - 1; i >= 0; i--) {
			if (i >= (int) sizeMax / 2) {
				generaTerminalConstante(i);
				size++;
			} else {
				if (nodos[2 * i + 1] != null && nodos[2 * i + 2] != null)
					generaBinaria(i);
				size++;
//				dimensionSimboloEn(i);
			}
		}
	}

	private void generarDegenerado(int sizeMax) {
		for (int i = sizeMax - 1; i >= 0; i--) {
			if (i >= (int) sizeMax / 2 || (nodos[2 * i + 1] == null)) { // terminal
				if (!hijoDerecho(i) && nodos[i + 1] != null) {
					if (!nodos[i + 1].esConstante())
						generaTerminalConstante(i);
					else
						generaTerminal(i);
				} else {
					generaHoja(i);
				}
			} else { // funcion
				if (nodos[2 * i + 2] != null) // binaria
					generaBinaria(i);
				else // unaria
					generaUnaria(i);
			}
			if (nodos[i] != null)
				size++;
		}
		if (nodos[0] == null)
			random_private();
	}

	public int iguales(int i, int j) {
		if (j >= nodos.length) {
			return 0;
		} else if (nodos[i] == null && nodos[j] == null) { // der == izq
			return 0;
		} else { // (nodos[i] != null && nodos[j] != null)
			if (nodos[i].orden() > nodos[j].orden()) // izq > der
				return 1;
			else if (nodos[i].orden() < nodos[j].orden()) // izq < der
				return -1;
			else { // izq == der
				if (nodos[i].esTerminal()) // esTerminal -> T o C.
					return 0;
				else {
					int value = iguales(2 * i + 1, 2 * j + 1);
					if (value == 0)
						return iguales(2 * i + 2, 2 * j + 2);
					return value;
				}
			}
		}
	}

	private void generaHoja(int i) {
		int random = (int) (Math.random() * (1 + AlgoUtil.getNterminales() + AlgoUtil.indiceNullHoja));
		if (random == 0)
			nodos[i] = AlgoUtil.getConstanteRandom();
		else if (random >= 1 && random <= AlgoUtil.getTerminales().size())
			nodos[i] = AlgoUtil.getTerminalRandom(random - 1);
		else
			nodos[i] = null;
	}

	private void generaTerminalConstante(int i) {
		int random = (int) (Math.random() * (1 + AlgoUtil.getNterminales()));
		if (random == 0)
			nodos[i] = AlgoUtil.getConstanteRandom();
		else
			nodos[i] = AlgoUtil.getTerminalRandom(random - 1);

	}

	private void generaTerminal(int i) {
		int random = (int) (Math.random() * (AlgoUtil.getNterminales()));
		nodos[i] = AlgoUtil.getTerminalRandom(random);
	}

	private void generaBinaria(int i) { // todas las binarias
		switch ((int) (Math.random() * 6)) {
		case 0:
			nodos[i] = new NodoSuma();
			break;
		case 1:
			nodos[i] = new NodoResta();
			break;
		case 2:
			nodos[i] = new NodoMax();
			break;
		case 3:
			nodos[i] = new NodoMin();
			break;
		case 4:
			nodos[i] = new NodoDivision();
			break;
		default:
			nodos[i] = new NodoMultiplicacion();
			break;
		}
	}

	private void generaUnaria(int i) {
		switch ((int) (Math.random() * 7)) {
		case 0:
			nodos[i] = new NodoRaiz();
			break;
		case 1:
			nodos[i] = new NodoPotencia();
			break;
		case 2:
			nodos[i] = new NodoMax0();
			break;
		case 3:
			nodos[i] = new NodoMin0();
			break;
		case 4:
			nodos[i] = new NodoNegacion();
			break;
		case 5:
			nodos[i] = new NodoLoge();
			break;
		default:
			nodos[i] = new NodoExponencial();
			break;
		}
	}

	/*
	 * CARGAR REGLAS YA DEFINIDAS
	 */
	public Regla generarRegla(String regla) {
		int sizeMax = (int) (Math.pow(2, AlgoUtil.profundidadMaximaGlobal) - 1);
		nodos = new Nodo[sizeMax];
		String[] valores = regla.split(" ");
		for (int i = valores.length - 1; i >= 0; i--)
			if (!valores[i].equals("null") && !valores[i].isEmpty()) {
				size++;
				switch (valores[i]) {
				case "/":
					nodos[i] = new NodoDivision();
					break;
				case "Math.exp":
					nodos[i] = new NodoExponencial();
					break;
				case "Math.log":
					nodos[i] = new NodoLoge();
					break;
				case "Math.max":
					if (nodos[2 * i + 2] != null)
						nodos[i] = new NodoMax();
					else
						nodos[i] = new NodoMax0();
					break;
				case "Math.min":
					if (nodos[2 * i + 2] != null)
						nodos[i] = new NodoMin();
					else
						nodos[i] = new NodoMin0();
					break;
				case "*":
					nodos[i] = new NodoMultiplicacion();
					break;
				case "-":
					if (nodos[2 * i + 2] != null)
						nodos[i] = new NodoResta();
					else
						nodos[i] = new NodoNegacion();
					break;
				case "Math.pow":
					nodos[i] = new NodoPotencia();
					break;
				case "Math.sqrt":
					nodos[i] = new NodoRaiz();
					break;
				case "+":
					nodos[i] = new NodoSuma();
					break;
				// Terminales
				case "Nn":
					nodos[i] = NodoT1Nn.getInstance();
					break;
				case "Nrn":
					nodos[i] = NodoT2Nrn.getInstance();
					break;
				case "Dcn":
					nodos[i] = NodoT3Dcn.getInstance();
					break;
				case "Din":
					nodos[i] = NodoT4Din.getInstance();
					break;
				case "Dc":
					nodos[i] = NodoT5Dc.getInstance();
					break;
				case "Pd":
					nodos[i] = NodoT6Pd.getInstance();
					break;
				case "Dle":
					nodos[i] = NodoT7Dle.getInstance();
					break;
				default:
					// SI parsea mal las reglas mirar aqui
					/*
					 * if (valores[i].equals("0") || valores[i].equals("0.0")) { if ((valores[(i -
					 * 1) / 2].equals("Math.min") || valores[(i - 1) / 2].equals("Math.max") && i %
					 * 2 != 0 && i != 0)) { tamaÃ±o--; // restamos el tamaÃ±o y no hacemos nada
					 * } else { nodos[i] = SetConstantes.get(Double.parseDouble(valores[i])); } }
					 * else { nodos[i] = SetConstantes.get(Double.parseDouble(valores[i])); }
					 */
					nodos[i] = AlgoUtil.getConstante(Double.parseDouble(valores[i]));
					break;
				}
//				if (nodos[i] != null)
//					if (nodos[i].esOperacion())
//						dimensionSimboloEn(i);
			}
		return this;
	}

//	public void asignaDimension() {
//		for (int i = nodos.length - 1; i >= 0; i--) {
//			if (nodos[i] != null)
//				dimensionSimboloEn(i);
//		}
//	}

	/*
	 * RECORRIDOS DEL Ã�RBOL
	 */
	public String representa() {
		List<String> elementos = new ArrayList<String>();
		representa(0, elementos);
		return generaRegla(elementos);
	}

	String monticulo;

	public String representaMonticulo() {
		if (monticulo == null) {
			monticulo = "";
			for (int i = 0; i < nodos.length; i++)
				if (nodos[i] != null)
					monticulo = monticulo + nodos[i].simbolo().toString() + " ";
				else
					monticulo = monticulo + "null ";
		}
		return monticulo;
	}

	public String representaMonticuloPorNivel() {
		String text = "";
		int nivel = 0;
		for (int i = 0; i < nodos.length; i++) {
			if ((((int) (Math.log(i + 1) / Math.log(2))) + 1) > nivel) {
				nivel++;
				text += "\n";
			}
			if (nodos[i] != null)
				text += "{" + nodos[i].simbolo() + " " + nodos[i].getDimension() + "}";
			else
				text += "{<NULL>}";
		}
		return text;
	}

	protected String generaRegla(List<String> elementos) {
		regla = "";
		for (String s : elementos)
			regla = regla + s;
		return regla;
	}

	protected void representa(int i, List<String> formula) {
		if (nodos[i] != null) {
			if (!nodos[i].esTerminal())
				formula.add("(");
			else
				formula.add(nodos[i].simbolo());

			if (nodos[i].simbolo().equals("Math.max") || nodos[i].simbolo().equals("Math.min") || nodos[i].esUnaria())
				formula.add(nodos[i].simbolo() + "(");

			if ((2 * i + 1) < nodos.length)
				representa(2 * i + 1, formula);

			if (nodos[i].simbolo().equals("Math.pow") || nodos[i].simbolo().equals("Math.max")
					|| nodos[i].simbolo().equals("Math.min"))
				formula.add(",");

			if (nodos[i].simbolo().equals("+") || (nodos[i].simbolo().equals("-") && !nodos[i].esUnaria())
					|| nodos[i].simbolo().equals("/") || nodos[i].simbolo().equals("*"))
				formula.add(nodos[i].simbolo());

			if (nodos[i].simbolo().equals("Math.pow"))
				formula.add("2");

			if ((nodos[i].simbolo().equals("Math.max") && nodos[i].esUnaria())
					|| (nodos[i].simbolo().equals("Math.min") && nodos[i].esUnaria()))
				formula.add("0");

			if ((2 * i + 2) < nodos.length)
				representa(2 * i + 2, formula);

			if (nodos[i].simbolo().equals("Math.max") || nodos[i].simbolo().equals("Math.min") || nodos[i].esUnaria())
				formula.add(")");

			if (!nodos[i].esTerminal())
				formula.add(")");
		}
	}

	public int posAzar() {
		List<Integer> pos = new ArrayList<Integer>();
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				pos.add(i);
		if (!pos.isEmpty())
			return pos.get((int) (Math.random() * pos.size()));
		return -1;
	}

	// FITNESS

	public Point computePrioritiesAndChooseNextCity() {
		if (AlgoUtil.nSM > 0) // nuevo version surrogada de la regla
			return computePrioritiesAndChooseNextCitySurrogate();
		return computePriorityAndChooseNextCityNOsurrogate();
	}

	private Point computePriorityAndChooseNextCityNOsurrogate() {
		double mostPriorityCity = -Double.MAX_VALUE;
		double priority;
		Point cityChoosed = null;
		for (Point city : noVisited) {
			setCandidateCity(city);
			priority = calculaPrioridadDe(this);
			if (!Double.isFinite(priority))
				priority = 0;
			if (priority > mostPriorityCity) {
				mostPriorityCity = priority;
				cityChoosed = city;
			}
		}
		// never should happen
		if (cityChoosed == null) { // the rule doesnt decide because priorities calculated
			// do something
			System.out.println("Rule doesnt decide " + representa());
		}
		return cityChoosed;
	}

	private Point computePrioritiesAndChooseNextCitySurrogate() {
		if (AlgoUtil.nSM > 1)
			AlgoUtil.fullSM = noVisited.size() / AlgoUtil.nSM;

		if (AlgoUtil.fullSM == 0 || AlgoUtil.fullSM >= noVisited.size())
			return computePriorityAndChooseNextCityNOsurrogate();

		List<FilteredPoint> points = new ArrayList<FilteredPoint>();
		// surrogate
		for (Point p : noVisited) {
			points.add(new FilteredPoint(calculaPrioridad(p), p));
		}

		Collections.sort(points); // ordena de mayor a menor

		double priority, mostPriorityCity = -Double.MAX_VALUE;
		Point city, cityChoosed = null;
		for (int i = 0; i < AlgoUtil.fullSM; i++) {
			city = points.get(i).point;
			setCandidateCity(city);
			priority = calculaPrioridadDe(this);
			if (!Double.isFinite(priority))
				priority = 0;
			if (priority > mostPriorityCity) {
				mostPriorityCity = priority;
				cityChoosed = city;
			}
		}
		return cityChoosed;
	}

	private double calculaPrioridad(Point p) {
		setCandidateCity(p);
		return 1.0 / getDistanceFromCurrent(); // como NN
	}

	public double calculaPrioridadDe(TSPFunction function) {
		return calculaPrioridadDe(function, 0);
	}

	protected double calculaPrioridadDe(TSPFunction function, int i) {
		if (nodos[i] != null) { // ojo esto es nuevo!
			if (nodos[i].esTerminal())
				return nodos[i].value(function);
			else {
				double izq = 0, der = 0;
				if ((2 * i + 1) < nodos.length) {
					izq = calculaPrioridadDe(function, (2 * i + 1));
					if ((2 * i + 2) < nodos.length && !nodos[i].esUnaria())
						der = calculaPrioridadDe(function, (2 * i + 2));
				}
				return nodos[i].value(izq, der);
			}
		}
		return 0;
	}
	// fin calculo coste conjuntos

	public double getCoste(Graph instance) {
//		MapaSolucionesRegla soluciones = AlgoUtil.soluciones.get(this);
		double value;
//		if (soluciones == null) {
//			soluciones = new MapaSolucionesRegla();
//			AlgoUtil.soluciones.put(this, soluciones);
//		}
//		if (soluciones.mapa.containsKey(instance)) {
//			value = soluciones.getSolucion(instance);
//		} else {
		Individual solucion = generateIndividualAccordingADeterministicMethod(instance);
		value = solucion.getCoste();
//			soluciones.setSolucion(instance, value);
//		}
		return value;
	}

	/*
	 * RESOLVER CONJUNTO DE ENTRENAMIENTO
	 */
	public boolean evalTrain = false;
	public double train;
//	protected Map<String, Double> solutions_train;

	public double train() {
		if (!evalTrain) {
			instancesTraining = "";
			train = 0;
			Individual solucion = null;
			double value;
			// MapaSolucionesRegla soluciones = AlgoUtil.soluciones.get(this);
			// if (soluciones == null) {
			// soluciones = new MapaSolucionesRegla();
			// AlgoUtil.soluciones.put(this, soluciones);
			// AlgoUtil.addRule(this);
			// }
//			System.out.println(AlgoUtil.set_train.size() + " " + AlgoUtil.fitness);
			for (Graph instance : AlgoUtil.set_train) {
				// if (soluciones.mapa.containsKey(instance)) {
				// value = soluciones.getSolucion(instance);
				// } else {
				solucion = generateIndividualAccordingADeterministicMethod(instance);
				value = solucion.getCoste();
				// soluciones.setSolucion(instance, value);
				// if (!solucion.valida()) System.out.println("solucion no valida");
				// }
				if (AlgoUtil.fitness == 1)
					value = (value / instance.getTotalCost() * Math.pow(instance.getSize(), 2)); // normalize?
				instancesTraining += instance.getName() + " " + value + ",";
				train += value;

			}
			train /= AlgoUtil.set_train.size();
			evalTrain = true;
		}
		return train;
	}

	public double coste(Graph instance) {
		return generateIndividualAccordingADeterministicMethod(instance).getCoste();
	}

//
//	public double train(String instancia) {
//		train();
//		return solutions_train.get(instancia);
//	}

	/*
	 * RESOLVER CONJUNTO DE TEST
	 */
//	public boolean evalTest;
	protected double test;
//	protected Map<String, Double> solutions_test;
	public String instancesTest;
	public int dominanciaTest;

	public double test() {
//		if (!evalTest) {
		instancesTest = "";
		dominanciaTest = 0;
//			solutions_test = new HashMap<String, Double>();
		test = 0;
		Individual solucion;
		double value;
		boolean mejora;
//		MapaSolucionesRegla soluciones = AlgoUtil.soluciones.get(this);
//		if (soluciones == null) {
//			soluciones = new MapaSolucionesRegla();
//			AlgoUtil.soluciones.put(this, soluciones);
//		}
		for (Graph instance : AlgoUtil.set_test) {
			mejora = false;
//			if (soluciones.mapa.containsKey(instance)) {
//				value = soluciones.getSolucion(instance);
//			} else {
			solucion = generateIndividualAccordingADeterministicMethod(instance);
			value = solucion.getCoste();
//				soluciones.setSolucion(instance, value);
//			}
//				solutions_test.put(instance.getName(), value);
			test += value;
			if (AlgoUtil.getPromedioPaperReferencia(instance) > value) {
				dominanciaTest++;
				mejora = true;
			}
			instancesTest += instance.getName() + " " + value + " " + mejora + ",";
		}
		test /= AlgoUtil.set_test.size();
//			evalTest = true;
//		}
		return test;
	}

	public double test2() {
		test = 0;
		instancesTest = "";
		Individual solucion;
		double value;
		boolean mejora;
//		MapaSolucionesRegla soluciones = AlgoUtil.soluciones.get(this);
//		if (soluciones == null) {
//			soluciones = new MapaSolucionesRegla();
//			AlgoUtil.soluciones.put(this, soluciones);
//		}
		for (Graph instance : AlgoUtil.set_test) {
			mejora = false;
//			if (soluciones.mapa.containsKey(instance)) {
//				value = soluciones.getSolucion(instance);
//			} else {
			solucion = generateIndividualAccordingADeterministicMethod(instance);
			value = solucion.getCoste();
//				soluciones.setSolucion(instance, value);
//			}
			test += value;
			instancesTest += instance.getName() + " " + value + " " + mejora + ",";
		}
		test /= AlgoUtil.set_test.size();
		return test;
	}

//	public double test(String instancia) {
//		test();
//		return solutions_test.get(instancia);
//	}

	/*
	 * RESOLVER CONJUNTO DE FILTRO
	 */
	public boolean evalFilter;
	protected double filter;
	public String instancesTraining;

	public double filter() {
		if (!evalFilter) {
			filter = 0;
			Individual solucion;
//		MapaSolucionesRegla soluciones = AlgoUtil.soluciones.get(this);
//		if (soluciones == null) {
//			soluciones = new MapaSolucionesRegla();
//			AlgoUtil.soluciones.put(this, soluciones);
//		}
			double value;
			for (Graph instance : AlgoUtil.set_filtro) {
//			if (soluciones.mapa.containsKey(instance)) {
//				value = soluciones.getSolucion(instance);
//			} else {
				solucion = generateIndividualAccordingADeterministicMethod(instance);
				value = solucion.getCoste();
//				soluciones.setSolucion(instance, value);
//			}
//			value = value / instance.getTotalCost() * Math.pow(instance.getSize(), 2);  // normalize? 
				if (AlgoUtil.fitness == 1)
					value = (value / instance.getTotalCost() * Math.pow(instance.getSize(), 2)); // normalize?
				filter += value;
			}
			filter /= AlgoUtil.set_filtro.size();
			evalFilter = true;
		}
		return filter;
	}

	// para el CSP
	public double filter(double cota) {
		filter = 0;
		Individual solucion;
		double value;
		for (Graph instance : AlgoUtil.set_filtro) {
			solucion = generateIndividualAccordingADeterministicMethod(instance, cota);
			value = solucion.getCoste();
			filter += value;
		}
		filter /= AlgoUtil.set_filtro.size();
		return filter;
	}

	/*
	 * OPERACIONES Y COMPROBACIONES DEBIDAS A LA IMPLEMENTACIÃ“N CON MONTICULOS
	 * BINARIOS
	 * 
	 * Inserta el subarbol j del monticulo en la posicion i de este monticulo
	 * 
	 */
	public void insertar(int i, int j, Regla monticulo) {
		if (monticulo.nodos[j] != null) {
			nodos[i] = monticulo.nodos[j].clona();
			size++;
			if ((2 * i + 1) < nodos.length && (2 * j + 1) < nodos.length)
				insertar(2 * i + 1, 2 * j + 1, monticulo);
			if ((2 * i + 2) < nodos.length && (2 * j + 2) < nodos.length)
				insertar(2 * i + 2, 2 * j + 2, monticulo);
		}
	}

	public void borrar(int i) {
		if (nodos[i] != null) {
			size--;
			nodos[i] = null;
			if ((2 * i + 1) < nodos.length)
				borrar(2 * i + 1);
			if ((2 * i + 2) < nodos.length)
				borrar(2 * i + 2);
		}
	}

	protected List<String> subarbol(int index) {
		List<String> subarbol = new ArrayList<String>();
		for (int i = index; i < nodos.length; i++)
			if (nodos[i] != null)
				subarbol.add(nodos[i].simbolo());
		return subarbol;
	}

	public boolean hijoDerecho(int i) {
		return i % 2 == 0 && i != 0;
	}

	protected boolean padreNull(int i) {
		return nodos[(i - 1) / 2] == null;
	}

	protected boolean padreFuncionBinaria(int i) {
		if (!padreNull(i))
			return nodos[(i - 1) / 2].esOperacion() && !nodos[(i - 1) / 2].esUnaria();
		return false;
	}

	protected boolean padreFuncionUnaria(int i) {
		if (!padreNull(i))
			return nodos[(i - 1) / 2].esOperacion() && nodos[(i - 1) / 2].esUnaria();
		return false;
	}

	protected boolean padreTerminal(int i) {
		if (!padreNull(i))
			return !nodos[(i - 1) / 2].esOperacion();
		return false;
	}

	protected boolean hoja(int i) {
		return i >= (int) nodos.length / 2;
	}

//	public void dimensionSimboloEn(int i) {
//		try {
//			if (nodos[i].esOperacion()) {
//				switch (nodos[i].simbolo()) {
//				case "/":
//					nodos[i].setDimension(nodos[2 * i + 1].getDimension() - nodos[2 * i + 2].getDimension());
//					break;
//				case "*":
//					nodos[i].setDimension(nodos[2 * i + 1].getDimension() + nodos[2 * i + 2].getDimension());
//					break;
//				case "Math.pow":
//					nodos[i].setDimension(nodos[2 * i + 1].getDimension() * 2.0);
//					break;
//				case "Math.sqrt":
//					nodos[i].setDimension(nodos[2 * i + 1].getDimension() / 2.0);
//					break;
//				case "Math.exp":
//					nodos[i].setDimension(0);
//					break;
//				case "Math.log":
//					nodos[i].setDimension(0);
//					break;
//				default:
//					nodos[i].setDimension(nodos[2 * i + 1].getDimension());
//					break;
//				}
//			}
//		} catch (Exception e) {
//			e.printStackTrace();
//			System.out.println("FALLO AL ASIGNAR DIMENSION");
//		}
//	}

	/*
	 * COMPARACIONES ENTRE REGLAS (MONTICULOS)
	 */
	@Override
	public boolean equals(Object obj) {
//		return Arrays.equals(nodos, ((Monticulo) obj).nodos);
		return representaMonticulo().equals(((Regla) obj).representaMonticulo());
//		return representa().equals(((Regla) obj).representa());
	}

	@Override
	public int hashCode() {
		return representaMonticulo().hashCode();
	}

	/*
	 * CARACTERISTICAS DE LA REGLA
	 */
	public int getSize() {
		return size;
	}

	public double getDimension() {
		return nodos[0].getDimension();
	}

	public int getProfundidad() {
		int ultimo = 0;
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				ultimo = i;
		return (int) (Math.log(ultimo + 1) / Math.log(2)) + 1;
	}

	public boolean esCompleta() {
		return size == (int) (Math.pow(2, getProfundidad()) - 1);
	}

	public boolean esValido() {
		if (nodos[0] == null)
			return false;
		for (int i = 0; i < nodos.length; i++) {
			if (nodos[i] != null) {
				if (i > 0) {
					if (nodos[(i - 1) / 2] == null) // padreNull
//						System.out.println("padreNull");
						return false;
					else if (nodos[(i - 1) / 2].esTerminal()) // padre terminal
//						System.out.println("padre terminal");
						return false;
				}
				if (nodos[i].esOperacion() && hoja(i)) // operacion y hoja
//					System.out.println("operacion y hoja");
					return false;
				if (!hoja(i)) {
					if (nodos[i].esTerminal() && (nodos[2 * i + 1] != null || nodos[2 * i + 2] != null))
						// es terminal y tiene hijos
//						System.out.println("es terminal y tiene hijos");
						return false;
					if (nodos[i].esOperacion() // es funcion con mal hijos
							&& (nodos[2 * i + 1] == null || (nodos[2 * i + 2] == null && !nodos[i].esUnaria())))
//						System.out.println("funcion con mal hijos");
						return false;
				}
			}
		}
		return true;
	}

	// simplifica
	public void simplifica() {
		for (int i = 0; i < nodos.length / 2; i++)
			if (nodos[i] != null && nodos[i].esBinariaInversa() && iguales(2 * i + 1, 2 * i + 2) <= 0) {
				intercambiaSubarbol(2 * i + 1, 2 * i + 2);
				i = 0;
			}
	}

	/*
	 * Metodo para reducir/simplificar las reglas
	 */
	void intercambiaSubarbol(int i, int j) {
		Regla copia = new Regla(this);
		copia.borrar(i);
		copia.insertar(i, j, this);
		copia.borrar(j);
		copia.insertar(j, i, this);
		nodos = copia.nodos;
	}

	public int getNsimbolos() {
		Set<String> simbolos = new HashSet<String>();
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				simbolos.add(nodos[i].simbolo());
		return simbolos.size();
	}

	public boolean esEnsemble() {
		return false;
	}

	@Override
	public int compareTo(Regla o) {
		return train() > o.train() ? 1 : -1;
	}

}

class FilteredPoint implements Comparable<FilteredPoint> {

	double priority;
	Point point;

	public FilteredPoint(double priority, Point point) {
		this.priority = priority;
		this.point = point;
	}

	@Override
	public int compareTo(FilteredPoint o) {
		return (int) (o.priority - priority);
	}

}