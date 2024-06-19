package dominio;

import java.util.ArrayList;
import java.util.List;


public class OperacionesAsignacionTrabajosMaquina {

	private static List<Intervalo> copiaIntervalos;

	public static List<Intervalo> getIntervalos() {
		return copiaIntervalos;
	}

	public static List<Integer> getSinAsignar(List<Integer> sin_asignar, int[] sti) {
		List<Integer> copia = new ArrayList<Integer>(sin_asignar.size() - 1);
		for (int i = 0; i < sti.length; i++)
			if (sti[i] == -1)
				copia.add(i);
		return copia;
	}

	public static List<Integer> getSinAsignar(Integer tarea, List<Integer> sin_asignar) {
		sin_asignar.remove(new Integer(tarea));
		return sin_asignar;
	}

	public static int[] getSti(Integer tarea, int valorSti, int[] sti) {
		sti[tarea] = valorSti; // st
		return sti;
	}

	public static void sacaIntervalos() {
		for (Intervalo i : copiaIntervalos)
			System.out.println(i);
	}

	public static int estimacionTardiness(Instancia instancia, int sti, int tarea) {
		return (int) Math.max(0, instancia.p[tarea] - (instancia.d[tarea] - sti));
	}

	public static List<Intervalo> copiaIntervalos(List<Intervalo> intervalos) {
		copiaIntervalos = new ArrayList<Intervalo>(intervalos.size());
		for (Intervalo i : intervalos)
			if (i.libre())
				copiaIntervalos.add(OperacionesAsignacionTrabajosMaquina.creaIntervalo(i));
		return copiaIntervalos;
	}

	public static boolean propaga(Intervalo intervalo, int duracion, List<Intervalo> copia) {
		copiaIntervalos = copia;
		if (intervalo != null) {
			if (intervalo.lon() == duracion)
				return asigno(intervalo);
			else if (intervalo.lon() > duracion)
				return dividoIntervalo(intervalo, duracion);
			else
				return intentoOcuparVariosIntervalos(intervalo, duracion);
		} else
			return false;
	}

	public static boolean dividoIntervalo(Intervalo intervalo, int duracion) {
		Intervalo aux = creaIntervalo(intervalo);
		int posicion = copiaIntervalos.indexOf(intervalo);
		copiaIntervalos.get(posicion).inicio = duracion + aux.inicio;
		aux.ocupo();
		aux.fin = duracion + aux.inicio;
		copiaIntervalos.add(posicion, aux);
		return true;
	}

	public static boolean intentoOcuparVariosIntervalos(Intervalo intervalo, int duracion) {
		Intervalo aux = creaIntervalo(intervalo);
		int acumulada = aux.lon();

		while (acumulada < duracion) {
			aux = buscaIntervaloSiguiente(aux.fin, copiaIntervalos);
			if (aux == null)
				return false;
			if (!aux.libre())
				return false;
			acumulada += aux.lon();
		}
		if (acumulada == duracion) {
			for (Intervalo i : copiaIntervalos)
				if (i.inicio >= intervalo.inicio && (intervalo.inicio + duracion) >= i.fin)
					i.ocupo();
			return true;
		} else {
			for (Intervalo i : copiaIntervalos)
				if (i.inicio >= intervalo.inicio && (intervalo.inicio + duracion) >= i.fin && !i.equals(aux))
					i.ocupo();
			return dividoIntervalo(aux, (-acumulada + duracion + aux.lon()));
		}
	}

	public static boolean asigno(Intervalo intervalo) {
		for (Intervalo i : copiaIntervalos)
			if (i.inicio == intervalo.inicio) {
				i.ocupo();
				return true;
			}
		return false;
	}

	public static Intervalo buscaUnIntervaloSiguienteLibre(int fin, List<Intervalo> intervalos) {
		for (Intervalo i : intervalos)
			if (i.inicio >= fin && i.libre())
				return creaIntervalo(i);
		return null;
	}

	public static Intervalo buscaIntervaloSiguiente(int fin, List<Intervalo> intervalos) {
		for (Intervalo i : intervalos)
			if (i.inicio == fin)
				return creaIntervalo(i);
		return null;
	}

	public static Intervalo creaIntervalo(Intervalo intervalo) {
		return new Intervalo(intervalo.cap, intervalo.inicio, intervalo.fin);
	}
}