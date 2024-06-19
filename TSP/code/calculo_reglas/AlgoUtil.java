package calculo_reglas;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.Random;
import java.util.Set;

import calculo_reglas.gp.GP_Reglas;
import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.terminales.NodoConstante;
import calculo_reglas.representacion.nodos.terminales.NodoT1Nn;
import calculo_reglas.representacion.nodos.terminales.NodoT2Nrn;
import calculo_reglas.representacion.nodos.terminales.NodoT3Dcn;
import calculo_reglas.representacion.nodos.terminales.NodoT4Din;
import calculo_reglas.representacion.nodos.terminales.NodoT5Dc;
import calculo_reglas.representacion.nodos.terminales.NodoT6Pd;
import calculo_reglas.representacion.nodos.terminales.NodoT7Dle;
import domain.Graph;

public class AlgoUtil {

	// parametros
	public static int SM = 1;
	public static List<Graph> set;
	public static List<Graph> set_test;
	public static List<Graph> set_train_full; // set disj set_test
	public static List<Graph> set_train; // subset of set_train_full
	public static List<Graph> set_filtro; // subset of set_train_full and set_train disj set_train_full
	public static int profundidadMaximaCompletasReglas = 4;
	public static int profundidadMaximaInicialReglas = 4;
	public static int profundidadMaximaGlobal = 8;
	public static int nConstantes = 9;
	public static boolean metodoKoza = true;
	public static int indiceNullHoja = 95;
	// 
	public static int cardinalidadEnsembles = 10;
	public static int combinationMethod = 1;
	
	public static int fitness = 0;
	
	//
	public static int fullSM = 0;
	public static int nSM = 0; // no surrogado=0; 1:constanteSurrogado; N>1 entre lo que se actualiza

	// interno
	public static Regla mejorGlobal;
	public static double evaluadosFull;
	public static double evaluadosPartial;
	// 
	public static boolean cache = true;
	public static boolean historical = true;
	public static Map<Regla, MapaSolucionesRegla> soluciones = new HashMap<Regla, MapaSolucionesRegla>();
	public static List<Regla> seleccionables = new ArrayList<Regla>();
	public static double coste;
	public static int size;
	public static int depth;
	public static int minutos;
	public static int intervalosMedicion;
	public static boolean monitoriza = true;
	public static double costeTotalSet = 0;

	protected static BufferedWriter bw;
	protected static long inicio;
	protected static List<Long> tiempos;
	protected static int indiceTiempos;
	protected static Algorithm algorithm;
	
	public static int seed;
	public static Random random;

	public static void run(String msg, Algorithm gp) {
		run(msg);
		algorithm = gp;
	}
	
	public static void run(String msg) {
		seed = new Random().nextInt(1000000000);
		random = new Random(seed);
		cargaInstancias();
		inicializaConstantes();
//		soluciones = new HashMap<Regla, MapaSolucionesRegla>();
		seleccionables = new ArrayList<Regla>();
		uniquesEnsembles = new HashSet<Regla>();
		uniquesRules = new HashSet<Regla>();
		coste = Integer.MAX_VALUE;
		size = Integer.MAX_VALUE;
		depth = Integer.MAX_VALUE;
		if (intervalosMedicion <= 0 || minutos <= 0) {
			monitoriza = false;
			intervalosMedicion = 1;
			minutos = 60 * 24; // el tiempo limite es un dia?
		}
		AlgoUtil.iniciaMedicionTiempo();
		AlgoUtil.iniciaLogger(msg);
	}
	
	static List<Nodo> constantes = new ArrayList<>();
	
	public static void inicializaConstantes() {
		constantes.clear();
		if (nConstantes == 3) {
			constantes.add(new NodoConstante(0.25));
			constantes.add(new NodoConstante(0.5));
			constantes.add(new NodoConstante(0.75));
		} 
		if (nConstantes == 9) {
			constantes.add(new NodoConstante(0.1));
			constantes.add(new NodoConstante(0.2));
			constantes.add(new NodoConstante(0.3));
			constantes.add(new NodoConstante(0.4));
			constantes.add(new NodoConstante(0.5));
			constantes.add(new NodoConstante(0.6));
			constantes.add(new NodoConstante(0.7));
			constantes.add(new NodoConstante(0.8));
			constantes.add(new NodoConstante(0.9));
		}
		if (nConstantes == 11) {
			constantes.add(new NodoConstante(0.0));
			constantes.add(new NodoConstante(0.1));
			constantes.add(new NodoConstante(0.2));
			constantes.add(new NodoConstante(0.3));
			constantes.add(new NodoConstante(0.4));
			constantes.add(new NodoConstante(0.5));
			constantes.add(new NodoConstante(0.6));
			constantes.add(new NodoConstante(0.7));
			constantes.add(new NodoConstante(0.8));
			constantes.add(new NodoConstante(0.9));
			constantes.add(new NodoConstante(1.0));
		}
	}
	
	public static Nodo getConstanteRandom() {		
		return constantes.get((int) (constantes.size() * Math.random()));
	}
	
	public static Nodo getConstante(double d) {
		return new NodoConstante(Math.round(d * 100.0) / 100.0);
	}
	
	public static Set<Regla> reglas = new HashSet<Regla>(); 
	public static Set<Regla> uniquesEnsembles = new HashSet<Regla>();  
	public static Set<Regla> uniquesRules = new HashSet<Regla>(); 

	public static void addRule(Regla cromosoma) {
		if (!cromosoma.esEnsemble()) {
			seleccionables.add(cromosoma);	
			uniquesRules.add(cromosoma);
		} else {
			uniquesEnsembles.add(cromosoma);
		}
	}

	// METODOS PARA EVALUAR REGLAS
	public static boolean mejoraTraining(Regla a, Regla b) {
		mejoraMejor(a);
		mejoraMejor(b);
		if (a.train() < b.train()) {
			return true;
		} else if (a.train() == b.train()) {
			if (a.getSize() < b.getSize()) {
				return true;
			} else if (a.train() == b.train() && a.getProfundidad() < b.getProfundidad()) {
				return true;
			}
		}
		return false;
	}
	
	public static boolean mejoraTraining_adaptative(Regla a, Regla b) {
		mejoraMejor(a);
		mejoraMejor(b);
		if (a.train() < b.train()) {
			return true;
		} else if (a.train() == b.train()) {
			if (a.getSize() < b.getSize()) {
				return true;
			} else if (a.train() == b.train() && a.getProfundidad() < b.getProfundidad()) {
				return true;
			}
		}
		return false;
	}

	public static Regla getReglaRandom() {
		return seleccionables.get(random.nextInt(seleccionables.size()));
	}

	public static boolean mejoraSurrogada(Regla a, Regla b) {
		if (a.filter() < b.filter())
			return true;
		else if (a.filter() == b.filter())
			if (a.getSize() < b.getSize())
				return true;
			else if (a.filter() == b.filter() && a.getProfundidad() < b.getProfundidad())
				return true;

		return false;
	}
	
	public static void mejoraMejor(Regla regla) {
		if (mejorGlobal == null || mejoraTraining(regla))
			actualizaMejor(regla);
	}
	
	public static boolean mejoraTraining(Regla a) {
		if (a.train() < mejorGlobal.train()) {
			return true;
		} else if (a.train() == mejorGlobal.train()) {
			if (a.getSize() < mejorGlobal.getSize()) {
				return true;
			} else if (a.train() == mejorGlobal.train() && a.getProfundidad() < mejorGlobal.getProfundidad()) {
				return true;
			}
		}
		return false;
	}

	private static void actualizaMejor(Regla regla) {
		coste = regla.train();
		size = regla.getSize();
		depth = regla.getProfundidad();
		mejorGlobal = regla;
	}
	
	// METODOS PARA GUARDAR DATOS
	public static void iniciaLogger(String mensaje) {
		try {
			bw = new BufferedWriter(new FileWriter(new File(mensaje + "_" + calculaHoraFecha() + "_" + seed + ".csv")));
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
	/**
	 * Este metodo se llama de forma recurrente para comprobar si se cumple el
	 * timeOut. Además, si toca, imprime una linea con información de la ejecucion.
	 * 
	 * @return
	 */
	public static boolean timeOut() {
		long value = System.currentTimeMillis() - inicio;
		if (monitoriza && indiceTiempos < tiempos.size() && value > tiempos.get(indiceTiempos)) {
			AlgoUtil.vln(algorithm.getLinea());
			indiceTiempos++;
		}
		return value > getTiempoLimite();
	}
	
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

	public static int getTiempoLimite() {
		return 1000 * 60 * minutos;
	}
	
	public static boolean mitadTiempo() {
		return (getTiempoLimite() / 2000.0) < tiempoEjecucion(); // entre 2 * 1000 ms
	}

	private static String calculaHoraFecha() {
		Date date = new Date();
		DateFormat hourdateFormat = new SimpleDateFormat("HH.mm.ss_dd.MM.yyyy");
		return hourdateFormat.format(date);
	}

	public static String set_testLocation;
	public static String trainingSetName = "InstancesTraining_7m";
	public static String filterSetName = "InstancesTraining_7m";
	
	public static int nInstanciasTraining = 10;
	public static int maxGlobalCities = 1000;
	public static int vecesCambiaTraining;

	// CONJUNTO DE ENTRENAMIENTO Y TEST
	public static void cargaInstancias() {
//		System.out.println("Loading test set...");
		set_testLocation = "InstancesTest/";
		set_test = cargaInstancias(cargaNombresInstancias(set_testLocation), set_testLocation);
//		System.out.println("Loading training set...");
//		System.out.println("Loading filter set...");
		set_filtro = cargaInstancias(cargaNombresInstancias(filterSetName + "/"), filterSetName + "/");

//		String instances_less_2000cities = "all4training/";
//		set_train_full = cargaInstancias(cargaNombresInstancias(instances_less_2000cities), instances_less_2000cities);
		set_train = cargaInstancias(cargaNombresInstancias(trainingSetName + "/"), trainingSetName + "/");
		
//		int totalCities = 0;
//		double totalWeight = 0;
//		for (Graph instance : set_train_full) {
//			totalCities+= instance.getSize();
//			totalWeight += instance.getTotalCost();
//		}
//		System.out.println(totalCities+" "+totalWeight);
//		
	}
	
	public static void cambiaConjuntoEntrenamientoRandom() {
		set_train = new ArrayList<Graph>();
		int r;
		int totalCiudades = 0;
		int contador = 0;
		while (set_train.size() < nInstanciasTraining && totalCiudades < maxGlobalCities && contador < set_train_full.size()) {
			r = random.nextInt(set_train_full.size());
			if (!set_train.contains(set_train_full.get(r)) && (totalCiudades + set_train_full.get(r).getSize()) <= maxGlobalCities) {
				set_train.add(set_train_full.get(r));
				totalCiudades += set_train_full.get(r).getSize();
				contador = 0;
			}
			contador++;
		}
	}
	
	public static void cambiaConjuntoEntrenamiento() {
		set_train = cargaInstancias(cargaNombresInstancias(trainingSetName + "/"), trainingSetName + "/");
//		System.out.println("training set size " + set_train.size());
//		int totalCities = 0;
//		double totalWeight = 0;
//		for (Graph instance : set_train) {
//			totalCities+= instance.getSize();
//			totalWeight += instance.getTotalCost();
//		}
//		System.out.println(totalCities+" "+totalWeight);
		
//		Graph instance = set_train.get(0);
//		for (int i = 1; i < set_train.size(); i++)
//			if (instance.getSize() < set_train.get(i).getSize())
//				instance = set_train.get(i);
//		set_train.clear();
//		set_train.add(instance);
//		System.out.println(instance.getName()+" "+set_train.size());
		AlgoUtil.mejorGlobal = null; // OJO
	}
	

	public static void cambiaConjuntoEntrenamiento(Graph instance) {
		set_train = new ArrayList<>();
		set_train.add(instance);
	}
	
	public static void cambiaConjuntoEntrenamiento(List<Graph> instances) {
		set_train = instances;
	}
	
	public static void cambiaConjuntoEntrenamiento(String ruta) {
		set_train = cargaInstancias(cargaNombresInstancias(ruta), ruta);
	}
		
	public static void cambiaConjuntoTest(String ruta) {
		set_test = cargaInstancias(cargaNombresInstancias(ruta), ruta);
	}
	
	public static void cambiaConjuntoTest(String ruta, String instancia) {
		set_test = new ArrayList<Graph>();
		Graph instance = new Graph(ruta + instancia);
		set_test.add(instance);
	}
	

	public static void cambiaConjuntoEntrenamiento(int indice) {
		set_train = new ArrayList<Graph>();
		set_train.add(set_train_full.get(indice));
		AlgoUtil.vecesCambiaTraining++;
	}

	public static List<Graph> cargaInstancias(List<String> nombres, String ruta) {
		List<Graph> set = new ArrayList<Graph>();
		costeTotalSet = 0;
		for (String nombre : nombres) {
			Graph instance = new Graph(ruta + nombre);
			if (instance.getSize() != 0) {
				set.add(instance);
			} else { // OJO DESCARTAR LA INSTANCIA SI NO TIENE COORDENADAS e.g. bays29 da errror!
				System.out.println("Format no valid " + instance.getName());
			}
			costeTotalSet += instance.getTotalCost();
		}
		return set;
	}

	// lista los fichero que hay en el directorio para poderlos cargar
	public static List<String> cargaNombresInstancias(String directorio) {
		List<String> nombres = new ArrayList<String>();
		File file = new File(directorio);
		if (file.isDirectory()) {
			File[] ficheros = file.listFiles();
			for (File fichero : ficheros)
				nombres.add(fichero.getName());
		}
		return nombres;
	}

	// LISTAR LOS TERMINALES DISPONIBLES
	public static Nodo getTerminalRandom(int random) {
		return identificadorTerminal(getTerminales().get(random).toString());
	}

	public static int getNterminales() {
		return getTerminales().size();
	}

	static List<String> terminales;

	static List<String> getTerminales() { // aqui se pueden comentar los terminales que no se quieran usar
		if (terminales == null) {
			terminales = new ArrayList<String>();
//			terminales.add("Nn");
//			terminales.add("Nrn");
			terminales.add("Dcn");
			terminales.add("Din");
			terminales.add("Dc");
			// O(n)
//			terminales.add("Pd");
//			terminales.add("Dle"); 
//			terminales.add("Dcn_avg");
//			terminales.add("Dcn_min");
//			terminales.add("Dcn_max");
//			terminales.add("Din_avg");
//			terminales.add("Din_min");
//			terminales.add("Din_max");
		}
		return terminales;
	}

	static Nodo identificadorTerminal(String terminal) {
		Nodo nodo = null;
		switch (terminal) {
		case "Nn":
			nodo = NodoT1Nn.getInstance();
			break;
		case "Nrn":
			nodo = NodoT2Nrn.getInstance();
			break;
		case "Dcn":
			nodo = NodoT3Dcn.getInstance();
			break;
		case "Din":
			nodo = NodoT4Din.getInstance();
			break;
		case "Dc":
			nodo = NodoT5Dc.getInstance();
			break;
		case "Pd":
			nodo = NodoT6Pd.getInstance();
			break;
		case "Dle":
			nodo = NodoT7Dle.getInstance(); 
			break;
//		case "Dcn_avg":
//			nodo = NodoT8Dcn_avg.getInstance();
//			break;
//		case "Dcn_min":
//			nodo = NodoT9Dcn_min.getInstance();
//			break;
//		case "Dcn_max":
//			nodo = NodoT10Dcn_max.getInstance();
//			break;
//		case "Din_avg":
//			nodo = NodoT11Din_avg.getInstance();
//			break;
//		case "Din_min":
//			nodo = NodoT12Din_min.getInstance();
//			break;
//		case "Din_max":
//			nodo = NodoT13Din_max.getInstance();
//			break;
		default:
			break;
		}
		return nodo;
	}
	
	public static double getPromedioPaperReferencia(Graph instance) {
		String nombre = instance.getName().replace(AlgoUtil.set_testLocation, "");
		switch (nombre) {
		// ESTAS SON LAS QUE DICEN QUE SON OTRAS
		case "xpr2308.tsp":
			return 8672.572;
		case "bcl380.tsp":
			return 1918.307;
		case "dkd1973.tsp":
			return 7792.836;
		case "pbd984.tsp":
			return 3281.114;
		case "dkg813.tsp":
			return 3931.528;
		case "rbx711.tsp":
			return 3632.072;
		case "rbu737.tsp":
			return 3934.535;
		case "dea2382.tsp":
			return 9927.748;
		case "bck2217.tsp":
			return 8038.587;
		case "xit1083.tsp":
			return 4262.549;
		case "icw1483.tsp":
			return 5334.868;
		case "dcb2086.tsp":
			return 7905.548;
		case "dka1376.tsp":
			return 5606.346;
		case "bva2144.tsp":
			return 7501.32;
		case "djc1785.tsp":
			return 7288.886;
		case "pka379.tsp":
			return 1603.686;
		case "fra1488.tsp":
			return 5124.243;
		case "xqc2175.tsp":
			return 8179.526;
		case "fnb1615.tsp":
			return 5974.971;
		case "xqf131.tsp":
			return 647.8742;
		// ESTAS SON LAS DE LA PRIMERA TABLA Y CON LAS QUE EMPIEZO COMPARANDOME
		case "ts225.tsp":
			return 136412.4;
		case "rat99.tsp":
			return 1381.677;
		case "rl1889.tsp":
			return 383303.7;
		case "u1817.tsp":
			return 69334.72;
		case "d1655.tsp":
			return 73740.45;
		case "bier127.tsp":
			return 136781.2;
		case "lin318.tsp":
			return 48039.78;
		case "eil51.tsp":
			return 469.4567;
		case "d493.tsp":
			return 40453.72;
		case "kroB100.tsp":
			return 25254.54;
		case "kroC100.tsp":
			return 24114.56;
		case "ch130.tsp":
			return 7012.582;
		case "pr299.tsp":
			return 56980.64;
		case "fl417.tsp":
			return 14555.84;
		case "d657.tsp":
			return 56882.87;
		case "kroA150.tsp":
			return 30660.12;
		case "fl1577.tsp":
			return 26163.75;
		case "u724.tsp":
			return 48423.29;
		case "pr264.tsp":
			return 60908.02;
		case "pr226.tsp":
			return 92837.77;
		case "pr439.tsp":
			return 130114.3;
		default:
//			System.out.println("No encuentro: " + nombre + " no encontrada");
			return 0;
		}
	}

	// OTRO METODOS
	public static void end() {
		close();
	}

	public static List<Regla> getSeleccionables() {
		return seleccionables;
	}

}
