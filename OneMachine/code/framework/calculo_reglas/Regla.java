package framework.calculo_reglas;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ThreadLocalRandom;

import dominio.Instancia;
import dominio.Intervalo;
import dominio.OperacionesAsignacionTrabajosMaquina;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante;
import framework.calculo_reglas.representacion.nodos.operadores.NodoDivision;
import framework.calculo_reglas.representacion.nodos.operadores.NodoExponencial;
import framework.calculo_reglas.representacion.nodos.operadores.NodoLoge;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMax0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMin0;
import framework.calculo_reglas.representacion.nodos.operadores.NodoMultiplicacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoNegacion;
import framework.calculo_reglas.representacion.nodos.operadores.NodoPotencia;
import framework.calculo_reglas.representacion.nodos.operadores.NodoRaiz;
import framework.calculo_reglas.representacion.nodos.operadores.NodoResta;
import framework.calculo_reglas.representacion.nodos.operadores.NodoSuma;
import framework.calculo_reglas.representacion.nodos.terminales.NodoCapArea;
import framework.calculo_reglas.representacion.nodos.terminales.NodoCapLineal;
import framework.calculo_reglas.representacion.nodos.terminales.NodoDValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoDmediaValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoFinIntervaloValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoInicioIntervaloValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoNJobs;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPmediaValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoSimulateTardinessValue;
import framework.core.AlgoUtil;

public class Regla extends PlanificadorReglas implements Comparable<Regla> {
	/*
	 * VARIABLES
	 */
	public Nodo[] nodos;
	public int size;
	protected String regla;
	protected boolean valido = false;

	/*
	 * CONSTRUCTORES
	 */
	public Regla() {
		this.nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.gestorReglas.profundidadMaximaGlobal) - 1)];
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
		if (AlgoUtil.gestorReglas.metodoKoza)
			return metodoKoza();
		else
			return random_private();
	}

	private Regla random_private() {
		do {
			nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.gestorReglas.profundidadMaximaGlobal) - 1)];
			size = 0;
			generarDegenerado(nodos.length);
			// se comprueba que la regla es válida
			if (esValido())
				valido = true;
		} while (!valido);
		return this;
	}

	public Regla metodoKoza() {
		// completo o degenerado aleatoriamente
		boolean completo = true;

		// profundidad máxima ente 1 y n
		int profundidadMaxima = (ThreadLocalRandom.current().nextInt(1,
				AlgoUtil.gestorReglas.profundidadMaximaInicialReglas) + 1);

		// si nos pasamos de un limite
		if (completo && profundidadMaxima > AlgoUtil.gestorReglas.profundidadMaximaCompletasReglas)
			profundidadMaxima = AlgoUtil.gestorReglas.profundidadMaximaCompletasReglas;

		int tamañoMaximo = (int) (Math.pow(2, profundidadMaxima) - 1);

		// generamos la regla
		do {
			nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.gestorReglas.profundidadMaximaGlobal) - 1)];
			size = 0;
			if (completo)
				generarCompleto(tamañoMaximo);
			else
				generarDegenerado(tamañoMaximo);

			// se comprueba que la regla es válida
			if (esValido())
				valido = true;

		} while (!valido);
		return this;
	}

	private void generarCompleto(int tamañoMaximo) {
		for (int i = tamañoMaximo - 1; i >= 0; i--) {
			if (i >= (int) tamañoMaximo / 2) {
				generaTerminalConstante(i);
				size++;
			} else {
				if (nodos[2 * i + 1] != null && nodos[2 * i + 2] != null) {
					if (Double.compare(nodos[2 * i + 1].getDimension(), nodos[2 * i + 2].getDimension()) == 0) 
						generaBinaria(i);
					else
						generaBinariaAlteraDimension(i);
				} else
					generaBinariaAlteraDimension(i);
				size++;
				dimensionSimboloEn(i);
			}
		}
	}
	
	public Regla generarDegeneradoPublic(int sizeMaximo) {
		generarDegenerado(sizeMaximo);
		return this;
	}

	private void generarDegenerado(int sizeMaximo) {
		List<String> simbolos = new ArrayList<String>();
		nodos = new Nodo[(int) (Math.pow(2, AlgoUtil.gestorReglas.profundidadMaximaGlobal) - 1)];
		size = 0;
		for (int i = sizeMaximo - 1; i >= 0; i--) {

//			System.out.println("vuelvo");
			simbolos.clear();

			if (i >= (int) sizeMaximo / 2 || (nodos[2 * i + 1] == null)) { // terminal
//				System.out.println("terminal");
				if (!hijoDerecho(i) && nodos[i + 1] != null) {
					if (nodos[i + 1].orden() != NodoPValue.getInstance().orden())
						simbolos.add("p");
					if (nodos[i + 1].orden() != NodoDValue.getInstance().orden())
						simbolos.add("d");
					if (nodos[i + 1].orden() != NodoPmediaValue.getInstance().orden())
						simbolos.add("pmedia");
					if (nodos[i + 1].orden() != NodoInicioIntervaloValue.getInstance().orden())
						simbolos.add("ii");
					
					// nuevos terminales
//					if (nodos[i + 1].orden() != NodoFinIntervaloValue.getInstance().orden())
//						simbolos.add("fi");
//					if (nodos[i + 1].orden() != NodoCapLineal.getInstance().orden())
//						simbolos.add("cl");
//					if (nodos[i + 1].orden() != NodoCapArea.getInstance().orden())
//						simbolos.add("ca");
//					if (nodos[i + 1].orden() != NodoSimulateTardinessValue.getInstance().orden())
//						simbolos.add("st");
//					if (nodos[i + 1].orden() != NodoDmediaValue.getInstance().orden())
//						simbolos.add("dmedia");
//					if (nodos[i + 1].orden() != NodoNJobs.getInstance().orden())
//						simbolos.add("N");
					
									
					if (!nodos[i + 1].esConstante())
						simbolos.add("c");
				} else {
					for (int j = 0; j < AlgoUtil.gestorReglas.indiceNullHoja; j++)
						simbolos.add("null");
					simbolos.add("p");
					simbolos.add("d");
					simbolos.add("pmedia");
					simbolos.add("ii");
					
					// nuevos terminales
//					simbolos.add("fi");
//					simbolos.add("cl");
//					simbolos.add("ca");
//					simbolos.add("st");
//					simbolos.add("dmedia");
//					simbolos.add("N");

					
					simbolos.add("c");
				}
			}

			else { // funcion
				if (nodos[2 * i + 2] != null) { // binaria
//					System.out.println("binaria");
					int value = iguales(2 * i + 1, 2 * i + 2);
					if (Double.compare(nodos[2 * i + 1].getDimension(), nodos[2 * i + 2].getDimension()) == 0) {
//						System.out.println("dimensional");
						if (value < 0) {
							simbolos.add("resta");
							simbolos.add("/");
						} else if (value > 0) {
							simbolos.add("+");
							simbolos.add("resta");
							simbolos.add("max");
							simbolos.add("min");
							simbolos.add("*");
							simbolos.add("/");
						} else {
//							System.out.println("reinicia");
							generarDegenerado(sizeMaximo);
						}
					} else {
//						System.out.println("no dimensional");
						if (value < 0) {
//							System.out.println("poda");
							simbolos.add("/");
						} else if (value > 0) {
//							System.out.println("no poda");
							simbolos.add("/");
							simbolos.add("*");
						} else {
//							System.out.println("reinicia");
							generarDegenerado(sizeMaximo); // la expresion se poda y no se genera
						}
					}
				}

				else { // unaria
//					System.out.println("unaria");
					if (Double.compare(nodos[2 * i + 1].getDimension(), 0.0) == 0) {	
						simbolos.add("exp");
						simbolos.add("log");
						/*
						 * Poda expresiones max(p,0), min(p, 0), max(max(?,0)), min(max(?,0)),
						 * min(min(?,0)), max(min(?,0))
						 */
						if (!nodos[2 * i + 1].esTerminal() && nodos[2 * i + 1].orden() != 13
								&& nodos[2 * i + 1].orden() != 11) {
							simbolos.add("max0");
							simbolos.add("min0");
						}
						simbolos.add("sqrt");
						simbolos.add("pow");
						/*
						 * Poda expresiones donde el subarbol no sea terminal, esto elimina expresiones
						 * -(-(?))=?, -(?+?)=-?-?...
						 */
						if (nodos[2 * i + 1].orden() <= 4)
							simbolos.add("neg");
					} else {
						/*
						 * Poda expresiones max(p,0), min(p, 0), max(max(?,0)), min(max(?,0)),
						 * min(min(?,0)), max(min(?,0))
						 */
						if (!nodos[2 * i + 1].esTerminal() && nodos[2 * i + 1].orden() != 13
								&& nodos[2 * i + 1].orden() != 11) {
							simbolos.add("max0");
							simbolos.add("min0");
						}
						simbolos.add("sqrt");
						simbolos.add("pow");
						/*
						 * Poda expresiones donde el subarbol no sea terminal, esto elimina expresiones
						 * -(-(?))=?, -(?+?)=-?-?...
						 */
						if (!nodos[2 * i + 1].esConstante())
							simbolos.add("neg");
					}
				}

			}

			if (simbolos.size() == 0)
				generarDegenerado(sizeMaximo);
			
//			System.out.println("simbolos:" + simbolos.size());
			if (simbolos.size() > 0) {
				String simbolo = simbolos.get((int) (simbolos.size() * Math.random()));
				switch (simbolo) {
				// terminales
				case "p":
					nodos[i] = NodoPValue.getInstance();
					break;
				case "d":
					nodos[i] = NodoDValue.getInstance();
					break;
				case "ii":
					nodos[i] = NodoInicioIntervaloValue.getInstance();
					break;
				case "pmedia":
					nodos[i] = NodoPmediaValue.getInstance();
					break;
				// nuevos terminales
				case "fi":
					nodos[i] = NodoFinIntervaloValue.getInstance();
					break;	
				case "cl":
					nodos[i] = NodoCapLineal.getInstance();
					break;	
				case "ca":
					nodos[i] = NodoCapArea.getInstance();
					break;	
				case "st":
					nodos[i] = NodoSimulateTardinessValue.getInstance();
					break;
				case "dmedia":
					nodos[i] = NodoDmediaValue.getInstance();
					break;
				case "N":
					nodos[i] = NodoNJobs.getInstance();
					break;
					
				case "c":
					nodos[i] = AlgoUtil.gestorReglas.getConstanteRandom();
					break;

				// unarias
				case "exp":
					nodos[i] = new NodoExponencial();
					break;
				case "log":
					nodos[i] = new NodoLoge();
					break;
				case "max0":
					nodos[i] = new NodoMax0();
					break;
				case "min0":
					nodos[i] = new NodoMin0();
					break;
				case "neg":
					nodos[i] = new NodoNegacion();
					break;
				case "pow":
					nodos[i] = new NodoPotencia();
					break;
				case "sqrt":
					nodos[i] = new NodoRaiz();
					break;

				// binarias
				case "+":
					nodos[i] = new NodoSuma();
					break;
				case "resta":
					nodos[i] = new NodoResta();
					break;
				case "max":
					nodos[i] = new NodoMax();
					break;
				case "min":
					nodos[i] = new NodoMin();
					break;
				case "/":
					nodos[i] = new NodoDivision();
					break;
				case "*":
					nodos[i] = new NodoMultiplicacion();
					break;

				}
				if (nodos[i] != null) {
					size++;
					if (nodos[i].esOperacion())
						dimensionSimboloEn(i);
				}
//			System.out.println(nodos[i]);
			}
		}
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

	int nTerminales = 4 + 1; // +1 constantes numericas
	
	private void generaHoja(int i) { // se añade el null
		int indiceNull = nTerminales + AlgoUtil.gestorReglas.indiceNullHoja;
		switch ((int) (Math.random() * indiceNull)) {
		case 0:
			nodos[i] = NodoDValue.getInstance();
			break;
		case 1:
			nodos[i] = NodoPValue.getInstance();
			break;
		case 2:
			nodos[i] = NodoPmediaValue.getInstance();
			break;
		case 3:
			nodos[i] = NodoInicioIntervaloValue.getInstance();
			break;
		case 4:
			nodos[i] = AlgoUtil.gestorReglas.getConstanteRandom();
			break;
		// nuevo terminales
//		case 5:
//			nodos[i] = NodoFinIntervaloValue.getInstance();
//			break;
//		case 6:
//			nodos[i] = NodoCapLineal.getInstance();
//			break;
//		case 7:
//			nodos[i] = NodoCapArea.getInstance();
//			break;
//		case 8:
//			nodos[i] =NodoSimulateTardinessValue.getInstance();
//			break;
//		case 9:
//			nodos[i] = NodoDmediaValue.getInstance();
//			break;
//		case 10:
//			nodos[i] = NodoNJobs.getInstance();
//			break;
		default: // null
			nodos[i] = null;
			break;
		}
	}

	private void generaTerminalConstante(int i) {
		switch ((int) (Math.random() * nTerminales)) {
		case 0:
			nodos[i] = NodoDValue.getInstance();
			break;
		case 1:
			nodos[i] = NodoPValue.getInstance();
			break;
		case 2:
			nodos[i] = NodoPmediaValue.getInstance();
			break;
		case 3:
			nodos[i] = NodoInicioIntervaloValue.getInstance();
			break;
		// nuevos terminales
//		case 4:
//			nodos[i] = NodoFinIntervaloValue.getInstance();
//			break;
//		case 5:
//			nodos[i] = NodoCapLineal.getInstance();
//			break;
//		case 6:
//			nodos[i] = NodoCapArea.getInstance();
//			break;
//		case 7:
//			nodos[i] =NodoSimulateTardinessValue.getInstance();
//			break;
//		case 8:
//			nodos[i] = NodoDmediaValue.getInstance();
//			break;
//		case 9:
//			nodos[i] = NodoNJobs.getInstance();
//			break;
		default:
			nodos[i] = AlgoUtil.gestorReglas.getConstanteRandom();
			break;
		}
	}

	private void generaTerminal(int i) {
		switch ((int) (Math.random() * (nTerminales -1))) {
		case 0:
			nodos[i] = NodoDValue.getInstance();
			break;
		case 1:
			nodos[i] = NodoPValue.getInstance();
			break;
		case 2:
			nodos[i] = NodoPmediaValue.getInstance();
			break;
		case 3:
			nodos[i] = NodoInicioIntervaloValue.getInstance();
			break;
		// nuevos terminales
//		case 4:
//			nodos[i] = NodoFinIntervaloValue.getInstance();
//			break;
//		case 5:
//			nodos[i] = NodoCapLineal.getInstance();
//			break;
//		case 6:
//			nodos[i] = NodoCapArea.getInstance();
//			break;
//		case 7:
//			nodos[i] =NodoSimulateTardinessValue.getInstance();
//			break;
//		case 8:
//			nodos[i] = NodoDmediaValue.getInstance();
//			break;
//		case 9:
//			nodos[i] = NodoNJobs.getInstance();
//			break;	
		}
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

	private void generaBinariaAlteraDimension(int i) { // solo las binarias que alteran la dimensión
		switch ((int) (Math.random() * 2)) {
		case 0:
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

	private void generaUnariaAlteraDimension(int i) {
		switch ((int) (Math.random() * 5)) {
		case 0:
			nodos[i] = new NodoRaiz();
			break;
		case 1:
			nodos[i] = new NodoMax0();
			break;
		case 2:
			nodos[i] = new NodoMin0();
			break;
		case 3:
			nodos[i] = new NodoNegacion();
			break;
		default:
			nodos[i] = new NodoPotencia();
			break;
		}
	}

	/*
	 * CARGAR REGLAS YA DEFINIDAS
	 */
	public Regla generarRegla(String regla) {
		int tamañoMaximo = (int) (Math.pow(2, AlgoUtil.gestorReglas.profundidadMaximaGlobal) - 1);
		nodos = new Nodo[tamañoMaximo];
		String[] valores = regla.split(" ");
		for (int i = valores.length - 1; i >= 0; i--)
			if (!valores[i].equals("null")) {
				size++;
				switch (valores[i]) {
				case "/":
					nodos[i] = new NodoDivision();
					break;
				case "instancia.d[tarea]":
					nodos[i] = NodoDValue.getInstance();
					break;
				case "Math.exp":
					nodos[i] = new NodoExponencial();
					break;
				case "intervalo.inicio":
					nodos[i] = NodoInicioIntervaloValue.getInstance();
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
				case "promedioDuraciones()":
					nodos[i] = NodoPmediaValue.getInstance();
					break;
				case "Math.pow":
					nodos[i] = new NodoPotencia();
					break;
				case "instancia.p[tarea]":
					nodos[i] = NodoPValue.getInstance();
					break;
				case "Math.sqrt":
					nodos[i] = new NodoRaiz();
					break;
				case "+":
					nodos[i] = new NodoSuma();
					break;
				case "intervalo.fin":
					nodos[i] = NodoFinIntervaloValue.getInstance();
					break;
				case "cap.lineal":
					nodos[i] = NodoCapLineal.getInstance();
					break;
				case "cap.area":
					nodos[i] = NodoCapArea.getInstance();
					break;
				case "simulateTardiness[tarea]":
					nodos[i] = NodoSimulateTardinessValue.getInstance();
					break;
				default:
					// SI parsea mal las reglas mirar aqui
					/*
					 * if (valores[i].equals("0") || valores[i].equals("0.0")) { if ((valores[(i -
					 * 1) / 2].equals("Math.min") || valores[(i - 1) / 2].equals("Math.max") && i %
					 * 2 != 0 && i != 0)) { tamaño--; // restamos el tamaño y no hacemos nada } else
					 * { nodos[i] = SetConstantes.get(Double.parseDouble(valores[i])); } } else {
					 * nodos[i] = SetConstantes.get(Double.parseDouble(valores[i])); }
					 */
					nodos[i] = AlgoUtil.gestorReglas.getConstante(Double.parseDouble(valores[i]));
					break;
				}
				if (nodos[i] != null)
					if (nodos[i].esOperacion())
						dimensionSimboloEn(i);
			}
		return this;
	}

	public Regla generarATC(double g) {
		nodos[0] = new NodoMultiplicacion();
		nodos[1] = new NodoDivision();
		nodos[2] = new NodoExponencial();
		nodos[3] = AlgoUtil.gestorReglas.getConstante(1.0);
		nodos[4] = NodoPValue.getInstance();
		nodos[5] = new NodoDivision();
		nodos[11] = new NodoNegacion();
		nodos[12] = new NodoMultiplicacion();
		nodos[23] = new NodoMax();
		nodos[25] = new NodoConstante(g);
		nodos[26] = NodoPmediaValue.getInstance();
		nodos[47] = AlgoUtil.gestorReglas.getConstante(0.0);
		nodos[48] = new NodoResta();
		nodos[97] = new NodoDValue();
		nodos[98] = new NodoSuma();
		nodos[197] = NodoInicioIntervaloValue.getInstance();
		nodos[198] = NodoPValue.getInstance();
		size = 17;
		asignaDimension();
		return this;
	}

	public Regla generarSPT() {
		nodos[0] = new NodoDivision();
		nodos[1] = AlgoUtil.gestorReglas.getConstante(1.0);
		nodos[2] = NodoPValue.getInstance();
		size = 3;
		asignaDimension();
		return this;
	}

	public Regla generarEDD() {
		nodos[0] = new NodoDivision();
		nodos[1] = AlgoUtil.gestorReglas.getConstante(1.0);
		nodos[2] = NodoDValue.getInstance();
		size = 3;
		asignaDimension();
		return this;
	}
	
	public Regla generarEDD2() {
		nodos[0] = NodoDValue.getInstance();
		size = 1;
		asignaDimension();
		return this;
	}

	public Regla generarBonita(double g) {
		nodos[0] = new NodoMin();
		nodos[1] = new NodoMultiplicacion();
		nodos[2] = new NodoNegacion();
		nodos[3] = new NodoResta();
		nodos[4] = AlgoUtil.gestorReglas.getConstante(g);
		nodos[5] = NodoPValue.getInstance();
		nodos[7] = NodoInicioIntervaloValue.getInstance();
		nodos[8] = NodoDValue.getInstance();
		size = 8;
		asignaDimension();
		return this;
	}

	public void asignaDimension() {
		for (int i = nodos.length - 1; i >= 0; i--) {
			if (nodos[i] != null)
				dimensionSimboloEn(i);
		}
	}

	/*
	 * RECORRIDOS DEL ÁRBOL
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

	public int posAzarMismaDim(Regla padre, int index) {
		List<Integer> pos = new ArrayList<Integer>();
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (Double.compare(nodos[i].getDimension(), padre.nodos[index].getDimension()) == 0 && !iguales(subarbol(i), padre.subarbol(index))) // si se omite && !iguales(subarbol(i), padre.subarbol(index)) 
					pos.add(i);
		if (!pos.isEmpty())
			return pos.get((int) (Math.random() * pos.size()));
		return -1;
	}

	private boolean iguales(List<String> subarbol, List<String> subarbol2) {
		if (subarbol.size() != subarbol2.size())
			return false;
		else
			for (int i = 0; i < subarbol.size(); i++)
				if (!subarbol.get(i).equals(subarbol2.get(i)))
					return false;
		return true;
	}

	/*
	 * RESOLVER CONJUNTO DE ENTRENAMIENTO
	 */
	public boolean evalTrain = false;
	public double train;
	protected Map<String, Integer> solutions_train;

	public double train() {
		if (!evalTrain) {
			solutions_train = new HashMap<String, Integer>();
			train = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.training) {
//				solucion = new PlanificadorReglas(this).planifica(instancia);
				double avg = 0.0;
				int n=1;
				for (int i = 0 ; i<n; i++) {
					solucion = planifica(instancia);
//					solutions_train.put(instancia.getNombre(), solucion);
					avg += solucion;
				}
				avg /= n;
				train += avg;
			}
			train /= AlgoUtil.gestorInstancias.training.size();
			evalTrain = true;
		}
		return train;
	}

	public int train(String instancia) {
		train();
		return solutions_train.get(instancia);
	}

	/*
	 * RESOLVER CONJUNTO DE TEST
	 */
	public boolean evalTest;
	protected double test;
	protected Map<String, Integer> solutions_test;

	public double test() {
		if (!evalTest) {
			solutions_test = new HashMap<String, Integer>();
			test = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.set_test) {
//				solucion = new PlanificadorReglas(this).planifica(instancia);
				solucion = planifica(instancia);
				solutions_test.put(instancia.getNombre(), solucion);
				test += solucion;
			}
			test /= AlgoUtil.gestorInstancias.set_test.size();
			evalTest = true;
		}
		return test;
	}

	public int test(String instancia) {
		test();
		return solutions_test.get(instancia);
	}

	/*
	 * RESOLVER CONJUNTO DE FILTRO
	 */
	public boolean evalFilter;
	protected double filter;

	public double filter() {
		if (!evalFilter) {
			filter = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.filtro) {
//				solucion = new PlanificadorReglas(this).planifica(instancia);
				solucion = planifica(instancia);
				filter += solucion;
			}
			filter /= AlgoUtil.gestorInstancias.filtro.size();
			evalFilter = true;
		}
		return filter;
	}

	/*
	 * CALCULAR LA PRIORIDAD -> ESTOS MÉTODOS LOS USA EL PLANIFICADOR
	 */
	@Override
	protected int buscarMejorTarea(Intervalo intervalo) {
		prioridad = new double[instancia.p.length];
		for (Integer tarea : sin_asignar)
			if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
					OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
					OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
				prioridad[tarea] = calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
			}
		return buscarTareaMasPrioritaria(intervalo.inicio);
	}
	
	public double calculaPrioridadDe(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo, 0);
	}

	private double calculaPrioridadDe(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo,
			int i) {
		if (nodos[i] != null) { // ojo esto es nuevo!
			if (nodos[i].esTerminal())
				return nodos[i].value(instancia, sin_asignar, tarea, intervalo);
			else {
				double izq = 0, der = 0;
				if ((2 * i + 1) < nodos.length) {
					izq = calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo, (2 * i + 1));
					if ((2 * i + 2) < nodos.length && !nodos[i].esUnaria())
						der = calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo, (2 * i + 2));
				}
				return nodos[i].value(izq, der);
			}
		}
		return 0;
	}
	// fin calculo coste conjuntos

	/*
	 * OPERACIONES Y COMPROBACIONES DEBIDAS A LA IMPLEMENTACIÓN CON MONTICULOS
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

	public boolean contieneP() {
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (nodos[i].orden() == NodoPValue.getInstance().orden())
					return true;
		return false;
	}

	public boolean contieneD() {
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (nodos[i].orden() == NodoDValue.getInstance().orden())
					return true;
		return false;
	}

	public boolean contieneInicioIntervalo() {
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (nodos[i].orden() == NodoInicioIntervaloValue.getInstance().orden())
					return true;
		return false;
	}

	public boolean contienePmedia() {
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (nodos[i].orden() == NodoPmediaValue.getInstance().orden())
					return true;
		return false;
	}

	public boolean contienePoD() {
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null)
				if (nodos[i].orden() == NodoPValue.getInstance().orden()
						|| nodos[i].orden() == NodoDValue.getInstance().orden())
					return true;
		return false;
	}

	public boolean contienePDInicioIntervalo() {
		boolean p = false, d = false, inicioIntervalo = false;
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null) {
				if (nodos[i].orden() == NodoPValue.getInstance().orden())
					p = true;
				if (nodos[i].orden() == NodoInicioIntervaloValue.getInstance().orden())
					d = true;
				if (nodos[i].orden() == NodoDValue.getInstance().orden())
					inicioIntervalo = true;
			}
		if (p && d && inicioIntervalo)
			return true;
		else
			return false;
	}

	public boolean contieneTerminales() {
		boolean p = false, d = false, inicioIntervalo = false, pmedia = false;
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null) {
				if (nodos[i].orden() == NodoPValue.getInstance().orden())
					p = true;
				if (nodos[i].orden() == NodoInicioIntervaloValue.getInstance().orden())
					d = true;
				if (nodos[i].orden() == NodoDValue.getInstance().orden())
					inicioIntervalo = true;
				if (nodos[i].orden() == NodoPmediaValue.getInstance().orden())
					pmedia = true;
			}
		if (p && d && inicioIntervalo && pmedia)
			return true;
		else
			return false;
	}

	public boolean esValido() {
		if (nodos[0] == null)
			return false;
		for (int i = 0; i < nodos.length; i++) {
			if (nodos[i] != null) {
				if (i > 0) {
					if (nodos[(i - 1) / 2] == null) // padreNull
						return false;
//						System.out.println("padreNull");
					else if (nodos[(i - 1) / 2].esTerminal()) // padre terminal
						return false;
//						System.out.println("padre terminal");

				}
				if (nodos[i].esOperacion() && hoja(i)) // operacion y hoja
					return false;
//					System.out.println("operacion y hoja");
				if (!hoja(i)) {
					if (nodos[i].esTerminal() && (nodos[2 * i + 1] != null || nodos[2 * i + 2] != null))
						// es terminal y tiene hijos
						return false;
//						System.out.println("es terminal y tiene hijos");
					if (nodos[i].esOperacion() // es funcion con mal hijos
							&& (nodos[2 * i + 1] == null || (nodos[2 * i + 2] == null && !nodos[i].esUnaria())))
						return false;
//						System.out.println("funcion con mal hijos");
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

	@Override
	public int compareTo(Regla o) {
		return train() > o.train() ? 1 : -1;
	}

}