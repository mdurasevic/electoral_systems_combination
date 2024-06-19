package calculo_ensembles_colaborativos;

import java.io.File;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Scanner;

import calculo_reglas.Regla;

public class GestorNS {
	
	List<Regla> N;
	
	public GestorNS() {
		cargaExpresiones();
	}
			
	void cargaExpresiones() {
		N = new ArrayList<Regla>();
		Scanner kbd;
		String text;
		try {
			kbd = new Scanner(new File("expresiones_vecinos.csv"));
			while (kbd.hasNext()) {
				text = kbd.nextLine().split(";")[0];
				if (text != null && !text.isEmpty()) {
					N.add(new Regla().generarRegla(text));
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public List<Regla> getVecinos(Regla regla) {
		List<Regla> vecinos = new ArrayList<>();
		List<Integer> I = getI(regla);
		for (Integer i : I) {
			for (Regla expresion : N)
					vecinos.add(generaVecino(regla, i, expresion));
		}
		return vecinos;
	}
	
	public List<Regla> getVecinos(Regla regla, int maximo) {
		HashSet<Regla> unicos = new HashSet<>();
		List<Integer> I = getI(regla);
		Random r = new Random();
		Regla vecino;
		int maxIntentos = 1000;
		int i=0;
		while (unicos.size() < maximo && i < maxIntentos) {
			vecino = generaVecino(regla, I.get(r.nextInt(I.size())), N.get(r.nextInt(N.size())));
			if (vecino.esValido())
				unicos.add(vecino);
			i++;
		}
		return new ArrayList<>(unicos);
	}

	Regla generaVecino(Regla regla, int i, Regla expresion) {
		return new Regla(i, 0, regla, expresion);
	}
	
	// por defecto todos los indices
	List<Integer> getI(Regla monticulo) {
		List<Integer> I = new ArrayList<Integer>();
		for (int i = 0; i < monticulo.nodos.length; i++)
			if (monticulo.nodos[i] != null)
				I.add(i);
		return I;
	}
	
}
