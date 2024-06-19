package framework.calculo_ensembles_colaborativos;

import java.util.List;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;

import framework.calculo_reglas.Regla;

public abstract class Ensemble extends Regla {

	public Regla[] reglas;
	public boolean calculadaReglaMenosAporta;

	public Ensemble(int cardinalidadEnsembles) {
		this.reglas = new Regla[cardinalidadEnsembles];
	}

	public Ensemble(Ensemble ensemble) {
		this.reglas = new Regla[ensemble.reglas.length];
		for (int i = 0; i < ensemble.reglas.length; i++)
			reglas[i] = ensemble.reglas[i];
	}

	public Ensemble(Regla[] copia) {
		int tamañoReal = 0;
		for (Regla regla : copia)
			if (regla == null)
				break;
			else
				tamañoReal++;
		reglas = new Regla[tamañoReal];
		for (int i = 0; i < tamañoReal; i++)
			if (copia[i] == null)
				break;
			else
				reglas[i] = copia[i];
	}

	public abstract Ensemble random(List<Regla> seleccionables);

	public int getSizeReal() {
		int sizeReal = 0;
		for (int i = 0; i < reglas.length; i++)
			if (reglas[i] != null)
				sizeReal++;
			else
				break;
		return sizeReal;
	}

	@Override
	public int getSize() {
		return reglas.length;
	}

	public void inserta(int index, Regla regla) {
		reglas[index] = regla;
		calculadaReglaMenosAporta = false;
		evalTrain = false;
		evalTest = false;
	}

	// Implementing Fisher–Yates shuffle
	public void mezclar() {
		Random rnd = ThreadLocalRandom.current();
		for (int i = reglas.length - 1; i > 0; i--) {
			int index = rnd.nextInt(i + 1);
			Regla a = reglas[index];
			reglas[index] = reglas[i];
			reglas[i] = a;
		}
	}

	public Regla get(int index) {
		return reglas[index];
	}

	@Override
	public String representa() {
		String ensemble = "[";
		for (Regla regla : reglas)
			if (regla != null)
				ensemble += regla.representa() + ",";
			else
				break;
		return ensemble + "]";
	}

	@Override
	public String representaMonticulo() {
		String ensemble = "[";
		for (Regla regla : reglas)
			if (regla != null)
				ensemble += regla.representaMonticulo() + ",";
			else
				break;
		return ensemble + "]";
	}
}
