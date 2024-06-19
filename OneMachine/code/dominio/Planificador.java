package dominio;

import java.util.ArrayList;
import java.util.List;

public abstract class Planificador {

	public Instancia instancia; // instancia a resolver
	public List<Integer> sin_asignar; // representan las tareas sin asignar
	public int[] sti; // guarda los sti de cada tarea
	public List<Intervalo> intervalos; // perfil de máquina resultante
	public int coste; // coste
	public double[] prioridad; // para el calculo de la prioridad
	public double[] AUX; // auxiliar cuando la regla tiene NaN se resuelve con EDD
	public static boolean indeterminacion = false;
//	public int[] tareas_seleccionadas;

	public double planifica(List<Instancia> instancias) {
		int costeTotal = 0;
		for (Instancia instancia : instancias)
			costeTotal += planifica(instancia);
		return (1.0 * costeTotal / instancias.size());
	}

	public int planifica(Instancia instancia) {
		indeterminacion = false;
		this.instancia = instancia;
		cargaInstancia(instancia);
		int tarea;
		int j = 0;
		while (!sin_asignar.isEmpty()) {
			for (Intervalo intervalo : intervalos) {
				if (intervalo.libre()) {
					tarea = buscarMejorTarea(intervalo);
					if (OperacionesAsignacionTrabajosMaquina.propaga(
							OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
							OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
						sin_asignar = OperacionesAsignacionTrabajosMaquina.getSinAsignar(tarea, sin_asignar);
						sti = OperacionesAsignacionTrabajosMaquina.getSti(tarea, intervalo.inicio, sti);
						coste += OperacionesAsignacionTrabajosMaquina.estimacionTardiness(instancia, intervalo.inicio, tarea);
						intervalos = OperacionesAsignacionTrabajosMaquina.getIntervalos();
//						tareas_seleccionadas[j] = tarea;
						j++;
						break;
					}
				}
			}
		}
//		System.out.println(coste);
		return coste;
	}

	protected abstract int buscarMejorTarea(Intervalo intervalo);

	protected double promedioDuraciones() {
		double promedio = 0.0;
		for (Integer tarea : sin_asignar)
			promedio += instancia.p[tarea];
		return promedio / sin_asignar.size();
	}
	
	public void cargaInstancia(Instancia instancia) {
		coste = 0;
		sti = new int[instancia.p.length];
		sin_asignar = new ArrayList<Integer>(sti.length);
		for (int i = 0; i < sti.length; i++) { // no esta ninguno asignado
			sti[i] = -1;
			sin_asignar.add(i);
		}
		intervalos = new ArrayList<Intervalo>(instancia.perfilMaquina.size());
		for (int i = 0; i < instancia.perfilMaquina.size(); i++)
			intervalos.add(i, instancia.perfilMaquina.get(i));
//		tareas_seleccionadas = new int[instancia.p.length];
	}

	@Override
	public String toString() {
		return this.getClass().getName().replaceAll("generadas.", "");
	}
}