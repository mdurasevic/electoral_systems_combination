package framework.calculo_reglas.local_search;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoNull;
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
import framework.calculo_reglas.representacion.nodos.terminales.NodoDValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoInicioIntervaloValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPValue;
import framework.calculo_reglas.representacion.nodos.terminales.NodoPmediaValue;
import framework.core.AlgoUtil;

public class Expresion extends Regla {

	private int id;
	private boolean completo = false;
	boolean tieneSimbolo = false, tieneP = false, tieneD = false, tieneInicioIntervalo = false;

	public Expresion() {
		super();
	}

	public Expresion(int tamañoMaximo) {
		super(tamañoMaximo);
	}

	public Expresion(List<Expresion> sucesores, int tamañoMaximo) { // estado inicial
		this(tamañoMaximo);
		this.setId(nodos.length - 1);
		sucesores.add(new Expresion(this, getId(), NodoNull.getInstance()));
		sucesores.add(new Expresion(this, getId(), NodoPValue.getInstance()));
		sucesores.add(new Expresion(this, getId(), NodoDValue.getInstance()));
		sucesores.add(new Expresion(this, getId(), NodoPmediaValue.getInstance()));
		sucesores.add(new Expresion(this, getId(), NodoInicioIntervaloValue.getInstance()));
		for (Nodo c : AlgoUtil.gestorReglas.constantes)
			sucesores.add(new Expresion(this, getId(), c.clona()));
	}

	public Expresion(Expresion padre, int id2, Nodo nuevo) { // estados sucesores
		this();
		setId(id2);
		tieneP = padre.tieneP;
		tieneD = padre.tieneD;
		tieneInicioIntervalo = padre.tieneInicioIntervalo;
		if (!nuevo.esNull()) {
			size = padre.size + 1;
			nodos[getId()] = nuevo.clona();
			for (int i = 0; i < padre.nodos.length; i++)
				if (padre.nodos[i] != null) {
					nodos[i] = padre.nodos[i].clona();
					nodos[i].setDimension(padre.nodos[i].getDimension());
				}
			dimensionSimboloEn(getId());
			if (nodos[0] != null)
				setCompleto(true);
			if (nuevo.orden() == NodoPValue.getInstance().orden())
				tieneP = true;
			if (nuevo.orden() == NodoDValue.getInstance().orden())
				tieneD = true;
			if (nuevo.orden() == NodoInicioIntervaloValue.getInstance().orden())
				tieneInicioIntervalo = true;
		} else {
			size = padre.size;
			for (int i = 0; i < padre.nodos.length; i++)
				if (padre.nodos[i] != null) {
					nodos[i] = padre.nodos[i].clona();
					nodos[i].setDimension(padre.nodos[i].getDimension());
				}
		}
		if (tieneP && tieneD && tieneInicioIntervalo)
			tieneSimbolo = true;
	}

	public List<Expresion> expande(List<Expresion> sucesores) {
		sucesores.clear();
		int i = getId() - 1;
		if (i >= 0 && size < nodos.length) { //
			if (hoja(i) || (nodos[2 * i + 1] == null)) { // terminal
				if (!hijoDerecho(i) && nodos[i + 1] != null) {
					/*
					 * Poda expresiones como max(p,p), pxp... pero también 0.25+0.5, no expresiones
					 * como pow2(0.25), exp(0.25)... Se podan expresiones entre binarias donde ambos
					 * terminales son iguales, o ambos terminales son constantes.
					 */
					if (nodos[i + 1].orden() != NodoPValue.getInstance().orden()) // tiene sentido tener max(p,p)
						sucesores.add(new Expresion(this, i, NodoPValue.getInstance()));
					if (nodos[i + 1].orden() != NodoDValue.getInstance().orden()) // lo mismo
						sucesores.add(new Expresion(this, i, NodoDValue.getInstance()));
					if (nodos[i + 1].orden() != NodoPmediaValue.getInstance().orden())// lo mismo
						sucesores.add(new Expresion(this, i, NodoPmediaValue.getInstance()));
					if (nodos[i + 1].orden() != NodoInicioIntervaloValue.getInstance().orden())
						sucesores.add(new Expresion(this, i, NodoInicioIntervaloValue.getInstance()));
					if (!nodos[i + 1].esConstante())
						for (Nodo c : AlgoUtil.gestorReglas.constantes)
							sucesores.add(new Expresion(this, i, c.clona()));
				} else {
					sucesores.add(new Expresion(this, i, NodoNull.getInstance()));
					sucesores.add(new Expresion(this, i, NodoPValue.getInstance()));
					sucesores.add(new Expresion(this, i, NodoDValue.getInstance()));
					sucesores.add(new Expresion(this, i, NodoPmediaValue.getInstance()));
					sucesores.add(new Expresion(this, i, NodoInicioIntervaloValue.getInstance()));
					for (Nodo c : AlgoUtil.gestorReglas.constantes)
						sucesores.add(new Expresion(this, i, c.clona()));
				}
			} else { // funcion
				if (nodos[2 * i + 2] != null) {
					int value = iguales(2 * i + 1, 2 * i + 2);
					/*
					 * value = 0: Podamos si ambos subarboles son iguales; value < 1: Podamos
					 * conmutativos si subarbol izquierdo < subarbol derecho; value > 1: Se generan
					 * todas los estados
					 */
					if (nodos[2 * i + 1].getDimension() == nodos[2 * i + 2].getDimension()) {
						if (value < 0) {
							sucesores.add(new Expresion(this, i, new NodoResta()));
							sucesores.add(new Expresion(this, i, new NodoDivision()));
						} else if (value > 0) {
							sucesores.add(new Expresion(this, i, new NodoSuma()));
							sucesores.add(new Expresion(this, i, new NodoResta()));
							sucesores.add(new Expresion(this, i, new NodoMax()));
							sucesores.add(new Expresion(this, i, new NodoMin()));
							sucesores.add(new Expresion(this, i, new NodoDivision()));
							sucesores.add(new Expresion(this, i, new NodoMultiplicacion()));
						} else {
						}
					} else {
						if (value < 0) {
							sucesores.add(new Expresion(this, i, new NodoDivision()));
						} else if (value > 0) {
							sucesores.add(new Expresion(this, i, new NodoDivision()));
							sucesores.add(new Expresion(this, i, new NodoMultiplicacion()));
						} else {
						}
					}
				} else { // unaria
					if (nodos[2 * i + 1].getDimension() == 0) {
						sucesores.add(new Expresion(this, i, new NodoExponencial()));
						sucesores.add(new Expresion(this, i, new NodoLoge()));
						/*
						 * Poda expresiones max(p,0), min(p, 0), max(max(?,0)), min(max(?,0)),
						 * min(min(?,0)), max(min(?,0))
						 */
						if (!nodos[2 * i + 1].esTerminal() && nodos[2 * i + 1].orden() != 13
								&& nodos[2 * i + 1].orden() != 11) {
							sucesores.add(new Expresion(this, i, new NodoMax0()));
							sucesores.add(new Expresion(this, i, new NodoMin0()));
						}
						sucesores.add(new Expresion(this, i, new NodoRaiz()));
						sucesores.add(new Expresion(this, i, new NodoPotencia()));
						/*
						 * Poda expresiones donde el subarbol no sea terminal, esto elimina expresiones
						 * -(-(?))=?, -(?+?)=-?-?...
						 */
						if (nodos[2 * i + 1].orden() <= 4)
							sucesores.add(new Expresion(this, i, new NodoNegacion()));
					} else {
						/*
						 * Poda expresiones max(p,0), min(p, 0), max(max(?,0)), min(max(?,0)),
						 * min(min(?,0)), max(min(?,0))
						 */
						if (!nodos[2 * i + 1].esTerminal() && nodos[2 * i + 1].orden() != 13
								&& nodos[2 * i + 1].orden() != 11) {
							sucesores.add(new Expresion(this, i, new NodoMax0()));
							sucesores.add(new Expresion(this, i, new NodoMin0()));
						}
						sucesores.add(new Expresion(this, i, new NodoRaiz()));
						sucesores.add(new Expresion(this, i, new NodoPotencia()));
						/*
						 * Poda expresiones donde el subarbol no sea terminal, esto elimina expresiones
						 * -(-(?))=?, -(?+?)=-?-?...
						 */
						if (nodos[2 * i + 1].orden() <= 4)
							sucesores.add(new Expresion(this, i, new NodoNegacion()));
					}
				}
			}
		}
		return sucesores;
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

	@Override
	public Expresion getClon() {
		Expresion clon = new Expresion();
		for (int i = 0; i < nodos.length; i++)
			if (nodos[i] != null) {
				clon.nodos[i] = nodos[i].clona();
				clon.nodos[i].setDimension(nodos[i].getDimension());
			}
		clon.size = size;
		clon.setCompleto(completo);
		clon.setId(id);
		return clon;
	}

	public boolean isCompleto() {
		return completo;
	}

	public void setCompleto(boolean completo) {
		this.completo = completo;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}
}