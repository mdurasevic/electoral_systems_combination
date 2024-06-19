package dominio;
public class Intervalo implements Comparable<Intervalo> {

	public int inicio, fin, cap;

	public Intervalo(int cap, int inicio) {
		this.cap = cap;
		this.inicio = inicio;
		this.fin = inicio + 1;
	}

	public Intervalo(int cap, int inicio, int fin) {
		this(cap, inicio);
		this.fin = fin;
	}

	public void ocupo() {
		cap--;
	}

	public boolean libre() {
		return cap > 0 && inicio != fin;
	}

	public int disponible() {
		return cap;
	}

	public void alarga() {
		fin++;
	}

	public int lon() {
		return fin - inicio;
	}

	@Override
	public int compareTo(Intervalo nodo2) {
		if (inicio > nodo2.inicio)
			return 1;
		else if (inicio == nodo2.inicio)
			return 0;
		else
			return -1;
	}

	@Override
	public boolean equals(Object obj) {
		return ((Intervalo) obj).inicio == this.inicio;
	}

	@Override
	public String toString() {
		return "[" + inicio + "," + fin + ") con capacidad disponible: " + cap + " y longitud: " + lon()
				+ " esta libre? " + libre();
	}

	public String representa() {
		return "[" + inicio + "," + fin + "]";
	}
}