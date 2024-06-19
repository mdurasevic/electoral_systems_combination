package framework.core;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import dominio.Instancia;

public class GestorInstancias {

	// CONJUNTOS DE EVALUACION
	public List<Instancia> training, filtro;

	// TODAS LAS INSTANCIAS
	public List<Instancia> set_test, set_entrenamiento, set_filtro;
	public List<List<Instancia>> conjuntosEntrenamiento;

	// PARAMETROS
	String ficheroEstructuraConjuntos = "conf/conjuntosNuevasInstancias.csv";
	String directorioFull = "All_nuevas" + "/";
	String directorioPartial = "filtrado" + "/";
	int nConjuntosEntrenamiento = 20; // 20 conjuntos de 50 instancias
	int cardinalidadFiltro = 5;
	public static int subconjuntoEntrenamiento = 10;
	public static int SM = 1;

	void inicializa() {
		cargaInstancias();
		asignaModeloEntrenamiento(subconjuntoEntrenamiento);
		asignaModeloSurrogado(SM);
	}

	public void cargaInstancias() {
		// se cargan las instancias
		List<String> nombresInstancias = getNombresInstancias();
		List<Instancia> instancias = new ArrayList<Instancia>();
		for (String nombre : nombresInstancias)
			instancias.add(new Instancia(directorioFull, nombre));
		// se reparten entre entrenamiento y est
		set_test = new ArrayList<Instancia>();
		set_entrenamiento = new ArrayList<Instancia>();
		for (int i = 0; i < instancias.size(); i++) {
			if (i % 2 == 0)
				set_test.add(instancias.get(i));
			else
				set_entrenamiento.add(instancias.get(i));
		}
	}
	
	public void cargaInstanciasNuevas() {
		// se cargan las instancias
		List<String> nombresInstancias = getNombresInstanciasDirectorio();
		List<Instancia> instancias = new ArrayList<Instancia>();
		for (String nombre : nombresInstancias)
			instancias.add(new Instancia("instances_moreSizes/", nombre));
		set_test = new ArrayList<Instancia>();
		for (int i = 0; i < instancias.size(); i++) 
			set_test.add(instancias.get(i));
	}

	private List<String> getNombresInstanciasDirectorio() {
		String directorio = "instances_moreSizes";

        // Crear un objeto File para representar el directorio
        File carpeta = new File(directorio);

        // Verificar si la ruta es un directorio
        if (carpeta.isDirectory()) {
            // Obtener la lista de archivos en el directorio
            File[] archivos = carpeta.listFiles();

            // Lista para almacenar los nombres de los archivos
            List<String> nombresArchivos = new ArrayList<>();

            // Iterar sobre los archivos y añadir sus nombres a la lista
            for (File archivo : archivos) {
                if (archivo.isFile()) {
                    nombresArchivos.add(archivo.getName());
                }
            }
            return nombresArchivos;
        }
		return null;
	}

	public void asignaModeloEntrenamiento(int subconjuntoEntrenamiento) {
		// a partir de las instancias de entrenamiento se generan subconjuntos de
		// entrenamiento
		conjuntosEntrenamiento = new ArrayList<List<Instancia>>();
		for (int i = 0; i < nConjuntosEntrenamiento; i++)
			conjuntosEntrenamiento.add(new ArrayList<Instancia>());
		int conjunto;
		for (int i = 0; i < set_entrenamiento.size(); i++) {
			conjunto = i % nConjuntosEntrenamiento;
			conjuntosEntrenamiento.get(conjunto).add(set_entrenamiento.get(i));
		}
		// se selecciona uno de los subconjuntos como conjunto de entrenamiento
		training = conjuntosEntrenamiento.get(subconjuntoEntrenamiento);
	}

	public void asignaModeloSurrogado(int SM2) {
		SM = SM2;
		set_filtro = leerInstancias(directorioPartial); // OJO con esto
		if (SM == 1)
			cambiaConjuntoFiltroRandom("i10_3_738 i10_3_449 i10_3_244 i10_3_135 i10_3_812"); // Random
		else if (SM == 2)
			cambiaConjuntoFiltroRandom("i10_3_773 i10_3_10 i10_3_532 i10_3_126 i10_3_692"); // GA
	}
	
	
	String[] filtrosRandom = new String[]
	{ 
			"i10_3_857 i10_3_333 i10_3_864 i10_3_215 i10_3_882",
			"i10_3_356 i10_3_495 i10_3_715 i10_3_407 i10_3_386",
			"i10_3_414 i10_3_376 i10_3_944 i10_3_385 i10_3_458",
			"i10_3_858 i10_3_257 i10_3_707 i10_3_985 i10_3_177",
			"i10_3_931 i10_3_545 i10_3_364 i10_3_412 i10_3_555",
			"i10_3_738 i10_3_449 i10_3_244 i10_3_135 i10_3_812", //5
			"i10_3_556 i10_3_7 i10_3_925 i10_3_743 i10_3_721",
			"i10_3_298 i10_3_901 i10_3_618 i10_3_48 i10_3_585", 
			"i10_3_302 i10_3_556 i10_3_964 i10_3_546 i10_3_803",
			"i10_3_419 i10_3_402 i10_3_707 i10_3_500 i10_3_677" 
	};

	String[] filtrosGA = new String[] 
	{ 
			"i10_3_61 i10_3_875 i10_3_532 i10_3_97 i10_3_773",
			"i10_3_494 i10_3_692 i10_3_933 i10_3_912 i10_3_10", 
			"i10_3_692 i10_3_126 i10_3_773 i10_3_532 i10_3_10",
			"i10_3_692 i10_3_61 i10_3_881 i10_3_738 i10_3_231", 
			"i10_3_867 i10_3_231 i10_3_90 i10_3_61 i10_3_288",
			"i10_3_773 i10_3_10 i10_3_532 i10_3_126 i10_3_692", //5
			"i10_3_61 i10_3_867 i10_3_231 i10_3_90 i10_3_703",
			"i10_3_61 i10_3_576 i10_3_692 i10_3_231 i10_3_90", 
			"i10_3_90 i10_3_867 i10_3_703 i10_3_231 i10_3_61",
			"i10_3_90 i10_3_692 i10_3_231 i10_3_576 i10_3_61"
	};

	public void cambiaConjuntoFiltroRandom(String instancia) {
		filtro = new ArrayList<Instancia>();
		String[] lineas = instancia.split(" ");
		for (String s : lineas)
			for (int i = 0; i < set_filtro.size(); i++)
				if (s.equals(set_filtro.get(i).nombre))
					filtro.add(set_filtro.get(i));
	}

	public void fijaConjuntoFiltroRandom() {
		filtro = new ArrayList<Instancia>();
		while (filtro.size() < cardinalidadFiltro) {
			filtro.add(set_filtro.get((int) (set_filtro.size() * Math.random())));
			System.out.println(filtro.get(filtro.size() - 1).nombre);
		}
	}

	public void cambioConjuntoEntrenamiento(int set) {
		training = conjuntosEntrenamiento.get(set);
	}

	public void cambiaConjuntoFiltro(List<Instancia> instancias) {
		filtro = instancias;
	}
	

	// carga las instancias deacuerdo a un fichero
	public List<String> getNombresInstancias() {
		String ficheroNombres = cargaFicheroNombres(ficheroEstructuraConjuntos);
		List<String> nombres = new ArrayList<String>();
		String[] lineas = ficheroNombres.split("\n");
		String[] linea;
		for (int i = 1; i < lineas.length; i++) { // primera linea cabecera se salta
			linea = lineas[i].split(";");
			nombres.add(linea[0]);
		}
		return nombres;
	}

	// interpreta los nombres que estan contenidos en un fichero
	String cargaFicheroNombres(String fichero) {
		Scanner kbd;
		String text = "";
		try {
			kbd = new Scanner(new File(fichero));
			while (kbd.hasNext()) {
				text += kbd.nextLine() + "\n";
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return text;
	}

	// carga directamente desde directorio
	List<Instancia> leerInstancias(String directorio) {
		List<String> nombresInstancias = cargaNombresInstancias(directorio);
		List<Instancia> instancias = new ArrayList<Instancia>();
		for (String nombreInstancia : nombresInstancias)
			instancias.add(new Instancia(directorio, nombreInstancia));
		return instancias;
	}

	// lista los fichero que hay en el directorio para poderlos cargar
	List<String> cargaNombresInstancias(String directorio) {
		List<String> nombres = new ArrayList<String>();
		File file = new File(directorio);
		if (file.isDirectory()) {
			File[] ficheros = file.listFiles();
			for (File fichero : ficheros)
				nombres.add(fichero.getName());
		}
		return nombres;
	}
}
