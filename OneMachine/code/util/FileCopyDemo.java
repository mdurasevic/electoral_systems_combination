package util;

import java.io.*;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;

import dominio.Instancia;
import framework.core.GestorInstancias;

import java.nio.file.*;

public class FileCopyDemo {
	public static void main(String[] args) throws IOException {

		GestorInstancias nuevo = new GestorInstancias();

		String directorioFull = "All_nuevas" + "/";

		List<String> nombresInstancias = nuevo.getNombresInstancias();

		// copiando las de test
		copia(nombresInstancias, nuevo.set_test, directorioFull, "test/");
		// copiando las de training
		copia(nombresInstancias, nuevo.training, directorioFull, "training/");
	}

	public static void copia(List<String> nombres, List<Instancia> conjunto, String directorioViejo,
			String directorioNuevo) throws IOException {
		for (String nombre : nombres) {
			for (Instancia instancia : conjunto) {
				if (nombre.contains(instancia.nombre + ".txt")) {
					Path temp = Files.move(Paths.get(directorioViejo + nombre), Paths.get(directorioNuevo + nombre));
					break;
				}
			}
		}
	}
}