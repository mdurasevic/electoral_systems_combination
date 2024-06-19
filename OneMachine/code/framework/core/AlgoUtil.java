package framework.core;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import _experimentos_gp_bilevelfunction.MapaSolucionesRegla;
import framework.calculo_reglas.Regla;

public class AlgoUtil {

	// ESTRUCTURAS
	public static final GestorParametrosReglasEnsembles gestorReglas = new GestorParametrosReglasEnsembles();
	public static GestorInstancias gestorInstancias;
	public static final Evaluador evaluadorReglas = new Evaluador();
	public static final Evaluador evaluadorEnsemblesColaborativos = new Evaluador();
//	public static final Evaluador evaluadorEnsemblesCoordinados = new Evaluador(); // ya no se utiliza
	
	// VARIABLES INTERNAS
	public static Algorithm algorithm;
	protected static BufferedWriter bw;
	protected static long inicio;
	protected static List<Long> tiempos;
	protected static int indiceTiempos;

	// PARAMETROS
	public static int minutos = 30;
	public static int intervalosMedicion = 3;
	public static boolean monitoriza = true;
	
	// ADICIONAL
	public static boolean cache = true;
	public static Map<Regla, MapaSolucionesRegla> soluciones = new HashMap<Regla, MapaSolucionesRegla>();
	public static boolean historical = true;


	/**
	 * Este metodo se llama de forma recurrente para comprobar si se cumple el
	 * timeOut. Además, si toca, imprime una linea con información de la ejecucion.
	 * 
	 * @return
	 */
	public static boolean timeOut() {
		long value = System.currentTimeMillis() - inicio;
		if (monitoriza && indiceTiempos < tiempos.size() && value > tiempos.get(indiceTiempos)) {
			algorithm.imprimeLinea();
			indiceTiempos++;
		}
		return value > getTiempoLimite();
	}

	/**
	 * Si se cambia el conjunto de entrenamiento entonces tenemos que vaciar los que
	 * teniamos evaluados con otro conjunto
	 * 
	 * @param set
	 */
	public static void cambiaConjuntoEntrenamiento(int set) {
		gestorInstancias.cambioConjuntoEntrenamiento(set);
		evaluadorReglas.evaluatedFull.clear();
		evaluadorReglas.seleccionables.clear();
		evaluadorEnsemblesColaborativos.evaluatedFull.clear();
		evaluadorEnsemblesColaborativos.seleccionables.clear();
	}
	
	public static void cambiaConjuntoFilter(int set) {
		gestorInstancias.asignaModeloSurrogado(set);
	}

	public static void end() {
		close();
	}

	// METODOS PARA GUARDAR DATOS
	public static void iniciaLogger(String mensaje) {
		try {
			bw = new BufferedWriter(new FileWriter(new File(mensaje + "_" + calculaHoraFecha() + "_" + new Random().nextInt() + ".csv")));
			ln(mensaje);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void vln(String linea) {
		ln(linea);
		nvln(linea);
	}

	protected static void nvln(String linea) {
		if (bw != null)
			try {
				bw.write(linea + "\n");
			} catch (IOException e) {
				e.printStackTrace();
			}
	}

	public static void ln(String linea) {
		linea = "[" + tiempoEjecucion() + "]" + linea;
		System.out.println(linea);
	}

	protected static void close() {
		try {
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	// METODOS PARA MEDIR EL TIEMPO
	public static void iniciaMedicionTiempo() {
		inicio = System.currentTimeMillis();
		estableceIntervalosMedicion(intervalosMedicion);
	}

	public static int tiempoEjecucion() {
		return (int) ((System.currentTimeMillis() - inicio) / 1000);
	}

	private static void estableceIntervalosMedicion(int n) {
		tiempos = new ArrayList<Long>();
		indiceTiempos = 0;
		long paso = (getTiempoLimite() / n);
		for (int i = 0; i < n; i++)
			tiempos.add(paso * (i + 1));
	}

	private static int getTiempoLimite() {
		return 1000 * 60 * minutos;
	}

	private static String calculaHoraFecha() {
		Date date = new Date();
		DateFormat hourdateFormat = new SimpleDateFormat("HH.mm.ss_dd.MM.yyyy");
		return hourdateFormat.format(date);
	}

	public static void iniciaEvaluadorReglas() {
		gestorInstancias = new GestorInstancias();
		gestorInstancias.inicializa();
	}

	//

}
