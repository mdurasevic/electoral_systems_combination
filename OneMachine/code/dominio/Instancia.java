package dominio;

import java.io.File;
import java.util.ArrayList;
import java.util.Scanner;

public class Instancia {


	// valores estáticos de inicializacion
	public String nombre;
	public double[] p;
	public double[] d;
	public ArrayList<Intervalo> perfilMaquina;

	public Instancia(String directorio, String fichero) {
		carga(directorio, fichero);
	}
	
	public Instancia(String fichero) {
		carga(fichero);
	}

	public void carga(String directorio, String fichero) {
		Scanner kbd;
		nombre = fichero.replaceAll(".txt", "").replaceAll(directorio, "");
		try {
			kbd = new Scanner(new File(directorio + fichero));
			boolean encontreUltimoIntervalo = false;
			int inicio, fin, capacidad, indice = 0;
			kbd.next(); // NOP:
			p = new double[Integer.parseInt(kbd.next())];
			d = new double[p.length];
			kbd.next(); // NINT:
			perfilMaquina = new ArrayList<>(Integer.parseInt(kbd.next()));
			while (kbd.hasNext()) {
				if (!encontreUltimoIntervalo) { // intervalos
					inicio = Integer.parseInt(kbd.next());
					fin = Integer.parseInt(kbd.next());
					capacidad = Integer.parseInt(kbd.next());
					perfilMaquina.add(new Intervalo(capacidad, inicio, fin));
					if (fin == 300000)
						encontreUltimoIntervalo = true;
				} else { // tareas
					indice = Integer.parseInt(kbd.next());
					p[indice - 1] = Integer.parseInt(kbd.next());
					d[indice - 1] = Integer.parseInt(kbd.next());
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void carga(String fichero) {
		Scanner kbd;
		nombre = fichero;
		try {
			kbd = new Scanner(new File(fichero));
			boolean encontreUltimoIntervalo = false;
			int inicio, fin, capacidad, indice = 0;
			kbd.next(); // NOP:
			p = new double[Integer.parseInt(kbd.next())];
			d = new double[p.length];
			kbd.next(); // NINT:
			perfilMaquina = new ArrayList<>(Integer.parseInt(kbd.next()));
			while (kbd.hasNext()) {
				if (!encontreUltimoIntervalo) { // intervalos
					inicio = Integer.parseInt(kbd.next());
					fin = Integer.parseInt(kbd.next());
					capacidad = Integer.parseInt(kbd.next());
					perfilMaquina.add(new Intervalo(capacidad, inicio, fin));
					if (fin == 300000)
						encontreUltimoIntervalo = true;
				} else { // tareas
					indice = Integer.parseInt(kbd.next());
					p[indice - 1] = Integer.parseInt(kbd.next());
					d[indice - 1] = Integer.parseInt(kbd.next());
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String getNombre() {
		return nombre.replace(".txt", "");
	}

	@Override
	public String toString() {
		return nombre;
	}

	@Override
	public boolean equals(Object obj) {
		return toString().equals(obj);
	}

}