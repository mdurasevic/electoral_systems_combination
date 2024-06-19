package framework.calculo_reglas.gp;

import java.io.File;
import java.util.HashSet;
import java.util.List;
import java.util.Scanner;
import java.util.Set;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;
import framework.core.Poblacion;

public class Poblacion_Reglas extends Poblacion<Regla> {

	public Poblacion_Reglas(int size) {
		super(size);
	}

	public void generaRandom() {
		for (int i = 0; i < sizeMax; i++)
			poblacion.add(new Regla().random());
	}

	@Override
	public Regla getBest() {
		if (mejor == null) {
			mejor = get(0);
			for (Regla cromosoma : poblacion) {
				if (AlgoUtil.evaluadorReglas.mejoraReal(cromosoma, mejor))
					mejor = cromosoma;
			}
		}
		return mejor;
	}

	public double getAvg() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += AlgoUtil.evaluadorReglas.getFullEvaluate(poblacion.get(i)).train();
		return 1.0 * coste / poblacion.size();
	}

	public double getSizeMedio() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += poblacion.get(i).getSize();
		return 1.0 * coste / poblacion.size();
	}

	public double getProfundidadMedia() {
		double coste = 0;
		for (int i = 0; i < poblacion.size(); i++)
			coste += poblacion.get(i).getProfundidad();
		return 1.0 * coste / poblacion.size();
	}

	public void cargaReglas(String reglas) {
		String fichero = "reglas";
		Scanner kbd;
		Set<String> strings = new HashSet<String>();
		try {
			kbd = new Scanner(new File("conf/" + fichero + ".csv"));
			while (kbd.hasNextLine()) {
				String regla = kbd.nextLine();
				if (!regla.equals("")) {
					if (!strings.contains(regla) && poblacion.size() < sizeMax)
						poblacion.add(new Regla().generarRegla(regla));
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public List<Regla> getPopulation() {
		return poblacion;
	}
}
