package framework.calculo_reglas;

import dominio.Intervalo;
import dominio.OperacionesAsignacionTrabajosMaquina;
import dominio.Planificador;

public class PlanificadorReglas extends Planificador {

	private Regla monticulo;

	public PlanificadorReglas() {
	};

	public PlanificadorReglas(Regla monticulo) {
		this.monticulo = monticulo;
	}

	@Override
	protected int buscarMejorTarea(Intervalo intervalo) {
		prioridad = new double[instancia.p.length];
		for (Integer tarea : sin_asignar)
			if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
					OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
					OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
				prioridad[tarea] = monticulo.calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
			}
		return buscarTareaMasPrioritaria(intervalo.inicio);
	}

	protected int buscarTareaMasPrioritaria(int inicioIntervalo) {
		if (sin_asignar.size() == 1) // ?
			return sin_asignar.get(0);
		double mayor = Double.NEGATIVE_INFINITY;
		int tarea = -1;
		for (Integer i : sin_asignar) {
			if (prioridad[i] >= mayor) {
				mayor = prioridad[i];
				tarea = i;
			} else if (Double.isNaN(prioridad[i])) {// la regla no decide
				indeterminacion = true;
				if (AUX == null)
					AUX = new double[instancia.p.length];
				AUX[i] = instancia.d[i];
			}
		}
		if (AUX != null) {
			tarea = sin_asignar.get(0);
			for (Integer i : sin_asignar) // se escoge la de menor due date
				if (AUX[i] != 0 && AUX[i] <= AUX[tarea]) // ojo aqui es donde puede haber conflictos con la regla EDD
					tarea = i;
		}
		AUX = null;
		return tarea;
	}

	// con ruleta
//	protected int buscarTareaMasPrioritaria(int inicioIntervalo) {
//		if (sin_asignar.size() == 1)
//			return sin_asignar.get(0);
//		int tarea = sin_asignar.get(0);
//		double sumaTotalPrioridades = 0;
//		for (Integer i : sin_asignar) {
//			if (Double.isNaN(prioridad[i])) {// la regla no decide
//				indeterminacion = true;
//				if (AUX == null)
//					AUX = new double[instancia.p.length];
//				AUX[i] = instancia.d[i];
//			} else {
//				prioridad[i] = Math.pow(prioridad[i], 2);
//				sumaTotalPrioridades += prioridad[i];
//			}
//		}
//		if (AUX != null) {
//			for (Integer i : sin_asignar) // se escoge la de menor due date
//				if (AUX[i] != 0 && AUX[i] <= AUX[tarea]) // ojo aqui es donde puede haber conflictos con la regla EDD
//					tarea = i;
//		}
//		if (!indeterminacion) {
//			double valorAleatorio = new Random().nextDouble() * sumaTotalPrioridades;
//			double sumaParcial = 0;
//			for (Integer i : sin_asignar) {
//				sumaParcial += prioridad[i];
//				if (sumaParcial >= valorAleatorio) {
//					tarea = i;
//					break;
//				}
//			}
//		}
//		AUX = null;
//		return tarea;
//	}
	
	
//	protected int buscarTareaMasPrioritaria(int inicioIntervalo) {
//		if (sin_asignar.size() == 1)
//			return sin_asignar.get(0);
//		int tarea = sin_asignar.get(0);
//		double sumaTotalPrioridades = 0;
//		for (Integer i : sin_asignar) {
//			if (Double.isNaN(prioridad[i])) {// la regla no decide
//				indeterminacion = true;
//				if (AUX == null)
//					AUX = new double[instancia.p.length];
//				AUX[i] = instancia.d[i];
//			} else {
//				prioridad[i] = Math.pow(prioridad[i], 2);
//				sumaTotalPrioridades += prioridad[i];
//			}
//		}
//		if (AUX != null) {
//			for (Integer i : sin_asignar) // se escoge la de menor due date
//				if (AUX[i] != 0 && AUX[i] <= AUX[tarea]) // ojo aqui es donde puede haber conflictos con la regla EDD
//					tarea = i;
//		}
//		if (!indeterminacion) {
//			double valorAleatorio = new Random().nextDouble() * sumaTotalPrioridades;
//			
//			List<Pair<Integer, Double>> indiceFitnessPairs = new ArrayList<>();
//			for (Integer i : sin_asignar) {
//			    indiceFitnessPairs.add(new Pair<>(i, prioridad[i]));
//			}
//			
//			Collections.sort(indiceFitnessPairs, Comparator.comparing(Pair::getValue, 
//					Comparator.naturalOrder()));
//			
//			double sumaParcial = 0;
//			for (Pair<Integer, Double> pair : indiceFitnessPairs) {
//				sumaParcial += pair.getValue();
//				if (sumaParcial >= valorAleatorio) {
//					tarea = pair.getKey();
//					break;
//				}
//			}
//		}
//		AUX = null;
//		return tarea;
//	}

}