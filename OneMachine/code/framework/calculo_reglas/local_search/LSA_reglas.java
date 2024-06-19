package framework.calculo_reglas.local_search;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;

public class LSA_reglas extends AlgorithmAbstract<Regla> {

//	public static int lsaCualquiera = 0;

	// VARIABLES INTERNAS
	GestorNS gestorEstructurasVecindad;

	// PARAMETROS
	public int LSA = 1; // 2:escaladaSimple, 1:maximoGradiente
	public int ME_LSA = 0; // si es mayor que 0 entonces se evaluan esas solo en el modelo completo, si no
							// se evaluan todos
	public int iteracionesMaximas = Integer.MAX_VALUE; // sin limite
	public int vecinosMaximos = Integer.MAX_VALUE; // no los limitamos si aplicamos modelos surrogados

	// METRICAS
	protected int vecinosGenerados;
	protected int vecinosEvaluados;
	protected int vecinosValidos;
	protected int iteraciones;
	protected int vecinosEvaluadosCompletamente;

	public LSA_reglas(int LSA, int ME_LSA, int iteracionesMaximas, int vecinosMaximos) {
		this.LSA = LSA;
		this.ME_LSA = ME_LSA;
		if (iteracionesMaximas == 0)
			this.iteracionesMaximas = Integer.MAX_VALUE;
		else
			this.iteracionesMaximas = iteracionesMaximas;
		if (vecinosMaximos == 0)
			this.vecinosMaximos = Integer.MAX_VALUE;
		else
			this.vecinosMaximos = vecinosMaximos;
		gestorEstructurasVecindad = new GestorNS();
	}

	public LSA_reglas() {
		gestorEstructurasVecindad = new GestorNS();
	}

	public LSA_reglas(String mensaje) {
		super(mensaje);
		gestorEstructurasVecindad = new GestorNS();
	}

	public static void main(String[] args) {
		AlgoUtil.monitoriza = true;
		AlgoUtil.minutos = 10;
		AlgoUtil.intervalosMedicion = 10;
		int modeloEntramiento = 10;
		LSA_reglas lsa = new LSA_reglas("mejora_reglas_SSHE");
		String monticulo = "/ 0.75 + null null Math.max * null null null null - intervalo.inicio Math.exp * null null null null null null null null instancia.d[tarea] instancia.p[tarea] null null 0.25 null instancia.p[tarea] 0.25";
		Regla nueva = new Regla().generarRegla(monticulo);
		AlgoUtil.gestorInstancias.asignaModeloEntrenamiento(modeloEntramiento);
		AlgoUtil.evaluadorReglas.actualizaMejor(nueva);
		AlgoUtil.vln(nueva.train() + ";" + nueva.test() + ";" + nueva.getSize() + ";" + nueva.getProfundidad() + ";"
				+ nueva.representa());
		lsa.run(nueva);
		AlgoUtil.end();
	}

	public Regla run(Regla inicial) {
		// estructuras
		Regla candidato = inicial;
		mejor = inicial;
		List<Regla> vecinos = new ArrayList<Regla>();
		List<Regla> validos = new ArrayList<Regla>();

		int iteracionesLocales = 0;
		// inicio LSA
		while (!timeOut() && iteracionesLocales < iteracionesMaximas) { // ojp si se limitan los vecinos o se utiliza
																		// modelo surrogado!!
//			if (lsaCualquiera == 1 && AlgoUtil.evaluadorReglas.yaMejorada(candidato))
//				break;

//			System.out.println("aplico LSA");
			AlgoUtil.evaluadorReglas.addLSA(candidato);
			iteracionesLocales++;
			iteraciones++;

			// 1- se calculan los vecinos
			vecinos = gestorEstructurasVecindad.getVecinos(candidato);

			vecinosGenerados += vecinos.size(); // se cuentan los vecinos generados

			// 2- se filtran los vecinos validos
			validos.clear();
			for (Regla vecino : vecinos)
				if (vecino.esValido())
					validos.add(vecino);

			vecinos = validos;
			vecinosValidos += vecinos.size();

			// 3- aplica la busqueda local

			// a) se evaluan todos los vecinos con el modelo completo
			if (ME_LSA == 0) {

				Collections.shuffle(vecinos); // ordenar de forma aleatoria

				int n = vecinosMaximos; // evaluamos solo una parte
				if (n > vecinos.size())
					n = vecinos.size();

				for (int i = 0; i < n; i++) {

					if (timeOut())
						break;

					vecinosEvaluados++;
					vecinosEvaluadosCompletamente++;

					if (AlgoUtil.evaluadorReglas.mejoraReal(validos.get(i), candidato)) {
						candidato = validos.get(i);
						if (LSA == 2) // si HC nos quedamos con el primer vecino que mejore
							break;
					}

				}
			}

			// b) se evaluan todos los vecinos con el modelo surrogado y solo los mejores
			// con el completo (ME_LSA)
			else {

				// se ordenan los vecinos según como resuelven el modelo surrogado
				ordenaReglasFiltro(validos);

				// solo se evaluan unos cuantos en el conjunto de entrenamiento
				int localLimiteVecinos = ME_LSA;
				if (localLimiteVecinos > validos.size())
					localLimiteVecinos = validos.size();

				vecinosEvaluados += validos.size();

				for (int i = 0; i < localLimiteVecinos; i++) {
					vecinosEvaluadosCompletamente++;
					if (AlgoUtil.evaluadorReglas.mejoraReal(validos.get(i), candidato)) {
						candidato = validos.get(i);
						if (LSA == 2) // si HC nos quedamos con el primer vecino que mejore
							break;
					}
				}
			}

			// 4- se comprueba el criterio de terminación
			if (AlgoUtil.evaluadorReglas.mejoraReal(candidato, mejor))
				mejor = candidato;
			else // si no mejora termina
				break;

		} // fin

		return mejor; // si no hay ningun vecino que mejore, se devolverá la regla inicial
	}

	void ordenaReglasFiltro(List<Regla> reglas) {
		Collections.sort(reglas, new Comparator<Regla>() {
			@Override
			public int compare(Regla p1, Regla p2) {
				return new Double(AlgoUtil.evaluadorReglas.getPartialEvaluate(p1).filter())
						.compareTo(AlgoUtil.evaluadorReglas.getPartialEvaluate(p2).filter()); // ojo con esto si se
																								// guardan las reglas
																								// evaluadas
																								// parcialmente en
																								// cache, puede
																								// desbordar la memoria
			}
		});
	}

	@Override
	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
		return AlgoUtil.evaluadorReglas.mejorGlobal.train() + ";" + AlgoUtil.evaluadorReglas.mejorGlobal.test() + ";"
				+ AlgoUtil.evaluadorReglas.mejorGlobal.getSize() + ";"
				+ AlgoUtil.evaluadorReglas.mejorGlobal.getProfundidad() + ";"
				+ AlgoUtil.evaluadorReglas.mejorGlobal.representa();
	}

}