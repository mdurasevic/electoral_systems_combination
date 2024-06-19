package _experimentosElectoralMethods;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class GeneraResumenAVGySD {
	public static void main(String[] args) {
		List<String[]> records = new ArrayList<>();
		String line;
		String csvSplitBy = ";";

		try (BufferedReader br = new BufferedReader(new FileReader("TSP_score.csv"))) {
			// Leer la primera línea de cabecera
			br.readLine();

			while ((line = br.readLine()) != null) {
				// Usar coma como separador
				String[] data = line.split(csvSplitBy);
				records.add(data);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

		String file = "Size;Method;Avg.;SD;RSD;\n";
		// Mostrar los registros cargados
		List<Double> values = new ArrayList<Double>();
		for (String[] record : records) {
//			values.add(Double.parseDouble(record[3]));
			values.add(Double.parseDouble(record[5]));
			if (values.size() == 30) {
//				file += record[0] + csvSplitBy + record[1] + csvSplitBy + promedio(values) + csvSplitBy + desviacion(values) + csvSplitBy + RSD(values) + "\n";
				file += record[0] + csvSplitBy + record[2] + csvSplitBy + record[3] + csvSplitBy + promedio(values) + csvSplitBy + desviacion(values) + csvSplitBy + RSD(values) + "\n";
				values.clear();
			}
		}
		guardarFichero(file, "resumen_AVGySD");
	}

	static void guardarFichero(String fichero, String nombre) {
		BufferedWriter bw;
		try {
			bw = new BufferedWriter(new FileWriter(new File(nombre + ".csv")));
			bw.write(fichero);
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	static double RSD(List<Double> v) {
		return desviacion(v) / promedio(v) * 100;
	}

	static double desviacion(List<Double> v) {
		double prom, sum = 0;
		int i, n = v.size();
		prom = promedio(v);
		for (i = 0; i < n; i++)
			sum += Math.pow(v.get(i) - prom, 2);
		return Math.sqrt(sum / (double) n);
	}

	static double promedio(List<Double> v) {
		double prom = 0.0;
		for (int i = 0; i < v.size(); i++)
			prom += v.get(i);
		return prom / (double) v.size();
	}
}